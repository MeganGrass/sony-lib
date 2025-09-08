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

#include <std_string.h>


#pragma pack(push, 1)
struct Sony_PlayStation_Executable_Header
{
	char key[0x08]{};			// 0x00	// PS-X EXE
	unsigned long text{};		// 0x08	// 
	unsigned long data{};		// 0x0C	// 
	unsigned long pc0{};		// 0x10	// Program Counter	// Execution start address
	unsigned long gp0{};		// 0x14	// General Purpose	// gp register initial value
	unsigned long t_addr{};		// 0x18	// Text				// Starting address of initialized text section
	unsigned long t_size{};		// 0x1C	// Text				// Size of text section
	unsigned long d_addr{};		// 0x20	// Data				// Starting address of initialized data section
	unsigned long d_size{};		// 0x24	// Data				// Size of initialized data section
	unsigned long b_addr{};		// 0x28	// Uninitialized data section start address
	unsigned long b_size{};		// 0x2C	// Uninitialized data section size
	unsigned long ps_addr{};	// 0x30	// Stack			// Stack start address (specified by the user)
	unsigned long ps_size{};	// 0x34	// Stack			// Stack size (specified by the user)
	unsigned long sp{};			// 0x38	// Stack Pointer	// Register shunt variable
	unsigned long fp{};			// 0x3C	// Frame Pointer	// Register shunt variable
	unsigned long gp{};			// 0x40	// Global Pointer	// Register shunt variable
	unsigned long ret{};		// 0x44	// Register shunt variable
	unsigned long base{};		// 0x48	// Register shunt variable
	char title[0x3C]{};			// 0x4C	// Sony Computer Entertainment Inc.
};
#pragma pack(pop)


enum class Sony_PlayStation_Memory_Region : std::uintmax_t
{
	KUSEG = 0x00000000,			// Mirror of KSEG0/KSEG1
	KSEG0 = 0x80000000,			// Mirror Physical Memory (Cached)
	KSEG1 = 0xA0000000,			// Normal Physical Memory (Uncached)
	KSEG2 = 0xC0000000			// Cache Control Hardware Registers
};


class Sony_PlayStation_Executable {
private:

	// Executable Header
	Sony_PlayStation_Executable_Header m_Header;

	// RAM
	std::vector<std::uint8_t> m_RAM;

	// RAM Size
	std::size_t m_RAMSize;

	// Memory Region
	Sony_PlayStation_Memory_Region e_MemoryRegion;

	// Flag
	bool b_Open;

public:

	/*
		Construction
	*/
	explicit Sony_PlayStation_Executable(std::filesystem::path Path) :
		m_Header(),
		m_RAM(0x00200000),
		m_RAMSize(0x00200000),
		e_MemoryRegion(Sony_PlayStation_Memory_Region::KSEG0),
		b_Open(false)
	{
		Open(Path);
	}

	explicit Sony_PlayStation_Executable(void) :
		m_Header(),
		m_RAM(0x00200000),
		m_RAMSize(0x00200000),
		e_MemoryRegion(Sony_PlayStation_Memory_Region::KSEG0),
		b_Open(false)
	{
	}

	virtual ~Sony_PlayStation_Executable(void)
	{
		Close();
	}


	/*
		Is the executable loaded into memory?
	*/
	bool operator !() { return !b_Open; }

	/*
		Print Command Line Help
	*/
	static void PrintHelp(void);

	/*
		Print Executable Header
	*/
	static void PrintHeader(Sony_PlayStation_Executable_Header Header);

	/*
		Command Line Interface
	*/
	std::size_t Commandline(StrVec Args);

	/*
		Get Executable Header
	*/
	Sony_PlayStation_Executable_Header GetHeader(void) const { return m_Header; }

	/*
		Get RAM Size
	*/
	std::size_t GetRAMSize(void) const { return m_RAMSize; }

	/*
		Set RAM Size
	*/
	void SetRAMSize(std::size_t _Size) { m_RAMSize = _Size; }

	/*
		Get Memory Region Integer
	*/
	std::uintmax_t GetMemoryRegion(void);

	/*
		Set Memory Region
	*/
	void SetMemoryRegion(Sony_PlayStation_Memory_Region Region) { e_MemoryRegion = Region; }

	/*
		Open from unsigned char buffer
	*/
	bool Open(std::vector<std::uint8_t> EXE);

	/*
		Open from file name
	*/
	bool Open(std::filesystem::path Path);

	/*
		Open overlay from unsigned char buffer
	*/
	bool OpenOverlay(std::uintmax_t _Offset, std::vector<std::uint8_t> BIN);

	/*
		Open overlay from file name
	*/
	bool OpenOverlay(std::uintmax_t _Offset, std::filesystem::path Path);

	/*
		Is the executable loaded into memory?
	*/
	bool IsOpen(void) const { return b_Open; }

	/*
		Close
	*/
	void Close(void);

	/*
		Get file position from executable pointer
	*/
	std::uintmax_t GetFileOffset(std::uintmax_t _Offset) const;

	/*
		Write zero padding
	*/
	std::uintmax_t Pad(std::uintmax_t _Offset, std::size_t _ElementSize);

	/*
		Read data from the executable
	*/
	std::uintmax_t Read(std::uintmax_t _Offset, void* _Data, std::size_t _ElementSize);

	/*
		Read data from the executable
	*/
	template<typename T, typename Traits = std::allocator_traits<T>>
	std::uintmax_t Read(std::uintmax_t _Offset, std::vector<T, Traits>& _Data, std::size_t _ElementSize)
	{
		if (!_Data.empty()) { _Data.clear(); _Data.shrink_to_fit(); }
		_Data.insert(_Data.end(), m_RAM.data() + (_Offset ^ GetMemoryRegion()), m_RAM.data() + (_Offset ^ GetMemoryRegion()) + _ElementSize);
		return (_Offset + _ElementSize);
	}

	/*
		Write data to the executable
	*/
	std::uintmax_t Write(std::uintmax_t _Offset, void* _Data, std::size_t _ElementSize);

	/*
		Write data to the executable
	*/
	template<typename T, typename Traits = std::allocator_traits<T>>
	std::uintmax_t Write(std::uintmax_t _Offset, std::vector<T, Traits> _Data)
	{
		for (auto& _Element : _Data)
		{
			std::memcpy(&m_RAM.data()[_Offset ^ GetMemoryRegion()], &_Element, sizeof(T));
			_Offset += sizeof(T);
		}
		return (_Offset + _Data.size());
	}

	/*
		Import data from file
	*/
	std::uintmax_t Import(std::uintmax_t _Offset, std::filesystem::path Path);

	/*
		Export data to file
	*/
	bool Export(std::uintmax_t _Offset, std::size_t _ElementSize, std::filesystem::path Path);

	/*
		Export RAM to unsigned char buffer
	*/
	bool ExportRAM(std::vector<std::uint8_t>& RAM);

	/*
		Export RAM to file
	*/
	bool ExportRAM(std::filesystem::path Path) const;

	/*
		Export Executable unsigned char buffer
	*/
	bool ExportEXE(std::vector<std::uint8_t>& EXE);

	/*
		Export Executable to file
	*/
	bool ExportEXE(std::filesystem::path Path);

};