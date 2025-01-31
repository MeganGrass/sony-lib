/*
*
*	Megan Grass
*	March 07, 2024
*
*
*	TODO: 
* 
*		Function to write RGB pixels
*
*/


#pragma once

#include <std_basic_fstream.h>

#include <std_string.h>

#include <std_image.h>

#include <functional>

#ifdef DeletePalette
#undef DeletePalette
#endif

#define MAX_TIM_PALETTE	256			// Maximum palette count for Sony PlayStation Texture Image (*TIM) files


#pragma pack(push, 1)


struct Sony_Texture_Header
{
	std::uint32_t ID : 8;			// 0x10
	std::uint32_t Version : 8;		// Always zero (0)
	std::uint32_t Reserved0 : 16;	// Always zero (0)
	std::uint32_t Mode : 3;			// 0 = 4bpp, 1 = 8bpp, 2 = 16bpp, 3 = 24bpp, 4 = Mixed
	std::uint32_t ClutFlag : 1;		// 0 = No CLUT, 1 = CLUT
	std::uint32_t Reserved1 : 28;	// Always zero (0)
};


struct Sony_Texture_Clut_File
{
	std::uint32_t ID : 8;			// 0x11
	std::uint32_t Version : 8;		// Always zero (0)
	std::uint32_t Reserved0 : 16;	// Always zero (0)
	std::uint32_t Mode : 3;			// Always 2 (16bpp)
	std::uint32_t Reserved1 : 29;	// Always zero (0)
};


struct Sony_Texture_Data_File
{
	std::uint32_t ID : 8;			// 0x12
	std::uint32_t Version : 8;		// Always zero (0)
	std::uint32_t Reserved0 : 16;	// Always zero (0)
	std::uint32_t Mode : 3;			// 0 = 4bpp, 1 = 8bpp, 2 = 16bpp, 3 = 24bpp, 4 = Mixed
	std::uint32_t Reserved1 : 29;	// Always zero (0)
};


struct Sony_Texture_Clut
{
	std::uint32_t Size;				// Size of CLUT Data
	std::uint16_t X;				// X Coordinate of Palette[][] in Frame Buffer
	std::uint16_t Y;				// Y Coordinate of Palette[][] in Frame Buffer
	std::uint16_t nColor;			// Color Amount in each Palette Table
	std::uint16_t nPalette;			// Palette Table Amount
};


struct Sony_Texture_Data
{
	std::uint32_t Size;				// Size of Pixel Data
	std::uint16_t X;				// X Coordinate of Pixel Data in Frame Buffer
	std::uint16_t Y;				// Y Coordinate of Pixel Data in Frame Buffer
	std::uint16_t Width;			// Texture Width
	std::uint16_t Height;			// Texture Height
};


struct Sony_Texture_4bpp
{
	std::uint8_t Pix0 : 4;			// Clut number 0
	std::uint8_t Pix1 : 4;			// Clut number 1
	std::uint8_t Pix2 : 4;			// Clut number 2
	std::uint8_t Pix3 : 4;			// Clut number 3
};


struct Sony_Texture_8bpp
{
	std::uint8_t Pix0 : 8;			// Clut number 0
	std::uint8_t Pix1 : 8;			// Clut number 1
};


struct Sony_Texture_16bpp
{
	std::uint16_t R : 5;			// Red
	std::uint16_t G : 5;			// Green
	std::uint16_t B : 5;			// Blue
	std::uint16_t STP : 1;			// Semitransparency Processing Flag
};


struct Sony_Texture_24bpp
{
	std::uint8_t R0 : 8;			// Red
	std::uint8_t G0 : 8;			// Green
	std::uint8_t B0 : 8;			// Blue
	std::uint8_t R1 : 8;			// Red
	std::uint8_t G1 : 8;			// Green
	std::uint8_t B1 : 8;			// Blue
};


enum class Sony_Texture_Transparency : int
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


#pragma pack(pop)


class Sony_PlayStation_Texture {
private:

	// File Header
	Sony_Texture_Header Header;

	// Palette Header
	Sony_Texture_Clut Clut;

	// Pixel Header
	Sony_Texture_Data Data;

	// Palette
	std::vector<std::vector<Sony_Texture_16bpp>> Palette;

	// Pixels
	std::vector<std::uint8_t> Pixels;

	// Superblack Transparency
	bool b_TransparencySuperblack;

	// Superimposed Transparency
	bool b_TransparencySuperimposed;

	// Transparency for STP flag only
	bool b_TransparencySTP;

