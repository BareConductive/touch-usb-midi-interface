/*******************************************************************************

 Bare Conductive Touch USB MIDI instrument
 -----------------------------------------

 Midi_interface.ino - USB MIDI touch instrument

 Remember to select Bare Conductive Touch Board (USB MIDI, iPad compatible)
 in the Tools -> Board menu

 Bare Conductive code written by Stefan Dzisiewski-Smith, Peter Krige, Pascal
 Loose and Szymon Kaliski.

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

// touch includes
#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>

// touch constants
const uint32_t BAUD_RATE = 115200;
const uint8_t MPR121_ADDR = 0x5C;
const uint8_t MPR121_INT = 4;

// MPR121 datastream behaviour constants
const bool MPR121_DATASTREAM_ENABLE = false;

// MIDI behaviour constants
const bool SWITCH_OFF = false;  // if set to "true", touching an electrode once turns the note on, touching it again turns it off
                                 // if set to "false", the note is only on while the electrode is touched
const uint8_t NOTES[] = {59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48};  // piano notes from C3 to B3 in semitones
                                                                           // a full overview to convert musical notes to MIDI note can be found here:
                                                                           // http://newt.phys.unsw.edu.au/jw/notes.html
const uint8_t CHANNEL = 0;  // default channel is 0

// MIDI variables
bool note_status[12] = {false, false, false, false, false, false, false, false, false, false, false, false};

void setup() {
  Serial.begin(BAUD_RATE);

  pinMode(LED_BUILTIN, OUTPUT);

  if (!MPR121.begin(MPR121_ADDR)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }

  MPR121.setInterruptPin(MPR121_INT);

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121.restoreSavedThresholds();
    MPR121_Datastream.begin(&Serial);
  } else {
    MPR121.setTouchThreshold(40);
    MPR121.setReleaseThreshold(20);
  }

  MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);
  MPR121.setGlobalCDT(CDT_4US);  // reasonable for larger capacitances
  
  digitalWrite(LED_BUILTIN, HIGH);  // switch on user LED while auto calibrating electrodes
  delay(1000);
  MPR121.autoSetElectrodes();  // autoset all electrode settings
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  MPR121.updateAll();

  for (int i=0; i < 12; i++) {
    if (MPR121.isNewTouch(i)) {
      // if we have a new touch, turn on the onboard LED and
      // send a "note on" message, or if in toggle mode,
      // toggle the message
      digitalWrite(LED_BUILTIN, HIGH);

      if (!SWITCH_OFF) {
        noteOn(CHANNEL, NOTES[i], 127);  // maximum velocity
      } else {
        if (note_status[i]) {
          noteOff(CHANNEL, NOTES[i], 127);  // maximum velocity
        } else {
          noteOn(CHANNEL, NOTES[i], 127);  // maximum velocity
        }
        note_status[i] = !note_status[i];  // toggle note status
      }
    } else if (MPR121.isNewRelease(i)) {
      // if we have a new release, turn off the onboard LED and
      // send a "note off" message (unless we're in toggle mode)
      digitalWrite(LED_BUILTIN, LOW);

      if (!SWITCH_OFF) {
        noteOff(CHANNEL, NOTES[i], 127);  // maximum velocity
      }
    }
  }

  // flush USB buffer to ensure all notes are sent
  MIDIUSB.flush();

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }

  delay(10);  // 10ms delay to give the USB MIDI target time to catch up
}

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDIUSB.write({0x09, 0x90 | (channel & 0x0F), pitch, velocity});
}

void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDIUSB.write({0x08, 0x80 | (channel & 0x0F), pitch, velocity});
}
