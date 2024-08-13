make = make
Build = build

EXTENSION_flag = -DRISC_V_EXT_ZICSR=1

cc = g++
CPP_FLAG = -std=c++17 -pedantic -Wparentheses -DXLEN=64 -MMD -Wall $(EXTENSION_flag)
LINK =

# debug flag or not
dbg = off
ifeq ($(dbg), on)
	CPP_FLAG += -g -fsanitize=undefined
else ifeq ($(dbg), off)
	CPP_FLAG += -O2 -DNDEBUG
else
    $(error dbg can't be '$(dbg)', it should be either 'on' or 'off')
endif

# add coverage record or not
cov = off
ifeq ($(cov), on)
	CPP_FLAG += -fprofile-arcs -ftest-coverage
	LINK += -lgcov
endif


SRCS = BUS.cpp main.cpp RISC_V_cmd.cpp RISC_V_emu.cpp RISC_V_load_guest.cpp \
RISC_V_unzip_instr.cpp Syscall.cpp

OBJS= $(SRCS:%.cpp=$(Build)/%.o)

DEPS = $(OBJS:%.o=%.d)

BIN = main.exe

INCLUDE = -Iinclude

RM = rm -rf

ifeq ($(make), mingw32-make)
	RM = DEL
endif

.PHONY: clean all add_test_src gen_cov_record run_test

all:$(Build) $(OBJS) $(BIN)
	make add_test_src

$(BIN) : $(OBJS)
	$(cc) $(OBJS) $(CPP_FLAG) $(LINK) -o $@

$(Build):
	$(shell mkdir -p $(Build))

$(Build)/BUS.o :./src/BUS.cpp
	$(cc) $< $(INCLUDE) $(CPP_FLAG) -c -o $@

$(Build)/main.o :src/main.cpp
	$(cc) $< $(INCLUDE) $(CPP_FLAG) -c -o $@

$(Build)/RISC_V_cmd.o :src/RISC_V_cmd.cpp
	$(cc) $< $(INCLUDE) $(CPP_FLAG) -c -o $@

$(Build)/RISC_V_emu.o :src/RISC_V_emu.cpp
	$(cc) $< $(INCLUDE) $(CPP_FLAG) -c -o $@

$(Build)/RISC_V_load_guest.o :src/RISC_V_load_guest.cpp
	$(cc) $< $(INCLUDE) $(CPP_FLAG) -c -o $@

$(Build)/RISC_V_unzip_instr.o :src/RISC_V_unzip_instr.cpp
	$(cc) $< $(INCLUDE) $(CPP_FLAG) -c -o $@

$(Build)/Syscall.o :src/Syscall.cpp
	$(cc) $< $(INCLUDE) $(CPP_FLAG) -c -o $@

-include $(DEPS)

clean:
	make clean -C ./test
	$(foreach dep, $(DEPS), $(RM) $(dep);)
	$(foreach obj, $(OBJS), $(RM) $(obj);)
	$(RM) $(wildcard ./*.exe) *.gcno *.gcda
	rm -rf result $(Build)

run_test:
	python3 ./run_test.py

add_test_src:
	make -C ./test

gen_cov_record:
	lcov -c -d . -o test.info --rc lcov_branch_coverage=1
	genhtml --branch-coverage -o result test.info
