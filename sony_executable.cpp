/*
*
*	Megan Grass
*	March 07, 2024
*
*
*	TODO:
*
*/


#include "sony_executable.h"


/*
    Print Command Line Help
*/
void Sony_PlayStation_Executable::PrintHelp(void)
{
    std::cout << "Sony PlayStation Executable: Help" << std::endl << std::endl;
    std::cout << "  OPEN <file>\t\t\t\tOpen executable for operation" << std::endl << std::endl;
    std::cout << "  OVERLAY <address> <file>\t\tOpen overlay for operation" << std::endl << std::endl;
    std::cout << "Following must be completed before OPEN and OVERLAY, if needed:" << std::endl << std::endl;
    std::cout << "  <ram> 2=2MB (default), 8=8MB" << std::endl;
    std::cout << "  <kseg> 0=0x00000000, 1=0x80000000 (default), 2=0xA0000000, 3=0xC0000000" << std::endl;
    std::cout << "  <address> must be hexadecimal" << std::endl << std::endl;
    std::cout << "  RAM <ram>\t\t\t\tSet the RAM size" << std::endl;
    std::cout << "  KSEG <kseg>\t\t\t\tSet the Kernel Memory region" << std::endl;
    std::cout << "  FILEPTR <address>\t\t\tConvert memory address to file offset and print" << std::endl << std::endl;
    std::cout << "Executable or overlay must be open prior to following operations:" << std::endl << std::endl;
    std::cout << "  <size> must be hexadecimal" << std::endl << std::endl;
    std::cout << "  HEADER\t\t\t\tPrint header" << std::endl;
    std::cout << "  PAD <address> <size>\t\t\tPadding (0) bytes" << std::endl;
    std::cout << "  READ <address> <size>\t\t\tRead, buffer is temporary until next READ" << std::endl;
    std::cout << "  WRITE <address>\t\t\tWrite, using data from temporary buffer" << std::endl;
    std::cout << "  IMPORT <address> <file>\t\tImport file to address" << std::endl;
    std::cout << "  EXPORT <address> <size> <file>\tExport file from address" << std::endl;
    std::cout << "  DUMPRAM <file>\t\t\tExport RAM to file" << std::endl;
    std::cout << "  DUMPEXE <file>\t\t\tExport EXE to file" << std::endl;
}


/*
    Print Executable Header
*/
void Sony_PlayStation_Executable::PrintHeader(Sony_PlayStation_Executable_Header Header)
{
    std::cout << "Sony PlayStation Executable:" << std::endl;
	std::cout << "\tKey: " << Header.key << std::endl;
    std::cout << "\tText: " << std::hex << Header.text << std::dec << std::endl;
    std::cout << "\tData: " << std::hex << Header.data << std::dec << std::endl;
    std::cout << "\tProgram Counter: " << std::hex << Header.pc0 << std::dec << std::endl;
    std::cout << "\tGeneral Purpose: " << std::hex << Header.gp0 << std::dec << std::endl;
    std::cout << "\tText Address: " << std::hex << Header.t_addr << std::dec << std::endl;
    std::cout << "\tText Size: " << std::hex << Header.t_size << std::dec << std::endl;
    std::cout << "\tData Address: " << std::hex << Header.d_addr << std::dec << std::endl;
    std::cout << "\tData Size: " << std::hex << Header.d_size << std::dec << std::endl;
    std::cout << "\tBSS Address: " << std::hex << Header.b_addr << std::dec << std::endl;
    std::cout << "\tBSS Size: " << std::hex << Header.b_size << std::dec << std::endl;
    std::cout << "\tStack Address: " << std::hex << Header.ps_addr << std::dec << std::endl;
    std::cout << "\tStack Size: " << std::hex << Header.ps_size << std::dec << std::endl;
    std::cout << "\tStack Pointer: " << std::hex << Header.sp << std::dec << std::endl;
    std::cout << "\tFrame Pointer: " << std::hex << Header.fp << std::dec << std::endl;
    std::cout << "\tGlobal Pointer: " << std::hex << Header.gp << std::dec << std::endl;
    std::cout << "\tReturn Address: " << std::hex << Header.ret << std::dec << std::endl;
    std::cout << "\tBase Address: " << std::hex << Header.base << std::dec << std::endl;
    std::cout << "\tTitle: " << Header.title << std::endl;
}


