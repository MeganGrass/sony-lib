/*
*
*	Megan Grass
*	March 07, 2024
*
*
*	TODO: 
* 
*		Function to update VH from v6 to v7
* 
*		Header needs to be updated when saving VH
*
*/


#include "sony_soundbank.h"


/*
	Open VH
*/
std::uintmax_t Sony_PlayStation_Soundbank::OpenVH(StdFile& File, std::uintmax_t _Ptr)
{
	if (b_VhOpen) { CloseVH(); }

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message("PlayStation Soundbank: Error, could not open VH at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}


	// Header
	File.Read(_Ptr, &Header, sizeof(Sony_Soundbank_Header));

	String Magic = { Header.Magic[0], Header.Magic[1], Header.Magic[2], Header.Magic[3] };

	if (Magic != "pBAV")
	{
		Str.Message("PlayStation Soundbank: Error, invalid VH at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
		return _Ptr;
	}

	_Ptr += sizeof(Sony_Soundbank_Header);


	// Program
	Program.resize(128);

	File.Read(_Ptr, Program.data(), (sizeof(Sony_Soundbank_Program) * 128));

	_Ptr += (sizeof(Sony_Soundbank_Program) * 128);


	// Tone
	Tone.resize(Header.nProgram);

	for (std::size_t i = 0; i < Header.nProgram; i++, _Ptr += (sizeof(Sony_Soundbank_Tone) * 16))
	{
		Tone[i].resize(16);

		File.Read(_Ptr, Tone[i].data(), (sizeof(Sony_Soundbank_Tone) * 16));
	}


	// Sample Pointer
	pSample.resize(256);

	File.Read(_Ptr, pSample.data(), (sizeof(uint16_t) * 256));

	_Ptr += (sizeof(uint16_t) * 256);

	b_VhOpen = true;


	// Complete
	return _Ptr;
}


/*
	Open VH
*/
bool Sony_PlayStation_Soundbank::OpenVH(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	OpenVH(m_File, _Ptr);

	return b_VhOpen;
}


/*
	Open VB
*/
std::uintmax_t Sony_PlayStation_Soundbank::OpenVB(StdFile& File, std::uintmax_t _Ptr)
{
	if (b_VbOpen) { CloseVB(); }

	if (!b_VhOpen)
	{
		Str.Message("PlayStation Soundbank: Error, header is not open");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message("PlayStation Soundbank: Error, could not open VB at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	std::vector<std::size_t> Pointer(Header.nVag + 1);

	for (std::uint16_t i = 0; i < Header.nVag + 1; i++)
	{
		std::size_t Size = (static_cast<std::size_t>(pSample[i]) << 3);

		_Ptr += Size;

		Pointer[i] = _Ptr;
	}

	Sample.resize(Header.nVag);

	for (std::size_t i = 0; i < Header.nVag; i++)
	{
		Sample[i].resize(Pointer[i + 1] - Pointer[i]);

		File.Read(Pointer[i], Sample[i].data(), Pointer[i + 1] - Pointer[i]);
	}

	b_VbOpen = true;

	return _Ptr;
}


/*
	Open VB
*/
bool Sony_PlayStation_Soundbank::OpenVB(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	OpenVB(m_File, _Ptr);

	return b_VbOpen;
}


/*
	Open VAB
*/
std::uintmax_t Sony_PlayStation_Soundbank::OpenVAB(StdFile& File, std::uintmax_t _Ptr)
{
	std::uintmax_t OldPtr = _Ptr;

	_Ptr = OpenVH(File, _Ptr);

	if (!b_VhOpen)
	{
		return OldPtr;
	}

	_Ptr = OpenVB(File, _Ptr);

	if (!b_VbOpen)
	{
		return OldPtr;
	}

	return _Ptr;
}


/*
	Open VAB
*/
bool Sony_PlayStation_Soundbank::OpenVAB(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	OpenVAB(m_File, _Ptr);

	return b_VhOpen && b_VbOpen;
}


/*
	Save VH
*/
std::uintmax_t Sony_PlayStation_Soundbank::SaveVH(StdFile& File, std::uintmax_t _Ptr)
{
	if (!b_VhOpen)
	{
		Str.Message("PlayStation Soundbank: Error, header is not open");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Write, true, false))
		{
			Str.Message("PlayStation Soundbank: Error, could not create VH at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	File.Write(_Ptr, &Header, sizeof(Sony_Soundbank_Header));

	_Ptr += sizeof(Sony_Soundbank_Header);

	File.Write(_Ptr, Program.data(), (sizeof(Sony_Soundbank_Program) * 128));

	_Ptr += (sizeof(Sony_Soundbank_Program) * 128);

	for (std::size_t i = 0; i < Tone.size(); i++, _Ptr += (sizeof(Sony_Soundbank_Tone) * 16))
	{
		File.Write(_Ptr, Tone[i].data(), (sizeof(Sony_Soundbank_Tone) * 16));
	}

	for (std::size_t i = 1, x = 0; i < Sample.size(); i++, x++)
	{
		std::uint16_t Size = static_cast<std::uint16_t>(Sample[x].size() >> 3);
		pSample[i] = Size;
	}

	for (std::size_t i = 0; i < pSample.size(); i++, _Ptr += sizeof(std::uint16_t))
	{
		File.Write(_Ptr, &pSample[i], sizeof(std::uint16_t));
	}

	return _Ptr;
}


/*
	Save VH
*/
bool Sony_PlayStation_Soundbank::SaveVH(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = SaveVH(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Save VB
*/
std::uintmax_t Sony_PlayStation_Soundbank::SaveVB(StdFile& File, std::uintmax_t _Ptr)
{
	if (!b_VbOpen)
	{
		Str.Message("PlayStation Soundbank: Error, samples are not open");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Write, true, false))
		{
			Str.Message("PlayStation Soundbank: Error, could not create VB at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	for (std::size_t i = 0; i < Sample.size(); i++)
	{
		File.Write(_Ptr, Sample[i].data(), Sample[i].size());
		_Ptr += Sample[i].size();
	}

	return _Ptr;
}


/*
	Save VB
*/
bool Sony_PlayStation_Soundbank::SaveVB(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = SaveVB(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Save VAB
*/
std::uintmax_t Sony_PlayStation_Soundbank::SaveVAB(StdFile& File, std::uintmax_t _Ptr)
{
	std::uintmax_t OldPtr = _Ptr;

	_Ptr = SaveVH(File, _Ptr);

	if (OldPtr == _Ptr)
	{
		return OldPtr;
	}

	_Ptr = SaveVB(File, _Ptr);

	if (OldPtr == _Ptr)
	{
		return OldPtr;
	}

	return _Ptr;
}


/*
	Save VAB
*/
bool Sony_PlayStation_Soundbank::SaveVAB(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = SaveVAB(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Close VH
*/
void Sony_PlayStation_Soundbank::CloseVH(void)
{
	b_VhOpen = false;
	std::memset(&Header, 0, sizeof(Sony_Soundbank_Header));
	Program.clear();
	Program.shrink_to_fit();
	Tone.clear();
	Tone.shrink_to_fit();
	pSample.clear();
	pSample.shrink_to_fit();
}


/*
	Close VB
*/
void Sony_PlayStation_Soundbank::CloseVB(void)
{
	b_VbOpen = false;
	Sample.clear();
	Sample.shrink_to_fit();
}


/*
	Close VAB
*/
void Sony_PlayStation_Soundbank::CloseVAB(void)
{
	CloseVH();
	CloseVB();
}


/*
	VH Size
*/
std::uintmax_t Sony_PlayStation_Soundbank::VhSize(void) const
{
	if (!b_VhOpen) { return 0; }

	std::uintmax_t FileSize = sizeof(Sony_Soundbank_Header);

	FileSize += (sizeof(Sony_Soundbank_Program) * 128) * Program.size();

	FileSize += (sizeof(Sony_Soundbank_Tone) * 16) * Tone.size();

	FileSize += (sizeof(std::uint16_t) * 256);

	return FileSize;
}


/*
	VB Size
*/
std::uintmax_t Sony_PlayStation_Soundbank::VbSize(void) const
{
	if (!b_VbOpen) { return 0; }

	std::uintmax_t FileSize = 0;

	for (std::size_t i = 0; i < Sample.size(); i++)
	{
		FileSize += Sample[i].size();
	}

	return FileSize;
}


/*
	VAB Size
*/
std::uintmax_t Sony_PlayStation_Soundbank::VabSize(void) const
{
	return VhSize() + VbSize();
}