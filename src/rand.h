/**
 * \file  rend.c
 * \brief 随机数处理器
 */
#ifndef RAND_H
#define RAND_H

#define _CRT_RAND_S
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define Random(x) (rand() << 0x10 | rand())
#define Random64(x) (Random(x) << 0x20 | Random(x))

/**
 * \brief 设置Xorshift随机数种子
 * \param seed 随机数种子
 */
void xSrand8(uint8_t seed);
void xSrand16(uint16_t seed);
void xSrand32(uint32_t seed);
void xSrand64(uint64_t seed);

/**
 * \brief 获取Xorshift随机数
 * \return 随机数
 * \note 该函数使用了混沌映射算法
 */
uint8_t xRand8();
uint16_t xRand16();
uint32_t xRand32();
uint64_t xRand64();

#endif // RAND_H