/*
*
*	Megan Grass
*	March 07, 2024
*
*/

#include "sony_texture.h"

#include <std_thread_pool.h>

DWORD Sony_PlayStation_Texture::m_TransparentColor = RGB(0, 0, 0);

String Sony_PlayStation_Texture::About(void)
{
	String Output = Str.FormatCStyle("PlayStation Texture:\n");

	Output += Str.FormatCStyle("\tID: %d\n", m_Header.ID);
	Output += Str.FormatCStyle("\tVersion Number: %d\n", m_Header.Version);
	Output += Str.FormatCStyle("\tPixel Mode: %d (%dbpp)\n", m_Header.PixelMode, GetDepth());
	Output += Str.FormatCStyle("\tCLUT Flag: %d\n", m_Header.ClutFlag);
	Output += Str.FormatCStyle("\tCLUT Size: 0x%X bytes\n", GetPaletteDataSize());
	Output += Str.FormatCStyle("\tCLUT X: %d\n", GetPaletteX());
	Output += Str.FormatCStyle("\tCLUT Y: %d\n", GetPaletteY());
	Output += Str.FormatCStyle("\tCLUT Width: %d\n", GetPaletteWidth());
	Output += Str.FormatCStyle("\tCLUT Height: %d\n", GetPaletteHeight());
	Output += Str.FormatCStyle("\tCLUT Count: %d palette(s)\n", GetPaletteCount());
	Output += Str.FormatCStyle("\tPixel Size: 0x%X bytes\n", GetPixelDataSize());
	Output += Str.FormatCStyle("\tPixel X: %d\n", GetPixelX());
	Output += Str.FormatCStyle("\tPixel Y: %d\n", GetPixelY());
	Output += Str.FormatCStyle("\tPixel Width: %d\n", GetWidth());
	Output += Str.FormatCStyle("\tPixel Height: %d\n", GetHeight());
	Output += Str.FormatCStyle("\tPixel Count: %d pixels\n", GetPixelCount());
	Output += Str.FormatCStyle("\tFile Size: 0x%X bytes\n", Size());

	return Output;
}

void Sony_PlayStation_Texture::Close(void)
{
	b_Open = false;
	std::memset(&m_Header, 0, sizeof(Sony_Texture_Header));
	std::memset(&m_PaletteHeader, 0, sizeof(Sony_Texture_Data));
	std::memset(&m_PixelHeader, 0, sizeof(Sony_Texture_Data));
	m_Palette.clear();
	m_Palette.shrink_to_fit();
	m_Pixels.clear();
	m_Pixels.shrink_to_fit();
	m_TransparentColor = 0;
	m_Transparency = Sony_Texture_Transparency::None;
}

void Sony_PlayStation_Texture::UpdateTransparencyFlags(void)
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
		TransparencySTP() = false;
		TransparencySuperimposed() = false;
		TransparencyExternal() = false;
	}

	TransparencyFlags() = Sony_Texture_Transparency::None;

	if (TransparencySuperblack())
	{
		TransparencyFlags() |= Sony_Texture_Transparency::Superblack;
	}

	if (TransparencySuperimposed())
	{
		TransparencyFlags() |= Sony_Texture_Transparency::Superimposed;
	}

	if (TransparencyExternal())
	{
		TransparencyFlags() |= Sony_Texture_Transparency::External;
	}

	if (TransparencySTP())
	{
		TransparencyFlags() = Sony_Texture_Transparency::STP;
		TransparencySuperblack() = false;
		TransparencySuperimposed() = false;
		TransparencyExternal() = false;
	}

	if (!TransparencyHalf() && !TransparencyFull() && !TransparencyInverse() && !TransparencyQuarter())
	{
		TransparencyFull() = true;
	}

	if (TransparencyHalf())
	{
		TransparencyFlags() |= Sony_Texture_Transparency::Half;
	}
	else if (TransparencyFull())
	{
		TransparencyFlags() |= Sony_Texture_Transparency::Full;
	}
	else if (TransparencyInverse())
	{
		TransparencyFlags() |= Sony_Texture_Transparency::Inverse;
	}
	else if (TransparencyQuarter())
	{
		TransparencyFlags() |= Sony_Texture_Transparency::Quarter;
	}
}

std::uint16_t Sony_PlayStation_Texture::GetDepth(void) const
{
	switch (m_Header.PixelMode)
	{
	case 0: return 4;
	case 1: return 8;
	case 2: return 16;
	case 3: return 24;
	}
	return 0;
}

void Sony_PlayStation_Texture::SetDepth(std::uint16_t Depth)
{
	switch (Depth)
	{
	case 4: m_Header.PixelMode = 0; break;
	case 8: m_Header.PixelMode = 1; break;
	case 16: m_Header.PixelMode = 2; break;
	case 24: m_Header.PixelMode = 3; break;
	}
}

void Sony_PlayStation_Texture::SetPaletteWidth(std::uint16_t Width)
{
	Width = std::clamp(Width, (uint16_t)0, (uint16_t)1024);

	if (GetDepth() == 4)
	{
		Width = (Width & 0x0F) ? (Width + (16 - (Width & 0x0F))) : Width;
	}
	else
	{
		Width = (Width & 0xFF) ? (Width + (256 - (Width & 0xFF))) : Width;
	}

	if (Width < m_PaletteHeader.Width)
	{
		m_PaletteHeader.Height = (m_PaletteHeader.Height * (((m_PaletteHeader.Width - Width) / GetPaletteColorMax()) * 2));
	}

	m_PaletteHeader.Width = Width;

	m_Palette.resize((size_t)(m_PaletteHeader.Width * m_PaletteHeader.Height));

	SetCF(!m_Palette.empty());

	UpdatePaletteDataSize();
}

void Sony_PlayStation_Texture::SetPaletteHeight(std::uint16_t Height)
{
	m_PaletteHeader.Height = std::clamp(Height, (uint16_t)0, (uint16_t)512);

	m_Palette.resize((size_t)(m_PaletteHeader.Width * m_PaletteHeader.Height));

	SetCF(!m_Palette.empty());

	UpdatePaletteDataSize();
}

std::uint32_t Sony_PlayStation_Texture::GetPalettePtr(std::uint16_t iPalette) const
{
	if (!GetCF() || GetPaletteWidth() == 0)
	{
		return 0;
	}

	iPalette = std::clamp(iPalette, (uint16_t)0, GetPaletteMaxIndex());

	std::uint16_t x = (iPalette * GetPaletteColorMax()) % GetPaletteWidth();
	std::uint16_t y = (iPalette * GetPaletteColorMax()) / GetPaletteWidth();

	return (y * GetPaletteWidth() + x);
}

std::vector<Sony_Pixel_16bpp> Sony_PlayStation_Texture::GrayScalePalette(std::uint16_t Depth)
{
	std::uint16_t nColors = 256;

	if (Depth == 4)
	{
		nColors = 16;
	}

	std::vector<Sony_Pixel_16bpp> ExPalette(nColors);

	if (Depth == 4)
	{
		for (std::uint16_t i = 0; i < nColors; i++)
		{
			ExPalette[i] = Create16bpp((uint8_t)(i * 16), (uint8_t)(i * 16), (uint8_t)(i * 16), false);
		}
	}
	else
	{
		for (std::uint16_t i = 0; i < nColors; i++)
		{
			ExPalette[i] = Create16bpp((uint8_t)i, (uint8_t)i, (uint8_t)i, false);
		}
	}

	return ExPalette;
}

std::vector<Sony_Pixel_16bpp> Sony_PlayStation_Texture::PaletteFromUChar(std::vector<std::uint8_t> Source) const
{
	if (Source.empty()) { return std::vector<Sony_Pixel_16bpp>(GetPaletteColorMax()); }

	std::uint16_t PaletteSize = GetPaletteSingleSize();
	std::uint32_t PaletteSizeMax = PaletteSize * GetPaletteCountMax();

	if (Source.size() > PaletteSizeMax)
	{
		PaletteSize = PaletteSizeMax;
	}

	if (Source.size() % PaletteSize)
	{
		Source.resize(Source.size() + (PaletteSize - (Source.size() % PaletteSize)));
	}

	std::vector<Sony_Pixel_16bpp> ExPalette((Source.size() / PaletteSize) * GetPaletteColorMax());

	for (std::size_t i = 0; i < (Source.size() / sizeof(Sony_Pixel_16bpp)); ++i)
	{
		std::size_t pSource = i * sizeof(Sony_Pixel_16bpp);

		if (pSource + sizeof(Sony_Pixel_16bpp) <= Source.size())
		{
			std::memcpy(&ExPalette[i], &Source[pSource], sizeof(Sony_Pixel_16bpp));
		}
	}

	return ExPalette;
}

