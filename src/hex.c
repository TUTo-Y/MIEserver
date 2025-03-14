#include "hex.h"

static const uint8_t hex_lookup[256] = {
    [0] = '0', [1] = '1', [2] = '2', [3] = '3', [4] = '4', [5] = '5', [6] = '6', [7] = '7', [8] = '8', [9] = '9', [10] = 'a', [11] = 'b', [12] = 'c', [13] = 'd', [14] = 'e', [15] = 'f', ['0'] = 0, ['1'] = 1, ['2'] = 2, ['3'] = 3, ['4'] = 4, ['5'] = 5, ['6'] = 6, ['7'] = 7, ['8'] = 8, ['9'] = 9, ['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15, ['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15};

static uint8_t hex_to_bin(char c)
{
    return hex_lookup[(unsigned char)c];
}
static char bin_to_hex(uint8_t b)
{
    return hex_lookup[b];
}

// 16进制转换为二进制
void my_hex2bin(const char *hex, uint8_t *bin, size_t bin_len)
{
    for (size_t i = 0; i < bin_len; i++)
    {
        bin[i] = (hex_to_bin(hex[i * 2]) << 4) | hex_to_bin(hex[i * 2 + 1]);
    }
}

// 二进制转换为16进制
void bin2hex(const uint8_t *bin, char *hex, size_t bin_len)
{
    for (size_t i = 0; i < bin_len; i++)
    {
        hex[i * 2] = bin_to_hex(bin[i] >> 4);
        hex[i * 2 + 1] = bin_to_hex(bin[i] & 0x0f);
    }
}

// 以2进制输出为16进制数据
void putbin2hex(const uint8_t *bin, size_t bin_len, FILE *fp)
{
    char hex[2];
    for (size_t i = 0; i < bin_len; i++)
    {
        hex[0] = bin_to_hex(bin[i] >> 4);
        hex[1] = bin_to_hex(bin[i] & 0x0f);
        fwrite(hex, 1, 2, fp);
    }
}

// 以16进制输出为2进制数据
void puthex2bin(const char *hex, size_t bin_len, FILE *fp)
{
    uint8_t bin;
    for (size_t i = 0; i < bin_len; i++)
    {
        bin = (hex_to_bin(hex[i * 2]) << 4) | hex_to_bin(hex[i * 2 + 1]);
        fwrite(&bin, 1, 1, fp);
    }
}

// 以2进制读取为16进制数据
void getbin2hex(char *hex, size_t bin_len, FILE *fp)
{
    uint8_t bin;
    for (size_t i = 0; i < bin_len; i++)
    {
        fread(&bin, 1, 1, fp);
        hex[i * 2] = bin_to_hex(bin >> 4);
        hex[i * 2 + 1] = bin_to_hex(bin & 0x0f);
    }
}

// 以16进制读取为2进制数据
void gethex2bin(uint8_t *bin, size_t bin_len, FILE *fp)
{
    char hex[2];
    for (size_t i = 0; i < bin_len; i++)
    {
        fread(hex, 1, 2, fp);
        bin[i] = (hex_to_bin(hex[0]) << 4) | hex_to_bin(hex[1]);
    }
}