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

//#include "485_test.h"
#define CMD_ACTIVE_LEN 27
#define CMD_CANCEL_LEN 11
#define CMD_MAX_LEN 27
#define CMD_MIN_LEN 1

#define START_FLAG0 0xfa
#define START_FLAG1 0xfb
unsigned const char default_cmd_head[] = {0xfa, 0xfb, 0, 0, 0, 0};
#define default_cmd_opcode
typedef enum {
    OP_CODE_START  = 0x00b0,
    OP_CODE_CANCEL = 0x00b1,
} OP_CODE;

#define OP_CODE_LEN16  16

typedef enum {
    CMD_DEF0 = 0,
    CMD_DEF1,
    CMD_DEF2,
    CMD_DEF3,
    CMD_DEF4,
    CMD_DEF5,
    CMD_DEF6,
    CMD_DEF7,
    CMD_DEF8,
    CMD_DEF9,
    CMD_DEF10,
    CMD_DEF11,
    CMD_DEF12,
    CMD_DEF13,
    CMD_DEF14,
    CMD_DEF15,
    CMD_DEF16,
    CMD_DEF17,
    CMD_DEF18,
    CMD_DEF19,
    CMD_DEF20,
    CMD_DEF21,
    CMD_DEF22,
    CMD_DEF23,
    CMD_DEF24,
    CMD_DEF25,
    CMD_DEF26,
    CMD_DEF_MAX
} CMD_DEF;


