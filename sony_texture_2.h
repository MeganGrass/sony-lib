/*
*
*	Megan Grass
*	February 24, 2025
*
*/


#pragma once

#include <std_basic_fstream.h>

#include <std_image.h>


#pragma pack(push, 1)


enum class Sony_Pixel_Mode : uint8_t
{
	RGBA32 = 0,							// 8:8:8:8
	RGBA24 = 1,							// 8:8:8:8
	RGBA16 = 2,							// 5:5:5:1
	RGBA16S = 10,						// 5:5:5:1
	RGB = 18,							// 8:8:8
	CLUT8 = 19,							// 8-bit Palette
	CLUT4 = 20,							// 4-bit Palette
	CLUT8H = 27,						// 8-bit Palette
	CLUT4HL = 36,						// 4-bit Palette
	CLUT4HH = 44,						// 4-bit Palette
	ZBUFF32 = 48,						// 8:8:8:8
	ZBUFF24 = 49,						// 8:8:8:8
	ZBUFF16 = 50,						// 5:5:5:1
	ZBUFF16S = 58,						// 5:5:5:1
	null = 0xFF							// unknown
};

struct Sony_Texture_2_Header
{
	int8_t Type[4];						// "TIM2" or "CLT2"

	uint8_t Version;					// file type version

	uint8_t Align;						/*
										 * data sector alignment
										 *	0 = 16 byte aligned data sections
										 *	1 = 128 byte aligned data sections
										*/

	uint16_t DataCount;					// texture count

	int64_t reserved : 64;				// zero padding
};

struct Sony_Texture_2_Data
{
	uint32_t Size;						// PaletteSize + PixelSize + HeaderSize

	uint32_t PaletteSize;				// total palette data size

	uint32_t PixelSize;					/*
										 * total pixel data size
										 *	- mipmap data is included
										*/

	uint16_t HeaderSize;				// size of this struct + size of mipmap header + size of extended data

	uint16_t ColorCount;				/*
										 * palette color amount
										 *	- multiple of 16 when CLUT4
										 *	- multiple of 256 when CLUT8
										*/

	uint8_t DataFormat;					// data format (always 0?)

	uint8_t MipMap;						/*
										 * mipmap level
										 *	0 = none (or palette only)
										 *	1-7 = Levels 0-6
										 *  - mipmap header immediately follows this header if Level 2+
										*/

	struct PALETTE
	{
		uint8_t PixelMode : 6;			/*
										* pixel format
										*	0 = no data
										*	1 = RGBA16
										*	2 = RGBA24
										*	3 = RGBA32
										*/

		uint8_t Compound : 1;			/*
										* compound flag
										*  - valid when Strorage Mode 2 and CLUT4
										*/

		uint8_t Align : 1;				/*
										* storage mode
										*	0 = Mode 1 (swizzled)
										*		8x2 grid when PixelMode is CLUT4
										*		16x16 grid when PixelMode is CLUT8
										*	1 = Mode 2 (linear)
										*		16x1 line when PixelMode is CLUT4
										*		256x1 line when PixelMode is CLUT8
										*/
	} PaletteAttr;

	uint8_t PixelMode;					/*
										 * pixel format
										 *	0 = no data
										 *	1 = RGBA16
										 *	2 = RGBA24
										 *	3 = RGBA32
										 *	4 = CLUT4
										 *	5 = CLUT8
										*/

	uint16_t Width;						/*
										 * texture width
										 *  - 1024 max
										 *	- multiple of 4 when CLUT4
										 *	- multiple of 2 when CLUT8
										*/

	uint16_t Height;					/*
										 * texture height
										 *  - 1024 max
										*/

	struct ATTRIBUTE
	{
		uint64_t PixelPtr : 14;			// pixel base pointer (/ 64)

		uint64_t TexelWidth : 6;		// texel width (/ 64)

		uint64_t PixelMode : 6;			// pixel format

