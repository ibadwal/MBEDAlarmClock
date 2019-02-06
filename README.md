# mbed-alarm-clock

MBED WiFi enabled alarm clock created with NXP LPC1768 board with the ESP8266 WiFi chip and the ILI9340 240x320 pixel color display. I modified the NTP-Client to enable the web search for the day/time information, and I modified the ILI9340 driver to work on this board since I didn't have access to all the ports.

Created with a speaker in mind for the alarm clock, where you can change the volume with a variable resistor. Also, I allowed users to change the alarm time and set/reset the alarm with analog buttons.

The mbed-os folder contains a slightly modified version of the mbed-os version 5, to allow the NTP Client to be used alongside the ESP8266 chip, as there were some dependency issues before.

The majority of new code is contained in demo.cpp in the root folder.
