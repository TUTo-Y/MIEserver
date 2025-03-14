#include "web.h"

int serverInit(const char *ip, int port)
{
    int sockfd;
    struct sockaddr_in server_addr;

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        PERROR("socket创建失败");
        exit(EXIT_FAILURE);
    }

    // 配置服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // 绑定套接字到IP地址和端口
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind失败");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 监听端口，等待客户端连接
    if (listen(sockfd, 32) == -1)
    {
        perror("listen失败");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    SUCESS("(%s : %d)服务器启动成功\n", ip, port);

    return sockfd;
}

int serverAccept(int server_fd, char IP[0x10], uint16_t *port)
{
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 接受客户端连接
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1)
    {
        perror("accept失败");
        return -1;
    }

    SUCESS("(%s : %d)新的客户端连接成功\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    if (IP)
        strncpy(IP, inet_ntoa(client_addr.sin_addr), 0x10);
    if (port)
        *port = ntohs(client_addr.sin_port);

    return client_fd;
}