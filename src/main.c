#include <stdio.h>
#include <gmssl/sm3.h>

int main()
{
    uint8_t dgst[SM3_DIGEST_SIZE];
    SM3_CTX *ctx;
    sm3_init(ctx);
    sm3_update(ctx, "abc", 3);
    sm3_finish(ctx, dgst);
    
    // 输出
    for (int i = 0; i < 32; i++)
    {
        printf("%02x", dgst[i]);
    }
    printf("\n");
    return 0;
}