
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include "485_bus.h"

int print_buf(char *prompt, unsigned char *buf, int size)
{               
        int i;
#define DISPLAY_LEN     16
        if (prompt != NULL)
                printf("%s", prompt);
        for (i = 0; i < size; i++) {
                if ( i % DISPLAY_LEN == 0)
                        printf("\n");
                printf("%02x ", buf[i]);
        }                         
        printf("\nend\n");
        return 0;
} 

int main(int argc, char **argv)
{
	int fd;
	char rbuf[1024];
	char wbuf[1024];
	char cbuf[1024];
	int cbuf_len;
	int rlen;
	int ret;
	static struct timeval tv;
	static struct timeval end;
	static long b_ms = 0;
	static long e_ms = 0;
	int i;

	char *dev_name;

	dev_name = argv[1];

	fd = com_485_init(dev_name, 921600);
	if (fd < 0)
		exit(1);
	
	memset(wbuf, 0, sizeof(wbuf));
	memcpy(wbuf, argv[2], strlen(argv[2]));
	wbuf[strlen(argv[2])] = '\n';

	memset(cbuf, 0, sizeof(cbuf));
	cbuf_len = 0;
	//printf("argc:%d\n", argc);
	for (i = 3; i < argc; i++) {
		//printf("argv[%d]:%s\n", i, argv[i]);
		memcpy(cbuf + cbuf_len, argv[i], strlen(argv[i]));
		cbuf_len += strlen(argv[i]);
		if (i != argc - 1) {
			cbuf[cbuf_len] = ' ';
			cbuf_len++;
		}
	}
	//printf("compare str: %s \n", cbuf);
	ret = com_485_write(fd, wbuf, strlen(wbuf));
	if (ret < 0 ) {
		printf("write error\n");
		exit(1);
	}

    gettimeofday(&tv, NULL);
	b_ms = tv.tv_sec*1000 + tv.tv_usec/1000;

	while (1) {
		memset(rbuf, 0, sizeof(rbuf));
		rlen = sizeof(rbuf);
		ret = com_485_read(fd, rbuf, &rlen, 500);
		if (ret !=0 ) {
			printf("write read error\n");
			exit(1);
		}
		//if (strstr(rbuf, "simulate ret=0") != NULL) {
		if (strstr(rbuf, cbuf) != NULL) {
			gettimeofday(&end, NULL);
			e_ms = end.tv_sec*1000 + end.tv_usec/1000;

			printf("receive :%s end using %ld ms\n", cbuf, e_ms - b_ms);
			break;
		}
	}
	com_485_deinit(dev_name, 115200);
	return 0;
}
