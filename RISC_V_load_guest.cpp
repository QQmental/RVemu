#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <string.h>

#include "RISC_V_load_guest.h"
#include "RISC_V_cpu_spec.h"
#include "program_mdata.h"

#include "elf.h"
#include "uleb128.h"

static bool Load_phdr(FILE *file_stream, const Elf64_Ehdr &hdr, uint32_t ith, Elf64_phdr_t *phdr);
static void Init_guest_segment_mapping(std::string program_name, nProgram_mdata::Program_mdata_t &program_mdata, char *mem, std::unique_ptr<uint8_t[]> &sh_RISC_V_attr);
static void Init_guest_RISC_V_attributes(nRISC_V_cpu_spec::RISC_V_Attributes &attr, const uint8_t *RISC_V_attributes_section);
static std::size_t Parse_uleb128(const uint8_t* src, std::size_t max_len, uint32_t &val);
static void Init_basic_CPU_attributes(std::string program_name, const nProgram_mdata::Program_mdata_t &program_mdata, nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute);



constexpr uint32_t Tag_file = 1;
constexpr uint32_t Tag_RISCV_stack_align = 4;
constexpr uint32_t Tag_RISCV_arch = 5;
constexpr uint32_t Tag_RISCV_unaligned_access = 6;
//Deprecated
constexpr uint32_t Tag_RISCV_priv_spec = 8;
//Deprecated
constexpr uint32_t Tag_RISCV_priv_spec_minor = 10;
//Deprecated
constexpr uint32_t Tag_RISCV_priv_spec_revision = 12;
constexpr uint32_t Tag_RISCV_atomic_abi = 14;
constexpr uint32_t Tag_RISCV_x3_reg_usage = 16;




bool nRISC_V_load_guest::Load_Elf_header(FILE *file_stream, Elf64_Ehdr *hdr)
{
    auto f = std::fread(reinterpret_cast<void*>(hdr), 1, sizeof(Elf64_Ehdr), file_stream);

    if (f != sizeof(Elf64_Ehdr))
    {
        std::cout << "error: size of the read file is small than a elf64 header\n";
        return false;
    }
    
    char magic_arr[4];
    memcpy(magic_arr, hdr, sizeof(magic_arr));

    if (   (magic_arr[0] != '\177') 
        || (magic_arr[1] != 'E') 
        || (magic_arr[2] != 'L') 
        || (magic_arr[3] != 'F'))
    {
        std::cout << "error: probably the file has no elf header \n";
        return false;
    }

    if (hdr->e_machine != EM_RISC_V || hdr->e_ident[EI_CLASS] != ELFCLASS64) {
        std::cout << "only RISC_V64 elf file is supported\n";
        return false;
    }

    if (std::fseek(file_stream, hdr->e_phoff + hdr->e_phentsize * hdr->e_phnum, SEEK_SET) != 0)
        std::cout << "seek file failed. The file is too small to hold each program header table\n";

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

std::size_t nRISC_V_load_guest::Get_least_memory_needed(const char *program_name)
{
    Elf64_Ehdr elf_hdr;

    auto file_stream = std::fopen(program_name, "rb");

    auto f = nRISC_V_load_guest::Load_Elf_header(file_stream, &elf_hdr);
    assert(f == true);

    Elf64_phdr_t phdr;

    nRISC_V_cpu_spec::RISC_V_Addr_t max{0}, min{0};

    for (int i = 0; i < elf_hdr.e_phnum; i++)
    {
        auto f = Load_phdr(file_stream, elf_hdr, i, &phdr);
        assert(f == true);

        if (phdr.p_type == PT_LOAD)
        {
            if (min == 0)
                min = phdr.p_vaddr;

            max = phdr.p_vaddr + phdr.p_memsz;
        }
    }

    return max - min;
}

// an array of uint8_t[] is allocted to save RISC_V_attr
// program_mdata.highest_addr is set to be the highest addr among those loaded segment
// a piece of memory is allocated for 'sh_RISC_V_attr' when a segment of type 'PT_RISC_V_ATTRIBUTES' exists.
static void Init_guest_segment_mapping(std::string program_name, nProgram_mdata::Program_mdata_t &program_mdata, char *mem, std::unique_ptr<uint8_t[]> &sh_RISC_V_attr)
{
    Elf64_Ehdr hdr;

    auto file_stream = std::fopen(program_name.c_str(), "rb");

    assert(file_stream != nullptr);

    [[maybe_unused]] auto f = nRISC_V_load_guest::Load_Elf_header(file_stream, &hdr);
    assert(f == true);

    if (hdr.e_type != ET_EXEC)
        nUtil::FATAL("input program should be an executable file\n");

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
            
            auto sz = std::fread(reinterpret_cast<void*>(&mem[phdr.p_vaddr - program_mdata.segment_base]), phdr.p_memsz, 1, file_stream);
            assert(sz != 0);

            //set memory to be 0 if its address exceeds p_filesz area
            memset(reinterpret_cast<void*>(&mem[phdr.p_vaddr - program_mdata.segment_base + phdr.p_filesz]), 
                   0,
                   phdr.p_memsz - phdr.p_filesz);

            program_mdata.highest_addr = phdr.p_vaddr;
        }
        else if (phdr.p_type == PT_RISC_V_ATTRIBUTES)
        {
            f = std::fseek(file_stream, phdr.p_offset, SEEK_SET);

            sh_RISC_V_attr = std::make_unique<uint8_t[]>(phdr.p_filesz);

            auto sz = std::fread(reinterpret_cast<void*>(sh_RISC_V_attr.get()), phdr.p_filesz, 1, file_stream);

            assert(sz != 0);
        }
    }

    assert(hdr.e_entry > 0);

    program_mdata.entry_point = hdr.e_entry; // initialize program counter

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

