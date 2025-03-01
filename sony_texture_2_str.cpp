/*
*
*	Megan Grass
*	February 26, 2025
*
*/

#include "sony_texture_2.h"

String Sony_PlayStation_Texture_2::PixelModeStr(Sony_Pixel_Mode Input)
{
	switch (Input)
	{
	case Sony_Pixel_Mode::RGBA32: return "RGBA32";
	case Sony_Pixel_Mode::RGBA24: return "RGBA24";
	case Sony_Pixel_Mode::RGBA16: return "RGBA16";
	case Sony_Pixel_Mode::RGBA16S: return "RGBA16S";
	case Sony_Pixel_Mode::RGB: return "RGB";
	case Sony_Pixel_Mode::CLUT8: return "CLUT8";
	case Sony_Pixel_Mode::CLUT4: return "CLUT4";
	case Sony_Pixel_Mode::CLUT8H: return "CLUT8H";
	case Sony_Pixel_Mode::CLUT4HL: return "CLUT4HL";
	case Sony_Pixel_Mode::CLUT4HH: return "CLUT4HH";
	case Sony_Pixel_Mode::ZBUFF32: return "ZBUFF32";
	case Sony_Pixel_Mode::ZBUFF24: return "ZBUFF24";
	case Sony_Pixel_Mode::ZBUFF16: return "ZBUFF16";
	case Sony_Pixel_Mode::ZBUFF16S: return "ZBUFF16S";
	case Sony_Pixel_Mode::null: return "no data";
	}
	return Str.FormatCStyle("unknown 0x%02X", std::to_underlying(Input));
}

String Sony_PlayStation_Texture_2::PaletteAlignStr(Sony_Pixel_Mode PixelMode, uint8_t Input)
{
	if (PixelMode != Sony_Pixel_Mode::CLUT4 && PixelMode != Sony_Pixel_Mode::CLUT8)
	{
		return Str.FormatCStyle("no data");
	}
	switch (Input)
	{
	case 0: return Str.FormatCStyle("Mode 1 (%s)", PixelMode == Sony_Pixel_Mode::CLUT4 ? "8x2 grid" : "16x16 grid");
	case 1: return Str.FormatCStyle("Mode 2 (%s)", PixelMode == Sony_Pixel_Mode::CLUT4 ? "16x1 line" : "256x1 line");
	default: return Str.FormatCStyle("unsupported 0x%02X", Input);
	}
	return Str.FormatCStyle("unsupported 0x%02X", Input);
}

String Sony_PlayStation_Texture_2::PixelFunctionStr(uint8_t Input)
{
	switch (Input)
	{
	case 0: return "Modulate";
	case 1: return "Decal";
	case 2: return "Translucency";
	case 3: return "Opaque";
	default: return Str.FormatCStyle("unsupported 0x%02X", Input);
	}
}

String Sony_PlayStation_Texture_2::FilterMagStr(uint8_t Input)
{
	switch (Input)
	{
	case 0: return "Nearest";
	case 1: return "Linear";
	default: return Str.FormatCStyle("unsupported 0x%02X", Input);
	}
}

String Sony_PlayStation_Texture_2::FilterMinStr(uint8_t Input)
{
	switch (Input)
	{
	case 0: return "Nearest";
	case 1: return "Linear";
	case 2: return "Nearest Mip Nearest";
	case 3: return "Nearest Mip Linear";
	case 4: return "Linear Mip Nearest";
	case 5: return "Linear Mip Linear";
	default: return Str.FormatCStyle("unsupported 0x%02X", Input);
	}
}

String Sony_PlayStation_Texture_2::MipTypeStr(uint8_t Input)
{
	switch (Input)
	{
	case 0: return "MIPTBP1/MIPTBP2";
	case 1: return "Auto TBP1-TBP3";
	default: return Str.FormatCStyle("unsupported 0x%02X", Input);
	}
}

String Sony_PlayStation_Texture_2::AlphaExpansionStr(uint8_t Input)
{
	switch (Input)
	{
	case 0: return "Superblack Transparency OFF";
	case 1: return "Superblack Transparency ON";
	default: return Str.FormatCStyle("unsupported 0x%02X", Input);
	}
}

String Sony_PlayStation_Texture_2::HeaderStr(void)
{
	std::string Identifier(reinterpret_cast<char*>(m_Header.Type));

	Identifier.resize(4);

	String Output = Str.FormatCStyle("PlayStation 2 Texture:\n");

	Output += Str.FormatCStyle("\tID: %s\n", Identifier.c_str());
	Output += Str.FormatCStyle("\tVersion: %02X\n", m_Header.Version);
	Output += Str.FormatCStyle("\tAlignment: %d bytes\n", m_Header.Align == 1 ? 128 : 16);
	Output += Str.FormatCStyle("\tTexture Count: %d\n", m_Header.DataCount);

	return Output;
}

