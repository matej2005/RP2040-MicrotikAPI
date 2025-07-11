
# RP2040-MicrotikAPI
This project is used to enable / disable firewall rule using RouterOS rest API

Project is based on RP2040-zero dev board and W5500 ethernet module

Powering via USB-C

## Building it
Here is schematic how to connect ethernet module with RP2040
Sometimes it started happening that ethernet module can´t boot, it was happening most times with long USB cable. I fixed it by adding cappacitor to the 3.3V power line.
![Schematic](./img/Schematic.png)

I builded it on pice of protoboard
![protoboard top](./img/bot.jpg)
![protoboard bottom](./img/top.jpg)

## Enclousure
I 3D printed rackmount case for this project
![Enclousure](./img/case.jpg)

## RouterBoard configuration
**Supported versions > RouterOS v7.1beta4**
 - Login in to your router using WinBox
 - Optional: Create Group API in `system/groups` with policy `write, rest-api, read, api`
 - In system/users create user for accessing via API with api group created before
 - Optional reserve DHCP address for this device and add it under Allowed addresses
 - Make sure that in IP/Services WWW is enabled
 - Create firewall rule with some specific comment that we will be targeting ex:"GPIO controled port" 
	
Terminal
 - user/group/add name=test policy=read,write,api,rest-api comment="Group for rest-api"
 - user/add name=api password=1234 group=api comment="User for rest-api"
 - ip/service/enable www
 - ip/firewall/nat/add comment="GPIO controled port" ...

## Firmware

 - open MicrotikAPI/MicrotikAPI.ino in arduino ide
 - install Raspberry Pi Pico by earlephilhower using board manager
 - set target board as Waveshare RP2040 Zero
 - install libraries
 - in file config.h
	- set `server[]` to RouterBoard IP
	- set `AUTH_USER` to user created in step 2.3
	- set `AUTH_PASSWD` to user password in step 2.3
	- set `targetVariableValue` to name of targeted rule
 - Power On the device or push RESET while holding BOOT button
 - Compile and upload


## V2 ideas
- use W5500-EVB-PICO
- web interface
- more gpio
- OTA
- stackable
- more apis

