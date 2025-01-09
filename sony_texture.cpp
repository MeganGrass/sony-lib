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


#include "sony_texture.h"


/*
	Special Transparency Processing (STP Flag) Color
*/
DWORD Sony_PlayStation_Texture::TransparentColor = RGB(0xFF, 0, 0xFF);


/*
	Texture information
*/
String Sony_PlayStation_Texture::About(void)
{
	Standard_String Str;

	String Output = Str.FormatCStyle("PlayStation Texture: \n");
	if (!b_Open)
	{
		Str.Message("PlayStation Texture: Error, texture is not open");
		return Output;
	}

	Output += Str.FormatCStyle("\tID: %d\n", Header.ID);
	Output += Str.FormatCStyle("\tVersion: %d\n", Header.Version);
	Output += Str.FormatCStyle("\tMode: %d (%dbpp)\n", Header.Mode, GetDepth());
	Output += Str.FormatCStyle("\tCF: %d\n", Header.ClutFlag);
	Output += Str.FormatCStyle("\tCLUT X: %d\n", Clut.X);
	Output += Str.FormatCStyle("\tCLUT Y: %d\n", Clut.Y);
	Output += Str.FormatCStyle("\tCLUT nColor: %d\n", Clut.nColor);
	Output += Str.FormatCStyle("\tCLUT nPalette: %d\n", Clut.nPalette);
	Output += Str.FormatCStyle("\tData X: %d\n", Data.X);
	Output += Str.FormatCStyle("\tData Y: %d\n", Data.Y);
	Output += Str.FormatCStyle("\tWidth: %d\n", GetWidth());
	Output += Str.FormatCStyle("\tHeight: %d\n", GetHeight());
	Output += Str.FormatCStyle("\tPixels: 0x%X bytes\n", Pixels.size());

	return Output;
}


/*
	Print texture information
*/
String Sony_PlayStation_Texture::Print(void)
{
	Standard_String Str;

	String Output = About();

	std::cout << Output << std::endl;

	return Output;
}


/*
	Create
*/
bool Sony_PlayStation_Texture::Create(std::uint32_t _Depth, std::uint16_t _Width, std::uint16_t _Height, std::uint16_t nPalette)
{
	if (b_Open) { Close(); }

	if ((_Depth != 4) && (_Depth != 8) && (_Depth != 16) && (_Depth != 24))
	{
		Str->Message("PlayStation Texture: Error, invalid depth: %d", _Depth);
		return false;
	}

	Header.ID = 0x10;
	Header.Version = 0;
	switch (_Depth)
	{
	case 4:
		Header.Mode = 0;
		Header.ClutFlag = true;
		break;
	case 8:
		Header.Mode = 1;
		Header.ClutFlag = true;
		break;
	case 16:
		Header.Mode = 2;
		Header.ClutFlag = false;
		break;
	case 24:
		Header.Mode = 3;
		Header.ClutFlag = false;
		break;
	}

	if (Header.ClutFlag)
	{
		Clut.X = 0;
		Clut.Y = 0;

		switch (Header.Mode)
		{
		case 0:
			Clut.nColor = 16;
			break;
		case 1:
			Clut.nColor = 256;
			break;
		}

		Clut.nPalette = nPalette;
		Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

		Palette.resize(Clut.nPalette);

		for (std::size_t i = 0; i < Palette.size(); i++)
		{
			Palette[i].resize(Clut.nColor);

			for (std::size_t x = 0; x < Palette[i].size(); x++)
			{
				if (Header.Mode == 0)
				{
					Palette[i][x] = Create16bpp((uint8_t)x * 16, (uint8_t)x * 16, (uint8_t)x * 16, true);
				}
				else
				{
					Palette[i][x] = Create16bpp((uint8_t)x, (uint8_t)x, (uint8_t)x, true);
				}
			}
		}
	}

	Data.X = 0;
	Data.Y = 0;
	switch (Header.Mode)
	{
	case 0:
		Data.Width = _Width / 4;
		break;
	case 1:
		Data.Width = _Width / 2;
		break;
	case 2:
		Data.Width = _Width;
		break;
	case 3:
		Data.Width = (_Width * 3) / 2;
		break;
	}
	Data.Height = _Height;
	switch (Header.Mode)
	{
	case 0:
	case 1:
	case 2:
		Data.Size = (((Data.Width * Data.Height) * 2) + sizeof(Sony_Texture_Data));
		break;
	case 3:
		Data.Size = (((Data.Width * Data.Height) * 3) + sizeof(Sony_Texture_Data));
		break;
	}

	Pixels.resize(Data.Size ^ sizeof(Sony_Texture_Data));

	b_Open = true;

	Print();

	return b_Open;
}


