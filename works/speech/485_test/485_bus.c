
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include "485_bus.h"

int STOP = 0;
int res;

int speed_arr[] = {
	B921600, B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200, B9600,
	B4800, B2400, B1200, B300,
};
int name_arr[] = { 
	921600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800, 2400,
	1200, 300,
};
#ifndef FALSE
#define FALSE	-1
#endif
#ifndef TRUE
#define TRUE	0
#endif
static int g_pgc_fd;
extern int print_buf(char *prompt, unsigned char *buf, int size);

static void set_speed (int fd, int speed)
{
	int i;
	int status;
	struct termios Opt;
	tcgetattr (fd, &Opt);
	for (i = 0; i < sizeof (speed_arr) / sizeof (int); i++)  {
		if (speed == name_arr[i]) {
			tcflush (fd, TCIOFLUSH);
			cfsetispeed (&Opt, speed_arr[i]);
			cfsetospeed (&Opt, speed_arr[i]);
			status = tcsetattr (fd, TCSANOW, &Opt);
			if (status != 0) {
				perror ("tcsetattr fd1");
				return;
			}
			tcflush (fd, TCIOFLUSH);
		}
	}
}
static int set_Parity (int fd, int databits, int stopbits, int parity)
{
	struct termios options;
	if (tcgetattr (fd, &options) != 0) {
		perror ("SetupSerial 1");
		return (FALSE);
	}
	options.c_cflag &= ~CSIZE;
	switch (databits) {
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf (stderr, "Unsupported data size\n");
		return (FALSE);
	}
	switch (parity) {
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;    /* Enable parity checking */
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);
		options.c_iflag |= INPCK; /* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;    /* Enable parity */
		options.c_cflag &= ~PARODD;
		options.c_iflag |= INPCK; /* Disnable parity checking */
		break;
	case 'S':
	case 's':           /*as no parity */
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf (stderr, "Unsupported parity\n");
		return (FALSE);
	}
	switch (stopbits) {
	case 1:
		options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		options.c_cflag |= CSTOPB;
		break;
	default:
		fprintf (stderr, "Unsupported stop bits\n");
		return (FALSE);
	}
	/* Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;
	tcflush (fd, TCIFLUSH);
	options.c_cc[VTIME] = 150;
	options.c_cc[VMIN] = 0;   /* Update the options and do it NOW */
	if (tcsetattr (fd, TCSANOW, &options) != 0) {
		perror ("SetupSerial 3");
		return (FALSE);
	}
	return (TRUE);
}
int com_485_write(int fd, unsigned char *buf, int len)
{
//	system("echo high > /sys/class/gpio/gpio56/direction");
	return  write(fd, buf, len);
}
int com_485_read(int fd, unsigned char *buf, int *len, int timeout_ms)
{
	int nread;
	fd_set rd;
	struct timeval time;
	int ret;
	unsigned char tmp_buf[64];

	int remain = *len;
	int single;
	int cp_len = 0;
	int flg = 1;

	time.tv_sec = 0;
	time.tv_usec = timeout_ms * 1000;
	if (timeout_ms == 0)
		flg = 0;

//	system("echo low > /sys/class/gpio/gpio56/direction");
	while (1) {
		FD_ZERO(&rd);
		FD_SET(fd, &rd);
		while (FD_ISSET(fd, &rd)) {
			if (flg == 0) {
				ret = select(fd+1, &rd, NULL, NULL, NULL);
			} else {
				ret = select(fd+1, &rd, NULL, NULL, &time);
			}
			if (ret == 0) {
				/* time out */
				goto timeout;
			} else if (ret < 0) {
				printf("select error \r\n");
				goto err;
			} else {
				memset(tmp_buf, 0, sizeof(tmp_buf));
				nread = read(fd, tmp_buf, sizeof(tmp_buf));
				if (nread < 0) {
					printf("read error \r\n");
					goto err;
				}
				flg = 1;
				single = remain > nread ? nread: remain;
				memcpy(buf + cp_len, tmp_buf, single);
				cp_len += single;
				remain -= single;
				if (remain <= 0 )
					break;
				time.tv_sec = 0;
				time.tv_usec = 500;
			}
		}
	}
timeout:
	*len = cp_len;
	return 0;
err:
	return -1;
}
int com_485_deinit(char *dev_name, int baud)
{
	close(g_pgc_fd);
	return 0;
}
int com_485_init(char *dev_name, int baud)
{
	int fd;
	fd = open (dev_name, O_RDWR);
	if (fd < 0) {
		perror ("serialport error\n");
		return -1;
	} else {
	//	printf ("open ");
	//	printf ("%s", ttyname (fd));
	//	printf (" succesfully\n");
	}

	set_speed (fd, baud);
	if (set_Parity (fd, 8, 1, 'N') == FALSE) {
		printf ("Set Parity Error\n");
		return -1;
	}

	//system("echo 56 > /sys/class/gpio/export");
	g_pgc_fd = fd;
	return fd;
} 
int com_485_write_read(int fd, unsigned char *wbuf, int wlen, unsigned char *rbuf, int *rlen, int timeout_ms)
{
	int ret;
	ret = com_485_write(fd, wbuf, wlen);
	if (ret < 0) {
		printf("write error \r\n");
		return ret;
	}

	ret = com_485_read(fd, rbuf, rlen, timeout_ms);
	if (ret < 0) {
		printf("read error \r\n");
		return ret;
	}
	if (*rlen == 0) {
		/* timeout */
		return 1;
	}
	return 0;
}
unsigned char calc_sum(unsigned char *begin, int len)
{
	unsigned char sum = 0;
	int i;
	for (i = 0; i < len; i++) {

		sum += *(begin + i);
	}
	return sum;
}
static unsigned char check_sum(unsigned char *begin, int len, unsigned char sum)
{
	unsigned char cal_sum = 0;
	int i;
	for (i = 0; i < len; i++) {
		cal_sum += *(begin + i);
	}
	return (cal_sum == sum);
}
