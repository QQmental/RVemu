#pragma once

// copy from here with some modification
// https://github.com/torvalds/linux/blob/16f73eb02d7e1765ccab3d2018e0bd98eb93d973/include/uapi/linux/elf.h#L220

/* 64-bit ELF base types. */
#include <stdint.h>

using Elf64_Addr = uint64_t;
using Elf64_Half = uint16_t;
using Elf64_SHalf = int16_t;
using Elf64_Off = uint64_t;
using Elf64_Sword = int32_t;
using Elf64_Word = uint32_t;
using Elf64_Xword = uint64_t;
using Elf64_Sxword = int64_t;



/* These constants are for the segment types stored in the image headers */
constexpr uint64_t PT_NULL    = 0;
constexpr uint64_t PT_LOAD    = 1;
constexpr uint64_t PT_DYNAMIC = 2;
constexpr uint64_t PT_INTERP  = 3;
constexpr uint64_t PT_NOTE    = 4;
constexpr uint64_t PT_SHLIB   = 5;
constexpr uint64_t PT_PHDR    = 6;
constexpr uint64_t PT_TLS     = 7;               /* Thread local storage segment */
constexpr uint64_t PT_LOOS    = 0x60000000;      /* OS-specific */
constexpr uint64_t PT_HIOS    = 0x6fffffff;      /* OS-specific */
constexpr uint64_t PT_LOPROC  = 0x70000000;
constexpr uint64_t PT_HIPROC  = 0x7fffffff;


/* These constants define the different elf file types */
constexpr uint64_t ET_NONE   = 0;
constexpr uint64_t ET_REL    = 1;
constexpr uint64_t ET_EXEC   = 2;
constexpr uint64_t ET_DYN    = 3;
constexpr uint64_t ET_CORE   = 4;
constexpr uint64_t ET_LOPROC = 0xff00;
constexpr uint64_t ET_HIPROC = 0xffff;

constexpr uint64_t EI_NIDENT = 16;

typedef struct elf64_sym {
  Elf64_Word st_name;		/* Symbol name, index in string tbl */
  unsigned char	st_info;	/* Type and binding attributes */
  unsigned char	st_other;	/* No defined meaning, 0 */
  Elf64_Half st_shndx;		/* Associated section index */
  Elf64_Addr st_value;		/* Value of the symbol */
  Elf64_Xword st_size;		/* Associated symbol size */
} Elf64_Sym;

typedef struct elf64_hdr {
  unsigned char	e_ident[EI_NIDENT];	/* ELF "magic number" */
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;		/* Entry point virtual address */
  Elf64_Off e_phoff;		/* Program header table file offset */
  Elf64_Off e_shoff;		/* Section header table file offset */
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;

#define	EI_MAG0		0		/* e_ident[] indexes */
#define	EI_MAG1		1
#define	EI_MAG2		2
#define	EI_MAG3		3
#define	EI_CLASS	4
#define	EI_DATA		5
#define	EI_VERSION	6
#define	EI_OSABI	7
#define	EI_PAD		8

#define	ELFMAG0		0x7f		/* EI_MAG */
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'
#define	ELFMAG		"\177ELF"
#define	SELFMAG		4

#define	ELFCLASSNONE	0		/* EI_CLASS */
#define	ELFCLASS32	1
#define	ELFCLASS64	2
#define	ELFCLASSNUM	3

#define ELFDATANONE	0		/* e_ident[EI_DATA] */
#define ELFDATA2LSB	1
#define ELFDATA2MSB	2

#define EV_NONE		0		/* e_version, EI_VERSION */
#define EV_CURRENT	1
#define EV_NUM		2

#define ELFOSABI_NONE	0
#define ELFOSABI_LINUX	3

#define EM_RISC_V 243

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} Elf64_phdr_t;

// RISC-V-specific section types.
constexpr uint32_t SHT_RISC_V_ATTRIBUTES = 0x70000003;

// RISC-V-specific segment types.
constexpr uint32_t PT_RISC_V_ATTRIBUTES = 0x70000003; // meaning: RISC-V ELF attribute section.