String Sony_PlayStation_Texture_2::DataStr(std::uint16_t iTexture)
{
	if (m_Texture.empty()) { return Str.FormatCStyle("PlayStation 2 Texture Data:\n\tno data\n"); }

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	String Output = Str.FormatCStyle("PlayStation 2 Texture Data:\n");

	Output += Str.FormatCStyle("\tSize: 0x%X bytes\n", m_Texture[iTexture]->Data.Size);
	Output += Str.FormatCStyle("\tPalette Size: 0x%X bytes\n", m_Texture[iTexture]->Data.PaletteSize);
	Output += Str.FormatCStyle("\tPixel Size: 0x%X bytes\n", m_Texture[iTexture]->Data.PixelSize);
	Output += Str.FormatCStyle("\tHeader Size: 0x%X bytes\n", m_Texture[iTexture]->Data.HeaderSize);
	Output += Str.FormatCStyle("\tPalette Color Count: %d\n", m_Texture[iTexture]->Data.ColorCount);
	Output += Str.FormatCStyle("\tData Format: %d\n", m_Texture[iTexture]->Data.DataFormat);

	if (m_Texture[iTexture]->Data.MipMap) { Output += Str.FormatCStyle("\tMip Map: Level %d\n", m_Texture[iTexture]->Data.MipMap - 1); }
	else { Output += Str.FormatCStyle("\tMip Map: no data\n"); }

	return Output;
}

String Sony_PlayStation_Texture_2::PaletteStr(std::uint16_t iTexture)
{
	if (m_Texture.empty()) { return Str.FormatCStyle("PlayStation 2 Texture Palette:\n\tno data\n"); }

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	String Output = Str.FormatCStyle("PlayStation 2 Texture Palette:\n");

	Output += Str.FormatCStyle("\tPixel Mode: %s\n", PixelModeStr(GetPixelModeVer2(m_Texture[iTexture]->Data.PaletteAttr.PixelMode)).c_str());
	Output += Str.FormatCStyle("\tCompound: %s\n", m_Texture[iTexture]->Data.PaletteAttr.Compound ? "true" : "false");
	Output += Str.FormatCStyle("\tAlign: %s\n", PaletteAlignStr((Sony_Pixel_Mode)m_Texture[iTexture]->Data.Attr.PixelMode, m_Texture[iTexture]->Data.PaletteAttr.Align).c_str());
	Output += Str.FormatCStyle("\tWidth: %d\n", m_Texture[iTexture]->Data.PaletteAttrEx.Width);
	Output += Str.FormatCStyle("\tU: %d\n", m_Texture[iTexture]->Data.PaletteAttrEx.U);
	Output += Str.FormatCStyle("\tV: %d\n", m_Texture[iTexture]->Data.PaletteAttrEx.V);

	return Output;
}

String Sony_PlayStation_Texture_2::PixelStr(std::uint16_t iTexture)
{
	if (m_Texture.empty()) { return Str.FormatCStyle("PlayStation 2 Texture Pixels:\n\tno data\n"); }

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	String Output = Str.FormatCStyle("PlayStation 2 Texture Pixels:\n");

	Output += Str.FormatCStyle("\tPixel Mode: %s\n", PixelModeStr(GetPixelModeVer2(m_Texture[iTexture]->Data.PixelMode)).c_str());
	Output += Str.FormatCStyle("\tWidth: %d\n", m_Texture[iTexture]->Data.Width);
	Output += Str.FormatCStyle("\tHeight: %d\n", m_Texture[iTexture]->Data.Height);

	return Output;
}

String Sony_PlayStation_Texture_2::AttrStr(std::uint16_t iTexture)
{
	if (m_Texture.empty()) { return Str.FormatCStyle("PlayStation 2 Texture Data:\n\tno data\n"); }

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	String Output = Str.FormatCStyle("PlayStation 2 Texture Attributes:\n");

	Output += Str.FormatCStyle("\tPixel Ptr: 0x%X\n", m_Texture[iTexture]->Data.Attr.PixelPtr);
	Output += Str.FormatCStyle("\tTexel Width: %d\n", m_Texture[iTexture]->Data.Attr.TexelWidth);
	Output += Str.FormatCStyle("\tPixel Mode: %s\n", PixelModeStr((Sony_Pixel_Mode)m_Texture[iTexture]->Data.Attr.PixelMode).c_str());
	Output += Str.FormatCStyle("\tPixel Width: %d\n", m_Texture[iTexture]->Data.Attr.Width);
	Output += Str.FormatCStyle("\tPixel Height: %d\n", m_Texture[iTexture]->Data.Attr.Height);
	Output += Str.FormatCStyle("\tColor Component: %s\n", m_Texture[iTexture]->Data.Attr.Comp ? "RGB" : "RGBA");
	Output += Str.FormatCStyle("\tPixel Function: %s\n", PixelFunctionStr(m_Texture[iTexture]->Data.Attr.Func).c_str());
	Output += Str.FormatCStyle("\tPalette Ptr: 0x%X\n", m_Texture[iTexture]->Data.Attr.PalettePtr);
	Output += Str.FormatCStyle("\tPalette Pixel Mode: %s\n",
		m_Texture[iTexture]->Data.Attr.PixelMode == std::to_underlying(Sony_Pixel_Mode::CLUT4) || m_Texture[iTexture]->Data.Attr.PixelMode == std::to_underlying(Sony_Pixel_Mode::CLUT8) ?
		PixelModeStr((Sony_Pixel_Mode)m_Texture[iTexture]->Data.Attr.PalettePixelMode).c_str() : "no data");
	Output += Str.FormatCStyle("\tPalette Align: %s\n", PaletteAlignStr((Sony_Pixel_Mode)m_Texture[iTexture]->Data.Attr.PixelMode, m_Texture[iTexture]->Data.Attr.PaletteAlign).c_str());
	Output += Str.FormatCStyle("\tPalette Entry Ptr: 0x%X\n", m_Texture[iTexture]->Data.Attr.PaletteBlockPtr);
	Output += Str.FormatCStyle("\tPalette Load Mode: %d\n", m_Texture[iTexture]->Data.Attr.PaletteLoad);

	return Output;
}

