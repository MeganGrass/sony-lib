/*
*
*	Megan Grass
*	March 07, 2024
*
*
*	TODO: 
*
*/


#include "sony_sequence.h"


/*
	Detect sequence type
*/
Sony_Sequence_Type Sony_PlayStation_Sequence::GetType(StdFile& File, std::uintmax_t _Ptr)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message("PlayStation Sequence: Error, could not open at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return Sony_Sequence_Type::Unknown;
		}
	}

	Sony_Sequence_SEQ SeqHeader{};

	File.Read(_Ptr, &SeqHeader, sizeof(Sony_Sequence_SEQ));

	String Magic = { SeqHeader.Magic[0], SeqHeader.Magic[1], SeqHeader.Magic[2], SeqHeader.Magic[3] };

	if (Magic != "pQES")
	{
		return Sony_Sequence_Type::Unknown;
	}

	if (SeqHeader.Version == 0)
	{
		return Sony_Sequence_Type::SEP;
	}

	if (SeqHeader.Version == 0x01000000)
	{
		return Sony_Sequence_Type::SEQ;
	}

	return Sony_Sequence_Type::Unknown;
}


/*
	Detect sequence type
*/
Sony_Sequence_Type Sony_PlayStation_Sequence::GetType(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	return GetType(m_File, _Ptr);
}


/*
	Read MIDI
*/
std::uintmax_t Sony_PlayStation_Sequence::readMIDI(StdFile& File, std::uintmax_t _Ptr)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message("PlayStation Sequence: Error, could not open at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return -1;
		}
	}

	std::uint32_t EndOfSeq = 0x002FFF00;

	std::uintmax_t pEndOfSeq = File.Find(&EndOfSeq, sizeof(std::uint32_t), _Ptr);

	if (pEndOfSeq == -1)
	{
		Str.Message("PlayStation Sequence: Error, could not find end of sequence");
		return -1;
	}

	pEndOfSeq += sizeof(std::uint32_t);

	Midi.resize(Midi.size() + 1);

	Midi[Midi.size() - 1].resize(pEndOfSeq - _Ptr);

	File.Read(_Ptr, Midi[Midi.size() - 1].data(), Midi[Midi.size() - 1].size());

	return Midi[Midi.size() - 1].size();
}