/*
	Open
*/
std::uintmax_t Sony_PlayStation_Texture::Open(StdFile& File, std::uintmax_t _Ptr)
{
	if (b_Open) { Close(); }

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str->Message("PlayStation Texture: Error, could not open at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	File.Read(_Ptr, &Header, sizeof(Sony_Texture_Header));

	if ((Header.ID != 0x10) ||
		(Header.Version != 0) ||
		(Header.Reserved0 != 0) ||
		((Header.Mode != 0) && (Header.Mode != 1) && (Header.Mode != 2) && (Header.Mode != 3) && (Header.Mode != 4)) ||
		((Header.ClutFlag != 0) && (Header.ClutFlag != 1)) ||
		(Header.Reserved1 != 0))
	{
		Str->Message("PlayStation Texture: Error, invalid header at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
		Close();
		return _Ptr;
	}

	if (Header.Mode == 4)
	{
		Str->Message("PlayStation Texture: Error, mixed-mode is unsupported");
		Close();
		return _Ptr;
	}

	_Ptr += sizeof(Sony_Texture_Header);

	if (Header.ClutFlag)
	{
		File.Read(_Ptr, &Clut, sizeof(Sony_Texture_Clut));

		_Ptr += sizeof(Sony_Texture_Clut);

		switch (Header.Mode)
		{
		case 0:
			if (Clut.nColor > 16)
			{
				Clut.nColor /= 2;
				Clut.nPalette *= 2;
			}
			break;
		case 1:
			if (Clut.nColor > 256)
			{
				Clut.nColor /= 2;
				Clut.nPalette *= 2;
			}
			break;
		}

		Palette.resize(Clut.nPalette);

		for (std::uint16_t i = 0; i < Clut.nPalette; i++)
		{
			Palette[i].resize(Clut.nColor);

			for (std::uint16_t x = 0; x < Clut.nColor; x++, _Ptr += sizeof(Sony_Texture_16bpp))
			{
				File.Read(_Ptr, &Palette[i][x], sizeof(Sony_Texture_16bpp));
			}
		}
	}

	File.Read(_Ptr, &Data, sizeof(Sony_Texture_Data));

	_Ptr += sizeof(Sony_Texture_Data);

	std::size_t PixelSize = Data.Size;

	if (PixelSize & sizeof(Sony_Texture_Data)) { PixelSize ^= sizeof(Sony_Texture_Data); }

	Pixels.resize(PixelSize);

	File.Read(_Ptr, Pixels.data(), Pixels.size());

	b_Open = true;

	return _Ptr + Pixels.size();
}


/*
	Open
*/
bool Sony_PlayStation_Texture::Open(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	Open(m_File, _Ptr);

	return b_Open;
}


/*
	Save
*/
std::uintmax_t Sony_PlayStation_Texture::Save(StdFile& File, std::uintmax_t _Ptr)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return _Ptr;
	}

	if (Palette.empty() && Pixels.empty())
	{
		Str->Message("PlayStation Texture: Error, palette and pixel data is empty");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			Str->Message("PlayStation Texture: Error, could not create at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	File.Write(_Ptr, &Header, sizeof(Sony_Texture_Header));

	_Ptr += sizeof(Sony_Texture_Header);

	if (Header.ClutFlag)
	{
		File.Write(_Ptr, &Clut, sizeof(Sony_Texture_Clut));

		_Ptr += sizeof(Sony_Texture_Clut);

		for (std::uint16_t i = 0; i < Clut.nPalette; i++)
		{
			for (std::uint16_t x = 0; x < Clut.nColor; x++, _Ptr += sizeof(Sony_Texture_16bpp))
			{
				File.Write(_Ptr, &Palette[i][x], sizeof(Sony_Texture_16bpp));
			}
		}
	}

	File.Write(_Ptr, &Data, sizeof(Sony_Texture_Data));

	_Ptr += sizeof(Sony_Texture_Data);

	File.Write(_Ptr, Pixels.data(), Pixels.size());

	return _Ptr + Pixels.size();
}


/*
	Save
*/
bool Sony_PlayStation_Texture::Save(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = Save(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Search
*/
std::vector<std::pair<std::uintmax_t, std::uintmax_t>> Sony_PlayStation_Texture::Search(StdFile& File, std::uintmax_t _Ptr)
{
	std::vector<std::pair<std::uintmax_t, std::uintmax_t>> SearchResults;

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str->Message("PlayStation Texture: Error, could not open at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return SearchResults;
		}
	}

	std::uintmax_t FileSize = File.Size();

	std::uintmax_t OldPtr = 0;

	while (_Ptr < FileSize)
	{
		Sony_Texture_Header Header;
		File.Read(_Ptr, &Header, sizeof(Sony_Texture_Header));

		if ((Header.ID == 0x10) &&
			(Header.Version == 0) &&
			(Header.Reserved0 == 0) &&
			((Header.Mode == 0) || (Header.Mode == 1) || (Header.Mode == 2) || (Header.Mode == 3) && (Header.Mode != 4)) &&
			((Header.ClutFlag == 0) || (Header.ClutFlag == 1)) &&
			(Header.Reserved1 == 0))
		{
			if (Header.Mode == 4) { _Ptr++; continue; }

			OldPtr = _Ptr;

			_Ptr += sizeof(Sony_Texture_Header);

			if (Header.ClutFlag)
			{
				Sony_Texture_Clut Clut;
				File.Read(_Ptr, &Clut, sizeof(Sony_Texture_Clut));

				if ((Clut.X & 0x0F) || (Clut.X < 0) || (Clut.X > 1024))
				{
					_Ptr = ++OldPtr;
					continue;
				}

				if ((Clut.Y < 0) || (Clut.Y > 512))
				{
					_Ptr = ++OldPtr;
					continue;
				}

				_Ptr += sizeof(Sony_Texture_Clut);
				_Ptr += (Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp)));
			}

			Sony_Texture_Data Data;
			File.Read(_Ptr, &Data, sizeof(Sony_Texture_Data));

			_Ptr += sizeof(Sony_Texture_Data);

			std::size_t PixelSize = Data.Size;
			if (PixelSize & sizeof(Sony_Texture_Data)) { PixelSize ^= sizeof(Sony_Texture_Data); }

			_Ptr += PixelSize;

			if (!Header.ClutFlag)
			{
				if ((Data.Width == 0) ||
					(Data.Height == 0) ||
					(Data.Width > 1024) ||
					(Data.Height > 512))
				{
					_Ptr = ++OldPtr;
					continue;
				}
			}

			std::uint32_t DataSize = 0;
			switch (Header.Mode)
			{
			case 0:
			case 1:
			case 2:
				DataSize = (((Data.Width * Data.Height) * 2));
				break;
			case 3:
				DataSize = (((Data.Width * Data.Height) * 3));
				break;
			}
			if (DataSize != PixelSize)
			{
				// std::cout << "DataSize: 0x" << std::hex << DataSize << std::dec << " != PixelSize: 0x" << std::hex << PixelSize << std::dec << std::endl;
				_Ptr = ++OldPtr;
				continue;
			}

			SearchResults.push_back(std::make_pair(OldPtr, (_Ptr - OldPtr)));
			//std::cout << "Found at 0x" << std::hex << OldPtr << std::dec << " with size 0x" << std::hex << (_Ptr - OldPtr) << std::dec << std::endl;
		}
		else
		{
			_Ptr++;
		}
	}

	return SearchResults;
}


/*
	Search
*/
std::vector<std::pair<std::uintmax_t, std::uintmax_t>> Sony_PlayStation_Texture::Search(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	return Search(m_File, _Ptr);
}


/*
	Close
*/
void Sony_PlayStation_Texture::Close(void)
{
	b_Open = false;
	std::memset(&Header, 0, sizeof(Sony_Texture_Header));
	std::memset(&Clut, 0, sizeof(Sony_Texture_Clut));
	std::memset(&Data, 0, sizeof(Sony_Texture_Data));
	if (!Palette.empty())
	{
		for (std::size_t i = 0; i < Palette.size(); i++)
		{
			Palette[i].clear();
		}
		Palette.clear();
	}
	Pixels.clear();
}


/*
	Get file size
*/
std::size_t Sony_PlayStation_Texture::Size(void) const
{
	if (!b_Open) { return 0; }

	std::size_t FileSize = sizeof(Sony_Texture_Header);

	if (Header.ClutFlag)
	{
		std::size_t PaletteSize = Clut.Size;

		if ((PaletteSize & sizeof(Sony_Texture_Clut)) == 0) { PaletteSize |= sizeof(Sony_Texture_Clut); }

		FileSize += PaletteSize;
	}

	std::size_t PixelSize = Data.Size;

	if ((PixelSize & sizeof(Sony_Texture_Data)) == 0) { PixelSize |= sizeof(Sony_Texture_Data); }

	FileSize += PixelSize;

	return FileSize;
}


/*
	Get depth
*/
std::uint32_t Sony_PlayStation_Texture::GetDepth(void) const
{
	if (!b_Open) { return 0; }

	std::uint32_t Depth = 0;

	switch (Header.Mode)
	{
	case 0: Depth = 4; break;
	case 1: Depth = 8; break;
	case 2: Depth = 16; break;
	case 3: Depth = 24; break;
	}

	return Depth;
}


/*
	Get width
*/
std::uint32_t Sony_PlayStation_Texture::GetWidth(void) const
{
	if (!b_Open) { return 0; }

	std::uint32_t Width = 0;

	switch (Header.Mode)
	{
	case 0:
		Width = Data.Width * 4;
		break;
	case 1:
		Width = Data.Width * 2;
		break;
	case 2:
		Width = Data.Width;
		break;
	case 3:
		Width = ((Data.Width * 2) / 3);
		break;
	}

	return Width;
}


/*
	Set width
*/
bool Sony_PlayStation_Texture::SetWidth(std::uint16_t _Width)
{
	if (!b_Open) { return false; }

	switch (Header.Mode)
	{
	case 0:
		Data.Width = _Width / 4;
		break;
	case 1:
		Data.Width = _Width / 2;
		break;
	case 2:
		Data.Width = _Width;
		break;
	case 3:
		Data.Width = (_Width * 3) / 2;
		break;
	}

	return false;
}


/*
	Get height
*/
std::uint32_t Sony_PlayStation_Texture::GetHeight(void) const
{
	if (!b_Open) { return 0; }

	return Data.Height;
}


/*
	Set height
*/
bool Sony_PlayStation_Texture::SetHeight(std::uint16_t _Height)
{
	if (!b_Open) { return false; }

	Data.Height = _Height;

	return true;
}


/*
	Get converted palette depth
*/
std::vector<std::vector<Sony_Texture_16bpp>> Sony_PlayStation_Texture::GetPalette(std::uint32_t _Depth)
{
	if (!b_Open) { return Palette; }

	if (Palette.empty()) { return Palette; }

	if (_Depth == GetDepth()) { return Palette; }

	std::vector<std::vector<Sony_Texture_16bpp>> NewPalette(Clut.nPalette);

	for (std::size_t i = 0; i < Palette.size(); i++)
	{
		_Depth == 4 ? NewPalette[i].resize(16) : NewPalette[i].resize(256);

		if (i >= NewPalette.size()) { break; }

		for (std::size_t x = 0; x < Palette[i].size(); x++)
		{
			if (x >= NewPalette[i].size()) { break; }

			NewPalette[i][x] = Palette[i][x];
		}
	}

	return NewPalette;
}


/*
	Convert unsigned char vector to palette
*/
std::vector<std::vector<Sony_Texture_16bpp>> Sony_PlayStation_Texture::ConvertToPalette(std::vector<std::uint8_t> Source) const
{
	std::uint32_t nColors = GetDepth() == 4 ? 16 : 256;
	std::uint32_t PaletteSize = (nColors * sizeof(Sony_Texture_16bpp));

	if (Source.empty()) { return std::vector<std::vector<Sony_Texture_16bpp>> {}; }

	if (Source.size() > static_cast<unsigned long long>(PaletteSize) * MAX_TIM_PALETTE)
	{
		std::cout << "Attempting to read too many palettes (" << Source.size() / PaletteSize << ")" << ", limiting to " << MAX_TIM_PALETTE << std::endl;
		PaletteSize = MAX_TIM_PALETTE;
	}

	if (Source.size() % PaletteSize)
	{
		Source.resize(Source.size() + (PaletteSize - (Source.size() % PaletteSize)));
	}

	std::size_t nPalette = Source.size() / PaletteSize;

	nPalette = std::clamp(nPalette, static_cast<std::size_t>(0), static_cast<std::size_t>(MAX_TIM_PALETTE));

	std::vector<std::vector<Sony_Texture_16bpp>> NewPalette(nPalette);

	for (std::size_t i = 0; i < nPalette; ++i)
	{
		NewPalette[i].resize(nColors);

		for (std::size_t x = 0; x < nColors; ++x)
		{
			std::size_t pSource = (i * nColors + x) * sizeof(Sony_Texture_16bpp);

			if (pSource + sizeof(Sony_Texture_16bpp) <= Source.size())
			{
				std::memcpy(&NewPalette[i][x], &Source[pSource], sizeof(Sony_Texture_16bpp));
			}
		}
	}

	return NewPalette;
}


/*
	Export all palettes as raw data
*/
void Sony_PlayStation_Texture::ExportPalette(std::filesystem::path _Filename)
{
	if (!b_Open)
	{
		std::cout << "PlayStation Texture: Error, texture is not open" << std::endl;
		return;
	}

	if (!Header.ClutFlag) { return; }

	if (!Clut.nPalette) { return; }

	if (!Clut.nColor) { return; }

	if (Palette.empty()) { return; }

	StdFile m_Input{ _Filename, FileAccessMode::Write_Ex, true, true };
	if (!m_Input.IsOpen())
	{
		Str->Message(L"PlayStation Texture: Error, could not open \"%ws\"", _Filename.filename().wstring().c_str());
		return;
	}

	if (Header.ClutFlag)
	{
		std::uintmax_t _Ptr = 0;

		for (std::uint16_t i = 0; i < Clut.nPalette; i++)
		{
			for (std::uint16_t x = 0; x < Clut.nColor; x++, _Ptr += sizeof(Sony_Texture_16bpp))
			{
				m_Input.Write(_Ptr, &Palette[i][x], sizeof(Sony_Texture_16bpp));
			}
		}
	}
}


/*
	Export palette from file
	 - single palette is exported from iClut index
*/
bool Sony_PlayStation_Texture::ExportMicrosoftPalette(std::filesystem::path Filename, std::size_t iClut)
{
	if (!b_Open)
	{
		std::cout << "PlayStation Texture: Error, texture is not open" << std::endl;
		return false;
	}

	if (Palette.empty())
	{
		std::cout << "PlayStation Texture: Error, palette is empty" << std::endl;
		return false;
	}

	StdFile m_Input{ Filename, FileAccessMode::Write_Ex, true, true };
	if (!m_Input)
	{
		Str->Message("PlayStation Texture: Error, could not create %s", Filename.filename().c_str());
		return false;
	}

	iClut = std::clamp(iClut, static_cast<std::size_t>(0), Palette.size() - 1);

	std::uint16_t DataSize = GetDepth() == 4 ? 16 : 256 * 4 + 4;
	std::uint16_t Version = 0x0300;
	std::uint16_t nColors = GetDepth() == 4 ? 16 : 256;
	std::uintmax_t pPalette = 0x16;
	std::uint32_t FileSize = 0x0A + DataSize;

	// RIFF Header
	m_Input.WriteStr(0x00, "RIFF\0");
	m_Input.Write(0x04, &FileSize, 4);
	m_Input.WriteStr(0x08, "PAL \0");
	m_Input.WriteStr(0x0C, "data\0");
	m_Input.Write(0x10, &DataSize, 2);
	m_Input.Write(0x12, &Version, 2);
	m_Input.Write(0x14, &nColors, 2);

	// Colors
	RGBQUAD Color{};
	for (std::size_t i = 0; i < nColors; i++)
	{
		Color.rgbRed = Red(Palette[iClut][i]);
		Color.rgbGreen = Green(Palette[iClut][i]);
		Color.rgbBlue = Blue(Palette[iClut][i]);
		Color.rgbReserved = STP(Palette[iClut][i]);

		m_Input.Write(pPalette, &Color, sizeof(RGBQUAD));

		pPalette += sizeof(RGBQUAD);
	}

	return true;
}


/*
	Import palette
*/
bool Sony_PlayStation_Texture::ImportPalette(std::vector<Sony_Texture_16bpp> _Palette, std::size_t iClut)
{
	if (!b_Open)
	{
		std::cout << "PlayStation Texture: Error, texture is not open" << std::endl;
		return false;
	}

	if (_Palette.empty())
	{
		std::cout << "PlayStation Texture: Error, import palette is empty" << std::endl;
		return false;
	}

	if (Palette.empty())
	{
		Palette.resize(1);
		Palette[0].resize(GetDepth() == 4 ? 16 : 256);
		Clut.nPalette = 1;
	}

	Header.ClutFlag = true;

	Clut.nColor = GetDepth() == 4 ? 16 : 256;

	iClut = std::clamp(iClut, static_cast<std::size_t>(0), Palette.size() - 1);

	for (std::size_t i = 0; i < Palette[iClut].size(); i++)
	{
		if (i >= _Palette.size()) { break; }

		Palette[iClut][i] = _Palette[i];
	}

	return true;
}


/*
	Import palette
*/
bool Sony_PlayStation_Texture::ImportPalette(std::vector<std::vector<Sony_Texture_16bpp>> _Palette)
{
	if (!b_Open)
	{
		std::cout << "PlayStation Texture: Error, texture is not open" << std::endl;
		return false;
	}

	if (_Palette.empty())
	{
		std::cout << "PlayStation Texture: Error, import palette is empty" << std::endl;
		return false;
	}

	Header.ClutFlag = true;

	Palette.clear();

	Palette.resize(Clut.nPalette = static_cast<std::uint16_t>(_Palette.size()));

	Clut.nColor = GetDepth() == 4 ? 16 : 256;

	for (std::size_t i = 0; i < Palette.size(); i++)
	{
		if (i >= _Palette.size()) { break; }

		Palette[i].resize(Clut.nColor);

		for (std::size_t x = 0; x < Palette[i].size(); x++)
		{
			if (x >= _Palette[i].size()) { break; }

			Palette[i][x] = _Palette[i][x];
		}
	}

	return true;
}


/*
	Import palette from unsigned char vector
*/
bool Sony_PlayStation_Texture::ImportPalette(std::vector<std::uint8_t> Source)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return false;
	}

	return ImportPalette(ConvertToPalette(Source));
}


