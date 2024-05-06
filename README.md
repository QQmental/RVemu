# RVemu
support risc-v64IMC

# build tool chain
download Newlib, and build it
https://github.com/riscv-collab/riscv-gnu-toolchain

# build project
make
# build options
#enable debug flag  
make dbg=on  
  
#enable code coverage record  
make cov=on  

# run project
./main.exe

# create code coverage report
make gen_cov_reocrd
