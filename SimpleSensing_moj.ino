#include <hellodrum.h>
#include <USB-MIDI.h>

#define DEBUG_DRUM

#include <LiquidCrystal.h>

/*
  EXAMPLE - Simple Piezo Sensing

  With this sample code, you will make snare and tom using two piezo.
  https://github.com/RyoKosaka/HelloDrum-arduino-Library
*/

///////////////////////////////   SETTING VALUE   ///////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

int led = 13;
char *lcd_str;
bool LED = false;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 

byte SNARE[6] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    10,  //scan time (1-)
    30,  //mask time (1-)
    38,  //note (0-127)
    1    //curve type (0-4)
};

byte TOM[6] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    10,  //scan time (1-)
    30,  //mask time(1-)
    50,  //note (0-127)
    1    //curve type(0-4)
};

byte HIHAT[7] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    0,   //curve type
    46,  //note of open
    42   //note of close
};

/*
byte HIHAT_PEDAL[7] = {
    90, //sensitivity (Hold the pedal at the position where you want to send 127(full open) in the MIDI CC, and read the Analog Read value in the TCRT5000. Divide the value by 10 and enter the value.)
    30, //threshold (Hold the pedal at the position where you want to send 0(close) in the MIDI CC, and read the Analog Read value in the TCRT5000. Divide the value by 10 and enter the value.)
    70, //scan start (When the analogRead value reaches this value * 10, the scan will start and the time will be recorded.)
    90, //scan end (When the analogRead value reaches this value * 10, the scan is finished and the time taken for the scan is referenced to determine the velocity.Then noteOn will be sent.The default value is 90, the same as Sensitivity. This means that the pedal is closed at the point where MIDI CC is 127.)
    10, //pedal sensitivity (This value multiplied by 100 is the time used to determine the velocity of the pedal. If the time taken from the scan start position to the scan end position is more than 1000 milliseconds, the velocity will be set to 1. So, if you feel that the velocity is low, either increase pedalSensitivity or increase the value of scan start.)
    0,  //curve type
    44  //note of pedal
};
*/

/////////////////////////////////////////////////////////////////////////////////////

//Using MIDI Library. If you want to use USB-MIDI, comment out the next two lines.
USBMIDI_CREATE_DEFAULT_INSTANCE();

//Uncomment the next two lines for using USB-MIDI with atmega32u4 or Teensy
//#include <USB-MIDI.h>
//USBMIDI_CREATE_DEFAULT_INSTANCE();

//The piezo named snare is connected to the A0 pin and the piezo named tom is connected to the A1 pin.

HelloDrum snare(0);

HelloDrum hihat(1);
//HelloDrum snare(1);
HelloDrum tom(2);

void setup()
{
  lcd.begin(16, 2); // ustawienie typu wyświetlacza, w tym wypadku 16x2
  
  pinMode(led, OUTPUT);
  pinMode(12,OUTPUT);

  MIDI.begin(10);
  
  hihat.setCurve(HIHAT[4]);
  snare.setCurve(SNARE[5]);
  tom.setCurve(TOM[5]);
}

void loop()
{
  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages - For each piezo, one line is required.
  snare.singlePiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);
  tom.singlePiezo(TOM[0], TOM[1], TOM[2], TOM[3]);
  hihat.singlePiezo(HIHAT[0], HIHAT[1], HIHAT[2], HIHAT[3]);
  //MIDI signals are transmitted with this IF statement - For each piezo, one IF statement is required

 int s_count = 0;
 int t_count = 0;
 int h_count = 0;

  if (hihat.hit == true)
  {
    h_count++;
    LED = not LED;
    digitalWrite(12,LED);
    MIDI.sendNoteOn(HIHAT[4], hihat.velocity, 1); //(note, velocity, channel)
    MIDI.sendNoteOff(HIHAT[4], 0, 10);
  }


  if (snare.hit == true)
  {
    s_count++;
    MIDI.sendNoteOn(SNARE[4], snare.velocity, 1); //(note, velocity, channel)
    MIDI.sendNoteOff(SNARE[4], 0, 10);
  }

  if (tom.hit == true)
  {
    t_count++;
    MIDI.sendNoteOn(TOM[4], tom.velocity, 1); //(note, velocity, channel)
    MIDI.sendNoteOff(TOM[4], 0, 10);
  }

  lcd.clear();
  sprintf(lcd_str, "H = %d, S = %d, T = %d", h_count, s_count, t_count);
  lcd.print(lcd_str);
  
}