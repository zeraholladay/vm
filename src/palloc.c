#include <stdalign.h>
#include <stddef.h>

#include "oom_handlers.h"
#include "palloc.h"

#define STRIDE(size)                                                           \
  ((size) + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1)

#define INDEX(base, index, stride)                                             \
  ((Wrapper *)((char *)(base) + ((index) * (stride))))

extern oom_handler_t palloc_oom_handler;

Pool *pool_init(size_t count, size_t size) {
  Pool *p = calloc(1, sizeof *(p));
  size_t stride =
      STRIDE(sizeof(Wrapper) + size); // array-aligned Wrapper and size

  if (!p) {
    palloc_oom_handler(NULL, OOM_LOCATION);
    return NULL;
  }

  p->pool = calloc(count, stride);

  if (!p->pool) {
    free(p), p = NULL;
    palloc_oom_handler(NULL, OOM_LOCATION);
    return NULL;
  }

  p->count = count;
  p->stride = stride;
  pool_reset_all(p);

  return p;
}

void pool_destroy(Pool **p) {
  free((*p)->pool), (*p)->pool = NULL;
  free(*p), *p = NULL;
}

void *pool_alloc(Pool *p) {
  if (!p->free_list) {
    palloc_oom_handler(p, OOM_LOCATION "free_list empty");
    return NULL;
  }
  Wrapper *wrapper = p->free_list;
  p->free_list = wrapper->next_free;
  return &wrapper->ptr;
}

void pool_free(Pool *p, void *ptr) {
  Wrapper *wrapper = (Wrapper *)((void *)ptr - offsetof(Wrapper, ptr));
  wrapper->next_free = p->free_list;
  p->free_list = wrapper;
}

unsigned int pool_reset_from_mark(Pool *p, Wrapper *mark) {
  if (!mark || !p->free_list)
    return 0;

  Wrapper *cur = mark;
  Wrapper *stop_at = p->free_list;

  unsigned int num_freed;

  for (num_freed = 0; cur != stop_at; ++num_freed) {
    Wrapper *next = cur->next_free;
    pool_free(p, &cur->ptr);
    cur = next;
  }

  return num_freed;
}

void pool_reset_all(Pool *p) {
  size_t count = p->count;
  size_t stride = p->stride;

  Wrapper *cur;

  for (unsigned int i = 0; i < count - 1; ++i) {
    cur = INDEX(p->pool, i, stride);
    cur->next_free = INDEX(p->pool, i + 1, stride);
  }

  INDEX(p->pool, count - 1, stride)->next_free = NULL;
  p->free_list = INDEX(p->pool, 0, stride);
}
