#include <stdio.h>

int main()
{
	printf("%lx\n", ~0xfff & 0xa900);
	return 0;
}