		uint64_t Width : 4;				/*
										 * pixel width
										 *	- multiple of 4 when pixel mode is CLUT4
										 *	- multiple of 2 when pixel mode is CLUT8
										*/

		uint64_t Height : 4;			// pixel height

		uint64_t Comp : 1;				/*
										 * pixel color component
										 *	0 = RGB
										 *	1 = RGBA
										*/

		uint64_t Func : 2;				/*
										 * pixel function
										 *	0 = Modulate (fragment color, 0x80 max)
										 *	1 = Decal (as-is)
										 *	2 = Highlight (translucent)
										 *	3 = Highlight 2 (opaque)
										*/

		uint64_t PalettePtr : 14;		// palette base pointer (/ 64)

		uint64_t PalettePixelMode : 4;	/*
										 * palette pixel Type
										 *	RGBA32
										 *	RGBA24
										 *	RGBA16
										 *	RGBA16S
										*/

		uint64_t PaletteAlign : 1;		/*
										 * storage mode
										 *	0 = Mode 1 (swizzled)
										 *		8x2 grid when CLUT4
										 *		16x16 grid when CLUT8
										 *	1 = Mode 2 (linear)
										 *		16x1 line when CLUT4
										 *		256x1 line when CLUT8
										*/

		uint64_t PaletteBlockPtr : 5;	/*
										 * palette entry pointer (/ 16)
										 *	- if Mode 2, this value must be set
										*/

		uint64_t PaletteLoad : 3;		/*
										 * palette load type
										 *	0 = do not load
										 *	1 = Load
										 *	2 = Load and copy PalettePtr to CBP0 register
										 *	3 = Load and copy PalettePtr to CBP1 register
										 *	4 = Load and copy PalettePtr to CBP0 register when PalettePtr != CBP0 register
										 *	5 = Load and copy PalettePtr to CBP1 register when PalettePtr != CBP1 register
										*/
	} Attr;

	struct SAMPLING
	{
		uint64_t FuncLOD : 1;			/*
										 * LOD calculation
										 *	0 = LOD = ((log2(1 / Q) << Weight) + Position)
										 *	1 = LOD = K
										*/

		uint64_t reserved0 : 1;			// zero padding

		uint64_t MipLevel : 3;			// Level (0-6)

		uint64_t FilterMag : 1;			/*
										 * Filter (when LOD < 0)
										 *	0 = Nearest
										 *	1 = Linear
										*/

		uint64_t FilterMin : 3;			/*
										 * Filter (when LOD >= 0)
										 *	0 = Nearest
										 *	1 = Linear
										 *	2 = Nearest Mipmap Nearest
										 *	3 = Nearest Mipmap Linear
										 *	4 = Linear Mipmap Nearest
										 *	5 = Linear Mipmap Linear
										*/

		uint64_t MipPtrType : 1;		/*
										 * base address type (Level 2+)
										 *	0 = use MIPTBP1 and MIPTBP2
										 *	1 = automatic calculation of TBP1-TBP3
										*/

		uint64_t reserved1 : 9;			// zero padding

		uint64_t Weight : 2;			// weight

		uint64_t reserved2 : 11;		// zero padding

		uint64_t Position : 12;			// position

		uint64_t reserved3 : 20;		// zero padding
	} Sampling;

	struct ALPHA
	{
		uint32_t Value0 : 8;			/*
										 * global alpha [0]
										 *  - 0x80 max when RGBA24
										 *	- TA0 bit of TEXA register
										*/

		uint32_t reserved0 : 7;			// zero padding

		uint32_t Func : 1;				/*
										 * function
										 *	- AEM bit of TEXA register
										 *	16bpp:
										 *		when false and 'A' is false, use Value0 for alpha channel
										 *		when true and 'A' is true, use Value1 for alpha channel
										 *		when true and 'A' is false, full transparency when RGB is solid black (0,0,0)
										 *	24bpp:
										 *		when false, use Value0 for alpha channel
										 *		when true, full transparency when RGB is solid black (0,0,0)
										 *	32bpp: N/A
										*/

