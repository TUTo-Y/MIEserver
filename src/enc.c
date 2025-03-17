#include "enc.h"

void encZucKeyIv(uint8_t key[ZUC_KEY_SIZE], uint8_t iv[ZUC_IV_SIZE])
{
    // 初始化key和iv
    for (int i = 0; i < ZUC_KEY_SIZE; i++)
    {
        key[i] = rand() % 256;
        iv[i] = rand() % 256;
    }
}

void encHash(const uint8_t *msg, size_t msg_len, uint8_t digest[SM3_DIGEST_SIZE])
{
    SM3_CTX ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, msg, msg_len);
    sm3_finish(&ctx, digest);
}

ENCkem encKEMEnc(const uint8_t *plain, size_t plain_len, const SM2_KEY *pub_key)
{
    // 密钥封装
    ENCkem kem = (ENCkem)malloc(sizeof(struct ENCkem_hand_t) + plain_len);
    kem->hand.size = plain_len;

    // 使用sm3计算哈希
    encHash(plain, plain_len, kem->hand.hash);

    // 使用ZUC加密数据
    ZUC_STATE zuc;
    uint8_t key[ZUC_KEY_SIZE + ZUC_IV_SIZE];

    encZucKeyIv(key, key + ZUC_KEY_SIZE);
    zuc_init(&zuc, key, key + ZUC_KEY_SIZE);
    zuc_encrypt(&zuc, plain, plain_len, kem->cipher);

    // 使用sm2加密ZUC密钥
    sm2_do_encrypt(pub_key, key, ZUC_KEY_SIZE + ZUC_IV_SIZE, &kem->hand.zuc_key);

    return kem;
}

uint8_t *encKEMDec(const ENCkem kem, const SM2_KEY *pri_key)
{
    // 分配内存
    uint8_t *plain = (uint8_t *)malloc(kem->hand.size);

    // 使用sm2解密ZUC密钥
    uint8_t key[ZUC_KEY_SIZE + ZUC_IV_SIZE];
    size_t key_len;
    sm2_do_decrypt(pri_key, &kem->hand.zuc_key, key, &key_len);

    // 使用ZUC解密数据
    ZUC_STATE zuc;
    zuc_init(&zuc, key, key + ZUC_KEY_SIZE);
    zuc_encrypt(&zuc, kem->cipher, kem->hand.size, plain);

    // 使用sm3计算哈希
    uint8_t hash[SM3_DIGEST_SIZE];
    encHash(plain, kem->hand.size, hash);

    // 比对哈希
    if (encHashCom(hash, kem->hand.hash))
    {
        DEBUG("密文哈希对比错误\n");
        free(plain);
        return NULL;
    }

    return plain;
}

enc_sm2_key_save_fun encSM2KeySaveFun[2] = {sm2_public_key_info_to_pem, sm2_private_key_info_to_pem};
enc_sm2_key_load_fun encSM2KeyLoadFun[2] = {sm2_public_key_info_from_pem, sm2_private_key_info_from_pem};
void encSM2KeySave(const SM2_KEY *key, uint8_t **buf, size_t *len, ENC_TYPE_SM2 type)
{
    FILE *fp = tmpfile();

    // 保存sm2密钥为pem格式公钥
    encSM2KeySaveFun[type](key, fp);

    *len = ftell(fp);
    *buf = (uint8_t *)malloc(*len);

    // 读取文件
    rewind(fp);
    fread(*buf, *len, 1, fp);
    fclose(fp);
}

void encSM2KeyLoad(SM2_KEY *key, const uint8_t *buf, size_t len, ENC_TYPE_SM2 type)
{
    FILE *fp = tmpfile();

    // 写入文件
    fwrite(buf, len, 1, fp);
    rewind(fp);

    // 读取sm2密钥
    encSM2KeyLoadFun[type](key, fp);

    fclose(fp);
}