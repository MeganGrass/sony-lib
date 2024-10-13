/*
*
*	CREDIT:
*
*		https://github.com/XProger/OpenResident
*		https://psx-spx.consoledev.net/macroblockdecodermdec/
*		https://github.com/grumpycoders/pcsx-redux/
*       http://jpsxdec.blogspot.com/2011/06/decoding-mpeg-like-bitstreams.html
*
*/


#pragma once

#include <cstdint>

#include <cstring>


class Sony_PlayStation_Bitstream {
private:

    void mdec_IDCT(int* block, int used_col);

    static inline void putQuadRGB24(std::uint8_t* image, int* Yblk, int Cr, int Cb);

    inline void YUV2RGB24(std::int32_t* blk, std::uint8_t* image);

public:

    /*
        Construction
    */
    explicit Sony_PlayStation_Bitstream(void) {}
    virtual ~Sony_PlayStation_Bitstream(void) {}

    std::uint64_t mdec_decode(std::uint8_t* data, std::int32_t version, std::int32_t width, std::int32_t height, std::int32_t qscale, std::uint8_t* dst);

};