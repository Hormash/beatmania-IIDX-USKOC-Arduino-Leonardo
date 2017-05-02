// Program used to test the USB Joystick object on the 
// Arduino Leonardo or Arduino Micro.
//
// Matthew Heironimus
// 2015-03-28
// Updated on 2015-11-18 to use the new Joystick library written for version 1.6.6.
//------------------------------------------------------------

#include "Joystick.h"

#define REPORT_DELAY 2000

#define ENCODER_Z_A 50
#define ENCODER_Z_B 51
#define ENCODER_Z_B_REGISTER 12
#define ENCODER_Z_MILLIS_TOLERANCE 100 // Amount of miliseconds to wait and change state of turntable buttons

#define ENC_L_A 2
#define ENC_L_B 3
#define ENC_L_B_ADDR 0
#define ENCODER_SENSITIVITY (double) 0.4686
#define ENCODER_PORT PIND

volatile int32_t encX = 0, encY = 0, encZ = 0, encZlast = 0, encZmillis = 0; // Storage for encoder states

int tmp;
uint8_t buttonCount = 9;
uint8_t buttonPins[] = {13,23,22,21,20,19,18,12,11};
int32_t encL=0;

void doEncL(){
  if((ENCODER_PORT >> ENC_L_B_ADDR)&1){
    encL++;
  } else {
    encL--;
  }
}

void setup() {
  delay(1000);

  for(int i=0;i<buttonCount;i++){
    pinMode(buttonPins[i],INPUT_PULLUP);
  }

  pinMode(ENC_L_A,INPUT_PULLUP);
  pinMode(ENC_L_B,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_L_A), doEncL, RISING);
}

void loop() {
 
  for(int i=0;i<buttonCount;i++){
    if(digitalRead(buttonPins[i])!=HIGH){
      Joystick.buttons |= (uint16_t)1 << i;
    } else {
      Joystick.buttons &= ~((uint16_t)1 << i);
    }
  }

  Joystick.xAxis = (uint8_t)((int32_t)(encL / ENCODER_SENSITIVITY) % 256);
  
  if( (int32_t)(encL / ENCODER_SENSITIVITY) - encZlast > 5) {
    if(millis() - encZmillis > ENCODER_Z_MILLIS_TOLERANCE || bitRead(Joystick.buttons,10)) {
      Joystick.buttons |= (uint16_t)1 << 10;
      Joystick.buttons &= ~((uint16_t)1 << 11);
      encZlast = (encL / ENCODER_SENSITIVITY);
      encZmillis = millis();
    }
  } else if ( (int32_t)(encL / ENCODER_SENSITIVITY) - encZlast < -5){
    if(millis() - encZmillis > ENCODER_Z_MILLIS_TOLERANCE || bitRead(Joystick.buttons,11)) {
      Joystick.buttons |= (uint16_t)1 << 11;
      Joystick.buttons &= ~((uint16_t)1 << 10);
      encZlast = (encL / ENCODER_SENSITIVITY);
      encZmillis = millis();
    }
  } else {
    if(millis() - encZmillis > ENCODER_Z_MILLIS_TOLERANCE) {
      Joystick.buttons &= ~((uint16_t)1 << 10);
      Joystick.buttons &= ~((uint16_t)1 << 11);
    }
  }
  Joystick.sendState();
  delayMicroseconds(REPORT_DELAY);
}