std::vector<std::uint8_t> Sony_PlayStation_Texture::UCharFromPalette(std::vector<Sony_Pixel_16bpp> Source) const
{
	if (Source.empty()) { return std::vector<std::uint8_t>(GetPaletteSingleSize()); }

	if (Source.size() > (size_t)(GetPaletteCountMax() * GetPaletteColorMax()))
	{
		Source.resize((size_t)(GetPaletteCountMax() * GetPaletteColorMax()));
	}

	if (Source.size() % GetPaletteColorMax())
	{
		Source.resize(Source.size() + (GetPaletteColorMax() - (Source.size() % GetPaletteColorMax())));
	}

	std::vector<std::uint8_t> ExPalette(Source.size() * sizeof(Sony_Pixel_16bpp));

	for (std::size_t i = 0; i < Source.size(); ++i)
	{
		std::size_t pSource = i * sizeof(Sony_Pixel_16bpp);

		if (pSource + sizeof(Sony_Pixel_16bpp) <= ExPalette.size())
		{
			std::memcpy(&ExPalette[pSource], &Source[i], sizeof(Sony_Pixel_16bpp));
		}
	}

	return ExPalette;
}

std::vector<Sony_Pixel_16bpp> Sony_PlayStation_Texture::ConvertPalette(std::vector<Sony_Pixel_16bpp> Source, std::uint16_t nColorSource, std::uint16_t nColorOut) const
{
	if ((nColorSource != 16 && nColorSource != 256) || (nColorOut != 16 && nColorOut != 256))
	{
		return std::vector<Sony_Pixel_16bpp>();
	}

	if (Source.size() % nColorSource)
	{
		Source.resize(Source.size() + (nColorSource - (Source.size() % nColorSource)));
	}

	std::size_t nPaletteOut = std::clamp((uint16_t)(Source.size() / nColorSource), (uint16_t)0, (uint16_t)GetPaletteCountMax());

	if (nColorSource < nColorOut)
	{
		nPaletteOut = (Source.size() / nColorSource);
	}
	else if (nColorSource > nColorOut)
	{
		nPaletteOut = (Source.size() / nColorOut);
	}

	std::vector<Sony_Pixel_16bpp> ExPalette(nPaletteOut * nColorOut);

	for (std::size_t i = 0; i < nPaletteOut; ++i)
	{
		for (std::size_t x = 0; x < nColorSource; ++x)
		{
			std::size_t pSource = (i * nColorSource + x);
			std::size_t pPalette = (i * nColorOut + x);

			if (pSource < Source.size())
			{
				ExPalette[pPalette] = Source[pSource];
			}
		}
	}

	return ExPalette;
}

bool Sony_PlayStation_Texture::CopyPalette(std::vector<Sony_Pixel_16bpp>& Out, std::uint16_t iPalette) const
{
	Out.clear();
	Out.shrink_to_fit();

	if (!GetPaletteCount())
	{
		return false;
	}

	std::uint32_t pPalette = GetPalettePtr(iPalette);

	std::copy(m_Palette.begin() + pPalette, m_Palette.begin() + pPalette + GetPaletteColorMax(), std::back_inserter(Out));

	return true;
}

bool Sony_PlayStation_Texture::PastePalette(std::vector<Sony_Pixel_16bpp> Out, std::uint16_t iPalette)
{
	if (!Out.empty())
	{
		if (!GetPaletteCount())
		{
			iPalette = 0;
			m_PaletteHeader.Width = GetPaletteColorMax();
			SetPaletteHeight(m_PaletteHeader.Height = 1);
		}

		if (Out.size() % GetPaletteColorMax())
		{
			Out.resize(Out.size() + (GetPaletteColorMax() - (Out.size() % GetPaletteColorMax())));
		}

		std::copy(Out.begin(), Out.end(), m_Palette.begin() + GetPalettePtr(iPalette));

		return true;
	}

	return false;
}

bool Sony_PlayStation_Texture::MovePalette(std::uint16_t iPalette, bool b_MoveRight)
{
	if (!GetPaletteCount() || !GetPaletteMaxIndex())
	{
		return false;
	}

	std::uint32_t pPalette = GetPalettePtr(iPalette);

	if (b_MoveRight && (iPalette < GetPaletteMaxIndex()))
	{
		std::uint16_t iNextPalette = pPalette + GetPaletteColorMax();
		std::swap_ranges(m_Palette.begin() + pPalette, m_Palette.begin() + iNextPalette, m_Palette.begin() + iNextPalette);
		return true;
	}
	else if (iPalette > 0)
	{
		std::uint16_t iPrevPalette = pPalette - GetPaletteColorMax();
		std::swap_ranges(m_Palette.begin() + iPrevPalette, m_Palette.begin() + pPalette, m_Palette.begin() + pPalette);
		return true;
	}

	return false;
}

void Sony_PlayStation_Texture::AddPalette(std::vector<Sony_Pixel_16bpp> Source)
{
	if (!GetPaletteCount())
	{
		m_PaletteHeader.Width = GetPaletteColorMax();
		m_PaletteHeader.Height = 0;
	}

	std::uint16_t AddHeight = 1;

	if (!Source.empty())
	{
		if (Source.size() % GetPaletteColorMax())
		{
			Source.resize(Source.size() + (GetPaletteColorMax() - (Source.size() % GetPaletteColorMax())));
		}

		m_Palette.insert(m_Palette.end(), Source.begin(), Source.end());

		AddHeight = std::clamp((uint16_t)((Source.size() + m_PaletteHeader.Width - 1) / m_PaletteHeader.Width), (uint16_t)0, (uint16_t)512);
	}
	else
	{
		std::vector<Sony_Pixel_16bpp> ExPalette(GetPaletteColorMax());

		m_Palette.insert(m_Palette.end(), ExPalette.begin(), ExPalette.end());
	}

	SetPaletteHeight(GetPaletteHeight() + AddHeight);

	SetCF(true);

	UpdatePaletteDataSize();
}

void Sony_PlayStation_Texture::InsertPalette(std::uint16_t iPalette, std::vector<Sony_Pixel_16bpp> Source)
{
	if (!GetPaletteCount())
	{
		AddPalette();
		return;
	}

	AddPalette();

	std::uint32_t pPalette = GetPalettePtr(iPalette);

	if (!Source.empty())
	{
		Source.resize(GetPaletteColorMax());

		m_Palette.insert(m_Palette.begin() + pPalette, Source.begin(), Source.end());
	}
	else
	{
		std::vector<Sony_Pixel_16bpp> ExPalette(GetPaletteColorMax());

		m_Palette.insert(m_Palette.begin() + pPalette, ExPalette.begin(), ExPalette.end());
	}

	m_Palette.resize((size_t)(m_PaletteHeader.Width* m_PaletteHeader.Height));
}

bool Sony_PlayStation_Texture::DeletePalette(std::uint16_t iPalette, bool b_All)
{
	if (!GetPaletteCount())
	{
		return false;
	}

	if (b_All)
	{
		m_Palette.clear();
		m_Palette.shrink_to_fit();
	}
	else
	{
		std::uint32_t pPalette = GetPalettePtr(iPalette);

		if (GetPaletteWidth() % GetPaletteColorMax())
		{
			std::vector<Sony_Pixel_16bpp> ExPalette(GetPaletteColorMax());

			std::copy(m_Palette.begin() + pPalette, m_Palette.begin() + pPalette + GetPaletteColorMax(), ExPalette.begin());
		}
		else
		{
			m_Palette.erase(m_Palette.begin() + pPalette, m_Palette.begin() + pPalette + GetPaletteColorMax());

			SetPaletteHeight(GetPaletteHeight() - 1);
		}
	}

	if (m_Palette.empty())
	{
		SetCF(false);

		SetPaletteWidth(0);
		SetPaletteHeight(0);

		UpdatePaletteDataSize();
	}

	return true;
}

