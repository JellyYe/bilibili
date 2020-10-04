#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


#define LOG_TAG "Test_log"
#include <utils/Log.h>


int main(int argc, char *argv[])
{
	printf("hello android world\n");

	int a = 34;
	int b = 44;
	char *c = "hello heleoel";

	LOGD("This is a simple log test\n");
	LOGI("a + b = %d\n", a+b);
	LOGW("c = %s\n", c);
	LOGE("error error!!!!\n");
	

	int fd = open("/dev/xxx", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}

	return 0;
}
