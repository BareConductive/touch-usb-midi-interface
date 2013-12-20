/**********************************************************
 
  Bare Conductive Touch USB MIDI instrument
 
 **********************************************************
  
  Turns the Touch Board into a USB MIDI interface and 
  sends key presses and releases in accordance with 
  electrode status. In its standard form, works as a USB
  MIDI keyboard.
  
  Requires Arduino 1.5.6+ or greater and ARCore Hardware 
  Cores (https://github.com/rkistner/arcore) - remember to
  select "Bare Conductive Touch Board (arcore, iPad compatible)
  in the Tools -> Board menu
  
 **********************************************************  
 
  Code is by Stefan Dzisiewski-Smith for Bare Conductive 

***********************************************************/


#include <MPR121.h>
#include <Wire.h>
MIDIEvent e;

#define numElectrodes 12

void setup() {
  MPR121.begin(0x5C);
  MPR121.setInterruptPin(4);
  MPR121.updateTouchData();
  e.type = 0x08;
  e.m3 = 127;  // maximum volume
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if(MPR121.touchStatusChanged()){
    MPR121.updateTouchData();
    for(int i=0; i<numElectrodes; i++){
      
      // MIDI note mapping from electrode number to MIDI note
      e.m2 = 48 + numElectrodes - 1 - i;
      
      if(MPR121.isNewTouch(i)){
        // if we have a new touch, turn on the onboard LED and
        // send a "note on" message
        digitalWrite(LED_BUILTIN, HIGH);
        e.m1 = 0x90; 
      } else if(MPR121.isNewRelease(i)){
        // if we have a new release, turn off the onboard LED and
        // send a "note off" message
        digitalWrite(LED_BUILTIN, LOW);
        e.m1 = 0x80;
      } else {
        // else set a flag to do nothing...
        e.m1 = 0x00;  
      }
      // only send a USB MIDI message if we need to
      if(e.m1 != 0x00){
        MIDIUSB.write(e);
      }
    }
    // flush USB buffer to ensure all notes are sent
    MIDIUSB.flush(); 
  }
}
