/*******************************************************************************

 Bare Conductive Touch USB MIDI instrument
 -----------------------------------------
 
 Midi_interface.ino - USB MIDI touch instrument
 
 Remember to select Bare Conductive Touch Board (USB MIDI, iPad compatible) 
 in the Tools -> Board menu
 
 Bare Conductive code written by Stefan Dzisiewski-Smith and Peter Krige.
 
 This work is licensed under a MIT license https://opensource.org/licenses/MIT
 
 Copyright (c) 2016, Bare Conductive
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

*******************************************************************************/

// compiler error handling
#include "Compiler_Errors.h"

#include <MPR121.h>
#include <Wire.h>

const uint8_t numElectrodes = 12;

// if toggle is set to true, touching once turns the note on, again turns it off
// if toggle is set to false, the note is only on while the electrode is touched
const boolean toggle = false; 

// piano notes from C3 to B3 in semitones - you can replace these with your own values
// for a custom note scale - http://newt.phys.unsw.edu.au/jw/notes.html has an excellent
// reference to convert musical notes to MIDI note numbers
const uint8_t notes[numElectrodes] = {59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48};
const uint8_t channel = 0; // default channel is 0

// if you're connecting to iMPC on the iPad, comment out the two lines above and uncomment the two lines below
// iMPC expects note values in a very specific range - these map to pads 1 to 12
// const uint8_t notes[numElectrodes] = {37, 36, 42, 82, 40, 38, 46, 44, 48, 47, 45, 43};
// const uint8_t channel = 12; // iMPC works with inputs on channel 12

boolean noteStatus[numElectrodes] = {false, false, false, false, false, false, false, false, false, false, false, false};

void setup() {
  MPR121.begin(0x5C);
  MPR121.setInterruptPin(4);
  MPR121.updateTouchData();
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if(MPR121.touchStatusChanged()){
    MPR121.updateTouchData();
    for(int i=0; i<numElectrodes; i++){
      if(MPR121.isNewTouch(i)){
        // if we have a new touch, turn on the onboard LED and
        // send a "note on" message, or if in toggle mode, 
        // toggle the message 

        digitalWrite(LED_BUILTIN, HIGH);

        if(!toggle){
          noteOn(channel, notes[i], 127); // maximum velocity
        } else {
          if(noteStatus[i]){
            noteOff(channel, notes[i], 127); // maximum velocity
          } else {
            noteOn(channel, notes[i], 127); // maximum velocity
          }
          noteStatus[i] = !noteStatus[i]; // toggle note status
        }
      } else if(MPR121.isNewRelease(i)){
        // if we have a new release, turn off the onboard LED and
        // send a "note off" message (unless we're in toggle mode)
        digitalWrite(LED_BUILTIN, LOW);
        if(!toggle){
          noteOff(channel, notes[i], 127); // maximum velocity
        }
      }
    }
    // flush USB buffer to ensure all notes are sent
    MIDIUSB.flush(); 
  }
}

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDIUSB.write({0x09, 0x90 | (channel & 0x0F), pitch, velocity});
}

void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDIUSB.write({0x08, 0x80 | (channel & 0x0F), pitch, velocity});
}