void Sony_PlayStation_Texture::SetWidth(std::uint16_t Width)
{
	switch (m_Header.PixelMode)
	{
	case 0:
		if (Width % 4)
		{
			Width = Width + (4 - (Width % 4));
		}
		m_PixelHeader.Width = Width / 4;
		break;
	case 1:
		if (Width % 2)
		{
			Width = Width + (2 - (Width % 2));
		}
		m_PixelHeader.Width = Width / 2;
		break;
	case 2:
		m_PixelHeader.Width = Width;
		break;
	case 3:
		m_PixelHeader.Width = (Width * 3) / 2;
		break;
	}

	m_Pixels.resize(GetPixelSize());

	UpdatePixelDataSize();
}

void Sony_PlayStation_Texture::SetHeight(std::uint16_t Height)
{
	m_PixelHeader.Height = Height;

	m_Pixels.resize(GetPixelSize());

	UpdatePixelDataSize();
}

bool Sony_PlayStation_Texture::ReadData(StdFile& File, std::uintmax_t pSource, Sony_Texture_Data& OutHeader, std::vector<std::uint8_t>& OutData)
{
	std::memset(&OutHeader, 0, sizeof(Sony_Texture_Data));

	OutData.clear();
	OutData.shrink_to_fit();

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not read data at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	if ((!File.Size()) || (pSource > File.Size()) || ((pSource + sizeof(Sony_Texture_Data)) > File.Size()))
	{
		Str.Message(L"PlayStation Texture Error: invalid data file size (0x%llX) and/or pointer (0x%llX)", File.Size(), pSource);
		return false;
	}

	Sony_Texture_Data ExData{};
	File.Read(pSource, &ExData, sizeof(Sony_Texture_Data));

	std::size_t DataSize = ExData.Size;
	if ((DataSize & sizeof(Sony_Texture_Data)) == sizeof(Sony_Texture_Data)) { DataSize ^= sizeof(Sony_Texture_Data); }

	if ((!ExData.Size) ||
		(ExData.Size > (uint32_t)File.Size()) ||
		((ExData.Size + pSource) > File.Size()) ||
		(DataSize != ((size_t)(ExData.Width * ExData.Height) * 2)))
	{
		Str.Message(L"PlayStation Texture Error: invalid data size (0x%llX)", ExData.Size);
		return false;
	}

	ExData.Size = ((size_t)(ExData.Width * ExData.Height) * 2) + sizeof(Sony_Texture_Data);

	std::memcpy(&OutHeader, &ExData, sizeof(Sony_Texture_Data));

	if (ExData.Width && ExData.Height)
	{
		OutData.resize(DataSize);
		File.Read(pSource + sizeof(Sony_Texture_Data), OutData.data(), OutData.size());
	}

	return true;
}

bool Sony_PlayStation_Texture::WriteData(StdFile& File, std::uintmax_t pSource, Sony_Texture_Data OutHeader, std::vector<std::uint8_t> OutData)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not write data at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	File.Write(pSource, &OutHeader, sizeof(Sony_Texture_Data));
	File.Write(pSource + sizeof(Sony_Texture_Data), OutData.data(), OutData.size());

	return true;
}

bool Sony_PlayStation_Texture::ReadPalette(StdFile& File, std::uintmax_t pSource, std::uint16_t nPalette, bool b_Add, bool b_Paste, std::uint16_t iPalette)
{
	if (!nPalette)
	{
		Str.Message(L"PlayStation Texture Error: invalid palette count (%d)", nPalette);
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not read palette at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	if (!File.Size())
	{
		Str.Message(L"PlayStation Texture Error: attempting to read invalid palette file size (0x%llX)", File.Size());
		return false;
	}

	if (pSource > File.Size())
	{
		Str.Message(L"PlayStation Texture Error: invalid palette pointer (0x%llX) for file size (0x%llX)", pSource, File.Size());
		return false;
	}

	nPalette = std::clamp(nPalette, (uint16_t)0, (uint16_t)GetPaletteCountMax());

	std::uint32_t ReadSize = (nPalette * GetPaletteSingleSize());

	if (ReadSize > File.Size())
	{
		ReadSize = (uint32_t)(File.Size() - pSource);
		nPalette = (uint16_t)(ReadSize / GetPaletteSingleSize());
	}

	if (!ReadSize)
	{
		Str.Message(L"PlayStation Texture Error: attempting to read invalid palette file size (0x%X)", ReadSize);
		return false;
	}

	if ((pSource + ReadSize) > File.Size())
	{
		Str.Message(L"PlayStation Texture Error: invalid palette file size (0x%llX) and/or pointer (0x%llX)", File.Size(), pSource);
		return false;
	}

	std::vector<std::uint8_t> ExPaletteData(ReadSize);

	File.Read(pSource, ExPaletteData.data(), ExPaletteData.size());

	if (b_Paste)
	{
		PastePalette(PaletteFromUChar(ExPaletteData), iPalette);
	}
	else if (b_Add)
	{
		AddPalette(PaletteFromUChar(ExPaletteData));
	}
	else
	{
		m_PaletteHeader.Width = GetPaletteColorMax();
		m_PaletteHeader.Height = nPalette;

		m_PaletteHeader.Size = (uint32_t)(ExPaletteData.size() + sizeof(Sony_Texture_Data));

		m_Palette.resize(ExPaletteData.size() / sizeof(Sony_Pixel_16bpp));

		std::memcpy(m_Palette.data(), ExPaletteData.data(), ExPaletteData.size());

		SetCF(true);
	}

	return true;
}

bool Sony_PlayStation_Texture::ReadPixels(StdFile& File, std::uintmax_t pSource, std::uint16_t Width, std::uint16_t Height)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not read pixels at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	if (!File.Size())
	{
		Str.Message(L"PlayStation Texture Error: attempting to read invalid pixel file size (0x%llX)", File.Size());
		return false;
	}

	if (pSource > File.Size())
	{
		Str.Message(L"PlayStation Texture Error: invalid pixel pointer (0x%llX) for file size (0x%llX)", pSource, File.Size());
		return false;
	}

	std::uint32_t ReadSize = GetPixelSize(GetDepth(), Width, Height);

	if (!ReadSize)
	{
		Str.Message(L"PlayStation Texture Error: attempting to read invalid pixel file size (0x%llX)", ReadSize);
		return false;
	}

	if ((pSource + ReadSize) > File.Size())
	{
		Str.Message(L"PlayStation Texture Error: invalid pixel file size (0x%llX) and/or pointer (0x%llX) when trying to read 0x%X bytes", File.Size(), pSource, ReadSize);
		return false;
	}

	std::uint16_t ExWidth = Width;

	switch (m_Header.PixelMode)
	{
	case 0:
		if (ExWidth % 4)
		{
			ExWidth = ExWidth + (4 - (ExWidth % 4));
		}
		ExWidth = ExWidth / 4;
		break;
	case 1:
		if (ExWidth % 2)
		{
			ExWidth = ExWidth + (2 - (ExWidth % 2));
		}
		ExWidth = ExWidth / 2;
		break;
	case 2:
		ExWidth = ExWidth;
		break;
	case 3:
		ExWidth = (ExWidth * 3) / 2;
		break;
	}

	m_PixelHeader.Width = ExWidth;
	m_PixelHeader.Height = Height;

	m_PixelHeader.Size = GetPixelDataSize();

	m_Pixels.resize(ReadSize);

	File.Read(pSource, m_Pixels.data(), m_Pixels.size());

	return true;
}

