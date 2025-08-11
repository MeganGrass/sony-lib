/*
*
*	Megan Grass
*	March 07, 2024
*
*/


#pragma once

#include <std_text.h>

#include <std_vertex.h>

#include "gte/lib_gte.h"


#ifndef ONE
#define ONE 4096	// GTE regards 4096 as 1.0
#endif


#pragma pack(push, 1)


struct Sony_PlayStation_Model_Header
{
	std::uint32_t Magic;				// 0x00	// 0x41
	struct
	{
		std::uint32_t Address : 1;		// 0 = Relative Pointers, 1 = Absolute Pointers
		std::uint32_t Reserved : 30;	// always zero (0)
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
	std::uint32_t ilen : 8;				// packet size (actual = ((ilen * 4) + 4))
	std::uint32_t Light : 1;			// 0 = On, 1 = Off
	std::uint32_t Face : 1;				// 0 = Single-faced polygon, 1 = Double-faced polygon (Backface Culling)
	std::uint32_t Gradation : 1;		// 0 = Solid-color, 1 = Gradation
	std::uint32_t reserved : 5;			// always zero (0)
	std::uint32_t Brightness : 1;		// 0 = On, 1 = Off (Texture Map Calculation)
	std::uint32_t Translucency : 1;		// 0 = Off, 1 = On
	std::uint32_t Texture : 1;			// 0 = Off, 1 = On
	std::uint32_t Quadrilateral : 1;	// 0 = Triangle, 1 = Quadrangle
	std::uint32_t Shader : 1;			// 0 = Flat, 1 = Gouraud
	std::uint32_t Code : 3;				// 1 = Triangle/Quadrangle, 2 = Line, 3 = Sprite
};


struct Sony_PlayStation_Model_Sprite_Header
{
	std::uint32_t olen : 8;				// 2D drawing primitive size
	std::uint32_t ilen : 8;				// packet size (actual = ((ilen * 4) + 4))
	std::uint32_t Light : 1;			// 0 = On, 1 = Off
	std::uint32_t Face : 1;				// 0 = Single-faced polygon, 1 = Double-faced polygon (Backface Culling)
	std::uint32_t Gradation : 1;		// 0 = Solid-color, 1 = Gradation
	std::uint32_t reserved : 5;			// always zero (0)
	std::uint32_t Brightness : 1;		// 0 = On, 1 = Off (Texture Map Calculation)
	std::uint32_t Translucency : 1;		// 0 = Off, 1 = On
	std::uint32_t Texture : 1;			// 0 = Off, 1 = On
	std::uint32_t Size : 2;				// 0 = Free, 1 = 1x1, 2 = 8x8, 3 = 16x16
	std::uint32_t Code : 3;				// 1 = Triangle/Quadrangle, 2 = Line, 3 = Sprite
};


struct Sony_PlayStation_Model_Texture_Attr
{
	std::uint16_t Page : 5;				// Texture page (0 to 31)
	std::uint16_t SemiTrans : 2;		/* Semi-transparency (valid if Translucency is enabled)
											0 = 50%back + 50%polygon
											1 = 100%back + 100%polygon
											2 = 100%back - 100%polygon
											3 = 100%back + 25%polygon
										*/
	std::uint16_t ColorMode : 2;		// 0 = 4-bit, 1 = 8-bit, 2 = 15-bit, 3 = 24-bit
	std::uint16_t reserved : 1;			// always zero (0)
	std::uint16_t Scaling : 1;			// 0 = None, 1 = Scaled (valid if SPRITEGp)
	std::uint16_t Rotation : 1;			// 0 = None, 1 = Rotated (valid if SPRITEGp)
	std::uint16_t Size : 4;				// Rectangle Size (valid if SPRITEGp)
};


struct Sony_PlayStation_Model_Texture_Clut
{
	std::uint16_t X : 6;				// X coordinate in VRAM
	std::uint16_t Y : 9;				// Y coordinate in VRAM
	std::uint16_t SemiTrans : 1;		// Semi-transparency (valid if SPRITEGp)
};


#pragma pack(pop)


enum class Sony_PlayStation_Primitive_Shader : std::uint32_t
{
	Flat = 0,
	Gouraud = 1
};


enum class Sony_PlayStation_Primitive_Type : std::uint32_t
{
	Polygon = 1,
	Line = 2,
	Sprite = 3
};


enum class Sony_PlayStation_Transparency_Rate : std::uint32_t
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


struct FIXED_PRIMITIVE
{
	struct TEXTURE
	{
		bool Active;
		bool Transparency;
		std::uint16_t Depth;
		std::size_t iPalette;
		Sony_PlayStation_Transparency_Rate TransparencyRate;
	} Texture{};
	bool Light{};
	bool Gradation{};
	bool BackfaceCull{};
	Sony_PlayStation_Primitive_Shader Shader;
	Sony_PlayStation_Primitive_Type Type;
	std::vector<vec3> Vector;
	std::vector<vec3> Normal;
	std::vector<unsigned long> Color;
	std::vector<vec2> UV;
};


struct FIXED_MODEL
{
	std::vector<std::vector<FIXED_PRIMITIVE>> Obj;
};


class Sony_PlayStation_Model {
private:

