# R4K-Oil

LoRaWAN Oil tank level sensor

## Bill of Materials

[RAK19003](https://store.rakwireless.com/products/wisblock-base-board-rak19003)

[RAK4631](https://store.rakwireless.com/products/rak4631-lpwan-node)

[RAK1901](https://store.rakwireless.com/products/rak1901-shtc3-temperature-humidity-sensor)

[RCWL-1601 Ultrasonic Sensor](https://www.adafruit.com/product/4007)

[Pololu Mini Pushbutton Power Switch with Reverse Voltage Protection, LV](https://www.pololu.com/product/2808)

## No-Code/Easy setup

If you don't want to make any custom changes to the firmware, follow these very easy instructions to flash your device with the R4K firmware.
- Download the latest release from: https://github.com/r4wk/R4K-Oil/releases
- Download this flash utility: https://github.com/adafruit/Adafruit_nRF52_nrfutil/releases
- Extract the flash utility and copy the latest R4K firmware to the same folder to make life easier
- Next you want to run the following command (In Windows open CMD app, navigate to the folder where you extracted the flash utility/firmware i.e. 'cd c:\adafruit')
  - adafruit-nrfutil dfu serial --package R4K-Oil_\<version\>.zip -p \<port\> -b 115200
- Make sure to change \<version\> to the latest version of the R4K firmware that you downloaded from releases (i.e. R4K-Oil_v0.2.zip)
- Make sure to change \<port\> to the COM port that your device is connected on: https://www.mathworks.com/help/supportpkg/arduinoio/ug/find-arduino-port-on-windows-mac-and-linux.html

## Code/PlatformIO

If you'd like to make changes to the base firmware (not needed). You can follow this guide:

- https://github.com/rakstars/WisBlock-RAK4631-Helium-Mapper/wiki/Make-a-Helium-Mapper-with-the-WisBlock#from-platformio

## Set up LoRa credentials/settings
- I highly advise using WisBlock-ToolBox app, this allows you do connect your device to Helium right from your phone via Bluetooth (Android Only)
  - https://play.google.com/store/apps/details?id=tk.giesecke.wisblock_toolbox&hl=en&gl=US
- Once you have that installed follow these instructions to get you connected to Helium LoRaWAN network
  - https://github.com/rakstars/WisBlock-RAK4631-Helium-Mapper/wiki/Make-a-Helium-Mapper-with-the-WisBlock#using-the-wisblock-toolbox-lpwan-setup-module
  
- If you don't want to use a phone or only have an iOS device, you can still setup the device with AT commands
- Over Serial/USB to run AT commands
  - https://github.com/rakstars/WisBlock-RAK4631-Helium-Mapper/wiki/Make-a-Helium-Mapper-with-the-WisBlock#setting-up-the-lorawan-parameters
- Over Bluetooth to run AT commands
  - https://github.com/rakstars/WisBlock-RAK4631-Helium-Mapper/wiki/Make-a-Helium-Mapper-with-the-WisBlock#over-ble