bool Sony_PlayStation_Texture::WritePalette(StdFile& File, std::uintmax_t pSource, std::uint16_t iPalette, bool b_WriteAll)
{
	if (!GetPaletteSize())
	{
		Str.Message(L"PlayStation Texture Error: palette data is empty");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not write raw palette data at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	if (b_WriteAll)
	{
		File.Write(pSource, m_Palette.data(), m_Palette.size() * sizeof(Sony_Pixel_16bpp));
	}
	else
	{
		std::uint32_t pPalette = GetPalettePtr(iPalette);
		File.Write(pSource, m_Palette.data() + pPalette, GetPaletteSingleSize());
	}

	return true;
}

bool Sony_PlayStation_Texture::WritePixels(StdFile& File, std::uintmax_t pSource)
{
	if (!GetPixelSize())
	{
		Str.Message(L"PlayStation Texture Error: pixel data is empty");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not write raw pixel data at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	File.Write(pSource, m_Pixels.data(), m_Pixels.size());

	return true;
}

bool Sony_PlayStation_Texture::ReadPaletteTIM(std::unique_ptr<Sony_PlayStation_Texture>& External, bool b_Add)
{
	if (!External->IsOpen() || !External->IsValid())
	{
		Str.Message(L"PlayStation Texture Error: external palette data is empty");
		return false;
	}

	if (!External->GetCF())
	{
		Str.Message(L"PlayStation Texture Error: external palette data is empty");
		return false;
	}

	if (b_Add)
	{
		AddPalette(ConvertPalette(External->GetPalette(), External->GetPaletteColorMax(), GetPaletteColorMax()));
	}
	else
	{
		m_PaletteHeader.X = External->GetPaletteX();
		m_PaletteHeader.Y = External->GetPaletteY();

		m_PaletteHeader.Width = External->GetPaletteWidth();
		m_PaletteHeader.Height = External->GetPaletteHeight();

		m_PaletteHeader.Size = External->GetPaletteDataSize();

		m_Palette = External->GetPalette();

		SetCF(true);
	}

	return true;
}

bool Sony_PlayStation_Texture::ReadPaletteTIM(StdFile& File, std::uintmax_t pSource, bool b_Add)
{
	std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

#ifdef _WINDOWS
	External->Str.hWnd = Str.hWnd;
#endif

	if (!External->OpenTIM(File, pSource, true, false))
	{
		return false;
	}

	return ReadPaletteTIM(External, b_Add);
}

bool Sony_PlayStation_Texture::ReadPixelsTIM(std::unique_ptr<Sony_PlayStation_Texture>& External)
{
	if (!External->IsOpen() || !External->IsValid())
	{
		Str.Message(L"PlayStation Texture Error: external pixel data is empty");
		return false;
	}

	//if (External->GetDepth() != GetDepth())
	//{
	//	Str.Message(L"PlayStation Texture Error: invalid pixel depth (%d) when reading external pixels", External->GetDepth());
	//	return false;
	//}

	if (!External->GetWidth() || !External->GetHeight())
	{
		Str.Message(L"PlayStation Texture Error: external pixel data is empty");
		return false;
	}

	switch (External->GetDepth())
	{
	case 4: m_Header.PixelMode = 0; break;
	case 8: m_Header.PixelMode = 1; break;
	case 16: m_Header.PixelMode = 2; break;
	case 24: m_Header.PixelMode = 3; break;
	}

	m_PixelHeader.X = External->GetPixelX();
	m_PixelHeader.Y = External->GetPixelY();

	switch (m_Header.PixelMode)
	{
	case 0: m_PixelHeader.Width = External->GetWidth() / 4; break;
	case 1: m_PixelHeader.Width = External->GetWidth() / 2; break;
	case 2: m_PixelHeader.Width = External->GetWidth(); break;
	case 3: m_PixelHeader.Width = ((External->GetWidth() / 2) * 3); break;
	}

	m_PixelHeader.Height = External->GetHeight();

	m_PixelHeader.Size = External->GetPixelDataSize();

	m_Pixels = External->GetPixels();

	return true;
}

bool Sony_PlayStation_Texture::ReadPixelsTIM(StdFile& File, std::uintmax_t pSource)
{
	std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

#ifdef _WINDOWS
	External->Str.hWnd = Str.hWnd;
#endif

	if (!External->OpenTIM(File, pSource, false, true))
	{
		return false;
	}

	return ReadPixelsTIM(External);
}

bool Sony_PlayStation_Texture::WritePaletteTIM(StdFile& File, std::uintmax_t pSource, std::uint16_t iPalette, bool b_WriteAll)
{
	if (!GetPaletteSize())
	{
		Str.Message(L"PlayStation Texture Error: palette data is empty");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not write palette data at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

#ifdef _WINDOWS
	External->Str.hWnd = Str.hWnd;
#endif

	if (!External->Create(GetDepth(), 0, 0, 0))
	{
		return false;
	}

	External->SetPaletteX(GetPaletteX());
	External->SetPaletteY(GetPaletteY());

	if (b_WriteAll)
	{
		External->SetPaletteWidth(GetPaletteWidth());
		External->SetPaletteHeight(GetPaletteHeight());
		External->GetPalette() = GetPalette();
	}
	else
	{
		std::vector<Sony_Pixel_16bpp> ExPalette(GetPaletteColorMax());

		if (!CopyPalette(ExPalette, iPalette))
		{
			return false;
		}

		if (!External->PastePalette(ExPalette, 0))
		{
			return false;
		}
	}

	External->SetCF(true);

	External->UpdatePaletteDataSize();

	return External->SaveTIM(File, pSource, true, false);
}

bool Sony_PlayStation_Texture::WritePixelsTIM(StdFile& File, std::uintmax_t pSource)
{
	if (!GetPixelSize())
	{
		Str.Message(L"PlayStation Texture Error: pixel data is empty");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not write pixel data at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

#ifdef _WINDOWS
	External->Str.hWnd = Str.hWnd;
#endif

	External->Create(GetDepth(), GetWidth(), GetHeight(), NULL);

	External->SetPixelX(GetPixelX());
	External->SetPixelY(GetPixelY());

	External->GetPixels() = m_Pixels;

	External->UpdatePixelDataSize();

	return External->SaveTIM(File, pSource, false, true);
}

bool Sony_PlayStation_Texture::OpenTIM(std::unique_ptr<Sony_PlayStation_Texture>& External, bool b_ReadPalette, bool b_ReadPixels)
{
	if (!b_ReadPalette && !b_ReadPixels)
	{
		Str.Message(L"PlayStation Texture Error: cannot ignore both palette and pixel data");
		return false;
	}

	if (!External->IsOpen() || !External->IsValid())
	{
		Str.Message(L"PlayStation Texture Error: external image is invalid");
		return false;
	}

	Close();

	m_Header.ID = 0x10;
	m_Header.Version = 0;
	m_Header.Reserved0 = 0;
	m_Header.PixelMode = External->GetDepth() == 4 ? 0 : External->GetDepth() == 8 ? 1 : External->GetDepth() == 16 ? 2 : External->GetDepth() == 24 ? 3 : 2;
	m_Header.ClutFlag = External->GetCF();
	m_Header.Reserved1 = 0;

	if (b_ReadPalette && External->GetCF())
	{
		m_PaletteHeader.X = External->GetPaletteX();
		m_PaletteHeader.Y = External->GetPaletteY();
		m_PaletteHeader.Width = External->GetPaletteWidth();
		m_PaletteHeader.Height = External->GetPaletteHeight();
		m_PaletteHeader.Size = External->GetPaletteDataSize();
		m_Palette = External->GetPalette();
	}

	if (b_ReadPixels)
	{
		m_PixelHeader.X = External->GetPixelX();
		m_PixelHeader.Y = External->GetPixelY();
		switch (m_Header.PixelMode)
		{
		case 0: m_PixelHeader.Width = External->GetWidth() / 4; break;
		case 1: m_PixelHeader.Width = External->GetWidth() / 2; break;
		case 2: m_PixelHeader.Width = External->GetWidth(); break;
		case 3: m_PixelHeader.Width = ((External->GetWidth() / 2) * 3); break;
		}
		m_PixelHeader.Height = External->GetHeight();
		m_PixelHeader.Size = External->GetPixelDataSize();
		m_Pixels = External->GetPixels();
	}

	b_Open = true;

	return b_Open;
}

bool Sony_PlayStation_Texture::OpenTIM(StdFile& File, std::uintmax_t pSource, bool b_ReadPalette, bool b_ReadPixels)
{
	if (!b_ReadPalette && !b_ReadPixels)
	{
		Str.Message(L"PlayStation Texture Error: cannot ignore both palette and pixel data");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not open texture image file at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	Sony_Texture_Header ExHeader{};
	Sony_Texture_Data ExPaletteHeader{};
	Sony_Texture_Data ExPixelHeader{};
	std::vector<std::uint8_t> ExPaletteData{};
	std::vector<std::uint8_t> ExPixelData{};

	File.Read(pSource, &ExHeader, sizeof(Sony_Texture_Header));

	if (ExHeader.ID != 0x10)
	{
		Str.Message(L"PlayStation Texture Error: invalid header id (%d) in \"%ws\"", ExHeader.ID, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ExHeader.Version != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid header version (%d) in \"%ws\"", ExHeader.Version, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ExHeader.Reserved0 != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid header reserved[0] (%d) in \"%ws\"", ExHeader.Reserved0, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if ((ExHeader.PixelMode != 0) && (ExHeader.PixelMode != 1) && (ExHeader.PixelMode != 2) && (ExHeader.PixelMode != 3) && (ExHeader.PixelMode != 4))
	{
		Str.Message(L"PlayStation Texture Error: invalid header mode (%d) in \"%ws\"", ExHeader.PixelMode, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if ((ExHeader.ClutFlag != 0) && (ExHeader.ClutFlag != 1))
	{
		Str.Message(L"PlayStation Texture Error: invalid header clut flag (%d) in \"%ws\"", ExHeader.ClutFlag, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ExHeader.Reserved1 != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid header reserved[1] (%d) in \"%ws\"", ExHeader.Reserved1, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ExHeader.PixelMode == 4)
	{
		Str.Message(L"PlayStation Texture Error: mixed-mode is unsupported");
		return false;
	}

	if (ExHeader.ClutFlag && (!ReadData(File, pSource + sizeof(Sony_Texture_Header), ExPaletteHeader, ExPaletteData)))
	{
		return false;
	}

	if (!ReadData(File, pSource + sizeof(Sony_Texture_Header) + ExPaletteHeader.Size, ExPixelHeader, ExPixelData))
	{
		return false;
	}

	Close();

	std::memcpy(&m_Header, &ExHeader, sizeof(Sony_Texture_Header));

	if (b_ReadPalette && ExHeader.ClutFlag)
	{
		m_Palette = PaletteFromUChar(ExPaletteData);
		std::memcpy(&m_PaletteHeader, &ExPaletteHeader, sizeof(Sony_Texture_Data));
		m_Header.ClutFlag = true;
	}
	else
	{
		m_PaletteHeader.X = 0;
		m_PaletteHeader.Y = 0;
		m_PaletteHeader.Width = 0;
		m_PaletteHeader.Height = 0;
		m_PaletteHeader.Size = 0;
		m_Header.ClutFlag = false;
	}

	if (b_ReadPixels && (ExPixelHeader.Width && ExPixelHeader.Height))
	{
		m_Header.PixelMode = ExHeader.PixelMode;
		m_Pixels.resize(ExPixelData.size());
		std::memcpy(&m_PixelHeader, &ExPixelHeader, sizeof(Sony_Texture_Data));
		std::memcpy(m_Pixels.data(), ExPixelData.data(), ExPixelData.size());
	}

	b_Open = true;

	return b_Open;
}

bool Sony_PlayStation_Texture::OpenCLT(StdFile& File, std::uintmax_t pSource, bool b_Add)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not open palette file at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	Sony_Texture_Header ClutHeader{};
	Sony_Texture_Data DataHeader{};
	std::vector<std::uint8_t> Data{};

	File.Read(pSource, &ClutHeader, sizeof(Sony_Texture_Header));

	if (ClutHeader.ID != 0x11)
	{
		Str.Message(L"PlayStation Texture Error: invalid clut file header id (%d) at 0x%llX in \"%ws\"", ClutHeader.ID, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ClutHeader.Version != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid clut file header version (%d) at 0x%llX in \"%ws\"", ClutHeader.Version, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ClutHeader.Reserved0 != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid clut file header reserved[0] (%d) at 0x%llX in \"%ws\"", ClutHeader.Reserved0, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ClutHeader.PixelMode != 2)
	{
		Str.Message(L"PlayStation Texture Error: invalid clut file header mode (%d) at 0x%llX in \"%ws\"", ClutHeader.PixelMode, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ClutHeader.ClutFlag != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid clut file header clut flag (%d) at 0x%llX in \"%ws\"", ClutHeader.ClutFlag, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (ClutHeader.Reserved1 != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid clut file header reserved[1] (%d) at 0x%llX in \"%ws\"", ClutHeader.Reserved1, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (!ReadData(File, pSource + sizeof(Sony_Texture_Header), DataHeader, Data))
	{
		return false;
	}

	if (Data.empty())
	{
		return false;
	}

	if (b_Add)
	{
		uint32_t DataSize = DataHeader.Size;
		if ((DataSize & sizeof(Sony_Texture_Data)) == sizeof(Sony_Texture_Data)) { DataSize ^= sizeof(Sony_Texture_Data); }

		if (DataSize % 2)
		{
			DataSize += 1;
		}

		uint16_t nColorSource = (uint16_t)(DataSize / sizeof(Sony_Pixel_16bpp));

		if (nColorSource > 256)
		{
			nColorSource = 256;
		}
		else if (nColorSource < 256)
		{
			nColorSource = 16;
		}

		AddPalette(ConvertPalette(PaletteFromUChar(Data), nColorSource, GetPaletteColorMax()));
	}
	else
	{
		m_PaletteHeader.X = DataHeader.X;
		m_PaletteHeader.Y = DataHeader.Y;

		m_PaletteHeader.Width = DataHeader.Width;
		m_PaletteHeader.Height = DataHeader.Height;

		m_PaletteHeader.Size = DataHeader.Size;

		m_Palette.resize(Data.size());

		std::memcpy(m_Palette.data(), Data.data(), Data.size());

		SetCF(true);
	}

	b_Open = true;

	return true;
}

bool Sony_PlayStation_Texture::OpenPXL(StdFile& File, std::uintmax_t pSource)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not open pixel file at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	Sony_Texture_Header PixelHeader{};
	Sony_Texture_Data DataHeader{};
	std::vector<std::uint8_t> Data{};

	File.Read(pSource, &PixelHeader, sizeof(Sony_Texture_Header));

	if (PixelHeader.ID != 0x12)
	{
		Str.Message(L"PlayStation Texture Error: invalid pixel file header id (%d) at 0x%llX in \"%ws\"", PixelHeader.ID, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (PixelHeader.Version != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid pixel file header version (%d) at 0x%llX in \"%ws\"", PixelHeader.Version, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (PixelHeader.Reserved0 != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid pixel file header reserved[0] (%d) at 0x%llX in \"%ws\"", PixelHeader.Reserved0, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if ((PixelHeader.PixelMode != 0) && (PixelHeader.PixelMode != 1) && (PixelHeader.PixelMode != 2) && (PixelHeader.PixelMode != 3) && (PixelHeader.PixelMode != 4))
	{
		Str.Message(L"PlayStation Texture Error: invalid pixel file header mode (%d) at 0x%llX in \"%ws\"", PixelHeader.PixelMode, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (PixelHeader.ClutFlag != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid pixel file header clut flag (%d) at 0x%llX in \"%ws\"", PixelHeader.ClutFlag, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (PixelHeader.Reserved1 != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid pixel file header reserved[1] (%d) at 0x%llX in \"%ws\"", PixelHeader.Reserved1, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (PixelHeader.PixelMode == 4)
	{
		Str.Message(L"PlayStation Texture Error: mixed-mode is unsupported");
		return false;
	}

	if (!ReadData(File, pSource + sizeof(Sony_Texture_Header), DataHeader, Data))
	{
		return false;
	}

	if (Data.empty())
	{
		return false;
	}

	m_Header.PixelMode = PixelHeader.PixelMode;

	m_PixelHeader.X = DataHeader.X;
	m_PixelHeader.Y = DataHeader.Y;

	m_PixelHeader.Width = DataHeader.Width;
	m_PixelHeader.Height = DataHeader.Height;

	m_PixelHeader.Size = DataHeader.Size;

	m_Pixels.resize(Data.size());

	std::memcpy(m_Pixels.data(), Data.data(), Data.size());

	b_Open = true;

	return true;
}

bool Sony_PlayStation_Texture::OpenBS(StdFile& File, std::uintmax_t pSource, std::uint16_t Width, std::uint16_t Height)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not open bitstream file at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	struct BitStream_Header
	{
		std::uint16_t Length;
		std::uint16_t ID;
		std::uint16_t Q_Scale;
		std::uint16_t Version;
	};

	BitStream_Header Header{};

	std::vector<std::uint8_t> Input(0x100000);

	if (File.Size() < 0x100000)
	{
		Input.resize((size_t)File.Size());
	}

	std::vector<std::uint8_t> Output(Width * Height * 4);

	File.Read(pSource, Input.data(), Input.size());

	std::memcpy(&Header, &Input.data()[0], sizeof(BitStream_Header));

	std::make_unique<Sony_PlayStation_Bitstream>()->mdec_decode(&Input.data()[8], Header.Version, Width, Height, Header.Q_Scale, &Output.data()[0]);

	std::unique_ptr<Sony_PlayStation_Texture> Texture = std::make_unique<Sony_PlayStation_Texture>(24, Width, Height, 0);

	size_t pOut = 0;
	for (std::uint16_t y = 0; y < Height; y++)
	{
		for (std::uint16_t x = 0; x < Width; x++, pOut += 4)
		{
			Texture->SetPixel(x, y, Sony_Pixel_24bpp{ Output[pOut + 0], Output[pOut + 1], Output[pOut + 2] });
		}
	}

	if (!OpenTIM(Texture, false, true))
	{
		return false;
	}

	b_Open = true;

	return true;
}

bool Sony_PlayStation_Texture::OpenPAL(StdFile& File, std::uintmax_t pSource, bool b_Add)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not open Microsoft RIFF palette at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return false;
		}
	}

	Microsoft_RIFF_Palette Header{};
	File.Read(pSource, &Header, sizeof(Microsoft_RIFF_Palette));

	std::string RIFF(reinterpret_cast<char*>(Header.RIFF));
	std::string PAL(reinterpret_cast<char*>(Header.PAL));
	std::string data(reinterpret_cast<char*>(Header.data));

	RIFF.resize(4);
	PAL.resize(4);
	data.resize(4);

	if (std::strcmp(RIFF.c_str(), "TIM2") != 0 || std::strcmp(PAL.c_str(), "PAL ") != 0 || std::strcmp(PAL.c_str(), "data") != 0)
	{
		Str.Message(L"PlayStation Texture Error: invalid Microsoft RIFF palette header at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (Header.Size != (Header.DataSize + 0x0A))
	{
		Str.Message(L"PlayStation Texture Error: invalid Microsoft RIFF palette file size (0x%08X) at 0x%llX in \"%ws\"", Header.Size, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (Header.Version != 0x0300)
	{
		Str.Message(L"PlayStation Texture Error: invalid Microsoft RIFF palette version (0x%04X) at 0x%llX in \"%ws\"", Header.Version, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	if (!Header.nColors)
	{
		Str.Message(L"PlayStation Texture Error: invalid Microsoft RIFF palette color count (0x%04X) at 0x%llX in \"%ws\"", Header.nColors, pSource, File.GetPath().filename().wstring().c_str());
		return false;
	}

	std::vector<Sony_Pixel_16bpp> ExPalette(Header.nColors);

	Pixel_32bpp Color{};

	for (std::size_t i = 0; i < Header.nColors; i++)
	{
		File.Read(pSource + 0x16 + (i * sizeof(Pixel_32bpp)), &Color, sizeof(Pixel_32bpp));

		ExPalette[i] = Create16bpp(Color.R, Color.G, Color.B, Color.A);
	}

	if (b_Add)
	{
		AddPalette(ExPalette);
	}
	else
	{
		if (ExPalette.size() % GetPaletteColorMax())
		{
			ExPalette.resize(ExPalette.size() + (GetPaletteColorMax() - (ExPalette.size() % GetPaletteColorMax())));
		}

		if (ExPalette.size() == 16)
		{
			m_PaletteHeader.Width = 16;
		}
		else
		{
			m_PaletteHeader.Width = 256;
		}

		m_PaletteHeader.Height = (uint16_t)(ExPalette.size() / GetPaletteColorMax());

		m_Palette.resize(ExPalette.size());

		std::copy(ExPalette.begin(), ExPalette.end(), m_Palette.begin());

		SetCF(true);

		UpdatePaletteDataSize();
	}

	b_Open = true;

	return true;
}

bool Sony_PlayStation_Texture::SaveTIM(StdFile& File, std::uintmax_t pSource, bool b_WritePalette, bool b_WritePixels)
{
	if (!b_WritePalette && !b_WritePixels)
	{
		Str.Message(L"PlayStation Texture Error: cannot ignore both palette and pixel data");
		return false;
	}

	if (!IsOpen())
	{
		Str.Message(L"PlayStation Texture Error: texture is not open");
		return false;
	}

	if (!IsValid())
	{
		Str.Message(L"PlayStation Texture Error: palette and pixel data is empty");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not create at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	File.Write(pSource, &m_Header, sizeof(Sony_Texture_Header));

	if (b_WritePalette && m_Header.ClutFlag && (m_PaletteHeader.Width && m_PaletteHeader.Height))
	{
		if (!WriteData(File, pSource + sizeof(Sony_Texture_Header), m_PaletteHeader, UCharFromPalette(m_Palette)))
		{
			return false;
		}
	}

	if (b_WritePixels && (m_PixelHeader.Width && m_PixelHeader.Height))
	{
		if (!WriteData(File, pSource + sizeof(Sony_Texture_Header) + GetPaletteDataSize(), m_PixelHeader, m_Pixels))
		{
			return false;
		}
	}
	else
	{
		Sony_Texture_Data PixelHeader{ sizeof(Sony_Texture_Data), 0, 0, 0, 0};
		File.Write(pSource + sizeof(Sony_Texture_Header) + GetPaletteDataSize(), &PixelHeader, sizeof(Sony_Texture_Data));
	}

	return true;
}

bool Sony_PlayStation_Texture::SaveCLT(StdFile& File, std::uintmax_t pSource)
{
	if (!GetPaletteCount())
	{
		Str.Message(L"PlayStation Texture Error: palette data is empty");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not create at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	Sony_Texture_Header ClutHeader{ 0x11, 0, 0, 2, 0, 0 };

	File.Write(pSource, &ClutHeader, sizeof(Sony_Texture_Header));

	return WriteData(File, pSource + sizeof(Sony_Texture_Header), m_PaletteHeader, UCharFromPalette(m_Palette));
}

bool Sony_PlayStation_Texture::SavePXL(StdFile& File, std::uintmax_t pSource)
{
	if (!GetPixelSize())
	{
		Str.Message(L"PlayStation Texture Error: pixel data is empty");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not create at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	Sony_Texture_Header PixelHeader{ 0x12, 0, 0, m_Header.PixelMode, 0, 0 };

	File.Write(pSource, &PixelHeader, sizeof(Sony_Texture_Header));

	return WriteData(File, pSource + sizeof(Sony_Texture_Header), m_PixelHeader, m_Pixels);
}

bool Sony_PlayStation_Texture::SavePAL(StdFile& File, std::uintmax_t pSource)
{
	if (!GetPaletteCount())
	{
		Str.Message(L"PlayStation Texture Error: palette data is empty");
		return false;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read_Ex, true, false))
		{
			if (!File.Open(File.GetPath(), FileAccessMode::Write_Ex, true, true))
			{
				Str.Message(L"PlayStation Texture Error: could not create at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
				return false;
			}
		}
	}

	Microsoft_RIFF_Palette Header{};

	std::memcpy(Header.RIFF, "RIFF", 4);
	Header.Size = (uint16_t)(m_Palette.size() * 4 + 4) + 0x0A;
	std::memcpy(Header.PAL, "PAL ", 4);
	std::memcpy(Header.data, "data", 4);
	Header.DataSize = (uint16_t)(m_Palette.size() * 4 + 4);
	Header.Version = 0x0300;
	Header.nColors = (uint16_t)m_Palette.size();

	File.Write(pSource, &Header, sizeof(Microsoft_RIFF_Palette));

	Pixel_32bpp Color{};

	for (std::size_t i = 0; i < m_Palette.size(); i++)
	{
		Color.R = m_Palette[i].Red();
		Color.G = m_Palette[i].Green();
		Color.B = m_Palette[i].Blue();
		m_Palette[i].A ? Color.A = 0x00 : Color.A = 0xFF;

		File.Write(pSource + 0x16 + (i * sizeof(Pixel_32bpp)), &Color, sizeof(Pixel_32bpp));
	}

	return true;
}

bool Sony_PlayStation_Texture::Create(Sony_Texture_Create_Ex ExInfo)
{
	std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

#ifdef _WINDOWS
	External->Str.hWnd = Str.hWnd;
#endif

	if (!External->Create(ExInfo.Depth, NULL, NULL, NULL))
	{
		return false;
	}

	switch (std::to_underlying(ExInfo.PixelType) & (TIM | PXL | RAW | BMP
#ifdef LIB_PNG
		| PNG
#endif
#ifdef LIB_JPEG
		| JPG
#endif
		))
	{
	case TIM:
		if (!External->ReadPixelsTIM(ExInfo.Pixel, ExInfo.pPixel))
		{
			return false;
		}
		break;
	case PXL:
		if (!External->OpenPXL(ExInfo.Pixel, ExInfo.pPixel))
		{
			return false;
		}
		break;
	case RAW:
		if (!External->ReadPixels(ExInfo.Pixel, ExInfo.pPixel, ExInfo.Width, ExInfo.Height))
		{
			return false;
		}
		break;
#ifdef LIB_JPEG
	case JPG:
#endif
#ifdef LIB_PNG
	case PNG:
#endif
	case BMP:
	{
		std::unique_ptr<Standard_Image> Image = std::make_unique<Standard_Image>();

#ifdef _WINDOWS
		Image->Str.hWnd = Str.hWnd;
#endif

		if (std::to_underlying(ExInfo.PixelType) & BMP)
		{
			if (!Image->OpenBMP(ExInfo.Pixel, ExInfo.pPixel))
			{
				return false;
			}
		}

#ifdef LIB_PNG
		if (std::to_underlying(ExInfo.PixelType) & PNG)
		{
			if (!Image->OpenPNG(ExInfo.Pixel, ExInfo.pPixel))
			{
				return false;
			}
		}
#endif

#ifdef LIB_JPEG
		if (std::to_underlying(ExInfo.PixelType) & JPG)
		{
			if (!Image->OpenJPEG(ExInfo.Pixel, ExInfo.pPixel))
			{
				return false;
			}
		}
#endif

		if (!External->ImportImage(Image))
		{
			return false;
		}
	}
	}

	External->GetPalette().clear();
	External->GetPalette().shrink_to_fit();

	switch (std::to_underlying(ExInfo.PaletteType) & (TIM | CLT | PAL | RAW))
	{
	case TIM:
		if (!External->ReadPaletteTIM(ExInfo.Palette, ExInfo.pPalette))
		{
			return false;
		}
		break;
	case CLT:
		if (!External->OpenCLT(ExInfo.Palette, ExInfo.pPalette))
		{
			return false;
		}
		break;
	case PAL:
		if (!External->OpenPAL(ExInfo.Palette, ExInfo.pPalette))
		{
			return false;
		}
		break;
	case RAW:
		if (!External->ReadPalette(ExInfo.Palette, ExInfo.pPalette, ExInfo.nPalette))
		{
			return false;
		}
		break;
	}

	if (!Create(!External->GetPixelCount() ? ExInfo.Depth : External->GetDepth(), ExInfo.Width, ExInfo.Height, NULL))
	{
		return false;
	}

	if (std::to_underlying(ExInfo.PaletteType) & (TIM | CLT | PAL | RAW))
	{
		if (!ReadPaletteTIM(External, false))
		{
			Close();
			return false;
		}
	}
	else if (ExInfo.nPalette)
	{
		for (auto i = 0; i < ExInfo.nPalette; i++)
		{
			AddPalette();
		}
	}

	if (External->GetPixelCount())
	{
		if (!ReadPixelsTIM(External))
		{
			Close();
			return false;
		}
	}

	return true;
}

bool Sony_PlayStation_Texture::Create(std::uint32_t Depth, std::uint16_t Width, std::uint16_t Height, std::uint16_t nPalette)
{
	if (Depth != 4 && Depth != 8 && Depth != 16 && Depth != 24)
	{
		Str.Message(L"PlayStation Texture Error: invalid depth (%d) when creating texture", Depth);
		return false;
	}

	if (Width > 1024)
	{
		Str.Message(L"PlayStation Texture Error: invalid width (%d) when creating texture, can't exceed 1024", Width);
		return false;
	}

	if (Height > 512)
	{
		Str.Message(L"PlayStation Texture Error: invalid height (%d) when creating texture, can't exceed 512", Height);
		return false;
	}

	if (Width)
	{
		if (Depth == 4 && (Width % 4))
		{
			Width = Width + (4 - (Width % 4));
		}

		if (Depth == 8 && (Width % 2))
		{
			Width = Width + (2 - (Width % 2));
		}
	}

	Close();

	m_Header.ID = 0x10;

	switch (Depth)
	{
	case 4:
		m_Header.PixelMode = 0;
		break;
	case 8:
		m_Header.PixelMode = 1;
		break;
	case 16:
		m_Header.PixelMode = 2;
		break;
	case 24:
		m_Header.PixelMode = 3;
		break;
	}

	if (nPalette)
	{
		m_PaletteHeader.Width = GetPaletteColorMax();
		m_PaletteHeader.Height = std::clamp(nPalette, (uint16_t)0, GetPaletteCountMax());

		std::vector<Sony_Pixel_16bpp> ExPalette = GrayScalePalette(Depth);

		for (std::uint16_t i = 0; i < nPalette; i++)
		{
			m_Palette.insert(m_Palette.end(), ExPalette.begin(), ExPalette.end());
		}

		m_Header.ClutFlag = true;

		UpdatePaletteDataSize();
	}

	switch (m_Header.PixelMode)
	{
	case 0:
		m_PixelHeader.Width = Width / 4;
		break;
	case 1:
		m_PixelHeader.Width = Width / 2;
		break;
	case 2:
		m_PixelHeader.Width = Width;
		break;
	case 3:
		m_PixelHeader.Width = (Width * 3) / 2;
		break;
	}

	m_PixelHeader.Height = Height;

	m_PixelHeader.Size = GetPixelDataSize();

	m_Pixels.resize(GetPixelSize());

	UpdateTransparencyFlags();

	b_Open = true;

	return b_Open;
}

void Sony_PlayStation_Texture::Search(StdFile& File, std::uintmax_t pSource,
	std::function<void(float, bool&)> ProgressCallback, std::function<void(std::filesystem::path, std::vector<std::pair<std::uintmax_t, std::uintmax_t>>&)> OnComplete)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message(L"PlayStation Texture Error: could not open at 0x%llX in \"%ws\"", pSource, File.GetPath().filename().wstring().c_str());
			return;
		}
	}
	std::uintmax_t FileSize = File.Size();

	if (pSource >= FileSize)
	{
		Str.Message(L"PlayStation Texture Error: invalid file size (0x%llX) and/or pointer (0x%llX)", FileSize, pSource);
		return;
	}

	std::vector<std::pair<std::uintmax_t, std::uintmax_t>> SearchResults;

	Sony_Texture_Header Header{};
	Sony_Texture_Data DataHeader{};

	std::uint32_t DataSize = 0;

	std::uintmax_t pRead = 0;

	bool b_Execute = false;

	while (pSource < FileSize)
	{
		ProgressCallback(((float)(pSource * 100) / (float)FileSize) / 100.0f, b_Execute);

		pRead = pSource;

		File.Read(pRead, &Header, sizeof(Sony_Texture_Header));

		if (Header.ID != 0x10) { pSource++; continue; }
		if (Header.Version != 0) { pSource++; continue; }
		if (Header.Reserved0 != 0) { pSource++; continue; }
		if ((Header.PixelMode != 0) && (Header.PixelMode != 1) && (Header.PixelMode != 2) && (Header.PixelMode != 3) && (Header.PixelMode != 4)) { pSource++; continue; }
		if ((Header.ClutFlag != 0) && (Header.ClutFlag != 1)) { pSource++; continue; }
		if (Header.Reserved1 != 0) { pSource++; continue; }

		if (Header.PixelMode == 4) { pSource++; continue; }

		pRead += sizeof(Sony_Texture_Header);

		if (Header.ClutFlag)
		{
			File.Read(pRead, &DataHeader, sizeof(Sony_Texture_Data));

			DataSize = DataHeader.Size;
			if ((DataSize & sizeof(Sony_Texture_Data)) == sizeof(Sony_Texture_Data)) { DataSize ^= sizeof(Sony_Texture_Data); }

			if ((!DataHeader.Size) ||
				(DataHeader.Size > (uint32_t)File.Size()) ||
				((DataHeader.Size + pRead) > File.Size()) ||
				(DataSize != (size_t)(DataHeader.Width * DataHeader.Height) * 2))
			{
				pSource++;
				continue;
			}

			pRead += ((size_t)(DataHeader.Width * DataHeader.Height) * 2) + sizeof(Sony_Texture_Data);
		}

		{
			File.Read(pRead, &DataHeader, sizeof(Sony_Texture_Data));

			DataSize = DataHeader.Size;
			if ((DataSize & sizeof(Sony_Texture_Data)) == sizeof(Sony_Texture_Data)) { DataSize ^= sizeof(Sony_Texture_Data); }

			if ((!DataHeader.Size) ||
				(DataHeader.Size > (uint32_t)File.Size()) ||
				((DataHeader.Size + pRead) > File.Size()) ||
				(DataSize != (size_t)(DataHeader.Width * DataHeader.Height) * 2))
			{
				pSource++;
				continue;
			}

			pRead += ((size_t)(DataHeader.Width * DataHeader.Height) * 2) + sizeof(Sony_Texture_Data);
		}

		if (!Header.ClutFlag && !DataSize)
		{
			pSource++;
			continue;
		}

		SearchResults.push_back(std::make_pair(pSource, (pRead - pSource)));

		pSource = pRead;

		ProgressCallback(((float)(pSource * 100) / (float)FileSize) / 100.0f, b_Execute);

		if (!b_Execute)
		{
			ProgressCallback(1.0f, b_Execute);
			break;
		}
	}

	ProgressCallback(((float)(pSource * 100) / (float)FileSize) / 100.0f, b_Execute);

	OnComplete(File.GetPath(), SearchResults);
}

void Sony_PlayStation_Texture::UpdateImagePalette(std::unique_ptr<Standard_Image>& Image, std::uint16_t iPalette)
{
	if (!GetCF() || !GetPaletteCount())
	{
		std::cout << "PlayStation Texture Warning: palette data is empty" << std::endl;
		return;
	}

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

	iPalette = std::clamp(iPalette, (uint16_t)0, GetPaletteMaxIndex());

	Sony_Pixel_16bpp Color{};

	Sony_Pixel_16bpp Transposed = GetPaletteColor(0, 0);

	Sony_Pixel_16bpp External = Create16bpp(m_TransparentColor, false);

	for (std::uint16_t i = 0; i < GetPaletteColorMax(); i++)
	{
		Color = GetPaletteColor(iPalette, i);

		if (b_TransparencySuperblack && !Color)
		{
			Image->SetPalette(i, 0);
			continue;
		}

		if (b_TransparencySuperimposed && (Color == Transposed))
		{
			Image->SetPalette(i, { Transposed.Blue(), Transposed.Green(), Transposed.Red(), (uint8_t)GetTransparency()});
			continue;
		}

		if (b_TransparencyExternal && (Color == External))
		{
			Image->SetPalette(i, { External.Blue(), External.Green(), External.Red(), (uint8_t)GetTransparency()});
			continue;
		}

		if (b_TransparencySTP && Color.A)
		{
			Image->SetPalette(i, { Color.Blue(), Color.Green(), Color.Red(), (uint8_t)GetTransparency()});
			continue;
		}

		Image->SetPalette(i, { Color.Blue(), Color.Green(), Color.Red(), (uint8_t)(Color.A ? 0x00 : 0xFF)});
	}

}

std::unique_ptr<Standard_Image> Sony_PlayStation_Texture::ExportImage(std::uint16_t iPalette)
{
	std::unique_ptr<Standard_Image> Image = std::make_unique<Standard_Image>();

#ifdef _WINDOWS
	Image->Str.hWnd = Str.hWnd;
#endif

	Image->Create(GetDepth(), GetWidth(), GetHeight());

	if (!b_Open)
	{
		Str.Message(L"PlayStation Texture Error: texture is not open");
		return Image;
	}

	if (m_Pixels.empty())
	{
		std::cout << "PlayStation Texture Warning: pixel data is empty" << std::endl;
		return Image;
	}

	std::uint16_t Depth = GetDepth();

	if (GetCF())
	{
		UpdateImagePalette(Image, iPalette);
	}

	std::uint16_t Width = GetWidth();

	std::uint16_t Height = GetHeight();

	Sony_Pixel_4bpp Color4{};

	Sony_Pixel_8bpp Color8{};

	Sony_Pixel_16bpp Color16{};

	Sony_Pixel_24bpp Color24{};

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

	auto GetAlpha24 = [this, GetTransparency, iPalette](Sony_Pixel_24bpp Color)
		{
			if (b_TransparencySuperblack && !Color.R && !Color.G && !Color.B)
			{
				return 0x00;
			}

			if (b_TransparencySuperimposed && !GetPalette().empty() &&
				(Color.R == GetPaletteColor(0, 0).Red()) &&
				(Color.G == GetPaletteColor(0, 0).Green()) &&
				(Color.B == GetPaletteColor(0, 0).Blue()))
			{
				return GetTransparency();
			}

			if (b_TransparencyExternal &&
				(Color.R == GetRValue(m_TransparentColor)) &&
				(Color.G == GetGValue(m_TransparentColor)) &&
				(Color.B == GetBValue(m_TransparentColor)))
			{
				return GetTransparency();
			}

			return 0xFF;
		};

	for (std::uint16_t Y = 0; Y < Height; Y++)
	{
		for (std::uint16_t X = 0; X < Width; X++)
		{
			switch (Depth)
			{
			case 4:
				Color4 = Get4bpp(X, Y);
				Image->SetPixel(X, Y, Color4.Pix0);
				Image->SetPixel(++X, Y, Color4.Pix1);
				break;
			case 8:
				Color8 = Get8bpp(X, Y);
				Image->SetPixel(X, Y, Color8.Pixel);
				break;
			case 16:
				Color16 = Get16bpp(X, Y);
				Image->SetPixel(X, Y, (Color16.A << 15) | (Color16.R << 10) | (Color16.G << 5) | Color16.B);
				break;
			case 24:
				Color24 = Get24bpp(X, Y);
				Image->SetPixel(X, Y, (GetAlpha24(Color24) << 24) | (Color24.R << 16) | (Color24.G << 8) | Color24.B);
				break;
			}
		}
	}

	return Image;
}

bool Sony_PlayStation_Texture::ImportImage(std::unique_ptr<Standard_Image>& Image)
{
#ifdef _WINDOWS
	Image->Str.hWnd = Str.hWnd;
#endif

	if (!Image->IsOpen())
	{
		Str.Message(L"PlayStation Texture Error: image is not open");
		return false;
	}

	if (!Image->IsValid())
	{
		Str.Message(L"PlayStation Texture Error: image palette and pixel data is empty");
		return false;
	}

	std::uint16_t Depth = (uint16_t)Image->GetDepth();

	std::uint16_t Width = (uint16_t)Image->GetWidth();

	std::uint16_t Height = (uint16_t)Image->GetHeight();

	Pixel_16bpp Color16{};

	Pixel_24bpp Color24{};

	Pixel_32bpp Color32{};

	Close();

	if (!Create(Depth == 32 ? 24 : Depth, Width, Height, (Depth == 4 || Depth == 8)))
	{
		return false;
	}

	if (Depth == 4 || Depth == 8)
	{
		for (size_t i = 0; i < Image->GetPalette().size() / sizeof(Pixel_32bpp); i++)
		{
			if (i >= GetPaletteColorMax())
			{
				break;
			}
			GetPalette()[i].R = (Image->GetPalette()[i].R >> 3);
			GetPalette()[i].G = (Image->GetPalette()[i].G >> 3);
			GetPalette()[i].B = (Image->GetPalette()[i].B >> 3);
			GetPalette()[i].A = Image->GetPalette()[i].A ? false : true;
		}
	}

	for (std::uint16_t Y = 0; Y < Height; Y++)
	{
		for (std::uint16_t X = 0; X < Width; X++)
		{
			switch (Depth)
			{
			case 4:
				SetPixel(X, Y, Sony_Pixel_4bpp{ (uint8_t)Image->GetPixel(X, Y), (uint8_t)Image->GetPixel(++X, Y) });
				break;
			case 8:
				SetPixel(X, Y, Sony_Pixel_8bpp{ (uint8_t)Image->GetPixel(X, Y) });
				break;
			case 16:
				Color16 = Image->Get16bpp(X, Y);
				SetPixel(X, Y, Sony_Pixel_16bpp{ Color16.R, Color16.G, Color16.B, Color16.A });
				break;
			case 24:
				Color24 = Image->Get24bpp(X, Y);
				SetPixel(X, Y, Sony_Pixel_24bpp{ Color24.R, Color24.G, Color24.B });
				break;
			case 32:
				Color32 = Image->Get32bpp(X, Y);
				SetPixel(X, Y, Sony_Pixel_24bpp{ Color32.R, Color32.G, Color32.B });
				break;
			}
		}
	}

	return true;
}