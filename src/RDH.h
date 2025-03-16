/**
 * @file RDH.h
 * @brief RDH - EI
 */
#ifndef RDH_H
#define RDH_H

#define _CRT_RAND_S
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <gmssl/sm3.h>

#include "rand.h"
// #include "log.h"

enum
{
    RDH_SUCESS = 0,
    RDH_ERROR
};
typedef int rdhStatus;

/**
 * \brief 使用洗牌算法打乱和恢复图像数据
 * \param img 图像数据
 * \param size 数据大小
 * \param key 随机数种子
 */
void rdhShuffleImage(uint8_t *img, int size, uint64_t key);
void rdhUnshuffleImage(uint8_t *img, int size, uint64_t key);

/**
 * \brief 将图像随机分成加密份额1和加密份额2
 * \param img 图像数据
 * \param size 数据大小
 * \param img1 加密份额1
 * \param img2 加密份额2
 */
void rdhSplitImage(const uint8_t *img, int size, uint8_t **img1, uint8_t **img2);
void rdhCombineImage(const uint8_t *img1, const uint8_t *img2, int size, uint8_t **img);

/**
 * \brief 嵌入bit流的数据
 * \param img1Line1 图像1行1
 * \param img1Line2 图像1行2
 * \param img1Line3 图像1行3
 * \param img2Line1 图像2行1
 * \param img2Line2 图像2行2
 * \param img2Line3 图像2行3
 * \param byte 字节流
 * \param total bit位总数
 * \param now 当前bit位
 * \return 嵌入的额外数据
 */
uint8_t rdhEmbedDataByte(uint8_t *img1Line1, uint8_t *img1Line2, uint8_t *img1Line3,
                         uint8_t *img2Line1, uint8_t *img2Line2, uint8_t *img2Line3,
                         const uint8_t *byte, int total, int *now);

/**
 * \brief 提取bit流的数据
 * \param img1Line1 图像1行1
 * \param img1Line2 图像1行2
 * \param img1Line3 图像1行3
 * \param img2Line1 图像2行1
 * \param img2Line2 图像2行2
 * \param img2Line3 图像2行3
 * \param byte 字节流
 * \param now 当前bit位
 * \param m 额外数据
 */
void rdhExtractDataByte(uint8_t *img1Line1, uint8_t *img1Line2, uint8_t *img1Line3,
                        uint8_t *img2Line1, uint8_t *img2Line2, uint8_t *img2Line3,
                        uint8_t *byte, int *now,
                        uint8_t m);

/**
 * \brief 嵌入数据
 * \param img1 图像份额1
 * \param img2 图像份额1
 * \param w 宽度
 * \param h 高度
 * \param data 数据
 * \param size 数据大小
 * \param m 嵌入的额外数据
 * \param mSize 额外数据大小
 * \return 状态码
 */
rdhStatus rdhEmbedData(uint8_t *img1, uint8_t *img2,
                       int w, int h,
                       uint8_t **m, int *mSize,
                       const uint8_t *data, int size);

/**
 * \brief 提取数据
 * \param img1 图像份额1
 * \param img2 图像份额1
 * \param w 宽度
 * \param h 高度
 * \param m 额外数据
 * \param mSize 额外数据大小
 * \param data 数据
 * \return 状态码
 */
rdhStatus rdhExtractData(uint8_t *img1, uint8_t *img2,
                         int w, int h,
                         const uint8_t *m, int mSize,
                         uint8_t **data);

/**
 * \brief 分配空间
 * \param 大小
 */
void *rdhMalloc(size_t size);
/**
 * \brief 释放空间
 * \param data 数据
 */
void rdhFree(void *data);

#endif // RDH_H