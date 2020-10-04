






#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{

	int fd = open("/dev/hello", O_RDWR);


	return 0;
}













