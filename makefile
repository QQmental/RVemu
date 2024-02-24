make = make
cc = g++
CPP_FLAG = -std=c++17 -g -pedantic -Wparentheses -fsanitize=undefined -DXLEN=64 -Werror -MMD -Wall

SRCS = BUS.cpp main.cpp RISC_V_cmd.cpp RISC_V_emu.cpp RISC_V_load_guest.cpp \
RISC_V_unzip_instr.cpp Syscall.cpp

OBJS= $(SRCS:%.cpp=%.o)

DEPS = $(OBJS:%.o=%.d)

BIN = main.exe

RM = rm -rf

ifeq ($(make), mingw32-make)
	RM = DEL
endif

.PHONY: clean all add_test_src

all:$(OBJS) $(BIN)
	

$(BIN) : $(OBJS)
	$(cc) $(OBJS) $(CPP_FLAG)  -o $@

BUS.o :BUS.cpp
	$(cc) $< $(CPP_FLAG) -c -o $@

main.o :main.cpp
	$(cc) $< $(CPP_FLAG) -c -o $@

RISC_V_cmd.o :RISC_V_cmd.cpp
	$(cc) $< $(CPP_FLAG) -c -o $@

RISC_V_emu.o :RISC_V_emu.cpp
	$(cc) $< $(CPP_FLAG) -c -o $@

RISC_V_load_guest.o :RISC_V_load_guest.cpp
	$(cc) $< $(CPP_FLAG) -c -o $@

RISC_V_unzip_instr.o :RISC_V_unzip_instr.cpp
	$(cc) $< $(CPP_FLAG) -c -o $@

Syscall.o :Syscall.cpp
	$(cc) $< $(CPP_FLAG) -c -o $@

-include $(DEPS)

clean:
	$(foreach dep, $(DEPS), $(RM) $(dep);)
	$(foreach obj, $(OBJS), $(RM) $(obj);)

add_test_src:
	make -C ./test