/*
 * -----------------------------------------------------------------------------
 * token_list.c
 *
 * Ordered token list implementation using a growable dynamic array.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "token_list.h"
#include <stdlib.h>  /* malloc, realloc, free */
#include <stdio.h>   /* fprintf, stderr */
#include <stddef.h>  /* NULL */

/*
 * tl_init - allocates initial storage for the token list.
 */
void tl_init(token_list_t *list) {
    list->tokens = (token_t *)malloc(TL_INIT_CAPACITY * sizeof(token_t));
    if (list->tokens == NULL) {
        fprintf(stderr, "tl_init: memory allocation failed\n");
        list->capacity = 0;
    } else {
        list->capacity = TL_INIT_CAPACITY;
    }
    list->count = 0;
}

/*
 * tl_grow - doubles the capacity of the token list when needed.
 * Returns 0 on success, -1 on allocation failure.
 */
static int tl_grow(token_list_t *list) {
    int new_cap = list->capacity * TL_GROWTH_FACTOR;
    token_t *new_buf = (token_t *)realloc(list->tokens, new_cap * sizeof(token_t));
    if (new_buf == NULL) {
        fprintf(stderr, "tl_grow: memory reallocation failed\n");
        return -1;
    }
    list->tokens = new_buf;
    list->capacity = new_cap;
    return 0;
}

/*
 * tl_add - appends a copy of the given token to the list.
 */
void tl_add(token_list_t *list, const token_t *tok) {
    if (list->count >= list->capacity) {
        if (tl_grow(list) != 0) {
            return; /* allocation failed; skip adding */
        }
    }
    list->tokens[list->count] = *tok;
    list->count++;
}

/*
 * tl_get - retrieves a pointer to the token at the given index.
 */
const token_t* tl_get(const token_list_t *list, int index) {
    if (index < 0 || index >= list->count) {
        return NULL;
    }
    return &list->tokens[index];
}

/*
 * tl_count - returns the number of tokens stored.
 */
int tl_count(const token_list_t *list) {
    return list->count;
}

/*
 * tl_free - frees the dynamic array memory.
 */
void tl_free(token_list_t *list) {
    if (list->tokens != NULL) {
        free(list->tokens);
        list->tokens = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}
