/*
 * -----------------------------------------------------------------------------
 * token.h
 *
 * Token data object definition. A token is <lexeme, category> with
 * optional metadata (source line, column).
 * Pure data object — no I/O in this module.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "../lang_spec/lang_spec.h"

/* Token structure */
typedef struct {
    char lexeme[MAX_LEXEME_LEN]; /* the recognized string       */
    token_category_t category;   /* category classification     */
    int line;                    /* source line (1-based)       */
    int col;                     /* source column (1-based)     */
} token_t;

/*
 * token_init - initialise a token with the given lexeme, category, line, col.
 */
void token_init(token_t *tok, const char *lexeme, token_category_t cat,
                int line, int col);

#endif /* TOKEN_H */
