#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
int main()
{
	for(int i = 0 ; i < 10 ; i++)
	{
		void *p = malloc(100);
		free(p);
	}
	//printf("Welcome to the emulated world!\n");
	return 255;
}
