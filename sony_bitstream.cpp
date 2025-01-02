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


#include "sony_bitstream.h"

#include "sony_bitstream_table.h"


#define AAN_CONST_BITS      12
#define AAN_EXTRA           12
#define SCALE(x, n)         ((x) >> (n))
#define SCALER(x, n)        (((x) + ((1 << (n)) >> 1)) >> (n))
#define SCALE8(c)           SCALER(c, 20)
#define FIX_1_082392200     SCALER(18159528, AAN_CONST_BITS)
#define FIX_1_414213562     SCALER(23726566, AAN_CONST_BITS)
#define FIX_1_847759065     SCALER(31000253, AAN_CONST_BITS)
#define FIX_2_613125930     SCALER(43840978, AAN_CONST_BITS)
#define MULR(a)             ((1434 * (a)))
#define MULB(a)             ((1807 * (a)))
#define MULG2(a, b)         ((-351 * (a)-728 * (b)))
#define MULY(a)             ((a) << 10)
#define CLAMP8(c)           (((c) < -128) ? 0 : (((c) > (255 - 128)) ? 255 : ((c) + 128)))
#define CLAMP_SCALE8(a)     CLAMP8(SCALE8(a))


struct BitStream
{
    uint16_t* data;
    uint32_t index;
    uint32_t value;

    BitStream(uint8_t* data) :
        data((uint16_t*)data),
        index(0),
        value(0)
    {
    }

    uint32_t getBit()
    {
        if (!index--)
        {
            value = *data++; // TODO BE support
            index = 15;
        }

        return (value >> index) & 1;
    }

    uint32_t getU(int32_t count)
    {
        uint32_t bits = 0;

        while (count--)
        {
            bits = (bits << 1) | getBit();
        }

        return bits;
    }

    int32_t getS(int32_t count)
    {
        if (getBit())
        {
            return getU(count - 1) + (1 << (count - 1));
        }
        else
        {
            return getU(count - 1) - (1 << count) + 1;
        }
    }

    void skip(int32_t count)
    {
        getU(count);
    }

    // http://jpsxdec.blogspot.com/2011/06/decoding-mpeg-like-bitstreams.html
    bool readCode(int32_t& skipCount, int32_t& ac)
    {
        if (getBit())
        {
            if (getBit())
            {
                skipCount = 0;
                ac = getBit() ? -1 : 1;
                return true;
            }
            return false; // end of block
        }

        int32_t nz = 1;
        while (!getBit())
        {
            nz++;
        }

        if (nz == 5) // escape code == 0b1000001
        {
            uint32_t esc = getU(16);
            skipCount = esc >> 10;
            ac = esc & 0x3FF;
            if (ac & 0x200)
                ac -= 0x400;
            return true;
        }

        const uint8_t* table;
        int32_t shift;
        if (nz < 6)
        {
            table = AC_LUT_1;
            shift = 1;
        }
        else if (nz < 9)
        {
            table = AC_LUT_6;
            shift = 6;
        }
        else
        {
            table = AC_LUT_9;
            shift = 9;
        }

        BitStream state = *this;
        uint32_t code = (1 << 7) | state.getU(7);

        code >>= nz - shift;

        // ASSERT(table);
		if (!table) return false;

        int32_t idx = table[code];

        // ASSERT(idx != 255);
		if (idx == 255) return false;

        const AC_ENTRY& e = MDEC_AC[idx];
        skip(e.length - nz - 1);
        skipCount = e.skip;
        ac = (code & (1 << (8 + shift - e.length))) ? -e.ac : e.ac;
        return true;
    }
};


