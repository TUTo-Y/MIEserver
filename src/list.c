#include "list.h"

// 用于初始化list
#define LIST_FD_BK_INIT(l)       \
    {                            \
        (l)->fd = (l)->bk = (l); \
    }

#define LIST_NODE_INIT(l)   \
    {                       \
        (l)->size = 0;      \
        LIST_FD_BK_INIT(l); \
    }

#define LIST_UNLINK(node, tmp)     \
    {                              \
        (tmp) = (node);            \
        (tmp)->fd->bk = (tmp)->bk; \
        (tmp)->bk->fd = (tmp)->fd; \
                                   \
        LIST_FD_BK_INIT(tmp);      \
    }

void listInitList(list *l)
{
    if (l == NULL)
        return;

    LIST_NODE_INIT(l);
}

list *listCreateNode()
{
    list *node = NULL;

    // 创建节点
    node = (list *)malloc(sizeof(list));
    if (node == NULL)
        return NULL;

    // 初始化节点
    LIST_NODE_INIT(node);

    return node;
}

list *listDataToNode(list *node, const void *data, size_t size, bool copy)
{
    if (node == NULL ||
        (copy == true && data == NULL))
        return NULL;

    // 复制数据
    if (copy == true)
    {
        node->data = malloc(size);
        if (node->data == NULL)
            return NULL;
        memcpy(node->data, data, size);
    }
    // 不复制数据，只复制指针
    else
    {
        node->data = (void *)data;
    }

    return node;
}

void listAddNodeInStart(list *l, list *node)
{
    if (l == NULL || node == NULL)
        return;

    list *tmp = l->bk;

    l->bk = node;
    node->fd = l;

    tmp->fd = node;
    node->bk = tmp;

    ++l->count;
}

void listAddNodeInEnd(list *l, list *node)
{
    if (l == NULL || node == NULL)
        return;

    list *tmp = l->fd;

    l->fd = node;
    node->bk = l;

    tmp->bk = node;
    node->fd = tmp;

    ++l->count;
}

list *listGetNodeFromStart(list *l)
{
    if (l == NULL || l->count == 0)
        return NULL;

    // list *node = l->bk;
    // list *bck = node->bk;

    // // 解链
    // l->bk = bck;
    // bck->fd = l;

    // // 重置node定位
    // LIST_FD_BK_INIT(node);

    list *node = l->bk;
    list *tmp;
    LIST_UNLINK(node, tmp);

    // 减少count
    --l->count;

    return node;
}

list *listGetNodeFromEnd(list *l)
{
    if (l == NULL || l->count == 0)
        return NULL;

    // list *node = l->fd;
    // list *fwd = node->fd;

    // // 解链
    // l->fd = fwd;
    // fwd->bk = l;

    // // 重置node定位
    // LIST_FD_BK_INIT(node);

    list *node = l->fd;
    list *tmp;
    LIST_UNLINK(node, tmp);

    // 减少count
    --l->count;

    return node;
}

void listDeleteNodeData(list *node, void (*freeFun)(void *))
{
    if (node == NULL)
        return;

    // 释放数据
    if (freeFun && node->data)
        freeFun(node->data);

    node->data = NULL;
}

void listDeleteNode(list *l, list *node, void (*freeFun)(void *))
{
    if (node == NULL || node == l)
        return;

    // 释放节点数据
    listDeleteNodeData(node, freeFun);

    // 若不是单个节点，则将节点从链表中取出
    if (node->fd != node || node->bk != node)
    {
        list *fd = node->fd;
        list *bk = node->bk;

        // 解链
        fd->bk = bk;
        bk->fd = fd;

        // 减少count
        --l->count;
    }

    // 释放节点
    free(node);

    return;
}

void listDeleteList(list *l, void (*freeFun)(void *))
{
    if (l == NULL)
        return;

    // 释放所有节点
    while (l->fd != l)
        listDeleteNode(l, l->fd, freeFun);

    return;
}

list *listSearchDataAddr(list *l, const void *data)
{
    if (l == NULL)
        return NULL;

    list *node = l->fd;
    while (node != l)
    {
        if (node->data == data)
            return node;
        node = node->fd;
    }

    return NULL;
}

void listDoFromStart(list *l, void (*doData)(list *l, list *node, void *arg), void *arg)
{
    if (l == NULL || doData == NULL)
        return;

    list *node = l->bk;
    list *bck = node->bk;

    while (node != l)
    {
        doData(l, node, arg);
        node = bck;
        bck = node->bk;
    }
}

void listDoFromEnd(list *l, void (*doData)(list *l, list *node, void *arg), void *arg)
{
    if (l == NULL || doData == NULL)
        return;

    list *node = l->fd;
    list *fwd = node->fd;

    while (node != l)
    {
        doData(l, node, arg);
        node = fwd;
        fwd = node->fd;
    }
}