/*
	Open SEQ
*/
std::uintmax_t Sony_PlayStation_Sequence::OpenSEQ(StdFile& File, std::uintmax_t _Ptr)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message("PlayStation Sequence: Error, could not open at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	Sony_Sequence_SEQ SeqHeader{};

	File.Read(_Ptr, &SeqHeader, sizeof(Sony_Sequence_SEQ));

	String Magic = { SeqHeader.Magic[0], SeqHeader.Magic[1], SeqHeader.Magic[2], SeqHeader.Magic[3] };

	if (Magic != "pQES")
	{
		Str.Message("PlayStation Sequence: Error, invalid SEQ type at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
		return _Ptr;
	}

	if (SeqHeader.Version != 0x01000000)
	{
		Str.Message("PlayStation Sequence: Error, invalid SEQ version at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
		return _Ptr;
	}

	std::uintmax_t SizeOfMidiData = readMIDI(File, _Ptr + sizeof(Sony_Sequence_SEQ));

	if (SizeOfMidiData == -1)
	{
		return _Ptr;
	}

	Header.push_back(SeqHeader.Header);

	_Ptr += sizeof(Sony_Sequence_SEQ) + SizeOfMidiData;

	return _Ptr;
}


/*
	Open SEQ
*/
bool Sony_PlayStation_Sequence::OpenSEQ(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = OpenSEQ(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Open SEP
*/
std::uintmax_t Sony_PlayStation_Sequence::OpenSEP(StdFile& File, std::uintmax_t _Ptr)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message("PlayStation Sequence: Error, could not open at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	Sony_Sequence_SEP SepHeader{};

	File.Read(_Ptr, &SepHeader, sizeof(Sony_Sequence_SEQ));

	String Magic = { SepHeader.Magic[0], SepHeader.Magic[1], SepHeader.Magic[2], SepHeader.Magic[3] };

	if (Magic != "pQES")
	{
		Str.Message("PlayStation Sequence: Error, invalid SEP type at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
		return _Ptr;
	}

	if (SepHeader.Version != 0)
	{
		Str.Message("PlayStation Sequence: Error, invalid SEP version at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
		return _Ptr;
	}

	std::uintmax_t Pointer = _Ptr + sizeof(Sony_Sequence_SEP);

	Sony_Sequence_SEP_Data SepData{};

	std::uint16_t Seq_id = 0;

	while ((Pointer < File.Size()) && (!File.eof()))
	{
		File.Read(Pointer, &SepData, sizeof(Sony_Sequence_SEP_Data));

		if (std::byteswap<std::uint16_t>(SepData.Seq_id) != Seq_id)
		{
			break;
		}

		Pointer += sizeof(Sony_Sequence_SEP_Data);

		std::uintmax_t SizeOfMidiData = readMIDI(File, Pointer);

		if (SizeOfMidiData == -1)
		{
			break;
		}

		Header.push_back(SepData.Header);

		Pointer += SizeOfMidiData;

		Seq_id++;
	}

	if (Pointer != _Ptr)
	{
		_Ptr = Pointer;
	}

	return _Ptr;
}


/*
	Open SEP
*/
bool Sony_PlayStation_Sequence::OpenSEP(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = OpenSEP(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Open
*/
std::uintmax_t Sony_PlayStation_Sequence::Open(StdFile& File, std::uintmax_t _Ptr)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Read, true, false))
		{
			Str.Message("PlayStation Sequence: Error, could not open at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	Sony_Sequence_Type Type = GetType(File, _Ptr);

	if (Type == Sony_Sequence_Type::Unknown)
	{
		Str.Message("PlayStation Sequence: Error, invalid sequence type at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
		return _Ptr;
	}
	else if (Type == Sony_Sequence_Type::SEQ)
	{
		return OpenSEQ(File, _Ptr);
	}
	else if (Type == Sony_Sequence_Type::SEP)
	{
		return OpenSEP(File, _Ptr);
	}

	return _Ptr;
}


/*
	Open
*/
bool Sony_PlayStation_Sequence::Open(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = Open(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Save SEQ
*/
std::uintmax_t Sony_PlayStation_Sequence::SaveSEQ(StdFile& File, std::size_t Midi_no, std::uintmax_t _Ptr)
{
	if ((Midi_no + 1) > Midi.size())
	{
		Str.Message("PlayStation Sequence: Error, invalid MIDI ID");
		return _Ptr;
	}

	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Write, true, false))
		{
			Str.Message("PlayStation Sequence: Error, could not create SEQ at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	Sony_Sequence_SEQ SeqHeader{};

	SeqHeader.Magic[0] = 0x70;
	SeqHeader.Magic[1] = 0x51;
	SeqHeader.Magic[2] = 0x45;
	SeqHeader.Magic[3] = 0x53;

	SeqHeader.Version = 0x01000000;

	std::memcpy(&SeqHeader.Header, &Header[Midi_no], sizeof(Sony_Sequence_Header));

	File.Write(_Ptr, &SeqHeader, sizeof(Sony_Sequence_SEQ));

	File.Write(_Ptr + sizeof(Sony_Sequence_SEQ), Midi[Midi_no].data(), Midi[Midi_no].size());

	return _Ptr + sizeof(Sony_Sequence_SEQ) + Midi[Midi_no].size();
}


/*
	Save SEQ
*/
bool Sony_PlayStation_Sequence::SaveSEQ(std::filesystem::path Path, std::size_t Midi_no, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = SaveSEQ(m_File, Midi_no, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Save SEP
*/
std::uintmax_t Sony_PlayStation_Sequence::SaveSEP(StdFile& File, std::uintmax_t _Ptr)
{
	if (!File.IsOpen())
	{
		if (!File.Open(File.GetPath(), FileAccessMode::Write, true, false))
		{
			Str.Message("PlayStation Sequence: Error, could not create SEP at 0x%llX in %s", _Ptr, File.GetPath().filename().string().c_str());
			return _Ptr;
		}
	}

	Sony_Sequence_SEP SepHeader{};

	SepHeader.Magic[0] = 0x70;
	SepHeader.Magic[1] = 0x51;
	SepHeader.Magic[2] = 0x45;
	SepHeader.Magic[3] = 0x53;

	SepHeader.Version = 0;

	File.Write(_Ptr, &SepHeader, sizeof(Sony_Sequence_SEP));

	_Ptr += sizeof(Sony_Sequence_SEP);

	for (std::size_t i = 0; i < Midi.size(); i++)
	{
		Sony_Sequence_SEP_Data SepData{};

		SepData.Seq_id = std::byteswap<std::uint16_t>(static_cast<std::uint16_t>(i));

		std::memcpy(&SepData.Header, &Header[i], sizeof(Sony_Sequence_Header));

		File.Write(_Ptr, &SepData, sizeof(Sony_Sequence_SEP_Data));

		File.Write(_Ptr + sizeof(Sony_Sequence_SEP_Data), Midi[i].data(), Midi[i].size());

		_Ptr += sizeof(Sony_Sequence_SEP_Data) + Midi[i].size();
	}

	return _Ptr;
}


/*
	Save SEP
*/
bool Sony_PlayStation_Sequence::SaveSEP(std::filesystem::path Path, std::uintmax_t _Ptr)
{
	StdFile m_File;

	m_File.SetPath(Path);

	std::uintmax_t OldPtr = _Ptr;

	_Ptr = SaveSEP(m_File, _Ptr);

	return OldPtr != _Ptr;
}


/*
	Close
*/
void Sony_PlayStation_Sequence::Close(void)
{
	Header.clear();
	Midi.clear();
}