		uint32_t Value1 : 8;			/*
										 * global alpha [1]
										 *	- TA1 bit of TEXA register
										*/

		uint32_t reserved1 : 6;			// zero padding

		uint32_t Blend : 1;				/*
										 * blending
										 *	0 = disabled
										 *	1 = enabled when MSB of 'A' is active
										 *	- PABE bit of PABE register
										*/

		uint32_t Correction : 1;		/*
										 * correction
										 *	0 = disabled
										 *	1 = enabled when MSB of 'A' is active
										 *	- FBA bit of FBA_1 and FBA_2 registers
										*/
	} Alpha;

	struct PALETTE_EX
	{
		uint32_t Width : 6;				/*
										 * buffer width (/ 64)
										 *	- valid when palette alignment is Mode 2
										*/

		uint32_t U : 6;					/*
										 * U offset (/ 16)
										 *	- valid when palette alignment is Mode 2
										*/

		uint32_t V : 10;				/*
										 * V offset
										 *	- valid when palette alignment is Mode 2
										*/

		uint32_t reserved2 : 10;		// zero padding
	} PaletteAttrEx;

};

struct Sony_Texture_2_MipMap
{
	int64_t Level1Ptr : 13;
	int64_t Level1Width : 6;
	int64_t Level2Ptr : 13;
	int64_t Level2Width : 6;
	int64_t Level3Ptr : 13;
	int64_t Level3Width : 6;
	int64_t reserved0 : 7;
	int64_t Level4Ptr : 13;
	int64_t Level4Width : 6;
	int64_t Level5Ptr : 13;
	int64_t Level5Width : 6;
	int64_t Level6Ptr : 13;
	int64_t Level6Width : 6;
	int64_t reserved1 : 7;
};

struct Sony_Texture_2_ExData
{
	int8_t Type[4];						// "eXt"

	uint32_t Size;						// size of this struct + size of data chunk

	uint32_t DataSize;					/*
										 * size of data beyond this struct
										 *  - if zero (0), comment string is available after this struct
										 *  - if this value equals size of this struct + Size, string comment is not available
										*/

	uint32_t reserved : 32;				// zero padding
};


typedef Pixel_4bpp Sony_Pixel_4bpp;

typedef Pixel_8bpp Sony_Pixel_8bpp;

typedef Pixel_16bppL Sony_Pixel_16bpp;

typedef Pixel_24bppL Sony_Pixel_24bpp;

typedef Pixel_32bppL Sony_Pixel_32bpp;


#pragma pack(pop)


class Sony_PlayStation_Texture_2 {

	Sony_Texture_2_Header m_Header;

	struct TEXTURE
	{
		Sony_Texture_2_Data Data{};

		std::pair<Sony_Texture_2_MipMap, std::vector<uint32_t>> MipMap;

		std::pair<Sony_Texture_2_ExData, std::vector<uint8_t>> ExData;

		std::vector<std::uint8_t> Palette;

		std::vector<std::uint8_t> Pixels;
	};

	std::vector<std::unique_ptr<TEXTURE>> m_Texture;

	bool b_Open;

	[[nodiscard]] constexpr auto Texture(std::uint16_t i) noexcept -> TEXTURE&
	{
		if (m_Texture.empty())
		{
			m_Texture.resize(1);
			i = 0;
		}
		if (m_Texture[i].get() == nullptr)
		{
			m_Texture[i] = std::make_unique<TEXTURE>();
		}
		return *m_Texture[i].get();
	}

	String PixelModeStr(Sony_Pixel_Mode Input);

	String PaletteAlignStr(Sony_Pixel_Mode PixelMode, uint8_t Input);

	String PixelFunctionStr(uint8_t Input);

	String FilterMagStr(uint8_t Input);

	String FilterMinStr(uint8_t Input);

	String MipTypeStr(uint8_t Input);

	String AlphaExpansionStr(uint8_t Input);

public:

	Standard_String Str;

	[[nodiscard]] String HeaderStr(void);

