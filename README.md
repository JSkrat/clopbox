# clopbox
multiple power pwm outputs controlled with uart

# Windows UART setup for speed

Device manager -> usb serial port -> properties -> port settings -> advanced -> BM options -> Latency timer -> set to minimum 1ms

This will reduce latency between actual response on uart and waitForReadyRead call in serialthread.cpp