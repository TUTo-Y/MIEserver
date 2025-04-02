#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmssl/sm4.h>

int main()
{
    // 初始化密钥
    uint8_t key[SM4_KEY_SIZE * 2] = {0};
    memcpy(key, "1234567890qwepoirtylkjasd,m.ndf.][.cq=-c:OAJPOfojawpabxwbioqhdoi{OWIQFQD:JWQD}]", SM4_KEY_SIZE * 2);

    // 使用 SM4 加密
    uint8_t *plaintext = "我超级爱吃汉堡包哦, 我是2219878724@@@@@@\n我超级爱吃汉堡包哦, 我是2219878724@@@@@@\n我超级爱吃汉堡包哦, 我是2219878724@@@@@@\n我超级爱吃汉堡包哦, 我是2219878724@@@@@@\n我超级爱吃汉堡包哦, 我是2219878724@@@@@@\n我超级爱吃汉堡包哦, 我是2219878724@@@@@@\n我超级爱吃汉堡包哦, 我是2219878724@@@@@@\n我超级爱吃汉堡包哦, 我是2219878724@@@@@@\n";
    size_t plaintext_len = (strlen(plaintext) + 1);
    uint8_t *ciphertext = NULL;
    size_t ciphertext_len = 0;
    size_t t1 = 0;
    size_t t2 = 0;

    printf("size = %ld\n\n", (strlen(plaintext) + 1));

    // 加密
    ciphertext = malloc((strlen(plaintext) + 1) + 0x20);
    ciphertext_len = 0;
    
    SM4_CBC_CTX sm4_key;
    t1 = 0x10;
    sm4_cbc_encrypt_init(&sm4_key, &key[0], &key[SM4_KEY_SIZE]);
    sm4_cbc_encrypt_update(&sm4_key, (const uint8_t *)plaintext, plaintext_len, ciphertext, &t1);
    sm4_cbc_encrypt_finish(&sm4_key, ciphertext + t1, &t2);
    ciphertext_len = t1 + t2;
    printf("t1 : %ld\nt2 : %ld\n\n", t1, t2);

    // 解密
    plaintext = malloc(ciphertext_len);
    plaintext_len = 0;
    t1 = 0;
    sm4_cbc_decrypt_init(&sm4_key, &key[0], &key[SM4_KEY_SIZE]);
    sm4_cbc_decrypt_update(&sm4_key, ciphertext, ciphertext_len, plaintext, &t1);
    sm4_cbc_decrypt_finish(&sm4_key, plaintext + t1, &t2);

    printf("t1 : %ld\nt2 : %ld\n\n", t1, t2);
    printf("文本内容:");
    printf("[%s]\n", plaintext);
    return 0;
}