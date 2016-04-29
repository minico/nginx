
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

//xjzhang, ���ڴ��p�з���һ����̬���飬��Ԫ�ظ���Ϊsize��ÿ��Ԫ�صĴ�СΪn��
ngx_array_t *
ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size)
{
    ngx_array_t *a;

    a = ngx_palloc(p, sizeof(ngx_array_t));
    if (a == NULL) {
        return NULL;
    }

    if (ngx_array_init(a, p, n, size) != NGX_OK) {
        return NULL;
    }

    return a;
}


//xjzhang,�ͷ�array���ڴ����ռ�õ��ڴ棬ʹ֮���Ա��������ã�
//���ǽ����ڸ�array�����ڴ����󲿷ַ���������
void
ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;

    p = a->pool;

	//xjzhang,�����array�洢�������ڴ��������ģ�������ռ�ڴ���գ����Ϊ���п��ã�
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

	//xjzhang,�����arrayͷ�������ڴ��������ģ�������ռ�ڴ���գ����Ϊ���п��ã�
    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}

//xjzhang, ��array a�з�����һ������Ԫ�أ����������ַ��
void *
ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
    size_t       size;
    ngx_pool_t  *p;

	//xjzhang, ���array�Ѿ�û�п���Ԫ��
    if (a->nelts == a->nalloc) {

        /* the array is full */

        size = a->size * a->nalloc;

        p = a->pool;

		//xjzhang, ���array����pool��󲿷ַ���ģ�����pool
		//���п��ÿռ䣬����pool�����·���һ��Ԫ�ش�С���ڴ棻
        if ((u_char *) a->elts + size == p->d.last
            && p->d.last + a->size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += a->size;
            a->nalloc++;

        } else {
            /* allocate a new array */
			//xjzhang, ���array������pool��󲿷ַ���ģ�
			//���ڸ�pool�����·��䵱ǰarray������С�Ŀռ���Ϊ��array��
			//����ԭ��array���ݿ�������array�У�������vector��̬����ԭ��
            new = ngx_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, size);
            a->elts = new;
            a->nalloc *= 2;
        }
    }

	//xjzhang, ������Ͻ���ʹ��Ԫ������1���������·����Ԫ�ص�ַ��
    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    return elt;
}


//xjzhang, ��array a�з���n����������Ԫ�أ����������ַ��
//����ʵ��ԭ��ͬngx_array_push()���ƣ�
void *
ngx_array_push_n(ngx_array_t *a, ngx_uint_t n)
{
    void        *elt, *new;
    size_t       size;
    ngx_uint_t   nalloc;
    ngx_pool_t  *p;

    size = n * a->size;

    if (a->nelts + n > a->nalloc) {

        /* the array is full */

        p = a->pool;

        if ((u_char *) a->elts + a->size * a->nalloc == p->d.last
            && p->d.last + size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += size;
            a->nalloc += n;

        } else {
            /* allocate a new array */

            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

            new = ngx_palloc(p, nalloc * a->size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, a->nelts * a->size);
            a->elts = new;
            a->nalloc = nalloc;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;
}
