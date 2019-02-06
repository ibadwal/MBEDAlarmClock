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
#include "ILI9340_Driver.h"
#include "SimpleFont.cpp"



////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, assigns the pins to the SPI object, set orientation, and sets screen dims.
////////////////////////////////////////////////////////////////////////////////////////////////
ILI9340_Display::ILI9340_Display(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName rst, PinName dc)
    : spi(mosi, miso, sclk), cs(cs), rst(rst), dc(dc) {
    _height = _TFTHEIGHT;
    _width = _TFTWIDTH;
    orientation = 0;
    }



////////////////////////////////////////////////////////////////////////////////////////////////
// Command writing code
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::WriteCommand(uint8_t command) {
    dc = 0;
    cs = 0;
    spi.write(command);
    cs = 1;
    }
  
  
  
////////////////////////////////////////////////////////////////////////////////////////////////  
// Data writing code
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::WriteData(uint8_t data) {
    cs = 0;
    dc = 1;
    spi.write(data);
    cs = 1;    
    }
    
    
    
////////////////////////////////////////////////////////////////////////////////////////////////
// Initilise the display
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DispInit(void) {
    //CtrlOutput();
    
    rst = 0;
    
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    //spi.format(8,3);
    spi.frequency(24000000); // actually seems to work up to about 20Mhz... way better than the 8mhz as std.
    
    // Toggle rst to reset
    rst = 1;
    wait(0.005);
    rst = 0;
    wait(0.020);
    rst = 1;
    wait(0.150);
    
    WriteCommand(0xEF);
    WriteData(0x03);
    WriteData(0x80);
    WriteData(0x02);

    WriteCommand(0xCF);  
    WriteData(0x00); 
    WriteData(0xC1); 
    WriteData(0x30); 

    WriteCommand(0xED);  
    WriteData(0x64); 
    WriteData(0x03); 
    WriteData(0x12); 
    WriteData(0x81); 
 
    WriteCommand(0xE8);  
    WriteData(0x85); 
    WriteData(0x00); 
    WriteData(0x78); 

    WriteCommand(0xCB);  
    WriteData(0x39); 
    WriteData(0x2C); 
    WriteData(0x00); 
    WriteData(0x34); 
    WriteData(0x02); 
 
    WriteCommand(0xF7);  
    WriteData(0x20); 

    WriteCommand(0xEA);  
    WriteData(0x00); 
    WriteData(0x00); 
 
    WriteCommand(ILI9340_PWCTR1);    //Power control 
    WriteData(0x23);   //VRH[5:0] 
 
    WriteCommand(ILI9340_PWCTR2);    //Power control 
    WriteData(0x10);   //SAP[2:0];BT[3:0] 
 
    WriteCommand(ILI9340_VMCTR1);    //VCM control 
    WriteData(0x3e); //�Աȶȵ���
    WriteData(0x28); 
  
    WriteCommand(ILI9340_VMCTR2);    //VCM control2 
    WriteData(0x86);  //--
 
    WriteCommand(ILI9340_MADCTL);    // Memory Access Control 
    WriteData(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);

    WriteCommand(ILI9340_PIXFMT);    
    WriteData(0x55); 
  
    WriteCommand(ILI9340_FRMCTR1);    
    WriteData(0x00);  
    WriteData(0x18); 
 
    WriteCommand(ILI9340_DFUNCTR);    // Display Function Control 
    WriteData(0x08); 
    WriteData(0x82);
    WriteData(0x27);  
 
    WriteCommand(0xF2);    // 3Gamma Function Disable 
    WriteData(0x00); 
 
    WriteCommand(ILI9340_GAMMASET);    //Gamma curve selected 
    WriteData(0x01); 
 
    WriteCommand(ILI9340_GMCTRP1);    //Set Gamma 
    WriteData(0x0F); 
    WriteData(0x31); 
    WriteData(0x2B); 
    WriteData(0x0C); 
    WriteData(0x0E); 
    WriteData(0x08); 
    WriteData(0x4E); 
    WriteData(0xF1); 
    WriteData(0x37); 
    WriteData(0x07); 
    WriteData(0x10); 
    WriteData(0x03); 
    WriteData(0x0E); 
    WriteData(0x09); 
    WriteData(0x00); 
  
    WriteCommand(ILI9340_GMCTRN1);    //Set Gamma 
    WriteData(0x00); 
    WriteData(0x0E); 
    WriteData(0x14); 
    WriteData(0x03); 
    WriteData(0x11); 
    WriteData(0x07); 
    WriteData(0x31); 
    WriteData(0xC1); 
    WriteData(0x48); 
    WriteData(0x08); 
    WriteData(0x0F); 
    WriteData(0x0C); 
    WriteData(0x31); 
    WriteData(0x36); 
    WriteData(0x0F); 

    WriteCommand(ILI9340_SLPOUT);    //Exit Sleep 
    wait(0.120);       
    WriteCommand(ILI9340_DISPON);    //Display on 
    
    }
    


