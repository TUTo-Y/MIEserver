/**
 * user/user 文本文件 : 用户名hash:USER_TYPE
 * user/sm3hash 二进制文件: 用户名(0x10) 密码hash(sm3)
 *
 */
#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <pthread.h>

#include "enc.h"
#include "log.h"
#include "list.h"
#include "config.h"

#define NAME_SIZE 0x20

typedef enum
{
    USER_DOCTOR,
    USER_PATIENT
} USER_TYPE;

// 基础用户数据
typedef struct
{
    uint8_t NameHash[SM3_DIGEST_SIZE]; // 用户名的hash
    uint8_t PassHash[SM3_DIGEST_SIZE]; // 密码的hash
    char Name[NAME_SIZE];              // 用户名
} user_base;

// 医生用户
typedef struct
{
} user_doctor;

// 病人用户
typedef struct
{
} user_patient;

// 用户
typedef struct
{
    USER_TYPE type; // 用户类型
    user_base base; // 基础数据

    union
    {
        user_doctor doctor;
        user_patient patient;
    };
} user_type;

// IP连接的用户
typedef struct
{
    SM2_KEY key; // 客户端公钥

    char IP[0x10]; // IP
    uint16_t port; // port

    user_type *user; // 用户
} user_IP;

extern list user;

typedef struct
{
    // 图像
    char *img1;
    char *img2;
    int w, h;
    size_t size; // 嵌入的数据大小

    // 图像的额外数据
    char *m;
    int mSize;
} user_img_data;

// 等待检查的用户
typedef struct
{
    struct pollfd *fds;
    user_IP *uds;

    // 图像密钥
    uint64_t key1;
    uint8_t key2[SM4_KEY_SIZE * 2];

    // 图像数据
    user_img_data imgData[5];
    int imgNum; // 图像数量
} user_wait;

// 等待治疗的医生
typedef struct
{
    struct pollfd *fds;
    user_IP *uds;
} user_wait2;

extern list userWait;  // 等待检查的用户
extern list userWait2; // 等待治疗的医生

void userInit();

void userQuit();

// 根据用户名的哈希值查找用户是否存在
user_type *userFindUser(uint8_t *userHash);

// 添加用户
void userAddUser(USER_TYPE type, char NameHash[SM3_DIGEST_SIZE], char PassHash[SM3_DIGEST_SIZE], char Name[0x20]);

void userFreeWait(user_wait *wait);

#endif // USER_H