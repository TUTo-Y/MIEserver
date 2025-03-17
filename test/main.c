#include <stdio.h>
#include <stdlib.h>
#include <gmssl/sm2.h>
int main()
{
    SM2_KEY key;
    SM2_KEY key_public;
    uint8_t buf[SM2_PRIVATE_KEY_BUF_SIZE] = {0};
    uint8_t *out = buf;
    uint8_t **out2 = &out;
    uint8_t *out3 = NULL;

    size_t outlen = 0;

    // 生成sm2密钥
    sm2_key_generate(&key);

    FILE *fp = tmpfile();

    // // 保存sm2密钥为pem格式公钥
    // sm2_public_key_info_to_pem(&key, fp);
    // rewind(fp);
    // // 从pem格式公钥中恢复公钥
    // sm2_public_key_info_from_pem(&key_public, fp);
    // fclose(fp);

    // 保存sm2密钥为der格式公钥
    sm2_public_key_info_to_der(&key, &out3, &outlen);

    // 从der格式公钥中恢复公钥
    sm2_public_key_info_from_der(&key_public, &out3, &outlen);

    sm2_public_key_print(stdout, 0, 0, NULL, &key_public);
    sm2_public_key_print(stdout, 0, 0, NULL, &key);
    
    // 加密一段数据
    uint8_t data[] = "hello world";
    SM2_CIPHERTEXT ciphertext;
    sm2_do_encrypt(&key_public, data, sizeof(data), &ciphertext);
    
    // 解密
    uint8_t t1[0x100];
    uint8_t *t3 = t1;
    size_t t2 = 0;
    sm2_do_decrypt(&key, &ciphertext, t3, &t2);

    // 打印解密结果
    printf("plaintext: %s\n", t1);

    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <gmssl/sm2.h>
// int main()
// {
//     SM2_KEY key;
//     SM2_KEY key_public;
//     uint8_t buf[SM2_PRIVATE_KEY_BUF_SIZE] = {0};
//     uint8_t *out = buf;
//     uint8_t **out2 = &out;
//     size_t outlen = 0;

//     // 生成sm2密钥
//     sm2_key_generate(&key);

//     // 保存sm2密钥为der格式公钥
//     sm2_public_key_info_to_der(&key, out2, &outlen);

//     // 从der格式公钥中恢复公钥
//     sm2_public_key_info_from_der(&key_public, out2, &outlen);

//     sm2_public_key_print(stdout, 0, 0, NULL, &key_public);
//     sm2_public_key_print(stdout, 0, 0, NULL, &key);
    
//     // 加密一段数据
//     uint8_t data[] = "hello world";
//     SM2_CIPHERTEXT ciphertext;
//     sm2_do_encrypt(&key_public, data, sizeof(data), &ciphertext);
    
//     // 解密
//     uint8_t t1[0x100];
//     uint8_t *t3 = t1;
//     size_t t2 = 0;
//     sm2_do_decrypt(&key, &ciphertext, t3, &t2);

//     // 打印解密结果
//     printf("plaintext: %s\n", t1);

//     return 0;
// }