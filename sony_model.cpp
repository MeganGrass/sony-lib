/*
*
*	Megan Grass
*	March 07, 2024
*
*/


#include "sony_model.h"


std::uintmax_t Sony_PlayStation_Model::Open(StdFile& File, std::uintmax_t _Ptr)
{
	if (b_Open) { Close(); }

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"Sony PlayStation Model Error: could not open at 0x%llX in \"%ws\"", _Ptr, File.GetPath().filename().wstring().c_str());
			return _Ptr;
		}
	}

	Sony_PlayStation_Model_Header Header{};
	File.Read(_Ptr , &Header, sizeof(Sony_PlayStation_Model_Header));

	if (!b_IgnoreMagic && Header.Magic != 0x41)
	{
		Str.Message(L"Sony PlayStation Model Error: invalid magic at 0x%llX in \"%ws\"", _Ptr, File.GetPath().filename().wstring().c_str());
		return _Ptr;
	}

	std::vector<Sony_PlayStation_Model_Index> Index(Header.nObject);
	File.Read(_Ptr + sizeof(Sony_PlayStation_Model_Header), Index.data(), Index.size() * sizeof(Sony_PlayStation_Model_Index));

	std::uintmax_t pPrimitive = 0;

	for (std::size_t i = 0; i < Index.size(); i++)
	{
		Sony_PlayStation_Model_Object Obj;

		Obj.Scale = Index[i].Scale;

		Obj.Vertice.resize(Index[i].nVertice);
		if (Header.Flag.Address)
		{
			File.Read(_Ptr + Index[i].pVertice, Obj.Vertice.data(), Obj.Vertice.size() * sizeof(SVECTOR));
		}
		else
		{
			File.Read(_Ptr + Index[i].pVertice + sizeof(Sony_PlayStation_Model_Header), Obj.Vertice.data(), Obj.Vertice.size() * sizeof(SVECTOR));
		}

		Obj.Normal.resize(Index[i].nNormal);
		if (Header.Flag.Address)
		{
			File.Read(_Ptr + Index[i].pNormal, Obj.Normal.data(), Obj.Normal.size() * sizeof(SVECTOR));
		}
		else
		{
			File.Read(_Ptr + Index[i].pNormal + sizeof(Sony_PlayStation_Model_Header), Obj.Normal.data(), Obj.Normal.size() * sizeof(SVECTOR));
		}

		Obj.Primitive.resize(Index[i].nPrimitive);
		if (Header.Flag.Address)
		{
			pPrimitive = _Ptr + Index[i].pPrimitive;
		}
		else
		{
			pPrimitive = _Ptr + Index[i].pPrimitive + sizeof(Sony_PlayStation_Model_Header);
		}
		for (std::size_t x = 0; x < Obj.Primitive.size(); x++)
		{
			File.Read(pPrimitive, &Obj.Primitive[x].Header, sizeof(Sony_PlayStation_Model_Primitive::Header));

			size_t PacketSize = static_cast<std::size_t>((Obj.Primitive[x].Header.ilen * 4) + 4);

			Obj.Primitive[x].Packet.resize(PacketSize);

			File.Read(pPrimitive, Obj.Primitive[x].Packet.data(), Obj.Primitive[x].Packet.size());

			pPrimitive += Obj.Primitive[x].Packet.size();
		}

		m_Object.push_back(Obj);
	}

	b_Open = true;

	return _Ptr + Size();
}

std::uintmax_t Sony_PlayStation_Model::Save(StdFile& File, std::uintmax_t _Ptr)
{
	if (!b_Open)
	{
		Str.Message(L"Sony PlayStation Model Error: model is not open");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"Sony PlayStation Model Error: could not create at 0x%llX in \"%ws\"", _Ptr, File.GetPath().filename().wstring().c_str());
				return _Ptr;
			}
		}
	}

	std::uintmax_t pIndex = _Ptr + sizeof(Sony_PlayStation_Model_Header);
	std::uintmax_t pPrimitive = pIndex + (sizeof(Sony_PlayStation_Model_Index) * ObjectCount());
	std::uintmax_t pVertice = pPrimitive + PrimitiveSize();
	std::uintmax_t pNormal = pVertice + (sizeof(SVECTOR) * GetVerticeCount());

	Sony_PlayStation_Model_Header Header{};
	Header.Magic = 0x41;
	Header.Flag.Address = false;
	Header.nObject = static_cast<std::uint32_t>(ObjectCount());
	File.Write(_Ptr, &Header, sizeof(Sony_PlayStation_Model_Header));

	for (std::size_t i = 0; i < ObjectCount(); i++)
	{
		Sony_PlayStation_Model_Index Index{};
		Index.pVertice = static_cast<std::uint32_t>((pVertice - _Ptr) - sizeof(Sony_PlayStation_Model_Header));
		Index.nVertice = static_cast<std::uint32_t>(m_Object[i].Vertice.size());
		Index.pNormal = static_cast<std::uint32_t>((pNormal - _Ptr) - sizeof(Sony_PlayStation_Model_Header));
		Index.nNormal = static_cast<std::uint32_t>(m_Object[i].Normal.size());
		Index.pPrimitive = static_cast<std::uint32_t>((pPrimitive - _Ptr) - sizeof(Sony_PlayStation_Model_Header));
		Index.nPrimitive = static_cast<std::uint32_t>(m_Object[i].Primitive.size());
		Index.Scale = m_Object[i].Scale;
		File.Write(pIndex, &Index, sizeof(Sony_PlayStation_Model_Index));
		pIndex += sizeof(Sony_PlayStation_Model_Index);

		File.Write(pVertice, m_Object[i].Vertice.data(), m_Object[i].Vertice.size() * sizeof(SVECTOR));
		pVertice += m_Object[i].Vertice.size() * sizeof(SVECTOR);

		File.Write(pNormal, m_Object[i].Normal.data(), m_Object[i].Normal.size() * sizeof(SVECTOR));
		pNormal += m_Object[i].Normal.size() * sizeof(SVECTOR);

		for (std::size_t x = 0; x < m_Object[i].Primitive.size(); x++)
		{
			File.Write(pPrimitive, m_Object[i].Primitive[x].Packet.data(), m_Object[i].Primitive[x].Packet.size());
			pPrimitive += m_Object[i].Primitive[x].Packet.size();
		}
	}

	return _Ptr + Size();
}