void Sony_PlayStation_Bitstream::mdec_IDCT(int* block, int used_col)
{
    int* ptr;

    if (used_col == -1)
    {
        int32_t v = block[0];
        for (int32_t i = 0; i < 64; i++)
        {
            block[i] = v;
        }
        return;
    }

    ptr = block;
    for (int32_t i = 0; i < 8; i++, ptr++)
    {
        if ((used_col & (1 << i)) == 0)
        {
            if (ptr[8 * 0])
            {
                ptr[0 * 8] =
                    ptr[1 * 8] =
                    ptr[2 * 8] =
                    ptr[3 * 8] =
                    ptr[4 * 8] =
                    ptr[5 * 8] =
                    ptr[6 * 8] =
                    ptr[7 * 8] = ptr[0];
                used_col |= (1 << i);
            }
            continue;
        }

        int32_t z10 = ptr[8 * 0] + ptr[8 * 4];
        int32_t z11 = ptr[8 * 0] - ptr[8 * 4];
        int32_t z13 = ptr[8 * 2] + ptr[8 * 6];
        int32_t z12 = SCALE((ptr[8 * 2] - ptr[8 * 6]) * FIX_1_414213562, AAN_CONST_BITS) - z13;

        int32_t tmp0 = z10 + z13;
        int32_t tmp3 = z10 - z13;
        int32_t tmp1 = z11 + z12;
        int32_t tmp2 = z11 - z12;

        z13 = ptr[8 * 3] + ptr[8 * 5];
        z10 = ptr[8 * 3] - ptr[8 * 5];
        z11 = ptr[8 * 1] + ptr[8 * 7];
        z12 = ptr[8 * 1] - ptr[8 * 7];

        int32_t tmp7 = z11 + z13;

        int32_t z5 = (z12 - z10) * FIX_1_847759065;
        int32_t tmp6 = SCALE(z10 * FIX_2_613125930 + z5, AAN_CONST_BITS) - tmp7;
        int32_t tmp5 = SCALE((z11 - z13) * FIX_1_414213562, AAN_CONST_BITS) - tmp6;
        int32_t tmp4 = SCALE(z12 * FIX_1_082392200 - z5, AAN_CONST_BITS) + tmp5;

        ptr[8 * 0] = (tmp0 + tmp7);
        ptr[8 * 7] = (tmp0 - tmp7);
        ptr[8 * 1] = (tmp1 + tmp6);
        ptr[8 * 6] = (tmp1 - tmp6);
        ptr[8 * 2] = (tmp2 + tmp5);
        ptr[8 * 5] = (tmp2 - tmp5);
        ptr[8 * 4] = (tmp3 + tmp4);
        ptr[8 * 3] = (tmp3 - tmp4);
    }

    ptr = block;
    if (used_col == 1)
    {
        for (int32_t i = 0; i < 8; i++)
        {
            ptr[i * 8 + 0] =
                ptr[i * 8 + 1] =
                ptr[i * 8 + 2] =
                ptr[i * 8 + 3] =
                ptr[i * 8 + 4] =
                ptr[i * 8 + 5] =
                ptr[i * 8 + 6] =
                ptr[i * 8 + 7] = ptr[8 * i];
        }
    }
    else
    {
        for (int32_t i = 0; i < 8; i++, ptr += 8)
        {
            int32_t z10 = ptr[0] + ptr[4];
            int32_t z11 = ptr[0] - ptr[4];
            int32_t z13 = ptr[2] + ptr[6];
            int32_t z12 = SCALE((ptr[2] - ptr[6]) * FIX_1_414213562, AAN_CONST_BITS) - z13;

            int32_t tmp0 = z10 + z13;
            int32_t tmp3 = z10 - z13;
            int32_t tmp1 = z11 + z12;
            int32_t tmp2 = z11 - z12;

            z13 = ptr[3] + ptr[5];
            z10 = ptr[3] - ptr[5];
            z11 = ptr[1] + ptr[7];
            z12 = ptr[1] - ptr[7];

            int32_t z5 = (z12 - z10) * FIX_1_847759065;
            int32_t tmp7 = z11 + z13;
            int32_t tmp6 = SCALE(z10 * FIX_2_613125930 + z5, AAN_CONST_BITS) - tmp7;
            int32_t tmp5 = SCALE((z11 - z13) * FIX_1_414213562, AAN_CONST_BITS) - tmp6;
            int32_t tmp4 = SCALE(z12 * FIX_1_082392200 - z5, AAN_CONST_BITS) + tmp5;

            ptr[0] = tmp0 + tmp7;

            ptr[7] = tmp0 - tmp7;
            ptr[1] = tmp1 + tmp6;
            ptr[6] = tmp1 - tmp6;
            ptr[2] = tmp2 + tmp5;
            ptr[5] = tmp2 - tmp5;
            ptr[4] = tmp3 + tmp4;
            ptr[3] = tmp3 - tmp4;
        }
    }
}


inline void Sony_PlayStation_Bitstream::putQuadRGB24(uint8_t* image, int* Yblk, int Cr, int Cb)
{
    int Y, R, G, B;

    R = MULR(Cr);
    G = MULG2(Cb, Cr);
    B = MULB(Cb);

    Y = MULY(Yblk[0]);
    image[0 * 3 + 0] = CLAMP_SCALE8(Y + R);
    image[0 * 3 + 1] = CLAMP_SCALE8(Y + G);
    image[0 * 3 + 2] = CLAMP_SCALE8(Y + B);
    Y = MULY(Yblk[1]);
    image[1 * 3 + 0] = CLAMP_SCALE8(Y + R);
    image[1 * 3 + 1] = CLAMP_SCALE8(Y + G);
    image[1 * 3 + 2] = CLAMP_SCALE8(Y + B);
    Y = MULY(Yblk[8]);
    image[16 * 3 + 0] = CLAMP_SCALE8(Y + R);
    image[16 * 3 + 1] = CLAMP_SCALE8(Y + G);
    image[16 * 3 + 2] = CLAMP_SCALE8(Y + B);
    Y = MULY(Yblk[9]);
    image[17 * 3 + 0] = CLAMP_SCALE8(Y + R);
    image[17 * 3 + 1] = CLAMP_SCALE8(Y + G);
    image[17 * 3 + 2] = CLAMP_SCALE8(Y + B);
}


