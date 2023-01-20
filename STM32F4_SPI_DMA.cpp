#include <STM32F4_SPI_DMA.h>

#include <stm32f4xx_ll_dma.h>
#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_bus.h>

void SPIDMAClass::begin(uint8_t _pin) {
  SPIClass::begin(_pin);

  // Setup DMA
  // first find out which SPI is used and set the DMA-related values accordingly
  SPI_TypeDef* _spi = getHandle()->Instance;
  if (_spi == SPI1) {
    _dma = DMA2;
    _txstream = LL_DMA_STREAM_3;
    _txchannel = LL_DMA_CHANNEL_3;
    LL_DMATX_IsActiveFlag_TC = LL_DMA_IsActiveFlag_TC3;
    LL_DMATX_ClearFlag_TC = LL_DMA_ClearFlag_TC3;
  } else if (_spi == SPI2) {
    _dma = DMA1;
    _txstream = LL_DMA_STREAM_4;
    _txchannel = LL_DMA_CHANNEL_0;
    LL_DMATX_IsActiveFlag_TC = LL_DMA_IsActiveFlag_TC4;
    LL_DMATX_ClearFlag_TC = LL_DMA_ClearFlag_TC4;
  } else if (_spi == SPI3) {
    _dma = DMA1;
    _txstream = LL_DMA_STREAM_5;
    _txchannel = LL_DMA_CHANNEL_0;
    LL_DMATX_IsActiveFlag_TC = LL_DMA_IsActiveFlag_TC5;
    LL_DMATX_ClearFlag_TC = LL_DMA_ClearFlag_TC5;
  } else {
    // software SPI, cannot use DMA
    _dma = NULL;
  }

  // DMA setup. Nothing to do if using software SPI
  if (_dma == NULL) {
    return;
  }

  // ensure SPI and DMA are disabled
  LL_DMA_DisableStream(_dma, _txstream);
  while (LL_DMA_IsEnabledStream(_dma, _txstream));
  LL_DMA_DeInit(_dma, _txstream);

  // enable DMA clock
  if (_dma == DMA1) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  } else if (_dma == DMA2) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
  }

  // DMA config
  LL_DMA_InitTypeDef DMATXConfig;
  LL_DMA_StructInit(&DMATXConfig);

  DMATXConfig.PeriphOrM2MSrcAddress =  LL_SPI_DMA_GetRegAddr(_spi);
  DMATXConfig.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
  DMATXConfig.Mode = LL_DMA_MODE_NORMAL;
  DMATXConfig.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
  DMATXConfig.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
  DMATXConfig.PeriphOrM2MSrcDataSize = LL_SPI_GetDataWidth(_spi) == LL_SPI_DATAWIDTH_8BIT ? LL_DMA_PDATAALIGN_BYTE : LL_DMA_PDATAALIGN_HALFWORD;
  DMATXConfig.MemoryOrM2MDstDataSize = LL_SPI_GetDataWidth(_spi) == LL_SPI_DATAWIDTH_8BIT ? LL_DMA_PDATAALIGN_BYTE : LL_DMA_PDATAALIGN_HALFWORD;
  DMATXConfig.Channel = _txchannel;

  LL_DMA_Init(_dma, _txstream, &DMATXConfig);
}

void SPIDMAClass::end() {
  // Disable DMA
  LL_DMA_DisableStream(_dma, _txstream);
  while (LL_DMA_IsEnabledStream(_dma, _txstream));
  LL_DMA_DeInit(_dma, _txstream);

  // Disable SPI through parent class
  SPIClass::end();
}

byte SPIDMAClass::DMAtransfer(uint8_t pin, uint8_t _data, SPITransferMode _mode) {
  DMAtransfer(pin, &_data, sizeof(uint8_t), _mode);
  // The returned value should be the received data, but we do not yet support receive
  return 0;
}

uint16_t SPIDMAClass::DMAtransfer16(uint8_t pin, uint16_t _data, SPITransferMode _mode) {
  DMAtransfer(pin, &_data, sizeof(uint16_t), _mode);
  // The returned value should be the received data, but we do not yet support receive
  return 0;
}

void SPIDMAClass::DMAtransfer(uint8_t pin, void *_buf, size_t _count, SPITransferMode _mode) {
  LL_DMA_DisableStream(_dma, _txstream);
  while (LL_DMA_IsEnabledStream(_dma, _txstream));
  LL_DMA_SetMemoryAddress(_dma, _txstream, (uint32_t) _buf);
  LL_DMA_SetDataLength(_dma, _txstream, _count);

  LL_SPI_EnableDMAReq_TX(getHandle()->Instance);

  if (pin != CS_PIN_CONTROLLED_BY_USER) {
    digitalWrite(pin, LOW);
  }

  LL_DMA_EnableStream(_dma, _txstream);
  while(!LL_DMATX_IsActiveFlag_TC(_dma));
  LL_DMATX_ClearFlag_TC(_dma);
  while (LL_SPI_IsActiveFlag_BSY(_spi));

  if (pin != CS_PIN_CONTROLLED_BY_USER && _mode == SPI_LAST) {
    digitalWrite(pin, HIGH);
  }

  LL_DMA_DisableStream(_dma, _txstream);
  while (LL_DMA_IsEnabledStream(_dma, _txstream));
  LL_SPI_DisableDMAReq_TX(getHandle()->Instance);
}

void SPIDMAClass::DMAtransfer(byte _pin, void *_bufout, void *_bufin, size_t _count, SPITransferMode _mode) {
  // We do not yet support receiving data, ignore _bufin for now
  DMAtransfer(_pin, _bufout, _count, _mode);
}
