





#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/input.h>

struct key_event{
	int code; //按键的名字---下键,回车键,ESC--KEY_ESC
	int value; //按键的状态---按下和抬起--1/0
};


int main(int argc, char *argv[])
{
	int ret;
	struct key_event event;

	int fd = open("/dev/key0", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}


	while(1)
	{
		ret = read(fd, &event, sizeof(struct key_event));
		if(ret < 0)
		{
			perror("read");
			exit(1);
		}
	
		if(event.code == KEY_DOWN)
		{
			if(event.value)
			{
				printf("<APP>-------KEY_DOWN pressed\n");
			}else{
				printf("<APP>-------KEY_DOWN up\n");
			}
			
		}
		

	}



	return 0;
}













