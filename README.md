# RVemu
support risc-v64IMC

# build tool chain
https://github.com/riscv-collab/riscv-gnu-toolchain  
download Newlib, and build it  


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
