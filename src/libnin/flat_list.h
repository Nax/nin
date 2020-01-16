#ifndef NIN_FLAT_LIST_H
#define NIN_FLAT_LIST_H 1

/*
 * A flat list is a data structure having linked list semantics
 * while being implemented on top of a pair of memory blocks.
 *
 * The structure can also be accessed by index, but the semantics are largely
 * different than with an array. The index can be thought of as the equivalent
 * of a pointer with respect to classical linked lists.
 *
 * The list is doubly-chained for convinience.
 */

#include <stdlib.h>
#include <libnin/util.h>

#define FLAT_LIST_DECL_INIT(name, prefix, type) \
void CONCAT(prefix, Init)(name* list)

#define FLAT_LIST_IMPL_INIT(name, prefix, type) \
FLAT_LIST_DECL_INIT(name, prefix, type)         \
{                                               \
    list->data = malloc(8 * sizeof(type));      \
    list->next = malloc(8 * sizeof(int));       \
    list->prev = malloc(8 * sizeof(int));       \
    list->idxFirst = -1;                        \
    list->idxLast = -1;                         \
    list->count = 0;                            \
    list->capacity = 8;                         \
    list->free = malloc(4 * sizeof(int));       \
    list->freeCount = 0;                        \
    list->freeCapacity = 4;                     \
}

#define FLAT_LIST_DECL_CLEAR(name, prefix, type) \
void CONCAT(prefix, Clear)(name* list)

#define FLAT_LIST_IMPL_CLEAR(name, prefix, type)\
FLAT_LIST_DECL_CLEAR(name, prefix, type)        \
{                                               \
    list->idxFirst = -1;                        \
    list->idxLast = -1;                         \
    list->count = 0;                            \
    list->freeCount = 0;                        \
}

#define FLAT_LIST_DECL_FREE(name, prefix, type) \
void CONCAT(prefix, Free)(name* list)

#define FLAT_LIST_IMPL_FREE(name, prefix, type) \
FLAT_LIST_DECL_FREE(name, prefix, type)         \
{                                               \
    free(list->data);                           \
    free(list->next);                           \
    free(list->prev);                           \
    free(list->free);                           \
    list->data = NULL;                          \
    list->next = NULL;                          \
    list->prev = NULL;                          \
    list->free = NULL;                          \
}

#define FLAT_LIST_DECL_ALLOC(name, prefix, type) \
int CONCAT(prefix, Alloc)(name* list)

#define FLAT_LIST_IMPL_ALLOC (name, prefix, type)   \
FLAT_LIST_DECL_ALLOC(name, prefix, type)            \
{                                                   \
    int tmp;                                        \
    int newCapacity;                                \
                                                    \
    if (list->count < list->capacity)               \
    {                                               \
        tmp = list->count++;                        \
    }                                               \
    else if (list->freeCount)                       \
    {                                               \
        tmp = list->free[--list->freeCount];        \
    }                                               \
    else                                            \
    {                                               \
        newCapacity = list->capacity + list->capacity / 2;                      \
        list->data = realloc(list->data, newCapacity * sizeof(*list->data));    \
        list->next = realloc(list->next, newCapacity * sizeof(*list->next));    \
        list->prev = realloc(list->prev, newCapacity * sizeof(*list->prev));    \
        list->capacity = newCapacity;                                           \
        tmp = list->count++;                                                    \
    }                                               \
                                                    \
    list->prev[tmp] = -1;                           \
    list->next[tmp] = -1;                           \
                                                    \
    return tmp;                                     \
}

#define FLAT_LIST_DECL_GET(name, prefix, type) \
type* CONCAT(prefix, Get)(name* list, int index)

#define FLAT_LIST_IMPL_GET (name, prefix, type)     \
FLAT_LIST_DECL_GET(name, prefix, type)              \
{                                                   \
    if (index < 0)                                  \
        return NULL;                                \
    return list->data + index;                      \
}

#define FLAT_LIST_DECL_NEXT(name, prefix, type) \
int CONCAT(prefix, Next)(name* list, int index)

#define FLAT_LIST_IMPL_NEXT (name, prefix, type)    \
FLAT_LIST_DECL_NEXT(name, prefix, type)             \
{                                                   \
    if (index < 0)                                  \
        return -1;                                  \
    return list->next[index];                       \
}

#define FLAT_LIST_DECL_HEAD(name, prefix, type) \
int CONCAT(prefix, Head)(name* list)

#define FLAT_LIST_IMPL_HEAD(name, prefix, type)     \
FLAT_LIST_DECL_HEAD(name, prefix, type)             \
{                                                   \
    return list->idxFirst;                          \
}

#define FLAT_LIST_DECL_PUSH(name, prefix, type)     \
int CONCAT(prefix, Push)(name* list, type* elem)

#define FLAT_LIST_IMPL_PUSH(name, prefix, type)     \
FLAT_LIST_DECL_PUSH(name, prefix, type)             \
{                                                   \
    int index;                                      \
                                                    \
    index = CONCAT(prefix, Alloc)(list);            \
    if (list->idxFirst < 0)                         \
    {                                               \
        list->idxFirst = index;                     \
    }                                               \
    else                                            \
    {                                               \
        list->next[list->idxLast] = index;          \
        list->prev[index] = list->idxLast;          \
    }                                               \
    list->idxLast = index;                          \
    return index;                                   \
}

#define FLAT_LIST_DECL(name, prefix, type)  \
    typedef struct                          \
    {                                       \
        type*   data;                       \
        int*    next;                       \
        int*    prev;                       \
        int     idxFirst;                   \
        int     idxLast;                    \
        int     count;                      \
        int     capacity;                   \
        int*    free;                       \
        int     freeCount;                  \
        int     freeCapacity;               \
    } name;                                 \
    FLAT_LIST_DECL_INIT(name, prefix, type);     \
    FLAT_LIST_DECL_CLEAR(name, prefix, type);    \
    FLAT_LIST_DECL_FREE(name, prefix, type);     \
    FLAT_LIST_DECL_ALLOC(name, prefix, type);    \
    FLAT_LIST_DECL_GET(name, prefix, type);      \
    FLAT_LIST_DECL_NEXT(name, prefix, type);     \
    FLAT_LIST_DECL_HEAD(name, prefix, type);     \
    FLAT_LIST_DECL_PUSH(name, prefix, type);

#define FLAT_LIST_IMPL(name, prefix, type)      \
    FLAT_LIST_IMPL_INIT(name, prefix, type)      \
    FLAT_LIST_IMPL_CLEAR(name, prefix, type)     \
    FLAT_LIST_IMPL_FREE(name, prefix, type)      \
    FLAT_LIST_IMPL_ALLOC(name, prefix, type)     \
    FLAT_LIST_IMPL_GET(name, prefix, type)       \
    FLAT_LIST_IMPL_NEXT(name, prefix, type)      \
    FLAT_LIST_IMPL_HEAD(name, prefix, type)      \
    FLAT_LIST_IMPL_PUSH(name, prefix, type)

#endif
