#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <string.h>

#include "RISC_V_load_guest.h"
#include "RISC_V_emu.h"
#include "RISC_V_cpu_spec.h"

#include "elf.h"
#include "uleb128.h"

static bool Load_Elf_header(FILE *file_stream, Elf64_Ehdr *hdr);
static bool Load_phdr(FILE *file_stream, const Elf64_Ehdr &hdr, uint32_t ith, Elf64_phdr_t *phdr);
void nRISC_V_load_guest::Init_guest_segment_mapping(std::string program_name, Program_mdata_t &program_mdata, char* mem, uint8_t *&sh_RISC_V_attr);
void nRISC_V_load_guest::Init_guest_RISC_V_attributes(nRISC_V_cpu_spec::RISC_V_Attributes &attr, const uint8_t *RISC_V_attributes_section);
void nRISC_V_load_guest::Init_guest_RISC_V_attributes(nRISC_V_cpu_spec::RISC_V_Attributes &attr, const uint8_t *RISC_V_attributes_section);
static std::size_t Parse_uleb128(const uint8_t* src, std::size_t max_len, uint32_t &val);

constexpr uint32_t Tag_file = 1;
constexpr uint32_t Tag_RISCV_stack_align = 4;
constexpr uint32_t Tag_RISCV_arch = 5;
constexpr uint32_t Tag_RISCV_unaligned_access = 6;
constexpr uint32_t Tag_RISCV_atomic_abi = 14;
constexpr uint32_t Tag_RISCV_x3_reg_usage = 16;




static bool Load_Elf_header(FILE *file_stream, Elf64_Ehdr *hdr)
{
    auto f = std::fread(reinterpret_cast<void*>(hdr), 1, sizeof(Elf64_Ehdr), file_stream);

    if (f != sizeof(Elf64_Ehdr))
    {
        std::cout << "error: size of the read file is small than a elf64 header\n";
        return false;
    }

    if (*reinterpret_cast<uint32_t*>(hdr) != *reinterpret_cast<const uint32_t*>(ELFMAG))
    {
        std::cout << "error: probably the file has no elf header \n";
        return false;
    }

    if (hdr->e_machine != EM_RISC_V || hdr->e_ident[EI_CLASS] != ELFCLASS64) {
        std::cout << "only RISC_V64 elf file is supported\n";
        return false;
    }

    return true;
}

static bool Load_phdr(FILE *file_stream, const Elf64_Ehdr &hdr, uint32_t ith, Elf64_phdr_t *phdr)
{
    fseek(file_stream, hdr.e_phoff + hdr.e_phentsize * ith, SEEK_SET);
     
    auto f = fread(reinterpret_cast<void*>(phdr), 1, sizeof(Elf64_phdr_t), file_stream);

    if(f != sizeof(Elf64_phdr_t))
        return false;

    return true;
}

void nRISC_V_load_guest::Init_basic_CPU_attributes(std::string program_name, nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute)
{
    Elf64_Ehdr hdr;

    auto file_stream = std::fopen(program_name.c_str(), "rb");

    assert(file_stream != nullptr);

    auto f = Load_Elf_header(file_stream, &hdr);
    assert(f == true);

    if (hdr.e_ident[EI_CLASS] == ELFCLASS64)
        CPU_attribute.xlen = 64;
    else if (hdr.e_ident[EI_CLASS] == ELFCLASS32)
        CPU_attribute.xlen = 32;

    if (hdr.e_ident[EI_DATA] == ELFDATA2LSB)
        CPU_attribute.endian = nUtil::eEndian::little_endian;
    else if(hdr.e_ident[EI_DATA] == ELFDATA2MSB)
        CPU_attribute.endian = nUtil::eEndian::big_endian;
    else if (hdr.e_ident[EI_DATA] == ELFCLASSNONE)
        CPU_attribute.endian = nUtil::eEndian::other;
    else
    {
        printf("undifined encoding %s %d\n", __func__, __LINE__);
        abort();
    }      
}

void nRISC_V_load_guest::Init_guest_segment_mapping(std::string program_name, Program_mdata_t &program_mdata, char* mem, uint8_t *&sh_RISC_V_attr)
{
    Elf64_Ehdr hdr;

    auto file_stream = std::fopen(program_name.c_str(), "rb");

    assert(file_stream != nullptr);

    auto f = Load_Elf_header(file_stream, &hdr);
    assert(f == true);

    if (std::fseek(file_stream, hdr.e_phoff + hdr.e_phentsize * hdr.e_phnum, SEEK_SET) != 0)
        std::cout << "seek file failed. The file is too small to hold each program header table\n";
    
    Elf64_phdr_t phdr;

    program_mdata = {0};

    for (int i = 0; i < hdr.e_phnum; i++)
    {
        f = Load_phdr(file_stream, hdr, i, &phdr);
        assert(f == true);

        if (phdr.p_type == PT_LOAD)
        {
            if (program_mdata.segment_base == 0)
                program_mdata.segment_base = phdr.p_vaddr;

            program_mdata.brk_addr = phdr.p_vaddr + phdr.p_memsz;
           
            f = std::fseek(file_stream, phdr.p_offset, SEEK_SET);
            assert(f == 0);
            
            std::fread(reinterpret_cast<void*>(&mem[phdr.p_vaddr - program_mdata.segment_base]), phdr.p_memsz, 1, file_stream);
            
            //set memory to be 0 if its address exceeds p_filesz area
            memset(reinterpret_cast<void*>(&mem[phdr.p_vaddr - program_mdata.segment_base + phdr.p_filesz]), 
                   0,
                   phdr.p_memsz - phdr.p_filesz);
        }
        else if (phdr.p_type == PT_RISC_V_ATTRIBUTES)
        {
            f = std::fseek(file_stream, phdr.p_offset, SEEK_SET);

            sh_RISC_V_attr = new uint8_t[phdr.p_filesz];

            std::fread(reinterpret_cast<void*>(sh_RISC_V_attr), phdr.p_filesz, 1, file_stream);
        }
    }

    assert(hdr.e_entry > 0);

    program_mdata.entry_point = hdr.e_entry - program_mdata.segment_base; // initialize program counter

    std::fclose(file_stream);
}


