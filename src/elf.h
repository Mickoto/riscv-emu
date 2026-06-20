#include <cstdint>
#include <stdint.h>
#include <endian.h>

// elf headers

typedef struct {
	uint32_t magic;
	uint8_t _class;
	uint8_t data;
	uint8_t version;
	uint8_t abi;
	uint8_t abiVersion;
	uint8_t reserved[7];
} ELFHeaderPref ;

typedef struct {
	ELFHeaderPref prefix;
	uint16_t type;
	uint16_t machine;
	uint32_t objVersion;

	uint32_t entryOffset;
	uint32_t programHeadersOffset;
	uint32_t sectionHeadersOffset;

	uint32_t eflags;
	uint16_t headerSize;
	uint16_t programHeaderSize;
	uint16_t programHeadersNum;
	uint16_t sectionHeaderSize;
	uint16_t sectionHeadersNum;
	uint16_t stringHeaderInd;
} ELFHeader32;

typedef struct {
	ELFHeaderPref prefix;
	uint16_t type;
	uint16_t machine;
	uint32_t objVersion;

	uint64_t entryOffset;
	uint64_t programHeadersOffset;
	uint64_t sectionHeadersOffset;

	uint32_t eflags;
	uint16_t headerSize;
	uint16_t programHeaderSize;
	uint16_t programHeadersNum;
	uint16_t sectionHeaderSize;
	uint16_t sectionHeadersNum;
	uint16_t stringHeaderInd;
} ELFHeader64;

constexpr uint32_t FourCC(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
#if BYTE_ORDER == BIG_ENDIAN
	return (uint32_t)(a << 24) | (uint32_t)(b << 16) | (uint32_t)(c << 8) | (d);
#else
	return (uint32_t)(d << 24) | (uint32_t)(c << 16) | (uint32_t)(b << 8) | (a);
#endif
}

const constexpr static uint32_t ELFMAGIC = FourCC(0x7F, 'E', 'L', 'F');

const static int CLASS32 = 1;
const static int CLASS64 = 2;

const static int DATALE = 1;
const static int DATABE = 2;

const static int CURRENT_VERSION = 1;

// TODO: research ABI field

const static short ELF_NO_TYPE = 0;
const static short ELF_TYPE_RELOCATABLE = 1;
const static short ELF_TYPE_EXECUTABLE = 2;
const static short ELF_TYPE_SHARED_OBJECT = 3;
const static short ELF_TYPE_CORE = 4;
const static short ELF_TYPE_OS_SPECIFIC_LB = 0xfe00;
const static short ELF_TYPE_OS_SPECIFIC_UB = 0xfeff;
const static short ELF_TYPE_PROCESSOR_SPECIFIC_LB = 0xff00;
const static short ELF_TYPE_PROCESSOR_SPECIFIC_UB = 0xffff;

