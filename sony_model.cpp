/*
*
*	Megan Grass
*	March 07, 2024
*
*
*	TODO:
*
*/


#include "sony_model.h"


/*
	Open
*/
std::uintmax_t Sony_PlayStation_Model::Open(StdFile& File, std::uintmax_t _Ptr)
{
	if (b_Open) { Close(); }

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str->Message("Sony PlayStation Model: Error, could not open at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	Sony_PlayStation_Model_Header Header{};
	File.Read(_Ptr , &Header, sizeof(Sony_PlayStation_Model_Header));

	if ((Header.Magic != 0x41) && (!b_IgnoreMagic))
	{
		Str->Message("Sony PlayStation Model: Error, invalid magic at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
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

		Object.push_back(Obj);
	}

	b_Open = true;

	return _Ptr + Size();
}


/*
	Open
*/
bool Sony_PlayStation_Model::Open(std::filesystem::path Input, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Input);

	Open(m_File, _Ptr);

	return b_Open;
}


/*
	Save
*/
std::uintmax_t Sony_PlayStation_Model::Save(StdFile& File, std::uintmax_t _Ptr)
{
	if (!b_Open)
	{
		Str->Message("Sony PlayStation Model: Error, model is not open");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Write, true, false))
		{
			Str->Message("Sony PlayStation Model: Error, could not create at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	std::uintmax_t pIndex = _Ptr + sizeof(Sony_PlayStation_Model_Header);
	std::uintmax_t pPrimitive = pIndex + (sizeof(Sony_PlayStation_Model_Index) * GetObjectCount());
	std::uintmax_t pVertice = pPrimitive + PrimitiveSize();
	std::uintmax_t pNormal = pVertice + (sizeof(SVECTOR) * GetVerticeCount());

	Sony_PlayStation_Model_Header Header{};
	Header.Magic = 0x41;
	Header.Flag.Address = false;
	Header.nObject = static_cast<std::uint32_t>(GetObjectCount());
	File.Write(_Ptr, &Header, sizeof(Sony_PlayStation_Model_Header));

	for (std::size_t i = 0; i < GetObjectCount(); i++)
	{
		Sony_PlayStation_Model_Index Index{};
		Index.pVertice = static_cast<std::uint32_t>((pVertice - _Ptr) - sizeof(Sony_PlayStation_Model_Header));
		Index.nVertice = static_cast<std::uint32_t>(Object[i].Vertice.size());
		Index.pNormal = static_cast<std::uint32_t>((pNormal - _Ptr) - sizeof(Sony_PlayStation_Model_Header));
		Index.nNormal = static_cast<std::uint32_t>(Object[i].Normal.size());
		Index.pPrimitive = static_cast<std::uint32_t>((pPrimitive - _Ptr) - sizeof(Sony_PlayStation_Model_Header));
		Index.nPrimitive = static_cast<std::uint32_t>(Object[i].Primitive.size());
		Index.Scale = Object[i].Scale;
		File.Write(pIndex, &Index, sizeof(Sony_PlayStation_Model_Index));
		pIndex += sizeof(Sony_PlayStation_Model_Index);

		File.Write(pVertice, Object[i].Vertice.data(), Object[i].Vertice.size() * sizeof(SVECTOR));
		pVertice += Object[i].Vertice.size() * sizeof(SVECTOR);

		File.Write(pNormal, Object[i].Normal.data(), Object[i].Normal.size() * sizeof(SVECTOR));
		pNormal += Object[i].Normal.size() * sizeof(SVECTOR);

		for (std::size_t x = 0; x < Object[i].Primitive.size(); x++)
		{
			File.Write(pPrimitive, Object[i].Primitive[x].Packet.data(), Object[i].Primitive[x].Packet.size());
			pPrimitive += Object[i].Primitive[x].Packet.size();
		}
	}

	return _Ptr + Size();
}


/*
	Save
*/
bool Sony_PlayStation_Model::Save(std::filesystem::path Output, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Output);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = Save(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Save object to individual TMD file
*/
bool Sony_PlayStation_Model::SaveObject(std::filesystem::path Output, std::size_t iObject)
{
	if (!b_Open)
	{
		Str->Message("Sony PlayStation Model: Error, model is not open");
		return false;
	}

	if ((iObject + 1) > Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, object index out of range");
		return false;
	}

	StdFile File{ Output, FileAccessMode::Write, true, false };

	if (!File.IsOpen())
	{
		Str->Message("Sony PlayStation Model: Error, could not create %s", Output.filename().string().c_str());
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
	Index.nVertice = static_cast<std::uint32_t>(Object[iObject].Vertice.size());
	Index.pNormal = static_cast<std::uint32_t>(pNormal - sizeof(Sony_PlayStation_Model_Header));
	Index.nNormal = static_cast<std::uint32_t>(Object[iObject].Normal.size());
	Index.pPrimitive = static_cast<std::uint32_t>(pPrimitive - sizeof(Sony_PlayStation_Model_Header));
	Index.nPrimitive = static_cast<std::uint32_t>(Object[iObject].Primitive.size());
	Index.Scale = Object[iObject].Scale;
	File.Write(pIndex, &Index, sizeof(Sony_PlayStation_Model_Index));

	File.Write(pVertice, Object[iObject].Vertice.data(), Object[iObject].Vertice.size() * sizeof(SVECTOR));

	File.Write(pNormal, Object[iObject].Normal.data(), Object[iObject].Normal.size() * sizeof(SVECTOR));

	for (std::size_t x = 0; x < Object[iObject].Primitive.size(); x++)
	{
		File.Write(pPrimitive, Object[iObject].Primitive[x].Packet.data(), Object[iObject].Primitive[x].Packet.size());
		pPrimitive += Object[iObject].Primitive[x].Packet.size();
	}

	return true;
}


/*
	Save all objects to individual TMD files
*/
bool Sony_PlayStation_Model::SaveAllObjects(std::filesystem::path Directory, std::filesystem::path Stem)
{
	if (!b_Open)
	{
		Str->Message("Sony PlayStation Model: Error, model is not open");
		return false;
	}

	Standard_FileSystem FS;
	std::filesystem::path Dir = FS.GetDirectory(Directory);
	FS.CreateDirectory(Dir / Stem.stem());

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		std::filesystem::path Output = Str->FormatCStyle("%s\\%s\\%s_%02d.tmd", Dir.string().c_str(), Stem.stem().string().c_str(), Stem.stem().string().c_str(), i);
		SaveObject(Output, i);
	}

	return true;
}


/*
	Push back object from file
*/
bool Sony_PlayStation_Model::AddObject(std::filesystem::path Input, std::size_t iObjectSrc)
{
	Sony_PlayStation_Model Tmd { Input };

	if (!Tmd)
	{
		Str->Message("Sony PlayStation Model: Error, could not open %s", Input.filename().string().c_str());
		return false;
	}

	if ((iObjectSrc + 1) > Tmd.Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, source object index out of range");
		return false;
	}

	Object.push_back(Tmd.Object[iObjectSrc]);

	return true;
}


/*
	Paste object from file
*/
bool Sony_PlayStation_Model::PasteObject(std::filesystem::path Input, std::size_t iObjectDst, std::size_t iObjectSrc)
{
	if ((iObjectDst + 1) > Object.size())
	{
		return AddObject(Input, iObjectSrc);
	}

	Sony_PlayStation_Model Tmd { Input };

	if (!Tmd)
	{
		Str->Message("Sony PlayStation Model: Error, could not open %s", Input.filename().string().c_str());
		return false;
	}

	if ((iObjectSrc + 1) > Tmd.Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, source object index out of range");
		return false;
	}

	Object[iObjectDst] = Tmd.Object[iObjectSrc];

	return true;
}


/*
	Insert object from file
*/
bool Sony_PlayStation_Model::InsertObject(std::filesystem::path Input, std::size_t iObjectDst, std::size_t iObjectSrc)
{
	if ((iObjectDst + 1) > Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, destination object index out of range");
		return false;
	}

	Sony_PlayStation_Model Tmd { Input };

	if (!Tmd.IsOpen())
	{
		Str->Message("Sony PlayStation Model: Error, could not open %s", Input.filename().string().c_str());
		return false;
	}

	if ((iObjectSrc + 1) > Tmd.Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, source object index out of range");
		return false;
	}

	Object.insert(Object.begin() + iObjectDst, Tmd.Object[iObjectSrc]);

	return true;
}


/*
	Get total vertice count
*/
std::size_t Sony_PlayStation_Model::GetVerticeCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nVertice = 0;

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		nVertice += Object[i].Vertice.size();
	}

	return nVertice;
}


/*
	Get total normal count
*/
std::size_t Sony_PlayStation_Model::GetNormalCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nNormal = 0;

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		nNormal += Object[i].Normal.size();
	}

	return nNormal;
}


