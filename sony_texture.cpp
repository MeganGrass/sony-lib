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


#include "sony_texture.h"


/*
	Transparency Color (external)
*/
DWORD Sony_PlayStation_Texture::m_TransparentColor = RGB(0, 0, 0);


/*
	Does the texture have either palette or pixel data?
*/
bool Sony_PlayStation_Texture::IsValid(void) const
{
	if (Pixels.empty() && Palette.empty())
	{
		std::cout << "PlayStation Texture: Invalid, palette and pixel data is empty" << std::endl;
		return false;
	}

	return true;
}


/*
	Texture information
*/
String Sony_PlayStation_Texture::About(void)
{
	String Output = Str.FormatCStyle("PlayStation Texture:\n");
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
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
	Output += Str.FormatCStyle("\tPalette: 0x%X bytes\n", GetPaletteDataSize());

	return Output;
}


/*
	Print texture information
*/
String Sony_PlayStation_Texture::Print(void)
{
	String Output = About();

	std::cout << Output << std::endl;

	return Output;
}


/*
	Update Transparency flags
*/
bool Sony_PlayStation_Texture::UpdateTransparency(void)
{
	if (GetPalette().empty())
	{
		TransparencySuperimposed() = false;

		if (GetDepth() <= 8)
		{
			TransparencySTP() = false;
		}
	}

	if (GetPalette().empty() && GetPixels().empty())
	{
		TransparencySuperblack() = false;
		TransparencySuperimposed() = false;
		TransparencySTP() = false;
		Transparency() = false;
	}

	m_Transparency = Sony_Texture_Transparency::None;

	if (TransparencySuperblack())
	{
		m_Transparency |= Sony_Texture_Transparency::Superblack;
	}

	if (TransparencySuperimposed())
	{
		m_Transparency |= Sony_Texture_Transparency::Superimposed;
	}

	if (Transparency())
	{
		m_Transparency |= Sony_Texture_Transparency::External;
	}

	if (TransparencySTP())
	{
		m_Transparency = Sony_Texture_Transparency::STP;
		TransparencySuperblack() = false;
		TransparencySuperimposed() = false;
		Transparency() = false;
	}

	if (!TransparencyHalf() && !TransparencyFull() && !TransparencyInverse() && !TransparencyQuarter())
	{
		TransparencyFull() = true;
	}

	if (TransparencyHalf())
	{
		m_Transparency |= Sony_Texture_Transparency::Half;
	}
	else if (TransparencyFull())
	{
		m_Transparency |= Sony_Texture_Transparency::Full;
	}
	else if (TransparencyInverse())
	{
		m_Transparency |= Sony_Texture_Transparency::Inverse;
	}
	else if (TransparencyQuarter())
	{
		m_Transparency |= Sony_Texture_Transparency::Quarter;
	}

	return true;
}


/*
	Create
*/
bool Sony_PlayStation_Texture::Create(std::uint32_t _Depth, std::uint16_t _Width, std::uint16_t _Height, std::uint16_t nPalette)
{
	if (b_Open) { Close(); }

	if (_Depth != 4 && _Depth != 8 && _Depth != 16 && _Depth != 24)
	{
		Str.Message(L"PlayStation Texture: Error, invalid depth: %d", _Depth);
		return false;
	}

	if (_Width > 1024)
	{
		Str.Message(L"PlayStation Texture: Error, invalid width: %d", _Width);
		return false;
	}

	if (_Height > 512)
	{
		Str.Message(L"PlayStation Texture: Error, invalid height: %d", _Height);
		return false;
	}

	if (_Depth == 4 && (_Width && (_Width % 4)))
	{
		Str.Message(L"PlayStation Texture: Error, 4bpp width must be a multiple of 4");
		return false;
	}
	else if (_Depth == 8 && (_Width && (_Width % 2)))
	{
		Str.Message(L"PlayStation Texture: Error, 8bpp width must be a multiple of 2");
		return false;
	}

	Header.ID = 0x10;
	Header.Version = 0;
	Header.Reserved0 = 0;
	Header.Reserved1 = 0;

	switch (_Depth)
	{
	case 4:
		Header.Mode = 0;
		break;
	case 8:
		Header.Mode = 1;
		break;
	case 16:
		Header.Mode = 2;
		break;
	case 24:
		Header.Mode = 3;
		break;
	}

	if (nPalette)
	{
		Header.ClutFlag = true;

		Clut.X = 0;
		Clut.Y = 0;

		Clut.nColor = _Depth == 4 ? 16 : 256;
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
		Data.Size = (((_Width * _Height) * 3) + sizeof(Sony_Texture_Data));
		break;
	}

	Pixels.resize(Data.Size ^ sizeof(Sony_Texture_Data));

	UpdateTransparency();

	return b_Open = true;
}


