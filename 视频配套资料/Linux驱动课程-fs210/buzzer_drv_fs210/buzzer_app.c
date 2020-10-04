#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define BUZZER_ON_C _IO('B',0x3347)
#define BUZZER_OFF_C _IO('B',0x3346)


int main(int argc,char argv[])
{
      int fd = -1;
      int buzzer_state = 0;

      fd = open("/dev/buzzer",O_RDWR);
      if(fd<0)
      {
	    perror("open");
	    exit(1);
      }

      while(1){
	    buzzer_state = 1;
	    //write(fd,&buzzer_state,sizeof(int));
	    ioctl(fd,BUZZER_ON_C);
	    usleep(5000000);
	    buzzer_state = 0;
	    //write(fd,&buzzer_state,sizeof(int));
	    ioctl(fd,BUZZER_OFF_C);
	    usleep(5000000);

      }

      close(fd);

      return 0;
}