/*
	Get total primitive count
*/
std::size_t Sony_PlayStation_Model::GetPrimitiveCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nPrimitive = 0;

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		nPrimitive += Object[i].Primitive.size();
	}

	return nPrimitive;
}


/*
	Get total triangle count
*/
std::size_t Sony_PlayStation_Model::GetTriangleCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nTriangle = 0;

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		for (std::size_t x = 0; x < Object[i].Primitive.size(); x++)
		{
			if ((Object[i].Primitive[x].Header.Code == 1) && (Object[i].Primitive[x].Header.Quadrilateral == 0))
			{
				nTriangle++;
			}
		}
	}

	return nTriangle;
}


/*
	Get object triangle count
*/
std::size_t Sony_PlayStation_Model::GetTriangleCount(std::size_t iObject) const
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, object index out of range");
		return 0;
	}

	std::size_t nTriangle = 0;

	for (std::size_t x = 0; x < Object[iObject].Primitive.size(); x++)
	{
		if ((Object[iObject].Primitive[x].Header.Code == 1) && (Object[iObject].Primitive[x].Header.Quadrilateral == 0))
		{
			nTriangle++;
		}
	}

	return nTriangle;
}


/*
	Get total quadrangle count
*/
std::size_t Sony_PlayStation_Model::GetQuadrangleCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nQuadrangle = 0;

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		for (std::size_t x = 0; x < Object[i].Primitive.size(); x++)
		{
			if ((Object[i].Primitive[x].Header.Code == 1) && (Object[i].Primitive[x].Header.Quadrilateral == 1))
			{
				nQuadrangle++;
			}
		}
	}

	return nQuadrangle;
}