/*
	Import palette from file
*/
bool Sony_PlayStation_Texture::ImportPalette(StdFile& File, std::uintmax_t _Ptr)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str->Message(L"PlayStation Texture: Error, could not open \"%ws\"", File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	std::vector<std::uint8_t> Source(File.Size());

	File.Read(_Ptr, Source.data(), Source.size());

	return ImportPalette(Source);
}


/*
	Import palette from file
	 - entire palette is completely replaced
*/
bool Sony_PlayStation_Texture::ImportPalette(std::filesystem::path Filename, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Filename);

	return ImportPalette(m_File, _Ptr);
}


/*
	Import palette from file
*/
bool Sony_PlayStation_Texture::ImportMicrosoftPalette(std::filesystem::path Filename, std::size_t iClut)
{
	if (!b_Open)
	{
		std::cout << "PlayStation Texture: Error, texture is not open" << std::endl;
		return false;
	}

	if (Palette.empty())
	{
		Palette.resize(1);
		Palette[0].resize(GetDepth() == 4 ? 16 : 256);
		Header.ClutFlag = true;
		Clut.nPalette = 1;
		Clut.nColor = GetDepth() == 4 ? 16 : 256;
	}

	StdFile m_Input{ Filename, FileAccessMode::Read_Ex, true, false };
	if (!m_Input.IsOpen())
	{
		Str->Message("Error, could not open file \"%ws\" for reading palette data", Filename.filename().wstring().c_str());
		return false;
	}

	iClut = std::clamp(iClut, static_cast<std::size_t>(0), Palette.size() - 1);

	std::uint16_t DataSize = 0;
	std::uint16_t nColors = 0;

	m_Input.Read(0x10, &DataSize, 2);
	m_Input.Read(0x14, &nColors, 2);

	RGBQUAD Color{};
	std::vector<Sony_Texture_16bpp> NewPalette(nColors);

	for (std::size_t i = 0; i < nColors; i++)
	{
		m_Input.Read(0x16 + (i * sizeof(RGBQUAD)), &Color, sizeof(RGBQUAD));

		NewPalette[i] = Create16bpp(Color.rgbRed, Color.rgbGreen, Color.rgbBlue, Color.rgbReserved);
	}

	return ImportPalette(NewPalette, iClut);
}


