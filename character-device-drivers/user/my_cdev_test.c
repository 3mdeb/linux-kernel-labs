/*
 * SO2 Lab - Linux device drivers (#4)
 * User-space test file
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../include/my_cdev.h"

#define DEVICE_PATH	"/dev/my_cdev"

/*
 * prints error message and exits
 */

static void error(const char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

/*
 * print use case
 */

static void usage(const char *argv0)
{
	printf("Usage: %s <options>\n options:\n"
			"\tp - print\n", argv0);
	exit(EXIT_FAILURE);
}

/*
 * Sample run:
 *  ./so2_cdev_test p		; print ioctl message
 *  ./so2_cdev_test d		; wait on wait_queue
 *  ./so2_cdev_test u		; wait on wait_queue
 */

int main(int argc, char **argv)
{
	int fd;
	char buffer[BUFFER_SIZE];

	if (argc < 2)
		usage(argv[0]);

	if (strlen(argv[1]) != 1)
		usage(argv[0]);

	fd = open(DEVICE_PATH, O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	switch (argv[1][0]) {
	case 'p':				/* print */
		if (ioctl(fd, MY_IOCTL_PRINT, 0) < 0) {
			perror("ioctl");
			exit(EXIT_FAILURE);
		}

		break;
	default:
		error("Wrong parameter");
	}

	close(fd);

	return 0;
}
