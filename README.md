# RVemu
support risc-v64IMC

# build toolchain
https://github.com/riscv-collab/riscv-gnu-toolchain  
download Newlib, and build it  


# build project
after building the toolchain, then
make
# build options
#enable debug flag  
make dbg=on  
  
#enable code coverage record  
make cov=on  

# run project
./main.exe path_to_your_emulated_program

# create code coverage report
make gen_cov_reocrd
