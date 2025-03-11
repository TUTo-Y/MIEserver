/**
 * @file log.h
 * @brief 调试相关的宏定义
 */
#ifndef LOG_H
#define LOG_H

#include <stdio.h>

// 输出字体颜色
#define RESET "\x1B[0m"  // 重置为默认颜色
#define _BLK "\x1B[30m"  // 黑色
#define _RED "\x1B[31m"  // 红色
#define _GRN "\x1B[32m"  // 绿色
#define _YEL "\x1B[33m"  // 黄色
#define _BLU "\x1B[34m"  // 蓝色
#define _MAG "\x1B[35m"  // 洋红色
#define _CYN "\x1B[36m"  // 青色
#define _WHT "\x1B[37m"  // 白色
#define _BBLK "\x1B[90m" // 亮黑色
#define _BRED "\x1B[91m" // 亮红色
#define _BGRN "\x1B[92m" // 亮绿色
#define _BYEL "\x1B[93m" // 亮黄色
#define _BBLU "\x1B[94m" // 亮蓝色
#define _BMAG "\x1B[95m" // 亮洋红色
#define _BCYN "\x1B[96m" // 亮青色
#define _BWHT "\x1B[97m" // 亮白色

#define BLK(str) "\x1B[30m" str RESET  // 黑色
#define RED(str) "\x1B[31m" str RESET  // 红色
#define GRN(str) "\x1B[32m" str RESET  // 绿色
#define YEL(str) "\x1B[33m" str RESET  // 黄色
#define BLU(str) "\x1B[34m" str RESET  // 蓝色
#define MAG(str) "\x1B[35m" str RESET  // 洋红色
#define CYN(str) "\x1B[36m" str RESET  // 青色
#define WHT(str) "\x1B[37m" str RESET  // 白色
#define BBLK(str) "\x1B[90m" str RESET // 亮黑色
#define BRED(str) "\x1B[91m" str RESET // 亮红色
#define BGRN(str) "\x1B[92m" str RESET // 亮绿色
#define BYEL(str) "\x1B[93m" str RESET // 亮黄色
#define BBLU(str) "\x1B[94m" str RESET // 亮蓝色
#define BMAG(str) "\x1B[95m" str RESET // 亮洋红色
#define BCYN(str) "\x1B[96m" str RESET // 亮青色
#define BWHT(str) "\x1B[97m" str RESET // 亮白色

#define ERROR(...) fprintf(stderr, _BRED __VA_ARGS__), fprintf(stderr, RESET);
#define DEBUG(...) fprintf(stdout, __VA_ARGS__)
#define SUCESS(...) fprintf(stdout, _BGRN __VA_ARGS__), fprintf(stdout, RESET);

#endif // LOG_H