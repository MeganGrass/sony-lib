/*
*
*	Megan Grass
*	February 26, 2025
*
*/

#include "sony_texture_2.h"

void Sony_PlayStation_Texture_2::DeswizzlePixels(
	std::vector<std::uint8_t> Source, std::vector<std::uint8_t>& Destination, std::uint16_t Depth, std::uint16_t Width, std::uint16_t Height, std::uint16_t TileWidth, std::uint16_t TileHeight)
{
	uint16_t TexelSize = 0;
	size_t pSource = 0;

	switch (Depth)
	{
	case 16: TexelSize = sizeof(Sony_Pixel_16bpp); break;
	case 24: TexelSize = sizeof(Sony_Pixel_24bpp); break;
	case 32: TexelSize = sizeof(Sony_Pixel_32bpp); break;
	default: Destination.resize(0); return;
	}

	Destination.resize((size_t)(Width * Height) * TexelSize);

	if (Source.size() < Destination.size()) { return; }

	for (uint16_t i = 0; i < Height / TileHeight; ++i)
	{
		for (uint16_t x = 0; x < Width / TileWidth; ++x)
		{
			for (uint16_t y = 0; y < TileHeight; ++y)
			{
				size_t pDest = (size_t)((i * TileHeight + y) * Width + (x * TileWidth)) * TexelSize;
				std::memcpy(&Destination.data()[pDest], &Source.data()[pSource], (size_t)(TileWidth * TexelSize));
				pSource += (size_t)(TileWidth * TexelSize);
			}
		}
	}
}

std::uint32_t Sony_PlayStation_Texture_2::GetPalettePtr(std::uint16_t iTexture, std::uint16_t iPalette)
{
	if (!IsPalette(iTexture) || !GetPaletteCount(iTexture)) { return 0; }

	iPalette = std::clamp(iPalette, (uint16_t)0, GetPaletteMaxIndex(iTexture));

	std::uint32_t Size = GetPaletteSingleSize(GetDepth(iTexture), GetPaletteDepth(iTexture));

	if (!Size) { return 0; }

	std::uint16_t x = (iPalette * Size) % Size;
	std::uint16_t y = (iPalette * Size) / Size;

	return (y * Size + x);
}

std::uint16_t Sony_PlayStation_Texture_2::GetPaletteCount(std::uint16_t iTexture)
{
	std::uint16_t PaletteSize = GetPaletteSingleSize(GetDepth(iTexture), GetPaletteDepth(iTexture));

	if (!PaletteSize)
	{
		std::cout << "PlayStation Texture 2 Error: invalid palette size 0x" << std::hex << Texture(iTexture).Data.PaletteSize << std::endl;
		return 0;
	}

	return Texture(iTexture).Data.PaletteSize / PaletteSize;
}

std::uint32_t Sony_PlayStation_Texture_2::GetPaletteSingleSize(std::uint16_t PixelDepth, std::uint16_t PaletteDepth)
{
	if (PixelDepth != 4 && PixelDepth != 8) { return 0; }

	if (PaletteDepth != 16 && PaletteDepth != 24 && PaletteDepth != 32) { return 0; }

	std::uint16_t DataSize = GetPaletteColorMax(PixelDepth);

	return PaletteDepth == 16 ? DataSize * 2 : PaletteDepth == 24 ? DataSize * 3 : DataSize * 4;
}

std::uint32_t Sony_PlayStation_Texture_2::GetMipMipDataSize(std::uint8_t Level)
{
	if (Level <= 1 || Level > 7) { return 0; }

	Level = std::clamp(Level, (uint8_t)2, (uint8_t)7);

	uint32_t Size = Level * sizeof(uint32_t);

	if (Size % 16) { Size += 16 - (Size % 16); }

	return sizeof(Sony_Texture_2_MipMap) + Size;
}

