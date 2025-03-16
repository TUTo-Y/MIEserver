#include "rand.h"
static uint8_t xorshift8_state = 1;
static uint16_t xorshift16_state = 1;
static uint32_t xorshift32_state = 1;
static uint64_t xorshift64_state = 1;

void xSrand8(uint8_t seed)
{
    xorshift8_state = seed;
}
void xSrand16(uint16_t seed)
{
    xorshift16_state = seed;
}
void xSrand32(uint32_t seed)
{
    xorshift32_state = seed;
}
void xSrand64(uint64_t seed)
{
    xorshift64_state = seed;
}
uint8_t xRand8()
{
    uint8_t x = xorshift8_state;
    x ^= x << 7;
    x ^= x >> 5;
    x ^= x << 3;
    xorshift8_state = x;
    return x;
}
uint16_t xRand16()
{
    uint16_t x = xorshift16_state;
    x ^= x << 13;
    x ^= x >> 9;
    x ^= x << 7;
    xorshift16_state = x;
    return x;
}
uint32_t xRand32()
{
    uint32_t x = xorshift32_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    xorshift32_state = x;
    return x;
}
uint64_t xRand64()
{
    uint64_t x = xorshift64_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    xorshift64_state = x;
    return x;
}