static std::size_t Parse_uleb128(const uint8_t* src, std::size_t max_len, uint32_t &val)
{
    val = 0;
    return bfs::DecodeUleb128(src, max_len, &val);
}

// ref: https://github.com/RISC_V-non-isa/RISC_V-elf-psabi-doc/blob/master/RISC_V-elf.adoc#rv-section-type

// null-terminated byte string : (NTBS)

// The attributes section start with a format-version (uint8 = 'A')
// followed by vendor specific sub-section(s). 
// A sub-section starts with sub-section length (uint32), vendor name (NTBS)
// and one or more sub-sub-section(s).

// A sub-sub-section consists of a tag (uleb128), sub-sub-section 
// length (uint32) followed by actual attribute tag,value pair(s).

// If tag of an attribute is an odd number, then the value is 
// a null-terminated byte string (NTBS), 
// otherwise, it's a uleb128 encoded integer.

void nRISC_V_load_guest::Init_guest_RISC_V_attributes(nRISC_V_cpu_spec::RISC_V_Attributes &attr, const uint8_t *sh_RISC_V_attr)
{
    assert(sh_RISC_V_attr[0] == static_cast<uint8_t>('A'));

    uint32_t sub_section_length = 0;

    memcpy(&sub_section_length, &sh_RISC_V_attr[1], sizeof(uint32_t));
  
    std::string vendor_name = reinterpret_cast<const char*>(  sh_RISC_V_attr 
                                                            + sizeof(sh_RISC_V_attr[0]) 
                                                            + sizeof(sub_section_length));
    
    assert(vendor_name == "riscv");
  
    auto sub_sub_section_offset = sizeof(sh_RISC_V_attr[0]) 
                                + sizeof(sub_section_length) 
                                + vendor_name.length()      /*vendor_name name*/
                                + 1;                        /*NULL character */
    
    uint32_t fst_sub_sub_section_tag = 0, fst_sub_sub_section_tag_len = 0;

    fst_sub_sub_section_tag_len = Parse_uleb128(sh_RISC_V_attr+sub_sub_section_offset , 5, fst_sub_sub_section_tag);

    uint32_t sub_sub_section_len = 0;
    assert(fst_sub_sub_section_tag == Tag_file);

    memcpy(&sub_sub_section_len, sh_RISC_V_attr + sub_sub_section_offset + fst_sub_sub_section_tag_len, sizeof(uint32_t));

    auto cur_actual_attr_offset = sub_sub_section_offset + fst_sub_sub_section_tag_len + sizeof(sub_sub_section_len);

    // smaller than file size of RISC_V_attributes segment
    while(cur_actual_attr_offset < sizeof(sh_RISC_V_attr[0]) + sub_section_length)
    {
        uint32_t sub_sub_section_tag = 0;

        auto sub_sub_section_tag_len 
        = Parse_uleb128(sh_RISC_V_attr + cur_actual_attr_offset , 5, sub_sub_section_tag);

        cur_actual_attr_offset += sub_sub_section_tag_len;
        
        std::size_t tag_val_len = 0;
        
        switch(sub_sub_section_tag)
        {
            case Tag_RISCV_stack_align:
                 attr.Tag_RISCV_stack_align.first = true;
                 tag_val_len = Parse_uleb128(sh_RISC_V_attr + cur_actual_attr_offset, 5, attr.Tag_RISCV_stack_align.second);
            break;

            case Tag_RISCV_arch:
                 attr.Tag_RISCV_arch.first = true;            
                 attr.Tag_RISCV_arch.second = reinterpret_cast<const char*>(sh_RISC_V_attr + cur_actual_attr_offset);
                 tag_val_len = attr.Tag_RISCV_arch.second.length() + 1;
            break;

            case Tag_RISCV_unaligned_access:
                 attr.Tag_RISCV_unaligned_access.first = true;                  
                 tag_val_len = Parse_uleb128(sh_RISC_V_attr + cur_actual_attr_offset, 5, attr.Tag_RISCV_unaligned_access.second);           
            break;

            case Tag_RISCV_atomic_abi:
                 attr.Tag_RISCV_atomic_abi.first = true;                        
                 tag_val_len = Parse_uleb128(sh_RISC_V_attr + cur_actual_attr_offset, 5, attr.Tag_RISCV_atomic_abi.second);
            break;                

            case Tag_RISCV_x3_reg_usage:  
                 attr.Tag_RISCV_x3_reg_usage.first = true;                        
                 tag_val_len = Parse_uleb128(sh_RISC_V_attr + cur_actual_attr_offset, 5, attr.Tag_RISCV_x3_reg_usage.second); 
            break;             
            
            default :
                printf("??? tag %d cur_offset %d\n", sub_sub_section_tag, cur_actual_attr_offset);
                assert(0);
        }
        cur_actual_attr_offset += tag_val_len;
    }
}


 