	// Semi-Transparency (50%back + 50%texture)
	bool b_TransparencyHalf;

	// Semi-Transparency (100%back + 100%texture)
	bool b_TransparencyFull;

	// Semi-Transparency (100%back - 100%texture)
	bool b_TransparencyInverse;

	// Semi-Transparency (100%back + 25%texture)
	bool b_TransparencyQuarter;

	// Transparency (external color)
	bool b_Transparency;

	// Transparency Color (external)
	static DWORD m_TransparentColor;

	// Transparency Flags
	Sony_Texture_Transparency m_Transparency;

	// Search Progress
	float m_SearchProgress;

	// Flag
	bool b_Open;

public:

	// Standard String
	Standard_String Str;

	/*
		Construction
	*/
	explicit Sony_PlayStation_Texture(std::filesystem::path Path, std::uintmax_t _Ptr = 0) :
		Header{},
		Clut{},
		Data{},
		Palette(),
		Pixels(),
		b_TransparencySuperblack(false),
		b_TransparencySuperimposed(false),
		b_TransparencySTP(false),
		b_TransparencyHalf(false),
		b_TransparencyFull(true),
		b_TransparencyInverse(false),
		b_TransparencyQuarter(false),
		b_Transparency(false),
		m_Transparency(Sony_Texture_Transparency::None),
		m_SearchProgress(0.0f),
		b_Open(false)
	{
		Open(Path, _Ptr);
	}

	explicit Sony_PlayStation_Texture(std::uint32_t _Depth, std::uint16_t _Width, std::uint16_t _Height, std::uint16_t nPalette) :
		Header{},
		Clut{},
		Data{},
		Palette(),
		Pixels(),
		b_TransparencySuperblack(false),
		b_TransparencySuperimposed(false),
		b_TransparencySTP(false),
		b_TransparencyHalf(false),
		b_TransparencyFull(true),
		b_TransparencyInverse(false),
		b_TransparencyQuarter(false),
		b_Transparency(false),
		m_Transparency(Sony_Texture_Transparency::None),
		m_SearchProgress(0.0f),
		b_Open(false)
	{
		Create(_Depth, _Width, _Height, nPalette);
	}

	explicit Sony_PlayStation_Texture(void) :
		Header{},
		Clut{},
		Data{},
		Palette(),
		Pixels(),
		b_TransparencySuperblack(false),
		b_TransparencySuperimposed(false),
		b_TransparencySTP(false),
		b_TransparencyHalf(false),
		b_TransparencyFull(true),
		b_TransparencyInverse(false),
		b_TransparencyQuarter(false),
		b_Transparency(false),
		m_Transparency(Sony_Texture_Transparency::None),
		m_SearchProgress(0.0f),
		b_Open(false)
	{
	}

	~Sony_PlayStation_Texture(void) = default;

	/*
		Is the texture is open?
	*/
	bool operator !() { return !b_Open; }

	/*
		Is the texture is open?
	*/
	bool IsOpen(void) const noexcept { return b_Open; }

	/*
		Does the texture have either palette or pixel data?
		 - one or the other is required for saving
	*/
	bool IsValid(void) const;

	/*
		Texture information
	*/
	String About(void);

	/*
		Print texture information
	*/
	String Print(void);

	/*
		Update Transparency flags
	*/
	bool UpdateTransparency(void);

	/*
		Create
	*/
	bool Create(std::uint32_t _Depth, std::uint16_t _Width, std::uint16_t _Height, std::uint16_t nPalette);

