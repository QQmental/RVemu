#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <array>
#include <unordered_map>

#include "Syscall.h"
#include "syscall_number.h"
#include "BUS.h"


// slightly modified from https://github.com/riscv-software-src/riscv-pk

#define GET(reg_num) exec_compnent.regs.gp_regs[nRISC_V_cpu_spec::gp_reg_abi_name:: reg_num];

using syscall_t = uint64_t(*)(nRISC_V_cmd::Exec_component &, Program_mdata_t &);

static uint64_t sys_unimplemented(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    FATALF("unimplemented syscall: %lu", exec_compnent.regs.gp_regs[nRISC_V_cpu_spec::gp_reg_abi_name::a7]);
    return 0;    
}

static uint64_t sys_exit(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto code = GET(a0);
    
    exit(code);
}

static uint64_t sys_close(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto fd = GET(a0);

    if (fd > 2) 
        return close(fd);
    return 0;
}

static uint64_t sys_write(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata) 
{
    auto fd = GET(a0);
    auto ptr = GET(a1);
    auto len = GET(a2);
    
    auto host_ptr = exec_compnent.bus.Get_raw_ptr(ptr);
    
    return write(fd, host_ptr, (size_t)len);
}

static uint64_t sys_fstat(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto fd = GET(a0); 
    auto addr = GET(a1);
    return fstat(fd, new(exec_compnent.bus.Get_raw_ptr(addr))struct stat);
}

static uint64_t sys_gettimeofday(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto tv_addr = GET(a0); 
    auto tz_addr = GET(a1);
    auto *tv = new(exec_compnent.bus.Get_raw_ptr(tv_addr))struct timeval;
    struct timezone *tz = nullptr;
    
    if (tz_addr != 0) 
        tz = new(exec_compnent.bus.Get_raw_ptr(tz_addr))struct timezone;
        
    return gettimeofday(tv, tz);
}

static uint64_t sys_brk(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto addr = GET(a0);

    if (addr == 0)
        addr = program_mdata.brk_addr;
    assert(addr >= program_mdata.segment_base);
    printf("addr: %lu \n",addr);
    program_mdata.brk_addr = addr;

    return addr;
}

// the O_* macros is OS dependent.
// here is a workaround to convert newlib flags to the host.
#define NEWLIB_O_RDONLY   0x0
#define NEWLIB_O_WRONLY   0x1
#define NEWLIB_O_RDWR     0x2
#define NEWLIB_O_APPEND   0x8
#define NEWLIB_O_CREAT  0x200
#define NEWLIB_O_TRUNC  0x400
#define NEWLIB_O_EXCL   0x800
#define REWRITE_FLAG(flag) if (flags & NEWLIB_ ##flag) hostflags |= flag;

static int convert_flags(int flags) {
    int hostflags = 0;
    REWRITE_FLAG(O_RDONLY);
    REWRITE_FLAG(O_WRONLY);
    REWRITE_FLAG(O_RDWR);
    REWRITE_FLAG(O_APPEND);
    REWRITE_FLAG(O_CREAT);
    REWRITE_FLAG(O_TRUNC);
    REWRITE_FLAG(O_EXCL);
    return hostflags;
}

static uint64_t sys_openat(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto dirfd = GET(a0); 
    auto nameptr = GET(a1); 
    auto flags = GET(a2); 
    auto mode = GET(a3);
    return openat(dirfd, (char *)exec_compnent.bus.Get_raw_ptr(nameptr), convert_flags(flags), mode);
}

static uint64_t sys_open(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto nameptr = GET(a0); 
    auto flags = GET(a1);
    auto mode = GET(a2);
    auto ret = open((char *)exec_compnent.bus.Get_raw_ptr(nameptr), convert_flags(flags), (mode_t)mode);
    return ret;
}

static uint64_t sys_lseek(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto fd = GET(a0); 
    auto offset = GET(a1);
    auto whence = GET(a2);
    return lseek(fd, offset, whence);
}

