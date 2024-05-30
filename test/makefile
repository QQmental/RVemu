export PATH=$PATH:/opt/riscv/bin

target = ../test.elf ../test_dump

.PHONY: target clean

$(target): test.c test3.c
	riscv64-unknown-elf-gcc test.c -g -O0 -march=rv64imafc -mabi=lp64 -o ../test.elf
	riscv64-unknown-elf-objdump -d ../test.elf > ../test_dump

	riscv64-unknown-elf-gcc test3.c -g -O0 -march=rv64imafc -mabi=lp64 -o ../test3.elf
	riscv64-unknown-elf-objdump -d ../test3.elf > ../test3_dump

clean:
	$(shell rm ../test.elf ../test_dump)