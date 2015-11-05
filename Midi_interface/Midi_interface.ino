/*******************************************************************************

 Bare Conductive Touch USB MIDI instrument
 -----------------------------------------
 
 Midi_interface.ino - USB MIDI touch instrument - based on a piano
 
 Remember to select Bare Conductive Touch Board (USB MIDI, iPad compatible) 
 in the Tools -> Board menu
 
 Bare Conductive code written by Stefan Dzisiewski-Smith and Peter Krige.
 
 This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 
 Unported License (CC BY-SA 3.0) http://creativecommons.org/licenses/by-sa/3.0/
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*******************************************************************************/

// compiler error handling
#include "Compiler_Errors.h"

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