int print_buf(char *prompt, unsigned char *buf, int size)
{
    int i;
    if (prompt != NULL)
        printf("%s", prompt);
    for (i = 0; i < size; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    return 0;
}

#define CMD_LEN 1024
unsigned char cbuf[CMD_LEN] = {0};
int cbuf_len = 0;
int s_count_all = 0;

const char* cmd_opcode_all[] = {
    "开始",
    "取消",
};
typedef enum {
    CONTENT_WUGU = 1,
    CONTENT_DOUJIANG = 2,
    CONTENT_KAFEI = 5,
} CONTENT_TYPE;
const char* cmd_type_all[] = {
    "不识别的类型",
    "磨五谷",
    "磨豆浆",
    "不识别的类型",
    "不识别的类型",
    "磨咖啡",
};
int cmd_result_print(unsigned short cmd_opcode, unsigned char cmd_type, unsigned int cmd_time) {
    if (OP_CODE_CANCEL == cmd_opcode) {
        printf("[RESULT][%d]:%s\n", ++s_count_all, cmd_opcode_all[1]);
        return 1;
    }
    if (0 == cmd_time) {
        printf("[RESULT][%d]:%s%s\n", ++s_count_all, cmd_opcode_all[0],cmd_type_all[cmd_type]);
        return 1;
    } else {
        printf("[RESULT][%d]:%d小时后%s\n", ++s_count_all, cmd_time/60, cmd_type_all[cmd_type]);
        return 1;
    }
    return 0;
}

int command_line_parse() {
    int i = 0;
    int ret = 0;
    unsigned short cmd_opcode = 0;
    unsigned short cmd_len = 0;
    unsigned short cmd_time = 0;
    unsigned char cmd_type = 0;

    if (cbuf_len < CMD_MIN_LEN || cbuf_len > CMD_MAX_LEN) {
        return -1;
    }
    for (i = 0; i < cbuf_len; i++) {
        switch(i) {
        case CMD_DEF0:
        case CMD_DEF1:
        case CMD_DEF2:
        case CMD_DEF3:
        case CMD_DEF4:
        case CMD_DEF5:
            if (default_cmd_head[i] != cbuf[i]) {
                printf("cmd err[%d]:default_cmd_head[%d]=0x%x, cbuf[%d]=0x%x\n", __LINE__,  i, default_cmd_head[i], i, cbuf[i]);
                return -2;
            }
            break;
        //opcode
        case CMD_DEF6:
            cmd_opcode |= cbuf[i] << 8;
            break;
        case CMD_DEF7:
            cmd_opcode |= cbuf[i];
            if (OP_CODE_START != cmd_opcode && OP_CODE_CANCEL != cmd_opcode) {
                printf("cmd err[%d]:cmd_opcode=%d\n", __LINE__, cmd_opcode);
                return -2;
            }
            break;
        //len
        case CMD_DEF8:
            cmd_len |= cbuf[i] << 8;
            break;
        case CMD_DEF9:
            cmd_len |= cbuf[i];
            if ((OP_CODE_START == cmd_opcode) && (OP_CODE_LEN16 == cmd_len)) {
                continue;
            } else if ((OP_CODE_CANCEL == cmd_opcode) && (0 == cmd_len)) {
                continue;
            } else {
                printf("cmd err[%d]:cmd_opcode=%d,cmd_len=%d\n", __LINE__, cmd_opcode, cmd_len);
                return -2;
            }
            break;
        //data[16]
        case CMD_DEF10:
            if (OP_CODE_CANCEL == cmd_opcode) {
                //checksum
                ret = cmd_result_print(cmd_opcode, 0, 0);
                return ret;
            }
            break;
        case CMD_DEF11:
        case CMD_DEF12:
            if (OP_CODE_START != cmd_opcode) {
                printf("cmd err[%d]:cmd_opcode=%d in data parse\n", __LINE__, cmd_opcode);
                return -2;
            }
            break;
        case CMD_DEF13:
            cmd_type = cbuf[i];
            break;
        case CMD_DEF14:
            cmd_time |= cbuf[i] << 8;
            break;
        case CMD_DEF15:
            cmd_time |= cbuf[i];
            break;
        case CMD_DEF16:
        case CMD_DEF17:
        case CMD_DEF18:
        case CMD_DEF19:
        case CMD_DEF20:
        case CMD_DEF21:
        case CMD_DEF22:
        case CMD_DEF23:
        case CMD_DEF24:
            if (0 != cbuf[i]) {
                printf("cmd err[%d]:buf[%d] = 0x%x\n", __LINE__, i, cbuf[i]);
                return -2;
            }
            break;
        case CMD_DEF25:
            if (OP_CODE_START != cmd_opcode || 1 != cbuf[i]) {
                printf("cmd err[%d]:cmd_opcode=%d in data parse\n", __LINE__, cmd_opcode);
                return -2;
            }
            break;
        case CMD_DEF26:
            //checksum
            ret = cmd_result_print(cmd_opcode, cmd_type, cmd_time);
            return ret;
            break;
        case CMD_DEF_MAX:
            printf("cmd err[%d]:cmd_opcode=%d in data parse\n", __LINE__, cmd_opcode);
            return -2;
            break;
        }
    }
    return 0;
}

void command_line_reset() {
    memset(cbuf, 0, CMD_LEN);
    cbuf_len = 0;
}

int command_line_compose(unsigned char* rbuf, int rlen) {
    int ret = 0;

    memcpy(cbuf + cbuf_len, rbuf, rlen);
    cbuf_len += rlen;
    ret = command_line_parse();
    if (0 != ret) {
        print_buf("debug:", cbuf, cbuf_len);
        command_line_reset();
    }
    return 0;
}

void print_help() {
    printf("Usage:\n");
    printf("sudo ./serial_parse 0 /dev/ttyUSB0  //从串口设备实时读\n");
    printf("sudo ./serial_parse 1 log.txt       //从log文件读取解析\n");
}

int parse_from_uart(char *dev_name) {
    int fd;
    int rlen = 0;
    int ret = 0;
    unsigned char rbuf[16] = {0};

    fd = com_485_init(dev_name, 9600);
    if (fd < 0) {
        exit(1);
    }

    while (1) {
        memset(rbuf, 0, sizeof(rbuf));
        rlen = sizeof(rbuf);
        ret = com_485_read(fd, rbuf, &rlen, 0);
        if (ret !=0 ) {
            printf("write read error\n");
            break;
        }
//        if (1 != rlen) {
//            printf("read len=%d, err!!!\n", rlen);
//            //break;
//        }
        command_line_compose(rbuf, rlen);
    }
    com_485_deinit(dev_name, 115200);
    return 0;
}

int byte_to_hex(char ch)
{
    if (ch >= 'a' && ch <= 'f') {
        return 10 + ch - 'a';
    } else if (ch >= 'A' && ch <= 'F') {
        return 10 + ch - 'A';
    } else if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else {
    }
    return -1;
}
int str_to_hex(char rbuf, unsigned char *val) {
    static unsigned char hex = 0;
    static int flag = 0;

    //Filtering illegal characters
    if (rbuf < 0x30 || rbuf > 0x66) {
        hex = 0;
        flag = 0;
        return -1;
    } else {
        if (!flag) {
            hex = 0;
            hex = (byte_to_hex(rbuf) & 0xf) << 4;
            flag = 1;
            return -1;
        } else {
            *val = hex | (byte_to_hex(rbuf) & 0xf);
            flag = 0;
            hex = 0;
            return 0;
        }
    }
    return -1;
}

int parse_from_file(char *dev_name) {
    FILE *fd;
    int ret = 0;
    char rbuf = 0;
    unsigned char rchar = 0;

    fd = fopen(dev_name, "r");
    if (NULL == fd) {
        printf("file not found.\n");
        return 0;
    }

    while (1) {
        rbuf = 0;
        ret = fread(&rbuf, 1, 1, fd);
        if (0 == ret) {
            printf("read end.\n");
            break;
        }
        if (!str_to_hex(rbuf, &rchar)) {
        //printf("===%02x,%02x\n",rbuf, rchar);
            command_line_compose(&rchar, 1);
        }
    }
    fclose(fd);
    return 0;
}

int main(int argc, char **argv)
{
    char *file_name = NULL;

    if (3 != argc) {
        print_help();
        return 0;
    }
    printf("Ver:0.0.1\n");

    command_line_reset();

    file_name = argv[2];

    if ('0' == argv[1][0]) {
        parse_from_uart(file_name);
    } else if ('1' == argv[1][0]) {
        parse_from_file(file_name);
    } else {
        print_help();
    }

    return 0;
}

