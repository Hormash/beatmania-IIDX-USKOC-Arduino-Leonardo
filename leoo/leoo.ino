#include "iivx_leo.h"
iivxReport_t report; 

#define REPORT_DELAY 2000
// Number of microseconds between HID reports
// 2000 = 500hz

#define ENC_L_A 0
#define ENC_L_B 1
#define ENC_L_B_ADDR 3
#define ENCODER_SENSITIVITY (double) 0.2343
#define ENCODER_PORT PIND
// encoder sensitivity = number of positions per rotation (400) / number of positions for HID report (256)
/*
 * connect encoders
 * Scratch to pins 0 and 1
 */



int tmp;
uint8_t buttonCount = 9;
uint8_t lightMode = 0;
// 0 = reactive lighting, 1 = HID lighting
uint8_t ledPins[] = {2,3,4,5,6,7,8,9,10};
uint8_t buttonPins[] = {11,12,13,18,19,20,21,22,23};
uint8_t sysPin = 0;
int32_t encL=0;
/* current pin layout
 *  pins 1 to 9 = LED 1 to 9
 *    connect pin to + termnial of LED
 *    connect ground to resistor and then - terminal of LED
 *  pins 11 to 13, A0 to A5 = Button input 1 to 9
 *    connect button pin to ground to trigger button press
 *  pins 0 = system pin
 *    connect system pin to ground 
 *      together with other buttons to change lighting scheme
 *    system button + button 1 = reactive lighting
 *    system button + button 3 = HID lighting
 */

void doEncL(){
  if((ENCODER_PORT >> ENC_L_B_ADDR)&1){
    encL++;
  } else {
    encL--;
  }
}


void lights(uint8_t lightDesc){
  for(int i=0;i<buttonCount-1;i++){
     if((lightDesc>>i)&1){
         digitalWrite(ledPins[i],HIGH);
     } else {
         digitalWrite(ledPins[i],LOW);
     }
  }
}

void setup() {
  delay(1000);
  // Setup I/O for pins
  for(int i=0;i<buttonCount;i++){
    pinMode(buttonPins[i],INPUT_PULLUP);
    pinMode(ledPins[i],OUTPUT);
  }
  pinMode(sysPin,INPUT_PULLUP);

  //setup interrupts
  pinMode(ENC_L_A,INPUT_PULLUP);
  pinMode(ENC_L_B,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_L_A), doEncL, RISING);
}

void loop() {
  // Read buttons
  for(int i=0;i<buttonCount;i++){
    if(digitalRead(buttonPins[i])!=HIGH){
      report.buttons |= (uint16_t)1 << i;
    } else {
      report.buttons &= ~((uint16_t)1 << i);
    }
  }
  // Read Encoders
  report.xAxis = (uint8_t)((int32_t)(encL / ENCODER_SENSITIVITY) % 256);
  // Light LEDs
  if(lightMode==0){
    lights(report.buttons);
  } else {
    lights(iivx_led);
  }
  // Detect lighting changes
  if(digitalRead(sysPin)!=HIGH){
    if(report.buttons == 1){
      lightMode = 0;
    } else if (report.buttons == 4){
      lightMode = 1;
    } else if (report.buttons == 16){
      report.buttons = (uint16_t)1 << 9;
    } else if (report.buttons == 64){
      report.buttons = (uint16_t)1 << 10;
    }
  }
  // Send report and delay
  iivx.setState(&report);
  delayMicroseconds(REPORT_DELAY);
}

