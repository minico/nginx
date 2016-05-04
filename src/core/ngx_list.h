
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

//list是由一些局部连续的内存(part)链接而成；
//向list push的时候如果当前分配内存已经用完，则新申请一个part连到当前list上；
//使用上有点儿类似array，只不过array新申请内存后需要将当前元素拷贝的新内存上以保证连续性；
typedef struct ngx_list_part_s  ngx_list_part_t;

struct ngx_list_part_s {
    void             *elts; //xjzhang,指向该part对应的内存；
    ngx_uint_t        nelts;//xjzhang,该part中已经使用的元素个数；
    ngx_list_part_t  *next; //xjzhang,指向该list中的下一个part;
};


typedef struct {
    ngx_list_part_t  *last;  //xjzhang,指向list中的最后一个part；
    ngx_list_part_t   part;	 //xjzhang, list中的第一个part;
    size_t            size;  //xjzhang,每个part中的元素大小，list创建的时候指定，以后不再改变；
    ngx_uint_t        nalloc;//xjzhang,每个part中的元素个数，list创建的时候指定，以后不再改变；
    ngx_pool_t       *pool;
} ngx_list_t;


ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }

    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return NGX_OK;
}


/*
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */

//xjzhang, 获取一个可用的元素，返回其地址；
void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
