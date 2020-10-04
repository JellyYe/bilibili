#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	printf("hello android world\n");

	int fd = open("/dev/xxx", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}


	return 0;
}