/*
	Add palette
*/
void Sony_PlayStation_Texture::AddPalette(void)
{
	if (!b_Open) { return; }

	if (Palette.empty())
	{
		Header.ClutFlag = true;
		Clut.nPalette = 0;
	}

	Palette.resize(++Clut.nPalette);

	Palette[Palette.size() - 1] = std::vector<Sony_Texture_16bpp>(GetDepth() == 4 ? 16 : 256);

	Clut.nColor = GetDepth() == 4 ? 16 : 256;

	if (GetDepth() == 4)
	{
		for (std::size_t i = 0; i < Palette[Palette.size() - 1].size(); i++)
		{
			Palette[Palette.size() - 1][i] = Create16bpp((uint8_t)i * 16, (uint8_t)i * 16, (uint8_t)i * 16, true);
		}
	}
	else //if (GetDepth() == 8)
	{
		for (std::size_t i = 0; i < Palette[Palette.size() - 1].size(); i++)
		{
			Palette[Palette.size() - 1][i] = Create16bpp((uint8_t)i, (uint8_t)i, (uint8_t)i, true);
		}
	}

	/*Palette[Palette.size() - 1][1] = Create16bpp(123, 165, 255, true);
	Palette[Palette.size() - 1][2] = Create16bpp(255, 123, 74, true);
	Palette[Palette.size() - 1][3] = Create16bpp(255, 165, 198, true);
	Palette[Palette.size() - 1][4] = Create16bpp(107, 255, 156, true);*/
}