	/*
		Open from file
		- TIM is read from the file at the specified pointer
	*/
	std::uintmax_t Open(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open from file
		- TIM is read from the file at the specified pointer
	*/
	bool Open(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Save to file
		- TIM is written to the file at the specified pointer
	*/
	std::uintmax_t Save(StdFile& File, std::uintmax_t _Ptr);

	/*
		Save to file
		- TIM is written to the file at the specified pointer
	*/
	bool Save(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Search for TIM files
		 - pair: <Position, Size>
	*/
	std::vector<std::pair<std::uintmax_t, std::uintmax_t>> Search(StdFile& File, std::uintmax_t _Ptr, std::function<void(float)> ProgressCallback);

	/*
		Search for TIM files
		 - pair: <Position, Size>
	*/
	std::vector<std::pair<std::uintmax_t, std::uintmax_t>> Search(std::filesystem::path Path, std::uintmax_t _Ptr, std::function<void(float)> ProgressCallback);

	/*
		Get search progress
	*/
	[[nodiscard]] float GetSearchProgress(void) const noexcept { return m_SearchProgress; }

	/*
		Open palette/s from file
		- CLT is read from the file at the specified pointer
		- if b_Add is true, palettes are added to the back in sequential order
		- if b_Add is false, entire palette is replaced
	*/
	std::uintmax_t OpenCLT(StdFile& File, std::uintmax_t _Ptr, bool b_Add);

	/*
		Close
		- clear all data and declare texture closed
	*/
	void Close(void);

	/*
		Get total file size
	*/
	[[nodiscard]] std::size_t Size(void) const;

	/*
		Get bits per pixel
	*/
	[[nodiscard]] std::uint16_t GetDepth(void) const;

	/*
		Get clut flag
		- false (CLUT not present), true (CLUT present)
	*/
	[[nodiscard]] bool GetCF(void) const { return Header.ClutFlag; }

	/*
		Set clut flag
		- false (CLUT not present), true (CLUT present)
	*/
	[[nodiscard]] void SetCF(bool ClutFlag) { Header.ClutFlag = ClutFlag; }

	/*
		Get/Set CLUT VRAM X coordinate
	*/
	[[nodiscard]] std::uint16_t& ClutX(void) { return Clut.X; }

	/*
		Get/Set CLUT VRAM Y coordinate
	*/
	[[nodiscard]] std::uint16_t& ClutY(void) { return Clut.Y; }

	/*
		Get/Set pixel VRAM X coordinate
	*/
	[[nodiscard]] std::uint16_t& DataX(void) { return Data.X; }

	/*
		Get/Set pixel VRAM Y coordinate
	*/
	[[nodiscard]] std::uint16_t& DataY(void) { return Data.Y; }

	/*
		Get max amount of colors per CLUT
	*/
	[[nodiscard]] std::uint16_t GetClutColorMax(void) const { return GetDepth() == 4 ? 16 : 256; }

	/*
		Get total CLUT count
	*/
	[[nodiscard]] std::uint16_t GetClutCount(void) const { return (std::uint16_t)Palette.size(); }

	/*
		Get max CLUT index
	*/
	[[nodiscard]] std::uint16_t GetClutMax(void) const { return Palette.empty() ? 0 : (std::uint16_t)Palette.size() - 1; }

	/*
		Get raw CLUT width
	*/
	[[nodiscard]] std::uint16_t GetPaletteWidth(void) const;

	/*
		Get raw CLUT height
	*/
	[[nodiscard]] std::uint16_t GetPaletteHeight(void) const;

	/*
		Get file size of all raw palettes
	*/
	[[nodiscard]] std::uint16_t GetPaletteDataSize(void) const;

	/*
		Get pixel width
	*/
	[[nodiscard]] std::uint16_t GetWidth(void) const;

	/*
		Set pixel width
	*/
	bool SetWidth(std::uint16_t _Width);

	/*
		Get pixel height
	*/
	[[nodiscard]] std::uint16_t GetHeight(void) const;

	/*
		Set pixel height
	*/
	bool SetHeight(std::uint16_t _Height);

	/*
		Auto-update "Size" field of Data Header
	*/
	void UpdateDataSize(void);

	/*
		Enable or disable Superblack Transparency
		- when enabled, solid black pixels are semi/fully transparent
		- disabled by default
	*/
	[[nodiscard]] bool& TransparencySuperblack(void) noexcept { return b_TransparencySuperblack; }

	/*
		Enable or disable Superimposed Transparency
		- when enabled, color entry at palette index (0) is semi/fully transparent
		- disabled by default
	*/
	[[nodiscard]] bool& TransparencySuperimposed(void) noexcept { return b_TransparencySuperimposed; }

	/*
		Enable or disable STP Transparency
		- when enabled, STP flag determines if Semi-Transparency is used
		- disabled by default
	*/
	[[nodiscard]] bool& TransparencySTP(void) noexcept { return b_TransparencySTP; }

	/*
		Enable or disable Half Transparency
		- when enabled, 50%back + 50%texture
		- disabled by default
	*/
	[[nodiscard]] bool& TransparencyHalf(void) noexcept { return b_TransparencyHalf; }

	/*
		Enable or disable Full Transparency
		- when enabled, 100%back + 100%texture
		- enabled by default
	*/
	[[nodiscard]] bool& TransparencyFull(void) noexcept { return b_TransparencyFull; }

	/*
		Enable or disable Inverse Transparency
		- when enabled, 100%back - 100%texture
		- disabled by default
	*/
	[[nodiscard]] bool& TransparencyInverse(void) noexcept { return b_TransparencyInverse; }

	/*
		Enable or disable Quarter Transparency
		- when enabled, 100%back + 25%texture
		- disabled by default
	*/
	[[nodiscard]] bool& TransparencyQuarter(void) noexcept { return b_TransparencyQuarter; }

	/*
		Enable or disable Transparency (external color)
		- when enabled, "Mask/TransparentColor" is semi/fully transparent
		- disabled by default
	*/
	[[nodiscard]] bool& Transparency(void) noexcept { return b_Transparency; }

	/*
		Get/Set transparent color (external)
	*/
	[[nodiscard]] DWORD& TransparentColor(void) { return m_TransparentColor; }

	/*
		Get/Set transparency flags
	*/
	[[nodiscard]] Sony_Texture_Transparency& TransparencyFlags(void) { return m_Transparency; }

	/*
		Get raw pixel data
	*/
	[[nodiscard]] std::vector<std::uint8_t>& GetPixels(void) { return Pixels; }

	/*
		Get raw palette data
	*/
	[[nodiscard]] std::vector<std::vector<Sony_Texture_16bpp>>& GetPalette(void) { return Palette; }

	/*
		Get palette
		 - export to 4bpp to 8bpp and vice-versa (original palette is not modified)
		 - converting from 8bpp to 4bpp will result in loss of color (256 -> 16), first 16 colors are preserved
	*/
	[[nodiscard]] std::vector<std::vector<Sony_Texture_16bpp>> GetPalette(std::uint32_t _Depth);

	/*
		Convert unsigned char vector to palette
		 - read up to 256 max palettes from raw source
		 - export to 4bpp to 8bpp and vice-versa (original palette is not modified)
		 - converting from 8bpp to 4bpp will result in loss of color (256 -> 16), first 16 colors are preserved
	*/
	[[nodiscard]] std::vector<std::vector<Sony_Texture_16bpp>> ConvertToPalette(std::vector<std::uint8_t> Source) const;

	/*
		Convert 16bpp vector to palette
		 - read up to 256 max palettes from raw source
		 - export to 4bpp to 8bpp and vice-versa (original palette is not modified)
		 - converting from 8bpp to 4bpp will result in loss of color (256 -> 16), first 16 colors are preserved
	*/
	[[nodiscard]] std::vector<std::vector<Sony_Texture_16bpp>> ConvertToPalette(std::vector<Sony_Texture_16bpp> Source) const;

	/*
		Move palette
	*/
	bool MovePalette(std::size_t iClut, bool Right);

	/*
		Add palette
	*/
	bool AddPalette(void);

	/*
		Add palette
		 - all palettes are added to the back in sequential order
	*/
	void AddPalette(std::vector<std::vector<Sony_Texture_16bpp>> Source);

	/*
		Add palette from file
		 - if b_RawData is true, _Filename is interpreted as raw data filename
		 - if b_RawData is false, _Filename is interpreted as PlayStation TIM filename
	*/
	bool AddPalette(bool b_RawData, std::filesystem::path _Filename);

	/*
		Add palette from file
	*/
	bool AddPalette(std::filesystem::path _Filename);

	/*
		Export all palettes as raw data
	*/
	void ExportPalette(std::filesystem::path _Filename);

	/*
		Export single palette to raw data
	*/
	void ExportPalette(std::filesystem::path _Filename, std::size_t iClut);

	/*
		Export all palettes to Sony Texture Image file
	*/
	bool ExportPaletteToTIM(std::filesystem::path _Filename);

	/*
		Export single palette to file
	*/
	bool ExportMicrosoftPalette(std::filesystem::path Filename, std::size_t iClut);

	/*
		Insert palette
		 - insert new (blank) palette at specified index
	*/
	void InsertPalette(std::size_t iClut);

	/*
		Import palette
		 - single palette is imported to iClut index
		 - import from either 4bpp or 8bpp
		 - converting from 8bpp to 4bpp will result in loss of color (256 -> 16), first 16 colors are preserved
	*/
	bool ImportPalette(std::vector<Sony_Texture_16bpp> _Palette, std::size_t iClut);

	/*
		Import palette
		 - entire palette is completely replaced
		 - import from either 4bpp or 8bpp
		 - converting from 8bpp to 4bpp will result in loss of color (256 -> 16), first 16 colors are preserved
	*/
	bool ImportPalette(std::vector<std::vector<Sony_Texture_16bpp>> _Palette);

	/*
		Import palette from unsigned char vector
		 - entire palette is completely replaced
	*/
	bool ImportPalette(std::vector<std::uint8_t> Source);

	/*
		Import palette from file
		 - entire palette is completely replaced
	*/
	bool ImportPalette(StdFile& File, std::uintmax_t _Ptr, std::uint16_t nClut);

	/*
		Import palette from file
		 - entire palette is completely replaced
	*/
	bool ImportPalette(std::filesystem::path Filename, std::uintmax_t _Ptr, std::uint16_t nClut);

	/*
		Import palette from file
		 - single palette is imported to iClut index from raw data
	*/
	bool ImportPalette(std::filesystem::path Filename, std::uintmax_t _Ptr, std::size_t iClut);

	/*
		Import palette from Sony Texture Image file
		 - entire palette is completely replaced
	*/
	bool ImportPaletteFromTIM(std::filesystem::path Filename, std::uintmax_t _Ptr = 0);

	/*
		Import palette from Sony Texture Image file
		 - entire palette is completely replaced
	*/
	bool ImportPaletteFromTIM(std::unique_ptr<Sony_PlayStation_Texture>& External);

	/*
		Import single palette from file
	*/
	bool ImportMicrosoftPalette(std::filesystem::path Filename, std::size_t iClut);

	/*
		Delete palette
		 - delete palette at specified index
	*/
	bool DeletePalette(std::size_t iClut);

	/*
		Delete all palettes
	*/
	void DeleteAllPalettes(void);

	/*
		Delete all pixels
	*/
	void DeleteAllPixels(void);

	/*
		Export pixels to raw data
	*/
	bool ExportPixels(std::filesystem::path Filename);

	/*
		Export pixels to Sony Texture Image file
	*/
	bool ExportPixelsToTIM(std::filesystem::path Filename);
	/*
		Import pixels from unsigned char vector
	*/
	bool ImportPixels(std::vector<std::uint8_t> Source);

	/*
		Import pixels from unsigned char vector
	*/
	bool ImportPixels(std::vector<std::uint8_t> Source, std::size_t Destination);

	/*
		Import pixels from file
	*/
	bool ImportPixels(StdFile& File, std::uintmax_t _Ptr, std::uint32_t _PixelCount);

	/*
		Import pixels from file
	*/
	bool ImportPixels(std::filesystem::path Filename, std::uintmax_t _Ptr);

	/*
		Import pixels from Sony Texture Image file
	*/
	bool ImportPixelsFromTIM(std::filesystem::path Filename, std::uintmax_t _Ptr, bool b_UpdateDepth = false);

	/*
		Import pixels from Sony Texture Image file
	*/
	bool ImportPixelsFromTIM(std::unique_ptr<Sony_PlayStation_Texture>& External, bool b_UpdateDepth);

	/*
		Get red color from 16bpp pixel/palette entry
	*/
	[[nodiscard]] std::uint8_t Red(Sony_Texture_16bpp Color) { return ((Color.R << 3) | (Color.R >> 2)); }

	/*
		Get green color from 16bpp pixel/palette entry
	*/
	[[nodiscard]] std::uint8_t Green(Sony_Texture_16bpp Color) { return ((Color.G << 3) | (Color.G >> 2)); }

	/*
		Get blue color from 16bpp pixel/palette entry
	*/
	[[nodiscard]] std::uint8_t Blue(Sony_Texture_16bpp Color) { return ((Color.B << 3) | (Color.B >> 2)); }

	/*
		Get Semitransparency Processing Flag from 16bpp pixel/palette entry
	*/
	[[nodiscard]] bool STP(Sony_Texture_16bpp Color) { return Color.STP; }

	/*
		Get/Set 4bpp pixel
	*/
	[[nodiscard]] Sony_Texture_4bpp& Get4bpp(std::size_t iPixel) { return *reinterpret_cast<Sony_Texture_4bpp*>(&Pixels.data()[iPixel]); }

	/*
		Get/Set 4bpp pixel
	*/
	[[nodiscard]] Sony_Texture_4bpp& Get4bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_4bpp*>(&Pixels.data()[(Y * (GetWidth() / 2)) + (X / 2)]); }

	/*
		Get/Set 8bpp pixel
	*/
	[[nodiscard]] Sony_Texture_8bpp& Get8bpp(std::size_t iPixel) { return *reinterpret_cast<Sony_Texture_8bpp*>(&Pixels.data()[iPixel]); }

	/*
		Get/Set 8bpp pixel
	*/
	[[nodiscard]] Sony_Texture_8bpp& Get8bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_8bpp*>(&Pixels.data()[((Y * GetWidth()) + X)]); }

	/*
		Get 16bpp pixel
	*/
	[[nodiscard]] Sony_Texture_16bpp Get16bpp(std::size_t iPixel) { return *reinterpret_cast<Sony_Texture_16bpp*>(&Pixels.data()[iPixel]); }

	/*
		Get 16bpp pixel
	*/
	[[nodiscard]] Sony_Texture_16bpp Get16bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_16bpp*>(&Pixels.data()[(Y * GetWidth() + X) * sizeof(Sony_Texture_16bpp)]); }

