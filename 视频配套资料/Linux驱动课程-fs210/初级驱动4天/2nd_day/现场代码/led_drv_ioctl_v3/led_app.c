






#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>


#define LED_NUM_ON  _IOW('L',0x3456, int)
#define LED_NUM_OFF  _IOW('L',0x3457, int)
#define LED_ALL_ON  _IO('L',0x3458)
#define LED_ALL_OFF  _IO('L',0x3459)


int main(int argc, char *argv[])
{

	int fd = open("/dev/led", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}



	while(1)
	{
		ioctl(fd, LED_NUM_ON, 3);
		sleep(1);
		ioctl(fd, LED_NUM_OFF, 3);
		sleep(1);

		ioctl(fd, LED_NUM_ON, 4);
		sleep(1);
		ioctl(fd, LED_NUM_OFF, 4);
		sleep(1);

		ioctl(fd, LED_ALL_ON);
		sleep(1);
		ioctl(fd, LED_ALL_OFF);
		sleep(1);
	
	}
	

	close(fd);

	return 0;
}