static uint64_t sys_read(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    auto fd = GET(a0);
    auto bufptr = GET(a1);
    auto count = GET(a2);
    return read(fd, (char *)exec_compnent.bus.Get_raw_ptr(bufptr), (size_t)count);
}
static const std::unordered_map<uint64_t, syscall_t> syscall_table = 
{
    std::make_pair(SYS_exit, sys_exit),
    std::make_pair(SYS_exit_group, sys_exit),
    std::make_pair(SYS_read, sys_read),
    std::make_pair(SYS_pread, sys_unimplemented),
    std::make_pair(SYS_write, sys_write),
    std::make_pair(SYS_openat, sys_openat),
    std::make_pair(SYS_close, sys_close),
    std::make_pair(SYS_fstat, sys_fstat),
    std::make_pair(SYS_statx, sys_unimplemented),
    std::make_pair(SYS_lseek, sys_lseek),
    std::make_pair(SYS_fstatat, sys_unimplemented),
    std::make_pair(SYS_linkat, sys_unimplemented),
    std::make_pair(SYS_unlinkat, sys_unimplemented),
    std::make_pair(SYS_mkdirat, sys_unimplemented),
    std::make_pair(SYS_renameat, sys_unimplemented),
    std::make_pair(SYS_getcwd, sys_unimplemented),
    std::make_pair(SYS_brk, sys_brk),
    std::make_pair(SYS_uname, sys_unimplemented),
    std::make_pair(SYS_getpid, sys_unimplemented),
    std::make_pair(SYS_getuid, sys_unimplemented),
    std::make_pair(SYS_geteuid, sys_unimplemented),
    std::make_pair(SYS_getgid, sys_unimplemented),
    std::make_pair(SYS_getegid, sys_unimplemented),
    std::make_pair(SYS_gettid, sys_unimplemented),
    std::make_pair(SYS_tgkill, sys_unimplemented),
    std::make_pair(SYS_mmap, sys_unimplemented),
    std::make_pair(SYS_munmap, sys_unimplemented),
    std::make_pair(SYS_mremap, sys_unimplemented),
    std::make_pair(SYS_mprotect, sys_unimplemented),
    std::make_pair(SYS_rt_sigaction, sys_unimplemented),
    std::make_pair(SYS_gettimeofday, sys_gettimeofday),
    std::make_pair(SYS_times, sys_unimplemented),
    std::make_pair(SYS_writev, sys_unimplemented),
    std::make_pair(SYS_faccessat, sys_unimplemented),
    std::make_pair(SYS_fcntl, sys_unimplemented),
    std::make_pair(SYS_ftruncate, sys_unimplemented),
    std::make_pair(SYS_getdents, sys_unimplemented),
    std::make_pair(SYS_dup, sys_unimplemented),
    std::make_pair(SYS_dup3, sys_unimplemented),
    std::make_pair(SYS_rt_sigprocmask, sys_unimplemented),
    std::make_pair(SYS_clock_gettime, sys_unimplemented),
    std::make_pair(SYS_chdir, sys_unimplemented),
    std::make_pair(SYS_open, sys_open),
    std::make_pair(SYS_link, sys_unimplemented),
    std::make_pair(SYS_unlink, sys_unimplemented),
    std::make_pair(SYS_mkdir, sys_unimplemented),
    std::make_pair(SYS_access, sys_unimplemented),
    std::make_pair(SYS_stat, sys_unimplemented),
    std::make_pair(SYS_lstat, sys_unimplemented),
    std::make_pair(SYS_time, sys_unimplemented)
};

uint64_t nSyscall::Do_syscall(nRISC_V_cmd::Exec_component &exec_compnent, Program_mdata_t &program_mdata)
{
    syscall_t f = nullptr;

    auto syscall_num = GET(a7);

    auto it = syscall_table.find(syscall_num);

    if (it == syscall_table.end()) 
        nUtil::FATAL("unknown syscall");

    f = it->second;

    return f(exec_compnent, program_mdata);
    
}
