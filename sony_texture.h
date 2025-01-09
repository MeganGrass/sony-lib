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
*		Resize pixels when adjusting width and height
*
*/


#pragma once

#include <std_basic_fstream.h>

#include <std_string.h>

#include <std_image.h>

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


struct Sony_Texture_Clut
{
	std::uint32_t Size;				// Size of CLUT Data
	std::int16_t X;					// X Coordinate of Palette[][] in Frame Buffer
	std::int16_t Y;					// Y Coordinate of Palette[][] in Frame Buffer
	std::uint16_t nColor;			// Color Amount in each Palette Table
	std::uint16_t nPalette;			// Palette Table Amount
};


struct Sony_Texture_Data
{
	std::uint32_t Size;				// Size of Pixel Data
	std::int16_t X;					// X Coordinate of Pixel Data in Frame Buffer
	std::int16_t Y;					// Y Coordinate of Pixel Data in Frame Buffer
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

	// ABGR Pixel Data
	std::vector<std::uint8_t> Pixels;

	// Semitransparency Processing Flag (STP) Color
	static DWORD TransparentColor;

	// Semitransparency Processing Flag (STP) 4bpp/8bpp
	bool b_STP4Bpp;

	// Semitransparency Processing Flag (STP) 16bpp
	bool b_STP16Bpp;

	// Superimposed Transparency
	bool b_TransparencySuperimposed;

	// Flag
	bool b_Open;

public:

	/*
		Construction
	*/
	explicit Sony_PlayStation_Texture(std::filesystem::path Path, std::uintmax_t _Ptr = 0) :
		Header{},
		Clut{},
		Data{},
		Palette(),
		Pixels(),
		b_STP4Bpp(true),
		b_STP16Bpp(false),
		b_TransparencySuperimposed(false),
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
		b_STP4Bpp(true),
		b_STP16Bpp(false),
		b_TransparencySuperimposed(false),
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
		b_STP4Bpp(true),
		b_STP16Bpp(false),
		b_TransparencySuperimposed(false),
		b_Open(false)
	{
	}

	virtual ~Sony_PlayStation_Texture(void)
	{
		Close();
	}

	/*
		Check if the texture is open
	*/
	bool operator !() { return !b_Open; }

	/*
		Check if the texture is open
	*/
	bool IsOpen(void) const noexcept { return b_Open; }

	/*
		Texture information
	*/
	String About(void);

	/*
		Print texture information
	*/
	String Print(void);

	/*
		Create
	*/
	bool Create(std::uint32_t _Depth, std::uint16_t _Width, std::uint16_t _Height, std::uint16_t nPalette);

