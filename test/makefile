export PATH+=$PATH:/opt/riscv/bin

target = ../test.elf ../test_dump

.PHONY: target clean

$(target): test.c example.c
	riscv64-unknown-elf-gcc test.c -g -O0 -march=rv64imafc -mabi=lp64 -o ../test.elf
	riscv64-unknown-elf-objdump -d ../test.elf > ../test_dump

	riscv64-unknown-elf-gcc example.c -g -O0 -march=rv64imafc -mabi=lp64 -o ../example.elf
	riscv64-unknown-elf-objdump -d ../example.elf > ../example_dump

clean:
	$(shell rm -rf ../test.elf ../test_dump)
	$(shell rm -rf ../example.elf ../example_dump)
