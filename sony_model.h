/*
*
*	Megan Grass
*	March 07, 2024
*
*
*	TODO:
*
*/


#pragma once

#include <std_basic_fstream.h>

#include <std_text.h>

#include "gte/lib_gte.h"


#pragma pack(push, 1)


struct Sony_PlayStation_Model_Header
{
	std::uint32_t Magic;				// 0x00	// 0x41
	struct
	{
		std::uint32_t Address : 1;		// 0 = Relative Pointers, 1 = Absolute Pointers
		std::uint32_t Reserved : 30;	// Zero (0)
	} Flag;				// 0x04	// Relative or Absolute Offsets?
	std::uint32_t nObject;				// 0x08	// Object count
};


struct Sony_PlayStation_Model_Index
{
	std::uint32_t pVertice;				// 0x00	// Vertice pointer
	std::uint32_t nVertice;				// 0x04	// Vertice count
	std::uint32_t pNormal;				// 0x08	// Normal pointer
	std::uint32_t nNormal;				// 0x0C	// Normal count
	std::uint32_t pPrimitive;			// 0x10	// Primitive pointer
	std::uint32_t nPrimitive;			// 0x14	// Primitive count
	std::int32_t Scale;					// 0x18	// Scale factor
};


struct Sony_PlayStation_Model_Primitive_Header
{
	std::uint32_t olen : 8;				// 2D drawing primitive size
	std::uint32_t ilen : 8;				// Packet size (actual = ((ilen * 4) + 4))
	std::uint32_t Light : 1;			// 0 = On, 1 = Off
	std::uint32_t Face : 1;				// 0 = Single-faced polygon, 1 = Double-faced polygon
	std::uint32_t Gradation : 1;		// 0 = Solid-color, 1 = Gradation
	std::uint32_t Reserved : 5;			// Zero (0)
	std::uint32_t Brightness : 1;		// 0 = On, 1 = Off
	std::uint32_t Translucency : 1;		// 0 = Off, 1 = On
	std::uint32_t Texture : 1;			// 0 = Off, 1 = On
	std::uint32_t Quadrilateral : 1;	// 0 = Triangle, 1 = Quadrangle
	std::uint32_t Shader : 1;			// 0 = Flat, 1 = Gouraud
	std::uint32_t Code : 3;				// 001 = Polygon (Triangle or Quadrangle), 010 = Line, 011 = Sprite
};


struct Sony_PlayStation_Model_Texture
{
	std::uint16_t Page : 5;				// Texture page (0 to 31)
	std::uint16_t SemiTrans : 2;		// Semi-transparency (valid if Translucency is enabled)
	std::uint16_t ColorMode : 2;		// 00 = 4-bit, 01 = 8-bit, 10 = 15-bit, 11 = 24-bit
	std::uint16_t Reserved : 7;			// Zero (0)
};


struct Sony_PlayStation_Model_Clut
{
	std::uint16_t X : 6;				// X coordinate in VRAM
	std::uint16_t Y : 9;				// Y coordinate in VRAM
	std::uint16_t Reserved : 1;			// Zero (0)
};


#pragma pack(pop)


enum class Sony_PlayStation_Primitive_Type : int
{
	Polygon = 1,
	Line = 2,
	Sprite = 3,
	Unknown = -1,
};


enum class Sony_PlayStation_Transparency_Rate : int
{
	Half = 0,							// 50%back + 50%polygon
	Full = 1,							// 100%back + 100%polygon
	Inverse = 2,						// 100%back - 100%polygon
	Quarter = 3							// 100%back + 25%polygon
};


struct Sony_PlayStation_Model_Primitive
{
	Sony_PlayStation_Model_Primitive_Header Header{};
	std::vector<std::uint8_t> Packet;
};


struct Sony_PlayStation_Model_Object
{
	std::int32_t Scale{};
	std::vector<SVECTOR> Vertice;
	std::vector<SVECTOR> Normal;
	std::vector<Sony_PlayStation_Model_Primitive> Primitive;
};


