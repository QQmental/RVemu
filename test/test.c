#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define dbg 1

#if dbg

int64_t fibo(int64_t num)
{
	if (num == 0 || num == 1)
		return 1;
	return fibo(num-1) + fibo(num-2);
}

uint64_t func2(uint64_t num)
{
	for(int i = 0 ; i < 100 ; i++)
		num ^= i;
	return num;
}

uint64_t func3_64(uint64_t num1, uint64_t num2, uint64_t num3)
{
	uint64_t *p1 = malloc(sizeof(uint64_t)), *p2 = malloc(sizeof(uint64_t)), *p3 = malloc(sizeof(uint64_t));

	*p1 = num1, *p2 = num2, *p3 = num3;
	uint64_t ret = *p1 + *p2 + *p3;
	
	free(p1);
	free(p2);
	free(p3);

	return ret;
}

uint32_t func3_32(uint32_t num1, uint32_t num2, uint32_t num3)
{
	uint32_t *p1 = malloc(sizeof(uint32_t)), *p2 = malloc(sizeof(uint32_t)), *p3 = malloc(sizeof(uint32_t));

	*p1 = num1, *p2 = num2, *p3 = num3;
	uint32_t ret = *p1 + *p2 + *p3;
	
	free(p1);
	free(p2);
	free(p3);

	return ret;
}

uint16_t func3_16(uint16_t num1, uint16_t num2, uint16_t num3)
{
	uint16_t *p1 = malloc(sizeof(uint16_t)), *p2 = malloc(sizeof(uint16_t)), *p3 = malloc(sizeof(uint16_t));

	*p1 = num1, *p2 = num2, *p3 = num3;
	uint16_t ret = *p1 + *p2 + *p3;
	
	free(p1);
	free(p2);
	free(p3);

	return ret;
}

uint8_t func3_8(uint8_t num1, uint8_t num2, uint8_t num3)
{
	uint8_t *p1 = malloc(sizeof(uint8_t)), *p2 = malloc(sizeof(uint8_t)), *p3 = malloc(sizeof(uint8_t));

	*p1 = num1, *p2 = num2, *p3 = num3;
	uint8_t ret = *p1 + *p2 + *p3;
	
	free(p1);
	free(p2);
	free(p3);

	return ret;
}

void write_mem(void *dst, void *src, size_t size)
{
	memcpy(dst, src, size);
}

uint64_t func4()
{
	int64_t num2 = fibo(10);
	if (num2 != 89)
		printf("shof\n");
	for(uint32_t i = 1 ; i < (1<<15) ; i+=16)
	{
		if ((i | 0) == 0)
			printf("shof\n");//abort();
		if ((i | i) != i)
			printf("shof\n");//abort();

		if ((i ^ 0) != i)
			printf("shof\n");//abort();
		if ((i ^ -1) != ~i)
			printf("shof\n");//abort();
		
		if ((i & 0) != 0)
			printf("shof\n");//abort();
		if ((i & -1) != i)
			printf("shof\n");//abort();
	}
	for(int i = -100 ; i < 100 ; i++)
	{
		if (func3_64(1, 2, -10000) != -9997)
			printf("shof\n");//abort();

		if (func3_64(100, 200, -10000) != -9700)
			printf("shof\n");//abort(); 

		{
			int a = 100, b = 200;
			if (func3_64(a, b, i) != 300 + i)
				printf("shof\n");//abort(); 
		}


		if (func3_32(1, 2, 3) != 6)
			printf("shof\n");//abort();

		if (func3_32(100, 200, 300) != 600)
			printf("shof\n");//abort();

		{
			int a = 100, b = 200;
			if (func3_32(a, b, i) != 300+i)
				printf("shof\n");//abort();
		}

		if (func3_16(1, 2, 3) != 6)
			printf("shof\n");//abort();

		if (func3_16(100, 200, 300) != 600)
			printf("shof\n");//abort();

		{
			int a = 100, b = 200;
			if (func3_16(a, b, i) != 300+i)
				printf("shof\n");//abort();
		}

		if (func3_8(1, 2, 3) != 6)
			printf("shof\n");//abort();

		if (func3_8(10, 20, 30) != 60)
			printf("shof\n");//abort();

		{
			int a = 10, b = 20, c = 30;
			if (func3_8(a, b, c) != 60)
				printf("shof\n");//abort();
		}
	} 
	return 0;
}