/*
	Open
*/
std::uintmax_t Sony_PlayStation_Texture::Open(StdFile& File, std::uintmax_t _Ptr)
{
	Close();

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture: Error, could not open at 0x%llX in \"%ws\"", _Ptr, File.GetPath().filename().wstring().c_str());
			return _Ptr;
		}
	}

	std::uintmax_t OrigPtr = _Ptr;

	{
		File.Read(_Ptr, &Header, sizeof(Sony_Texture_Header));

		_Ptr += sizeof(Sony_Texture_Header);

		if (Header.ID != 0x10)
		{
			Str.Message(L"PlayStation Texture: Error, invalid header id (%d) at 0x%llX in \"%ws\"", Header.ID, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if (Header.Version != 0)
		{
			Str.Message(L"PlayStation Texture: Error, invalid header version (%d) at 0x%llX in \"%ws\"", Header.Version, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if (Header.Reserved0 != 0)
		{
			Str.Message(L"PlayStation Texture: Error, invalid header reserved[0] (%d) at 0x%llX in \"%ws\"", Header.Reserved0, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if ((Header.Mode != 0) && (Header.Mode != 1) && (Header.Mode != 2) && (Header.Mode != 3) && (Header.Mode != 4))
		{
			Str.Message(L"PlayStation Texture: Error, invalid header mode (%d) at 0x%llX in \"%ws\"", Header.Mode, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if ((Header.ClutFlag != 0) && (Header.ClutFlag != 1))
		{
			Str.Message(L"PlayStation Texture: Error, invalid header clut flag (%d) at 0x%llX in \"%ws\"", Header.ClutFlag, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if (Header.Reserved1 != 0)
		{
			Str.Message(L"PlayStation Texture: Error, invalid header reserved[1] (%d) at 0x%llX in \"%ws\"", Header.Reserved1, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if (Header.Mode == 4)
		{
			Str.Message(L"PlayStation Texture: Error, mixed-mode is unsupported");
			Close();
			return OrigPtr;
		}
	}

	if (Header.ClutFlag)
	{
		File.Read(_Ptr, &Clut, sizeof(Sony_Texture_Clut));

		_Ptr += sizeof(Sony_Texture_Clut);

		if (Clut.Size != ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut)))
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut size (%d bytes) at 0x%llX in \"%ws\"", Clut.Size, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if ((!Clut.Size) || (Clut.Size > (std::uint32_t)File.Size()))
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut size (%d bytes) at 0x%llX in \"%ws\"", Clut.Size, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if (!Clut.nColor)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut width (%d bytes) at 0x%llX in \"%ws\"", Clut.nColor, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if (!Clut.nPalette)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut height (%d bytes) at 0x%llX in \"%ws\"", Clut.nPalette, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if ((Clut.X < 0) || (Clut.X > (std::int16_t)(1024 - Clut.nColor)))
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut x (%d) at 0x%llX in \"%ws\"", Clut.X, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		if ((Clut.Y < 0) || (Clut.Y > (std::int16_t)(512 - Clut.nPalette)))
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut y (%d) at 0x%llX in \"%ws\"", Clut.Y, OrigPtr, File.GetPath().filename().wstring().c_str());
			Close();
			return OrigPtr;
		}

		std::uint16_t nPalette = 0;

		if (!Header.Mode)
		{
			nPalette = ((Clut.nColor * Clut.nPalette * 2) / (16 * sizeof(Sony_Texture_16bpp)));

			Clut.nColor = 16;
		}
		else
		{
			nPalette = ((Clut.nColor * Clut.nPalette * 2) / (256 * sizeof(Sony_Texture_16bpp)));

			Clut.nColor = 256;
		}

		Palette.resize(Clut.nPalette = nPalette);

		Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

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

	if (Header.Mode <= 1)
	{
		if (Header.Mode == 0)
		{
			if ((Data.Width * 4) % 4)
			{
				Str.Message(L"PlayStation Texture: Error, 4bpp width must be a multiple of 4 at 0x%llX in \"%ws\"", OrigPtr, File.GetPath().filename().wstring().c_str());
				Close();
				return OrigPtr;
			}
		}
		else
		{
			if ((Data.Width * 2) % 2)
			{
				Str.Message(L"PlayStation Texture: Error, 8bpp width must be a multiple of 2 at 0x%llX in \"%ws\"", OrigPtr, File.GetPath().filename().wstring().c_str());
				Close();
				return OrigPtr;
			}
		}
	}

	std::size_t DataSize = Data.Size;
	if ((DataSize & sizeof(Sony_Texture_Data)) == sizeof(Sony_Texture_Data)) { DataSize ^= sizeof(Sony_Texture_Data); }

	if (!Header.ClutFlag && !DataSize)
	{
		Str.Message(L"PlayStation Texture: Error, no pixel or palette data at 0x%llX in \"%ws\"", OrigPtr, File.GetPath().filename().wstring().c_str());
		Close();
		return OrigPtr;
	}

	_Ptr += sizeof(Sony_Texture_Data);

	if (DataSize)
	{
		Pixels.resize(DataSize);

		File.Read(_Ptr, Pixels.data(), Pixels.size());
	}

	b_Open = true;

	return _Ptr + DataSize;
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
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return _Ptr;
	}

	if (!IsValid())
	{
		Str.Message(L"PlayStation Texture: Error, palette and pixel data is empty");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture: Error, could not create at 0x%llX in \"%ws\"", _Ptr, File.GetPath().filename().wstring().c_str());
				return _Ptr;
			}
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

	Data.Size = ((Data.Width * Data.Height) * 2) + sizeof(Sony_Texture_Data);

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
std::vector<std::pair<std::uintmax_t, std::uintmax_t>> Sony_PlayStation_Texture::Search(StdFile& File, std::uintmax_t _Ptr, std::function<void(float)> ProgressCallback)
{
	std::vector<std::pair<std::uintmax_t, std::uintmax_t>> SearchResults;

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture: Error, could not open at 0x%llX in \"%ws\"", _Ptr, File.GetPath().filename().wstring().c_str());
			return SearchResults;
		}
	}

	m_SearchProgress = 0.0f;

	std::uintmax_t FileSize = File.Size();
	std::uintmax_t pTexture = 0;

	while (_Ptr < FileSize)
	{
		Sony_Texture_Header Header;
		File.Read(_Ptr, &Header, sizeof(Sony_Texture_Header));

		if (Header.ID != 0x10) { _Ptr++; continue; }
		if (Header.Version != 0) { _Ptr++; continue; }
		if (Header.Reserved0 != 0) { _Ptr++; continue; }
		if ((Header.Mode != 0) && (Header.Mode != 1) && (Header.Mode != 2) && (Header.Mode != 3) && (Header.Mode != 4)) { _Ptr++; continue; }
		if ((Header.ClutFlag != 0) && (Header.ClutFlag != 1)) { _Ptr++; continue; }
		if (Header.Reserved1 != 0) { _Ptr++; continue; }
		if (Header.Mode == 4) { _Ptr++; continue; }		// mixed-mode is unsupported

		{
			pTexture = _Ptr;

			_Ptr += sizeof(Sony_Texture_Header);

			if (Header.ClutFlag)
			{
				Sony_Texture_Clut Clut;
				File.Read(_Ptr, &Clut, sizeof(Sony_Texture_Clut));

				if (Clut.Size != ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut)))
				{
					std::cout << "Error, invalid clut size (" << Clut.Size << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if ((!Clut.Size) || (Clut.Size > (std::uint32_t)FileSize))
				{
					std::cout << "Error, invalid clut size (" << Clut.Size << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if (!Clut.nColor)
				{
					std::cout << "Error, invalid clut width (" << Clut.nColor << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if (!Clut.nPalette)
				{
					std::cout << "Error, invalid clut height (" << Clut.nPalette << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if ((Clut.X < 0) || (Clut.X > (std::int16_t)(1024 - Clut.nColor)))
				{
					std::cout << "Error, invalid vram clut x (" << Clut.X << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if ((Clut.Y < 0) || (Clut.Y > (std::int16_t)(512 - Clut.nPalette)))
				{
					std::cout << "Error, invalid vram clut y (" << Clut.Y << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				_Ptr += sizeof(Sony_Texture_Clut);

				_Ptr += (Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp)));
			}

			Sony_Texture_Data Data;
			File.Read(_Ptr, &Data, sizeof(Sony_Texture_Data));

			std::size_t DataSize = Data.Size;
			if ((DataSize & sizeof(Sony_Texture_Data)) == sizeof(Sony_Texture_Data)) { DataSize ^= sizeof(Sony_Texture_Data); }

			if (!Header.ClutFlag && !DataSize)
			{
				_Ptr = ++pTexture;
				continue;
			}

			if (DataSize)
			{
				if (Header.Mode == 0)
				{
					if ((Data.Width * 4) % 4)
					{
						std::cout << "PlayStation Texture: Error, 4bpp width must be a multiple of 4 at 0x" << std::hex << pTexture << std::endl;
						_Ptr = ++pTexture;
						continue;
					}
				}
				else if (Header.Mode == 1)
				{
					if ((Data.Width * 2) % 2)
					{
						std::cout << "PlayStation Texture: Error, 8bpp width must be a multiple of 2 at 0x" << std::hex << pTexture << std::endl;
						_Ptr = ++pTexture;
						continue;
					}
				}

				std::uint32_t PixelSize = ((Data.Width * Data.Height) * 2);

				if (PixelSize != DataSize)
				{
					std::cout << "Error, invalid pixel size (" << std::hex << PixelSize << " bytes, expected " << DataSize << " bytes) at 0x" << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if (Data.Size > (std::uint32_t)FileSize)
				{
					std::cout << "Error, invalid pixel size (" << std::hex << Data.Size << " bytes) at 0x" << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if (!Data.Width)
				{
					std::cout << "Error, invalid pixel width (" << Data.Width << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if (!Data.Height)
				{
					std::cout << "Error, invalid pixel height (" << Data.Height << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				/*std::uint32_t Width = 0;

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
				if (Width > 1024)
				{
					std::cout << "Error, invalid pixel width (" << Width << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if (Data.Height > 512)
				{
					std::cout << "Error, invalid pixel height (" << Data.Height << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}*/

				/*if ((Data.X > (std::int16_t)(1024 - Width)))
				{
					std::cout << "Error, invalid vram pixel x (" << Data.X << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}

				if ((Data.Y > (std::int16_t)(512 - Data.Height)))
				{
					std::cout << "Error, invalid vram pixel y (" << Data.Y << ") at 0x" << std::hex << pTexture << std::endl;
					_Ptr = ++pTexture;
					continue;
				}*/

				_Ptr += sizeof(Sony_Texture_Data);

				_Ptr += DataSize;
			}

			SearchResults.push_back(std::make_pair(pTexture, (_Ptr - pTexture)));
			//std::cout << "PlayStation Texture Image (*.TIM) found at 0x" << std::hex << pTexture << " with size 0x" << (_Ptr - pTexture) << std::endl;
		}

		float Progress = (float)(_Ptr * 100) / (float)FileSize;
		if (Progress != m_SearchProgress)
		{
			ProgressCallback(Progress / 100.0f);
			m_SearchProgress = Progress;
			//std::cout << "Search Progress: " << Progress << "%" << std::endl;
		}
	}

	return SearchResults;
}


/*
	Search
*/
std::vector<std::pair<std::uintmax_t, std::uintmax_t>> Sony_PlayStation_Texture::Search(std::filesystem::path Path, std::uintmax_t _Ptr, std::function<void(float)> ProgressCallback)
{
	StdFile m_File;

	m_File.SetPath(Path);

	return Search(m_File, _Ptr, ProgressCallback);
}


/*
	Open CLUT from file
*/
std::uintmax_t Sony_PlayStation_Texture::OpenCLT(StdFile& File, std::uintmax_t _Ptr, bool b_Add)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture: Error, could not open at 0x%llX in \"%ws\"", _Ptr, File.GetPath().filename().wstring().c_str());
			return _Ptr;
		}
	}

	std::uintmax_t OrigPtr = _Ptr;

	Sony_Texture_Clut_File ClutHeader{};
	File.Read(_Ptr, &ClutHeader, sizeof(Sony_Texture_Clut_File));

	{
		if (ClutHeader.ID != 0x11)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut header id (%d) at 0x%llX in \"%ws\"", ClutHeader.ID, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if (ClutHeader.Version != 0)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut header version (%d) at 0x%llX in \"%ws\"", ClutHeader.Version, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if (ClutHeader.Reserved0 != 0)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut header reserved[0] (%d) at 0x%llX in \"%ws\"", ClutHeader.Reserved0, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if (ClutHeader.Mode != 2)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut header mode (%d) at 0x%llX in \"%ws\"", ClutHeader.Mode, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if (ClutHeader.Reserved1 != 0)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut header reserved[1] (%d) at 0x%llX in \"%ws\"", ClutHeader.Reserved1, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		_Ptr += sizeof(Sony_Texture_Clut_File);
	}

	Sony_Texture_Clut ClutData{};
	File.Read(_Ptr, &ClutData, sizeof(Sony_Texture_Clut));

	{
		if (ClutData.Size != ((ClutData.nPalette * (ClutData.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut)))
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut size (%d bytes) at 0x%llX in \"%ws\"", ClutData.Size, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if ((!ClutData.Size) || (ClutData.Size > (std::uint32_t)File.Size()))
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut size (%d bytes) at 0x%llX in \"%ws\"", ClutData.Size, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if (!ClutData.nColor)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut width (%d bytes) at 0x%llX in \"%ws\"", ClutData.nColor, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if (!ClutData.nPalette)
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut height (%d bytes) at 0x%llX in \"%ws\"", ClutData.nPalette, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if ((ClutData.X < 0) || (ClutData.X > (std::int16_t)(1024 - ClutData.nColor)))
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut x (%d) at 0x%llX in \"%ws\"", ClutData.X, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		if ((ClutData.Y < 0) || (ClutData.Y > (std::int16_t)(512 - ClutData.nPalette)))
		{
			Str.Message(L"PlayStation Texture: Error, invalid clut y (%d) at 0x%llX in \"%ws\"", ClutData.Y, OrigPtr, File.GetPath().filename().wstring().c_str());
			return OrigPtr;
		}

		_Ptr += sizeof(Sony_Texture_Clut);
	}

	std::vector<Sony_Texture_16bpp> ExternalPalette(ClutData.nPalette * ClutData.nColor);
	File.Read(_Ptr, ExternalPalette.data(), ExternalPalette.size());

	if (!b_Add)
	{
		AddPalette(ConvertToPalette(ExternalPalette));
	}
	else
	{
		ImportPalette(ConvertToPalette(ExternalPalette));
	}

	return _Ptr + ExternalPalette.size() * sizeof(Sony_Texture_16bpp);
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
	for (std::size_t i = 0; i < Palette.size(); i++)
	{
		Palette[i].clear();
	}
	Palette.clear();
	Pixels.clear();
	b_TransparencySuperblack = false;
	b_TransparencySuperimposed = false;
	b_TransparencySTP = false;
	b_TransparencyHalf = false;
	b_TransparencyFull = true;
	b_TransparencyInverse = false;
	b_TransparencyQuarter = false;
	b_Transparency = false;
	m_TransparentColor = RGB(0, 0, 0);
	m_Transparency = Sony_Texture_Transparency::None;
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
std::uint16_t Sony_PlayStation_Texture::GetDepth(void) const
{
	switch (Header.Mode)
	{
	case 0: return 4;
	case 1: return 8;
	case 2: return 16;
	case 3: return 24;
	}

	return 0;
}


/*
	Get width
*/
std::uint16_t Sony_PlayStation_Texture::GetWidth(void) const
{
	switch (Header.Mode)
	{
	case 0: return Data.Width * 4;
	case 1: return Data.Width * 2;
	case 2: return Data.Width;
	case 3: return ((Data.Width * 2) / 3);
	}

	return 0;
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
		if ((Data.Width * 4) % 4)
		{
			Data.Width += 4 - (Data.Width % 4);
		}
		break;
	case 1:
		Data.Width = _Width / 2;
		if ((Data.Width * 2) % 2)
		{
			Data.Width += 2 - (Data.Width % 2);
		}
		break;
	case 2:
		Data.Width = _Width;
		break;
	case 3:
		Data.Width = (_Width * 3) / 2;
		break;
	}

	UpdateDataSize();

	return false;
}


/*
	Get height
*/
std::uint16_t Sony_PlayStation_Texture::GetHeight(void) const
{
	return Data.Height;
}


/*
	Set height
*/
bool Sony_PlayStation_Texture::SetHeight(std::uint16_t _Height)
{
	if (!b_Open) { return false; }

	Data.Height = _Height;

	UpdateDataSize();

	return true;
}


/*
	Auto-update "Size" field of Data Header
*/
void Sony_PlayStation_Texture::UpdateDataSize(void)
{
	if (!b_Open) { return; }

	Data.Size = ((Data.Width * Data.Height) * 2) + sizeof(Sony_Texture_Data);

	std::size_t DataSize = Data.Size;
	if ((DataSize & sizeof(Sony_Texture_Data)) == sizeof(Sony_Texture_Data)) { DataSize ^= sizeof(Sony_Texture_Data); }

	if (Pixels.size() != DataSize)
	{
		Pixels.resize(DataSize);
	}
}


/*
	Get raw palette data size
*/
std::uint16_t Sony_PlayStation_Texture::GetPaletteDataSize(void) const
{
	if (!b_Open) { return 0; }

	if (!Header.ClutFlag) { return 0; }

	if (Palette.empty()) { return 0; }

	std::uint16_t BlockSize = 0;

	if (!Header.Mode)
	{
		BlockSize = 16 * 2;
	}
	else
	{
		BlockSize = 256 * 2;
	}

	return Clut.nPalette * BlockSize;
}


/*
	Get raw palette width
*/
std::uint16_t Sony_PlayStation_Texture::GetPaletteWidth(void) const
{
	if (!b_Open) { return 0; }

	if (!Header.ClutFlag) { return 0; }

	if (Palette.empty()) { return 0; }

	return GetDepth() == 4 ? 16 : 256;
}


/*
	Get raw palette height
*/
std::uint16_t Sony_PlayStation_Texture::GetPaletteHeight(void) const
{
	if (!b_Open) { return 0; }

	if (!Header.ClutFlag) { return 0; }

	if (Palette.empty()) { return 0; }

	return Clut.nPalette;
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

	if (Source.size() > (size_t)PaletteSize * MAX_TIM_PALETTE)
	{
		std::cout << "Attempting to read too many palettes (" << Source.size() / PaletteSize << ")" << ", limiting to " << MAX_TIM_PALETTE << std::endl;
		PaletteSize = MAX_TIM_PALETTE;
	}

	if (Source.size() % PaletteSize)
	{
		Source.resize(Source.size() + (PaletteSize - (Source.size() % PaletteSize)));
	}

	std::size_t nPalette = Source.size() / PaletteSize;

	nPalette = std::clamp(nPalette, (size_t)0, (size_t)MAX_TIM_PALETTE);

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
	Convert 16bpp vector to palette
*/
std::vector<std::vector<Sony_Texture_16bpp>> Sony_PlayStation_Texture::ConvertToPalette(std::vector<Sony_Texture_16bpp> Source) const
{
	std::uint32_t nColors = GetDepth() == 4 ? 16 : 256;

	std::uint32_t PaletteSize = (nColors * sizeof(Sony_Texture_16bpp));

	if (Source.empty()) { return std::vector<std::vector<Sony_Texture_16bpp>> {}; }

	if (Source.size() % PaletteSize)
	{
		Source.resize(Source.size() + (PaletteSize - (Source.size() % PaletteSize)));
	}

	std::size_t nPalette = Source.size() / PaletteSize;

	nPalette = std::clamp(nPalette, (size_t)0, (size_t)MAX_TIM_PALETTE);

	std::vector<std::vector<Sony_Texture_16bpp>> NewPalette(nPalette);

	for (std::size_t i = 0; i < nPalette; ++i)
	{
		NewPalette[i].resize(nColors);

		for (std::size_t x = 0; x < nColors; ++x)
		{
			std::size_t pSource = (i * nColors + x);

			if (pSource < Source.size())
			{
				NewPalette[i][x] = Source[pSource];
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
		Str.Message(L"PlayStation Texture: Error, could not open \"%ws\"", _Filename.filename().wstring().c_str());
		return;
	}

	std::uintmax_t _Ptr = 0;

	for (std::size_t i = 0; i < Palette.size(); i++)
	{
		for (std::size_t x = 0; x < Palette[i].size(); x++, _Ptr += sizeof(Sony_Texture_16bpp))
		{
			m_Input.Write(_Ptr, &Palette[i][x], sizeof(Sony_Texture_16bpp));
		}
	}
}


/*
	Export single palette to raw data
*/
void Sony_PlayStation_Texture::ExportPalette(std::filesystem::path _Filename, std::size_t iClut)
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

	iClut = std::clamp(size_t(iClut), size_t(0), size_t(GetClutMax()));

	StdFile m_Input{ _Filename, FileAccessMode::Write_Ex, true, true };
	if (!m_Input.IsOpen())
	{
		Str.Message(L"PlayStation Texture: Error, could not open \"%ws\"", _Filename.filename().wstring().c_str());
		return;
	}

	std::uintmax_t _Ptr = 0;

	for (std::uint16_t x = 0; x < Clut.nColor; x++, _Ptr += sizeof(Sony_Texture_16bpp))
	{
		m_Input.Write(_Ptr, &Palette[iClut][x], sizeof(Sony_Texture_16bpp));
	}
}


/*
	Export all palettes to Sony Texture Image file
*/
bool Sony_PlayStation_Texture::ExportPaletteToTIM(std::filesystem::path _Filename)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>();

	ExternalTexture->Str.hWnd = Str.hWnd;

	ExternalTexture->Create(GetDepth(), NULL, NULL, uint16_t(GetClutCount()));

	if (!ExternalTexture->IsOpen())
	{
		return false;
	}

	ExternalTexture->ClutX() = ClutX();
	ExternalTexture->ClutY() = ClutY();

	ExternalTexture->ImportPalette(GetPalette());

	StdFile m_Input{ _Filename, FileAccessMode::Write_Ex, true, true };
	if (!m_Input.IsOpen())
	{
		Str.Message(L"PlayStation Texture: Error, could not open file \"%ws\" for writing palette data", _Filename.filename().wstring().c_str());
		return false;
	}

	return ExternalTexture->Save(m_Input);
}


/*
	Export palette from file
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
		Str.Message(L"PlayStation Texture: Error, could not create \"%ws\"", Filename.filename().wstring().c_str());
		return false;
	}

	iClut = std::clamp(iClut, (size_t)0, (size_t)GetClutMax());

	std::uint16_t DataSize = GetDepth() == 4 ? 16 * 4 + 4 : 256 * 4 + 4;
	std::uint16_t Version = 0x0300;
	std::uint16_t nColors = GetDepth() == 4 ? 16 : 256;
	std::uintmax_t pPalette = 0x16;
	std::uint32_t FileSize = 0x0A + DataSize;
	RGBQUAD Color{};

	m_Input.WriteStr(0x00, "RIFF\0");
	m_Input.Write(0x04, &FileSize, 4);
	m_Input.WriteStr(0x08, "PAL \0");
	m_Input.WriteStr(0x0C, "data\0");
	m_Input.Write(0x10, &DataSize, 2);
	m_Input.Write(0x12, &Version, 2);
	m_Input.Write(0x14, &nColors, 2);

	for (std::size_t i = 0; i < nColors; i++, pPalette += sizeof(RGBQUAD))
	{
		Color.rgbRed = Red(Palette[iClut][i]);
		Color.rgbGreen = Green(Palette[iClut][i]);
		Color.rgbBlue = Blue(Palette[iClut][i]);
		Color.rgbReserved = STP(Palette[iClut][i]);

		m_Input.Write(pPalette, &Color, sizeof(RGBQUAD));
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

	Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

	iClut = std::clamp(iClut, (size_t)0, (size_t)GetClutMax());

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

	Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

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
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	return ImportPalette(ConvertToPalette(Source));
}


/*
	Import palette from file
*/
bool Sony_PlayStation_Texture::ImportPalette(StdFile& File, std::uintmax_t _Ptr, std::uint16_t nClut)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (!nClut)
	{
		Str.Message(L"PlayStation Texture: Error, palette count is empty (0)");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture: Error, could not open \"%ws\"", File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	if (!File.Size())
	{
		Str.Message(L"PlayStation Texture: Error, \"%ws\" file size is invalid (0 bytes)", File.GetPath().filename().wstring().c_str());
		return false;
	}

	std::vector<std::uint8_t> Source((GetClutColorMax() * sizeof(Sony_Texture_16bpp)) * nClut);

	File.Read(_Ptr, Source.data(), Source.size());

	return ImportPalette(ConvertToPalette(Source));
}


/*
	Import palette from file
*/
bool Sony_PlayStation_Texture::ImportPalette(std::filesystem::path Filename, std::uintmax_t _Ptr, std::uint16_t nClut)
{
	StdFile m_File;

	m_File.SetPath(Filename);

	return ImportPalette(m_File, _Ptr, nClut);
}


/*
	Import palette from file
*/
bool Sony_PlayStation_Texture::ImportPalette(std::filesystem::path Filename, std::uintmax_t _Ptr, std::size_t iClut)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	StdFile m_Input{ Filename, FileAccessMode::Read, true, false };
	if (!m_Input.IsOpen())
	{
		Str.Message(L"PlayStation Texture: Error, could not open file \"%ws\"", Filename.filename().wstring().c_str());
		return false;
	}

	uintmax_t Filesize = m_Input.Size() - _Ptr;
	Filesize = std::clamp(Filesize, uintmax_t(0), GetDepth() == 4 ? uintmax_t(MAX_TIM_PALETTE * 32) : uintmax_t(MAX_TIM_PALETTE * 512));

	std::vector<std::uint8_t> Source((size_t)Filesize);

	m_Input.Read(_Ptr, Source.data(), Source.size());

	iClut = std::clamp(size_t(iClut), size_t(0), size_t(GetClutMax()));

	std::vector<std::vector<Sony_Texture_16bpp>> _Palette = ConvertToPalette(Source);

	if (!_Palette.empty())
	{
		if (GetPalette().empty())
		{
			AddPalette();
		}

		ImportPalette(_Palette[0], iClut);
	}

	return true;
}


/*
	Import palette from Sony Texture Image file
*/
bool Sony_PlayStation_Texture::ImportPaletteFromTIM(std::filesystem::path Filename, std::uintmax_t _Ptr)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>();

	ExternalTexture->Str.hWnd = Str.hWnd;

	ExternalTexture->Open(Filename, _Ptr);

	return ImportPaletteFromTIM(ExternalTexture);
}


/*
	Import palette from Sony Texture Image file
*/
bool Sony_PlayStation_Texture::ImportPaletteFromTIM(std::unique_ptr<Sony_PlayStation_Texture>& External)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	External->Str.hWnd = Str.hWnd;

	if (!External->IsOpen())
	{
		Str.Message(L"PlayStation Texture: Error, external texture is not open");
		return false;
	}

	if (External->GetPalette().empty())
	{
		Str.Message(L"PlayStation Texture: Error, palette data was not found in external texture");
		return false;
	}

	if ((GetDepth() != External->GetDepth()))
	{
		ImportPalette(External->GetPalette(External->GetDepth()));
	}
	else
	{
		ImportPalette(External->GetPalette());
	}

	ClutX() = External->ClutX();
	ClutY() = External->ClutY();

	return true;
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

	StdFile m_Input{ Filename, FileAccessMode::Read_Ex, true, false };
	if (!m_Input.IsOpen())
	{
		Str.Message(L"Error, could not open file \"%ws\" for reading palette data", Filename.filename().wstring().c_str());
		return false;
	}

	if (iClut >= Palette.size())
	{
		Palette.resize(++Clut.nPalette);
		Palette[GetClutMax()].resize(GetDepth() == 4 ? 16 : 256);

		Clut.nColor = GetDepth() == 4 ? 16 : 256;
		Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

		iClut = GetClutMax();
	}

	iClut = std::clamp(iClut, (size_t)0, (size_t)GetClutMax());

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
	Move palette
*/
bool Sony_PlayStation_Texture::MovePalette(std::size_t iClut, bool Right)
{
	if (!b_Open) { return false; }

	if (GetPalette().empty()) { return false; }

	if (!GetClutMax()) { return false; }

	iClut = std::clamp(uint16_t(iClut), uint16_t(0), GetClutMax());

	if (Right)
	{
		if (iClut < GetClutMax())
		{
			std::swap(GetPalette()[iClut], GetPalette()[iClut + 1]);
			return true;
		}
	}
	else
	{
		if (iClut > 0)
		{
			std::swap(GetPalette()[iClut], GetPalette()[iClut - 1]);
			return true;
		}
	}

	return true;
}


/*
	Add palette
*/
bool Sony_PlayStation_Texture::AddPalette(void)
{
	if (!b_Open) { return false; }

	Header.ClutFlag = true;

	if (Palette.empty())
	{
		Clut.nPalette = 0;
	}

	Palette.resize(++Clut.nPalette);

	Palette[GetClutMax()] = std::vector<Sony_Texture_16bpp>(GetDepth() == 4 ? 16 : 256);

	Clut.nColor = GetDepth() == 4 ? 16 : 256;

	Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

	if (GetDepth() == 4)
	{
		for (std::size_t i = 0; i < Palette[GetClutMax()].size(); i++)
		{
			Palette[GetClutMax()][i] = Create16bpp((uint8_t)i * 16, (uint8_t)i * 16, (uint8_t)i * 16, true);
		}
	}
	else //if (GetDepth() == 8)
	{
		for (std::size_t i = 0; i < Palette[GetClutMax()].size(); i++)
		{
			Palette[GetClutMax()][i] = Create16bpp((uint8_t)i, (uint8_t)i, (uint8_t)i, true);
		}
	}

	return true;

	/*Palette[GetClutMax()][1] = Create16bpp(123, 165, 255, true);
	Palette[GetClutMax()][2] = Create16bpp(255, 123, 74, true);
	Palette[GetClutMax()][3] = Create16bpp(255, 165, 198, true);
	Palette[GetClutMax()][4] = Create16bpp(107, 255, 156, true);*/
}


/*
	Add palette
*/
void Sony_PlayStation_Texture::AddPalette(std::vector<std::vector<Sony_Texture_16bpp>> Source)
{
	if (Source.empty())
	{
		Str.Message(L"PlayStation Texture: Error, source does not contain any color palettes");
		return;
	}

	Header.ClutFlag = true;

	if (Palette.empty())
	{
		Clut.nPalette = 0;
	}

	std::uint16_t OldCount = Clut.nPalette;

	if (Palette.empty())
	{
		Clut.nPalette = static_cast<std::uint16_t>(Source.size());
	}
	else
	{
		Clut.nPalette += static_cast<std::uint16_t>(Source.size());
	}

	Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

	Palette.resize(Clut.nPalette);

	for (std::size_t i = 0; i < Source.size(); i++)
	{
		if (i >= Palette.size()) { break; }
		Palette[OldCount + i] = Source[i];
	}
}


/*
	Add palette from file
*/
bool Sony_PlayStation_Texture::AddPalette(bool b_RawData, std::filesystem::path _Filename)
{
	if (!b_Open) { return false; }

	if (b_RawData)
	{
		StdFile m_Input{ _Filename, FileAccessMode::Read, true, false };
		if (!m_Input.IsOpen())
		{
			Str.Message(L"PlayStation Texture: Error, could not open \"%ws\"", _Filename.filename().wstring().c_str());
			return false;
		}

		std::vector<std::uint8_t> Source((size_t)m_Input.Size());

		m_Input.Read(0, Source.data(), Source.size());

		std::vector<std::vector<Sony_Texture_16bpp>> ExternalPalette = ConvertToPalette(Source);

		AddPalette(ExternalPalette);
	}
	else
	{
		std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>(_Filename);
		if (!ExternalTexture->IsOpen())
		{
			return false;
		}

		if (!ExternalTexture->Header.ClutFlag)
		{
			Str.Message(L"PlayStation Texture: Error, \"%ws\" does not contain any color palettes", _Filename.filename().wstring().c_str());
			return false;
		}

		AddPalette(ExternalTexture->GetPalette(GetDepth()));
	}

	return true;
}


/*
	Add palette from file
*/
bool Sony_PlayStation_Texture::AddPalette(std::filesystem::path _Filename)
{
	if (!b_Open) { return false; }

	std::uint32_t iLastPalette = GetClutCount();

	StringW Extension = _Filename.extension().wstring();

	if (Str.ToUpper(Extension) == L".TIM")
	{
		return AddPalette(false, _Filename);
	}
	else if (Str.ToUpper(Extension) == L".PAL")
	{
		return ImportMicrosoftPalette(_Filename, GetPalette().size());
	}
	else
	{
		return AddPalette(true, _Filename);
	}

	return false;
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

	++Clut.nPalette;

	Clut.nColor = GetDepth() == 4 ? 16 : 256;

	Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

	Palette.insert(Palette.begin() + iClut, std::vector<Sony_Texture_16bpp>(GetDepth() == 4 ? 16 : 256));

	iClut = std::clamp(iClut, size_t(0), size_t(GetClutMax()));

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
bool Sony_PlayStation_Texture::DeletePalette(std::size_t iClut)
{
	if (!b_Open) { return false; }

	if (!Header.ClutFlag) { return false; }

	if (Palette.empty()) { return false; }

	if (Palette.size() == 1)
	{
		Header.ClutFlag = false;

		std::memset(&Clut, 0, sizeof(Sony_Texture_Clut));

		for (std::size_t i = 0; i < Palette.size(); i++)
		{
			Palette[i].clear();
		}

		Palette.clear();

		Clut.Size = sizeof(Sony_Texture_Clut);

		return true;
	}

	iClut = std::clamp(iClut, size_t(0), size_t(GetClutMax()));

	Palette.erase(Palette.begin() + iClut);

	Palette.resize(--Clut.nPalette);

	Clut.Size = ((Clut.nPalette * (Clut.nColor * sizeof(Sony_Texture_16bpp))) + sizeof(Sony_Texture_Clut));

	return true;
}


/*
	Delete all palettes
*/
void Sony_PlayStation_Texture::DeleteAllPalettes(void)
{
	if (!b_Open) { return; }

	Header.ClutFlag = false;

	std::memset(&Clut, 0, sizeof(Sony_Texture_Clut));

	for (std::size_t i = 0; i < Palette.size(); i++)
	{
		Palette[i].clear();
	}

	Palette.clear();
}


/*
	Delete all pixels
*/
void Sony_PlayStation_Texture::DeleteAllPixels(void)
{
	if (!b_Open) { return; }

	std::memset(&Data, 0, sizeof(Sony_Texture_Clut));

	Data.Size = sizeof(Sony_Texture_Data);

	Pixels.clear();
}


/*
	Export pixels to raw data
*/
bool Sony_PlayStation_Texture::ExportPixels(std::filesystem::path Filename)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	StdFile m_Input{ Filename, FileAccessMode::Write_Ex, true, true };
	if (!m_Input.IsOpen())
	{
		Str.Message(L"Error, could not open file \"%ws\" for writing pixel data", Filename.filename().wstring().c_str());
		return false;
	}

	m_Input.Write(0, GetPixels().data(), GetPixels().size());

	return true;
}


/*
	Export pixels to Sony Texture Image file
*/
bool Sony_PlayStation_Texture::ExportPixelsToTIM(std::filesystem::path Filename)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	bool bClutFlag = GetCF();

	SetCF(false);

	Save(Filename);

	SetCF(bClutFlag);

	return true;
}


/*
	Import pixels from unsigned char vector
*/
bool Sony_PlayStation_Texture::ImportPixels(std::vector<std::uint8_t> Source)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (Source.empty())
	{
		Str.Message(L"PlayStation Texture: Error, invalid source pixel size: %d", Source.size());
		return false;
	}

	// expected size...

	Pixels.resize(Source.size());

	std::memcpy(&Pixels.data()[0], Source.data(), Source.size());

	UpdateDataSize();

	return true;
}


/*
	Import pixels from unsigned char vector
*/
bool Sony_PlayStation_Texture::ImportPixels(std::vector<std::uint8_t> Source, std::size_t Destination)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (Source.empty())
	{
		Str.Message(L"PlayStation Texture: Error, invalid source pixel size: %d", Source.size());
		return false;
	}

	// expected size...

#ifdef max
#undef max
#endif

	Pixels.resize(std::max(Pixels.size(), Destination + Source.size()));

	std::memcpy(&Pixels.data()[Destination], Source.data(), Source.size());

	UpdateDataSize();

	return true;
}


/*
	Import pixels from file
*/
bool Sony_PlayStation_Texture::ImportPixels(StdFile& File, std::uintmax_t _Ptr, std::uint32_t _PixelCount)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (!GetWidth() && !GetHeight())
	{
		Str.Message(L"PlayStation Texture: Error, texture resolution is invalid (0 x 0), can't yet resize...");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture: Error, could not read pixels at 0x%llX in \"%ws\"", _Ptr, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	if (_Ptr > File.Size())
	{
		Str.Message(L"PlayStation Texture: Error, pixel data pointer is out of bounds");
		return false;
	}

	/*size_t Filesize = File.Size() - _Ptr;
	Filesize = std::clamp(Filesize, size_t(0), File.Size());

	if (GetPixels().size() > Filesize)
	{
		Str.Message(L"PlayStation Texture: Error, required pixel size (%llx bytes at 0x%llx) exceeds \"%ws\" file size (%llx bytes)", GetPixels().size(), _Ptr, File.GetPath().filename().wstring().c_str(), File.Size());
		return false;
	}*/

	Pixels.resize(_PixelCount);

	File.Read(_Ptr, Pixels.data(), _PixelCount);

	UpdateDataSize();

	return true;
}


/*
	Import pixels from file
*/
bool Sony_PlayStation_Texture::ImportPixels(std::filesystem::path Filename, std::uintmax_t _Ptr)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	StdFile m_Input{ Filename, FileAccessMode::Read, true, false };
	if (!m_Input.IsOpen())
	{
		Str.Message(L"PlayStation Texture: Error, could not read pixels at 0x%llX in \"%ws\"", _Ptr, Filename.filename().wstring().c_str());
		return false;
	}

	if (!m_Input.Size())
	{
		Str.Message(L"PlayStation Texture: Error, \"%ws\" file size is invalid (0 bytes)", Filename.filename().wstring().c_str());
		return false;
	}

	return ImportPixels(m_Input, _Ptr, uint32_t(m_Input.Size()));
}


/*
	Import pixels from Sony Texture Image file
*/
bool Sony_PlayStation_Texture::ImportPixelsFromTIM(std::filesystem::path Filename, std::uintmax_t _Ptr, bool b_UpdateDepth)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>();

	ExternalTexture->Str.hWnd = Str.hWnd;

	ExternalTexture->Open(Filename, _Ptr);

	return ImportPixelsFromTIM(ExternalTexture, b_UpdateDepth);
}


/*
	Import pixels from Sony Texture Image file
*/
bool Sony_PlayStation_Texture::ImportPixelsFromTIM(std::unique_ptr<Sony_PlayStation_Texture>& External, bool b_UpdateDepth)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return false;
	}

	External->Str.hWnd = Str.hWnd;

	if (!External->IsOpen())
	{
		Str.Message(L"PlayStation Texture: Error, external texture is not open");
		return false;
	}

	if (External->GetPixels().empty())
	{
		Str.Message(L"Error, pixel data was not found in external texture");
		return false;
	}

	if (!b_UpdateDepth)
	{
		if ((GetDepth() != External->GetDepth()))
		{
			Str.Message(L"Error, texture depth (%d) does not match external texture depth (%d)", GetDepth(), External->GetDepth());
			return false;
		}
	}

	GetPixels().clear();

	b_UpdateDepth ? Header.Mode = External->Header.Mode : Header.Mode = Header.Mode;

	SetWidth(External->GetWidth());
	SetHeight(External->GetHeight());

	GetPixels() = External->GetPixels();

	DataX() = External->DataX();
	DataY() = External->DataY();

	UpdateDataSize();

	return true;
}


