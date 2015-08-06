[![Bare Conductive](https://www.dropbox.com/s/7qmvpvst3kal3qv/LOGO_256x106.png?dl=1)](http://www.bareconductive.com/)

# Bare Conductive Touch USB MIDI interface code
Simple touch-to-USB-MIDI code for the [Bare Conductive Touch Board](http://www.bareconductive.com/shop/touch-board/). Touching an electrode sends a USB MIDI message with a note - the 12 electrodes are mapped to a scale with one semitone per electrode - 12 semitones = 1 octave!

## Requirements
* [Arduino](http://arduino.cc/en/Main/Software) 1.5.6 or later

* Arduino's instructions for installing libraries are available [here](http://arduino.cc/en/Guide/Libraries)
	
* [Bare Conductive MPR121 library](https://github.com/bareconductive/mpr121) (zip file download [here](https://github.com/bareconductive/mpr121/archive/public.zip))

* [ARcore Arduino Hardware Plugin](https://github.com/rkistner/arcore) (zip file download [here](https://github.com/rkistner/arcore/archive/master.zip)) - instructions on installing this hardware plugin folder are available [here](https://github.com/rkistner/arcore).


## Install

1. Close the Arduino IDE if you have it open.
1. Download the [.zip](https://github.com/BareConductive/touch-usb-midi-interface/archive/public.zip) or clone the repository to your local machine - if downloading the .zip, extract the contents somewhere that suits you.
1. Take the **Midi_interface** folder and move it to **Arduino Sketchbook Folder**. This will be different for each operating system: 

	**Windows**
	
	Libraries\\Documents\\Arduino
	
	or
	
	My Documents\\Arduino	
	
	**Mac**
	
	Documents/Arduino
	
	**Linux (Ubuntu)**
	
	Home/Arduino


	If this folder does not exist, create it first.
1. Reopen the Arduino IDE - you should now be able to open the sketch in the **File -> Sketchbook** menu.
