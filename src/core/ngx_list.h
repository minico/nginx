
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

//list����һЩ�ֲ��������ڴ�(part)���Ӷ��ɣ�
//��list push��ʱ�������ǰ�����ڴ��Ѿ����꣬��������һ��part������ǰlist�ϣ�
//ʹ�����е������array��ֻ����array�������ڴ����Ҫ����ǰԪ�ؿ��������ڴ����Ա�֤�����ԣ�
typedef struct ngx_list_part_s  ngx_list_part_t;

struct ngx_list_part_s {
    void             *elts; //xjzhang,ָ���part��Ӧ���ڴ棻
    ngx_uint_t        nelts;//xjzhang,��part���Ѿ�ʹ�õ�Ԫ�ظ�����
    ngx_list_part_t  *next; //xjzhang,ָ���list�е���һ��part;
};


typedef struct {
    ngx_list_part_t  *last;  //xjzhang,ָ��list�е����һ��part��
    ngx_list_part_t   part;	 //xjzhang, list�еĵ�һ��part;
    size_t            size;  //xjzhang,ÿ��part�е�Ԫ�ش�С��list������ʱ��ָ�����Ժ��ٸı䣻
    ngx_uint_t        nalloc;//xjzhang,ÿ��part�е�Ԫ�ظ�����list������ʱ��ָ�����Ժ��ٸı䣻
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

//xjzhang, ��ȡһ�����õ�Ԫ�أ��������ַ��
void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