/*
	Insert palette
*/
void Sony_PlayStation_Texture::InsertPalette(std::size_t iClut)
{
	if (Palette.empty())
	{
		AddPalette();
		return;
	}

	if (!b_Open) { return; }

	if (!Header.ClutFlag) { return; }

	Palette.resize(++Clut.nPalette);

	Clut.nColor = GetDepth() == 4 ? 16 : 256;

	iClut = std::clamp(iClut, static_cast<std::size_t>(0), Palette.size() - 1);

	Palette.insert(Palette.begin() + iClut, std::vector<Sony_Texture_16bpp>(GetDepth() == 4 ? 16 : 256));

	if (GetDepth() == 4)
	{
		for (std::size_t i = 0; i < Palette[iClut].size(); i++)
		{
			Palette[iClut][i] = Create16bpp((uint8_t)i * 16, (uint8_t)i * 16, (uint8_t)i * 16, true);
		}
	}
	else //if (GetDepth() == 8)
	{
		for (std::size_t i = 0; i < Palette[iClut].size(); i++)
		{
			Palette[iClut][i] = Create16bpp((uint8_t)i, (uint8_t)i, (uint8_t)i, true);
		}
	}

	/*Palette[iClut][1] = Create16bpp(123, 165, 255, true);
	Palette[iClut][2] = Create16bpp(255, 123, 74, true);
	Palette[iClut][3] = Create16bpp(255, 165, 198, true);
	Palette[iClut][4] = Create16bpp(107, 255, 156, true);*/
}