class Sony_PlayStation_Model {
private:

	// Sony PlayStation Model Object
	std::vector<Sony_PlayStation_Model_Object> Object;

	// Flag
	bool b_Open;
	bool b_IgnoreMagic;

public:

	/*
		Construction
	*/
	explicit Sony_PlayStation_Model(std::filesystem::path Path) :
		b_Open(false),
		b_IgnoreMagic(false)
	{
		Open(Path);
	}
	explicit Sony_PlayStation_Model(void) :
		b_Open(false),
		b_IgnoreMagic(false)
	{
	}
	virtual ~Sony_PlayStation_Model(void)
	{
		Close();
	}

	/*
		Check if the model is open
	*/
	bool operator !() { return !b_Open; }

	/*
		Check if the model is open
	*/
	bool IsOpen(void) const noexcept { return b_Open; }

	/*
		Force open if object container is not empty
	*/
	bool ForceOpen(void) noexcept { if (!Object.empty()) { b_Open = true; } return b_Open; }

	/*
		Ignore magic number in header when opening file
	*/
	void IgnoreMagic(bool IgnoreMagic) noexcept { b_IgnoreMagic = IgnoreMagic; }

	/*
		Open
	*/
	std::uintmax_t Open(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open
	*/
	bool Open(std::filesystem::path Input, std::uintmax_t _Ptr = 0);

	/*
		Save
	*/
	std::uintmax_t Save(StdFile& File, std::uintmax_t _Ptr);

	/*
		Save
	*/
	bool Save(std::filesystem::path Output, std::uintmax_t _Ptr = 0);

	/*
		Save object to individual TMD file
	*/
	bool SaveObject(std::filesystem::path Output, std::size_t iObject);

	/*
		Save all objects to individual TMD files
	*/
	bool SaveAllObjects(std::filesystem::path Directory, std::filesystem::path Stem = "obj");

	/*
		Get total object count
	*/
	std::size_t GetObjectCount(void) const { return Object.size(); }

	/*
		Push back empty object
	*/
	void AddObject(Sony_PlayStation_Model_Object Input) { Object.push_back(Input); }

	/*
		Push back object from file
	*/
	bool AddObject(std::filesystem::path Input, std::size_t iObjectSrc);

	/*
		Copy object to buffer
	*/
	void CopyObject(std::size_t iObject, Sony_PlayStation_Model_Object& Output) { Output = Object[iObject]; }

	/*
		Paste object from buffer
	*/
	void PasteObject(std::size_t iObject, Sony_PlayStation_Model_Object Input) { Object[iObject] = Input; }

	/*
		Paste object from file
	*/
	bool PasteObject(std::filesystem::path Input, std::size_t iObjectDst, std::size_t iObjectSrc);

	/*
		Insert object from buffer
	*/
	void InsertObject(std::size_t iObject, Sony_PlayStation_Model_Object Input) { Object.insert(Object.begin() + iObject, Input); }

	/*
		Insert object from file
	*/
	bool InsertObject(std::filesystem::path Input, std::size_t iObjectDst, std::size_t iObjectSrc);

	/*
		Delete object
	*/
	void DeleteObject(std::size_t iObject) { Object.erase(Object.begin() + iObject); }

	/*
		Get total vertice count
	*/
	std::size_t GetVerticeCount(void) const;

	/*
		Get object vertice count
	*/
	std::size_t GetVerticeCount(std::size_t iObject) const { return Object[iObject].Vertice.size(); }

	/*
		Get total normal count
	*/
	std::size_t GetNormalCount(void) const;

	/*
		Get object normal count
	*/
	std::size_t GetNormalCount(std::size_t iObject) const { return Object[iObject].Normal.size(); }

	/*
		Get total primitive count
	*/
	std::size_t GetPrimitiveCount(void) const;

	/*
		Get object primitive count
	*/
	std::size_t GetPrimitiveCount(std::size_t iObject) const { return Object[iObject].Primitive.size(); }

	/*
		Get total triangle count
	*/
	std::size_t GetTriangleCount(void) const;

	/*
		Get object triangle count
	*/
	std::size_t GetTriangleCount(std::size_t iObject) const;

	/*
		Get total quadrangle count
	*/
	std::size_t GetQuadrangleCount(void) const;

	/*
		Get object quadrangle count
	*/
	std::size_t GetQuadrangleCount(std::size_t iObject) const;

	/*
		Get total line count
	*/
	std::size_t GetLineCount(void) const;

	/*
		Get object line count
	*/
	std::size_t GetLineCount(std::size_t iObject) const;

	/*
		Get total sprite count
	*/
	std::size_t GetSpriteCount(void) const;

	/*
		Get object sprite count
	*/
	std::size_t GetSpriteCount(std::size_t iObject) const;

	/*
		Is light source calculation enabled for primitive?
	*/
	bool IsLight(std::size_t iObject, std::size_t iPrimitive) const { return Object[iObject].Primitive[iPrimitive].Header.Light; }

	/*
		Is polygonal primitive double-faced?
	*/
	bool IsDoubleFace(std::size_t iObject, std::size_t iPrimitive) const { return Object[iObject].Primitive[iPrimitive].Header.Face; }

	/*
		Is gradation enabled for non-textured polygonal primitive?
	*/
	bool IsGradation(std::size_t iObject, std::size_t iPrimitive) const { return Object[iObject].Primitive[iPrimitive].Header.Gradation; }

	/*
		Is brightness calculation enabled for texture-mapped polygonal primitive?
	*/
	bool IsBrightness(std::size_t iObject, std::size_t iPrimitive) const { return Object[iObject].Primitive[iPrimitive].Header.Brightness; }

	/*
		Is translucency enabled for individual primitive?
	*/
	bool IsTranslucency(std::size_t iObject, std::size_t iPrimitive) const { return Object[iObject].Primitive[iPrimitive].Header.Translucency; }

	/*
		Is polygonal primitive a quadrilateral?
	*/
	bool IsQuadrilateral(std::size_t iObject, std::size_t iPrimitive) const { return Object[iObject].Primitive[iPrimitive].Header.Quadrilateral; }

	/*
		Is polygonal primitive gouraud-shaded?
	*/
	bool IsGouraud(std::size_t iObject, std::size_t iPrimitive) const { return Object[iObject].Primitive[iPrimitive].Header.Shader; }

	/*
		Get primitive type
	*/
	Sony_PlayStation_Primitive_Type GetType(std::size_t iObject, std::size_t iPrimitive) const;

	/*
		Get sprite texture dimensions
	*/
	void GetSpriteDimension(std::size_t iObject, std::size_t iPrimitive, std::uint16_t& Width, std::uint16_t& Height) const;

	/*
		Get texture CLUT X/Y coordinates
	*/
	void GetClutXY(std::uint16_t CLUT, std::uint16_t& X, std::uint16_t& Y) const;

	/*
		Get texture page
	*/
	std::uint16_t GetTexturePage(std::uint16_t Page) const;

	/*
		Get texture color mode
	*/
	std::uint16_t GetColorMode(std::uint16_t Page) const;

	/*
		Get texture transparency rate
	*/
	Sony_PlayStation_Transparency_Rate GetTransparencyRate(std::uint16_t Page) const;

	/*
		Get total primitive block size
	*/
	std::size_t PrimitiveSize(void) const;

	/*
		Get object primitive block size
	*/
	std::size_t PrimitiveSize(std::size_t iObject) const;

	/*
		Get file size
	*/
	std::uintmax_t Size(void) const;

	/*
		Close
	*/
	void Close(void);

};