String Sony_PlayStation_Texture_2::SamplingStr(std::uint16_t iTexture)
{
	if (m_Texture.empty()) { return Str.FormatCStyle("PlayStation 2 Texture Sampling:\n\tno data\n"); }

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	String Output = Str.FormatCStyle("PlayStation 2 Texture Sampling:\n");

	Output += Str.FormatCStyle("\tLOD Function: %d\n", m_Texture[iTexture]->Data.Sampling.FuncLOD);
	Output += Str.FormatCStyle("\tMip Map: Level %d\n", m_Texture[iTexture]->Data.Sampling.MipLevel);
	Output += Str.FormatCStyle("\tFilter Mag: %s\n", FilterMagStr(m_Texture[iTexture]->Data.Sampling.FilterMag).c_str());
	Output += Str.FormatCStyle("\tFilter Min: %s\n", FilterMinStr(m_Texture[iTexture]->Data.Sampling.FilterMin).c_str());
	Output += Str.FormatCStyle("\tMip Type: %s\n", MipTypeStr(m_Texture[iTexture]->Data.Sampling.MipPtrType).c_str());
	Output += Str.FormatCStyle("\tLOD Weight: %d\n", m_Texture[iTexture]->Data.Sampling.Weight);
	Output += Str.FormatCStyle("\tLOD Position: %d\n", m_Texture[iTexture]->Data.Sampling.Position);

	return Output;
}

String Sony_PlayStation_Texture_2::AlphaStr(std::uint16_t iTexture)
{
	if (m_Texture.empty()) { return Str.FormatCStyle("PlayStation 2 Texture Alpha:\n\tno data\n"); }

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	String Output = Str.FormatCStyle("PlayStation 2 Texture Alpha:\n");

	Output += Str.FormatCStyle("\tAlpha 0: 0x%02X\n", m_Texture[iTexture]->Data.Alpha.Value0);
	Output += Str.FormatCStyle("\tFunction: %s\n", m_Texture[iTexture]->Data.Alpha.Func ? "true" : "false");	//AlphaExpansionStr(m_Texture[iTexture]->Data.Alpha.Expansion).c_str());
	Output += Str.FormatCStyle("\tAlpha 1: 0x%02X\n", m_Texture[iTexture]->Data.Alpha.Value1);
	Output += Str.FormatCStyle("\tBlend: %s\n", m_Texture[iTexture]->Data.Alpha.Blend ? "true" : "false");
	Output += Str.FormatCStyle("\tCorrection: %s\n", m_Texture[iTexture]->Data.Alpha.Correction ? "true" : "false");

	return Output;
}

String Sony_PlayStation_Texture_2::ExDataStr(std::uint16_t iTexture)
{
	if (m_Texture.empty()) { return Str.FormatCStyle("PlayStation 2 Texture Extended Data:\n\tno data\n"); }

	iTexture = std::clamp(iTexture, (uint16_t)0, (uint16_t)(m_Texture.size() - 1));

	std::uintmax_t BlockSize = m_Header.Align ? 128 : 16;

	std::string Identifier(reinterpret_cast<char*>(m_Texture[iTexture]->ExData.first.Type));

	Identifier.resize(3);

	String Output = Str.FormatCStyle("PlayStation 2 Texture Extended Data:\n");

	if (!std::strcmp(Identifier.c_str(), "eXt\0"))
	{
		Output += Str.FormatCStyle("\tID: %s\n", Identifier.c_str());
		Output += Str.FormatCStyle("\tSize: 0x%X bytes\n", m_Texture[iTexture]->ExData.first.Size);
		Output += Str.FormatCStyle("\tData Size: 0x%X bytes\n", m_Texture[iTexture]->ExData.first.DataSize);

		if ((m_Texture[iTexture]->ExData.first.DataSize + sizeof(Sony_Texture_2_ExData)) != m_Texture[iTexture]->ExData.first.Size)
		{
			Output += Str.FormatCStyle("\tComment: %s\n", reinterpret_cast<char*>(m_Texture[iTexture]->ExData.second.data()));
		}
	}

	return Output;
}