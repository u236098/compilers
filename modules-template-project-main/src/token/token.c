/*
 * -----------------------------------------------------------------------------
 * token.c
 *
 * Token data object implementation.
 * Copies the lexeme character-by-character (no string library for recognition).
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "token.h"

/*
 * token_init - fills a token struct with the provided values.
 * Copies the lexeme character by character.
 */
void token_init(token_t *tok, const char *lexeme, token_category_t cat,
                int line, int col) {
    int i = 0;
    while (lexeme[i] != '\0' && i < MAX_LEXEME_LEN - 1) {
        tok->lexeme[i] = lexeme[i];
        i++;
    }
    tok->lexeme[i] = '\0';
    tok->category = cat;
    tok->line = line;
    tok->col = col;
}
