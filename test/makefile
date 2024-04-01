export PATH=$PATH:/opt/riscv/bin

target = ../test.exe ../test_dump

.PHONY: target clean

$(target): test.c
	riscv64-unknown-elf-gcc test.c -g -march=rv64imafc -mabi=lp64 -o ../test.exe
	riscv64-unknown-elf-objdump -d ../test.exe > ../test_dump

clean:
	$(shell rm ../test.exe ../test_dump)