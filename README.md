Maixduino
========

# Arduino Core for K210 based devices



## Supported devices

### Sipeed Maix series
  - [Amigo](https://dl.sipeed.com/MAIX/HDK/Sipeed-Amigo)
  - [Cube](https://dl.sipeed.com/MAIX/HDK/Sipeed-Maix-Cube)
  - [Go](https://dl.sipeed.com/MAIX/HDK/Sipeed-Maix-GO)
  - [Maixduino](https://dl.sipeed.com/MAIX/HDK/Sipeed-Maixduino)
  - [Dock](https://dl.sipeed.com/MAIX/HDK/Sipeed-Maix-Dock)
  - [BiT](https://dl.sipeed.com/MAIX/HDK/Sipeed-Maix-Bit)
  - [Nano](https://dl.sipeed.com/MAIX/HDK/Sipeed-Maix-Nano)

### M5Stack
 * [M5StickV](https://m5stack.com/products/stickv)
 * [M5UnitV](https://m5stack.com/collections/m5-unit/products/unitv-ai-camera)

### Various custom boards
 * [LAMLOEI AOIT DAAN](https://github.com/lamloei/AIoTDaaN/tree/master/hardware/20190505-R2/AIoTDaaN_R2/pdf)
 * [IOXGD4](https://github.com/ioxgd/IOXGD-hardware/tree/master/IOXGD4/pdf)

## Docs

Docs: [wiki.sipeed.com](https://wiki.sipeed.com/soft/maixduino/zh/index.html)

## Install

Refer install doc: [Install](https://maixduino.sipeed.com/en/get_started/install.html)

## Installing

### From Board Manager

 1. [Download and install the Arduino IDE](https://www.arduino.cc/en/Main/Software) (at least version v1.9.8)
 2. Start the Arduino IDE
 3. Go into Preferences
 4. Add ```https://dl.sipeed.com/MAIX/Maixduino/package_Maixduino_k210_index.json``` as an "Additional Board Manager URL"
 5. Open the Boards Manager from the Tools -> Board menu and install "Maixduino(K210)"
 6. Select your K210 board from the Tools -> Board menu

### From git

 1. Follow steps from Board Manager section above
 2. ```cd <SKETCHBOOK>```, where ```<SKETCHBOOK>``` is your Arduino Sketch folder:
  * OS X: ```~/Documents/Arduino```
  * Linux: ```~/Arduino```
  * Windows: ```~/Documents/Arduino```
 3. Create a folder named ```hardware```, if it does not exist, and change directories to it
 4. Clone this repo: ```git clone https://github.com/Sipeed/Maixduino Maixduino/k210```
 5. Restart the Arduino IDE

### Firmware flashing
The firmware of the K210 devices is updated with a Python-based [kflash](https://github.com/sipeed/kflash.py) cross-platform tool.
Follow ```kflash``` github page on installation instruction.


###### Change board settings in Tools section on the top of Arduino IDE

 1. Board: The same as your dev board
 2. Burn Toolfirmware: leave default, for Maix Go Kit - ```open-ec```
 3. Burn Baudrate: Decrease it if download fails
 4. Port: Serial port, e.g. ```/dev/ttyUSB0```
 5. Programmer: ```k-flash```

## Credits

This core is based on and compatible with the [Sipeed Maixduino Core](https://github.com/sipeed/Maixduino)