/*
	Get object quadrangle count
*/
std::size_t Sony_PlayStation_Model::GetQuadrangleCount(std::size_t iObject) const
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, object index out of range");
		return 0;
	}

	std::size_t nQuadrangle = 0;

	for (std::size_t x = 0; x < Object[iObject].Primitive.size(); x++)
	{
		if ((Object[iObject].Primitive[x].Header.Code == 1) && (Object[iObject].Primitive[x].Header.Quadrilateral == 1))
		{
			nQuadrangle++;
		}
	}

	return nQuadrangle;
}


/*
	Get total line count
*/
std::size_t Sony_PlayStation_Model::GetLineCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nLine = 0;

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		for (std::size_t x = 0; x < Object[i].Primitive.size(); x++)
		{
			if (Object[i].Primitive[x].Header.Code == 2)
			{
				nLine++;
			}
		}
	}

	return nLine;

}


/*
	Get object line count
*/
std::size_t Sony_PlayStation_Model::GetLineCount(std::size_t iObject) const
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, object index out of range");
		return 0;
	}

	std::size_t nLine = 0;

	for (std::size_t x = 0; x < Object[iObject].Primitive.size(); x++)
	{
		if (Object[iObject].Primitive[x].Header.Code == 2)
		{
			nLine++;
		}
	}

	return nLine;
}


/*
	Get total sprite count
*/
std::size_t Sony_PlayStation_Model::GetSpriteCount(void) const
{
	if (!b_Open) { return 0; }

	std::size_t nSprite = 0;

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		for (std::size_t x = 0; x < Object[i].Primitive.size(); x++)
		{
			if (Object[i].Primitive[x].Header.Code == 3)
			{
				nSprite++;
			}
		}
	}

	return nSprite;
}


/*
	Get object sprite count
*/
std::size_t Sony_PlayStation_Model::GetSpriteCount(std::size_t iObject) const
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, object index out of range");
		return 0;
	}

	std::size_t nSprite = 0;

	for (std::size_t x = 0; x < Object[iObject].Primitive.size(); x++)
	{
		if (Object[iObject].Primitive[x].Header.Code == 3)
		{
			nSprite++;
		}
	}

	return nSprite;
}


/*
	Get primitive type
*/
Sony_PlayStation_Primitive_Type Sony_PlayStation_Model::GetType(std::size_t iObject, std::size_t iPrimitive) const
{
	if (!b_Open) { return Sony_PlayStation_Primitive_Type::Unknown; }

	if ((iObject + 1) > Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, object index out of range");
		return Sony_PlayStation_Primitive_Type::Unknown;
	}

	if ((iPrimitive + 1) > Object[iObject].Primitive.size())
	{
		Str->Message("Sony PlayStation Model: Error, primitive index out of range");
		return Sony_PlayStation_Primitive_Type::Unknown;
	}

	switch (Object[iObject].Primitive[iPrimitive].Header.Code)
	{
	case 1:
		return Sony_PlayStation_Primitive_Type::Polygon;
	case 2:
		return Sony_PlayStation_Primitive_Type::Line;
	case 3:
		return Sony_PlayStation_Primitive_Type::Sprite;
	}

	return Sony_PlayStation_Primitive_Type::Unknown;
}


