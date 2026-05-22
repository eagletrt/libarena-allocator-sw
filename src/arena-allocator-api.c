/*!
 * \file arena-allocator-api.c
 * \date 2026-03-09
 * \authors Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Simple implementation of an arena allocator.
 * \details An arena allocator is a memory management technuque where a large
 *     block of memory (the arena) and uses it to manage smaller allocations
 *     efficiently.
 *     This method allows for faster allocation and deallocation since all
 *     memory can be freed at once when the arena is no longer needed.
 *     This implementation of the allocator uses a dynamic array which gets
 *     reallocated when more memory is needed to keep track of all the
 *     allocated memory.
 *
 * \warning Memory handled by the arena allocator should not be freed outside
 *      of the allocator itself.
 *      This might caused undefined behavior caused by double free of the same
 *      memory region.
 */
#include "arena-allocator.h"
#include "arena-allocator-api.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

/*!
 * \brief Creates a new item and adds it to the arena allocator array.
 * \details Based on the assumption that there is free space in the arena
 *      allocator array, it allocates a new item saving its information on the
 *      array itself.
 *
 * \param[in] harena A reference to the arena allocator handler.
 * \param[in] size   The size of the item to allocate in **bytes**.
 * \return A reference to the newly allocated item or NULL on failure.
 */
void *prv_arena_allocator_api_item_push(struct ArenaAllocatorHandler *harena, size_t size) {
    assert(harena != NULL);
    assert(harena->size < harena->capacity);
    assert(size > 0);

    void *item = malloc(size);
    if (item == NULL) {
        return NULL;
    }
    struct ArenaAllocatorItem *items = harena->items;
    items[harena->size].value = item;
    ++harena->size;
    return item;
}

/*!
 * \brief Creates a new item and adds it to the arena allocator array
 *      reallocating it if necessary.
 * \details If the array is full (i.e. size == capacity) it is reallocated with
 *      double its current capacity and then the item is pushed into it.
 *
 * \param[in] harena A reference to the arena allocator handler.
 * \param[in] size   The size of the item to allocate in **bytes**.
 * \return A reference to the newly allocated item or NULL on failure.
 */
void *prv_arena_allocator_api_item_push_with_alloc(struct ArenaAllocatorHandler *harena, size_t size) {
    assert(harena != NULL);
    assert(size > 0);

    if (harena->capacity == 0U) {
        /*!
         * If no memory is allocated for the array, it is allocated with a
         * capacity of 1.
         */
        struct ArenaAllocatorItem *items = (struct ArenaAllocatorItem *)malloc(sizeof(*items));
        if (items == NULL) {
            return NULL;
        }
        harena->items = items;
        harena->capacity = 1U;
    } else if (harena->size == harena->capacity) {
        /*!
         * Every other allocation doubles the size of the array to keep the
         * complexity amortized constant O(1)
         *
         * For more info see:
         *  - https://cs.stackexchange.com/questions/9380/why-is-push-back-in-c-vectors-constant-amortized
         */
        struct ArenaAllocatorItem *items = (struct ArenaAllocatorItem *)realloc(
            harena->items,
            harena->capacity * 2U * sizeof(*items));
        if (items == NULL) {
            return NULL;
        }
        harena->items = items;
        harena->capacity *= 2U;
    }
    return prv_arena_allocator_api_item_push(harena, size);
}

void arena_allocator_api_init(struct ArenaAllocatorHandler *harena) {
    if (harena == NULL) {
        return;
    }
    memset(harena, 0, sizeof(*harena));
}

void *arena_allocator_api_alloc(struct ArenaAllocatorHandler *harena, size_t size) {
    if (harena == NULL || size == 0U) {
        return NULL;
    }
    return prv_arena_allocator_api_item_push_with_alloc(harena, size);
}

void *arena_allocator_api_calloc(struct ArenaAllocatorHandler *harena, size_t size, size_t count) {
    /*!
     * Calloc function its just a shorthand to write alloc with a size equal to
     * size * count
     */
    if (harena == NULL || size == 0U || count == 0U) {
        return NULL;
    }
    return arena_allocator_api_alloc(harena, size * count);
}

void arena_allocator_api_free(struct ArenaAllocatorHandler *harena) {
    if (harena == NULL || harena->items == NULL) {
        return;
    }
    /*! Free all individual items */
    for (size_t i = 0U; i < harena->size; ++i) {
        void *item = harena->items[i].value;
        if (item != NULL) {
            free(item);
        }
    }
    /*! Free the arena allocator array */
    free(harena->items);

    harena->size = 0U;
    harena->capacity = 0U;
    harena->items = NULL;
}