const static unsigned char MACHINE_UNSPECIFIED = 0x00; //	No specific instruction set
// const static unsigned char aaa = 0x01 // 	AT&T WE 32100
// const static unsigned char aaa = 0x02 // 	SPARC
// const static unsigned char aaa = 0x03 // 	x86
// const static unsigned char aaa = 0x04 // 	Motorola 68000 (M68k)
// const static unsigned char aaa = 0x05 // 	Motorola 88000 (M88k)
// const static unsigned char aaa = 0x06 // 	Intel MCU
const static unsigned char MACHINE_INTEL8086 = 0x07; // 	Intel 80860
const static unsigned char MACHINE_MIPS = 0x08; // 	MIPS
// const static unsigned char aaa = 0x09 // 	IBM System/370
// const static unsigned char aaa = 0x0A // 	MIPS RS3000 Little-endian
// const static unsigned char aaa = 0x0F // 	Hewlett-Packard PA-RISC
// const static unsigned char aaa = 0x13 // 	Intel 80960
// const static unsigned char aaa = 0x14 // 	PowerPC
// const static unsigned char aaa = 0x15 // 	PowerPC (64-bit)
// const static unsigned char aaa = 0x16 // 	S390, including S390x
// const static unsigned char aaa = 0x17 // 	IBM SPU/SPC
// const static unsigned char aaa = 0x24 // 	NEC V800
// const static unsigned char aaa = 0x25 // 	Fujitsu FR20
// const static unsigned char aaa = 0x26 // 	TRW RH-32
// const static unsigned char aaa = 0x27 // 	Motorola RCE
// const static unsigned char aaa = 0x28 // 	Arm (up to Armv7/AArch32)
// const static unsigned char aaa = 0x29 // 	Digital Alpha
// const static unsigned char aaa = 0x2A // 	SuperH
// const static unsigned char aaa = 0x2B // 	SPARC Version 9
// const static unsigned char aaa = 0x2C // 	Siemens TriCore embedded processor
// const static unsigned char aaa = 0x2D // 	Argonaut RISC Core
// const static unsigned char aaa = 0x2E // 	Hitachi H8/300
// const static unsigned char aaa = 0x2F // 	Hitachi H8/300H
// const static unsigned char aaa = 0x30 // 	Hitachi H8S
// const static unsigned char aaa = 0x31 // 	Hitachi H8/500
// const static unsigned char aaa = 0x32 // 	IA-64
// const static unsigned char aaa = 0x33 // 	Stanford MIPS-X
// const static unsigned char aaa = 0x34 // 	Motorola ColdFire
// const static unsigned char aaa = 0x35 // 	Motorola M68HC12
// const static unsigned char aaa = 0x36 // 	Fujitsu MMA Multimedia Accelerator
// const static unsigned char aaa = 0x37 // 	Siemens PCP
// const static unsigned char aaa = 0x38 // 	Sony nCPU embedded RISC processor
// const static unsigned char aaa = 0x39 // 	Denso NDR1 microprocessor
// const static unsigned char aaa = 0x3A // 	Motorola Star*Core processor
// const static unsigned char aaa = 0x3B // 	Toyota ME16 processor
// const static unsigned char aaa = 0x3C // 	STMicroelectronics ST100 processor
// const static unsigned char aaa = 0x3D // 	Advanced Logic Corp. TinyJ embedded processor family
const static unsigned char AMDx86_64 = 0x3E; // 	AMD x86-64
// const static unsigned char aaa = 0x3F // 	Sony DSP Processor
// const static unsigned char aaa = 0x40 // 	Digital Equipment Corp. PDP-10
// const static unsigned char aaa = 0x41 // 	Digital Equipment Corp. PDP-11
// const static unsigned char aaa = 0x42 // 	Siemens FX66 microcontroller
// const static unsigned char aaa = 0x43 // 	STMicroelectronics ST9+ 8/16 bit microcontroller
// const static unsigned char aaa = 0x44 // 	STMicroelectronics ST7 8-bit microcontroller
// const static unsigned char aaa = 0x45 // 	Motorola MC68HC16 Microcontroller
// const static unsigned char aaa = 0x46 // 	Motorola MC68HC11 Microcontroller
// const static unsigned char aaa = 0x47 // 	Motorola MC68HC08 Microcontroller
// const static unsigned char aaa = 0x48 // 	Motorola MC68HC05 Microcontroller
// const static unsigned char aaa = 0x49 // 	Silicon Graphics SVx
// const static unsigned char aaa = 0x4A // 	STMicroelectronics ST19 8-bit microcontroller
// const static unsigned char aaa = 0x4B // 	Digital VAX
// const static unsigned char aaa = 0x4C // 	Axis Communications 32-bit embedded processor
// const static unsigned char aaa = 0x4D // 	Infineon Technologies 32-bit embedded processor
// const static unsigned char aaa = 0x4E // 	Element 14 64-bit DSP Processor
// const static unsigned char aaa = 0x4F // 	LSI Logic 16-bit DSP Processor
// const static unsigned char aaa = 0x8C // 	TMS320C6000 Family
// const static unsigned char aaa = 0xAF // 	MCST Elbrus e2k
// const static unsigned char aaa = 0xB7 // 	Arm 64-bits (Armv8/AArch64)
// const static unsigned char aaa = 0xDC // 	Zilog Z80
// const static unsigned char aaa = 0xF3 // 	RISC-V
// const static unsigned char aaa = 0xF7 // 	Berkeley Packet Filter
// const static unsigned char aaa = 0x101 // 	WDC 65C816
// const static unsigned char aaa = 0x102 // 	LoongArch 


// section headers

typedef struct {
	uint32_t nameOffset;
	uint32_t type;
	
	uint32_t flags;
	uint32_t address;
	uint32_t offset;
	uint32_t size;

	uint32_t link;
	uint32_t info;

	uint32_t addressAlign;
	uint32_t entrySize;
} SectionHeader32;

typedef struct {
	uint32_t nameOffset;
	uint32_t type;
	
	uint64_t flags;
	uint64_t address;
	uint64_t offset;
	uint64_t size;

	uint32_t link;
	uint32_t info;

	uint64_t addressAlign;
	uint64_t entrySize;
} SectionHeader64;