void func5()
{
	typedef int32_t item_t;

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)%(16+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)%(16+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)/(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)/(1+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)^(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if ((*c) != ((c-ptr)^(1+i)))
				printf("shof\n");

		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)|(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)|(1+i)))
				printf("shof\n");
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)&(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)&(1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * -i % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * -i % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * 5 % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * 5 % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8)));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8))))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if ((c-ptr) < i)
				*c = 1;
			else
				*c = 0;
		}

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			item_t tmp;
			if ((c-ptr) < i)
				tmp = 1;
			else
				tmp = 0;

			if (*c != tmp)
				printf("shof\n");
		}
		free(ptr);
	}

}

void func6()
{
	typedef uint64_t item_t;

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)%(16+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)%(16+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)/(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)/(1+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)^(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)^(1+i)))
				printf("shof\n");// abort();
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)|(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)|(1+i)))
				printf("shof\n");
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)&(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)&(1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * i % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * i % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * 5 % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * 5 % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8)));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8))))
				printf("shof\n");
		}
		free(ptr);
	}
	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if ((c-ptr) % 2 == 0)
				*c = 1;
			else
				*c = 0;
		}

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			item_t tmp;
			if ((c-ptr) % 2 == 0)
				tmp = 1;
			else
				tmp = 0;

			if (*c != tmp)
				printf("shof\n");
		}
		free(ptr);
	}
}

void func7()
{
	typedef int64_t item_t;

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)%(16+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)%(16+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)/(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)/(1+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)^(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)^(1+i)))
				printf("shof\n");// abort();
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)|(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)|(1+i)))
				printf("shof\n");
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)&(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)&(1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * i % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * i % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * 5 % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * 5 % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8)));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8))))
				printf("shof\n");
		}
		free(ptr);
	}
	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if ((c-ptr) < i)
				*c = 1;
			else
				*c = 0;
		}

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			item_t tmp;
			if ((c-ptr) < i)
				tmp = 1;
			else
				tmp = 0;

			if (*c != tmp)
				printf("shof\n");
		}
		free(ptr);
	}
}

void func8()
{
	typedef int8_t item_t;

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)%(16+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)%(16+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)/(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)/(1+i))
				printf("shof\n");//abort();abort();
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)^(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)^(1+i)))
				printf("shof\n");// abort();
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)|(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)|(1+i)))
				printf("shof\n");
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)&(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)&(1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * i % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * i % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * 5 % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * 5 % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8)));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8))))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if ((c-ptr) < i)
				*c = 1;
			else
				*c = 0;
		}

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			item_t tmp;
			if ((c-ptr) < i)
				tmp = 1;
			else
				tmp = 0;

			if (*c != tmp)
				printf("shof\n");
		}
		free(ptr);
	}
}

void func9()
{
	typedef int16_t item_t;

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)%(16+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)%(16+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)/(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != (c-ptr)/(1+i))
				printf("shof\n");//abort();
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)^(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)^(1+i)))
				printf("shof\n");// abort();
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)|(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)|(1+i)))
				printf("shof\n");
		}
		free(ptr);
	} 

 	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = (c-ptr)&(1+i);

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr)&(1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * i % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * i % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) * 5 % (1+i));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) * 5 % (1+i)))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = 0 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
			*c = ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8)));

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if (*c != ((c-ptr) >> ((1+i)%(sizeof(*ptr) * 8))))
				printf("shof\n");
		}
		free(ptr);
	}

	for(int i = -100 ; i < 31 ; i++)
	{
		item_t *ptr = malloc(sizeof(item_t) * 128);
		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			if ((c-ptr) < i)
				*c = 1;
			else
				*c = 0;
		}

		for(item_t *c = ptr ; c - ptr < 128 ; c++)
		{
			item_t tmp;
			if ((c-ptr) < i)
				tmp = 1;
			else
				tmp = 0;

			if (*c != tmp)
				printf("shof\n");
		}
		free(ptr);
	}
}
#endif

int main(int argc, char* argv[])
{
#if dbg	
	for(int i = 0 ; i < 10 ; i++)
	{
		void *p = malloc(i+1);

		free(p);
	}
  	func4();
 	func5();
 	func6();
	func7();
	func8();
	func9();  
	

	char *ptr = malloc(256);
#endif


	int num = 0x94879487;
	char *str = "Hello, this is the emulated world!";

	printf("%x\n", num);
	//printf("%d %d", 25, 25);
	int x = atoi("1234");
	char arr1[16] = {"aaaaabbbbbcccc"}, arr2[16]= {"cccccbbbbbaaaaa"};

	//strcpy(arr1, arr2);
	char *p1 = strdup(arr1);
	printf("%s %s\n",arr1, arr2);	
	printf("%s %s %s\n", strdup(arr1), strdup(arr2), argv[0]);
	//printf("%d\n", 0x78787878);
	


	return 0;
}
