#include <iostream>
#include "mbed.h"
#include "ILI9340_Driver.h"
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include "ntp-client/NTPClient.h"
#include "ESP8266Interface.h"
//http stuff

using std::cout;
using std::endl;

int hours;
int minutes;
int seconds;

PwmOut speaker(p21);
DigitalIn stop_alarm_button(p17);
DigitalIn button1(p18);
DigitalIn button2(p19);
DigitalIn button3(p20);
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);

AnalogIn light_voltage(p16);
AnalogIn variable_volate(p15);
//AnalogOut ext_led1(p17);

ESP8266Interface wifi(p28, p27);

void play_alarm(){
    //Generate a 500Hz tone using PWM hardware output
    int but_alarm = 1 - stop_alarm_button;
    while (but_alarm==0){
        speaker.period(1/500.0); // 500hz period
        speaker=1-light_voltage; //75% duty cycle - max volume
        wait(0.5);
        speaker=0.0; // turn off audio
        wait(0.5);
        seconds = seconds + 1;
        if (seconds >= 60){
            minutes++;
            seconds = 0;
        }
        but_alarm = 1 - stop_alarm_button;
    }
}

int main() {
    // create the display object
    ILI9340_Display tft = ILI9340_Display(p5, p6, p7, p24, p25, p26);
    stop_alarm_button.mode(PullUp);
    play_alarm();

    // initialise the display
    tft.DispInit();
    
    // clears the screen to remove all noise data
    tft.FillScreen(ILI9340_CYAN);
    
    //set up the wifi from ESP8266
    printf("Wifi Starting (using wifi)\r\n");
    int ret = wifi.connect("Indo", "indowifipass", NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
      printf("\nConnection error: %d\n", ret);
      return -1;
    }
    printf("Connected\n\r");
    printf("Client IP Address is %s\r\n", wifi.get_ip_address());
    //connect the NTP client with the wifi from ESP8266
    NTPClient ntp(&wifi);
    
    //temp begin
    int alarm_hours = 22;
    int alarm_minutes = 14;
    int alarm_seconds = 30;
    //temp end
    int year;
    int month;
    int day;
    
    time_t timestamp;
    time_t temp_timestamp = ntp.get_timestamp();
    tm *ltm;
    
    //get the time from the ntp server
    timestamp = ntp.get_timestamp();
    //printf("timestamp before: %d\n\r", timestamp);
    //change the time to eastern time (5 hours -> 300 minutes -> 18000 seconds)
    //timestamp = timestamp - 18000;
    //printf("timestamp after: %d\n\r", timestamp);
        
    //check to see if retrieving the data broke (might be broken value)
    ltm = localtime(&timestamp);
    
    while ((((int)ltm->tm_year) + 1900) > 2020){
        timestamp = ntp.get_timestamp();
        ltm = localtime(&timestamp);    
    }
        
    hours = ((ltm->tm_hour - 5) % 24 + 24) % 24;
    minutes = ltm->tm_min;
    seconds = ltm->tm_sec;
    year = ((int)ltm->tm_year) + 1900;
    printf("%d\n\r", year);
    month = ((int)ltm->tm_mon) + 1;
    printf("%d\n\r", month);
    day = ((int)ltm->tm_mday) + 1;
    printf("%d\n\r", day);
    if (hours >= 7){
        day--;
    }
    bool first_run = true;
    bool setting_alarm = false;
    
    
    //weather stuff test using https://os.mbed.com/cookbook/Twitter and https://os.mbed.com/cookbook/Working-with-the-networking-stack as examples
    //HttpRequest* get_req = new HttpRequest(&wifi, HTTP_GET, "https://weather-broker-cdn.api.bbci.co.uk/en/forecast/rss/3day/4929022"
    //store value in a buffer
    char* weather_buf[100];
    
    while(true) {
        int but_1 = 1 - button1;
        int but_2 = 1 - button2;
        int but_3 = 1 - button3;
        button1.mode(PullUp);
        button2.mode(PullUp);
        button3.mode(PullUp);
        //default values are 1
        myled1 = !button1;
        myled2 = !button2;
        myled3 = !button3;
        
        //alarm set check
        if (first_run == false){
            char alarm_buf[50];
            //reset the screen, alarm is set, reset alarm boolean
            if ((but_3 != 0) && (setting_alarm == true)){
                printf("reset\n\r");
                tft.FillRect(45, 40, 180, 34, ILI9340_CYAN);
                setting_alarm = false;
            }
            //first check for setting alarm
            else if ((but_3 != 0) && (setting_alarm == false)){
                printf("CONDITION\n\r");
                alarm_hours = hours;
                alarm_minutes = minutes;
                alarm_seconds = 0;
                tft.DrawString("Set Alarm:", 50, 40, 2, ILI9340_BLUE);
                setting_alarm = true;
            }
            //change hours
            if ((but_2 != 0) && (setting_alarm == true)){
                printf("hours++\n\r");
                alarm_hours = (alarm_hours+1)%24;
            }
            //change minutes
            if ((but_1 != 0) && (setting_alarm == true)){
                printf("minutes++\n\r");
                alarm_minutes = (alarm_minutes+1)%60;
            }
            //redisplay alarm time
            if (setting_alarm == true){
                tft.FillRect(45, 60, 180, 14, ILI9340_CYAN);
                sprintf(alarm_buf, "%d:%d:%d", alarm_hours, alarm_minutes, alarm_seconds);
                tft.DrawString(alarm_buf, 50, 60, 2, ILI9340_BLACK);
            }
        }
        
        //increment the time
        seconds++;
        //condition for a minute
        if (seconds >= 60){
            minutes++;
            seconds = 0;
        }
        //condition for an hour
        if (minutes >= 60){
            hours++;
            minutes = 0;
        }
        hours = (hours%24);
        
        char time_buf[50];
        sprintf(time_buf, "%d:%d:%d", hours, minutes, seconds);
        
        char date_buf[50];
        sprintf(date_buf, "%d/%d/%d", month, day, year);
        
        // Small amount of text to the display.
        //clear the area
        tft.FillRect(45, 200, 180, 14, ILI9340_CYAN);
        //draw the time
        tft.DrawString("TIME:", 80, 180, 2, ILI9340_BLUE);
        tft.DrawString(time_buf, 50, 200, 2, ILI9340_BLACK);
        //draw the date
        tft.DrawString("DATE:", 80, 225, 2, ILI9340_BLUE);
        tft.DrawString(date_buf, 45, 245, 2, ILI9340_BLACK);
        
        if ((hours == alarm_hours) && (minutes == alarm_minutes) && (seconds == alarm_seconds)){
            play_alarm();
        }
        
        // wait
        if (first_run == true){
            first_run = false;
        }
        wait(0.85);
    }
}