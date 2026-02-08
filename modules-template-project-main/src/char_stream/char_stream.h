/*
 * -----------------------------------------------------------------------------
 * char_stream.h
 *
 * Input cursor module. Provides character-by-character access to the
 * input file with line/column tracking and lookahead (peek).
 * This module only reads characters — it never classifies, skips, or
 * groups them.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include <stdio.h>

/* Sentinel value returned when end-of-file is reached */
#define CS_EOF (-1)

/* Char stream state */
typedef struct {
    FILE *fp;      /* input file handle               */
    int current;   /* most recently read character     */
    int line;      /* current line number (1-based)    */
    int col;       /* current column number (1-based)  */
    int has_peek;  /* 1 if a character has been peeked */
    int peek_ch;   /* the peeked character             */
} char_stream_t;

/*
 * cs_open - opens the input file and initialises the stream.
 * Returns 0 on success, non-zero on failure.
 */
int cs_open(char_stream_t *cs, const char *filename);

/*
 * cs_peek - returns the next character without consuming it.
 * Returns CS_EOF at end of file.
 */
int cs_peek(char_stream_t *cs);

/*
 * cs_get - reads and consumes the next character, advancing the cursor.
 * Updates line and column counters.
 * Returns CS_EOF at end of file.
 */
int cs_get(char_stream_t *cs);

/*
 * cs_line - returns the current line number.
 */
int cs_line(const char_stream_t *cs);

/*
 * cs_col - returns the current column number.
 */
int cs_col(const char_stream_t *cs);

/*
 * cs_close - closes the input file and resets the stream.
 */
void cs_close(char_stream_t *cs);

#endif /* CHAR_STREAM_H */
