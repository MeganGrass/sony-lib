/*
*
*	Megan Grass
*	March 07, 2024
*
*/


#pragma once

#include <std_basic_fstream.h>

#include <std_image.h>

#include <functional>

#include <sony_bitstream.h>

#ifdef DeletePalette
#undef DeletePalette
#endif


#pragma pack(push, 1)


struct Sony_Texture_Header
{
	std::uint32_t ID : 8;			// 0x10 = TIM, 0x11 = CLT, 0x12 = PXL
	std::uint32_t Version : 8;		// always zero (0)
	std::uint32_t Reserved0 : 16;	// always zero (0)
	std::uint32_t PixelMode : 3;	// 0 = 4bpp, 1 = 8bpp, 2 = 16bpp, 3 = 24bpp, 4 = Mixed (always 2 if CLT)
	std::uint32_t ClutFlag : 1;		// 0 = no palette data, 1 = palette data is available (always zero if CLT or PXL)
	std::uint32_t Reserved1 : 28;	// always zero (0)
};


struct Sony_Texture_Data
{
	std::uint32_t Size;				// size of this struct + size of data
	std::uint16_t X;				// VRAM x position
	std::uint16_t Y;				// VRAM y position
	std::uint16_t Width;			// data width
	std::uint16_t Height;			// data height
};


struct Sony_Texture_4bpp
{
	std::uint8_t Pix0 : 4;			// pixel 0
	std::uint8_t Pix1 : 4;			// pixel 1
	std::uint8_t Pix2 : 4;			// pixel 2
	std::uint8_t Pix3 : 4;			// pixel 3
};


struct Sony_Texture_8bpp
{
	std::uint8_t Pix0 : 8;			// pixel 0
	std::uint8_t Pix1 : 8;			// pixel 1
};


struct Sony_Texture_16bpp
{
	std::uint16_t R : 5;			// red
	std::uint16_t G : 5;			// green
	std::uint16_t B : 5;			// blue
	std::uint16_t STP : 1;			// semi-transparency processing flag
	bool operator ! (void) const
	{
		return !this->R && !this->G && !this->B && !this->STP;
	}
	bool operator = (const Sony_Texture_16bpp& External)
	{
		std::memcpy(this, &External, sizeof(Sony_Texture_16bpp));
		return true;
	}
	bool operator == (const Sony_Texture_16bpp& External) const
	{
		return (this->R == External.R) && (this->G == External.G) && (this->B == External.B) && (this->STP == External.STP);
	}
	bool operator != (const Sony_Texture_16bpp& External) const
	{
		return (this->R != External.R) || (this->G != External.G) || (this->B != External.B) || (this->STP != External.STP);
	}
};


struct Sony_Texture_24bpp
{
	std::uint8_t R0 : 8;			// red	\|
	std::uint8_t G0 : 8;			// green |-> pixel 0
	std::uint8_t B0 : 8;			// blue	/|
	std::uint8_t R1 : 8;			// red	\|
	std::uint8_t G1 : 8;			// green |-> pixel 1
	std::uint8_t B1 : 8;			// blue	/|
};


#pragma pack(pop)


enum class Sony_Texture_Transparency : int32_t
{
	None = 0,						// No Options
	Superblack = (1 << 0),			// Full Transparency for solid black
	Superimposed = (1 << 1),		// Semi/Full Transparency for palette index 0
	External = (1 << 2),			// Semi/Full Transparency for external source
	Half = (1 << 3),				// Semi-Transparency: 50%back + 50%texture (incompatible with Full, Inverse and Quarter)
	Full = (1 << 4),				// Semi-Transparency: 100%back + 100%texture (incompatible with Half, Inverse and Quarter)
	Inverse = (1 << 5),				// Semi-Transparency: 100%back - 100%texture (incompatible with Half, Full and Quarter)
	Quarter = (1 << 6),				// Semi-Transparency: 100%back + 25%texture (incompatible with Half, Full and Inverse)
	STP = (1 << 7),					// STP flag determines if Semi-Transparency is used (4bpp, 8bpp and 16bpp only)
};

static Sony_Texture_Transparency operator | (Sony_Texture_Transparency _Mode0, Sony_Texture_Transparency _Mode1)
{
	return static_cast<Sony_Texture_Transparency>(std::to_underlying(_Mode0) | std::to_underlying(_Mode1));
}

static Sony_Texture_Transparency operator |= (Sony_Texture_Transparency& _Mode0, Sony_Texture_Transparency _Mode1)
{
	return _Mode0 = static_cast<Sony_Texture_Transparency>(std::to_underlying(_Mode0) | std::to_underlying(_Mode1));
}

static Sony_Texture_Transparency operator ^ (Sony_Texture_Transparency _Mode0, Sony_Texture_Transparency _Mode1)
{
	return static_cast<Sony_Texture_Transparency>(std::to_underlying(_Mode0) ^ std::to_underlying(_Mode1));
}

static Sony_Texture_Transparency operator ^= (Sony_Texture_Transparency& _Mode0, Sony_Texture_Transparency _Mode1)
{
	return _Mode0 = static_cast<Sony_Texture_Transparency>(std::to_underlying(_Mode0) ^ std::to_underlying(_Mode1));
}


struct Sony_Texture_Create_Ex
{
	std::uint16_t Depth;			// 4, 8, 16, 24
	std::uint16_t Width;			// 1024 max
	std::uint16_t Height;			// 512 max
	std::uint16_t nPalette;			// number of raw palettes to create/read
	ImageType PaletteType;			// palette file type
	ImageType PixelType;			// pixel file type
	std::uintmax_t pPalette;		// absolute pointer to palette data/file
	std::uintmax_t pPixel;			// absolute pointer to pixel data/file
	std::filesystem::path Palette;	// palette file
	std::filesystem::path Pixel;	// pixel file
	explicit Sony_Texture_Create_Ex(void) : Depth(16), Width(0), Height(0), nPalette(0), PaletteType(ImageType::None), PixelType(ImageType::None), pPalette(0), pPixel(0), Palette(), Pixel() {}
};


