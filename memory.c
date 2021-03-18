#include <unistd.h>
#include <stddef.h>
#include "memory.h"

#define MIN_ALLOC(x) (x >= sizeof(struct m_block) ? sizeof(size_t) + x : sizeof(struct m_block))

// align  to 4 bytes (((x-1) / 4) * 4) + 4
#define ALIGN(x) ((((x - 1) >> 2) << 2) + 4)

typedef struct m_block m_block;

struct m_block
{
    size_t size;
    struct m_block *next;
};

m_block mem_list;

void split_block(m_block *prev, size_t size)
{
    m_block *curr, *leftover;
    curr = prev->next;

    if (curr->size >= size + sizeof(m_block))
    {
        // split block
        leftover = (m_block *)((char *)curr + size);
        prev->next = leftover;
        leftover->next = curr->next;
        leftover->size = curr->size - size;
        curr->size = size;
    }
    else
    {
        prev->next = curr->next;
    }
}

m_block *extend_heap(m_block *prev, size_t size)
{
    void *p, *n;
    m_block *curr;

    p = sbrk(0);
    // if sbreak fails return null
    if (sbrk(size) == (void *)-1)
        return NULL;
    n = sbrk(0);

    curr = (m_block *)p;
    curr->size = (char *)n - (char *)p;
    prev->next = curr;

    return curr;
}

void *malloc(size_t size)
{
    size_t size_al = ALIGN(size);
    m_block *prev, *curr;

    size_al = MIN_ALLOC(size_al);

    prev = &mem_list;
    curr = mem_list.next;
    while (curr)
    {
        if (curr->size >= size_al)
        {
            split_block(prev, size_al);
            break;
        }

        prev = curr;
        curr = curr->next;
    }

    if (!curr)
    {
        curr = extend_heap(prev, size_al);
        if (!curr)
            return NULL;
        split_block(prev, size_al);
    }
    curr->next = NULL;
    return &(curr->next);
}

void free(void *ptr)
{
    if (ptr == NULL)
        return;

    m_block *blk = (m_block *)((char *)ptr - sizeof(size_t));
    m_block *prev, *next;
    size_t top;

    blk->next = NULL;
    prev = &mem_list;
    next = mem_list.next;

    while (next && next < blk)
    {
        prev = next;
        next = next->next;
    }

    if (prev == &mem_list)
    {
        top = 0;
    }
    else
    {
        top = (size_t)prev + prev->size;
    }

    if (top == (size_t)blk)
    {
        /* Coalesce with previous block */
        prev->size += blk->size;
        blk = prev;
    }
    else
    {
        blk->next = next;
        prev->next = blk;
    }

    if ((size_t)blk + blk->size == (size_t)next)
    {
        /* Coalesce with next block */
        blk->size += next->size;
        blk->next = next->next;
    }
}