	// Sony PlayStation Model Object
	std::vector<Sony_PlayStation_Model_Object> m_Object;

	// Flag
	bool b_Open;
	bool b_IgnoreMagic;

public:

	explicit Sony_PlayStation_Model(void) :
		b_Open(false),
		b_IgnoreMagic(false)
	{
	}

	explicit Sony_PlayStation_Model(std::filesystem::path Path) :
		b_Open(false),
		b_IgnoreMagic(false)
	{
		Open(Path);
	}

	explicit Sony_PlayStation_Model(std::filesystem::path Path, std::uintmax_t _Ptr) :
		b_Open(false),
		b_IgnoreMagic(false)
	{
		Open(Path, _Ptr);
	}

	explicit Sony_PlayStation_Model(HWND hWnd, std::filesystem::path Path, std::uintmax_t _Ptr, bool IgnoreMagic) :
		b_Open(false),
		b_IgnoreMagic(IgnoreMagic)
	{
		Str.hWnd = hWnd;
		Open(Path, _Ptr);
	}

	virtual ~Sony_PlayStation_Model(void) = default;

	// Standard String for debugging/messages
	Standard_String Str;

	// Is the model open?
	[[nodiscard]] bool operator !() { return !b_Open; }

	// Is the model open?
	[[nodiscard]] bool IsOpen(void) const noexcept { return b_Open; }

	// Force open if object container is not empty
	bool ForceOpen(void) noexcept { if (!m_Object.empty()) { b_Open = true; } return b_Open; }

	// Ignore magic number in header when opening file
	void IgnoreMagic(bool IgnoreMagic) noexcept { b_IgnoreMagic = IgnoreMagic; }

	// Object Data
	[[nodiscard]] const Sony_PlayStation_Model_Object& Object(std::size_t iObject) const { return m_Object[iObject]; }

	// Normal Data
	[[nodiscard]] const SVECTOR& Normal(std::size_t iObject, std::size_t iNormal) const { return m_Object[iObject].Normal[iNormal]; }

	// Vertice Data
	[[nodiscard]] const SVECTOR& Vertice(std::size_t iObject, std::size_t iVertice) const { return m_Object[iObject].Vertice[iVertice]; }

	// Primitive Data
	[[nodiscard]] const Sony_PlayStation_Model_Primitive& Primitive(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive]; }

	// Packet Data
	[[nodiscard]] const std::vector<std::uint8_t>& Packet(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Packet; }

	// Open
	std::uintmax_t Open(StdFile& File, std::uintmax_t _Ptr);

	// Open
	bool Open(std::filesystem::path Path, std::uintmax_t _Ptr = 0)
	{
		StdFile m_File;
		m_File.SetPath(Path);
		Open(m_File, _Ptr);
		return b_Open;
	}

	// Save
	std::uintmax_t Save(StdFile& File, std::uintmax_t _Ptr);

	// Save
	bool Save(std::filesystem::path Path, std::uintmax_t _Ptr = 0, bool b_Truncate = true)
	{
		StdFile m_File;

		if (b_Truncate)
		{
			m_File.Open(Path, FileAccessMode::Write_Ex, true, true);
		}
		else
		{
			m_File.Open(Path, FileAccessMode::Read_Ex, true, false);
		}

		std::uintmax_t m_Ptr = _Ptr;

		_Ptr = Save(m_File, _Ptr);

		return m_Ptr != _Ptr;
	}