////////////////////////////////////////////////////////////////////////////////////////////////
// Sets the rotation of the display
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::SetRotation(uint8_t m) {

  WriteCommand(ILI9340_MADCTL);
  orientation = m % 4; // can't be higher than 3
  
  switch (orientation) {
   case 0:
     WriteData(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
     _width  = _TFTWIDTH;
     _height = _TFTHEIGHT;
     break;
   case 1:
     WriteData(ILI9340_MADCTL_MV | ILI9340_MADCTL_BGR);
     _width  = _TFTHEIGHT;
     _height = _TFTWIDTH;
     break;
  case 2:
    WriteData(ILI9340_MADCTL_MY | ILI9340_MADCTL_BGR);
     _width  = _TFTWIDTH;
     _height = _TFTHEIGHT;
    break;
   case 3:
     WriteData(ILI9340_MADCTL_MV | ILI9340_MADCTL_MY | ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
     _width  = _TFTHEIGHT;
     _height = _TFTWIDTH;
     break;
  }
}




////////////////////////////////////////////////////////////////////////////////////////////////
// Invert the colours of the display in hardware
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::InvertDisplay(bool i) {
  WriteCommand(i ? ILI9340_INVON : ILI9340_INVOFF);
}




////////////////////////////////////////////////////////////////////////////////////////////////    
// Set address window for writing data to.
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

  WriteCommand(ILI9340_CASET); // Column addr set
  WriteData(x0 >> 8);
  WriteData(x0 & 0xFF);     // XSTART 
  WriteData(x1 >> 8);
  WriteData(x1 & 0xFF);     // XEND

  WriteCommand(ILI9340_PASET); // Row addr set
  WriteData(y0>>8);
  WriteData(y0);     // YSTART
  WriteData(y1>>8);
  WriteData(y1);     // YEND

  WriteCommand(ILI9340_RAMWR); // write to RAM
}




////////////////////////////////////////////////////////////////////////////////////////////////
// To draw the humble pixel
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawPixel(uint16_t x, uint16_t y, uint16_t colour) {
    if((x < 1) ||(x >= _width) || (y < 1) || (y >= _height)) return;
   
    SetAddrWindow(x,y,x+1,y+1);
    
    dc = 1;
    cs = 0;
    
    spi.write(colour >> 8);
    spi.write(colour);
    
    cs = 1;
    }
    


////////////////////////////////////////////////////////////////////////////////////////////////
// Fill the screen with a colour
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::FillScreen(uint16_t colour) {
    SetAddrWindow(0,0,_width,_height);
    
    dc = 1;
    cs = 0;
    
    unsigned int total = _width * _height;
    unsigned int position = 0;
    
    while (position < total) {
        spi.write(colour >> 8);
        spi.write(colour);
        position++;
        }
    cs = 1;
    }
    


////////////////////////////////////////////////////////////////////////////////////////////////
// Draws a vertical line fast
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colour) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;

  if((y+h-1) >= _height) 
    h = _height-y;

  SetAddrWindow(x, y, x, y+h-1);

  uint8_t hi = colour >> 8, lo = colour;

  dc = 1;
  cs = 0;

  while (h--) {
    spi.write(hi);
    spi.write(lo);
  }
  cs = 1;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// Draws a horizontal line fast
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colour) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _height)  w = _width-x;
  SetAddrWindow(x, y, x+w-1, y);

  uint8_t hi = colour >> 8, lo = colour;
  dc = 1;
  cs = 0;
  while (w--) {
    spi.write(hi);
    spi.write(lo);
  }
  cs = 1;
}