bool Sony_PlayStation_Texture_2::OpenTIM2(StdFile& File, std::uintmax_t pSource, bool b_ReadPalette, bool b_ReadPixels)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture 2 Error: could not open texture image file at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	Sony_Texture_2_Header Header{};
	File.Read(pSource, &Header, sizeof(Sony_Texture_2_Header));

	std::string Identifier(reinterpret_cast<char*>(Header.Type));
	Identifier.resize(4);

	if (std::strcmp(Identifier.c_str(), "TIM2") != 0 && std::strcmp(Identifier.c_str(), "CLT2") != 0)
	{
		Str.Message(L"PlayStation Texture 2 Error: invalid header id (%s) at 0x%llX in \"%ws\"", Identifier.c_str(), pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (Header.Version == 3 && Header.Align)
	{
		Str.Message(L"PlayStation Texture 2 Error: unsupported data sector alignment (0x%02X)", Header.Align);
		return false;
	}

	if (Header.Version == 4 && Header.Align > 1)
	{
		Str.Message(L"PlayStation Texture 2 Error: unsupported data sector alignment (0x%02X)", Header.Align);
		return false;
	}

	if (!Header.DataCount)
	{
		Str.Message(L"PlayStation Texture 2 Error: no texture data found in \"%ws\"", File.GetPath().filename().wstring().c_str());
		return false;
	}

	std::vector<TEXTURE> Texture(Header.DataCount);

	std::uintmax_t BlockSize = Header.Align ? 128 : 16;

	std::uintmax_t Pointer = pSource + sizeof(Sony_Texture_2_Header);

	if (Pointer % BlockSize) { Pointer += BlockSize - (Pointer % BlockSize); }

	for (std::uint16_t i = 0; i < Header.DataCount; i++)
	{
		File.Read(Pointer, &Texture[i].Data, sizeof(Sony_Texture_2_Data));

		if (Texture[i].Data.MipMap >= 2)
		{
			File.Read(Pointer + sizeof(Sony_Texture_2_Data), &Texture[i].MipMap.first, sizeof(Sony_Texture_2_MipMap));

			Texture[i].MipMap.second.resize((size_t)(Texture[i].Data.MipMap - 1));

			File.Read(Pointer + sizeof(Sony_Texture_2_Data) + sizeof(Sony_Texture_2_MipMap), Texture[i].MipMap.second.data(), Texture[i].MipMap.second.size() * sizeof(uint32_t));
		}

		if ((size_t)Texture[i].Data.HeaderSize - (GetMipMipDataSize(Texture[i].Data.MipMap) + sizeof(Sony_Texture_2_Data)))
		{
			File.Read(Pointer + sizeof(Sony_Texture_2_Data) + GetMipMipDataSize(Texture[i].Data.MipMap), &Texture[i].ExData.first, sizeof(Sony_Texture_2_ExData));

			std::string Identifier(reinterpret_cast<char*>(Texture[i].ExData.first.Type));

			Identifier.resize(3);

			if (!std::strcmp(Identifier.c_str(), "eXt"))
			{
				if ((Texture[i].ExData.first.DataSize + sizeof(Sony_Texture_2_ExData)) != Texture[i].ExData.first.Size)
				{
					Texture[i].ExData.second.resize(Texture[i].ExData.first.Size);

					File.Read(Pointer + sizeof(Sony_Texture_2_Data) + GetMipMipDataSize(Texture[i].Data.MipMap) + sizeof(Sony_Texture_2_ExData),
						Texture[i].ExData.second.data(), Texture[i].ExData.second.size());
				}
			}
			else
			{
				std::cout << "PlayStation Texture 2: unknown extended data 0x" << std::hex << Texture[i].ExData.first.Size << std::endl;
			}
		}

		if (b_ReadPixels)
		{
			Texture[i].Pixels.resize(Texture[i].Data.PixelSize);
			File.Read(Pointer + Texture[i].Data.HeaderSize, Texture[i].Pixels.data(), Texture[i].Pixels.size());
		}
		else
		{
			Texture[i].Data.PixelSize = 0;
			Texture[i].Data.PixelMode = 0;
			Texture[i].Data.Width = 0;
			Texture[i].Data.Height = 0;

			Texture[i].Data.Attr.PixelPtr = 0;
			Texture[i].Data.Attr.TexelWidth = 0;
			Texture[i].Data.Attr.PixelMode = 0;
			Texture[i].Data.Attr.Width = 0;
			Texture[i].Data.Attr.Height = 0;
		}

		if (b_ReadPalette)
		{
			Texture[i].Palette.resize(Texture[i].Data.PaletteSize);
			File.Read(Pointer + Texture[i].Data.HeaderSize + Texture[i].Data.PixelSize, Texture[i].Palette.data(), Texture[i].Palette.size());
		}
		else
		{
			Texture[i].Data.PaletteSize = 0;

			Texture[i].Data.PaletteAttr.PixelMode = 0;
			Texture[i].Data.PaletteAttr.Compound = 0;
			Texture[i].Data.PaletteAttr.Align = 0;

			Texture[i].Data.Attr.PalettePtr = 0;
			Texture[i].Data.Attr.PalettePixelMode = 0;
			Texture[i].Data.Attr.PaletteAlign = 0;
			Texture[i].Data.Attr.PaletteBlockPtr = 0;
			Texture[i].Data.Attr.PaletteLoad = 0;

			Texture[i].Data.PaletteAttrEx.Width = 0;
			Texture[i].Data.PaletteAttrEx.U = 0;
			Texture[i].Data.PaletteAttrEx.V = 0;
		}

		Pointer += Texture[i].Data.Size;

		if (Pointer % BlockSize) { Pointer += BlockSize - (Pointer % BlockSize); }

		if (Pointer >= File.Size()) { break; }
	}

	{
		std::memcpy(&m_Header, &Header, sizeof(Sony_Texture_2_Header));

		m_Texture.resize(Texture.size());

		for (std::uint16_t i = 0; i < (uint16_t)m_Texture.size(); i++)
		{
			m_Texture[i] = std::make_unique<TEXTURE>();

			std::memcpy(&m_Texture[i]->Data, &Texture[i].Data, sizeof(Sony_Texture_2_Data));

			std::memcpy(&m_Texture[i]->MipMap.first, &Texture[i].MipMap.first, sizeof(Sony_Texture_2_MipMap));
			m_Texture[i]->MipMap.second.resize(Texture[i].MipMap.second.size());
			std::memcpy(m_Texture[i]->MipMap.second.data(), Texture[i].MipMap.second.data(), Texture[i].MipMap.second.size() * sizeof(uint32_t));

			std::memcpy(&m_Texture[i]->ExData.first, &Texture[i].ExData.first, sizeof(Sony_Texture_2_ExData));
			m_Texture[i]->ExData.second.resize(Texture[i].ExData.second.size());
			std::memcpy(m_Texture[i]->ExData.second.data(), Texture[i].ExData.second.data(), Texture[i].ExData.second.size());

			m_Texture[i]->Pixels.resize(Texture[i].Pixels.size());
			std::memcpy(m_Texture[i]->Pixels.data(), Texture[i].Pixels.data(), Texture[i].Pixels.size());

			m_Texture[i]->Palette.resize(Texture[i].Palette.size());
			std::memcpy(m_Texture[i]->Palette.data(), Texture[i].Palette.data(), Texture[i].Palette.size());

			//Str.Message(File.GetPath().filename().string() + "\r\n\r\n" + HeaderStr() + DataStr(i) + PaletteStr(i) + PixelStr(i) + AttrStr(i) + SamplingStr(i) + AlphaStr(i) + ExDataStr(i));
		}
	}

	b_Open = true;

	return b_Open;
}

void Sony_PlayStation_Texture_2::UpdateImagePalette(std::unique_ptr<Standard_Image>& Image, std::uint16_t iTexture, std::uint16_t iPalette)
{
	if (m_Texture.empty()) { return; }

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	if (!IsPalette(iTexture) || !GetPaletteCount(iTexture))
	{
		std::cout << "PlayStation Texture 2 Warning: palette data is empty" << std::endl;
		return;
	}

	iPalette = std::clamp(iPalette, (uint16_t)0, GetPaletteMaxIndex(iTexture));

	std::uint16_t PixelDepth = GetDepth(iTexture);
	std::uint16_t PaletteDepth = GetPaletteDepth(iTexture);
	std::uint16_t MaxColors = GetPaletteColorMax(PixelDepth);

	if (!Texture(iTexture).Data.PaletteAttr.Align)
	{
		std::vector<std::uint8_t> Destination;
		DeswizzlePixels(Texture(iTexture).Palette, Destination, PaletteDepth, GetVRAMPaletteWidth(iTexture), GetVRAMPaletteHeight(iTexture));
		Texture(iTexture).Palette = Destination;
	}

	std::uint8_t Alpha0 = Texture(iTexture).Data.Alpha.Value0;
	std::uint8_t Alpha1 = Texture(iTexture).Data.Alpha.Value1;
	std::uint8_t Function = Texture(iTexture).Data.Alpha.Func;

	for (std::uint16_t i = 0; i < MaxColors; i++)
	{
		if (PaletteDepth == 16)
		{
			Sony_Pixel_16bpp Color = GetPaletteColor16bpp(iTexture, iPalette, i);

			if (!Color.A && !Function)
			{
				Image->SetPalette(i, { Color.Blue(), Color.Green(), Color.Red(), Alpha0 });
			}

			else if (!Color.A && Function && !Color.Red() && !Color.Green() && !Color.Blue())
			{
				Image->SetPalette(i, { 0x00, 0x00, 0x00, 0x00 });
			}

			else if (Color.A)
			{
				Image->SetPalette(i, { Color.Blue(), Color.Green(), Color.Red(), Alpha1 });
			}

			else
			{
				Image->SetPalette(i, { Color.Blue(), Color.Green(), Color.Red(), (uint8_t)(Color.A ? 0x00 : 0xFF) });
			}
		}
		if (PaletteDepth == 24)
		{
			Sony_Pixel_24bpp Color = GetPaletteColor24bpp(iTexture, iPalette, i);

			if (!Function)
			{
				Image->SetPalette(i, { Color.B, Color.G, Color.R, Alpha0 });
			}

			else if (Function && !Color.R && !Color.G && !Color.B)
			{
				Image->SetPalette(i, { 0x00, 0x00, 0x00, 0x00 });
			}

			else
			{
				Image->SetPalette(i, { Color.B, Color.G, Color.R, 0xFF });
			}
		}
		if (PaletteDepth == 32)
		{
			Sony_Pixel_32bpp Color = GetPaletteColor32bpp(iTexture, iPalette, i);
			Image->SetPalette(i, { Color.B, Color.G, Color.R, Color.A });
		}
	}
}

std::unique_ptr<Standard_Image> Sony_PlayStation_Texture_2::ExportImage(std::uint16_t iTexture, std::uint16_t iPalette)
{
	if (!b_Open || m_Texture.empty())
	{
		Str.Message(L"PlayStation Texture 2 Error: texture is not open");
		return std::make_unique<Standard_Image>();
	}

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	if (Texture(iTexture).Pixels.empty())
	{
		std::cout << "PlayStation Texture 2 Warning: pixel data is empty" << std::endl;
		return std::make_unique<Standard_Image>();
	}

	std::uint16_t Depth = GetDepth(iTexture);

	std::uint16_t Width = GetWidth(iTexture);

	std::uint16_t Height = GetHeight(iTexture);

	std::unique_ptr<Standard_Image> Image = std::make_unique<Standard_Image>();

	Image->Str.hWnd = Str.hWnd;

	Image->Create(Depth, Width, Height);

	std::uint16_t PaletteDepth = GetPaletteDepth(iTexture);

	if (Depth == 4 || Depth == 8)
	{
		UpdateImagePalette(Image, iPalette);
	}

	Sony_Pixel_4bpp Color4{};

	Sony_Pixel_8bpp Color8{};

	Sony_Pixel_16bpp Color16{};

	Sony_Pixel_24bpp Color24{};

	Sony_Pixel_32bpp Color32{};

	std::uint8_t Alpha0 = Texture(iTexture).Data.Alpha.Value0;
	std::uint8_t Alpha1 = Texture(iTexture).Data.Alpha.Value1;
	std::uint8_t Function = Texture(iTexture).Data.Alpha.Func;

	if (PaletteDepth == 16 && !Alpha0) { Alpha0 = 0xFF; }

	for (std::uint16_t Y = 0; Y < Height; Y++)
	{
		for (std::uint16_t X = 0; X < Width; X++)
		{
			switch (Depth)
			{
			case 4:
				Color4 = Get4bpp(iTexture, X, Y);
				Image->SetPixel(X, Y, Color4.Pix0);
				Image->SetPixel(++X, Y, Color4.Pix1);
				break;
			case 8:
				Color8 = Get8bpp(iTexture, X, Y);
				Image->SetPixel(X, Y, Color8.Pixel);
				break;
			case 16:
				Color16 = Get16bpp(iTexture, X, Y);
				if (!Color16.A && !Function)
				{
					Image->SetPixel(X, Y, (Alpha0 << 15) | (Color16.R << 10) | (Color16.G << 5) | Color16.B);
				}
				else if (!Color16.A && Function && !Color16.Red() && !Color16.Green() && !Color16.Blue())
				{
					Image->SetPixel(X, Y, 0x00);
				}
				else if (Color16.A)
				{
					Image->SetPixel(X, Y, (Alpha1 << 15) | (Color16.R << 10) | (Color16.G << 5) | Color16.B);
				}
				else
				{
					Image->SetPixel(X, Y, (Color16.A << 15) | (Color16.R << 10) | (Color16.G << 5) | Color16.B);
				}
				break;
			case 24:
				Color24 = Get24bpp(iTexture, X, Y);
				if (!Function)
				{
					Image->SetPixel(X, Y, (Alpha0 << 24) | (Color24.R << 16) | (Color24.G << 8) | Color24.B);
				}
				else if (Function && !Color24.R && !Color24.G && !Color24.B)
				{
					Image->SetPixel(X, Y, 0x00);
				}
				else
				{
					Image->SetPixel(X, Y, (0xFF << 24) | (Color24.R << 16) | (Color24.G << 8) | Color24.B);
				}
				break;
			case 32:
				Color32 = Get32bpp(iTexture, X, Y);
				Image->SetPixel(X, Y, (Color32.A << 24) | (Color32.R << 16) | (Color32.G << 8) | Color32.B);
				break;
			}
		}
	}

	return Image;
}