inline void Sony_PlayStation_Bitstream::YUV2RGB24(int32_t* blk, uint8_t* image)
{
    int32_t* Yblk = blk + 64 * 2;
    int32_t* Crblk = blk;
    int32_t* Cbblk = blk + 64;

    for (int32_t y = 0; y < 16; y += 2, Crblk += 4, Cbblk += 4, Yblk += 8, image += 8 * 3 * 3)
    {
        if (y == 8)
        {
            Yblk += 64;
        }

        for (int32_t x = 0; x < 4; x++, image += 6, Crblk++, Cbblk++, Yblk += 2)
        {
            putQuadRGB24(image, Yblk, *Crblk, *Cbblk);
            putQuadRGB24(image + 8 * 3, Yblk + 64, *(Crblk + 4), *(Cbblk + 4));
        }
    }
}


std::uint64_t Sony_PlayStation_Bitstream::mdec_decode(std::uint8_t* data, std::int32_t version, std::int32_t width, std::int32_t height, std::int32_t qscale, std::uint8_t* dst)
{
    BitStream bs(data);

    std::int32_t prev[3] = { 0, 0, 0 };
    std::int32_t blocks[6][8 * 8]{}; // Cr, Cb, YTL, YTR, YBL, YBR

	std::int32_t dataSize = width * height * 4;

    for (std::int32_t bX = 0; bX < width / 16; bX++)
    {
        for (std::int32_t bY = 0; bY < height / 16; bY++)
        {
            std::memset(blocks, 0, sizeof(blocks));

            for (int i = 0; i < 6; i++)
            {
                std::int32_t* block = blocks[i];

                std::int32_t dc;

                if (version == 2) // fixed 10-bit DC
                {
                    dc = bs.getU(10);
                    if (dc >> 9)
                    {
                        dc -= 1024;
                    }
                }
                else // variable DC bits
                {
                    std::int32_t nz = 0;
                    while (bs.getBit())
                        nz++;

                    if (i >= 2) // Luma
                    {
                        if (nz == 0)
                        {
                            if (bs.getBit())
                            {
                                dc = bs.getS(2);
                            }
                            else
                            {
                                dc = (bs.getBit() << 1) - 1;
                            }
                        }
                        else if (nz == 1)
                        {
                            dc = bs.getBit() ? bs.getS(3) : 0;
                        }
                        else
                        {
                            dc = bs.getS(nz + 2);
                        }

                        nz = 2;
                    }
                    else // Chroma
                    {
                        if (nz == 0)
                        {
                            if (bs.getBit())
                            {
                                dc = (bs.getBit() << 1) - 1;
                            }
                            else
                            {
                                dc = 0;
                            }
                        }
                        else
                        {
                            dc = bs.getS(nz + 1);
                        }

                        nz = i;
                    }

                    dc <<= 2;
                    dc += prev[nz];
                    prev[nz] = dc;
                    //ASSERT(prev[nz] >= -512 && prev[nz] <= 511);
					if (prev[nz] < -512 || prev[nz] > 511) return 0;
                }

                block[0] = SCALER(dc * MDEC_QTABLE[0], AAN_EXTRA - 3);

                std::int32_t used_col = 0;

                std::int32_t skip, ac;
                std::int32_t index = 0;
                while (bs.readCode(skip, ac))
                {
                    index += skip + 1;
                    //ASSERT(index < 64);
					if (index >= 64) break;

                    block[MDEC_ZSCAN[index]] = SCALER(ac * MDEC_QTABLE[index] * qscale, AAN_EXTRA);

                    used_col |= (MDEC_ZSCAN[index] > 7) ? 1 << (MDEC_ZSCAN[index] & 7) : 0;
                }

                if (index == 0) used_col = -1;

                mdec_IDCT(block, used_col);
            }

            std::uint8_t rgb[16 * 16 * 3]{};
            YUV2RGB24(&blocks[0][0], (std::uint8_t*)rgb);

            std::uint8_t* src = rgb;
            std::uint8_t* ptr = dst + (width * bY * 16 + bX * 16) * 4;
            for (int y = 0; y < 16; y++)
            {
                for (int x = 0; x < 16; x++)
                {
					if (ptr + 4 > dst + dataSize) break;
                    *ptr++ = *src++;
                    *ptr++ = *src++;
                    *ptr++ = *src++;
                    *ptr++ = 255;
                }
                ptr += (width - 16) * 4;
            }
        }
    }

    return (uint8_t*)bs.data - data;
}