	// Save object to individual TMD file
	bool SaveObject(std::filesystem::path Path, std::size_t iObject);

	// Save all objects to individual TMD files
	bool SaveAllObjects(std::filesystem::path Directory, std::filesystem::path Stem = "obj");

	// Get total object count
	[[nodiscard]] std::size_t ObjectCount(void) const { return m_Object.size(); }

	// Push back empty object
	void AddObject(Sony_PlayStation_Model_Object Object) { m_Object.push_back(Object); }

	// Push back object from file
	bool AddObject(std::filesystem::path Path, std::size_t iObjectSrc);

	// Copy object to buffer
	void CopyObject(std::size_t iObject, Sony_PlayStation_Model_Object& Output) { Output = m_Object[iObject]; }

	// Paste object from buffer
	void PasteObject(std::size_t iObject, Sony_PlayStation_Model_Object Input) { m_Object[iObject] = Input; }

	// Paste object from file
	bool PasteObject(std::filesystem::path Path, std::size_t iObjectDst, std::size_t iObjectSrc);

	// Insert object from buffer
	void InsertObject(std::size_t iObject, Sony_PlayStation_Model_Object Input) { m_Object.insert(m_Object.begin() + iObject, Input); }

	// Insert object from file
	bool InsertObject(std::filesystem::path Path, std::size_t iObjectDst, std::size_t iObjectSrc);

	// Delete object
	void DeleteObject(std::size_t iObject) { m_Object.erase(m_Object.begin() + iObject); }

	// Get total vertice count
	[[nodiscard]] std::size_t GetVerticeCount(void) const;

	// Get object vertice count
	[[nodiscard]] std::size_t GetVerticeCount(std::size_t iObject) const { return m_Object[iObject].Vertice.size(); }

	// Get total normal count
	[[nodiscard]] std::size_t GetNormalCount(void) const;

	// Get object normal count
	[[nodiscard]] std::size_t GetNormalCount(std::size_t iObject) const { return m_Object[iObject].Normal.size(); }

	// Get total primitive count
	[[nodiscard]] std::size_t GetPrimitiveCount(void) const;

	// Get object primitive count
	[[nodiscard]] std::size_t GetPrimitiveCount(std::size_t iObject) const { return m_Object[iObject].Primitive.size(); }

	// Get total triangle count
	[[nodiscard]] std::size_t GetTriangleCount(void) const;

	// Get object triangle count
	[[nodiscard]] std::size_t GetTriangleCount(std::size_t iObject);

	// Get total quadrangle count
	[[nodiscard]] std::size_t GetQuadrangleCount(void) const;

	// Get object quadrangle count
	[[nodiscard]] std::size_t GetQuadrangleCount(std::size_t iObject);

	// Get total line count
	[[nodiscard]] std::size_t GetLineCount(void) const;

	// Get object line count
	[[nodiscard]] std::size_t GetLineCount(std::size_t iObject);

	// Get total sprite count
	[[nodiscard]] std::size_t GetSpriteCount(void) const;

	// Get object sprite count
	[[nodiscard]] std::size_t GetSpriteCount(std::size_t iObject);

	// Is light source calculation enabled for primitive?
	[[nodiscard]] bool IsLight(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Header.Light; }

	// Is polygonal primitive double-faced?
	[[nodiscard]] bool IsDoubleFace(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Header.Face; }

	// Is gradation enabled for non-textured polygonal primitive?
	[[nodiscard]] bool IsGradation(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Header.Gradation; }

	// Is brightness calculation enabled for texture-mapped polygonal primitive?
	[[nodiscard]] bool IsBrightness(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Header.Brightness; }

	// Is translucency enabled for individual primitive?
	[[nodiscard]] bool IsTranslucency(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Header.Translucency; }

	// Is polygonal primitive texture-mapped?
	[[nodiscard]] bool IsTexture(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Header.Texture; }

	// Is polygonal primitive a quadrilateral?
	[[nodiscard]] bool IsQuadrilateral(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Header.Quadrilateral; }

	// Is polygonal primitive gouraud-shaded?
	[[nodiscard]] bool IsGouraud(std::size_t iObject, std::size_t iPrimitive) const { return m_Object[iObject].Primitive[iPrimitive].Header.Shader; }

