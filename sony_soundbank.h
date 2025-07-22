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


#pragma once

#include <std_text.h>


#pragma pack(push, 1)


struct Sony_Soundbank_Header
{
	char Magic[4];						// 0x00 // VABp
	std::uint32_t Version;				// 0x04 // VAB Version
	std::uint32_t ID;					// 0x08 // VAB ID
	std::uint32_t Size;					// 0x0C // sizeof(VH) + sizeof(VB)
	std::uint16_t Reserved0;			// 0x10 // System Reserved
	std::uint16_t nProgram;				// 0x12 // Program Count
	std::uint16_t nTone;				// 0x14 // Tone Count
	std::uint16_t nVag;					// 0x16 // VAG Count
	std::uint8_t Volume;				// 0x18 // Master Volume
	std::uint8_t Pan;					// 0x19 // Master Pan
	std::uint8_t Attribute1;			// 0x1A // Attribute
	std::uint8_t Attribute2;			// 0x1B // Attribute
	std::uint32_t Reserved1;			// 0x1C // System Reserved
};


struct Sony_Soundbank_Program
{
	std::uint8_t nTone;					// 0x00 // Tone Count
	std::uint8_t Volume;				// 0x01 // Volume
	std::uint8_t Priority;				// 0x02 // Priority
	std::uint8_t Mode;					// 0x03 // Mode
	std::uint8_t Pan;					// 0x04 // Pan
	char Reserved0;						// 0x05 // System Reserved
	std::int16_t Attribute;				// 0x06 // Attribute
	std::uint32_t Reserved1;			// 0x08 // System Reserved
	std::uint32_t Reserved2;			// 0x0C // System Reserved
};


struct Sony_Soundbank_Tone
{
	std::uint8_t Priority;				// 0x00 // Priority
	std::uint8_t Mode;					// 0x01 // Mode (0=Normal, 4=Reverb)
	std::uint8_t Volume;				// 0x02 // Volume
	std::uint8_t Pan;					// 0x03 // Pan (0=left, 63=center, 127=right)
	std::uint8_t CenterNote;			// 0x04 // Center Note
	std::uint8_t PitchShift;			// 0x05 // Center Note Fine Tune
	std::uint8_t NoteMin;				// 0x06 // Note Limit
	std::uint8_t NoteMax;				// 0x07 // Note Limit
	std::uint8_t VibratoDepth;			// 0x08 // Vibrato Depth
	std::uint8_t VibratoDuration;		// 0x09 // Vibrato Duration
	std::uint8_t PortamentoDepth;		// 0x0A // Portamento Depth
	std::uint8_t PortamentoDuration;	// 0x0B // Portamento Duration
	std::uint8_t PitchBendMin;			// 0x0C // Pitch Bend
	std::uint8_t PitchBendMax;			// 0x0D // Pitch Bend
	std::uint8_t Reserved1;				// 0x0E // System Reserved
	std::uint8_t Reserved2;				// 0x0F // System Reserved
	std::uint16_t adsr1;				// 0x10 // Attack, Decay, Sustain and Release
	std::uint16_t adsr2;				// 0x12 // Attack, Decay, Sustain and Release
	std::int16_t Program;				// 0x14 // Master Program ID
	std::int16_t Vag;					// 0x16 // VAG ID
	std::int16_t Reserved[4];			// 0x18 // System Reserved
};


#pragma pack(pop)


class Sony_PlayStation_Soundbank {
private:

	// File Header
	Sony_Soundbank_Header Header;

	// Program
	std::vector<Sony_Soundbank_Program> Program;

	// Tone
	std::vector<std::vector<Sony_Soundbank_Tone>> Tone;

	// Sample Pointer
	std::vector<std::uint16_t> pSample;

	// Sample
	std::vector<std::vector<std::uint8_t>> Sample;

	// Flag
	bool b_VhOpen;
	bool b_VbOpen;

public:

	// Standard String
	Standard_String Str;

	/*
		Construction
	*/
	explicit Sony_PlayStation_Soundbank(std::filesystem::path Path, std::uintmax_t _Ptr = 0) :
		Header(),
		Program(),
		Tone(),
		pSample(256),
		Sample(),
		b_VhOpen(false),
		b_VbOpen(false)
	{
		OpenVAB(Path, _Ptr);
	}

	explicit Sony_PlayStation_Soundbank(void) :
		Header(),
		Program(),
		Tone(),
		pSample(256),
		Sample(),
		b_VhOpen(false),
		b_VbOpen(false)
		{}

	virtual ~Sony_PlayStation_Soundbank(void)
	{
		CloseVAB();
	}

	/*
		Open VH
	*/
	std::uintmax_t OpenVH(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open VH
	*/
	bool OpenVH(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Open VB
	*/
	std::uintmax_t OpenVB(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open VB
	*/
	bool OpenVB(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Open VAB
	*/
	std::uintmax_t OpenVAB(StdFile& File, std::uintmax_t _Ptr);

	/*
		Open VAB
	*/
	bool OpenVAB(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Save VH
	*/
	std::uintmax_t SaveVH(StdFile& File, std::uintmax_t _Ptr);

	/*
		Save VH
	*/
	bool SaveVH(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Save VB
	*/
	std::uintmax_t SaveVB(StdFile& File, std::uintmax_t _Ptr);

	/*
		Save VB
	*/
	bool SaveVB(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Save VAB
	*/
	std::uintmax_t SaveVAB(StdFile& File, std::uintmax_t _Ptr);

	/*
		Save VAB
	*/
	bool SaveVAB(std::filesystem::path Path, std::uintmax_t _Ptr = 0);

	/*
		Close VH
	*/
	void CloseVH(void);

	/*
		Close VB
	*/
	void CloseVB(void);

	/*
		Close VAB
	*/
	void CloseVAB(void);

	/*
		VH Size
	*/
	[[nodiscard]] std::uintmax_t VhSize(void) const;

	/*
		VB Size
	*/
	[[nodiscard]] std::uintmax_t VbSize(void) const;

	/*
		VAB Size
	*/
	[[nodiscard]] std::uintmax_t VabSize(void) const;

};