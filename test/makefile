export PATH=$PATH:/opt/riscv/bin

target = ../test.exe ../test_dump

.PHONY: target clean

$(target): test.c test3.c
	riscv64-unknown-elf-gcc test.c -g -O0 -march=rv64imafc -mabi=lp64 -o ../test.exe
	riscv64-unknown-elf-objdump -d ../test.exe > ../test_dump

	riscv64-unknown-elf-gcc test3.c -g -O0 -march=rv64imafc -mabi=lp64 -o ../test3.exe
	riscv64-unknown-elf-objdump -d ../test3.exe > ../test3_dump

clean:
	$(shell rm ../test.exe ../test_dump)