	// Get shader type
	[[nodiscard]] Sony_PlayStation_Primitive_Shader GetShader(std::size_t iObject, std::size_t iPrimitive) const
	{
		return Sony_PlayStation_Primitive_Shader(m_Object[iObject].Primitive[iPrimitive].Header.Shader);
	}

	// Get primitive type
	[[nodiscard]] Sony_PlayStation_Primitive_Type GetType(std::size_t iObject, std::size_t iPrimitive) const { return Sony_PlayStation_Primitive_Type(m_Object[iObject].Primitive[iPrimitive].Header.Code); }

	// Get sprite texture dimensions
	void GetSpriteDimension(std::size_t iObject, std::size_t iPrimitive, std::uint16_t& Width, std::uint16_t& Height);

	// Get sprite header
	[[nodiscard]] Sony_PlayStation_Model_Sprite_Header GetSpriteHeader(std::size_t iObject, std::size_t iPrimitive) const
	{
		Sony_PlayStation_Model_Sprite_Header Header{};
		std::memcpy(&Header, &m_Object[iObject].Primitive[iPrimitive].Packet, sizeof(Sony_PlayStation_Model_Sprite_Header));
		return Header;
	}

	// Get texture CLUT X/Y coordinates (CLY/CLX of CBA)
	[[nodiscard]] Sony_PlayStation_Model_Texture_Clut GetClutXY(std::uint16_t CBA) const
	{
		Sony_PlayStation_Model_Texture_Clut Clut{};
		std::memcpy(&Clut, &CBA, sizeof(Sony_PlayStation_Model_Texture_Clut));
		return Clut;
	}

	// Get CLUT ID
	[[nodiscard]] std::uint16_t GetClutID(std::uint16_t CBA) const
	{
		Sony_PlayStation_Model_Texture_Clut Clut{};
		std::memcpy(&Clut, &CBA, sizeof(Sony_PlayStation_Model_Texture_Clut));
		return Clut.Y & 0xF;
	}

	// Get texture page (TPN)
	[[nodiscard]] std::uint16_t GetTexturePage(std::uint16_t TSB) const
	{
		Sony_PlayStation_Model_Texture_Attr Attr{};
		std::memcpy(&Attr, &TSB, sizeof(Sony_PlayStation_Model_Texture_Attr));
		return Attr.Page;
	}

	// Get texture transparency rate (ABR)
	[[nodiscard]] Sony_PlayStation_Transparency_Rate GetTransparencyRate(std::uint16_t TSB) const
	{
		Sony_PlayStation_Model_Texture_Attr Attr{};
		std::memcpy(&Attr, &TSB, sizeof(Sony_PlayStation_Model_Texture_Attr));
		return Sony_PlayStation_Transparency_Rate(Attr.SemiTrans);
	}

	// Get texture color mode (TPF)
	[[nodiscard]] std::uint16_t GetColorMode(std::uint16_t TSB) const
	{
		Sony_PlayStation_Model_Texture_Attr Attr{};
		std::memcpy(&Attr, &TSB, sizeof(Sony_PlayStation_Model_Texture_Attr));
		switch (Attr.ColorMode)
		{
		case 0: return 4;
		case 1: return 8;
		case 2: return 15;
		case 3: return 24;
		default: return 0;
		}
	}

	// Get total primitive block size
	[[nodiscard]] std::size_t PrimitiveSize(void) const;

	// Get object primitive block size
	[[nodiscard]] std::size_t PrimitiveSize(std::size_t iObject);

	// Get file size
	[[nodiscard]] std::uintmax_t Size(void) const;

	// Close
	void Close(void);

	/*
		Export model to fixed-typed
		 - convert all vector, normal and UV data to standardized float
		 - provide texture width and height to convert UV coordinates
		 - if b_TexelDX9 is true, UV coordinates will be adjust for half-texel correction
		 - if b_POT is true, texture width and height will be converted to power of two
	*/
	[[nodiscard]] std::unique_ptr<FIXED_MODEL> Export(std::uint16_t TextureWidth = 0, std::uint16_t TextureHeight = 0, bool b_TexelDX9 = false, bool b_POT = true) const;

};