class Sony_PlayStation_Texture {

	// Texture Image (*.TIM) File Header
	Sony_Texture_Header m_Header;

	// Palette Header
	Sony_Texture_Data m_PaletteHeader;

	// Pixel Header
	Sony_Texture_Data m_PixelHeader;

	// Palette
	std::vector<Sony_Texture_16bpp> m_Palette;

	// Pixels
	std::vector<std::uint8_t> m_Pixels;

	// Transparency Flags
	Sony_Texture_Transparency m_Transparency;

	// Transparency Rate (50%back + 50%texture)
	bool b_TransparencyHalf;

	// Transparency Rate (100%back + 100%texture)
	bool b_TransparencyFull;

	// Transparency Rate (100%back - 100%texture)
	bool b_TransparencyInverse;

	// Transparency Rate (100%back + 25%texture)
	bool b_TransparencyQuarter;

	// Superblack Transparency
	bool b_TransparencySuperblack;

	// STP Flag Transparency
	bool b_TransparencySTP;

	// Superimposed Transparency
	bool b_TransparencySuperimposed;

	// External Color Transparency
	bool b_TransparencyExternal;

	// External Transparent Color
	static DWORD m_TransparentColor;

	// Flag
	bool b_Open;

	// File Type constants
	static constexpr int TIM = std::to_underlying(ImageType::TIM);
	static constexpr int CLT = std::to_underlying(ImageType::CLT);
	static constexpr int PXL = std::to_underlying(ImageType::PXL);
#ifdef LIB_JPEG
	static constexpr int JPG = std::to_underlying(ImageType::JPG);
#endif
#ifdef LIB_PNG
	static constexpr int PNG = std::to_underlying(ImageType::PNG);
#endif
	static constexpr int PAL = std::to_underlying(ImageType::PAL);
	static constexpr int BMP = std::to_underlying(ImageType::BMP);
	static constexpr int RAW = std::to_underlying(ImageType::RAW);

public:

	// Standard String
	Standard_String Str;

	explicit Sony_PlayStation_Texture(void) :
		m_Header{},
		m_PaletteHeader{},
		m_PixelHeader{},
		m_Palette(),
		m_Pixels(),
		m_Transparency(Sony_Texture_Transparency::None),
		b_TransparencyHalf(false),
		b_TransparencyFull(false),
		b_TransparencyInverse(false),
		b_TransparencyQuarter(false),
		b_TransparencySuperblack(false),
		b_TransparencySTP(false),
		b_TransparencySuperimposed(false),
		b_TransparencyExternal(false),
		b_Open(false)
	{
	}

	explicit Sony_PlayStation_Texture(std::filesystem::path Path, std::size_t pSource = 0) :
		m_Header{},
		m_PaletteHeader{},
		m_PixelHeader{},
		m_Palette(),
		m_Pixels(),
		m_Transparency(Sony_Texture_Transparency::None),
		b_TransparencyHalf(false),
		b_TransparencyFull(false),
		b_TransparencyInverse(false),
		b_TransparencyQuarter(false),
		b_TransparencySuperblack(false),
		b_TransparencySTP(false),
		b_TransparencySuperimposed(false),
		b_TransparencyExternal(false),
		b_Open(false)
	{
		OpenTIM(Path, pSource);
	}

	explicit Sony_PlayStation_Texture(std::uint16_t Depth, std::uint16_t Width, std::uint16_t Height, std::uint16_t nPalette) :
		m_Header{},
		m_PaletteHeader{},
		m_PixelHeader{},
		m_Palette(),
		m_Pixels(),
		m_Transparency(Sony_Texture_Transparency::None),
		b_TransparencyHalf(false),
		b_TransparencyFull(false),
		b_TransparencyInverse(false),
		b_TransparencyQuarter(false),
		b_TransparencySuperblack(false),
		b_TransparencySTP(false),
		b_TransparencySuperimposed(false),
		b_TransparencyExternal(false),
		b_Open(false)
	{
		Create(Depth, Width, Height, nPalette);
	}

	explicit Sony_PlayStation_Texture(std::unique_ptr<Sony_PlayStation_Texture>& External) :
		m_Header{},
		m_PaletteHeader{},
		m_PixelHeader{},
		m_Palette(),
		m_Pixels(),
		m_Transparency(Sony_Texture_Transparency::None),
		b_TransparencyHalf(false),
		b_TransparencyFull(false),
		b_TransparencyInverse(false),
		b_TransparencyQuarter(false),
		b_TransparencySuperblack(false),
		b_TransparencySTP(false),
		b_TransparencySuperimposed(false),
		b_TransparencyExternal(false),
		b_Open(false)
	{
		OpenTIM(External);
	}

	~Sony_PlayStation_Texture(void) = default;

	/*
		Is the texture open?
	*/
	[[nodiscard]] bool IsOpen(void) const { return b_Open; }

	/*
		Does the texture have either palette or pixel data?
	*/
	bool IsValid(void) const { return !(m_Pixels.empty() && m_Palette.empty()); }

	/*
		Get basic information about the texture
	*/
	String About(void);

	/*
		Print basic information about the texture
	*/
	void Print(void) { std::cout << About() << std::endl; }

	/*
		Clear all data and declare texture as closed
	*/
	void Close(void);

