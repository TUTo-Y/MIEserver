#ifndef WEB_H
#define WEB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/mman.h>
#include <time.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <poll.h>

#include "log.h"

typedef uint64_t FLAG;
enum
{
    WEB_MSG_SUCESS,     // 成功
    WEB_MSG_ERROR,      // 错误
    WEB_MSG_DISCONNECT, // 断开连接

    WEB_MSG_CPUBLIC_KEY, // 发送公钥

    WEB_MSG_LOGIN,            // 登录
    WEB_MSG_LOGIN_DOCTOR,     // 登录医生
    WEB_MSG_LOGIN_PATIENT,    // 登录患者
    WEB_MSG_REGISTER,         // 注册
    WEB_MSG_REGISTER_DOCTOR,  // 注册医生
    WEB_MSG_REGISTER_PATIENT, // 注册患者

    WEB_MSG_LOGIN_ERROR_USER, // 登录用户不存在
    WEB_MSG_LOGIN_ERROR_PASS, // 登录密码错误
    WEB_MSG_LOGIN_ERROR_REPE, // 重复登录

    WEB_MSG_PAITENT_UPLOAD, // 患者上传图像及其数据
};


/**
 * \brief 初始化服务器
 * \param ip 服务器ip
 * \param port 服务器端口
 * \return 服务器文件描述符
 */
int serverInit(const char *ip, int port);

/**
 * \brief 接受连接
 * \param 服务器文件描述符
 * \return 客户端文件描述符
 */
int serverAccept(int server_fd, char IP[0x10], uint16_t *port);

/**
 * \brief 发送flag
 * \param fd 文件描述符
 * \param flag 标志
 */
static inline void webSendFlag(int fd, FLAG flag)
{
    send(fd, (const void *)&flag, sizeof(FLAG), 0);
}

/**
 * \brief 接收flag
 * \param fd 文件描述符
 * \return 标志
 */
static inline FLAG webRecvFlag(int fd)
{
    FLAG flag;
    int ret = recv(fd, (void *)&flag, sizeof(FLAG), MSG_WAITALL);
    if (ret <= 0)
        flag = WEB_MSG_DISCONNECT;
    return flag;
}

/**
 * \brief 发送数据
 * \param data 数据
 * \param size 数据大小
 */
static inline void webSendData(int fd, const char *data, size_t size)
{
    // 发送长度
    send(fd, (const void *)&size, sizeof(size), 0);
    // 发送大小
    send(fd, (const void *)data, size, 0);
}

/**
 * \brief 接收数据
 * \param data 数据
 * \param size 数据大小
 */
static inline char *webRecvData(int fd, char **data, size_t *size)
{
    size_t s;
    char *d = NULL;

    // 接收长度
    recv(fd, (void *)&s, sizeof(size_t), MSG_WAITALL);
    // 接收数据
    d = (uint8_t *)malloc(s);
    recv(fd, (void *)d, s, MSG_WAITALL);

    // 赋值
    if (data)
        *data = d;
    if (size)
        *size = s;

    return d;
}

#endif // WEB_H