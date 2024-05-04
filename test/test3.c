#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int int_cmp(const void *p1, const void *p2)
{
    if (*(int*)p1 > *(int*)p2)
        return -1;
    else if (*(int*)p1 < *(int*)p2)
        return 1;
    else
        return 0;
}

int int64_cmp(const void *p1, const void *p2)
{
    if (*(int64_t*)p1 > *(int64_t*)p2)
        return -1;
    else if (*(int64_t*)p1 < *(int64_t*)p2)
        return 1;
    else
        return 0;
}

int main(int argc, char* argv[])
{
{
    int64_t *ptr = malloc(sizeof(int64_t) * 32);
    for(int64_t *p = ptr ; p - ptr < 32 ; p++)
        *p = p-ptr - 10000000000;
    qsort(ptr, 32, sizeof(int64_t), int_cmp);

    for(int64_t *p = ptr, x = 31- 10000000000 ; p - ptr < 32 ; p++, x--)
    {
        if (*p != x)
            printf("??? %d %d\n", *p, p-ptr);
    }   
}
{
    int *ptr = malloc(sizeof(int) * 32);
    for(int *p = ptr ; p - ptr < 32 ; p++)
        *p = p-ptr;
    qsort(ptr, 32, sizeof(int), int_cmp);

    for(int *p = ptr, x =31 ; p - ptr < 32 ; p++, x--)
    {
        if (*p != x)
            printf("??? %d %d\n", *p, p-ptr);
    }
}
    int len = 10;
    int arr[len];
    for(int i = 0 ; i < argc ; i++)
        printf("%s\n",argv[i]);

}