	/*
		Update Transparency flags
		- parse various transparency boolean flags and update TransparencyFlags
	*/
	void UpdateTransparencyFlags(void);

	/*
		Get/Set transparency flags
	*/
	[[nodiscard]] Sony_Texture_Transparency& TransparencyFlags(void) { return m_Transparency; }

	/*
		Enable or disable Half transparency rate
		- 50%back + 50%texture
	*/
	[[nodiscard]] bool& TransparencyHalf(void) noexcept { return b_TransparencyHalf; }

	/*
		Enable or disable Full transparency rate
		- 100%back + 100%texture
	*/
	[[nodiscard]] bool& TransparencyFull(void) noexcept { return b_TransparencyFull; }

	/*
		Enable or disable Inverse transparency rate
		- 100%back - 100%texture
	*/
	[[nodiscard]] bool& TransparencyInverse(void) noexcept { return b_TransparencyInverse; }

	/*
		Enable or disable Quarter transparency rate
		- 100%back + 25%texture
	*/
	[[nodiscard]] bool& TransparencyQuarter(void) noexcept { return b_TransparencyQuarter; }

	/*
		Enable or disable Superblack transparency
		- solid black pixels (0,0,0,0) are fully transparent
	*/
	[[nodiscard]] bool& TransparencySuperblack(void) noexcept { return b_TransparencySuperblack; }

	/*
		Enable or disable STP transparency
		- colors with semi-transparency flag are semi/fully transparent
		- 4bpp, 8bpp and 16bpp only
	*/
	[[nodiscard]] bool& TransparencySTP(void) noexcept { return b_TransparencySTP; }

	/*
		Enable or disable Superimposed transparency
		- color entry at palette index (0) is semi/fully transparent
	*/
	[[nodiscard]] bool& TransparencySuperimposed(void) noexcept { return b_TransparencySuperimposed; }

	/*
		Enable or disable external color transparency
		- "TransparencyColor" is semi/fully transparent
	*/
	[[nodiscard]] bool& TransparencyExternal(void) noexcept { return b_TransparencyExternal; }

	/*
		Get/Set external transparent color
	*/
	[[nodiscard]] DWORD& TransparencyColor(void) noexcept { return m_TransparentColor; }

	/*
		Get bits per pixel
	*/
	[[nodiscard]] std::uint16_t GetDepth(void) const;

	/*
		Set bits per pixel
	*/
	void SetDepth(std::uint16_t Depth);

	/*
		Get CLUT Flag
		- false (no palette data), true (palette data is available)
	*/
	[[nodiscard]] bool GetCF(void) const { return m_Header.ClutFlag; }

	/*
		Set CLUT Flag
		- false (no palette data), true (palette data is available)
		- palette data is not modified when setting this flag
	*/
	[[nodiscard]] void SetCF(bool ClutFlag) { m_Header.ClutFlag = ClutFlag; }

	/*
		Update palette data header "Size" field
	*/
	void UpdatePaletteDataSize(void) { m_PaletteHeader.Size = GetPaletteDataSize(); }

	/*
		Get/Set raw palette data
	*/
	[[nodiscard]] std::vector<Sony_Texture_16bpp>& GetPalette(void) { return m_Palette; }

	/*
		Get max amount of palettes that can be stored
	*/
	[[nodiscard]] std::uint16_t GetPaletteCountMax(void) const { return GetDepth() == 4 ? ((1024 * 512) / 16) : ((1024 * 512) / 256); }

	/*
		Get max amount of colors per palette
		- 16 (4bpp) or 256 (8bpp/16bpp/24bpp)
	*/
	[[nodiscard]] std::uint16_t GetPaletteColorMax(void) const { return GetDepth() == 4 ? 16 : 256; }

	/*
		Get total palette count
	*/
	[[nodiscard]] std::uint16_t GetPaletteCount(void) const { return GetCF() ? (m_PaletteHeader.Width / GetPaletteColorMax()) * m_PaletteHeader.Height : 0; }

	/*
		Get max accessible palette index
	*/
	[[nodiscard]] std::uint16_t GetPaletteMaxIndex(void) const { return GetPaletteCount() ? GetPaletteCount() - 1 : 0; }

	/*
		Get total file size of single palette data (w/out header)
	*/
	[[nodiscard]] std::uint32_t GetPaletteSingleSize(void) const { return GetPaletteColorMax() * sizeof(Sony_Texture_16bpp); }

	/*
		Get total file size of all palette data (w/out header)
	*/
	[[nodiscard]] std::uint32_t GetPaletteSize(void) const { return GetPaletteCount() * GetPaletteSingleSize(); }

	/*
		Get total file size of all palette data (w/ header)
	*/
	[[nodiscard]] std::uint32_t GetPaletteDataSize(void) const { return GetCF() ? GetPaletteSize() + sizeof(Sony_Texture_Data) : 0; }

	/*
		Get palette data VRAM X coordinate
	*/
	std::uint16_t GetPaletteX(void) const { return m_PaletteHeader.X; }

	/*
		Set palette data VRAM X coordinate
	*/
	void SetPaletteX(std::uint16_t X) { m_PaletteHeader.X = std::clamp(X, (std::uint16_t)0, (std::uint16_t)(1024 - GetPaletteWidth())); }

	/*
		Get palette data VRAM Y coordinate
	*/
	[[nodiscard]] std::uint16_t GetPaletteY(void) const { return m_PaletteHeader.Y; }

	/*
		Set palette data VRAM Y coordinate
	*/
	void SetPaletteY(std::uint16_t Y) { m_PaletteHeader.Y = std::clamp(Y, (std::uint16_t)0, (std::uint16_t)(512 - GetPaletteHeight())); }

