/*
*
*	Megan Grass
*	March 07, 2024
*
*
*	TODO: 
*
*/


#pragma once

#include <std_basic_fstream.h>

#include <std_text.h>


#pragma pack(push, 1)


struct Sony_Sequence_Header
{
	std::uint16_t Resolution;		// Ticks per quarter note
	std::uint8_t Tempo[3];			// Tempo (length of quarter note in microseconds)
	std::uint8_t RhythmN;			// Numerator of time signature
	std::uint8_t RhythmD;			// Denominator of time signature
};


struct Sony_Sequence_SEQ
{
	char Magic[4];					// 0x00 // SEQp
	std::uint32_t Version;			// 0x04	// Always 1
	Sony_Sequence_Header Header;	// 0x08	// Sequence Header
};


struct Sony_Sequence_SEP
{
	char Magic[4];					// 0x00 // SEQp
	std::uint16_t Version;			// 0x04	// Always 0
};


struct Sony_Sequence_SEP_Data
{
	std::uint16_t Seq_id;			// 0x00	// SEQ ID
	Sony_Sequence_Header Header;	// 0x02	// Sequence Header
};


#pragma pack(pop)


enum class Sony_Sequence_Type : int
{
	SEQ = (1 << 0),
	SEP = (1 << 1),
	Unknown = 0
};


class Sony_PlayStation_Sequence {
private:

	// Header
	std::vector<Sony_Sequence_Header> Header;

	// MIDI Data
	std::vector<std::vector<std::uint8_t>> Midi;

	// Read MIDI data
	std::uintmax_t readMIDI(StdFile& File, std::uintmax_t _Ptr);

public:

	/*
		Construction
	*/
	explicit Sony_PlayStation_Sequence(void) :
		Header(),
		Midi()
	{
	}
	virtual ~Sony_PlayStation_Sequence(void)
	{
		Close();
	}

	/*
		Detect sequence type
	*/
	Sony_Sequence_Type GetType(StdFile& File, std::uintmax_t _Ptr);

	/*
		Detect sequence type
	*/
	Sony_Sequence_Type GetType(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Get MIDI Count
	*/
	std::size_t GetMidiCount(void) const { return Midi.size(); }

	/*
		Open SEQ
	*/
	std::uintmax_t OpenSEQ(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open SEP
	*/
	bool OpenSEQ(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Open SEP
	*/
	std::uintmax_t OpenSEP(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open SEP
	*/
	bool OpenSEP(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Open
	*/
	std::uintmax_t Open(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open
	*/
	bool Open(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Save SEQ
	*/
	std::uintmax_t SaveSEQ(StdFile& File, std::size_t Midi_no, std::uintmax_t _Ptr);

	/*
		Save SEQ
	*/
	bool SaveSEQ(std::filesystem::path Path, std::size_t Midi_no, std::uintmax_t _Ptr = 0);

	/*
		Save SEP
	*/
	std::uintmax_t SaveSEP(StdFile& File, std::uintmax_t _Ptr);

	/*
		Save SEP
	*/
	bool SaveSEP(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Close
	*/
	void Close(void);

};