bool Sony_PlayStation_Model::SaveObject(std::filesystem::path Path, std::size_t iObject)
{
	if (!b_Open)
	{
		Str.Message(L"Sony PlayStation Model Error: model is not open");
		return false;
	}

	if ((iObject + 1) > m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object index out of range");
		return false;
	}

	StdFile File{ Path, FileAccessMode::Write, true, false };

	if (!File.IsOpen())
	{
		Str.Message(L"Sony PlayStation Model: Error, could not create \"%ws\"", Path.filename().wstring().c_str());
		return false;
	}

	std::uintmax_t pIndex = sizeof(Sony_PlayStation_Model_Header);
	std::uintmax_t pPrimitive = pIndex + sizeof(Sony_PlayStation_Model_Index);
	std::uintmax_t pVertice = pPrimitive + PrimitiveSize(iObject);
	std::uintmax_t pNormal = pVertice + (sizeof(SVECTOR) * GetVerticeCount(iObject));

	Sony_PlayStation_Model_Header Header{};
	Header.Magic = 0x41;
	Header.Flag.Address = false;
	Header.nObject = 1;
	File.Write(0, &Header, sizeof(Sony_PlayStation_Model_Header));

	Sony_PlayStation_Model_Index Index{};
	Index.pVertice = static_cast<std::uint32_t>(pVertice - sizeof(Sony_PlayStation_Model_Header));
	Index.nVertice = static_cast<std::uint32_t>(m_Object[iObject].Vertice.size());
	Index.pNormal = static_cast<std::uint32_t>(pNormal - sizeof(Sony_PlayStation_Model_Header));
	Index.nNormal = static_cast<std::uint32_t>(m_Object[iObject].Normal.size());
	Index.pPrimitive = static_cast<std::uint32_t>(pPrimitive - sizeof(Sony_PlayStation_Model_Header));
	Index.nPrimitive = static_cast<std::uint32_t>(m_Object[iObject].Primitive.size());
	Index.Scale = m_Object[iObject].Scale;
	File.Write(pIndex, &Index, sizeof(Sony_PlayStation_Model_Index));

	File.Write(pVertice, m_Object[iObject].Vertice.data(), m_Object[iObject].Vertice.size() * sizeof(SVECTOR));

	File.Write(pNormal, m_Object[iObject].Normal.data(), m_Object[iObject].Normal.size() * sizeof(SVECTOR));

	for (std::size_t x = 0; x < m_Object[iObject].Primitive.size(); x++)
	{
		File.Write(pPrimitive, m_Object[iObject].Primitive[x].Packet.data(), m_Object[iObject].Primitive[x].Packet.size());
		pPrimitive += m_Object[iObject].Primitive[x].Packet.size();
	}

	return true;
}

bool Sony_PlayStation_Model::SaveAllObjects(std::filesystem::path Directory, std::filesystem::path Stem)
{
	if (!b_Open)
	{
		Str.Message(L"Sony PlayStation Model Error: model is not open");
		return false;
	}

	std::filesystem::path Dir = Standard_FileSystem().GetDirectory(Directory);
	Standard_FileSystem().CreateDirectory(Dir / Stem.stem());

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		std::filesystem::path Output = Str.FormatCStyle(L"%ws\\%ws\\%ws_%02d.tmd", Dir.wstring().c_str(), Stem.stem().wstring().c_str(), Stem.stem().wstring().c_str(), i);
		SaveObject(Output, i);
	}

	return true;
}

bool Sony_PlayStation_Model::AddObject(std::filesystem::path Path, std::size_t iObjectSrc)
{
	Sony_PlayStation_Model Tmd { Path };

	if (!Tmd.IsOpen())
	{
		Str.Message(L"Sony PlayStation Model Error: could not open \"%ws\"", Path.filename().wstring().c_str());
		return false;
	}

	if ((iObjectSrc + 1) > Tmd.m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object index out of range");
		return false;
	}

	m_Object.push_back(Tmd.m_Object[iObjectSrc]);

	return true;
}

bool Sony_PlayStation_Model::PasteObject(std::filesystem::path Path, std::size_t iObjectDst, std::size_t iObjectSrc)
{
	if ((iObjectDst + 1) > m_Object.size())
	{
		return AddObject(Path, iObjectSrc);
	}

	Sony_PlayStation_Model Tmd { Path };

	if (!Tmd.IsOpen())
	{
		Str.Message(L"Sony PlayStation Model Error: could not open \"%ws\"", Path.filename().wstring().c_str());
		return false;
	}

	if ((iObjectSrc + 1) > Tmd.m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object index out of range");
		return false;
	}

	m_Object[iObjectDst] = Tmd.m_Object[iObjectSrc];

	return true;
}