/*
	Delete palette
*/
void Sony_PlayStation_Texture::DeletePalette(std::size_t iClut)
{
	if (!b_Open) { return; }

	if (!Header.ClutFlag) { return; }

	if (Palette.size() == 1)
	{
		Header.ClutFlag = false;
		std::memset(&Clut, 0, sizeof(Sony_Texture_Clut));
		for (std::size_t i = 0; i < Palette.size(); i++)
		{
			Palette[i].clear();
		}
		Palette.clear();
		return;
	}

	iClut = std::clamp(iClut, static_cast<std::size_t>(0), Palette.size() - 1);

	Palette.erase(Palette.begin() + iClut);

	Palette.resize(--Clut.nPalette);
}


/*
	Import pixels from unsigned char vector
*/
bool Sony_PlayStation_Texture::ImportPixels(std::vector<std::uint8_t> Source)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (Source.empty())
	{
		Str->Message("PlayStation Texture: Error, invalid source pixel size: %d", Source.size());
		return false;
	}

	// expected size...

	Pixels.resize(Source.size());

	std::memcpy(&Pixels.data()[0], Source.data(), Source.size());

	return true;
}


/*
	Import pixels from unsigned char vector
*/
bool Sony_PlayStation_Texture::ImportPixels(std::vector<std::uint8_t> Source, std::size_t Destination)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (Source.size() > Pixels.size())
	{
		Str->Message("PlayStation Texture: Error, invalid source pixel size: %d, expected no greater than %d", Source.size(), Pixels.size());
		return false;
	}

	if ((Destination + Pixels.size()) > Pixels.size())
	{
		Str->Message("PlayStation Texture: Error, invalid destination: %d, would overflow at %d", Destination, (Destination + Pixels.size()) - Pixels.size());
		return false;
	}

	std::memcpy(&Pixels.data()[Destination], Source.data(), Source.size());

	return true;
}


