#ifndef _RLE_DECOMPRESSION_H_
#define _RLE_DECOMPRESSION_H_

  const uint8_t RLE_COMPRESSED_DATA = 0x80;
  const uint8_t RLE_COMPRESSED_0xFF = 0x40;
  const uint8_t RLE_COMPRESSED_0x00 = 0x20;

#ifdef _RLE_MIRROR_SUPPORT_
  uint8_t *pgm_RLEdecompress( uint8_t *compressedData,
                              uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                              bool     mirrorFlag );
#else
  uint8_t *pgm_RLEdecompress( uint8_t *compressedData,
                              uint8_t *uncompressedData, uint16_t uncompressedByteCount );
#endif

#endif
