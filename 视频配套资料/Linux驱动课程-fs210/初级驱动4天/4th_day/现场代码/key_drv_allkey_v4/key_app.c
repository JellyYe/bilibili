





#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/input.h>
#include <poll.h>

struct key_event{
	int code; //按键的名字---下键,回车键,ESC--KEY_ESC
	int value; //按键的状态---按下和抬起--1/0
};


int main(int argc, char *argv[])
{
	int ret;
	struct key_event event;
	char bkd_buf[128];

	int fd = open("/dev/key0", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}


	
	//监控两个fd---标准输入,和当前key按键
	struct pollfd pfds[2];

	pfds[0].fd = 0;
	pfds[0].events = POLLIN ;

	pfds[1].fd = fd;
	pfds[1].events = POLLIN;



	while(1)
	{	
		ret = poll(pfds, 2,  -1);
		if(ret < 0)
		{
			perror("poll");
			exit(1);
		}
		if(ret > 0)
		{
				//查询是谁
			if(pfds[0].revents & POLLIN)
				{
					//接收标准输入
					fgets(bkd_buf,	128,  stdin);
					printf("bkd_buf = %s\n", bkd_buf);
					
				}
		
				if(pfds[1].revents & POLLIN)
				{	
					ret = read(pfds[1].fd, &event, sizeof(struct key_event));
					if(ret < 0)
					{
						perror("read");
						exit(1);
					}


					switch(event.code)
					{
						case KEY_DOWN:
							if(event.value)
							{
								printf("<APP>-------KEY_DOWN pressed\n");
							}else{
								printf("<APP>-------KEY_DOWN up\n");
							}
						
							break;
						case KEY_UP:
							if(event.value)
							{
								printf("<APP>-------KEY_UP pressed\n");
							}else{
								printf("<APP>-------KEY_UP up\n");
							}
							break;

						case KEY_RIGHT:
							if(event.value)
							{
								printf("<APP>-------KEY_RIGHT pressed\n");
							}else{
								printf("<APP>-------KEY_RIGHT up\n");
							}
							break;
						case KEY_LEFT:
							if(event.value)
							{
								printf("<APP>-------KEY_LEFT pressed\n");
							}else{
								printf("<APP>-------KEY_LEFT up\n");
							}
							break;

						default:
							printf("unkown key code");
							

					}
				
					
				}
			}
		


	}
	



	return 0;
}