	[[nodiscard]] String DataStr(std::uint16_t iTexture);

	[[nodiscard]] String PaletteStr(std::uint16_t iTexture);

	[[nodiscard]] String PixelStr(std::uint16_t iTexture);

	[[nodiscard]] String AttrStr(std::uint16_t iTexture);

	[[nodiscard]] String SamplingStr(std::uint16_t iTexture);

	[[nodiscard]] String AlphaStr(std::uint16_t iTexture);

	[[nodiscard]] String ExDataStr(std::uint16_t iTexture);

	explicit Sony_PlayStation_Texture_2(void) :
		m_Header(),
		m_Texture(),
		b_Open(false)
	{
	}

	explicit Sony_PlayStation_Texture_2(std::filesystem::path Path, std::size_t pSource = 0) :
		m_Header(),
		m_Texture(),
		b_Open(false)
	{
		OpenTIM2(Path, pSource);
	}

	~Sony_PlayStation_Texture_2(void) = default;

	/*
		Available Data?
	*/
	[[nodiscard]] bool IsOpen(void) const { return b_Open; }

	/*
		Available Textures?
	*/
	[[nodiscard]] bool IsValid(void) const { return !m_Texture.empty(); }

	/*
		Available Palette?
	*/
	[[nodiscard]] bool IsPalette(std::uint16_t iTexture) { return Texture(iTexture).Data.PaletteSize; }

	/*
		Available Pixels?
	*/
	[[nodiscard]] bool IsPixel(std::uint16_t iTexture) { return Texture(iTexture).Data.PixelSize; }

	/*
		Get bits per pixel
	*/
	[[nodiscard]] std::uint16_t GetDepth(std::uint16_t iTexture)
	{
		switch (GetPixelMode(Texture(iTexture).Data.Attr.PixelMode))
		{
		case Sony_Pixel_Mode::RGBA32: return 32;
		case Sony_Pixel_Mode::RGBA24: return 24;
		case Sony_Pixel_Mode::RGBA16: return 16;
		case Sony_Pixel_Mode::RGBA16S: return 16;
		case Sony_Pixel_Mode::RGB: return 24;
		case Sony_Pixel_Mode::CLUT8: return 8;
		case Sony_Pixel_Mode::CLUT4: return 4;
		case Sony_Pixel_Mode::CLUT8H: return 8;
		case Sony_Pixel_Mode::CLUT4HL: return 4;
		case Sony_Pixel_Mode::CLUT4HH: return 4;
		case Sony_Pixel_Mode::ZBUFF32: return 32;
		case Sony_Pixel_Mode::ZBUFF24: return 24;
		case Sony_Pixel_Mode::ZBUFF16: return 16;
		case Sony_Pixel_Mode::ZBUFF16S: return 16;
		default: return 0;
		}
	}

	/*
		Get palette bits per pixel
	*/
	[[nodiscard]] std::uint16_t GetPaletteDepth(std::uint16_t iTexture)
	{
		switch (GetPixelModeVer2(Texture(iTexture).Data.PaletteAttr.PixelMode))
		{
		case Sony_Pixel_Mode::RGBA32: return 32;
		case Sony_Pixel_Mode::RGBA24: return 24;
		case Sony_Pixel_Mode::RGBA16: return 16;
		default: return 0;
		}
	}