static void Init_guest_RISC_V_attributes(nRISC_V_cpu_spec::RISC_V_Attributes &attr, const uint8_t *sh_RISC_V_attr)
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
            // used for read Deprecated tag
            uint32_t garbage;

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

            case Tag_RISCV_priv_spec:
            case Tag_RISCV_priv_spec_minor:
            case Tag_RISCV_priv_spec_revision:
                 tag_val_len = Parse_uleb128(sh_RISC_V_attr + cur_actual_attr_offset, 5, garbage);
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
                printf("??? tag %d cur_offset %lu\n", sub_sub_section_tag, cur_actual_attr_offset);
                assert(0);
        }
        cur_actual_attr_offset += tag_val_len;
    }
}

static void Init_basic_CPU_attributes(std::string program_name, const nProgram_mdata::Program_mdata_t &program_mdata, nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute)
{
    Elf64_Ehdr hdr;

    auto file_stream = std::fopen(program_name.c_str(), "rb");

    assert(file_stream != nullptr);

    [[maybe_unused]] auto f = nRISC_V_load_guest::Load_Elf_header(file_stream, &hdr);
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

void nRISC_V_load_guest::Load_guest_program(Loaded_guest_configure &config)
{
    std::unique_ptr<uint8_t[]> sh_RISC_V_attr {};
    
    auto least_space_needed =  nRISC_V_load_guest::Get_least_memory_needed(config.program_name.c_str());
    config.mem = std::unique_ptr<char[]>(new char[config.runtime_data_space + config.reserved_space + least_space_needed]);

    Init_guest_segment_mapping(config.program_name, config.program_mdata, config.mem.get(), sh_RISC_V_attr);
    Init_guest_RISC_V_attributes(config.program_mdata.CPU_attributes.RISC_V_attributes, sh_RISC_V_attr.get());
    
    if (config.program_mdata.CPU_attributes.RISC_V_attributes.Tag_RISCV_stack_align.first == true)
        config.program_mdata.stack_pointer_alignment = config.program_mdata.CPU_attributes.RISC_V_attributes.Tag_RISCV_stack_align.second;
    else
        config.program_mdata.stack_pointer_alignment = 16;

    config.program_mdata.highest_addr = (config.program_mdata.highest_addr + config.runtime_data_space + config.reserved_space) & (~(config.program_mdata.stack_pointer_alignment - 1));

    Init_basic_CPU_attributes(config.program_name, config.program_mdata, config.program_mdata.CPU_attributes);
}