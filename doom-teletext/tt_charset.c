#include "tt_charset.h"
#include "tt_encoding.h"

//returns a  byte that describes a teletext cell with 0 to 6 pixels set
uint8_t GetTeletextEncodingMosaic(bool pixelTopLeft,
                                  bool pixelTopRight,
                                  bool pixelCenterLeft,
                                  bool pixelCenterRight,
                                  bool pixelBottomLeft,
                                  bool pixelBottomRight)
{
    // from https://galax.xyz/TELETEXT/CHARSET.HTM :
    // Empty mosaic is 0x20
    // Add these values for each pixel that is set:
    //
    //   HEX
    // -------
    // | 1| 2|
    // |--|--|
    // | 4| 8|
    // |--|--|
    // |10|40|
    // -------
    
    char value = 0x20;
    
    value += pixelTopLeft     ? 0x01 : 0;
    value += pixelTopRight    ? 0x02 : 0;
    value += pixelCenterLeft  ? 0x04 : 0;
    value += pixelCenterRight ? 0x08 : 0;
    value += pixelBottomLeft  ? 0x10 : 0;
    value += pixelBottomRight ? 0x40 : 0;
    
    return Parity(value);
}

//returns a  byte that describes a teletext cell with 0 to 6 pixels set
//usage: GetTeletextEncodingMosaicByBitMask(0b100010)
//       for a cell with top left and bottom left pixel set.
//       -------
//       ||||  |
//       |     |
//       ||||  |
//       -------
uint8_t GetTeletextEncodingMosaicByBitMask(uint8_t mosaicMask)
{  
    return GetTeletextEncodingMosaic(
        mosaicMask & 0b00000001,
        mosaicMask & 0b00000010,
        mosaicMask & 0b00000100,
        mosaicMask & 0b00001000,
        mosaicMask & 0b00010000,
        mosaicMask & 0b00100000);
}
