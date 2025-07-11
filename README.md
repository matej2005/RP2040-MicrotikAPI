
# RP2040-MicrotikAPI
This project is used to enable / disable firewall rule using RouterOS rest API

Project is based on RP2040-zero dev board and W5500 ethernet module

## Building it
				<schematic>

## Enclousure
				I 3D printed rackmount case for this project
				<img>

## RouterBoard configuration

        1. Login in to your router using WinBox
        2. Optional: Create Group API in `system/groups` with policy `rest-api`
        3. In system/users create user for accessing via API with api group created before
        4. Optional reserve DHCP address for this device and add it under Allowed adresses
        5. Make sure that in IP/Services WWW is enabled
	

## Firmware

- open MicrotikAPI/MicrotikAPI.ino in arduino ide
- install Raspberry Pi Pico by earlephilhower using board manager
- set target board as Waveshare RP2040 Zero
- install libraries
- in file config.h
	- set server[] to RouterBoard IP
	- set AUTH_USER to user created in step 2.3
	- set AUTH_PASSWD to user password in step 2.3
	- set targetVariableValue to name of targeted rule
- Compile and upload


## V2
- use W5500-EVB-PICO
- web interface

