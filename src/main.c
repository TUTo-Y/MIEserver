#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>

#include "config.h"
#include "enc.h"
#include "web.h"
#include "user.h"

// test
#include "RDH.h"

#if 1
SM2_KEY keyServer;

void run();
void Check();
void Init();
void Quit();
void signalQuit(int signal);

// 退出信号量
sem_t sem_quit;

int main()
{
    Init();

    // 初始化poll
    struct pollfd *fds;
    fds = malloc(FDS_NUM * sizeof(struct pollfd));
    for (int i = 0; i < FDS_NUM; i++)
    {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }

    // 初始化用户集
    user_IP *uds;
    uds = malloc(FDS_NUM * sizeof(user_IP));
    memset(uds, 0, FDS_NUM * sizeof(user_IP));

    // 初始化客户端
    int server_fd = serverInit(SERVER_IP, SERVER_PORT);
    fds[0].fd = server_fd;

    while (1)
    {
        // 等待socket读相关事件
        int ready_num = poll(fds, FDS_NUM, TIMEOUT);

        // 退出
        if ((ready_num == -1) || (sem_trywait(&sem_quit) == 0))
        {
            break;
        }

        // 处理事件
        while (ready_num)
        {
            // 服务器socket的消息(新的连接)
            if (fds[0].revents == POLLIN)
            {
                int i;
                // 找到空闲位置
                for (i = 1; i < FDS_NUM; i++)
                {
                    // 将该客户端套接字，放到数组中有空缺的地方
                    if (fds[i].fd == -1)
                    {
                        // 初始化该结构体
                        memset(&uds[i], 0, sizeof(user_IP));

                        // 获取新连接
                        fds[i].fd = serverAccept(server_fd, uds[i].IP, &uds[i].port);

                        break;
                    }
                }
                if (i == FDS_NUM)
                {
                    PERROR("连接数已满");
                }
                fds[0].revents = 0;
            }

            // 其他消息
            else
            {
                for (int i = 1; i < FDS_NUM; i++)
                {
                    // 数据可读
                    if (fds[i].fd != -1 &&
                        fds[i].revents == POLLIN)
                    {
                        FLAG flag = webRecvFlag(fds[i].fd);
                        switch (flag)
                        {
                            // 断连
                        case WEB_MSG_DISCONNECT:
                        {
                            ERROR("(%s : %d)断开连接 : %s\n", uds[i].IP, uds[i].port, strerror(errno));
                            close(fds[i].fd);
                            fds[i].fd = -1;
                            break;
                        }

                            // 获取用户公钥
                        case WEB_MSG_CPUBLIC_KEY:
                        {
                            ENCkem data = (ENCkem)webRecvData(fds[i].fd, NULL, NULL); // 获取数据
                            uint8_t *pem = encKEMDec(data, &keyServer);               // 解密数据
                            size_t pem_len = encKEMSizeText(data);                    // 获取解密后数据长度
                            encSM2PublicLoad(&uds[i].key, pem, pem_len);              // 加载公钥
                            encKEMFree(data);
                            free(pem);
                            SUCESS("(%s : %d)成功添加用户公钥\n", uds[i].IP, uds[i].port);

                            break;
                        }

                        // 登录
                        case WEB_MSG_LOGIN:
                        {
                            // 获取kem包
                            ENCkem kem = (ENCkem)webRecvData(fds[i].fd, NULL, NULL);

                            // 解包
                            uint8_t *buf = encKEMDec(kem, &keyServer);
                            uint8_t user_hash[SM3_DIGEST_SIZE] = {0};
                            uint8_t pass_hash[SM3_DIGEST_SIZE] = {0};
                            char name[0x20] = {0};
                            memcpy(user_hash, buf, SM3_DIGEST_SIZE);
                            memcpy(pass_hash, buf + SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
                            memcpy(name, buf + SM3_DIGEST_SIZE * 2, 0x20);

                            // 释放数据
                            encKEMFree(kem);
                            free(buf);

                            // 检查用户是否已经存在
                            user_type *type = userFindUser((char *)user_hash);
                            if (type == NULL)
                            {
                                webSendFlag(fds[i].fd, WEB_MSG_ERROR);
                                webSendFlag(fds[i].fd, WEB_MSG_LOGIN_ERROR_USER);
                                ERROR("(%s : %d)尝试登录不存在的用户 : %s\n", uds[i].IP, uds[i].port, name);
                                break;
                            }

                            // 检查用户密码是否正确
                            if (encHashCom((uint8_t *)type->base.PassHash, pass_hash) != 0)
                            {
                                webSendFlag(fds[i].fd, WEB_MSG_ERROR);
                                webSendFlag(fds[i].fd, WEB_MSG_LOGIN_ERROR_PASS);
                                ERROR("(%s : %d)尝试使用错误密码登录用户 : %s\n", uds[i].IP, uds[i].port, name);
                                break;
                            }

                            // 检查是否重复登录
                            bool repe = false;
                            for (int j = 1; j < FDS_NUM; j++)
                            {
                                if (
                                    fds[j].fd != -1 &&
                                    uds[j].user != NULL &&
                                    encHashCom((uint8_t *)uds[j].user->base.NameHash, (uint8_t *)user_hash) == 0)
                                {
                                    webSendFlag(fds[i].fd, WEB_MSG_ERROR);
                                    webSendFlag(fds[i].fd, WEB_MSG_LOGIN_ERROR_REPE);
                                    ERROR("(%s : %d)重复登录用户 : %s\n", uds[i].IP, uds[i].port, name);
                                    repe = true;
                                    break;
                                }
                            }
                            if (repe)
                                break;

                            // 绑定当前IP到用户
                            uds[i].user = type;

                            // 发送登录成功
                            webSendFlag(fds[i].fd, WEB_MSG_SUCESS);
                            webSendFlag(fds[i].fd, type->type == USER_DOCTOR ? WEB_MSG_LOGIN_DOCTOR : WEB_MSG_LOGIN_PATIENT);

                            SUCESS("(%s : %d)用户登录 : %s\n", uds[i].IP, uds[i].port, name);
                            break;
                        }

                        // 注册
                        case WEB_MSG_REGISTER:
                        {

                            // 注册的类型
                            FLAG flag = webRecvFlag(fds[i].fd);

                            // 获取kem包
                            ENCkem kem = (ENCkem)webRecvData(fds[i].fd, NULL, NULL);

                            // 解包
                            uint8_t *buf = encKEMDec(kem, &keyServer);
                            uint8_t user_hash[SM3_DIGEST_SIZE] = {0};
                            uint8_t pass_hash[SM3_DIGEST_SIZE] = {0};
                            char name[0x20] = {0};
                            memcpy(user_hash, buf, SM3_DIGEST_SIZE);
                            memcpy(pass_hash, buf + SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
                            memcpy(name, buf + SM3_DIGEST_SIZE * 2, 0x20);

                            // 检查用户是否已经存在
                            user_type *type = userFindUser((char *)user_hash);
                            if (type != NULL)
                            {
                                webSendFlag(fds[i].fd, WEB_MSG_ERROR);
                                ERROR("(%s : %d)重复注册%s : %s\n", uds[i].IP, uds[i].port, flag == WEB_MSG_REGISTER_DOCTOR ? "医生" : "患者", name);
                                break;
                            }

                            // 添加用户
                            userAddUser(flag == WEB_MSG_REGISTER_DOCTOR ? USER_DOCTOR : USER_PATIENT, (char *)user_hash, (char *)pass_hash, name);

                            // 发送注册结果
                            webSendFlag(fds[i].fd, WEB_MSG_SUCESS);

                            SUCESS("(%s : %d)注册%s : %s\n", uds[i].IP, uds[i].port, flag == WEB_MSG_REGISTER_DOCTOR ? "医生" : "患者", name);
                            break;
                        }

                        // 患者上传图像及数据
                        case WEB_MSG_PAITENT_UPLOAD:
                        {
                            user_wait wait;
                            // 接受密钥并解密
                            ENCkem kem = (ENCkem)webRecvData(fds[i].fd, NULL, NULL);
                            uint8_t *data = encKEMDec(kem, &keyServer);
                            memcpy((void *)&wait.key1, data, sizeof(uint64_t));
                            memcpy((void *)&wait.key2, data + sizeof(uint64_t), SM4_KEY_SIZE * 2);

                            // 接受图像
                            size_t wh = webRecvFlag(fds[i].fd);
                            wait.w = wh >> 32;
                            wait.h = wh & 0xffffffff;
                            wait.img1 = webRecvData(fds[i].fd, NULL, NULL);
                            wait.img2 = webRecvData(fds[i].fd, NULL, NULL);

                            // 接受m
                            wait.mSize = webRecvFlag(fds[i].fd);
                            wait.m = webRecvData(fds[i].fd, NULL, NULL);

                            // 设置list
                            wait.fds = &fds[i];
                            wait.uds = &uds[i];
                            listAddNodeInEnd(&userWait, listDataToNode(listCreateNode(), &wait, sizeof(user_wait), true));

                            // 释放数据
                            encKEMFree(kem);
                            free(data);

                            run();
                            break;
                        }
                        case WEB_MSG_DOCTOR_WAIT:
                        {
                            user_wait2 wait;
                            wait.fds = &fds[i];
                            wait.uds = &uds[i];
                            listAddNodeInEnd(&userWait2, listDataToNode(listCreateNode(), &wait, sizeof(user_wait2), true));

                            run();
                            break;
                        }
                        default:
                            DEBUG("(%s : %d)未知消息 : %lu\n", uds[i].IP, uds[i].port, flag);
                            break;
                        }

                        fds[0].revents = 0;
                    }
                }
            }

            ready_num--;
        }
    }

    // 退出所有连接
    close(server_fd);
    for (int i = 1; i < FDS_NUM; i++)
    {
        if (fds[i].fd != -1)
        {
            webSendFlag(fds[i].fd, WEB_MSG_DISCONNECT);
            close(fds[i].fd);
        }
    }

    free(fds);
    free(uds);
    Quit();

    // SUCESS("正常退出");
    return 0;
}

void Check()
{
    // 创建user目录
    struct stat st = {0};
    if (stat(USER_DIR_PATH, &st) == -1)
    {
        if (mkdir(USER_DIR_PATH, 0700) == -1)
        {
            PERROR("创建用户目录失败");
        }
        else
        {
            SUCESS("创建用户目录: %s\n", USER_DIR_PATH);
        }
    }

    // 检查用户文件
    FILE *file = fopen(USER_FILE_PATH, "r");
    if (file == NULL)
    {
        file = fopen(USER_FILE_PATH, "w");
        if (file == NULL)
        {
            PERROR("创建文件目录失败");
        }
        else
        {
            SUCESS("创建用户文件: %s\n", USER_FILE_PATH);
        }
    }
    fclose(file);
}

void Init()
{
    // 检查用户目录和文件
    Check();

    // 初始化用户文件
    userInit();

    // 读取私钥文件
    FILE *fp = fopen(SERVER_PRIVATE_FILE_PATH, "r");
    sm2_private_key_info_from_pem(&keyServer, fp);
    fclose(fp);

    // 初始化退出信号量
    sem_init(&sem_quit, 0, 0);

    // 注册退出信号
    if (signal(SIGINT, signalQuit) == SIG_ERR)
    {
        PERROR("无法注册信号处理函数");
        exit(-1);
    }
    if (signal(SIGTERM, signalQuit) == SIG_ERR)
    {
        PERROR("无法注册信号处理函数");
        exit(-1);
    }
}

void Quit()
{
    // 退出用户文件
    userQuit();

    // 销毁信号量
    sem_destroy(&sem_quit);
}

void signalQuit(int signal)
{
    sem_post(&sem_quit); // 释放信号量以退出主循环
}

void run()
{
    start:
    // 检查是否有匹配的
    if (userWait.count != 0 && userWait2.count != 0)
    {

        // 获取医生和患者
        list *node;
        node = listGetNodeFromStart(&userWait);
        user_wait *patient = ((user_wait *)node->data);
        free(node);

        node = userWait2.bk;
        DEBUG("匹配:医生(%s : %d) 患者(%s : %d)\n", ((user_wait *)node->data)->uds->IP, ((user_wait *)node->data)->uds->port, patient->uds->IP, patient->uds->port);

        /* 向患者确认是否允许医生查看数据 */
        webSendFlag(patient->fds->fd, WEB_MSG_PAITENT_CHECK);
        webSendData(patient->fds->fd, (char *)&((user_wait *)node->data)->uds->user->base.Name, NAME_SIZE);
        if (webRecvFlag(patient->fds->fd) == WEB_MSG_PAITENT_YES)
        {
            // 取出医生
            node = listGetNodeFromStart(&userWait2);
            user_wait2 *doctor = (user_wait2 *)node->data;
            free(node);

            /* 向医生发送数据 */
            webSendFlag(doctor->fds->fd, WEB_MSG_DOCTOR_WAIT_OVER);

            uint8_t key[sizeof(size_t) + SM4_KEY_SIZE * 2];
            memcpy(key, (void *)&patient->key1, sizeof(size_t));
            memcpy(key + sizeof(size_t), (void *)&patient->key2, SM4_KEY_SIZE * 2);
            ENCkem kem = (ENCkem)encKEMEnc((uint8_t *)key, sizeof(key), &doctor->uds->key);
            webSendData(doctor->fds->fd, (char *)kem, encKEMSizeKEM(kem));
            encKEMFree(kem);
            webSendFlag(doctor->fds->fd, ((size_t)patient->w )<< 32 | patient->h);
            webSendData(doctor->fds->fd, patient->img1, patient->w * patient->h);
            webSendData(doctor->fds->fd, patient->img2, patient->w * patient->h);

            webSendFlag(doctor->fds->fd, patient->mSize);
            webSendData(doctor->fds->fd, patient->m, patient->mSize);

            /* 接受医生的新数据 */
            FLAG flag = webRecvFlag(doctor->fds->fd);
            kem = (ENCkem)webRecvData(doctor->fds->fd, NULL, NULL);
            uint8_t *data = encKEMDec(kem, &keyServer);
            size_t data_size= encKEMSizeText(kem);
            encKEMFree(kem);

            /* 发送给患者 */
            webSendFlag(patient->fds->fd, WEB_MSG_PAITENT_DOWNLOAD);
            kem = encKEMEnc(data, data_size, &patient->uds->key);
            webSendData(patient->fds->fd, (char *)kem, encKEMSizeKEM(kem));
            encKEMFree(kem);
            free(data);
        }
    }
}

#else
int main()
{
    char str[0x100] = {"HelloWorld!"};
    uint8_t hash[SM3_DIGEST_SIZE];
    encHash((uint8_t *)str, strlen(str), (uint8_t *)hash);

    char hex[0x100] = {0};
    bin2hex(hash, hex, SM3_DIGEST_SIZE);
    printf("hash: %s\n", hex);
    hex2bin(hex, hash, SM3_DIGEST_SIZE);
    bin2hex(hash, hex, SM3_DIGEST_SIZE);
    printf("hash: %s\n", hex);

    return 0;
}
#endif