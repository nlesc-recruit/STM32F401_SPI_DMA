# STM32F4 SPI with DMA
Extension of the [STM32 Arduino core](https://github.com/stm32duino/Arduino_Core_STM32) SPI library with DMA support for F4 devices, such as the Black Pill. Tested with version 2.3.0 of the Arduino core.


### Usage
In place of the `transfer` or `transfer16` method of the SPI class, use `DMAtransfer` or `DMAtransfer16`.
The DMA controller is set up before each transfer and disconnected from the SPI interface afterwards, so
DMA and non-DMA transfers can be mixed in the same application.

#### Example:
```
#include <STM32F4_SPI_DMA.h>

// Create a class that can be used in place of SPIClass.
// The mosi/miso/sclk pins must match one of the hardware SPI instances.
SPIDMAClass _spi(mosi, miso, sclk);

// Transfer some data
uint8_t data;
_spi.DMAtransfer(data);

uint16_t value;
_spi.DMAtransfer16(value);

// Transfer a larger buffer of data
uint8_t buffer[100];
_spi.DMAtransfer(buffer, sizeof(buffer));
```


### Current limitations
- Transfer only, use the non-DMA methods if receive is needed.
- Only tested with a Black Pill, STM32F401CC.

### License
This worked is licensed under the Apache License, version 2.0.
