int main(){

  asm volatile("li a0, -1");
  asm volatile("li a1, -1");
  //asm volatile("slli a0, a0, 37");
  //asm volatile("slli a1, a1, 37");
  //asm volatile("addi a0, a0, 7");
  //asm volatile("addi a1, a1, 3");
  asm volatile("mulh a2, a1, a0");
  asm volatile("addi a2, a2, 0");
  //asm volatile("unimp");  //In test code, this instructions is defined as the end of test

  unsigned long x = -1;
  long y = -1;
  unsigned long z = ((long)x) * y;

  return 0;
}