/*
	Import pixels from file
*/
std::uintmax_t Sony_PlayStation_Texture::ImportPixels(StdFile& File, std::uintmax_t _Ptr, std::uint32_t _PixelCount)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		Str->Message("PlayStation Texture: Error, could not read pixels at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
		return _Ptr;
	}

	Pixels.resize(_PixelCount);

	File.Read(_Ptr, Pixels.data(), _PixelCount);

	Data.Size = _PixelCount + sizeof(Sony_Texture_Data);

	return _Ptr + _PixelCount;
}


/*
	Create 16bpp color
*/
Sony_Texture_16bpp Sony_PlayStation_Texture::Create16bpp(std::uint8_t R, std::uint8_t G, std::uint8_t B, bool STP)
{
	Sony_Texture_16bpp Color{};
	Color.R = (R >> 3);
	Color.G = (G >> 3);
	Color.B = (B >> 3);
	Color.STP = STP;
	return Color;
}


/*
	Update Standard Image Palette
*/
void Sony_PlayStation_Texture::UpdateBitmapPalette(std::unique_ptr<Standard_Image>& Image, std::size_t iClut, DWORD Mask)
{
	Standard_String Str;

	if (!b_Open)
	{
		Str.Message("PlayStation Texture: Error, texture is not open");
		return;
	}

	if ((!Header.ClutFlag) || (Palette.empty()))
	{
		std::cout << "PlayStation Texture: Warning, texture does not have a CLUT" << std::endl;
		return;
	}

#if defined(_WIN64)
	iClut = std::clamp(iClut, 0ULL, Palette.size() - 1);
#else
	iClut = std::clamp(iClut, 0U, Palette.size() - 1);
#endif

	if (Header.ClutFlag)
	{
		DWORD ColorMask;

		Sony_Texture_16bpp Mask16;

		if ((b_TransparencySuperimposed) && (b_STP4Bpp || b_STP16Bpp))
		{
			Mask16 = Palette[iClut][0];
			ColorMask = TransparentColor = RGB(Red(Mask16), Green(Mask16), Blue(Mask16));
		}
		else
		{
			Mask16 = Create16bpp(Mask, false);
			ColorMask = RGB(Red(Mask16), Green(Mask16), Blue(Mask16));
		}

		for (std::size_t i = 0; i < Palette[iClut].size(); i++)
		{
			if ((b_TransparencySuperimposed) && (b_STP4Bpp || b_STP16Bpp))
			{
				if ((Palette[iClut][i].R == Mask16.R) && (Palette[iClut][i].G == Mask16.G) && (Palette[iClut][i].B == Mask16.B) && (Palette[iClut][i].STP == Mask16.STP))
				{
					Image->SetPalette(i, ColorMask);
				}
				else
				{
					Image->SetPalette(i, RGB(Red(Palette[iClut][i]), Green(Palette[iClut][i]), Blue(Palette[iClut][i])));
				}
			}
			else if ((!Palette[iClut][i].R) && (!Palette[iClut][i].G) && (!Palette[iClut][i].B) && (!Palette[iClut][i].STP) && (b_STP4Bpp || b_STP16Bpp))
			{
				Image->SetPalette(i, ColorMask);
			}
			else
			{
				Image->SetPalette(i, RGB(Red(Palette[iClut][i]), Green(Palette[iClut][i]), Blue(Palette[iClut][i])));
			}
		}
	}
}