/*
	Update Standard Image Palette
*/
void Sony_PlayStation_Texture::UpdateBitmapPalette(std::unique_ptr<Standard_Image>& Image, std::size_t iClut, DWORD Mask)
{
	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return;
	}

	if ((!Header.ClutFlag) || (Palette.empty()))
	{
		std::cout << "PlayStation Texture: Warning, texture does not have a CLUT" << std::endl;
		return;
	}

	iClut = std::clamp(iClut, size_t(0), size_t(GetClutMax()));

	auto GetTransparency = [this]()
		{
			if (b_TransparencyHalf || b_TransparencyFull || b_TransparencyInverse || b_TransparencyQuarter)
			{
				if (b_TransparencyHalf) { return 0x80; }
				else if (b_TransparencyFull) { return 0x00; }
				else if (b_TransparencyInverse) { return 0xFF; }
				else if (b_TransparencyQuarter) { return 0x40; }
			}
			return 0x00;
		};

	if (Header.ClutFlag)
	{
		Sony_Texture_16bpp Mask16 = Create16bpp(Mask, false);

		for (std::size_t i = 0; i < Palette[iClut].size(); i++)
		{
			if (b_TransparencySuperblack && !Palette[iClut][i].R && !Palette[iClut][i].G && !Palette[iClut][i].B && !Palette[iClut][i].STP)
			{
				Image->SetPalette(i, 0);
				continue;
			}

			if (b_TransparencySuperimposed && ((Palette[iClut][i].R == Palette[iClut][0].R) && (Palette[iClut][i].G == Palette[iClut][0].G) && (Palette[iClut][i].B == Palette[iClut][0].B)))
			{
				Image->SetPalette(i, (GetTransparency() << 24) | (Red(Palette[iClut][0]) << 16) | (Green(Palette[iClut][0]) << 8) | Blue(Palette[iClut][0]));
				continue;
			}
			
			if (b_Transparency && (Palette[iClut][i].R == Mask16.R) && (Palette[iClut][i].G == Mask16.G) && (Palette[iClut][i].B == Mask16.B))
			{
				Image->SetPalette(i, (GetTransparency() << 24) | (Red(Mask16) << 16) | (Green(Mask16) << 8) | Blue(Mask16));
				continue;
			}

			if (b_TransparencySTP && !STP(Palette[iClut][i]))
			{
				Image->SetPalette(i, (GetTransparency() << 24) | (Red(Palette[iClut][i]) << 16) | (Green(Palette[iClut][i]) << 8) | Blue(Palette[iClut][i]));
				continue;
			}

			Image->SetPalette(i, (0xFF << 24) | (Red(Palette[iClut][i]) << 16) | (Green(Palette[iClut][i]) << 8) | Blue(Palette[iClut][i]));
		}
	}
}


