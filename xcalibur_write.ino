//Inject registers values into Xcalibur Video Encoder on a Original Xbox console with a v1.6 motherboard.
//Written by Ryan Wendland
//This proof of concept (PoC) was written to fix Xcalibur setting issues on a Xbox v1.6 console when running XeniumOS at 480p.
//Usage:
//1. Hook Arduino to Xboxes SMBus, (SDA,CLK and GND), wait for Xenium to boot into XeniumOS. Confirm you have graphical issues
//   as described here https://github.com/Ryzee119/OpenXenium/issues/2.
//2. Program this code to the Arduino to inject new SMBus registers.
//3. This is purely a PoC and the inject values will be lost each reboot, so its not particulary useful.

#include <Wire.h>

#define XCALIBUR_ADD 0x70

//From https://github.com/XboxDev/cromwell/blob/master/drivers/video/xcalibur-regs.h
//These are registers from Xcalibur starting at address 0x00. Each address is 4 bytes.
const u32 HDTV_XCal_Vals_480p[] = {
  0x02000000, 0x30004000, 0x00000000, 0x00000000,
  0x08000000, 0xa6c08324, 0xcf6f4600, 0x00000000,
  0xffffffff, 0x00000000, 0x00000000, 0xffffffff,
  0x0f000000, 0x00000000, 0xffffffff, 0x01000000,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
  0x00000000, 0x00000000, 0x00000000, 0x01000000,
  0xffffffff, 0xffffffff, 0xffffffff, 0x04000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0xfb001000, 0xdb016000, 0xe900e801,
  0x01000200, 0x02000300, 0xe801e700, 0x6200da01,
  0x1000fb00, 0x00000000, 0xfc000e00, 0xe0015100,
  0xc600ec01, 0x01000200, 0x02000200, 0xec01c500,
  0x5200e001, 0x0e00fc00, 0x04000000, 0x00000000,
  0x00000000, 0x04000000, 0x00000000, 0x00000000,
  0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0x0d020c03, 0x2594001e, 0x1c000028, 0xffffffff,
  0x0d020c03, 0x2594001e, 0x60000028, 0xffffffff,
  0x10808000, 0xf0000000, 0x4c000000, 0x0dd22000,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0x10000001, 0x20808000, 0x20808000, 0x20808000,
  0x20808000, 0x20808000, 0x20808000, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0x00000000, 0x00000000, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0x05000000, 0x0f000000, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

unsigned long *XCal_Reg;
unsigned char *regs;

void setup() {
  Wire.begin(0xDD); // join i2c bus (address optional for master)
  regs = malloc(4);
  XCal_Reg = HDTV_XCal_Vals_480p;
}

void loop() {

  //This code block is from
  //https://github.com/XboxDev/cromwell/blob/master/drivers/video/BootVgaInitialization.c
  uint32_t i = 0;
  ReadfromSMBus(XCALIBUR_ADD, 4, 4, &i);
  WriteToSMBus(XCALIBUR_ADD, 4, 4, 0x0F000000);
  ReadfromSMBus(XCALIBUR_ADD, 0, 4, &i);
  WriteToSMBus(XCALIBUR_ADD, 0, 4, 0x00000000);

  for (i = 0; i < 0x90; i++) {
    //Endianness.
    memcpy(regs, (unsigned char*)(&XCal_Reg[i]) + 3, 0x01);
    memcpy(regs + 1, (unsigned char*)(&XCal_Reg[i]) + 2, 0x01);
    memcpy(regs + 2, (unsigned char*)(&XCal_Reg[i]) + 1, 0x01);
    memcpy(regs + 3, (unsigned char*)(&XCal_Reg[i]), 0x01);
    WriteToSMBus(XCALIBUR_ADD, i, 4, *(unsigned long*)regs);
  }
  while (1);
}

int8_t ReadfromSMBus(uint8_t Address, uint8_t bRegister, uint8_t Size, uint32_t* Data_from_smbus) {
  Wire.beginTransmission(Address);
  Wire.write(bRegister);
  if (Wire.endTransmission(false) == 0) { //Needs to be false. Send I2c repeated start, dont release bus yet
    Wire.requestFrom(Address, Size);
    for (uint8_t i = 0; i < Size; i++) {
      Data_from_smbus[i] = Wire.read();
    }
    return 0;
  } else {
    return -1;
  }
}

int WriteToSMBus(uint8_t Address, uint8_t bRegister, uint8_t Size, uint32_t Data_to_smbus) {
  Wire.beginTransmission(Address);
  Wire.write(bRegister);
  Wire.write(Size); //Xcalibur writes seem to have a Size byte in the data.
  Wire.write(Data_to_smbus & 0xff);
  Wire.write((Data_to_smbus >> 8) & 0xff );
  Wire.write((Data_to_smbus >> 16) & 0xff );
  Wire.write((Data_to_smbus >> 24) & 0xff );
  Wire.endTransmission();    // stop transmitting
}
