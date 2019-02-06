/***************************************************************
    ILI9340_Driver  v1.1    01.06.14    Ian Weston
    
Driver and integrated graphics library for displays that use the 
ILI9340 controller in SPI mode. Such as the Adafruit 2.2" display.

This code has been ported from several sources. The driver section
was completely ported from the Adafruits Arduino source code, and
the graphics functions were ported from the Adafruits GFX library
and some elements were ported from code by Elmicros seeduio port.

Future revisions will include more advanced graphics functions.

***************************************************************/



#include "mbed.h"

#ifndef ILI9340_DRIVER_h
#define ILI9340_DRIVER_h


#define _TFTWIDTH  240
#define _TFTHEIGHT 320

#define ILI9340_NOP     0x00
#define ILI9340_SWRESET 0x01
#define ILI9340_RDDID   0x04
#define ILI9340_RDDST   0x09

#define ILI9340_SLPIN   0x10
#define ILI9340_SLPOUT  0x11
#define ILI9340_PTLON   0x12
#define ILI9340_NORON   0x13

#define ILI9340_RDMODE  0x0A
#define ILI9340_RDMADCTL  0x0B
#define ILI9340_RDPIXFMT  0x0C
#define ILI9340_RDIMGFMT  0x0A
#define ILI9340_RDSELFDIAG  0x0F

#define ILI9340_INVOFF  0x20
#define ILI9340_INVON   0x21
#define ILI9340_GAMMASET 0x26
#define ILI9340_DISPOFF 0x28
#define ILI9340_DISPON  0x29

#define ILI9340_CASET   0x2A
#define ILI9340_PASET   0x2B
#define ILI9340_RAMWR   0x2C
#define ILI9340_RAMRD   0x2E

#define ILI9340_PTLAR   0x30
#define ILI9340_MADCTL  0x36


#define ILI9340_MADCTL_MY  0x80
#define ILI9340_MADCTL_MX  0x40
#define ILI9340_MADCTL_MV  0x20
#define ILI9340_MADCTL_ML  0x10
#define ILI9340_MADCTL_RGB 0x00
#define ILI9340_MADCTL_BGR 0x08
#define ILI9340_MADCTL_MH  0x04

#define ILI9340_PIXFMT  0x3A

#define ILI9340_FRMCTR1 0xB1
#define ILI9340_FRMCTR2 0xB2
#define ILI9340_FRMCTR3 0xB3
#define ILI9340_INVCTR  0xB4
#define ILI9340_DFUNCTR 0xB6

#define ILI9340_PWCTR1  0xC0
#define ILI9340_PWCTR2  0xC1
#define ILI9340_PWCTR3  0xC2
#define ILI9340_PWCTR4  0xC3
#define ILI9340_PWCTR5  0xC4
#define ILI9340_VMCTR1  0xC5
#define ILI9340_VMCTR2  0xC7

#define ILI9340_RDID1   0xDA
#define ILI9340_RDID2   0xDB
#define ILI9340_RDID3   0xDC
#define ILI9340_RDID4   0xDD

#define ILI9340_GMCTRP1 0xE0
#define ILI9340_GMCTRN1 0xE1
/*
#define ILI9340_PWCTR6  0xFC

*/

// Color definitions
#define ILI9340_BLACK   0x0000
#define ILI9340_BLUE    0x001F
#define ILI9340_RED     0xF800
#define ILI9340_GREEN   0x07E0
#define ILI9340_CYAN    0x07FF
#define ILI9340_MAGENTA 0xF81F
#define ILI9340_YELLOW  0xFFE0  
#define ILI9340_WHITE   0xFFFF



class ILI9340_Display {
    
    public:
    
    uint16_t _height;
    uint16_t _width;
    
    ILI9340_Display(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName rst, PinName dc);
    
    void DispInit();
    void WriteCommand(uint8_t);
    void WriteData(uint8_t);
    void SetRotation(uint8_t);
    void InvertDisplay(bool);
    void SetAddrWindow(uint16_t, uint16_t, uint16_t, uint16_t);
    
    void DrawPixel(uint16_t, uint16_t, uint16_t);
    void FillScreen(uint16_t);
    void DrawFastVLine(int16_t, int16_t, int16_t, uint16_t);
    void DrawFastHLine(int16_t, int16_t, int16_t, uint16_t);
    void FillRect(int16_t, int16_t, int16_t, int16_t, uint16_t);
    void DrawRect(int16_t, int16_t, int16_t, int16_t, uint16_t);
    void DrawCircle(int16_t, int16_t, int16_t, uint16_t);
    void FillCircle(int16_t, int16_t, int16_t, uint16_t);
    void FillCircleHelper(int16_t, int16_t, int16_t, uint8_t, int16_t, uint16_t);
    void DrawCircleHelper( int16_t, int16_t, int16_t, uint8_t, uint16_t);
    void DrawRoundRect(int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t);
    void FillRoundRect(int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t);
    uint16_t Colour565(uint8_t, uint8_t, uint8_t);
    
    void DrawAscii(unsigned char, uint16_t, uint16_t, uint16_t, uint16_t);
    void DrawString(char *string, uint16_t, uint16_t, uint8_t, uint16_t);
    void IntToChars (char*, int, uint8_t, uint8_t, uint16_t, uint16_t, uint8_t, uint16_t);
    
    void Swap(int16_t*, int16_t*);
    void DrawLine(int16_t, int16_t, int16_t, int16_t, uint16_t);
    
    protected:
    SPI spi; // mosi, miso, sclk
    DigitalOut cs;
    DigitalOut rst;
    DigitalOut dc;
    
    uint8_t orientation;
       
    };





#endif