/*
	Get sprite texture dimensions
*/
void Sony_PlayStation_Model::GetSpriteDimension(std::size_t iObject, std::size_t iPrimitive, std::uint16_t& Width, std::uint16_t& Height) const
{
	Sony_PlayStation_Primitive_Type Type = GetType(iObject, iPrimitive);

	if (Type != Sony_PlayStation_Primitive_Type::Sprite)
	{
		Str->Message("Sony PlayStation Model: Error, primitive is not a sprite");
		return;
	}

	std::uint32_t Size = Object[iObject].Primitive[iPrimitive].Header.Quadrilateral | Object[iObject].Primitive[iPrimitive].Header.Shader;

	switch (Size)
	{
	case 0:
		break;
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

	if (Object[iObject].Primitive[iPrimitive].Packet.size() != 16)
	{
		Str->Message("Sony PlayStation Model: Error, invalid packet size for free-size sprite");
		Width = 0;
		Height = 0;
		return;
	}

	std::memcpy(&Width, &Object[iObject].Primitive[iPrimitive].Packet.data()[12], sizeof(std::uint16_t));
	std::memcpy(&Height, &Object[iObject].Primitive[iPrimitive].Packet.data()[14], sizeof(std::uint16_t));

}


/*
	Get texture CLUT X/Y coordinates
*/
void Sony_PlayStation_Model::GetClutXY(std::uint16_t CLUT, std::uint16_t& X, std::uint16_t& Y) const
{
	Sony_PlayStation_Model_Clut Clut{ CLUT };

	X = Clut.X;
	Y = Clut.Y;
}


/*
	Get texture page
*/
std::uint16_t Sony_PlayStation_Model::GetTexturePage(std::uint16_t Page) const
{
	Sony_PlayStation_Model_Texture Texture{ Page };

	return Texture.Page;
}


/*
	Get texture color mode
*/
std::uint16_t Sony_PlayStation_Model::GetColorMode(std::uint16_t Page) const
{
	Sony_PlayStation_Model_Texture Texture{ Page };

	switch (Texture.ColorMode)
	{
	case 0:
		return 4;
	case 1:
		return 8;
	case 2:
		return 15;
	case 3:
		return 24;
	}

	return 0;
}


/*
	Get texture transparency rate
*/
Sony_PlayStation_Transparency_Rate Sony_PlayStation_Model::GetTransparencyRate(std::uint16_t Page) const
{
	Sony_PlayStation_Model_Texture Texture{ Page };

	switch (Texture.SemiTrans)
	{
	case 0:
		return Sony_PlayStation_Transparency_Rate::Half;
	case 1:
		return Sony_PlayStation_Transparency_Rate::Full;
	case 2:
		return Sony_PlayStation_Transparency_Rate::Inverse;
	case 3:
		return Sony_PlayStation_Transparency_Rate::Quarter;
	}

	return Sony_PlayStation_Transparency_Rate::Half;
}


/*
	Get total primitive block size
*/
std::size_t Sony_PlayStation_Model::PrimitiveSize(void) const
{
	if (!b_Open) { return 0; }

	std::size_t PrimitiveSize = 0;

	for (std::size_t i = 0; i < Object.size(); i++)
	{
		for (std::size_t x = 0; x < Object[i].Primitive.size(); x++)
		{
			PrimitiveSize += Object[i].Primitive[x].Packet.size();
		}
	}

	return PrimitiveSize;
}


/*
	Get object primitive block size
*/
std::size_t Sony_PlayStation_Model::PrimitiveSize(std::size_t iObject) const
{
	if (!b_Open) { return 0; }

	if ((iObject + 1) > Object.size())
	{
		Str->Message("Sony PlayStation Model: Error, object index out of range");
		return 0;
	}

	std::size_t PrimitiveSize = 0;

	for (std::size_t x = 0; x < Object[iObject].Primitive.size(); x++)
	{
		PrimitiveSize += Object[iObject].Primitive[x].Packet.size();
	}

	return PrimitiveSize;
}


/*
	Get file size
*/
std::uintmax_t Sony_PlayStation_Model::Size(void) const
{
	if (!b_Open) { return 0; }

	std::uintmax_t pIndex = sizeof(Sony_PlayStation_Model_Header);
	std::uintmax_t pPrimitive = pIndex + (sizeof(Sony_PlayStation_Model_Index) * GetObjectCount());
	std::uintmax_t pVertice = pPrimitive + PrimitiveSize();
	std::uintmax_t pNormal = pVertice + (sizeof(SVECTOR) * GetVerticeCount());
	std::uintmax_t Size = pNormal + (sizeof(SVECTOR) * GetNormalCount());

	return Size;
}


/*
	Close
*/
void Sony_PlayStation_Model::Close(void)
{
	b_Open = false;
	for (std::size_t i = 0; i < Object.size(); i++)
	{
		Object[i].Vertice.clear();
		Object[i].Normal.clear();
		for (std::size_t x = 0; x < Object[i].Primitive.size(); x++)
		{
			std::memset(&Object[i].Primitive[x].Header, 0, sizeof(Sony_PlayStation_Model_Primitive::Header));
			Object[i].Primitive[x].Packet.clear();
		}
		Object[i].Primitive.clear();
	}
	Object.clear();
}