	/*
		Get palette width
	*/
	[[nodiscard]] std::uint16_t GetPaletteWidth(void) const { return m_PaletteHeader.Width; }

	/*
		Set palette width
		- 4bpp: single palette is 16 x 1 (16 colors)
		- 8bpp: single palette is 256 x 1 (256 colors)
		- width must be divisible by 16 or 256
		- if given width is less than current width, palette height will be modified to preserve data
	*/
	void SetPaletteWidth(std::uint16_t Width);

	/*
		Get palette height
	*/
	[[nodiscard]] std::uint16_t GetPaletteHeight(void) const { return m_PaletteHeader.Height; }

	/*
		Set palette height
		- 4bpp: single palette is 16 x 1 (16 colors)
		- 8bpp: single palette is 256 x 1 (256 colors)
		- if palette width is not divisible by 16 (4bpp) or 256 (8bpp/16bpp/24bpp):
		  additional palettes may be added as superblack (0,0,0,0) padding where needed
	*/
	void SetPaletteHeight(std::uint16_t Height);

	/*
		Get absolute pointer to raw palette data chunk by given palette index
	*/
	[[nodiscard]] std::uint32_t GetPalettePtr(std::uint16_t iPalette) const;

	/*
		Create grayscale palette
		- 16 colors (4bpp) or 256 colors (8bpp/16bpp/24bpp)
	*/
	[[nodiscard]] std::vector<Sony_Texture_16bpp> GrayScalePalette(std::uint16_t nColors);

	/*
		Convert unsigned char vector source to palette data
		- if source is empty, return is single superblack (0,0,0,0) palette
	*/
	[[nodiscard]] std::vector<Sony_Texture_16bpp> PaletteFromUChar(std::vector<std::uint8_t> Source) const;

	/*
		Convert palette data source to unsigned char vector
		- if source is empty, return is single superblack (0,0,0,0) palette-size chunk
	*/
	[[nodiscard]] std::vector<std::uint8_t> UCharFromPalette(std::vector<Sony_Texture_16bpp> Source) const;

	/*
		Parse conversion of source palette data
		 - export from 4bpp (16 colors per palette) to 8bpp (256 colors per palette) and vice-versa
		 - accepted values for nColorSource and nColorOut: 16 or 256
		 - empty palette on return if nColor is invalid
	*/
	[[nodiscard]] std::vector<Sony_Texture_16bpp> ConvertPalette(std::vector<Sony_Texture_16bpp> Source, std::uint16_t nColorSource, std::uint16_t nColorOut) const;

	/*
		Copy palette at the specified index
		- return value: true (data was copied), false (no change)
	*/
	bool CopyPalette(std::vector<Sony_Texture_16bpp>& Out, std::uint16_t iPalette) const;

	/*
		Paste palette to the specified index
		- return value: true (data was pasted), false (no change)
	*/
	bool PastePalette(std::vector<Sony_Texture_16bpp> Out, std::uint16_t iPalette);

	/*
		Rearrange the palette vector
		- return value: true (data was relocated), false (no change)
	*/
	bool MovePalette(std::uint16_t iPalette, bool b_MoveRight);

	/*
		Append palette/s at back of palette vector
		- palette height is dynamically adjusted
		- if source palette is empty, single superblack (0,0,0,0) palette is appended
		- if palette width is not divisible by 16 (4bpp) or 256 (8bpp/16bpp/24bpp):
		  additional palettes may be added as superblack (0,0,0,0) padding where needed
	*/
	void AddPalette(std::vector<Sony_Texture_16bpp> Source = {});

	/*
		Insert palette at the specified index
		- palette height is increased by one (1)
		- if source palette is empty, superblack (0,0,0,0) palette is inserted
		- if palette width is not divisible by 16 (4bpp) or 256 (8bpp/16bpp/24bpp):
		  additional palettes may be added as superblack (0,0,0,0) padding where needed
	*/
	void InsertPalette(std::uint16_t iPalette, std::vector<Sony_Texture_16bpp> Source = {});

	/*
		Delete palette at the specified index
		- if palette width is not divisible by 16 (4bpp) or 256 (8bpp/16bpp/24bpp):
		  palette at index is set to superblack (0,0,0,0)
		  palette width/height is not modified
		- otherwise:
		  palette at index is erased
		  palette height is decreased by one (1)
		- if b_All is true:
		  all palette data is erased
		  palette width/height is set to zero (0)
		- return value: true (data was deleted), false (no change)
	*/
	bool DeletePalette(std::uint16_t iPalette, bool b_All = false);

	/*
		Update pixel data header "Size" field
	*/
	void UpdatePixelDataSize(void) { m_PixelHeader.Size = GetPixelDataSize(); }

	/*
		Get/Set raw pixel data
	*/
	[[nodiscard]] std::vector<std::uint8_t>& GetPixels(void) { return m_Pixels; }

	/*
		Get total pixel count
	*/
	[[nodiscard]] std::uint32_t GetPixelCount(void) const { return m_PixelHeader.Width * m_PixelHeader.Height; }

	/*
		Get total file size of all pixel data (w/out header) from given depth, width and height
	*/
	[[nodiscard]] std::uint32_t GetPixelSize(std::uint16_t Depth, std::uint16_t Width, std::uint16_t Height) const
	{
		return Depth == 4 ? ((Width / 2) * Height) : Depth == 8 ? (Width * Height) : Depth == 16 ? ((Width * 2) * Height) : Depth == 24 ? ((Width * 3) * Height) : 0;
	}

	/*
		Get total file size of all pixel data (w/out header)
	*/
	[[nodiscard]] std::uint32_t GetPixelSize(void) const { return ((m_PixelHeader.Width * m_PixelHeader.Height) * 2); }

