#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

int64_t fibo(int64_t num)
{
	if (num == 0 || num == 1)
		return num;
	return fibo(num-1) + fibo(num-2);
}

int main()
{
	/*for(int i = 0 ; i < 10 ; i++)
	{
		void *p = malloc(100);
		free(p);
	}*/
	int64_t num = fibo(10);
	printf("Welcome to the emulated world!\n");
	return 255;
}
