/*
 * This code is not PERFECT and CLEAN, must be looked as a sample
 * wich allow find ideas to use the driver of VFD M66004SP
 * I try let functions as simple possible and without any library
 * or jump to others codes wich don't let use it.
 * Each funtion must be looked as independent block and this
 * make it possible implement it on any solution.
 */
#include <stdio.h>
int VFD_clk = 7;
int VFD_data = 9;
int VFD_cs = 8;
/***************************************************************/
#ifndef __VFD_H__
#define __VFD_H__
#include <stdio.h>
/*---------------------------------------------------------------------------*/
#define VFD_clk           (7)  /* serial clock for both M66004 and '165 */
#define VFD_data           (9)  /* serial data to M66004 */
#define VFD_cs            (8)  /* chip select M66004 */
/*---------------------------------------------------------------------------*/
#endif
/***************************************************************/
void vfd_init(void){
  /* Setup pins */ 
  /* Init the display board */
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_data, OUTPUT);
  pinMode(VFD_cs, OUTPUT);
  
  digitalWrite(VFD_clk, HIGH);
  digitalWrite(VFD_data, LOW);
  digitalWrite(VFD_cs, HIGH);
  /* set 16 chars length of display */
  send_byte(0x00 | 0x04);  //13 grids, I define it. Initial used the & instead of |, but not working to the test
  // of the port 0 and port 1, with pipe work ok. Check it in time.
  delay(4);
  /* Enable auto increment (should never be turned off) */
  send_byte(0xF4 | 0x01); // active autoincrement
  delay(4);
  /* Set dimmer value + freq (128/fOSC) */
  send_byte(0x08 | 0x07);  // dimmer maxi
  delay(4);
  send_byte(0xF0 | 0x06); // Constant display freq
  delay(4);
  write_ram();
  delay(4);
}
/*-------------------send command with the CS signal------------------------*/
static void send_byte(uint8_t b)
{
  uint8_t i;
  uint8_t bit = 0x80;
  // start condition
  digitalWrite(VFD_cs, LOW);
  // MSB first, shift on SCK lo->hi transition
  //for (i = 0; i < 8; i += 1) {
     for (i = 0; i < 8; i ++) {
    digitalWrite(VFD_clk, LOW);
    if(b & bit) {
      digitalWrite(VFD_data, HIGH);
    } else {
      digitalWrite(VFD_data, LOW);
    }
    digitalWrite(VFD_clk, HIGH);
    bit = bit >> 1;             // shift so we check the next one in order
  }
  // end condition
  digitalWrite(VFD_cs, HIGH);
  delay(1);
}
/*-----------------------send command without CS-----------------------------*/
static void send_byte_without_CS(uint8_t b)
{
  uint8_t i;
  uint8_t bit = 0x80;
  // MSB first, shift on VFD_clk lo->hi transition
  //for (i = 0; i < 8; i += 1) {
     for (i = 0; i < 8; i ++) {
    digitalWrite(VFD_clk, LOW);
    delayMicroseconds(1);
    if(b & bit) {
      digitalWrite(VFD_data, HIGH);
      delayMicroseconds(1);
    } else {
      digitalWrite(VFD_data, LOW);
      delayMicroseconds(1);
    }
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(1);
    bit = bit >> 1;             // shift so we check the next one in order
  }
  delay(1);
}
/*****************************************************************************************/
static void send_arrayVector(uint8_t *bv, uint8_t len)
{
  uint8_t i, j, b;
  // start condition
  digitalWrite(VFD_cs, LOW);
  // transmit each byte
  for (j = 0; j < len; j += 1) {
    uint8_t bit = 0x80;
    /* load next byte to transmit */
    b = *(bv + j);
    /* MSB first, shift on SCK lo->hi transition */
    for (i = 0; i < 8; i += 1) {
      digitalWrite(VFD_clk, LOW);
      if(b & bit) {
        digitalWrite(VFD_data, HIGH);
      } else {
        digitalWrite(VFD_data, LOW);
      }
      digitalWrite(VFD_clk, HIGH);
      bit = bit >> 1;             /* shift so we check the next one in order */
    }
    /* small delay here, tBUSY = 6 cycles at 500 kHz =~ 14 µs == 14 cycles at 1 MHz. */
  }
  // end condition
  digitalWrite(VFD_cs, HIGH);
}
/*******************************************************************************************/
void write_ram(){
digitalWrite(VFD_cs, LOW);
delay(1);
send_byte_without_CS(0xFC);// Display digit is set to the  first Digits(first digit is on the left side to this display)
delay(1);
send_byte_without_CS(0x00 ); // Position o RAM address to write
delay(1);
send_byte_without_CS(0b00000000 ); // 1th column of grup 7*5  // the bit corresponding to 0 of byte is not matter the value.
delay(1);
send_byte_without_CS(0b00000000 ); // 2th column of grup 7*5
delay(1);
send_byte_without_CS(0b00000000 ); // 3th column of grup 7*5
delay(1);
send_byte_without_CS(0b00000000 ); // 4th column of grup 7*5
delay(1);
send_byte_without_CS(0b00000000 ); // 5th column of grup 7*5
delay(1);
digitalWrite(VFD_cs, HIGH);
delay(1);
}
/*********************************************************************************/
void fill_grid_11_ram(){
digitalWrite(VFD_cs, LOW);
delay(1);
send_byte_without_CS(0xFC);// Display digit is set to the  first Digits(first digit is on the left side to this display)
delay(1);
send_byte_without_CS(0x00 ); // Position o RAM address to write
delay(1);
for(int n=0; n<5; n++){
  send_byte_without_CS(0x3E); // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
}
digitalWrite(VFD_cs, HIGH);
delay(1);
}
/********************************************************************************/
void grid_11_fill_RAM(){
  uint8_t RAM=0x00;
  uint8_t data=0x00;
  uint8_t column=0;
  // Here I fill the matrix of 5*7 and use the total of 16 position of RAM customer define.
  // you can refill the remaning symbols until from 16 to 24 symbols by active mode.
      for( RAM=0; RAM < 16; RAM ++){
      digitalWrite(VFD_cs, LOW);
      delayMicroseconds(1);
                    send_byte_without_CS(0xFC);// Write to RAM chars of customer.
                    delayMicroseconds(1);
                    send_byte_without_CS(0x00 | RAM ); // Position o RAM address to write
                    delayMicroseconds(1);
 
                  switch (RAM){
                  case 0x00:
                            send_byte_without_CS(0x02);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00); 
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x01:
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x02);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x02:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x02);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x03:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x02);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x04:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x02);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x05:
                            send_byte_without_CS(0x04);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00); 
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x06:
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x04);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x07:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x04);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x08:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x04);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x09:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x04);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x0A:
                            send_byte_without_CS(0x08);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00); 
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x0B:
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x08);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x0C:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x08);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x0D:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x08);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x0E:  
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x08);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                  case 0x0F:  
                            send_byte_without_CS(0x10);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                            send_byte_without_CS(0x00);
                  break; // the bit 7, 6 & 0, don't matter. Don't have symbols assigned.
                   }
      digitalWrite(VFD_cs, HIGH);
      delayMicroseconds(10);
    } 
} 
/***************************************************************************************/
void test(){
  unsigned long bit32 = 0x00000001;  // pay attention the int is 2 bytes in arduino uno, but is 4 bytes in arduino Duo! Check and take care with this!
  unsigned long var = 0x00;
  unsigned long bites = 0x00000001;
  unsigned long mask;
  uint8_t shift = 0x00;
  char word0, word1, word2, word3, word4, word5;
  bool flag = true;
  uint8_t data = 0x00;
unsigned int x = 5;        // binary: 0000000000000101
unsigned int y = 14;
unsigned int result = x << y;  // binary: 0100000000000000 - the first 1 in 101 was discarded
unsigned short MyShort = 0xEFAB;
unsigned char char1; // lower byte
unsigned char char2; // upper byte
// Split short into two char
char1 = MyShort & 0xFF;
char2 = MyShort >> 8;
// merge two char into short
MyShort = (char2 << 8) | char1;
Serial.print ("char1 : ");Serial.print (char1, HEX);Serial.print (", char2 : ");Serial.print (char2, HEX);
Serial.println();
Serial.print( x , BIN);
Serial.print (" : " );
Serial.println(result, BIN);

uint8_t intSize = sizeof(int);
uint8_t longSize = sizeof(long);
uint8_t shortSize = sizeof(short);
uint8_t byteSize = sizeof(byte);
uint8_t doubleSize = sizeof(double);
Serial.print("int : ");Serial.print(intSize, HEX);
Serial.print(", long : ");Serial.print(longSize, HEX);
Serial.print(", short : ");Serial.print(shortSize, HEX);
Serial.print(", byte : ");Serial.print(byteSize, HEX);
Serial.print(", double : ");Serial.print(doubleSize,  HEX);
Serial.println();
//for (mask=0x00000001; mask > 0; mask <<= 1) { //iterate through bit mask
  for (shift =0; shift < 32; shift ++){
    bites=((bit32 << shift) & 0xFFFFFFFF);
            word0=((bites) & 0xFF);
            word1=(bites >> 8);
            word2=(bites >> 16);
            word3=(bites >> 24);
            Serial.print("Var : ");
            Serial.print(var, BIN);
            Serial.print(", Bites : ");
            Serial.println(bites, BIN); 
            //debug
            Serial.println(bites, HEX);
            Serial.print(word0, HEX); 
            Serial.print( " , ");
            Serial.print(word1, HEX); 
            Serial.print( " , ");
            Serial.print(word2, HEX); 
            Serial.print( " , ");
            Serial.print(word3, HEX); 
            Serial.print( " , ");
            Serial.print(word4, HEX); 
            Serial.print(" ! ");
            Serial.print(shift); 
            Serial.println(" $ ");
  }
}
/***********************************************************************************/
void grid_11_tst(){
  uint8_t RAM =0x00;
  unsigned long bit32 = 0x00000001;  // pay attention the int is 2 bytes in arduino uno, but is 4 bytes in arduino Duo! Check and take care with this!
  unsigned long bites= 0x00;
  unsigned long var =0x00;
  uint8_t shift = 0x00;
  uint8_t word0, word1, word2, word3, word4;
  bool flag = true;
  uint8_t data = 0x00;
  
 for( RAM=0; RAM < 16; RAM ++){
      digitalWrite(VFD_cs, LOW);
      delayMicroseconds(1);
          for (shift =0; shift < 32; shift ++){
            bites=((bit32 << shift) & 0xFFFFFFFF);
            word0=((bites) & 0xFF);
            word1=(bites >> 8);
            word2=(bites >> 16);
            word3=(bites >> 24);
          
                    send_byte_without_CS(0xFC);// Write to RAM chars of customer.
                    delayMicroseconds(1);
                    send_byte_without_CS(0x00 | RAM ); // Position o RAM address to write
                    delayMicroseconds(1);
                    send_byte_without_CS(word0);
                    send_byte_without_CS(word1);
                    send_byte_without_CS(word2);
                    send_byte_without_CS(word3);
                    send_byte_without_CS(0x00); // The last bits don't have symbols assigned.
          }
      digitalWrite(VFD_cs, HIGH);
      delayMicroseconds(1);
 } 
}
/*************************************************************************************/
void show_pixel(){
  unsigned int RAM=0;
  uint8_t tmp =0x00;
  uint8_t data=0;
  bool flag = true;
  send_arrayVector("          ", 10); // send space to all acitvated digits to clear VFD
  send_byte(0xE0);
  delayMicroseconds(5);
send_byte(0xF0 | 0x04); // Set to autoincrement
delayMicroseconds(10);
// I only see 14 symbols, but the grid have 24. All position 0 of tabel of symbol are empty, also the 5º of first byte.
    for( RAM=0; RAM < 16; RAM++){
          digitalWrite(VFD_cs, LOW);
          delayMicroseconds(10);
          send_byte_without_CS(0xE0); // Here I fix it to stay on grid eleven to see all symbols of this grid.
          send_byte_without_CS(0x90 + RAM); // Display the contents of 16 position of customer RAM
          digitalWrite(VFD_cs, HIGH);
          delay(500);
    } 
}
/*************************************************************************************/
void show(){
  unsigned int RAM=0;
  uint8_t tmp =0x00;
  uint8_t data=0;
  bool flag = true;
  send_arrayVector("          ", 10); // send space to all acitvated digits to clear VFD
  send_byte(0xE0);
  delayMicroseconds(5);
  for( RAM=0; RAM < 16; RAM++){
      digitalWrite(VFD_cs, LOW);
      delayMicroseconds(5);
                if (RAM > 0x09){  // 0x09 is grid number 10, on the VFD of 10 grids
                    if (flag== true){
                      flag = false;
                      //Serial.print(" Flag: ");Serial.print (flag);Serial.print (", RAM: ");Serial.println(RAM);
                       send_byte_without_CS(0xE0);
                delayMicroseconds(50);
                     }
                }
          send_byte_without_CS((0x41 + RAM));
          delayMicroseconds(5);
      digitalWrite(VFD_cs, HIGH);
      delay(500);  
    }
    delay(500);
send_byte(0xF0 | 0x05); // Set to autoincrement
delayMicroseconds(10);
// I only see 14 symbols, but the grid have 24. All position 0 of tabel of symbol are empty, also the 5º of first byte.
    for( RAM=0; RAM < 16; RAM++){
          digitalWrite(VFD_cs, LOW);
          delayMicroseconds(10);
          send_byte_without_CS(0xEA); // Here I fix it to stay on grid eleven to see all symbols of this grid.
          send_byte_without_CS(0x90 + RAM); // Display the contents of 16 position of customer RAM
          digitalWrite(VFD_cs, HIGH);
          delay(500);
    } 
}
/***********************************************************************/
void clean(){
send_byte(0xE0);
delayMicroseconds(5);
send_arrayVector("          ", 10); // send space to all acitvated digits to clear VFD
delayMicroseconds(5);
send_byte(0xE0);
delayMicroseconds(5);
}
/***********************************************************************/
void ports(){
send_arrayVector("ON/OFF 0&1", 10);
delay(1000);
clean();
send_arrayVector("ON 0 & 1", 0x08);
send_byte(0xF8 | 0x03); // active port0 and port1
delay(1000);
clean();
send_arrayVector("ON Port 0", 0x09);
send_byte(0xF8 | 0x01); // active port 0
delay(1000);
clean();
send_arrayVector("ON Port 1", 0x09);
send_byte(0xF8 | 0x02); // active port 1 
delay(1000);
clean();
send_arrayVector("0 & 1 OFF", 0x09);
send_byte(0xF8 | 0x00); // deactive port0 ant port1
}
/*******************************************************************************/
void individualNumbers(){
digitalWrite(VFD_cs, LOW);
delayMicroseconds(5);
send_byte_without_CS(0xE0);// Display digit is set to the  first Digits(first digit is on the left side to this display)
delayMicroseconds(1);
send_byte_without_CS(0x30); // char
delayMicroseconds(1);
send_byte_without_CS(0x31); // char
delayMicroseconds(1);
send_byte_without_CS(0x32); // char
delayMicroseconds(1);
send_byte_without_CS(0x33); // char
delayMicroseconds(1);
send_byte_without_CS(0x34); // char
delayMicroseconds(1);
send_byte_without_CS(0x35); // char
delayMicroseconds(1);
send_byte_without_CS(0x36); // char
delayMicroseconds(1);
send_byte_without_CS(0x37); // char
delayMicroseconds(1);
send_byte_without_CS(0x38); // char
delayMicroseconds(1);
send_byte_without_CS(0x39); // char
delayMicroseconds(1);
digitalWrite(VFD_cs, HIGH);
delayMicroseconds(5);
}
void onAll(){
  /* next lines is only to blink all segments in the VFD */
send_byte(0xF0 | 0x03);// All on
delay(500);
send_byte(0xF0 | 0x00);// All on
delay(500);
send_byte(0xF0 | 0x03);// All on
delay(500);
send_byte(0xF0 | 0x00);// All on
delay(500);
send_byte(0xF0 | 0x03);// All on
delay(500);
send_byte(0xF0 | 0x00);// All on
delay(500);
}
/***********************************************************************/
void setup() {
  // put your setup code here, to run once:
 vfd_init();
delay(500);
Serial.begin(9600);
}
/**********************************************************************/
void loop() {
// put your main code here, to run repeatedly:
  
send_byte(0xF0 | 0x05);// 0xF5 to setting autoincrement, no autoincrement 0xF4
delay(1);
send_byte(0xF0 | 0x01);// Normal operation or all on or all off
delay(1);
onAll(); // This function do VFD all Bright.
send_byte(0xE0);
delayMicroseconds(1);
send_arrayVector("           ", 11); // send space to all acitvated digits to clear VFD
delayMicroseconds(1);
send_byte(0xF0 | 0x05);// 0xF5 to setting autoincrement, no autoincrement 0xF4
delayMicroseconds(5);
send_byte(0xF0 | 0x01);// Normal operation or all on or all off
delayMicroseconds(5);
individualNumbers(); // Write to display the numbers from 0 to 9 but by single mode digit.
delay(1000);
clean();
ports();  // This function make activation and deactivation of port 0 and 1. Pins 17 & 18 of M66004SP
delay(1000);
clean();
show_pixel(); // Here turn on pixel by pixel from matrix 5*7( start pixel 0 finish pixel 16) is range of memory RAM customer defined.
delay(1000);
clean();
send_byte(0x00 | 0x02);  //11 grids, I define it. Initial used the & instead of |, but not working to the test
// of the port 0 and port 1, with pipe work ok. Check it in time.
delayMicroseconds(10);
send_byte(0xF0 | 0x05);// 0xF5 to setting autoincrement, no autoincrement 0xF4
grid_11_fill_RAM(); // Here I fill os 16 positions of customer RAM with user bits to matrix 5*7.
delayMicroseconds(10);
show(); // This function show the 16 letters and the 16 bits
}
