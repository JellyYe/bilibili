





#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>


#define LED_NUM_ON  _IOW('L',0x3456, int)
#define LED_NUM_OFF  _IOW('L',0x3457, int)
#define LED_ALL_ON  _IO('L',0x3458)
#define LED_ALL_OFF  _IO('L',0x3459)

#define VIRT_BTN_VOL_UP _IO('L', 0x3460)
#define VIRT_BTN_VOL_DOWN _IO('L', 0x3461)

// ./app up
int main(int argc, char *argv[])
{

	if(argc < 2)
	{
		printf("usage : ./app up/down\n");
		exit(1);
		
	}
		

	int fd = open("/dev/led1", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}

	if(!strcmp(argv[1], "up")){
		ioctl(fd, VIRT_BTN_VOL_UP, 0);

	}else if(!strcmp(argv[1], "down")){
		ioctl(fd, VIRT_BTN_VOL_DOWN, 0);
	}
	
	

	

	close(fd);

	return 0;
}