	/*
		Create 16bpp color
	*/
	[[nodiscard]] Sony_Texture_16bpp Create16bpp(std::uint8_t R, std::uint8_t G, std::uint8_t B, bool STP) { return { std::uint16_t(R >> 3), std::uint16_t(G >> 3), std::uint16_t(B >> 3), STP }; }

	/*
		Create 16bpp color
	*/
	[[nodiscard]] Sony_Texture_16bpp Create16bpp(DWORD Color, bool STP) { return Create16bpp(GetRValue(Color), GetGValue(Color), GetBValue(Color), STP); }

	/*
		Get 24bpp pixel
	*/
	[[nodiscard]] Sony_Texture_24bpp Get24bpp(std::uint32_t iPixel) { return *reinterpret_cast<Sony_Texture_24bpp*>(&Pixels[iPixel]); }

	/*
		Get 24bpp pixel
	*/
	[[nodiscard]] Sony_Texture_24bpp Get24bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_24bpp*>(&Pixels.data()[(Y * ((GetWidth() * 24 + 7) / 8)) + (X * (24 / 8))]); }

	/*
		Create 24bpp pixel
	*/
	[[nodiscard]] Sony_Texture_24bpp Create24bpp(std::uint8_t R0, std::uint8_t G0, std::uint8_t B0, std::uint8_t R1, std::uint8_t G1, std::uint8_t B1) { return { R0, G0, B0, R1, G1, B1 }; }

	/*
		Create 24bpp pixel
	*/
	[[nodiscard]] Sony_Texture_24bpp Create24bpp(DWORD Color0, DWORD Color1) { return { GetRValue(Color0), GetGValue(Color0), GetBValue(Color0), GetRValue(Color1), GetGValue(Color1), GetBValue(Color1) }; }

	/*
		Set Pixel (4bpp)
	*/
	void SetPixel(uint32_t X, uint32_t Y, Sony_Texture_4bpp Color) { *reinterpret_cast<Sony_Texture_4bpp*>(&Pixels.data()[(Y * (GetWidth() / 2)) + (X / 2)]) = Color; }

	/*
		Set Pixel (8bpp)
	*/
	void SetPixel(uint32_t X, uint32_t Y, Sony_Texture_8bpp Color) { *reinterpret_cast<Sony_Texture_8bpp*>(&Pixels.data()[((Y * GetWidth()) + X)]) = Color; }

	/*
		Set Pixel (16bpp)
	*/
	void SetPixel(uint32_t X, uint32_t Y, Sony_Texture_16bpp Color) { *reinterpret_cast<Sony_Texture_16bpp*>(&Pixels.data()[(Y * GetWidth() + X) * sizeof(Sony_Texture_16bpp)]) = Color; }

	/*
		Set Pixel (24bpp)
	*/
	void SetPixel(uint32_t X, uint32_t Y, Sony_Texture_24bpp Color) { *reinterpret_cast<Sony_Texture_24bpp*>(&Pixels.data()[(Y * ((GetWidth() * 24 + 7) / 8)) + (X * (24 / 8))]) = Color; }

	/*
		Update Standard Image Palette
	*/
	void UpdateBitmapPalette(std::unique_ptr<Standard_Image>& Image, std::size_t iClut = 0, DWORD Mask = m_TransparentColor);

	/*
		Get Standard Image Object
	*/
	std::unique_ptr<Standard_Image> GetBitmap(std::size_t iClut = 0, DWORD Mask = m_TransparentColor);

	/*
		Save As Bitmap
	*/
	bool SaveAsBitmap(std::filesystem::path Filename, std::size_t iClut = 0) { return GetBitmap(iClut)->SaveAsBitmap(Filename); }

};