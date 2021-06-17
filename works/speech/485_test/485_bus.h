#ifndef __485_BUS_H
#define __485_BUS_H

struct pgc_info {
	unsigned char header;
	unsigned char ctrl[2];
	unsigned char info_len;
	unsigned char info[42];
	unsigned char chk_sum;
	unsigned char tail;
};
struct pgc_single_info {
	unsigned char header;
	unsigned char ctrl[2];
	unsigned char info_len;
	unsigned char info[7];
	unsigned char chk_sum;
	unsigned char tail;
};
struct pgc_resp {
	unsigned char header;
	unsigned char ctrl[2];
	unsigned char tag;
	unsigned char chk_sum;
	unsigned char tail;
};

#define PGC_HEADER	0x7e
#define PGC_SYNC	0x55
#define PGC_TAIL	0xe7


#define PGC_CLS_OPEN	0x11
#define PGC_CLS_CLOSE	0x33
#define PGC_CLS_DATA	0x77
#define PGC_CLS_ACK	0xAA

#define PGC_CHK_OK	0x00
#define PGC_CHK_FAIL	0x11

int com_485_init(char *dev_name, int baud);

int com_485_write(int fd, unsigned char *buf, int len);

int com_485_read(int fd, unsigned char *buf, int *len, int timeout_ms);

int com_485_pgc_write_read(int fd, unsigned char *wbuf, int wlen, unsigned char *rbuf, int *rlen, int timeout_ms);

unsigned char calc_sum(unsigned char *begin, int len);

int pgc_control(int fd, unsigned char *car_num, unsigned char stop, unsigned char door, int open_close);

int com_485_deinit(char *dev_name, int baud);

#endif
