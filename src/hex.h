#ifndef HEX_H
#define HEX_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// SBgmssl，拿着这么好的函数名，结果用一堆if，害得我还没发用这个函数名，真是浪费
#define hex2bin my_hex2bin

// 16进制转换为2进制
void my_hex2bin(const char *hex, uint8_t *bin, size_t bin_len);

// 2进制转换为16进制
void bin2hex(const uint8_t *bin, char *hex, size_t bin_len);

// 以2进制输出为16进制数据
void putbin2hex(const uint8_t *bin, size_t bin_len, FILE *fp);

// 以16进制输出为2进制数据
void puthex2bin(const char *hex, size_t bin_len, FILE *fp);

// 以2进制读取为16进制数据
void getbin2hex(char *hex, size_t bin_len, FILE *fp);

// 以16进制读取为2进制数据
void gethex2bin(uint8_t *bin, size_t bin_len, FILE *fp);

#endif // HEX_H