	/*
		Get Pixel Mode from value
	*/
	[[nodiscard]] Sony_Pixel_Mode GetPixelMode(std::uint8_t Input)
	{
		switch (Input)
		{
		case std::to_underlying(Sony_Pixel_Mode::RGBA32): return Sony_Pixel_Mode::RGBA32;
		case std::to_underlying(Sony_Pixel_Mode::RGBA24): return Sony_Pixel_Mode::RGBA24;
		case std::to_underlying(Sony_Pixel_Mode::RGBA16): return Sony_Pixel_Mode::RGBA16;
		case std::to_underlying(Sony_Pixel_Mode::RGBA16S): return Sony_Pixel_Mode::RGBA16S;
		case std::to_underlying(Sony_Pixel_Mode::RGB): return Sony_Pixel_Mode::RGB;
		case std::to_underlying(Sony_Pixel_Mode::CLUT8): return Sony_Pixel_Mode::CLUT8;
		case std::to_underlying(Sony_Pixel_Mode::CLUT4): return Sony_Pixel_Mode::CLUT4;
		case std::to_underlying(Sony_Pixel_Mode::CLUT8H): return Sony_Pixel_Mode::CLUT8H;
		case std::to_underlying(Sony_Pixel_Mode::CLUT4HL): return Sony_Pixel_Mode::CLUT4HL;
		case std::to_underlying(Sony_Pixel_Mode::CLUT4HH): return Sony_Pixel_Mode::CLUT4HH;
		case std::to_underlying(Sony_Pixel_Mode::ZBUFF32): return Sony_Pixel_Mode::ZBUFF32;
		case std::to_underlying(Sony_Pixel_Mode::ZBUFF24): return Sony_Pixel_Mode::ZBUFF24;
		case std::to_underlying(Sony_Pixel_Mode::ZBUFF16): return Sony_Pixel_Mode::ZBUFF16;
		case std::to_underlying(Sony_Pixel_Mode::ZBUFF16S): return Sony_Pixel_Mode::ZBUFF16S;
		default: return Sony_Pixel_Mode::null;
		}
	}

	/*
		Get Pixel Mode from value (alternate version for fixed-index values 0-5)
	*/
	[[nodiscard]] Sony_Pixel_Mode GetPixelModeVer2(std::uint8_t Input)
	{
		return Input == 0 ? Sony_Pixel_Mode::null :
			Input == 1 ? Sony_Pixel_Mode::RGBA16 :
			Input == 2 ? Sony_Pixel_Mode::RGBA24 :
			Input == 3 ? Sony_Pixel_Mode::RGBA32 :
			Input == 4 ? Sony_Pixel_Mode::CLUT4 :
			Input == 5 ? Sony_Pixel_Mode::CLUT8 :
			Sony_Pixel_Mode::null;
	}

	/*
		Get/Set raw pixel data
	*/
	[[nodiscard]] std::vector<std::uint8_t>& GetPixels(std::uint16_t iTexture) { return Texture(iTexture).Pixels; }

	/*
		Get total pixel count
	*/
	[[nodiscard]] std::uint32_t GetPixelCount(std::uint16_t iTexture) { return Texture(iTexture).Data.Width * Texture(iTexture).Data.Height; }

	/*
		Get total file size of all pixel data from given depth, width and height
	*/
	[[nodiscard]] std::uint32_t GetPixelSize(std::uint16_t Depth, std::uint16_t Width, std::uint16_t Height)
	{
		return Depth == 4 ? ((size_t)((Width / 2) * Height) * sizeof(Sony_Pixel_4bpp)) :
			Depth == 8 ? ((size_t)(Width * Height) * sizeof(Sony_Pixel_8bpp)) :
			Depth == 16 ? ((size_t)(Width * Height) * sizeof(Sony_Pixel_16bpp)) :
			Depth == 24 ? ((size_t)(Width * Height) * sizeof(Sony_Pixel_24bpp)) :
			Depth == 32 ? ((size_t)(Width * Height) * sizeof(Sony_Pixel_32bpp)) : 0;
	}

	/*
		Get total file size of pixel data
	*/
	[[nodiscard]] std::uint32_t GetPixelSize(std::uint16_t iTexture) { return Texture(iTexture).Data.PixelSize; }

	/*
		Get pixel width
	*/
	[[nodiscard]] std::uint16_t GetWidth(std::uint16_t iTexture) { return Texture(iTexture).Data.Width; }

	/*
		Get pixel height
	*/
	[[nodiscard]] std::uint16_t GetHeight(std::uint16_t iTexture) { return Texture(iTexture).Data.Height; }

