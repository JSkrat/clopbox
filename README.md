# clopbox
multiple power pwm outputs controlled with uart with runtime configurable waveform generators

## Windows UART setup for speed

Device manager -> usb serial port -> properties -> port settings -> advanced -> BM options -> Latency timer -> set to minimum 1ms

This will reduce latency between actual response on uart and waitForReadyRead call in serialthread.cpp so minimal request period 
is about 8ms.

