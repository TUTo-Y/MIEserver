#include <gmssl/sm3.h>
#include <gmssl/sm4.h>

#ifdef _MSC_VER
// MSVC 编译器
__declspec(align(16)) typedef struct
#else
// GCC/Clang 编译器
typedef struct __attribute__((aligned(0x10)))
#endif
{
    wchar_t name[0x10];
    wchar_t age[0x4];
    wchar_t state[0x10];
    wchar_t advice[0x20];
} PACK;




int main()
{
    char plain[16] = {"1111111111"};
    char out[0x100] = {""};
    char out2[0x100] = {""};
    size_t len = 0;
    size_t len1 = 0;
    size_t len2 = 0;
    char key[SM4_KEY_SIZE] = {"这是key"};
    char iv[SM4_KEY_SIZE] = {"这是iv"};

    SM4_CBC_CTX ctx;
    sm4_cbc_encrypt_init(&ctx, key, iv);
    sm4_cbc_encrypt_update(&ctx, plain, sizeof(plain), out, &len1);
    printf("outlen1: %ld\n", len1);
    sm4_cbc_encrypt_finish(&ctx, out + len1, &len2);
    printf("outlen1: %ld\n", len2);
    len = len1 + len2;

    sm4_cbc_decrypt_init(&ctx, key, iv);
    sm4_cbc_decrypt_update(&ctx, out, len, out2, &len1);
    printf("outlen2: %ld\n", len1);
    sm4_cbc_decrypt_finish(&ctx, out2 + len1, &len2);
    printf("outlen2: %ld\n", len2);

    printf("out2: %s\n", out2);

    return 0;
}