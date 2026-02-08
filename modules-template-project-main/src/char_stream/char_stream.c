/*
 * -----------------------------------------------------------------------------
 * char_stream.c
 *
 * Input cursor implementation. Reads file one character at a time,
 * tracking line and column numbers.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "char_stream.h"
#include <stddef.h>  /* NULL */

#define CS_FIRST_LINE 1   /* initial line number  */
#define CS_FIRST_COL  1   /* initial column number */
#define CS_COL_INC    1   /* column increment per character */

/*
 * cs_open - opens a file for reading and initialises the cursor.
 */
int cs_open(char_stream_t *cs, const char *filename) {
    cs->fp = fopen(filename, "r");
    if (cs->fp == NULL) {
        return -1;
    }
    cs->current = 0;
    cs->line = CS_FIRST_LINE;
    cs->col = CS_FIRST_COL;
    cs->has_peek = 0;
    cs->peek_ch = 0;
    return 0;
}

/*
 * cs_peek - returns the next character without consuming it.
 */
int cs_peek(char_stream_t *cs) {
    if (cs->has_peek) {
        return cs->peek_ch;
    }
    cs->peek_ch = fgetc(cs->fp);
    if (cs->peek_ch == EOF) {
        cs->peek_ch = CS_EOF;
    }
    cs->has_peek = 1;
    return cs->peek_ch;
}

/*
 * cs_get - reads and consumes the next character.
 * Handles newline to update line/col counters.
 */
int cs_get(char_stream_t *cs) {
    int ch;
    if (cs->has_peek) {
        ch = cs->peek_ch;
        cs->has_peek = 0;
    } else {
        ch = fgetc(cs->fp);
        if (ch == EOF) {
            ch = CS_EOF;
        }
    }

    if (ch != CS_EOF) {
        cs->current = ch;
        if (ch == '\n') {
            cs->line += CS_COL_INC;
            cs->col = CS_FIRST_COL;
        } else {
            cs->col += CS_COL_INC;
        }
    }
    return ch;
}

/*
 * cs_line - returns the current line number.
 */
int cs_line(const char_stream_t *cs) {
    return cs->line;
}

/*
 * cs_col - returns the current column number.
 */
int cs_col(const char_stream_t *cs) {
    return cs->col;
}

/*
 * cs_close - closes the file.
 */
void cs_close(char_stream_t *cs) {
    if (cs->fp != NULL) {
        fclose(cs->fp);
        cs->fp = NULL;
    }
}
