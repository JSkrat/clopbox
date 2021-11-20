# clopbox
multiple power pwm outputs controlled with uart with runtime configurable waveform generators

## AVR fuses

By default only 3 outputs are working, so it is necessary to 

## Windows UART setup for speed

Device manager -> usb serial port -> properties -> port settings -> advanced -> BM options -> Latency timer -> set to minimum 1ms

This will reduce latency between actual response on uart and waitForReadyRead call in serialthread.cpp so minimal request period 
is about 8ms.

## The program can't start because Qt5Core.dll is missing from your computer.

Edit the system environment variables -> Environment Variables -> edit Path -> add `C:\Qt\5.12.5\mingw73_64\bin`