bool Sony_PlayStation_Model::InsertObject(std::filesystem::path Path, std::size_t iObjectDst, std::size_t iObjectSrc)
{
	if ((iObjectDst + 1) > m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object destination index out of range");
		return false;
	}

	Sony_PlayStation_Model Tmd { Path };

	if (!Tmd.IsOpen())
	{
		Str.Message(L"Sony PlayStation Model Error: could not open \"%ws\"", Path.filename().wstring().c_str());
		return false;
	}

	if ((iObjectSrc + 1) > Tmd.m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object source index out of range");
		return false;
	}

	m_Object.insert(m_Object.begin() + iObjectDst, Tmd.m_Object[iObjectSrc]);

	return true;
}

std::size_t Sony_PlayStation_Model::GetVerticeCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nVertice = 0;

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		nVertice += m_Object[i].Vertice.size();
	}

	return nVertice;
}

std::size_t Sony_PlayStation_Model::GetNormalCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nNormal = 0;

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		nNormal += m_Object[i].Normal.size();
	}

	return nNormal;
}

std::size_t Sony_PlayStation_Model::GetPrimitiveCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nPrimitive = 0;

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		nPrimitive += m_Object[i].Primitive.size();
	}

	return nPrimitive;
}

std::size_t Sony_PlayStation_Model::GetTriangleCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nTriangle = 0;

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		for (std::size_t x = 0; x < m_Object[i].Primitive.size(); x++)
		{
			if ((m_Object[i].Primitive[x].Header.Code == 1) && (m_Object[i].Primitive[x].Header.Quadrilateral == 0))
			{
				nTriangle++;
			}
		}
	}

	return nTriangle;
}

std::size_t Sony_PlayStation_Model::GetTriangleCount(std::size_t iObject)
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object index out of range");
		return 0;
	}

	std::size_t nTriangle = 0;

	for (std::size_t x = 0; x < m_Object[iObject].Primitive.size(); x++)
	{
		if ((m_Object[iObject].Primitive[x].Header.Code == 1) && (m_Object[iObject].Primitive[x].Header.Quadrilateral == 0))
		{
			nTriangle++;
		}
	}

	return nTriangle;
}

std::size_t Sony_PlayStation_Model::GetQuadrangleCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nQuadrangle = 0;

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		for (std::size_t x = 0; x < m_Object[i].Primitive.size(); x++)
		{
			if ((m_Object[i].Primitive[x].Header.Code == 1) && (m_Object[i].Primitive[x].Header.Quadrilateral == 1))
			{
				nQuadrangle++;
			}
		}
	}

	return nQuadrangle;
}

std::size_t Sony_PlayStation_Model::GetQuadrangleCount(std::size_t iObject)
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object index out of range");
		return 0;
	}

	std::size_t nQuadrangle = 0;

	for (std::size_t x = 0; x < m_Object[iObject].Primitive.size(); x++)
	{
		if ((m_Object[iObject].Primitive[x].Header.Code == 1) && (m_Object[iObject].Primitive[x].Header.Quadrilateral == 1))
		{
			nQuadrangle++;
		}
	}

	return nQuadrangle;
}

std::size_t Sony_PlayStation_Model::GetLineCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nLine = 0;

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		for (std::size_t x = 0; x < m_Object[i].Primitive.size(); x++)
		{
			if (m_Object[i].Primitive[x].Header.Code == 2)
			{
				nLine++;
			}
		}
	}

	return nLine;

}

std::size_t Sony_PlayStation_Model::GetLineCount(std::size_t iObject)
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object index out of range");
		return 0;
	}

	std::size_t nLine = 0;

	for (std::size_t x = 0; x < m_Object[iObject].Primitive.size(); x++)
	{
		if (m_Object[iObject].Primitive[x].Header.Code == 2)
		{
			nLine++;
		}
	}

	return nLine;
}

std::size_t Sony_PlayStation_Model::GetSpriteCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nSprite = 0;

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		for (std::size_t x = 0; x < m_Object[i].Primitive.size(); x++)
		{
			if (m_Object[i].Primitive[x].Header.Code == 3)
			{
				nSprite++;
			}
		}
	}

	return nSprite;
}

std::size_t Sony_PlayStation_Model::GetSpriteCount(std::size_t iObject)
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object index out of range");
		return 0;
	}

	std::size_t nSprite = 0;

	for (std::size_t x = 0; x < m_Object[iObject].Primitive.size(); x++)
	{
		if (m_Object[iObject].Primitive[x].Header.Code == 3)
		{
			nSprite++;
		}
	}

	return nSprite;
}