/*
	Get Standard Image Object
*/
std::unique_ptr<Standard_Image> Sony_PlayStation_Texture::GetBitmap(std::size_t iClut, DWORD Mask)
{
	std::unique_ptr<Standard_Image> Image = std::make_unique<Standard_Image>(ImageFormat::BMP, GetWidth(), GetHeight(), GetDepth());

	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture: Error, texture is not open");
		return Image;
	}

	if (Pixels.empty())
	{
		std::cout << "PlayStation Texture: Warning, pixel data is empty" << std::endl;
		return Image;
	}

	std::uint32_t _Depth = GetDepth();

	if (Header.ClutFlag && !Palette.empty()) { UpdateBitmapPalette(Image, iClut, Mask); }

	std::uint32_t _Width = GetWidth();

	std::uint32_t _Height = GetHeight();

	Sony_Texture_4bpp Color4{};

	Sony_Texture_8bpp Color8{};

	Sony_Texture_16bpp Color16{};

	Sony_Texture_24bpp Color24{};

	auto GetTransparency = [this]()
		{
			if (b_TransparencyHalf || b_TransparencyFull || b_TransparencyInverse || b_TransparencyQuarter)
			{
				if (b_TransparencyHalf) { return 0x80; }
				else if (b_TransparencyFull) { return 0x00; }
				else if (b_TransparencyInverse) { return 0xFF; }
				else if (b_TransparencyQuarter) { return 0x40; }
			}
			return 0x00;
		};

	auto GetAlpha24 = [this, GetTransparency, iClut, Mask](Sony_Texture_24bpp Color, bool iColor)
		{
			uint8_t R = iColor ? Color.R1 : Color.R0;
			uint8_t G = iColor ? Color.G1 : Color.G0;
			uint8_t B = iColor ? Color.B1 : Color.B0;

			if (b_TransparencySuperblack && !R && !G && !B)
			{
				return 0x00;
			}

			if (b_TransparencySuperimposed && !GetPalette().empty() &&
				(R == Red(GetPalette()[iClut][0])) &&
				(G == Green(GetPalette()[iClut][0])) &&
				(B == Green(GetPalette()[iClut][0])))
			{
				return GetTransparency();
			}

			if (b_Transparency &&
				(R == GetRValue(Mask)) &&
				(G == GetGValue(Mask)) &&
				(B == GetBValue(Mask)))
			{
				return GetTransparency();
			}

			return 0xFF;
		};

	for (std::uint32_t Y = 0; Y < _Height; Y++)
	{
		for (std::uint32_t X = 0; X < _Width; X++)
		{
			switch (_Depth)
			{
			case 4:
				Color4 = Get4bpp(X, Y);
				Image->SetPixel(X, Y, Color4.Pix0);
				Image->SetPixel(++X, Y, Color4.Pix1);
				Image->SetPixel(++X, Y, Color4.Pix2);
				Image->SetPixel(++X, Y, Color4.Pix3);
				break;
			case 8:
				Color8 = Get8bpp(X, Y);
				Image->SetPixel(X, Y, Color8.Pix0);
				Image->SetPixel(++X, Y, Color8.Pix1);
				break;
			case 16:
				Color16 = Get16bpp(X, Y);
				Image->SetPixel(X, Y, (Color16.STP << 15) | (Color16.R << 10) | (Color16.G << 5) | Color16.B);
				break;
			case 24:
				Color24 = Get24bpp(X, Y);
				Image->SetPixel(X, Y, (GetAlpha24(Color24, 0) << 24) | (Color24.R0 << 16) | (Color24.G0 << 8) | Color24.B0);
				Image->SetPixel(++X, Y, (GetAlpha24(Color24, 1) << 24) | (Color24.R1 << 16) | (Color24.G1 << 8) | Color24.B1);
				break;
			}
		}
	}

	return Image;
}