//
// elf.h:	Data structures for loading ELF files
//			Types must be declared in another header
//

#pragma once

#ifndef _ELF_H
#define _ELF_H

#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3

#define ELF_MAGIC			0x464c457f
#define ELF_AMD64			0x3e
#define ELF_64BIT			2
#define ELF_LITTLEENDIAN	1
#define ELF_EVCURRENT		1

#pragma pack(1)

typedef struct {
	UINT32	e_mag ;					// 0x7f 45 4C 46 (0x7f ELF)
	UINT8	e_class ;				// 2 means 64-bit
	UINT8	e_data ;				// 1 means little-endian
	UINT8	e_ident ;				// Always set to 1
	UINT8	e_osabi ;				// Always set to 0
	UINT8	e_abiversion ;			// Ignore?
	UINT8	e_pad0[7] ;				// Should all be set to 0
	UINT16	e_type ;				// ET_REL == 1, ET_EXEC == 2, ET_DYN == 3
	UINT16	e_machine ;				// 0x3e for x86_64 (amd64)
	UINT32	e_version ;				// 1 for original version of elf
	UINT64	e_entry ;				// Program entry point
	UINT64	e_phoff ;				// Offset to program headers in file
	UINT64	e_shoff ;				// Offset to section headers in file
	UINT32	e_flags ;				// varies
	UINT16	e_ehsize ;				// Size of header, should be 64 bytes for x86_64
	UINT16	e_phentsize ;			// Size of program header table entry
	UINT16	e_phnum ;				// Number of entries in program header table
	UINT16	e_shentsize ;			// Size of section header table entry
	UINT16	e_shnum ;				// Number of entries in section header table
	UINT16	e_shstrndx ;			// Index of section header table with section names
} Elf64Header ;


#define PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_HDR		6


typedef struct {
	UINT32	p_type ;				// Type of the segment
	UINT32	p_flags ;				// "Segment dependent flags"
	UINT64	p_offset ;				// Offset of the segment in the file
	UINT64	p_vaddr ;				// Virtual address of the segment in memory
	UINT64	p_paddr ;				// Physical address (maybe irrelevant)
	UINT64	p_filesz ;				// Size (in bytes) of the segment in the file (may be 0)
	UINT64	p_memsz ;				// Size in memory (may be 0)
	UINT64	p_align ;				// (0,1) == no alignment, others power of 2 where p_vaddr = p_offset % p_align
} Elf64ProgramHeader ;


#define SHT_NULL		0			// This SHT unused
#define SHT_PROGBITS	1			// Program data
#define SHT_SYMTAB		2			// Symbol table
#define SHT_STRTAB		3			// String table
#define SHT_RELA		4			// Relocaiton entries with addends
#define SHT_HASH		5			// Symbol hash table
#define SHT_DYNAMIC		6			// Dynamic linking information
#define SHT_NOTE		7			// Notes?
#define SHT_NOBITS		8			// Unitialized data (.bss)
#define SHT_REL			9			// Relocation entry, no addends

#define SHF_WRITE		1			// Writable
#define SHF_ALLOC		2			// Occupies memory during exec
#define SHF_EXECINSTR	4			// Executable
#define SHF_MERGE		16			// Might be merged
#define SHF_STRINGS		32			// Contains null terminated strings


typedef struct {
	UINT32		sh_name ;			// An offset to a string in the .shstrtab section with the section name
	UINT32		sh_type ;			// Type from above #define's
	UINT64		sh_flags ;			// Flags from above #define's
	UINT64		sh_addr ;			// Virtual address of the section in memory
	UINT64		sh_offset ;			// Offset of the section in the file (bytes)
	UINT64		sh_size ;			// Size in bytes of the section (may be 0)
	UINT32		sh_link ;			// Section index of associated index
	UINT32		sh_info ;			// Extra info about section
	UINT64		sh_addralign ;		// Required alignment of section in memory (must be power of 2)
	UINT64		sh_entsize ;		// Size in bytes of each entry that have fixed sized entries, otherwise 0
} Elf64SectionHeader ;

#endif
