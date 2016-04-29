
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

//xjzhang, 在内存池p中分配一个动态数组，其元素个数为size，每个元素的大小为n；
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


//xjzhang,释放array在内存池中占用的内存，使之可以被重新利用；
//但是仅限于该array是在内存池最后部分分配的情况；
void
ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;

    p = a->pool;

	//xjzhang,如果该array存储区是在内存池最后分配的，则将其所占内存回收，标记为空闲可用；
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

	//xjzhang,如果该array头部是在内存池最后分配的，则将其所占内存回收，标记为空闲可用；
    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}

//xjzhang, 在array a中分配下一个可用元素，并返回其地址；
void *
ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
    size_t       size;
    ngx_pool_t  *p;

	//xjzhang, 如果array已经没有可用元素
    if (a->nelts == a->nalloc) {

        /* the array is full */

        size = a->size * a->nalloc;

        p = a->pool;

		//xjzhang, 如果array是在pool最后部分分配的，并且pool
		//仍有可用空间，则在pool后面新分配一个元素大小的内存；
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
			//xjzhang, 如果array不是在pool最后部分分配的，
			//则在该pool中重新分配当前array两倍大小的空间作为新array；
			//并把原先array内容拷贝的新array中，这类似vector动态增长原理；
            new = ngx_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, size);
            a->elts = new;
            a->nalloc *= 2;
        }
    }

	//xjzhang, 分配完毕将已使用元素数加1，并返回新分配的元素地址；
    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    return elt;
}


//xjzhang, 在array a中分配n个连续可用元素，并返回其地址；
//具体实现原理同ngx_array_push()类似；
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