////////////////////////////////////////////////////////////////////////////////////////////////
// Draws a filled rectangle 
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colour) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  SetAddrWindow(x, y, x+w-1, y+h-1);

  uint8_t hi = colour >> 8, lo = colour;

  dc = 1;
  cs = 0;

  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      spi.write(hi);
      spi.write(lo);
    }
  }
  cs = 1;
}




////////////////////////////////////////////////////////////////////////////////////////////////
// Draw an unfilled rectangle
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
    DrawFastHLine(x, y, w, color);
    DrawFastHLine(x, y+h-1, w, color);
    DrawFastVLine(x, y, h, color);
    DrawFastVLine(x+w-1, y, h, color);
}





////////////////////////////////////////////////////////////////////////////////////////////////
// draw an unfilled circle
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t colour){
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
 
    DrawPixel(x0  , y0+r, colour);
    DrawPixel(x0  , y0-r, colour);
    DrawPixel(x0+r, y0  , colour);
    DrawPixel(x0-r, y0  , colour);
 
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
 
        DrawPixel(x0 + x, y0 + y, colour);
        DrawPixel(x0 - x, y0 + y, colour);
        DrawPixel(x0 + x, y0 - y, colour);
        DrawPixel(x0 - x, y0 - y, colour);
        DrawPixel(x0 + y, y0 + x, colour);
        DrawPixel(x0 - y, y0 + x, colour);
        DrawPixel(x0 + y, y0 - x, colour);
        DrawPixel(x0 - y, y0 - x, colour);
    }
}





////////////////////////////////////////////////////////////////////////////////////////////////
// Draw a filled circle
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t colour) {
  DrawFastVLine(x0, y0-r, 2*r+1, colour);
  FillCircleHelper(x0, y0, r, 3, 0, colour);
}





////////////////////////////////////////////////////////////////////////////////////////////////
// used to draw circles and roundrects!
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t colour) {
 
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;
 
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
 
    if (cornername & 0x1) {
      DrawFastVLine(x0+x, y0-y, 2*y+1+delta, colour);
      DrawFastVLine(x0+y, y0-x, 2*x+1+delta, colour);
    }
    if (cornername & 0x2) {
      DrawFastVLine(x0-x, y0-y, 2*y+1+delta, colour);
      DrawFastVLine(x0-y, y0-x, 2*x+1+delta, colour);
    }
  }
}





////////////////////////////////////////////////////////////////////////////////////////////////
// used for drawing rounded corner radii
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t colour) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;
 
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      DrawPixel(x0 + x, y0 + y, colour);
      DrawPixel(x0 + y, y0 + x, colour);
    } 
    if (cornername & 0x2) {
      DrawPixel(x0 + x, y0 - y, colour);
      DrawPixel(x0 + y, y0 - x, colour);
    }
    if (cornername & 0x8) {
      DrawPixel(x0 - y, y0 + x, colour);
      DrawPixel(x0 - x, y0 + y, colour);
    }
    if (cornername & 0x1) {
      DrawPixel(x0 - y, y0 - x, colour);
      DrawPixel(x0 - x, y0 - y, colour);
    }
  }
}




////////////////////////////////////////////////////////////////////////////////////////////////
// draw a rounded rectangle!
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t colour) {
  // smarter version
  DrawFastHLine(x+r  , y    , w-2*r, colour); // Top
  DrawFastHLine(x+r  , y+h-1, w-2*r, colour); // Bottom
  DrawFastVLine(  x    , y+r  , h-2*r, colour); // Left
  DrawFastVLine(  x+w-1, y+r  , h-2*r, colour); // Right
  // draw four corners
  DrawCircleHelper(x+r    , y+r    , r, 1, colour);
  DrawCircleHelper(x+w-r-1, y+r    , r, 2, colour);
  DrawCircleHelper(x+w-r-1, y+h-r-1, r, 4, colour);
  DrawCircleHelper(x+r    , y+h-r-1, r, 8, colour);
}





