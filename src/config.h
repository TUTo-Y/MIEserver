#ifndef CONFIG_H
#define CONFIG_H

// 用户目录和文件
#define USER_DIR_PATH "user/"
#define USER_FILE_PATH "user/user"

// 私钥文件
#define SERVER_PRIVATE_FILE_PATH "private.pem"

// IP和端口
#define SERVER_IP "192.168.32.129"
#define SERVER_PORT 8899

// 最大连接数
#define FDS_NUM 128

// 最大超时
#define TIMEOUT 5000


#endif // CONFIG_H