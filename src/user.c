#include "user.h"

list user;
list userWait;


void userInit()
{
    // 初始化用户链表
    listInitList(&user);
    listInitList(&userWait);

    // 打开用户文件
    FILE *userFile = fopen(USER_FILE_PATH, "r");
    if (userFile == NULL)
    {
        PERROR("打开用户文件失败");
        exit(-1);
    }

    // 依次读取每一个用户
    char hash_s[SM3_DIGEST_SIZE * 2 + 1] = {0};
    int flag = 0;
    while (fscanf(userFile, "%64s:%d", hash_s, &flag) != -1)
    {
        // 转换NameHash
        char NameHash[SM3_DIGEST_SIZE] = {0};
        encStr2Hash(hash_s, NameHash);

        // 打开用户配置文件
        char path[0x100] = {0};
        strncpy(path, USER_DIR_PATH, 0x100);
        strncat(path, hash_s, 0x100 - strlen(path));
        FILE *userConfig = fopen(path, "rb");
        if (userConfig == NULL)
        {
            ERROR("打开用户配置文件失败: %s\n", hash_s);
            exit(-1);
        }

        // 读取基础数据
        user_type type = {0};
        type.type = flag;
        memcpy(type.base.NameHash, NameHash, SM3_DIGEST_SIZE);
        fread(type.base.Name, NAME_SIZE, 1, userConfig);
        fread(type.base.PassHash, SM3_DIGEST_SIZE, 1, userConfig);

        // 根据docker和patient类型读取数据
        switch (flag)
        {
        case USER_DOCTOR:
            // fread(&type.doctor, sizeof(user_doctor), 1, userConfig);
            break;
        case USER_PATIENT:
            // fread(&type.patient, sizeof(user_patient), 1, userConfig);
            break;
        }

        // 关闭用户配置文件
        fclose(userConfig);

        // 写入链表
        listAddNodeInEnd(&user, listDataToNode(listCreateNode(), &type, sizeof(user_type), true));
    }

    fclose(userFile);
}

void userQuit()
{
    // 写入用户文件
    FILE *userFile = fopen(USER_FILE_PATH, "w");
    if (userFile == NULL)
    {
        PERROR("打开用户文件失败");
        exit(-1);
    }

    // 依次写入每一个用户
    list *node = NULL;
    while (node = listGetNodeFromStart(&user))
    {
        user_type *type = (user_type *)node->data;

        char hash_s[SM3_DIGEST_SIZE * 2 + 1] = {0};
        bin2hex(type->base.NameHash, hash_s, SM3_DIGEST_SIZE);

        // 向user文件写入数据
        fprintf(userFile, "%s:%d\n", hash_s, type->type);

        // 打开用户配置文件
        char path[0x100] = {0};
        strncpy(path, USER_DIR_PATH, 0x100);
        strncat(path, hash_s, 0x100 - strlen(path));
        FILE *userConfig = fopen(path, "wb");
        if (userConfig == NULL)
        {
            ERROR("打开用户配置文件失败: %s\n", path);
            continue;
        }

        // 写入基础数据
        fwrite(type->base.Name, NAME_SIZE, 1, userConfig);
        fwrite(type->base.PassHash, SM3_DIGEST_SIZE, 1, userConfig);

        // 根据docker和patient类型写入数据
        switch (type->type)
        {
        case USER_DOCTOR:
            // fwrite(&type->doctor, sizeof(user_doctor), 1, userConfig);
            break;
        case USER_PATIENT:
            // fwrite(&type->patient, sizeof(user_patient), 1, userConfig);
            break;
        }

        // 关闭文件
        fclose(userConfig);

        // 删除该节点
        listDeleteNode(NULL, node, free);
    }

    fclose(userFile);

    // 删除链表
    listDeleteList(&user, free);
    listDeleteList(&userWait, free);
}

user_type *userFindUser(uint8_t *userHash)
{
    list *node = user.fd;
    while (node != &user)
    {
        user_type *type = (user_type *)node->data;

        if (encHashCom((uint8_t *)type->base.NameHash, (uint8_t *)userHash) == 0)
            return type;

        node = node->fd;
    }
    return NULL;
}

void userAddUser(USER_TYPE type, char NameHash[SM3_DIGEST_SIZE], char PassHash[SM3_DIGEST_SIZE], char Name[NAME_SIZE])
{
    // 创建用户
    user_type tmp = {0};
    tmp.type = type;
    memcpy(tmp.base.NameHash, NameHash, SM3_DIGEST_SIZE);
    memcpy(tmp.base.PassHash, PassHash, SM3_DIGEST_SIZE);
    memcpy(tmp.base.Name, Name, NAME_SIZE);

    // 写入链表
    listAddNodeInEnd(&user, listDataToNode(listCreateNode(), &tmp, sizeof(user_type), true));
}