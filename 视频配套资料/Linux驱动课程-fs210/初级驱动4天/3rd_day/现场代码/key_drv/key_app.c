






#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

	int fd = open("/dev/led", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}


	//À­µÆ---Ð´1
	int on = 1;
	write(fd, &on, 4);

	sleep(1);
	on = 0;
	write(fd, &on, 4);

	close(fd);

	return 0;
}