void Sony_PlayStation_Model::GetSpriteDimension(std::size_t iObject, std::size_t iPrimitive, std::uint16_t& Width, std::uint16_t& Height)
{
	Sony_PlayStation_Primitive_Type Type = GetType(iObject, iPrimitive);

	if (Type != Sony_PlayStation_Primitive_Type::Sprite)
	{
		Str.Message(L"Sony PlayStation Model Error: primitive is not a sprite");
		return;
	}

	Sony_PlayStation_Model_Sprite_Header Header{};
	std::memcpy(&Header, &m_Object[iObject].Primitive[iPrimitive].Header, sizeof(Sony_PlayStation_Model_Sprite_Header));

	switch (Header.Size)
	{
	case 0: break;
	case 1:
		Width = 1;
		Height = 1;
		return;
	case 2:
		Width = 8;
		Height = 8;
		return;
	case 3:
		Width = 16;
		Height = 16;
		return;
	}

	if (m_Object[iObject].Primitive[iPrimitive].Packet.size() != 16)
	{
		Str.Message(L"Sony PlayStation Model Error: invalid packet size for free-size sprite");
		Width = 0;
		Height = 0;
		return;
	}

	std::memcpy(&Width, &m_Object[iObject].Primitive[iPrimitive].Packet.data()[12], sizeof(std::uint16_t));
	std::memcpy(&Height, &m_Object[iObject].Primitive[iPrimitive].Packet.data()[14], sizeof(std::uint16_t));

}

std::size_t Sony_PlayStation_Model::PrimitiveSize(void) const
{
	if (!b_Open) { return 0; }

	std::size_t PrimitiveSize = 0;

	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		for (std::size_t x = 0; x < m_Object[i].Primitive.size(); x++)
		{
			PrimitiveSize += m_Object[i].Primitive[x].Packet.size();
		}
	}

	return PrimitiveSize;
}

std::size_t Sony_PlayStation_Model::PrimitiveSize(std::size_t iObject)
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > m_Object.size())
	{
		Str.Message(L"Sony PlayStation Model Error: object index out of range");
		return 0;
	}

	std::size_t PrimitiveSize = 0;

	for (std::size_t x = 0; x < m_Object[iObject].Primitive.size(); x++)
	{
		PrimitiveSize += m_Object[iObject].Primitive[x].Packet.size();
	}

	return PrimitiveSize;
}

std::uintmax_t Sony_PlayStation_Model::Size(void) const
{
	if (!b_Open) { return 0; }

	std::uintmax_t pIndex = sizeof(Sony_PlayStation_Model_Header);
	std::uintmax_t pPrimitive = pIndex + (sizeof(Sony_PlayStation_Model_Index) * ObjectCount());
	std::uintmax_t pVertice = pPrimitive + PrimitiveSize();
	std::uintmax_t pNormal = pVertice + (sizeof(SVECTOR) * GetVerticeCount());
	std::uintmax_t Size = pNormal + (sizeof(SVECTOR) * GetNormalCount());

	return Size;
}

void Sony_PlayStation_Model::Close(void)
{
	b_Open = false;
	for (std::size_t i = 0; i < m_Object.size(); i++)
	{
		m_Object[i].Vertice.clear();
		m_Object[i].Normal.clear();
		for (std::size_t x = 0; x < m_Object[i].Primitive.size(); x++)
		{
			std::memset(&m_Object[i].Primitive[x].Header, 0, sizeof(Sony_PlayStation_Model_Primitive::Header));
			m_Object[i].Primitive[x].Packet.clear();
		}
		m_Object[i].Primitive.clear();
	}
	m_Object.clear();
}

