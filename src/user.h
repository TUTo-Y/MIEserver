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
    char Name[NAME_SIZE];           // 用户名
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

void userInit();

void userQuit();

// 根据用户名的哈希值查找用户是否存在
user_type *userFindUser(uint8_t *userHash);

// 添加用户
void userAddUser(USER_TYPE type, char NameHash[SM3_DIGEST_SIZE], char PassHash[SM3_DIGEST_SIZE], char Name[0x20]);

#endif // USER_H