////////////////////////////////////////////////////////////////////////////////////////////////
// fill a rounded rectangle!
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t colour) {
  // smarter version
  FillRect(x+r, y, w-2*r, h, colour);
 
  // draw four corners
  FillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, colour);
  FillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, colour);
}





////////////////////////////////////////////////////////////////////////////////////////////////
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t ILI9340_Display::Colour565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}




////////////////////////////////////////////////////////////////////////////////////////////////
// Writes an ascii character to the display
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawAscii(unsigned char ascii, uint16_t x, uint16_t y, uint16_t size, uint16_t colour) {
    SetAddrWindow(x, y, x+size, y+size);
    
    if( (ascii < 0x20) || (ascii > 0x7e) )      //check for valid ASCII char
    {
        ascii = '?';                            //char not supported
    }
    for(unsigned char i=0; i<8; i++)
    {
        unsigned char temp = simpleFont[ascii - 0x20][i];
        for(unsigned char f=0; f<8; f++)
        {
            if( (temp>>f) & 0x01 )
            {
                switch(orientation)
                {                
                case '0':
                    FillRect(x+f*size, y-i*size, size, size, colour);
                    break;
                case '1':
                     FillRect(x-i*size, x-f*size, size, size, colour);
                     break;
                case '2':
                     FillRect(x-f*size, y+i*size, size, size, colour);
                     break;
                case '3':
                default:
                       FillRect(x+i*size, y+f*size, size, size, colour);
                }
            }    
        }
    }
}





////////////////////////////////////////////////////////////////////////////////////////////////
// Writes a character array to the display
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawString(char *string, uint16_t x, uint16_t y, uint8_t size, uint16_t colour)
{
    while(*string)
    {
        DrawAscii(*string, x, y, size, colour);
        *string++;
        switch(orientation)
        {        
        case '0':          
            if(y > 0) y-=8*size;              //Change position to next char 
              break;
        case '1':        
            if(x > 0) x-=8*size;                       
            break;
        case '2':          
            if(y < _height) y+=8*size;   
            break;
        case '3':
        default:        
              if(x < _width) x+=8*size; 
        }          
    }
}





////////////////////////////////////////////////////////////////////////////////////////////////
// Converts integers into a character array
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::IntToChars (char* buffer, int value, uint8_t spaceonbuffer, uint8_t countbase, uint16_t x, uint16_t y, uint8_t size, uint16_t colour) {
    int workvalue = value;
    int i;
    int valuetowrite;
    int  end_i = 0;

    if (value < 0)
    {
        workvalue = -value;
        end_i = 1;
        buffer[0] = '-';
    }

    for (i = spaceonbuffer - 1; i >= end_i; i--)
    {
        valuetowrite = (workvalue % countbase);
        if (workvalue == 0)
        {
            if (i == (spaceonbuffer - 1))
            {
                buffer[i] = 48;                        // ASCII 0
            } else {
                buffer[i] = 32;                        // ASCII SPACE
            }
        } else {
            if (valuetowrite > 9)
            {
                buffer[i] = valuetowrite + 55;        // ASCII A-Z
            } else {
                buffer[i] = valuetowrite + 48;        // ASCII of that character
            }
        };
        workvalue = (workvalue - valuetowrite) / countbase;
    }
    
    DrawString(buffer, x, y, size, colour);
}





////////////////////////////////////////////////////////////////////////////////////////////////
// Functional code to swap data contents of 16bit registers
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::Swap(int16_t *a, int16_t *b) {
    
    int16_t x = *a;
    *a = *b;
    *b = x;    
    }




////////////////////////////////////////////////////////////////////////////////////////////////
// Draws a line with any length and orientation
////////////////////////////////////////////////////////////////////////////////////////////////
void ILI9340_Display::DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour){
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        Swap(&x0, &y0);
        Swap(&x1, &y1);
    }
 
    if (x0 > x1) {
        Swap(&x0, &x1);
        Swap(&y0, &y1);
    }
 
    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
 
    int16_t err = dx / 2;
    int16_t ystep;
 
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }
 
    for (; x0<=x1; x0++) {
        if (steep) {
            DrawPixel(y0, x0, colour);
        } else {
            DrawPixel(x0, y0, colour);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}








