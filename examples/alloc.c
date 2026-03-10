/*!
 * \file alloc.c
 * \date 2026-03-09
 * \authors Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Simple example of a vector-scalar multiplication with single
 *      allocations of different types using the arena allocator.
 * \details In the example a 3D vector structure consisting of three floats and
 *      another float are allocated and their product is saved in another vector
 *      allocated in the same way.
 *      At the end of the program all the allocated memory is freed to avoid
 *      memory leaks.
 */
#include <stdio.h>

#include "arena-allocator.h"
#include "arena-allocator-api.h"

/*!
 * \brief 3D vector structure
 */
struct Vec3 {
    float x; /*!< The x component of the vector */
    float y; /*!< The y component of the vector */
    float z; /*!< The z component of the vector */
};

/*!
 * \brief Utility function to print the content of a 3d vector
 *
 * \param[in] v The vector to print
 */
void print_vec3(struct Vec3 *v) {
    printf("[%4.2f %4.2f %4.2f]", v->x, v->y, v->z);
}

int main(void) {
    struct ArenaAllocatorHandler harena;
    arena_allocator_api_init(&harena);

    struct Vec3 *v = (struct Vec3 *)arena_allocator_api_alloc(&harena, sizeof(*v));
    float *k = (float *)arena_allocator_api_alloc(&harena, sizeof(*k));

    /*! Initialization of the vector and scalar values */
    v->x = 1.53f;
    v->y = 5.92f;
    v->z = 2.01f;
    *k = 2.5f;

    printf("%.2f * ", *k);
    print_vec3(v);
    printf(" = ");

    /*! Allocate and calculate a vector multiplication with a scalar value */
    struct Vec3 *w = (struct Vec3 *)arena_allocator_api_alloc(&harena, sizeof(*w));
    w->x = v->x * (*k);
    w->y = v->y * (*k);
    w->z = v->z * (*k);
    print_vec3(w);
    printf("\n");

    /*! Remember to always free the memory at the end of the program */
    arena_allocator_api_free(&harena);
    return 0;
}