const static uint32_t SEC_TYPE_NULL  = 0x0; 	// Section header table entry unused
const static uint32_t SEC_TYPE_PROGBITS  = 0x1; 	// Program data
const static uint32_t SEC_TYPE_SYMTAB  = 0x2; 	// Symbol table
const static uint32_t SEC_TYPE_STRTAB  = 0x3; 	// String table
const static uint32_t SEC_TYPE_RELA  = 0x4; 	// Relocation entries with addends
const static uint32_t SEC_TYPE_HASH  = 0x5; 	// Symbol hash table
const static uint32_t SEC_TYPE_DYNAMIC  = 0x6; 	// Dynamic linking information
const static uint32_t SEC_TYPE_NOTE  = 0x7; 	// Notes
const static uint32_t SEC_TYPE_NOBITS  = 0x8; 	// Program space with no data (bss)
const static uint32_t SEC_TYPE_REL  = 0x9; 	// Relocation entries, no addends
const static uint32_t SEC_TYPE_SHLIB  = 0x0A; 	// Reserved
const static uint32_t SEC_TYPE_DYNSYM  = 0x0B; 	// Dynamic linker symbol table
const static uint32_t SEC_TYPE_INIT_ARRAY  = 0x0E; 	// Array of constructors
const static uint32_t SEC_TYPE_FINI_ARRAY  = 0x0F; 	// Array of destructors
const static uint32_t SEC_TYPE_PREINIT_ARRAY  = 0x10; 	// Array of pre-constructors
const static uint32_t SEC_TYPE_GROUP  = 0x11; 	// Section group
const static uint32_t SEC_TYPE_SYMTAB_SHNDX  = 0x12; 	// Extended section indices
const static uint32_t SEC_TYPE_NUM  = 0x13; 	// Number of defined types.
// 0x60000000 	SHT_LOOS 	Start OS-specific.

const static uint64_t SHF_WRITE = 0x1; 	// Writable
const static uint64_t SHF_ALLOC = 0x2; 	// Occupies memory during execution
const static uint64_t SHF_EXECINSTR = 0x4; 	// Executable
const static uint64_t SHF_MERGE = 0x10; 	// Might be merged
const static uint64_t SHF_STRINGS = 0x20; 	// Contains null-terminated strings
const static uint64_t SHF_INFO_LINK = 0x40; 	// 'sh_info' contains SHT index
const static uint64_t SHF_LINK_ORDER = 0x80; 	// Preserve order after combining
const static uint64_t SHF_OS_NONCONFORMING = 0x100; 	// Non-standard OS specific handling required
const static uint64_t SHF_GROUP = 0x200; 	// Section is member of a group
const static uint64_t SHF_TLS = 0x400; 	// Section hold thread-local data
const static uint64_t SHF_MASKOS = 0x0FF00000; 	// OS-specific
const static uint64_t SHF_MASKPROC = 0xF0000000; 	// Processor-specific
const static uint64_t SHF_ORDERED = 0x4000000; 	// Special ordering requirement (Solaris)
const static uint64_t SHF_EXCLUDE = 0x8000000; 	// Section is excluded unless referenced or allocated (Solaris) 

typedef struct {
	uint32_t type;
	uint32_t offset;
	uint32_t vAddr;
	uint32_t phAddr;
	uint32_t sizeInFile;
	uint32_t sizeInMemory;
	uint32_t segmentFlags;
	uint32_t align;
} SegmentHeader32;

typedef struct {
	uint32_t type;
	uint32_t segmentFlags;
	uint64_t offset;
	uint64_t vAddr;
	uint64_t phAddr;
	uint64_t sizeInFile;
	uint64_t sizeInMemory;
	uint64_t align;
} SegmentHeader64;

const static uint32_t SEGMENT_FLAG_EXECUTE = 0x1;
const static uint32_t SEGMENT_FLAG_WRITE = 0x2;
const static uint32_t SEGMENT_FLAG_READ = 0x4;

const static uint32_t PT_NULL = 0x00000000; //		Program header table entry unused.
const static uint32_t PT_LOAD = 0x00000001; //		Loadable segment.
const static uint32_t PT_DYNAMIC = 0x00000002; //	Dynamic linking information.
const static uint32_t PT_INTERP = 0x00000003; //	Interpreter information.
const static uint32_t PT_NOTE = 0x00000004; //		Auxiliary information.
const static uint32_t PT_SHLIB = 0x00000005; //	Reserved.
const static uint32_t PT_PHDR = 0x00000006; //		Segment containing program header table itself.
const static uint32_t PT_TLS = 0x00000007; //	Thread-Local Storage template.
const static uint32_t PT_LOOS = 0x60000000; //		Reserved inclusive range. Operating system specific.
const static uint32_t PT_HIOS = 0x6FFFFFFF; //
const static uint32_t PT_LOPROC = 0x70000000; //	Reserved inclusive range. Processor specific. 
