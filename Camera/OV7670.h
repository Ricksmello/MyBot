#pragma once
#include "I2SCamera.h"
#include "I2C.h"

class OV7670: public I2SCamera
{
  public:
  enum Mode
  {
    QQQVGA_RGB565,
    QQVGA_RGB565,
    QVGA_RGB565,
    VGA_RGB565,
  };
  int xres, yres;

  protected:
  static const int ADDR = 0x42;
  
  Mode mode;
  I2C i2c;

  void testImage();
  void saturation(int s);
  void frameControl(int hStart, int hStop, int vStart, int vStop);
  void QQVGA();
  void QQVGARGB565();
  void QQQVGA();
  void QQQVGARGB565();
  void inline writeRegister(unsigned char reg, unsigned char data)
  {
    i2c.writeRegister(ADDR, reg, data);
  }

  public:
  OV7670(OV7670::Mode m, const int SIOD, const int SIOC, const int VSYNC, const int HREF, const int XCLK, const int PCLK, const int D0, const int D1, const int D2, const int D3, const int D4, const int D5, const int D6, const int D7);


//camera registers
  static const int ADCCTR0 = 0x20;
  static const int COM11_50HZ = 0x08;
  static const int COM11_EXP = 0x0;
  static const int COM11_HZAUTO = 0x10;
  static const int COM11_NIGHT = 0x80;
  static const int COM11_NMFR = 0x60;
  static const int COM13_UVSAT = 0x40;
  static const int COM15_R00FF = 0xc0;
  static const int COM15_RGB565 = 0x10;
  static const int COM16_AWBGAIN = 0x08; /* AWB gain enable */
  static const int COM17_CBAR = 0x08; /* DSP Color bar */
  static const int COM7_FMT_VGA = 0x00;
  static const int COM8_AEC = 0x0;
  static const int COM8_AECSTEP = 0x40;/* Unlimited AEC step size */
  static const int COM8_AGC = 0x04;/* Auto gain enable */
  static const int COM8_AWB = 0x02;/* White balance enable */
  static const int COM8_BFILT = 0x20;/* Band filter enable */
  static const int COM8_FASTAEC = 0x80;
  static const int REG_AEB = 0x25; /* AGC lower limit */
  static const int REG_AECH = 0x10; /* More bits of AEC value */
  static const int REG_AEW = 0x24; /* AGC upper limit */
  static const int REG_BD50MAX = 0xa5;
  static const int REG_BD60MAX = 0xab;
  static const int REG_BLUE = 0x01; /* blue gain */
  static const int REG_CLKRC = 0x11; /* Clock control */
  static const int REG_COM1 = 0x04; /* Control 1 */
  static const int REG_COM10 = 0x15;
  static const int REG_COM11 = 0x3B; /* Control 11 */
  static const int REG_COM12 = 0x3C; /* Control 12 */
  static const int REG_COM13 = 0x3D; /* Control 13 */
  static const int REG_COM14 = 0x3E; /* Control 14 */
  static const int REG_COM15 = 0x40; /* Control 15 */
  static const int REG_COM16 = 0x41; /* Control 16 */
  static const int REG_COM17 = 0x42; /* Control 17 */
  static const int REG_COM3 = 0x0C;
  static const int REG_COM4 = 0x0d; /* Control 4 */
  static const int REG_COM5 = 0x0e; /* All "reserved" */
  static const int REG_COM6 = 0x0f; /* Control 6 */
  static const int REG_COM7 = 0x12; /* Control 7 */
  static const int REG_COM8 = 0x13;
  static const int REG_COM9 = 0x14;
  static const int REG_EDGE = 0x3f;
  static const int REG_GAIN = 0x00; /* Gain lower 8 bits (rest in vref) */
  static const int REG_GFIX = 0x69;
  static const int REG_HAECC1 = 0x9f;
  static const int REG_HAECC2 = 0xa0;
  static const int REG_HAECC3 = 0xa6;
  static const int REG_HAECC4 = 0xa7;
  static const int REG_HAECC5 = 0xa8;
  static const int REG_HAECC6 = 0xa9;
  static const int REG_HAECC7 = 0xaa;
  static const int REG_HREF = 0x32; /* HREF pieces */
  static const int REG_HSTART = 0x17; /* Horiz start high bits */
  static const int REG_HSTOP = 0x18; /* Horiz stop high bits */
  static const int REG_MVFP = 0x1E;
  static const int REG_RED = 0x02; /* red gain */
  static const int REG_REG76 = 0x76;
  static const int REG_RGB444 = 0x8C;
  static const int REG_SCALING_DCWCTR = 0x72;
  static const int REG_SCALING_PCLK_DELAY = 0xa2;
  static const int REG_SCALING_PCLK_DIV = 0x73;
  static const int REG_SCALING_XSC = 0x70;
  static const int REG_SCALING_YSC = 0x71;
  static const int REG_TSLB = 0x3A;
  static const int REG_VPT = 0x26;
  static const int REG_VREF = 0x0b; /* Product ID LSB */
  static const int REG_VSTART = 0x10; /* Vert start high bits */
  static const int REG_VSTOP = 0x1A; /* Vert stop high bits */
};