	/*
		Get total file size of all pixel data (w/ header)
	*/
	[[nodiscard]] std::uint32_t GetPixelDataSize(void) const { return (m_PixelHeader.Width && m_PixelHeader.Height) ? GetPixelSize() + sizeof(Sony_Texture_Data) : 0; }

	/*
		Get pixel data VRAM X coordinate
	*/
	[[nodiscard]] std::uint16_t GetPixelX(void) const { return m_PixelHeader.X; }

	/*
		Set pixel data VRAM X coordinate
	*/
	void SetPixelX(std::uint16_t X) { m_PixelHeader.X = std::clamp(X, (std::uint16_t)0, (std::uint16_t)(1024 - GetWidth())); }

	/*
		Get pixel data VRAM Y coordinate
	*/
	[[nodiscard]] std::uint16_t GetPixelY(void) const { return m_PixelHeader.Y; }

	/*
		Set pixel data VRAM Y coordinate
	*/
	void SetPixelY(std::uint16_t Y) { m_PixelHeader.Y = std::clamp(Y, (std::uint16_t)0, (std::uint16_t)(512 - GetHeight())); }

	/*
		Get pixel width
	*/
	[[nodiscard]] std::uint16_t GetWidth(void) const
	{
		switch (m_Header.PixelMode)
		{
		case 0: return m_PixelHeader.Width * 4;
		case 1: return m_PixelHeader.Width * 2;
		case 2: return m_PixelHeader.Width;
		case 3: return ((m_PixelHeader.Width * 2) / 3);
		}
		return 0;
	}

	/*
		Set pixel width
	*/
	void SetWidth(std::uint16_t Width);

	/*
		Get pixel height
	*/
	[[nodiscard]] std::uint16_t GetHeight(void) const { return m_PixelHeader.Height; }

	/*
		Set pixel height
	*/
	void SetHeight(std::uint16_t Height);

	/*
		Get 8-bit red color from 16-bit color
	*/
	[[nodiscard]] std::uint8_t GetRed(Sony_Texture_16bpp Color) { return ((Color.R << 3) | (Color.R >> 2)); }

	/*
		Get 8-bit green color from 16-bit color
	*/
	[[nodiscard]] std::uint8_t GetGreen(Sony_Texture_16bpp Color) { return ((Color.G << 3) | (Color.G >> 2)); }

	/*
		Get 8-bit blue color from 16-bit color
	*/
	[[nodiscard]] std::uint8_t GetBlue(Sony_Texture_16bpp Color) { return ((Color.B << 3) | (Color.B >> 2)); }

	/*
		Get Semi-Transparency Processing Flag from 16-bit color
	*/
	[[nodiscard]] bool GetSTP(Sony_Texture_16bpp Color) { return Color.STP; }

	/*
		Create 16-bit color from 8-bit RGB
	*/
	[[nodiscard]] Sony_Texture_16bpp Create16bpp(std::uint8_t R, std::uint8_t G, std::uint8_t B, bool STP) { return { (uint16_t)(R >> 3), (uint16_t)(G >> 3), (uint16_t)(B >> 3), STP }; }

	/*
		Create 16-bit color from 8-bit RGB
	*/
	[[nodiscard]] Sony_Texture_16bpp Create16bpp(DWORD Color, bool STP) { return { (uint16_t)((Color & 0xFF) >> 3), (uint16_t)(((Color >> 8) & 0xFF) >> 3), (uint16_t)(((Color >> 16) & 0xFF) >> 3), STP }; }

	/*
		Create 24-bit pixel data from 8-bit RGB
	*/
	[[nodiscard]] Sony_Texture_24bpp Create24bpp(std::uint8_t R0, std::uint8_t G0, std::uint8_t B0, std::uint8_t R1, std::uint8_t G1, std::uint8_t B1) { return { R0, G0, B0, R1, G1, B1 }; }

	/*
		Get 16-bit palette color
		- if palette is empty, return is superblack (0,0,0,0) color
	*/
	[[nodiscard]] Sony_Texture_16bpp GetPaletteColor(std::uint16_t iPalette, std::uint16_t iColor) { return !GetPaletteCount() ? Sony_Texture_16bpp{ 0, 0, 0, false } : m_Palette[(size_t)GetPalettePtr(iPalette) + iColor]; }

	/*
		Set 16-bit palette color
	*/
	void SetPaletteColor(std::uint16_t iPalette, std::uint16_t iColor, Sony_Texture_16bpp Color) { if (GetPaletteCount()) { m_Palette[(size_t)GetPalettePtr(iPalette) + iColor] = Color; } }

	/*
		Set 4-bit pixel data
	*/
	void SetPixel(size_t X, size_t Y, Sony_Texture_4bpp Color) { std::memcpy(&m_Pixels[(Y * (GetWidth() / 2)) + (X / 2)], &Color, sizeof(Sony_Texture_4bpp)); }

	/*
		Set 8-bit pixel data
	*/
	void SetPixel(size_t X, size_t Y, Sony_Texture_8bpp Color) { std::memcpy(&m_Pixels[((Y * GetWidth()) + X)], &Color, sizeof(Sony_Texture_8bpp)); }

	/*
		Set 16-bit pixel data
	*/
	void SetPixel(size_t X, size_t Y, Sony_Texture_16bpp Color) { std::memcpy(&m_Pixels[(Y * GetWidth() + X) * sizeof(Sony_Texture_16bpp)], &Color, sizeof(Sony_Texture_16bpp)); }

	/*
		Set 24-bit pixel data
	*/
	void SetPixel(size_t X, size_t Y, Sony_Texture_24bpp Color) { std::memcpy(&m_Pixels[(Y * (((size_t)GetWidth() * 24 + 7) / 8)) + (X * (24 / 8))], &Color, sizeof(Sony_Texture_24bpp)); }

