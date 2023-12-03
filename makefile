make = mingw32-make
cc = g++
CPP_FLAG = -std=c++17 -g -pedantic -Wparentheses -DXLEN=32 -Werror -MMD

SRCS = BUS.cpp main.cpp RISC_V_cmd.cpp RISC_V_emu.cpp RISC_V_load_guest.cpp \
RISC_V_unzip_instr.cpp

OBJS= $(SRCS:%.cpp=%.o)

DEPS = $(OBJS:%.o=%.d)

BIN = main.exe

RM = rm

ifeq ($(make), mingw32-make)
	RM = DEL
endif

.PHONY: clean all

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

-include $(DEPS)

clean:
	$(foreach dep, $(DEPS), $(RM) $(dep))
	$(foreach obj, $(OBJS), $(RM) $(obj))