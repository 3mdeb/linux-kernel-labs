#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../include/my_cdev.h"

#define DEVICE_PATH	"/dev/my_cdev"

int main(int argc, char **argv)
{
	int fd;
	char buffer[BUFFER_SIZE];

	fd = open(DEVICE_PATH, O_RDONLY);
	if (fd < 0) {
		perror("open failed");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, MY_IOCTL_PRINT, 0) < 0) {
		perror("ioctl failed");
		exit(EXIT_FAILURE);
	}

	close(fd);

	return 0;
}
