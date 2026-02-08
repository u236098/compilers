/*
 * -----------------------------------------------------------------------------
 * token_list.h
 *
 * Ordered token list (dynamic array). Stores tokens in the order they
 * appear in the input. No formatting or scanning logic here.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

#include "../token/token.h"

/* Initial capacity and growth factor for the dynamic array */
#define TL_INIT_CAPACITY 128
#define TL_GROWTH_FACTOR 2

/* Token list structure */
typedef struct {
    token_t *tokens;   /* dynamic array of tokens       */
    int count;         /* number of tokens stored        */
    int capacity;      /* current allocated capacity     */
} token_list_t;

/*
 * tl_init - initialise an empty token list.
 */
void tl_init(token_list_t *list);

/*
 * tl_add - append a token to the list. Grows the array if needed.
 */
void tl_add(token_list_t *list, const token_t *tok);

/*
 * tl_get - retrieve a token by index. Returns NULL if out of bounds.
 */
const token_t* tl_get(const token_list_t *list, int index);

/*
 * tl_count - returns the number of tokens in the list.
 */
int tl_count(const token_list_t *list);

/*
 * tl_free - releases the memory used by the token list.
 */
void tl_free(token_list_t *list);

#endif /* TOKEN_LIST_H */