/*
	Get Standard Image Object
*/
std::unique_ptr<Standard_Image> Sony_PlayStation_Texture::GetBitmap(std::size_t iClut, DWORD Mask)
{
	Standard_String Str;

	std::unique_ptr<Standard_Image> Image = std::make_unique<Standard_Image>(ImageFormat::BMP, GetWidth(), GetHeight(), GetDepth());

	if (!b_Open)
	{
		Str.Message("PlayStation Texture: Error, texture is not open");
		return Image;
	}

	if (Pixels.empty())
	{
		std::cout << "PlayStation Texture: Warning, pixel data is empty" << std::endl;
		return Image;
	}

	std::uint32_t _Depth = GetDepth();

	if ((_Depth == 4) || (_Depth == 8)) { UpdateBitmapPalette(Image, iClut, Mask); }

	std::uint32_t _Width = GetWidth();

	std::uint32_t _Height = GetHeight();

	std::uint32_t Offset = (GetWidth() * GetDepth()) / 8;

	Sony_Texture_4bpp Color4{};

	Sony_Texture_16bpp Color16{};

	Sony_Texture_16bpp Mask16 = Create16bpp(Mask, false);

	Sony_Texture_24bpp Color24{};

	for (std::uint32_t Y = 0; Y < _Height; Y++)
	{
		for (std::uint32_t X = 0; X < _Width; X++)
		{
			std::uint32_t i = (Y * Offset) + (X * (_Depth / 8));

			switch (_Depth)
			{
			case 4:
				i = (Y * (_Width / 2)) + (X / 2);
				Color4 = Get4bpp(i);
				Image->SetPixel(X, Y, (Color4.Pix2 << 12) | (Color4.Pix3 << 8) | (Color4.Pix0 << 4) | Color4.Pix1);
				break;
			case 8:
				Image->SetPixel(X, Y, Pixels[i]);
				break;
			case 16:
				Color16 = Get16bpp(X, Y);
				if ((!Color16.R) && (!Color16.G) && (!Color16.B) && (!Color16.STP) && b_STP16Bpp)
				{
					Color16 = Mask16;
				}
				Image->SetPixel(X, Y, (Color16.R << 10) | (Color16.G << 5) | Color16.B);
				break;
			case 24:
				Color24 = Get24bpp(X, Y);
				Image->SetPixel(X, Y, RGB(Color24.R0, Color24.G0, Color24.B0));
				Image->SetPixel(X + 1, Y, RGB(Color24.R1, Color24.G1, Color24.B1));
				break;
			}
		}
	}

	return Image;
}