std::unique_ptr<FIXED_MODEL> Sony_PlayStation_Model::Export(std::uint16_t TextureWidth, std::uint16_t TextureHeight, bool b_TexelDX9, bool b_POT) const
{
	std::unique_ptr<FIXED_MODEL> Model = std::make_unique<FIXED_MODEL>();

	if (!b_Open || !ObjectCount()) { return Model; }

	float TexelOffset = 0.0f;
	if (b_TexelDX9) { TexelOffset = 0.5f; }

	constexpr std::uint16_t TexturePageWidth = 128;

	struct UV
	{
		std::uint8_t U0, V0;
		std::uint16_t CBA;
		std::uint8_t U1, V1;
		std::uint16_t TSB;
		std::uint8_t U2, V2;
		std::uint16_t pad0;
		std::uint8_t U3, V3;
		std::uint16_t pad1;
	} UV{};

	struct RGB
	{
		std::uint8_t R0, G0, B0, code;
		std::uint8_t R1, G1, B1, pad0;
		std::uint8_t R2, G2, B2, pad1;
		std::uint8_t R3, G3, B3, pad2;
	} RGB{};

	struct POSITION
	{
		std::uint16_t V0, V1;
		std::uint16_t V2, V3;
	} POSITION{};

	struct FLAT
	{
		std::uint16_t N0, V0;
		std::uint16_t V1, V2;
		std::uint16_t V3, pad;
	} FLAT{};

	struct GOURAUD
	{
		std::uint16_t N0, V0;
		std::uint16_t N1, V1;
		std::uint16_t N2, V2;
		std::uint16_t N3, V3;
	} GOURAUD{};

	struct SPRITE
	{
		std::uint16_t V;
		std::uint16_t TSB;
		std::uint8_t TU, TV;
		std::uint16_t CBA;
		std::uint16_t W, H;
	} SPRITE{};

	if (b_POT)
	{
		auto NextPowerOfTwo = [](int32_t x) {
			if (x <= 0) { return 1; }
			x--;
			for (int Shift = 1; Shift < 32; Shift <<= 1) { x |= x >> Shift; }
			return x + 1;
			};

		TextureWidth = NextPowerOfTwo(TextureWidth);
		TextureHeight = NextPowerOfTwo(TextureHeight);
	}

	auto ToFloat = [](auto Value) noexcept { half h = Value; return (float)(h) / ONE; };

	Model->Obj.resize(ObjectCount());

	for (size_t i = 0; i < ObjectCount(); i++)
	{
		Model->Obj[i].resize(Object(i).Primitive.size());

		for (size_t x = 0; x < Object(i).Primitive.size(); x++)
		{
			bool b_Light = IsLight(i, x);
			bool b_Face = IsDoubleFace(i, x);
			bool b_Gradation = IsGradation(i, x);
			bool b_Brightness = IsBrightness(i, x);
			bool b_Translucency = IsTranslucency(i, x);
			bool b_Texture = IsTexture(i, x);
			bool b_Quadrilateral = IsQuadrilateral(i, x);
			bool b_Gouraud = IsGouraud(i, x);
			bool b_Polygon = GetType(i, x) == Sony_PlayStation_Primitive_Type::Polygon;
			bool b_Line = GetType(i, x) == Sony_PlayStation_Primitive_Type::Line;
			bool b_Sprite = GetType(i, x) == Sony_PlayStation_Primitive_Type::Sprite;

			bool b_NoLightFlat = (b_Light && b_Brightness && !b_Gouraud);
			bool b_NoLightGradation = (b_Light && b_Brightness && b_Gouraud);

			bool b_LightFlat = (!b_Light && !b_Gouraud);
			bool b_LightGouraud = (!b_Light && b_Gouraud);

			bool b_RGB = (b_Gradation || b_NoLightGradation);
			bool b_Solid = (!b_Texture || b_NoLightFlat);

			std::vector<std::uint8_t> Primitive = Packet(i, x);

			std::size_t Pointer = sizeof(Sony_PlayStation_Model_Primitive_Header);

			std::size_t VertexCount = b_Sprite ? 4 : b_Line ? 2 : b_Quadrilateral ? 6 : 3;

			Model->Obj[i][x].Light = !b_Light;
			Model->Obj[i][x].Gradation = b_RGB;
			Model->Obj[i][x].BackfaceCull = !b_Face;
			Model->Obj[i][x].Shader = GetShader(i, x);
			Model->Obj[i][x].Type = GetType(i, x);

			Model->Obj[i][x].Vector.resize(VertexCount);
			Model->Obj[i][x].Normal.resize(VertexCount);
			Model->Obj[i][x].Color.resize(VertexCount);
			Model->Obj[i][x].UV.resize(VertexCount);

			if (b_Polygon)
			{
				if (b_Texture)
				{
					if (b_Quadrilateral)
					{
						std::memcpy(&UV, &Primitive[Pointer], sizeof(UV));
						Pointer += sizeof(UV);
					}
					else
					{
						std::memcpy(&UV, &Primitive[Pointer], 0x0C);
						Pointer += 0x0C;
					}

					std::uint16_t TexturePageX = GetTexturePage(UV.TSB) * TexturePageWidth;

					Model->Obj[i][x].Texture.Active = true;
					Model->Obj[i][x].Texture.Transparency = b_Translucency;
					Model->Obj[i][x].Texture.Depth = GetColorMode(UV.TSB);
					Model->Obj[i][x].Texture.iPalette = GetClutID(UV.CBA);
					Model->Obj[i][x].Texture.TransparencyRate = GetTransparencyRate(UV.TSB);

					if (b_Quadrilateral)
					{
						Model->Obj[i][x].UV[0].Set(((float)(UV.U0 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V0) + TexelOffset) / TextureHeight);
						Model->Obj[i][x].UV[1].Set(((float)(UV.U1 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V1) + TexelOffset) / TextureHeight);
						Model->Obj[i][x].UV[2].Set(((float)(UV.U2 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V2) + TexelOffset) / TextureHeight);
						Model->Obj[i][x].UV[3].Set(((float)(UV.U1 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V1) + TexelOffset) / TextureHeight);
						Model->Obj[i][x].UV[4].Set(((float)(UV.U3 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V3) + TexelOffset) / TextureHeight);
						Model->Obj[i][x].UV[5].Set(((float)(UV.U2 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V2) + TexelOffset) / TextureHeight);
					}
					else
					{
						Model->Obj[i][x].UV[0].Set(((float)(UV.U0 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V0) + TexelOffset) / TextureHeight);
						Model->Obj[i][x].UV[1].Set(((float)(UV.U1 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V1) + TexelOffset) / TextureHeight);
						Model->Obj[i][x].UV[2].Set(((float)(UV.U2 + TexturePageX) + TexelOffset) / TextureWidth, ((float)(UV.V2) + TexelOffset) / TextureHeight);
					}
				}

				if (b_RGB)
				{
					if (b_Quadrilateral)
					{
						std::memcpy(&RGB, &Primitive[Pointer], sizeof(RGB));
						Pointer += sizeof(RGB);
					}
					else
					{
						std::memcpy(&RGB, &Primitive[Pointer], 0x0C);
						Pointer += 0x0C;
					}
				}
				else if (b_Solid)
				{
					std::memcpy(&RGB.R0, &Primitive[Pointer], 0x04);
					std::memcpy(&RGB.R1, &Primitive[Pointer], 0x04);
					std::memcpy(&RGB.R2, &Primitive[Pointer], 0x04);
					if (b_Quadrilateral) { std::memcpy(&RGB.R3, &Primitive[Pointer], 0x04); }
					Pointer += 0x04;
				}

				if (b_RGB || b_Solid)
				{
					if (b_Quadrilateral)
					{
						Model->Obj[i][x].Color[0] = RGB.B0 | (RGB.G0 << 8) | (RGB.R0 << 16) | 0xFF << 24;
						Model->Obj[i][x].Color[1] = RGB.B1 | (RGB.G1 << 8) | (RGB.R1 << 16) | 0xFF << 24;
						Model->Obj[i][x].Color[2] = RGB.B2 | (RGB.G2 << 8) | (RGB.R2 << 16) | 0xFF << 24;
						Model->Obj[i][x].Color[3] = RGB.B1 | (RGB.G1 << 8) | (RGB.R1 << 16) | 0xFF << 24;
						Model->Obj[i][x].Color[4] = RGB.B3 | (RGB.G3 << 8) | (RGB.R3 << 16) | 0xFF << 24;
						Model->Obj[i][x].Color[5] = RGB.B2 | (RGB.G2 << 8) | (RGB.R2 << 16) | 0xFF << 24;
					}
					else
					{
						Model->Obj[i][x].Color[0] = RGB.B0 | (RGB.G0 << 8) | (RGB.R0 << 16) | 0xFF << 24;
						Model->Obj[i][x].Color[1] = RGB.B1 | (RGB.G1 << 8) | (RGB.R1 << 16) | 0xFF << 24;
						Model->Obj[i][x].Color[2] = RGB.B2 | (RGB.G2 << 8) | (RGB.R2 << 16) | 0xFF << 24;
					}
				}

				if (b_Light)
				{
					if (b_Quadrilateral) { std::memcpy(&POSITION, &Primitive[Pointer], sizeof(POSITION)); }
					else { std::memcpy(&POSITION, &Primitive[Pointer], 0x06); }

					if (b_Quadrilateral)
					{
						Model->Obj[i][x].Vector[0].Set(ToFloat(Vertice(i, POSITION.V0).vx), ToFloat(Vertice(i, POSITION.V0).vy), ToFloat(Vertice(i, POSITION.V0).vz));
						Model->Obj[i][x].Vector[1].Set(ToFloat(Vertice(i, POSITION.V1).vx), ToFloat(Vertice(i, POSITION.V1).vy), ToFloat(Vertice(i, POSITION.V1).vz));
						Model->Obj[i][x].Vector[2].Set(ToFloat(Vertice(i, POSITION.V2).vx), ToFloat(Vertice(i, POSITION.V2).vy), ToFloat(Vertice(i, POSITION.V2).vz));
						Model->Obj[i][x].Vector[3].Set(ToFloat(Vertice(i, POSITION.V1).vx), ToFloat(Vertice(i, POSITION.V1).vy), ToFloat(Vertice(i, POSITION.V1).vz));
						Model->Obj[i][x].Vector[4].Set(ToFloat(Vertice(i, POSITION.V3).vx), ToFloat(Vertice(i, POSITION.V3).vy), ToFloat(Vertice(i, POSITION.V3).vz));
						Model->Obj[i][x].Vector[5].Set(ToFloat(Vertice(i, POSITION.V2).vx), ToFloat(Vertice(i, POSITION.V2).vy), ToFloat(Vertice(i, POSITION.V2).vz));
					}
					else
					{
						Model->Obj[i][x].Vector[0].Set(ToFloat(Vertice(i, POSITION.V0).vx), ToFloat(Vertice(i, POSITION.V0).vy), ToFloat(Vertice(i, POSITION.V0).vz));
						Model->Obj[i][x].Vector[1].Set(ToFloat(Vertice(i, POSITION.V1).vx), ToFloat(Vertice(i, POSITION.V1).vy), ToFloat(Vertice(i, POSITION.V1).vz));
						Model->Obj[i][x].Vector[2].Set(ToFloat(Vertice(i, POSITION.V2).vx), ToFloat(Vertice(i, POSITION.V2).vy), ToFloat(Vertice(i, POSITION.V2).vz));
					}
				}
				else if (b_LightFlat)
				{
					if (b_Quadrilateral) std::memcpy(&FLAT, &Primitive[Pointer], sizeof(FLAT));
					else { std::memcpy(&FLAT, &Primitive[Pointer], 0x08); }

					if (b_Quadrilateral)
					{
						Model->Obj[i][x].Vector[0].Set(ToFloat(Vertice(i, FLAT.V0).vx), ToFloat(Vertice(i, FLAT.V0).vy), ToFloat(Vertice(i, FLAT.V0).vz));
						Model->Obj[i][x].Vector[1].Set(ToFloat(Vertice(i, FLAT.V1).vx), ToFloat(Vertice(i, FLAT.V1).vy), ToFloat(Vertice(i, FLAT.V1).vz));
						Model->Obj[i][x].Vector[2].Set(ToFloat(Vertice(i, FLAT.V2).vx), ToFloat(Vertice(i, FLAT.V2).vy), ToFloat(Vertice(i, FLAT.V2).vz));
						Model->Obj[i][x].Vector[3].Set(ToFloat(Vertice(i, FLAT.V1).vx), ToFloat(Vertice(i, FLAT.V1).vy), ToFloat(Vertice(i, FLAT.V1).vz));
						Model->Obj[i][x].Vector[4].Set(ToFloat(Vertice(i, FLAT.V3).vx), ToFloat(Vertice(i, FLAT.V3).vy), ToFloat(Vertice(i, FLAT.V3).vz));
						Model->Obj[i][x].Vector[5].Set(ToFloat(Vertice(i, FLAT.V2).vx), ToFloat(Vertice(i, FLAT.V2).vy), ToFloat(Vertice(i, FLAT.V2).vz));

						Model->Obj[i][x].Normal[0].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
						Model->Obj[i][x].Normal[1].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
						Model->Obj[i][x].Normal[2].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
						Model->Obj[i][x].Normal[3].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
						Model->Obj[i][x].Normal[4].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
						Model->Obj[i][x].Normal[5].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
					}
					else
					{
						Model->Obj[i][x].Vector[0].Set(ToFloat(Vertice(i, FLAT.V0).vx), ToFloat(Vertice(i, FLAT.V0).vy), ToFloat(Vertice(i, FLAT.V0).vz));
						Model->Obj[i][x].Vector[1].Set(ToFloat(Vertice(i, FLAT.V1).vx), ToFloat(Vertice(i, FLAT.V1).vy), ToFloat(Vertice(i, FLAT.V1).vz));
						Model->Obj[i][x].Vector[2].Set(ToFloat(Vertice(i, FLAT.V2).vx), ToFloat(Vertice(i, FLAT.V2).vy), ToFloat(Vertice(i, FLAT.V2).vz));

						Model->Obj[i][x].Normal[0].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
						Model->Obj[i][x].Normal[1].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
						Model->Obj[i][x].Normal[2].Set(ToFloat(Normal(i, FLAT.N0).vx), ToFloat(Normal(i, FLAT.N0).vy), ToFloat(Normal(i, FLAT.N0).vz));
					}
				}
				else if (b_LightGouraud)
				{
					if (b_Quadrilateral) { std::memcpy(&GOURAUD, &Primitive[Pointer], sizeof(GOURAUD)); }
					else { std::memcpy(&GOURAUD, &Primitive[Pointer], 0x0C); }

					if (b_Quadrilateral)
					{
						Model->Obj[i][x].Vector[0].Set(ToFloat(Vertice(i, GOURAUD.V0).vx), ToFloat(Vertice(i, GOURAUD.V0).vy), ToFloat(Vertice(i, GOURAUD.V0).vz));
						Model->Obj[i][x].Vector[1].Set(ToFloat(Vertice(i, GOURAUD.V1).vx), ToFloat(Vertice(i, GOURAUD.V1).vy), ToFloat(Vertice(i, GOURAUD.V1).vz));
						Model->Obj[i][x].Vector[2].Set(ToFloat(Vertice(i, GOURAUD.V2).vx), ToFloat(Vertice(i, GOURAUD.V2).vy), ToFloat(Vertice(i, GOURAUD.V2).vz));
						Model->Obj[i][x].Vector[3].Set(ToFloat(Vertice(i, GOURAUD.V1).vx), ToFloat(Vertice(i, GOURAUD.V1).vy), ToFloat(Vertice(i, GOURAUD.V1).vz));
						Model->Obj[i][x].Vector[4].Set(ToFloat(Vertice(i, GOURAUD.V3).vx), ToFloat(Vertice(i, GOURAUD.V3).vy), ToFloat(Vertice(i, GOURAUD.V3).vz));
						Model->Obj[i][x].Vector[5].Set(ToFloat(Vertice(i, GOURAUD.V2).vx), ToFloat(Vertice(i, GOURAUD.V2).vy), ToFloat(Vertice(i, GOURAUD.V2).vz));

						Model->Obj[i][x].Normal[0].Set(ToFloat(Normal(i, GOURAUD.N0).vx), ToFloat(Normal(i, GOURAUD.N0).vy), ToFloat(Normal(i, GOURAUD.N0).vz));
						Model->Obj[i][x].Normal[1].Set(ToFloat(Normal(i, GOURAUD.N1).vx), ToFloat(Normal(i, GOURAUD.N1).vy), ToFloat(Normal(i, GOURAUD.N1).vz));
						Model->Obj[i][x].Normal[2].Set(ToFloat(Normal(i, GOURAUD.N2).vx), ToFloat(Normal(i, GOURAUD.N2).vy), ToFloat(Normal(i, GOURAUD.N2).vz));
						Model->Obj[i][x].Normal[3].Set(ToFloat(Normal(i, GOURAUD.N1).vx), ToFloat(Normal(i, GOURAUD.N1).vy), ToFloat(Normal(i, GOURAUD.N1).vz));
						Model->Obj[i][x].Normal[4].Set(ToFloat(Normal(i, GOURAUD.N3).vx), ToFloat(Normal(i, GOURAUD.N3).vy), ToFloat(Normal(i, GOURAUD.N3).vz));
						Model->Obj[i][x].Normal[5].Set(ToFloat(Normal(i, GOURAUD.N2).vx), ToFloat(Normal(i, GOURAUD.N2).vy), ToFloat(Normal(i, GOURAUD.N2).vz));
					}
					else
					{
						Model->Obj[i][x].Vector[0].Set(ToFloat(Vertice(i, GOURAUD.V0).vx), ToFloat(Vertice(i, GOURAUD.V0).vy), ToFloat(Vertice(i, GOURAUD.V0).vz));
						Model->Obj[i][x].Vector[1].Set(ToFloat(Vertice(i, GOURAUD.V1).vx), ToFloat(Vertice(i, GOURAUD.V1).vy), ToFloat(Vertice(i, GOURAUD.V1).vz));
						Model->Obj[i][x].Vector[2].Set(ToFloat(Vertice(i, GOURAUD.V2).vx), ToFloat(Vertice(i, GOURAUD.V2).vy), ToFloat(Vertice(i, GOURAUD.V2).vz));

						Model->Obj[i][x].Normal[0].Set(ToFloat(Normal(i, GOURAUD.N0).vx), ToFloat(Normal(i, GOURAUD.N0).vy), ToFloat(Normal(i, GOURAUD.N0).vz));
						Model->Obj[i][x].Normal[1].Set(ToFloat(Normal(i, GOURAUD.N1).vx), ToFloat(Normal(i, GOURAUD.N1).vy), ToFloat(Normal(i, GOURAUD.N1).vz));
						Model->Obj[i][x].Normal[2].Set(ToFloat(Normal(i, GOURAUD.N2).vx), ToFloat(Normal(i, GOURAUD.N2).vy), ToFloat(Normal(i, GOURAUD.N2).vz));
					}
				}

				continue;
			}

			if (b_Line)
			{
				if (b_Gouraud)
				{
					std::memcpy(&RGB, &Primitive[Pointer], 0x08);
					Pointer += 0x08;
				}
				else
				{
					std::memcpy(&RGB.R0, &Primitive[Pointer], 0x04);
					std::memcpy(&RGB.R1, &Primitive[Pointer], 0x04);
					Pointer += 0x04;
				}

				std::memcpy(&POSITION, &Primitive[Pointer], 0x04);

				Model->Obj[i][x].Gradation = b_Gouraud;

				Model->Obj[i][x].Color[0] = RGB.B0 | (RGB.G0 << 8) | (RGB.R0 << 16) | 0xFF << 24;
				Model->Obj[i][x].Color[1] = RGB.B1 | (RGB.G1 << 8) | (RGB.R1 << 16) | 0xFF << 24;

				Model->Obj[i][x].Vector[0].Set(ToFloat(Vertice(i, POSITION.V0).vx), ToFloat(Vertice(i, POSITION.V0).vy), ToFloat(Vertice(i, POSITION.V0).vz));
				Model->Obj[i][x].Vector[1].Set(ToFloat(Vertice(i, POSITION.V1).vx), ToFloat(Vertice(i, POSITION.V1).vy), ToFloat(Vertice(i, POSITION.V1).vz));

				continue;
			}

			if (b_Sprite)
			{
				Sony_PlayStation_Model_Sprite_Header SpriteHeader = GetSpriteHeader(i, x);

				if (SpriteHeader.Size) { std::memcpy(&SPRITE, &Primitive[Pointer], 0x08); }
				else { std::memcpy(&SPRITE, &Primitive[Pointer], sizeof(SPRITE)); }

				std::uint16_t TexturePageX = GetTexturePage(UV.TSB) * TexturePageWidth;

				Model->Obj[i][x].Texture.Active = true;
				Model->Obj[i][x].Texture.Transparency = b_Translucency;
				Model->Obj[i][x].Texture.Depth = GetColorMode(UV.TSB);
				Model->Obj[i][x].Texture.iPalette = GetClutID(UV.CBA);
				Model->Obj[i][x].Texture.TransparencyRate = GetTransparencyRate(UV.TSB);

				std::uint16_t SpriteWidth = 0;
				std::uint16_t SpriteHeight = 0;

				switch (SpriteHeader.Size)
				{
				case 0:
					SpriteWidth = SPRITE.W;
					SpriteHeight = SPRITE.H;
					break;
				case 1:
					SpriteWidth = 1;
					SpriteHeight = 1;
					break;
				case 2:
					SpriteWidth = 8;
					SpriteHeight = 8;
					break;
				case 3:
					SpriteWidth = 16;
					SpriteHeight = 16;
					break;
				}

				float HalfW = ToFloat(SpriteWidth) * 0.5f;
				float HalfH = ToFloat(SpriteHeight) * 0.5f;

				Model->Obj[i][x].Vector[0].Set(ToFloat(Vertice(i, SPRITE.V).vx) - HalfW, ToFloat(Vertice(i, SPRITE.V).vy) - HalfH, ToFloat(Vertice(i, SPRITE.V).vz));
				Model->Obj[i][x].Vector[1].Set(ToFloat(Vertice(i, SPRITE.V).vx) + HalfW, ToFloat(Vertice(i, SPRITE.V).vy) - HalfH, ToFloat(Vertice(i, SPRITE.V).vz));
				Model->Obj[i][x].Vector[2].Set(ToFloat(Vertice(i, SPRITE.V).vx) + HalfW, ToFloat(Vertice(i, SPRITE.V).vy) + HalfH, ToFloat(Vertice(i, SPRITE.V).vz));
				Model->Obj[i][x].Vector[3].Set(ToFloat(Vertice(i, SPRITE.V).vx) - HalfW, ToFloat(Vertice(i, SPRITE.V).vy) + HalfH, ToFloat(Vertice(i, SPRITE.V).vz));

				Model->Obj[i][x].UV[0].Set((float)(UV.U0 + TexturePageX) / TextureWidth, (float)(UV.V0) / TextureHeight);
				Model->Obj[i][x].UV[1].Set((float)(UV.U0 + TexturePageX + SpriteWidth) / TextureWidth, (float)(UV.V0) / TextureHeight);
				Model->Obj[i][x].UV[2].Set((float)(UV.U0 + TexturePageX + SpriteWidth) / TextureWidth, (float)(UV.V0 + SpriteHeight) / TextureHeight);
				Model->Obj[i][x].UV[3].Set((float)(UV.U0 + TexturePageX) / TextureWidth, (float)(UV.V0 + SpriteHeight) / TextureHeight);

				continue;
			}
		}
	}

	return Model;
}