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
	Print texture information
*/
void Sony_PlayStation_Texture::Print(void)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return;
	}

	std::cout << "PlayStation Texture: " << std::endl;
	std::cout << "\tID: " << Header.ID << std::endl;
	std::cout << "\tVersion: " << Header.Version << std::endl;
	std::cout << "\tMode: " << Header.Mode << " (" << GetDepth() << "bpp)" << std::endl;
	std::cout << "\tCF: " << Header.ClutFlag << std::endl;
	std::cout << "\tCLUT X: " << Clut.X << std::endl;
	std::cout << "\tCLUT Y: " << Clut.Y << std::endl;
	std::cout << "\tCLUT nColor: " << Clut.nColor << std::endl;
	std::cout << "\tCLUT nPalette: " << Clut.nPalette << std::endl;
	std::cout << "\tData X: " << Data.X << std::endl;
	std::cout << "\tData Y: " << Data.Y << std::endl;
	std::cout << "\tWidth: " << GetWidth() << std::endl;
	std::cout << "\tHeight: " << GetHeight() << std::endl;
	std::cout << "\tPixels: 0x" << std::hex << Pixels.size() << " bytes" << std::dec << std::endl;

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

		for (std::uint16_t i = 0; i < Clut.nPalette; i++)
		{
			Palette[i].resize(Clut.nColor);
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

	if ((Header.ID == 0x10) && (Header.Version == 0) && (Header.Mode == 4))
	{
		Str->Message("PlayStation Texture: Error, mixed-mode is unsupported");
		Close();
		return _Ptr;
	}

	if ((Header.ID != 0x10) && (Header.Version != 0) && (Header.Mode != 0) && (Header.Mode != 1) && (Header.Mode != 2) && (Header.Mode != 3))
	{
		Str->Message("PlayStation Texture: Error, invalid header at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
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
			Palette[i].resize((static_cast<std::size_t>(Clut.nColor) * sizeof(Sony_Texture_16bpp)));

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

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Write, true, false))
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
	Read palette from file
*/
std::uintmax_t Sony_PlayStation_Texture::ReadPalette(StdFile& File, std::uintmax_t _Ptr, std::size_t iClut)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (!Header.ClutFlag)
	{
		Str->Message("PlayStation Texture: Error, texture does not have a CLUT");
		return false;
	}

	if ((iClut + 1) > Palette.size())
	{
		Str->Message("PlayStation Texture: Warning, invalid CLUT index requested: %d", iClut);
		return false;
	}

	for (std::uint16_t x = 0; x < Clut.nColor; x++, _Ptr += sizeof(Sony_Texture_16bpp))
	{
		File.Read(_Ptr, &Palette[iClut][x], sizeof(Sony_Texture_16bpp));
	}

	return _Ptr;
}


/*
	Copy palette from unsigned char vector
*/
bool Sony_PlayStation_Texture::CopyPalette(std::vector<std::uint8_t> Source, std::size_t iClut)
{
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return false;
	}

	if (!Header.ClutFlag)
	{
		Str->Message("PlayStation Texture: Error, texture does not have a CLUT");
		return false;
	}

	if (Source.size() != (Clut.nColor * sizeof(Sony_Texture_16bpp)))
	{
		Str->Message("PlayStation Texture: Error, invalid palette size: %d, expected %d", Source.size(), (Clut.nColor * sizeof(Sony_Texture_16bpp)));
		return false;
	}

	if ((iClut + 1) > Palette.size())
	{
		Str->Message("PlayStation Texture: Warning, invalid CLUT index requested: %d", iClut);
		return false;
	}

	std::memcpy(Palette[iClut].data(), Source.data(), Source.size());

	return true;
}


/*
	Read pixels from file
*/
std::uintmax_t Sony_PlayStation_Texture::ReadPixels(StdFile& File, std::uintmax_t _Ptr, std::uint32_t _PixelCount)
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
	Copy pixels from unsigned char vector
*/
bool Sony_PlayStation_Texture::CopyPixels(std::vector<std::uint8_t> Source, std::size_t Destination)
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
}


/*
	Get file size
*/
std::uintmax_t Sony_PlayStation_Texture::Size(void) const
{
	if (!b_Open) { return 0; }

	std::uintmax_t FileSize = sizeof(Sony_Texture_Header);

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
	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return;
	}

	if ((Header.ClutFlag) && ((iClut + 1) > Palette.size()))
	{
		Str->Message("PlayStation Texture: Warning, invalid CLUT index requested: %d, using 0 instead", iClut);
		iClut = 0;
	}

	if (Header.ClutFlag)
	{
		Sony_Texture_16bpp Mask16 = Create16bpp(Mask, false);
		DWORD ColorMask = RGB(Red(Mask16), Green(Mask16), Blue(Mask16));
		for (std::size_t i = 0; i < Palette[iClut].size(); i++)
		{
			if ((!Palette[iClut][i].R) && (!Palette[iClut][i].G) && (!Palette[iClut][i].B) && (!Palette[iClut][i].STP) && b_STP4Bpp)
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
	std::unique_ptr<Standard_Image> Image = std::make_unique<Standard_Image>(ImageFormat::BMP, GetWidth(), GetHeight(), GetDepth());

	if (!b_Open)
	{
		Str->Message("PlayStation Texture: Error, texture is not open");
		return Image;
	}

	UpdateBitmapPalette(Image, iClut, Mask);

	std::uint32_t _Width = GetWidth();

	std::uint32_t _Height = GetHeight();

	std::uint32_t _Depth = GetDepth();

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