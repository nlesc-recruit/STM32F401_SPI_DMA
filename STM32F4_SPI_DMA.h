/* Custom SPI library
 * This library inherits from the STM32 version and adds DMA support for STM32 F401.
 * Additional DMA transfer functions are added. After each transfer DMA is disabled, such
 * that the normal transfer functions still work as well
 */
#ifndef STM32F4_SPI_DMA_H
#define STM32F4_SPI_DMA_H

#include <SPI.h>


class SPIDMAClass: public SPIClass {
  public:
    using SPIClass::SPIClass;

    void begin(uint8_t _pin = CS_PIN_CONTROLLED_BY_USER);
    void end();

    byte DMAtransfer (uint8_t pin, uint8_t _data, SPITransferMode _mode = SPI_LAST);
    uint16_t DMAtransfer16(uint8_t pin, uint16_t _data, SPITransferMode _mode = SPI_LAST);
    void DMAtransfer(uint8_t pin, void *_buf, size_t _count, SPITransferMode _mode = SPI_LAST);
    // the byte type is not a typo, but matches the STM32 SPI library.
    void DMAtransfer(byte pin, void *_bufout, void *_bufin, size_t _count, SPITransferMode _mode = SPI_LAST);

    byte DMAtransfer (uint8_t _data, SPITransferMode _mode = SPI_LAST) {
      return DMAtransfer(CS_PIN_CONTROLLED_BY_USER, _data, _mode);
    }
    uint16_t DMAtransfer16(uint16_t _data, SPITransferMode _mode = SPI_LAST) {
      return DMAtransfer16(CS_PIN_CONTROLLED_BY_USER, _data, _mode);
    }
    void DMAtransfer(void *_buf, size_t _count, SPITransferMode _mode = SPI_LAST) {
      return DMAtransfer(CS_PIN_CONTROLLED_BY_USER, _buf, _count, _mode);
    }
    void DMAtransfer(void *_bufout, void *_bufin, size_t _count, SPITransferMode _mode = SPI_LAST) {
      return DMAtransfer(CS_PIN_CONTROLLED_BY_USER, _bufout, _bufin, _count, _mode);
    }

  private:
    DMA_TypeDef* _dma;
    uint32_t _txstream;
    uint32_t _txchannel;
    uint32_t (*LL_DMATX_IsActiveFlag_TC) (DMA_TypeDef* DMAx);
    void (*LL_DMATX_ClearFlag_TC) (DMA_TypeDef* DMAx);
};
#endif  // STM32F4_SPI_DMA_H
