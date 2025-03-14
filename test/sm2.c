#include <stdio.h>
#include <stdlib.h>
#include <gmssl/sm2.h>

int main()
{
    SM2_KEY key;
    uint8_t buf[SM2_PRIVATE_KEY_BUF_SIZE] = {0};
    uint8_t *out = buf;
    size_t outlen = 0;
    FILE *fp;
    // 生成密钥
    sm2_key_generate(&key);
    // 
    
    // 保存der格式公钥
    sm2_public_key_info_to_der(&key, &out, &outlen);
    fp = fopen("public.der", "wb");
    fwrite(out, 1, outlen, fp);
    fclose(fp);
    printf("成功保存def格式公钥\n");

    // 保存pem格式公钥
    fp = fopen("public.pem", "w");
    sm2_public_key_info_to_pem(&key, fp);
    fclose(fp);
    printf("成功保存pem格式公钥\n");

    // 保存der私钥
    sm2_private_key_info_to_der(&key, &out, &outlen);
    fp = fopen("private.der", "wb");
    fwrite(out, 1, outlen, fp);
    fclose(fp);
    printf("成功保存der格式私钥\n");

    // 保存pem私钥
    fp = fopen("private.pem", "w");
    sm2_private_key_info_to_pem(&key, fp);
    fclose(fp);
    printf("成功保存pem格式私钥\n");
    
    return 0;
}