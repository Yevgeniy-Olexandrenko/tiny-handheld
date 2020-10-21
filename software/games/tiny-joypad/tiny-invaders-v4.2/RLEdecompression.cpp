
#include <Arduino.h>
#include "RLEdecompression.h"

/*--------------------------------------------------------------*/
// Input:  compressedData        - pointer to compressed data in PROGMEM
//         uncompressedDatar     - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
//         mirrorFlag            - mirror the data while uncompressing
#ifdef _RLE_MIRROR_SUPPORT_
uint8_t *pgm_RLEdecompress( uint8_t *compressedData,
                            uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                            bool     mirrorFlag )
#else
uint8_t *pgm_RLEdecompress( uint8_t *compressedData,
                            uint8_t *uncompressedData, uint16_t uncompressedByteCount )
#endif
{
  #ifdef _RLE_MIRROR_SUPPORT_
    // mirror data?
    if ( mirrorFlag )
    {
      // set output pointer behind the end of the buffer
      uncompressedData += uncompressedByteCount;
    }
  #endif
  
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = pgm_read_byte( compressedData++ );

    if ( count & RLE_COMPRESSED_DATA  )
    {
      // prepare special value (just in case)
      uint8_t value = (count & RLE_COMPRESSED_0xFF ) ? 0xff : 0x00;
      // special value?
      if ( !( count & ( RLE_COMPRESSED_0xFF | RLE_COMPRESSED_0x00 ) ) )
      {
        // get stored value
        value = pgm_read_byte( compressedData++ );
      }
      
      // remove special bit
      count &= 0x1f;
      // add one item again
      count++;
      
      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
      #ifdef _RLE_MIRROR_SUPPORT_
        if ( mirrorFlag )
        {
          *--uncompressedData = value;
        }
        else
      #endif
        {
          *uncompressedData++ = value;
        }
      }
    }
    else
    {
      // add one item again
      count++;
      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        uint8_t value = pgm_read_byte( compressedData++ );
      #ifdef _RLE_MIRROR_SUPPORT_
        if ( mirrorFlag )
        {
          *--uncompressedData = value;
        }
        else
      #endif
        {
          *uncompressedData++ = value;
        }
      }
    }
    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}
