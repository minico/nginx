
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef _NGX_QUEUE_H_INCLUDED_
#define _NGX_QUEUE_H_INCLUDED_

//xjzhang,  用双向循环链表实现了一个queue;
//queue有头结点，头结点不存放数据；
//实际上从ngx_queue_s的定义可以看出，所有节点都不包含数据；
//那么如果用这个queue来阻止某个类型的数据呢？
//如果某个类型的数据需要使用queue来组织，则需要在该类型中定义一个ngx_queue_t变量；
//然后将该类型每个实例中的ngx_queue_t变量连接起来即可；
//那么如何通过ngx_queue_t变量来获取其对应的数据呢？
//这就需要使用后面定义的ngx_queue_data();
typedef struct ngx_queue_s  ngx_queue_t;

struct ngx_queue_s {
    ngx_queue_t  *prev;
    ngx_queue_t  *next;
};

//xjzhang,  初始化queue，只有一个头结点；
#define ngx_queue_init(q)                                                     \
    (q)->prev = q;                                                            \
    (q)->next = q

//xjzhang, 如果只有头节点则queue为空；
#define ngx_queue_empty(h)                                                    \
    (h == (h)->prev)

//xjzhang,在头结点h之后插入数据；
#define ngx_queue_insert_head(h, x)                                           \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x


#define ngx_queue_insert_after   ngx_queue_insert_head

//xjzhang,在queue h尾部插入数据；
#define ngx_queue_insert_tail(h, x)                                           \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x

//xjzhang,返回queue h的第一个数据节点；
#define ngx_queue_head(h)                                                     \
    (h)->next

//xjzhang,返回queue h的最后一个数据节点；
#define ngx_queue_last(h)                                                     \
    (h)->prev


#define ngx_queue_sentinel(h)                                                 \
    (h)


#define ngx_queue_next(q)                                                     \
    (q)->next


#define ngx_queue_prev(q)                                                     \
    (q)->prev


#if (NGX_DEBUG)

#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next;                                              \
    (x)->prev = NULL;                                                         \
    (x)->next = NULL

#else

//xjzhang, 移出节点x；
#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next

#endif


#define ngx_queue_split(h, q, n)                                              \
    (n)->prev = (h)->prev;                                                    \
    (n)->prev->next = n;                                                      \
    (n)->next = q;                                                            \
    (h)->prev = (q)->prev;                                                    \
    (h)->prev->next = h;                                                      \
    (q)->prev = n;

//xjzhang, 将queue n接到queue h后面；
//h和n指向的都是空的头结点；空节点n不会加入queue h中
#define ngx_queue_add(h, n)                                                   \
    (h)->prev->next = (n)->next;                                              \
    (n)->next->prev = (h)->prev;                                              \
    (h)->prev = (n)->prev;                                                    \
    (h)->prev->next = h;


//xjzhang, q是一个类型为ngx_queue_t的变量，link是类型type中定义的ngx_queue_t变量，也就是q；
//offsetof(type, link)取得q在类型type中的地址偏移量；
//(u_char *) q - offsetof(type, link)获取的便是类型type的实例变量地址；
#define ngx_queue_data(q, type, link)                                         \
    (type *) ((u_char *) q - offsetof(type, link))


ngx_queue_t *ngx_queue_middle(ngx_queue_t *queue);
void ngx_queue_sort(ngx_queue_t *queue,
    ngx_int_t (*cmp)(const ngx_queue_t *, const ngx_queue_t *));


#endif /* _NGX_QUEUE_H_INCLUDED_ */
