





#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/input.h>




int main(int argc, char *argv[])
{
	int ret;
	struct input_event event;

	int fd = open("/dev/event0", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}


	while(1)
	{
		ret = read(fd, &event, sizeof(struct input_event));
		if(ret < 0)
		{
			perror("read");
			exit(1);
		}

		if(event.type == EV_KEY)
		{
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
	
		
		

	}



	return 0;
}