/*
    Command Line Interface
*/
std::size_t Sony_PlayStation_Executable::Commandline(StrVec Args)
{
    Sony_PlayStation_Executable Exe;

    Standard_String Str;

    Standard_FileSystem FS;

    std::size_t ArgCount = 1;

    std::vector<std::uint8_t> _Buffer;

    for (std::size_t i = ArgCount; i < Args.size(); i++)
    {
        Str.ToUpper(Args[i]);

        if (Args[i] == "HELP")
        {
            PrintHelp();
            ArgCount++;
        }

        if (Args[i] == "HEADER")
        {
            if (Exe.IsOpen())
            {
				Exe.PrintHeader(Exe.m_Header);
            }
            else
            {
				std::cout << "Sony PlayStation Executable: Executable is not open" << std::endl;
			}
            ArgCount++;
        }

        if (Args[i] == "OPEN")
        {
            if (i + 1 < Args.size())
            {
                if (Exe.Open(Args[i + 1]))
                {
                    std::cout << "Sony PlayStation Executable: Successfully opened " << FS.GetFileName(Args[i + 1]) << std::endl;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: Failed to open " << FS.GetFileName(Args[i + 1]) << std::endl;
                }
                ArgCount++;
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Open error, no file specified" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "OVERLAY")
        {
            if (i + 1 < Args.size())
            {
                std::uintmax_t Offset = std::strtoull(Args[i + 1].c_str(), nullptr, 10);
                if (i + 2 < Args.size())
                {
                    if (Exe.OpenOverlay(Offset, Args[i + 2]))
                    {
                        std::cout << "Sony PlayStation Executable: Successfully opened " << FS.GetFileName(Args[i + 2]) << std::endl;
                    }
                    else
                    {
                        std::cout << "Sony PlayStation Executable: Failed to open " << FS.GetFileName(Args[i + 2]) << std::endl;
                    }
                    ArgCount++;
                }
                else
				{
					std::cout << "Sony PlayStation Executable: Open overlay error, no file specified" << std::endl;
				}
                ArgCount++;
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Open overlay error, no address specified" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "RAM")
        {
            if (i + 1 < Args.size())
            {
				std::uintmax_t Size = std::strtoull(Args[i + 1].c_str(), nullptr, 10);
                if (Size == 2)
                {
					Exe.SetRAMSize(0x00200000);
                    std::cout << "Sony PlayStation Executable: RAM size is 2MB" << std::endl;
                }
				else if (Size == 8)
                {
                    Exe.SetRAMSize(0x00800000);
                    std::cout << "Sony PlayStation Executable: RAM size is 8MB" << std::endl;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: RAM size error, invalid size" << std::endl;
                }
                ArgCount++;
            }
            else
            {
				std::cout << "Sony PlayStation Executable: RAM size error, no size specified" << std::endl;
			}
            ArgCount++;
        }

        if (Args[i] == "KSEG")
        {
            if (i + 1 < Args.size())
            {
				std::uintmax_t Region = std::strtoull(Args[i + 1].c_str(), nullptr, 10);
                if (Region < 4)
                {
                    switch (Region)
                    {
                    case 0: Exe.SetMemoryRegion(Sony_PlayStation_Memory_Region::KUSEG); break;
                    case 1: Exe.SetMemoryRegion(Sony_PlayStation_Memory_Region::KSEG0); break;
                    case 2: Exe.SetMemoryRegion(Sony_PlayStation_Memory_Region::KSEG1); break;
                    case 3: Exe.SetMemoryRegion(Sony_PlayStation_Memory_Region::KSEG2); break;
                    }
					std::cout << "Sony PlayStation Executable: Memory region is " << std::hex << Exe.GetMemoryRegion() << " - " << Exe.GetMemoryRegion() + Exe.GetRAMSize() << std::dec << std::endl;
				}
                else
                {
					std::cout << "Sony PlayStation Executable: Memory region error, invalid region" << std::endl;
				}
                ArgCount++;
			}
            else
            {
				std::cout << "Sony PlayStation Executable: Memory region error, no region specified" << std::endl;
			}
            ArgCount++;
        }

        if (Args[i] == "FILEPTR")
        {
            if (i + 1 < Args.size())
            {
                std::uintmax_t Offset = std::strtoull(Args[i + 1].c_str(), nullptr, 16);
                std::cout << "Sony PlayStation Executable: File offset is " << std::hex << Exe.GetFileOffset(Offset) << std::dec << std::endl;
                ArgCount++;
            }
            else
            {
                std::cout << "Sony PlayStation Executable: File offset error, no address specified" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "PAD")
        {
            if (Exe.IsOpen())
            {
                if (i + 1 < Args.size())
                {
                    std::uintmax_t Offset = std::strtoull(Args[i + 1].c_str(), nullptr, 16);
                    if (i + 2 < Args.size())
                    {
                        std::uintmax_t Size = std::strtoull(Args[i + 2].c_str(), nullptr, 16);
                        std::cout << "Sony PlayStation Executable: Padding " << std::hex << Size << " bytes at " << Offset << std::dec << std::endl;
                        std::uintmax_t pNext = Exe.Pad(Offset, Size);
                        std::cout << "Sony PlayStation Executable: Successfully padded " << std::hex << Size << " bytes, next offset is " << pNext << std::dec << std::endl;
                        ArgCount++;
                    }
                    else
                    {
                        std::cout << "Sony PlayStation Executable: Pad error, no size specified" << std::endl;
                    }
                    ArgCount++;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: Pad error, no address specified" << std::endl;
                }
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Executable is not open" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "READ")
        {
            if (Exe.IsOpen())
            {
                if (i + 1 < Args.size())
                {
                    std::uintmax_t Offset = std::strtoull(Args[i + 1].c_str(), nullptr, 16);
                    if (i + 2 < Args.size())
                    {
                        if (!_Buffer.empty()) { _Buffer.clear(); _Buffer.shrink_to_fit(); }
                        std::uintmax_t Size = std::strtoull(Args[i + 2].c_str(), nullptr, 16);
                        std::cout << "Sony PlayStation Executable: Reading " << std::hex << Size << " bytes from " << Offset << std::dec << std::endl;
                        std::uintmax_t pNext = Exe.Read(Offset, _Buffer, Size);
                        std::cout << "Sony PlayStation Executable: Successfully read " << std::hex << Size << " bytes, next offset is " << pNext << std::dec << std::endl;
                        ArgCount++;
                    }
                    else
                    {
                        std::cout << "Sony PlayStation Executable: Read error, no size specified" << std::endl;
                    }
                    ArgCount++;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: Read error, no address specified" << std::endl;
                }
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Executable is not open" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "WRITE")
        {
            if (Exe.IsOpen())
            {
                if (i + 1 < Args.size())
                {
                    if (!_Buffer.empty())
                    {
                        std::uintmax_t Offset = std::strtoull(Args[i + 1].c_str(), nullptr, 16);
                        std::cout << "Sony PlayStation Executable: Writing " << std::hex << _Buffer.size() << " bytes from " << Offset << std::dec << std::endl;
                        std::uintmax_t pNext = Exe.Write(Offset, _Buffer.data(), _Buffer.size());
                        std::cout << "Sony PlayStation Executable: Successfully wrote " << std::hex << _Buffer.size() << " bytes, next offset is " << pNext << std::dec << std::endl;
                    }
                    else
                    {
						std::cout << "Sony PlayStation Executable: Write error, temporary buffer is empty" << std::endl;
					}
                    ArgCount++;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: Write error, no address specified" << std::endl;
                }
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Executable is not open" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "IMPORT")
        {
            if (Exe.IsOpen())
            {
                if (i + 1 < Args.size())
                {
                    std::uintmax_t Offset = std::strtoull(Args[i + 1].c_str(), nullptr, 16);
                    if (i + 2 < Args.size())
                    {
                        if (FS.Exists(Args[i + 2]))
                        {
                            std::cout << "Sony PlayStation Executable: Importing " << FS.GetFileName(Args[i + 2]) << " to " << std::hex << Offset << std::dec << std::endl;
                            std::uintmax_t pNext = Exe.Import(Offset, Args[i + 2]);
                            std::cout << "Sony PlayStation Executable: Successfully imported " << std::hex << (pNext - Offset) << " bytes, next offset is " << pNext << std::dec << std::endl;
                        }
                        else
                        {
                            std::cout << "Sony PlayStation Executable: Import error, file not found" << std::endl;
                        }
                        ArgCount++;
                    }
                    else
                    {
                        std::cout << "Sony PlayStation Executable: Import error, no file specified" << std::endl;
                    }
                    ArgCount++;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: Import error, no address specified" << std::endl;
                }
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Executable is not open" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "EXPORT")
        {
            if (Exe.IsOpen())
            {
                if (i + 1 < Args.size())
                {
                    std::uintmax_t Offset = std::strtoull(Args[i + 1].c_str(), nullptr, 16);
                    if (i + 2 < Args.size())
                    {
                        std::uintmax_t Size = std::strtoull(Args[i + 2].c_str(), nullptr, 16);
                        if (i + 3 < Args.size())
                        {
                            std::cout << "Sony PlayStation Executable: Exporting " << FS.GetFileName(Args[i + 3]) << " from " << std::hex << Offset << std::dec << std::endl;
                            Exe.Export(Offset, Size, Args[i + 3]);
                            std::cout << "Sony PlayStation Executable: Successfully exported " << std::hex << Size << " bytes, next offset is " << (Offset + Size) << std::dec << std::endl;
                            ArgCount++;
                        }
                        else
                        {
                            std::cout << "Sony PlayStation Executable: Export error, no file specified" << std::endl;
                        }
                        ArgCount++;
                    }
                    else
                    {
                        std::cout << "Sony PlayStation Executable: Export error, no size specified" << std::endl;
                    }
                    ArgCount++;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: Export error, no address specified" << std::endl;
                }
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Executable is not open" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "DUMPRAM")
        {
            if (Exe.IsOpen())
            {
                if (i + 1 < Args.size())
                {
                    if (Exe.ExportRAM(Args[i + 1]))
                    {
                        std::cout << "Sony PlayStation Executable: Successfully exported RAM as " << FS.GetFileName(Args[i + 1]) << std::endl;
                    }
                    else
                    {
                        std::cout << "Sony PlayStation Executable: Failed to export RAM" << std::endl;
                    }
                    ArgCount++;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: RAM export error, no file specified" << std::endl;
                }
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Executable is not open" << std::endl;
            }
            ArgCount++;
        }

        if (Args[i] == "DUMPEXE")
        {
            if (Exe.IsOpen())
            {
                if (i + 1 < Args.size())
                {
                    if (Exe.ExportEXE(Args[i + 1]))
                    {
                        std::cout << "Sony PlayStation Executable: Successfully exported EXE as " << FS.GetFileName(Args[i + 1]) << std::endl;
                    }
                    else
                    {
                        std::cout << "Sony PlayStation Executable: Failed to export EXE" << std::endl;
                    }
                    ArgCount++;
                }
                else
                {
                    std::cout << "Sony PlayStation Executable: EXE export error, no file specified" << std::endl;
                }
            }
            else
            {
                std::cout << "Sony PlayStation Executable: Executable is not open" << std::endl;
            }
            ArgCount++;
        }

        i = ArgCount;
    }

    if (Exe.IsOpen()) { Exe.Close(); }

    return ArgCount;
}


/*
    Get Memory Region
*/
std::uintmax_t Sony_PlayStation_Executable::GetMemoryRegion(void)
{
    switch (e_MemoryRegion)
    {
    case Sony_PlayStation_Memory_Region::KUSEG: return 0x00000000;
    case Sony_PlayStation_Memory_Region::KSEG0: return 0x80000000;
    case Sony_PlayStation_Memory_Region::KSEG1: return 0xA0000000;
    case Sony_PlayStation_Memory_Region::KSEG2: return 0xC0000000;
    }
    return 0x80000000;
}


/*
	Open
*/
bool Sony_PlayStation_Executable::Open(std::vector<std::uint8_t> EXE)
{
	if (EXE.empty()) { return false; }

	if (b_Open) { Close(); }

	std::memcpy(&m_Header, &EXE.data()[0x00], sizeof(Sony_PlayStation_Executable_Header));

    if (!m_RAMSize) { SetRAMSize(0x00200000); }

    if (m_RAM.size() != m_RAMSize) { m_RAM.resize(m_RAMSize); }

	std::memcpy(&m_RAM.data()[m_Header.t_addr ^ GetMemoryRegion()], &EXE[0x800], m_Header.t_size);

	return b_Open = true;
}
bool Sony_PlayStation_Executable::Open(std::filesystem::path Path)
{
    StdFile m_File { Path, FileAccessMode::Read, true, false };

    bool bRet = false; 

    if (m_File.IsOpen())
    {
        std::vector<std::uint8_t> EXE(m_File.Size());

        m_File.Read(0, EXE.data(), m_File.Size());

        m_File.Close();

        bRet = Open(EXE);
    }

	return bRet;
}


/*
    Open Overlay
*/
bool Sony_PlayStation_Executable::OpenOverlay(std::uintmax_t _Offset, std::vector<std::uint8_t> BIN)
{
    if (BIN.empty()) { return false; }

    if (b_Open) { Close(); }

    if (!m_RAMSize) { SetRAMSize(0x00200000); }

    if (m_RAM.size() != m_RAMSize) { m_RAM.resize(m_RAMSize); }

    std::memcpy(&m_RAM.data()[_Offset ^ GetMemoryRegion()], &BIN.data()[0x00], BIN.size());

    return b_Open = true;
}
bool Sony_PlayStation_Executable::OpenOverlay(std::uintmax_t _Offset, std::filesystem::path Path)
{
    StdFile m_File { Path, FileAccessMode::Read, true, false };

    bool bRet = false;

    if (m_File.IsOpen())
    {
        std::vector<std::uint8_t> BIN(m_File.Size());

        m_File.Read(0, BIN.data(), m_File.Size());

        m_File.Close();

        bRet = OpenOverlay(_Offset, BIN);
    }

    return bRet;
}


/*
	Close
*/
void Sony_PlayStation_Executable::Close(void)
{
    b_Open = false;
	std::memset(&m_Header, 0, sizeof(Sony_PlayStation_Executable_Header));
	m_RAM.clear();
	m_RAM.shrink_to_fit();
}


/*
    File Offset
*/
std::uintmax_t Sony_PlayStation_Executable::GetFileOffset(std::uintmax_t _Offset) const
{
    if (!b_Open) { return 0; }

    _Offset ^= static_cast<unsigned long long>(m_Header.t_addr);

    return _Offset += 0x800;
}


/*
	Zero Padding
*/
std::uintmax_t Sony_PlayStation_Executable::Pad(std::uintmax_t _Offset, std::size_t _ElementSize)
{
	if (!b_Open) { return false; }

	std::vector<std::uint8_t> _Buffer(_ElementSize);

	std::memcpy(&m_RAM.data()[_Offset ^ GetMemoryRegion()], &_Buffer.data()[0x00], _Buffer.size());

    return (_Offset + _ElementSize);
}


/*
	Read
*/
std::uintmax_t Sony_PlayStation_Executable::Read(std::uintmax_t _Offset, void* _Data, std::size_t _ElementSize)
{
	if (!b_Open) { return 0; }

	std::memcpy(_Data, &m_RAM.data()[_Offset ^ GetMemoryRegion()], _ElementSize);

	return (_Offset + _ElementSize);
}


/*
	Write
*/
std::uintmax_t Sony_PlayStation_Executable::Write(std::uintmax_t _Offset, void* _Data, std::size_t _ElementSize)
{
	if (!b_Open) { return 0; }

	std::memcpy(&m_RAM.data()[_Offset ^ GetMemoryRegion()], _Data, _ElementSize);

	return (_Offset + _ElementSize);
}


/*
	Import
*/
std::uintmax_t Sony_PlayStation_Executable::Import(std::uintmax_t _Offset, std::filesystem::path Path)
{
	if (!b_Open) { return 0; }

    StdFile m_File { Path, FileAccessMode::Read, true, false };

    std::uintmax_t iRet = 0;

    if (m_File.IsOpen())
    {
        std::vector<std::uint8_t> BIN(m_File.Size());

        m_File.Read(0, BIN.data(), m_File.Size());

        m_File.Close();

        iRet = Write(_Offset, BIN.data(), BIN.size());
    }

    return iRet;
}


/*
	Export
*/
bool Sony_PlayStation_Executable::Export(std::uintmax_t _Offset, std::size_t _ElementSize, std::filesystem::path Path)
{
	if (!b_Open) { return false; }

    StdFile m_File;

	return m_File.Create(Path, &m_RAM.data()[_Offset ^ GetMemoryRegion()], _ElementSize);
}


/*
	Export RAM
*/
bool Sony_PlayStation_Executable::ExportRAM(std::vector<std::uint8_t>& RAM)
{
	if (!b_Open) { return false; }

	if (!RAM.empty()) { RAM.clear(); RAM.shrink_to_fit(); }

    RAM.resize(m_RAM.size());

	std::memcpy(&RAM.data()[0x00], &m_RAM.data()[0x00], m_RAM.size());

	return true;
}
bool Sony_PlayStation_Executable::ExportRAM(std::filesystem::path Path) const
{
	if (!b_Open) { return false; }

    StdFile m_File;

	return m_File.Create(Path, m_RAM);
}


/*
	Export Executable
*/
bool Sony_PlayStation_Executable::ExportEXE(std::vector<std::uint8_t>& EXE)
{
	if (!b_Open) { return false; }

	if (!EXE.empty()) { EXE.clear(); EXE.shrink_to_fit(); }

	//std::uintmax_t Size = ((m_RAMSize - (static_cast<uintmax_t>(m_Header.t_addr) ^ GetMemoryRegion())) + 0x800);

    std::uintmax_t Size = (static_cast<uintmax_t>(m_Header.t_size) + static_cast<uintmax_t>(m_Header.d_size) + static_cast<uintmax_t>(m_Header.b_size) + static_cast<uintmax_t>(m_Header.ps_size) + 0x800);

    EXE.resize(Size);

	std::memcpy(&EXE.data()[0x00], &m_Header, sizeof(Sony_PlayStation_Executable_Header));

	Size -= 0x800;

	std::memcpy(&EXE.data()[0x800], &m_RAM.data()[m_Header.t_addr ^ GetMemoryRegion()], Size);

	return true;
}
bool Sony_PlayStation_Executable::ExportEXE(std::filesystem::path Path)
{
	if (!b_Open) { return false; }

	std::vector<std::uint8_t> EXE;

	if (!ExportEXE(EXE)) { return false; }

    StdFile m_File;

	return m_File.Create(Path, EXE);
}