	/*
		Get 4-bit pixel data
	*/
	[[nodiscard]] Sony_Texture_4bpp Get4bpp(std::size_t iPixel) { return iPixel + sizeof(Sony_Texture_4bpp) <= m_Pixels.size() ? *reinterpret_cast<Sony_Texture_4bpp*>(&m_Pixels[iPixel]) : Sony_Texture_4bpp { 0, 0, 0, 0 }; }

	/*
		Get 4-bit pixel data
	*/
	[[nodiscard]] Sony_Texture_4bpp Get4bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_4bpp*>(&m_Pixels[(Y * (GetWidth() / 2)) + (X / 2)]); }

	/*
		Get 16-bit palette color from 4-bit pixel data
	*/
	[[nodiscard]] Sony_Texture_16bpp Get4bppColor(std::size_t X, std::size_t Y, std::uint16_t iPalette) { return GetPaletteColor(iPalette, Get4bpp(X, Y).Pix0); }

	/*
		Get 8-bit pixel data
	*/
	[[nodiscard]] Sony_Texture_8bpp Get8bpp(std::size_t iPixel) { return iPixel + sizeof(Sony_Texture_8bpp) <= m_Pixels.size() ? *reinterpret_cast<Sony_Texture_8bpp*>(&m_Pixels[iPixel]) : Sony_Texture_8bpp { 0, 0 }; }

	/*
		Get 8-bit pixel data
	*/
	[[nodiscard]] Sony_Texture_8bpp Get8bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_8bpp*>(&m_Pixels[((Y * GetWidth()) + X)]); }

	/*
		Get 16-bit palette color from 8-bit pixel data
	*/
	[[nodiscard]] Sony_Texture_16bpp Get8bppColor(std::size_t X, std::size_t Y, std::uint16_t iPalette) { return GetPaletteColor(iPalette, Get8bpp(X, Y).Pix0); }

	/*
		Get 16-bit color from pixel data
	*/
	[[nodiscard]] Sony_Texture_16bpp Get16bpp(std::size_t iPixel) { return iPixel + sizeof(Sony_Texture_16bpp) <= m_Pixels.size() ? *reinterpret_cast<Sony_Texture_16bpp*>(&m_Pixels[iPixel]) : Sony_Texture_16bpp { 0, 0, 0, false }; }

	/*
		Get 16-bit color from pixel data
	*/
	[[nodiscard]] Sony_Texture_16bpp Get16bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_16bpp*>(&m_Pixels[(Y * GetWidth() + X) * sizeof(Sony_Texture_16bpp)]); }

	/*
		Get 24-bit colors from pixel data
	*/
	[[nodiscard]] Sony_Texture_24bpp Get24bpp(std::size_t iPixel) { return iPixel + sizeof(Sony_Texture_24bpp) <= m_Pixels.size() ? *reinterpret_cast<Sony_Texture_24bpp*>(&m_Pixels[iPixel]) : Sony_Texture_24bpp{ 0, 0, 0, 0, 0, 0 }; }

	/*
		Get 24-bit colors from pixel data
	*/
	[[nodiscard]] Sony_Texture_24bpp Get24bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_24bpp*>(&m_Pixels[(Y * (((size_t)GetWidth() * 24 + 7) / 8)) + (X * (24 / 8))]); }

	/*
		Get total Texture Image (*.TIM) file size
	*/
	[[nodiscard]] std::uint32_t Size(void) const { return sizeof(Sony_Texture_Header) + GetPaletteDataSize() + GetPixelDataSize(); }

	/*
		Read header data from file
	*/
	bool ReadData(StdFile& File, std::uintmax_t pSource, Sony_Texture_Data& OutHeader, std::vector<std::uint8_t>& OutData);

	/*
		Read header data from file
	*/
	bool ReadData(std::filesystem::path Path, std::uintmax_t pSource, Sony_Texture_Data& OutHeader, std::vector<std::uint8_t>& OutData);

	/*
		Write header data to file
	*/
	bool WriteData(StdFile& File, std::uintmax_t pSource, Sony_Texture_Data OutHeader, std::vector<std::uint8_t> OutData);

	/*
		Write header data to file
	*/
	bool WriteData(std::filesystem::path Path, std::uintmax_t pSource, Sony_Texture_Data OutHeader, std::vector<std::uint8_t> OutData);

	/*
		Import raw palette data from file
		- if b_Add is true, entire palette data is appended to the current palette data
		- if b_Paste is true, single palette data is pasted to the specified index
		  otherwise, the current palette is completely replaced with new data
	*/
	bool ReadPalette(StdFile& File, std::uintmax_t pSource, std::uint16_t nPalette, bool b_Add, bool b_Paste = false, std::uint16_t iPalette = 0);

	/*
		Import raw palette data from file
		- if b_Add is true, entire palette data is appended to the current palette data
		- if b_Paste is true, single palette data is pasted to the specified index
		  otherwise, the current palette is completely replaced with new data
	*/
	bool ReadPalette(std::filesystem::path Path, std::uintmax_t pSource, std::uint16_t nPalette, bool b_Add = false, bool b_Paste = false, std::uint16_t iPalette = 0);

	/*
		Import raw pixel data from file
	*/
	bool ReadPixels(StdFile& File, std::uintmax_t pSource, std::uint16_t Width, std::uint16_t Height);

	/*
		Import raw pixel data from file
	*/
	bool ReadPixels(std::filesystem::path Path, std::uintmax_t pSource, std::uint16_t Width, std::uint16_t Height);

	/*
		Export raw palette data to file
		- if b_WriteAll is true, iPalette is ignored and all palettes are written to the file
	*/
	bool WritePalette(StdFile& File, std::uintmax_t pSource, std::uint16_t iPalette, bool b_WriteAll);

	/*
		Export raw palette data to file
		- if b_WriteAll is true, iPalette is ignored and all palettes are written to the file
	*/
	bool WritePalette(std::filesystem::path Path, std::uintmax_t pSource, std::uint16_t iPalette = 0, bool b_WriteAll = true, bool b_Truncate = true);

	/*
		Write raw pixel data to file
	*/
	bool WritePixels(StdFile& File, std::uintmax_t pSource);

	/*
		Write raw pixel data to file
		- if b_Truncate is true, the file is truncated to the total size of the pixel data
	*/
	bool WritePixels(std::filesystem::path Path, std::uintmax_t pSource, bool b_Truncate = true);

	/*
		Import pixel data from Texture Image (*.TIM) file
		- if b_Add is true, the palette data is appended to the current palette data
		  otherwise, the current palette is completely replaced with new data
	*/
	bool ReadPaletteTIM(std::unique_ptr<Sony_PlayStation_Texture>& External, bool b_Add);

	/*
		Import palette data from Texture Image (*.TIM) file
		- if b_Add is true, the palette data is appended to the current palette data
		  otherwise, the current palette is completely replaced with new data
	*/
	bool ReadPaletteTIM(StdFile& File, std::uintmax_t pSource, bool b_Add);

	/*
		Import palette data from Texture Image (*.TIM) file
		- if b_Add is true, the palette data is appended to the current palette data
		  otherwise, the current palette is completely replaced with new data
	*/
	bool ReadPaletteTIM(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_Add = false);

	/*
		Import pixel data from Texture Image (*.TIM) file
	*/
	bool ReadPixelsTIM(std::unique_ptr<Sony_PlayStation_Texture>& External);

	/*
		Import pixel data from Texture Image (*.TIM) file
	*/
	bool ReadPixelsTIM(StdFile& File, std::uintmax_t pSource);

	/*
		Import pixel data from Texture Image (*.TIM) file
	*/
	bool ReadPixelsTIM(std::filesystem::path Path, std::uintmax_t pSource = 0);

	/*
		Export palette data to Texture Image (*.TIM) file
		- if b_WriteAll is true, iPalette is ignored and all palettes are written to the file
	*/
	bool WritePaletteTIM(StdFile& File, std::uintmax_t pSource, std::uint16_t iPalette, bool b_WriteAll);

	/*
		Export palette data to Texture Image (*.TIM) file
		- if b_WriteAll is true, iPalette is ignored and all palettes are written to the file
	*/
	bool WritePaletteTIM(std::filesystem::path Path, std::uintmax_t pSource = 0, std::uint16_t iPalette = 0, bool b_WriteAll = true);

	/*
		Export pixel data to Texture Image (*.TIM) file
	*/
	bool WritePixelsTIM(StdFile& File, std::uintmax_t pSource);

	/*
		Export pixel data to Texture Image (*.TIM) file
	*/
	bool WritePixelsTIM(std::filesystem::path Path, std::uintmax_t pSource = 0);

	/*
		Import (replace) Texture Image (*.TIM) file
		- if b_ReadPalette is true, the palette data is imported (if available)
		- if b_ReadPixels is true, the pixel data is imported (if available)
		- if neither b_ReadPalette nor b_ReadPixels is true, operation is aborted
	*/
	bool OpenTIM(std::unique_ptr<Sony_PlayStation_Texture>& TIM, bool b_ReadPalette = true, bool b_ReadPixels = true);

	/*
		Open Texture Image (*.TIM) file
		- if b_ReadPalette is true, the palette data is read from the file
		- if b_ReadPixels is true, the pixel data is read from the file
		- if neither b_ReadPalette nor b_ReadPixels is true, operation is aborted
	*/
	bool OpenTIM(StdFile& File, std::uintmax_t pSource, bool b_ReadPalette, bool b_ReadPixels);

	/*
		Open Texture Image (*.TIM) file
	*/
	bool OpenTIM(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_ReadPalette = true, bool b_ReadPixels = true);

	/*
		Open Texture Image CLUT (*.CLT) file
		- if b_Add is true, the palette data is appended to the current palette data
		  otherwise, the current palette is completely replaced with new data
	*/
	bool OpenCLT(StdFile& File, std::uintmax_t pSource, bool b_Add);

	/*
		Open Texture Image CLUT (*.CLT) file
		- if b_Add is true, the palette data is appended to the current palette data
		  otherwise, the current palette is completely replaced with new data
	*/
	bool OpenCLT(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_Add = false);

	/*
		Open Texture Image pixel (*.PXL) file
	*/
	bool OpenPXL(StdFile& File, std::uintmax_t pSource);

	/*
		Open Texture Image pixel (*.PXL) file
	*/
	bool OpenPXL(std::filesystem::path Path, std::uintmax_t pSource = 0);

	/*
		Open Bitstream (*.BS) file
	*/
	bool OpenBS(StdFile& File, std::uintmax_t pSource, std::uint16_t Width, std::uint16_t Height);

	/*
		Open Bitstream (*.BS) file
	*/
	bool OpenBS(std::filesystem::path Path, std::uintmax_t pSource, std::uint16_t Width, std::uint16_t Height);

	/*
		Open Microsoft RIFF Palette (*.PAL) file
		- if b_Add is true, the palette data is appended to the current palette data
		  otherwise, the current palette is completely replaced with new data
	*/
	bool OpenPAL(StdFile& File, std::uintmax_t pSource, bool b_Add);

	/*
		Open Microsoft RIFF Palette (*.PAL) file
		- if b_Add is true, the palette data is appended to the current palette data
		  otherwise, the current palette is completely replaced with new data
	*/
	bool OpenPAL(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_Add = false);

	/*
		Save Texture Image (*.TIM) file
		- if b_WritePalette is true, the palette data is written to the file
		- if b_WritePixels is true, the pixel data is written to the file
		- if neither b_WritePalette nor b_WritePixels is true, operation is aborted
	*/
	bool SaveTIM(StdFile& File, std::uintmax_t pSource, bool b_WritePalette, bool b_WritePixels);

	/*
		Save Texture Image (*.TIM) file
		- if b_WritePalette is true, the palette data is written to the file
		- if b_WritePixels is true, the pixel data is written to the file
		- if b_Truncate is true, the file is truncated to the total size of the TIM data
		- if neither b_WritePalette nor b_WritePixels is true, operation is aborted
	*/
	bool SaveTIM(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_WritePalette = true, bool b_WritePixels = true, bool b_Truncate = true);

	/*
		Save Texture Image CLUT (*.CLT) file
	*/
	bool SaveCLT(StdFile& File, std::uintmax_t pSource);

	/*
		Save Texture Image CLUT (*.CLT) file
		- if b_Truncate is true, the file is truncated to the total size of the CLT data
	*/
	bool SaveCLT(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_Truncate = true);

	/*
		Save Texture Image pixel (*.PXL) file
	*/
	bool SavePXL(StdFile& File, std::uintmax_t pSource);

	/*
		Save Texture Image pixel (*.PXL) file
		- if b_Truncate is true, the file is truncated to the total size of the PXL data
	*/
	bool SavePXL(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_Truncate = true);

	/*
		Save Microsoft RIFF Palette (*.PAL) file
	*/
	bool SavePAL(StdFile& File, std::uintmax_t pSource);

	/*
		Save Microsoft RIFF Palette (*.PAL) file
		- if b_Truncate is true, the file is truncated to the total size of the CLT data
	*/
	bool SavePAL(std::filesystem::path Path, std::uintmax_t pSource = 0, bool b_Truncate = true);

	/*
		Create Texture Image (*.TIM) file
	*/
	bool Create(Sony_Texture_Create_Ex ExInfo);

	/*
		Create Texture Image (*.TIM) file
	*/
	bool Create(std::uint32_t Depth, std::uint16_t Width, std::uint16_t Height, std::uint16_t nPalette);

	/*
		Search for Texture Image (*.TIM) files
		- ProgressCallback: search progress percentage (0..1.0f) and reference to execution status (true/false)
		-> initialize ProgressCallback with execution status set to true
		-> if execution status is set to false during operation, the search is aborted
		- OnComplete: file name and vector of pairs (file pointer, total file size)
	*/
	void Search(StdFile& File, std::uintmax_t pSource,
		std::function<void(float, bool&)> ProgressCallback, std::function<void(std::filesystem::path, std::vector<std::pair<std::uintmax_t, std::uintmax_t>>&)> OnComplete);

	/*
		Search for Texture Image (*.TIM) files
		- ProgressCallback: search progress percentage (0..1.0f) and reference to execution status (true/false)
		-> initialize ProgressCallback with execution status set to true
		- if execution status is set to false during operation, the search is aborted
		- OnComplete: file name and vector of pairs (file pointer, total file size)
	*/
	void Search(std::filesystem::path Path, std::uintmax_t pSource,
		std::function<void(float, bool&)> ProgressCallback, std::function<void(std::filesystem::path, std::vector<std::pair<std::uintmax_t, std::uintmax_t>>&)> OnComplete);

	/*
		Update Standard Image Palette
	*/
	void UpdateImagePalette(std::unique_ptr<Standard_Image>& Image, std::uint16_t iPalette = 0);

	/*
		Export Standard Image Object
	*/
	std::unique_ptr<Standard_Image> ExportImage(std::uint16_t iPalette = 0);

	/*
		Import Standard Image Object
	*/
	bool ImportImage(std::unique_ptr<Standard_Image>& Image);

	/*
		Save Bitmap Graphic (*.BMP) file
	*/
	bool SaveBMP(std::filesystem::path Filename, std::uintmax_t pSource = 0, std::uint16_t iPalette = 0, bool b_Truncate = true) { return ExportImage(iPalette)->SaveBMP(Filename, pSource, b_Truncate); }

	/*
		Open Bitmap Graphic (*.BMP) file
	*/
	bool OpenBMP(std::filesystem::path Path, std::uintmax_t pSource = 0);

#ifdef LIB_PNG
	/*
		Save Portable Network Graphics (*.PNG) file
	*/
	bool SavePNG(std::filesystem::path Filename, std::uintmax_t pSource = 0, std::uint16_t iPalette = 0, bool b_Truncate = true) { return ExportImage(iPalette)->SavePNG(Filename, pSource, b_Truncate); }

	/*
		Open Portable Network Graphics (*.PNG) file
	*/
	bool OpenPNG(std::filesystem::path Path, std::uintmax_t pSource = 0);
#endif

#ifdef LIB_JPEG
	/*
		Save Joint Photographic Experts Group (*.JPG) file
	*/
	bool SaveJPG(std::filesystem::path Filename, std::uintmax_t pSource = 0, std::uint16_t iPalette = 0, bool b_Truncate = true) { return ExportImage(iPalette)->SaveJPG(Filename, pSource, b_Truncate); }

	/*
		Open Joint Photographic Experts Group (*.JPG) file
	*/
	bool OpenJPG(std::filesystem::path Path, std::uintmax_t pSource = 0);
#endif

};