	/*
		Deswizzle pixel buffer
	*/
	void DeswizzlePixels(
		std::vector<std::uint8_t> Source, std::vector<std::uint8_t>& Destination,
		std::uint16_t Depth,
		std::uint16_t Width, std::uint16_t Height,
		std::uint16_t TileWidth = 8, std::uint16_t TileHeight = 2);

	/*
		Get 4-bit pixel data
	*/
	[[nodiscard]] Sony_Pixel_4bpp Get4bpp(std::uint16_t iTexture, std::size_t X, std::size_t Y)
	{ return *reinterpret_cast<Sony_Pixel_4bpp*>(&Texture(iTexture).Pixels[(Y * (GetWidth(iTexture) / 2)) + (X / 2)]); }

	/*
		Get 8-bit pixel data
	*/
	[[nodiscard]] Sony_Pixel_8bpp Get8bpp(std::uint16_t iTexture, std::size_t X, std::size_t Y)
	{ return *reinterpret_cast<Sony_Pixel_8bpp*>(&Texture(iTexture).Pixels[((Y * GetWidth(iTexture)) + X)]); }

	/*
		Get 16-bit color from pixel data
	*/
	[[nodiscard]] Sony_Pixel_16bpp Get16bpp(std::uint16_t iTexture, std::size_t X, std::size_t Y)
	{ return *reinterpret_cast<Sony_Pixel_16bpp*>(&Texture(iTexture).Pixels[(Y * GetWidth(iTexture) + X) * sizeof(Sony_Pixel_16bpp)]); }

	/*
		Get 24-bit colors from pixel data
	*/
	[[nodiscard]] Sony_Pixel_24bpp Get24bpp(std::uint16_t iTexture, std::size_t X, std::size_t Y)
	{ return *reinterpret_cast<Sony_Pixel_24bpp*>(&Texture(iTexture).Pixels[(Y * (((size_t)GetWidth(iTexture) * 24 + 7) / 8)) + (X * (24 / 8))]); }

	/*
		Get 32-bit colors from pixel data
	*/
	[[nodiscard]] Sony_Pixel_32bpp Get32bpp(std::uint16_t iTexture, std::size_t X, std::size_t Y)
	{ return *reinterpret_cast<Sony_Pixel_32bpp*>(&Texture(iTexture).Pixels[(Y * GetWidth(iTexture) + X) * sizeof(Sony_Pixel_32bpp)]); }

	/*
		Get/Set raw palette data
	*/
	[[nodiscard]] std::vector<std::uint8_t>& GetPalette(std::uint16_t iTexture) { return Texture(iTexture).Palette; }

	/*
		Get absolute pointer to raw palette data chunk by given palette index
	*/
	[[nodiscard]] std::uint32_t GetPalettePtr(std::uint16_t iTexture, std::uint16_t iPalette);

	/*
		Get max amount of colors per palette
	*/
	[[nodiscard]] std::uint16_t GetPaletteColorMax(std::uint16_t PixelDepth) { return PixelDepth == 4 ? 16 : 256; }

	/*
		Get total palette count
	*/
	[[nodiscard]] std::uint16_t GetPaletteCount(std::uint16_t iTexture);

	/*
		Get max accessible palette index
	*/
	[[nodiscard]] std::uint16_t GetPaletteMaxIndex(std::uint16_t iTexture) { return GetPaletteCount(iTexture) ? GetPaletteCount(iTexture) - 1 : 0; }

	/*
		Get total file size of single palette data by given pixel depths
	*/
	[[nodiscard]] std::uint32_t GetPaletteSingleSize(std::uint16_t PixelDepth, std::uint16_t PaletteDepth);

	/*
		Get total file size of all palette data
	*/
	[[nodiscard]] std::uint32_t GetPaletteSize(std::uint16_t iTexture) { return Texture(iTexture).Data.PaletteSize; }

