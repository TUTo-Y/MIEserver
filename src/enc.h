#ifndef ENC_H
#define ENC_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmssl/sm2.h>
#include <gmssl/sm3.h>
#include <gmssl/zuc.h>

#include "log.h"

// 生成ZUC密钥
void encZucKeyIv(uint8_t key[ZUC_KEY_SIZE], uint8_t iv[ZUC_IV_SIZE]);

/**
 * \brief 使用SM3获取消息摘要(密码)
 * \param 需要获取摘要的消息
 * \param 消息长度
 * \param 摘要
 * \return 是否成功
 */
void encHash(const uint8_t *msg, size_t msg_len, uint8_t digest[SM3_DIGEST_SIZE]);

/**
 * \brief 使用比对SM3获取消息摘要(密码)
 * \param a 消息摘要a
 * \param b 消息摘要b
 * \return 是否相等, 0相等, 非0不相等
 */
static inline int encHashCom(const uint8_t a[SM3_DIGEST_SIZE], const uint8_t b[SM3_DIGEST_SIZE])
{
    // return memcmp(a, b, SM3_DIGEST_SIZE);
    return memcmp(a, b, SM3_DIGEST_SIZE) ? (DEBUG("SM3对比错误: a != b\n"), 1) : 0;
}

/**
 * 密钥封装
 */
struct ENCkem_hand_t
{
    SM2_CIPHERTEXT zuc_key; // 被sm2加密后的ZUC密钥

    uint8_t hash[SM3_DIGEST_SIZE]; // 密文哈希
    size_t size;                   // 密文长度信息
};
struct ENCkem_t
{
    struct ENCkem_hand_t hand;
    uint8_t cipher[]; // 密文
};
typedef struct ENCkem_t *ENCkem;

/**
 * \brief 使用sm2公钥和zuc加密消息
 * \param plain 明文
 * \param plain_len 明文长度
 * \param pub_key 公钥
 * \return 加密后的封装密文
 */
ENCkem encKEMEnc(const uint8_t *plain, size_t plain_len, const SM2_KEY *pub_key);

/**
 * \brief 使用sm2私钥和zuc解密消息
 * \param kem 封装密文
 * \param pri_key 私钥
 * \return 明文
 */
uint8_t *encKEMDec(const ENCkem kem, const SM2_KEY *pri_key);

/**
 * \brief 获取明文/密文长度
 * \param kem 封装密文
 * \return 明文长度
 */
static inline size_t encKEMSizeText(const ENCkem kem)
{
    return kem->hand.size;
}

/**
 * \brief 获取kem结构体长度
 * \param kem 封装密文
 * \return kem结构体长度
 */
static inline size_t encKEMSizeKEM(const ENCkem kem)
{
    return sizeof(struct ENCkem_hand_t) + encKEMSizeText(kem);
}

/**
 * \brief 释放封装密文
 * \param kem 封装密文
 */
static inline void encKEMFree(ENCkem kem)
{
    free(kem);
}

#endif // ENC_H