	/*
		Open
	*/
	std::uintmax_t Open(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open
	*/
	bool Open(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Save
	*/
	std::uintmax_t Save(StdFile& File, std::uintmax_t _Ptr);

	/*
		Save
	*/
	bool Save(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Search
		 - pair: <Position, Size>
	*/
	std::vector<std::pair<std::uintmax_t, std::uintmax_t>> Search(StdFile& File, std::uintmax_t _Ptr);

	/*
		Search
		 - pair: <Position, Size>
	*/
	std::vector<std::pair<std::uintmax_t, std::uintmax_t>> Search(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Close
	*/
	void Close(void);

	/*
		Get file size
	*/
	[[nodiscard]] std::size_t Size(void) const;

	/*
		Get depth
	*/
	[[nodiscard]] std::uint32_t GetDepth(void) const;

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
		Get CLUT amount
	*/
	[[nodiscard]] std::uint32_t GetClutSize(void) const { return Clut.nPalette; }

	/*
		Get CLUT X coordinate
	*/
	[[nodiscard]] std::int16_t& GetClutX(void) { return Clut.X; }

	/*
		Get CLUT Y coordinate
	*/
	[[nodiscard]] std::int16_t& GetClutY(void) { return Clut.Y; }

	/*
		Get data X coordinate
	*/
	[[nodiscard]] std::int16_t& GetDataX(void) { return Data.X; }

	/*
		Get data Y coordinate
	*/
	[[nodiscard]] std::int16_t& GetDataY(void) { return Data.Y; }

	/*
		Get width
	*/
	[[nodiscard]] std::uint32_t GetWidth(void) const;

	/*
		Set width
	*/
	bool SetWidth(std::uint16_t _Width);

	/*
		Get height
	*/
	[[nodiscard]] std::uint32_t GetHeight(void) const;

	/*
		Set height
	*/
	bool SetHeight(std::uint16_t _Height);

	/*
		Get pixel data
	*/
	[[nodiscard]] std::vector<std::uint8_t>& GetPixels(void) { return Pixels; }

	/*
		Get palette
	*/
	[[nodiscard]] std::vector<std::vector<Sony_Texture_16bpp>>& GetPalette(void) { return Palette; }

	/*
		Get palette
		 - palette is not modified
		 - convert from 4bpp to 8bpp and vice-versa
		 - converting from 8bpp to 4bpp will result in loss of color (256 -> 16), first 16 colors are preserved
	*/
	[[nodiscard]] std::vector<std::vector<Sony_Texture_16bpp>> GetPalette(std::uint32_t _Depth);

	/*
		Convert unsigned char vector to palette
		 - palette is not modified
		 - 256 max palettes read from source
		 - convert from 4bpp to 8bpp and vice-versa
		 - converting from 8bpp to 4bpp will result in loss of color (256 -> 16), first 16 colors are preserved
	*/
	[[nodiscard]] std::vector<std::vector<Sony_Texture_16bpp>> ConvertToPalette(std::vector<std::uint8_t> Source) const;

	/*
		Get/Set transparent color
	*/
	[[nodiscard]] DWORD& GetTransparentColor(void) { return TransparentColor; }

	/*
		Enable or disable Semitransparency Processing (STP Flag) for 4bpp and 8bpp textures
		- when enabled, solid black pixels in 4bpp and 8bpp textures are replaced with Mask/TransparentColor
		- enabled by default
	*/
	bool& STP4Bpp(void) noexcept { return b_STP4Bpp; }

	/*
		Enable or disable Semitransparency Processing (STP Flag) for 16bpp textures
		- when enabled, solid black pixels in 16bpp textures are replaced with Mask/TransparentColor
		- disabled by default
	*/
	bool& STP16Bpp(void) noexcept { return b_STP16Bpp; }

	/*
		Enable or disable Superimposed Transparency
		- when enabled, "TransparentColor" is always set to palette index (0)
		- disabled by default
	*/
	bool& TransparencySuperimposed(void) noexcept { return b_TransparencySuperimposed; }

	/*
		Add palette
	*/
	void AddPalette(void);

	/*
		Export all palettes as raw data
	*/
	void ExportPalette(std::filesystem::path _Filename);

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
	bool ImportPalette(StdFile& File, std::uintmax_t _Ptr);

	/*
		Import palette from file
		 - entire palette is completely replaced
	*/
	bool ImportPalette(std::filesystem::path Filename, std::uintmax_t _Ptr);

	/*
		Import palette from file
		 - single palette is imported to iClut index
	*/
	bool ImportMicrosoftPalette(std::filesystem::path Filename, std::size_t iClut);

	/*
		Export palette from file
		 - single palette is exported from iClut index
	*/
	bool ExportMicrosoftPalette(std::filesystem::path Filename, std::size_t iClut);

	/*
		Delete palette
		 - delete palette at specified index
		 - cannot delete single-only palette (eg, must have at least 1 palette)
	*/
	void DeletePalette(std::size_t iClut);

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
	std::uintmax_t ImportPixels(StdFile& File, std::uintmax_t _Ptr, std::uint32_t _PixelCount);

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
		Get 4bpp pixel
	*/
	[[nodiscard]] Sony_Texture_4bpp Get4bpp(std::size_t iPixel) { return *reinterpret_cast<Sony_Texture_4bpp*>(&Pixels.data()[iPixel]); }

	/*
		Get 8bpp pixel
	*/
	[[nodiscard]] Sony_Texture_8bpp Get8bpp(std::size_t iPixel) { return *reinterpret_cast<Sony_Texture_8bpp*>(&Pixels.data()[iPixel]); }

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
	[[nodiscard]] Sony_Texture_16bpp Create16bpp(std::uint8_t R, std::uint8_t G, std::uint8_t B, bool STP);

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
	[[nodiscard]] Sony_Texture_24bpp Get24bpp(std::size_t X, std::size_t Y) { return *reinterpret_cast<Sony_Texture_24bpp*>(&Pixels.data()[(Y * GetWidth() + X) * sizeof(Sony_Texture_24bpp)]); }

	/*
		Update Standard Image Palette
	*/
	void UpdateBitmapPalette(std::unique_ptr<Standard_Image>& Image, std::size_t iClut = 0, DWORD Mask = TransparentColor);

	/*
		Get Standard Image Object
	*/
	std::unique_ptr<Standard_Image> GetBitmap(std::size_t iClut = 0, DWORD Mask = TransparentColor);

};