	/*
		Get pixel width of single palette in vram
		 - palette color pixel data alignment:
			Mode 1: 8x2 (CLUT4) or 16x16 (CLUT8)
			Mode 2: 16x1 (CLUT4) or 256x1 (CLUT8)
	*/
	[[nodiscard]] std::uint16_t GetVRAMPaletteWidth(std::uint16_t iTexture) { return Texture(iTexture).Data.PaletteAttr.Align ? GetDepth(iTexture) == 4 ? 16 : 256 : GetDepth(iTexture) == 4 ? 8 : 16; }

	/*
		Get pixel height of single palette in vram
		 - palette color pixel data alignment:
			Mode 1: 8x2 (CLUT4) or 16x16 (CLUT8)
			Mode 2: 16x1 (CLUT4) or 256x1 (CLUT8)
	*/
	[[nodiscard]] std::uint16_t GetVRAMPaletteHeight(std::uint16_t iTexture) { return Texture(iTexture).Data.PaletteAttr.Align ? 1 : GetDepth(iTexture) == 4 ? 2 : 16; }

	/*
		Get 16-bit palette color
		- if palette is empty, return is superblack (0,0,0,0) color
	*/
	[[nodiscard]] Sony_Pixel_16bpp GetPaletteColor16bpp(std::uint16_t iTexture, std::uint16_t iPalette, std::uint16_t iColor)
	{
		return !GetPaletteCount(iTexture) ? Sony_Pixel_16bpp{ 0, 0, 0, false } :
			*reinterpret_cast<Sony_Pixel_16bpp*>(&Texture(iTexture).Palette[(size_t)GetPalettePtr(iTexture, iPalette) + iColor * sizeof(Sony_Pixel_16bpp)]);
	}

	/*
		Get 24-bit palette color
		- if palette is empty, return is superblack (0,0,0) color
	*/
	[[nodiscard]] Sony_Pixel_24bpp GetPaletteColor24bpp(std::uint16_t iTexture, std::uint16_t iPalette, std::uint16_t iColor)
	{
		return !GetPaletteCount(iTexture) ? Sony_Pixel_24bpp{ 0, 0, 0 } :
			*reinterpret_cast<Sony_Pixel_24bpp*>(&Texture(iTexture).Palette[(size_t)GetPalettePtr(iTexture, iPalette) + iColor * sizeof(Sony_Pixel_24bpp)]);
	}

	/*
		Get 32-bit palette color
		- if palette is empty, return is superblack (0,0,0,0) color
	*/
	[[nodiscard]] Sony_Pixel_32bpp GetPaletteColor32bpp(std::uint16_t iTexture, std::uint16_t iPalette, std::uint16_t iColor)
	{
		return !GetPaletteCount(iTexture) ? Sony_Pixel_32bpp{ 0, 0, 0, 0 } :
			*reinterpret_cast<Sony_Pixel_32bpp*>(&Texture(iTexture).Palette[(size_t)GetPalettePtr(iTexture, iPalette) + iColor * sizeof(Sony_Pixel_32bpp)]);
	}

	/*
		Get total file size of mipmap header data
	*/
	[[nodiscard]] std::uint32_t GetMipMipDataSize(std::uint8_t Level);

	/*
		Open Texture Image 2 (*.TM2) file
	*/
	bool OpenTIM2(StdFile& File, std::uintmax_t pSource, bool b_ReadPalette, bool b_ReadPixels);

	/*
		Open Texture Image 2 (*.TM2) file
	*/
	bool OpenTIM2(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_ReadPalette = false, bool b_ReadPixels = false)
	{
		StdFile m_File;
		m_File.SetPath(Path);
		return OpenTIM2(m_File, pSource, b_ReadPalette, b_ReadPixels);
	}

	/*
		Update Standard Image Palette
	*/
	void UpdateImagePalette(std::unique_ptr<Standard_Image>& Image, std::uint16_t iTexture = 0, std::uint16_t iPalette = 0);

	/*
		Export Standard Image Object
	*/
	std::unique_ptr<Standard_Image> ExportImage(std::uint16_t iTexture = 0, std::uint16_t iPalette = 0);

};