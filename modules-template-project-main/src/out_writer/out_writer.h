/*
 * -----------------------------------------------------------------------------
 * out_writer.h
 *
 * Output writer for the .cscn token file.
 * Formats the token list into RELEASE or DEBUG format and writes it
 * to <filename>.<ext>scn.
 *
 * RELEASE format:
 *   - Tokens on same lines as source code lines, separated by single spaces.
 *   - No empty lines in output.
 *   - Format: <lexeme, CATEGORY> <lexeme, CATEGORY> ...
 *
 * DEBUG format:
 *   - Same as RELEASE but each token line starts with the input line number.
 *   - An empty line separates each token line.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef OUT_WRITER_H
#define OUT_WRITER_H

#include "../token_list/token_list.h"

/* ---- Output format options ---- */
#define OUTFORMAT_RELEASE 0
#define OUTFORMAT_DEBUG   1

#ifndef OUTFORMAT
#define OUTFORMAT OUTFORMAT_RELEASE  /* default to RELEASE */
#endif

/* ---- Token formatting delimiters ---- */
#define TOK_FMT_OPEN  '<'
#define TOK_FMT_CLOSE '>'
#define TOK_FMT_SEP   ','
#define TOK_FMT_SPACE ' '

/*
 * ow_build_output_filename - constructs the output filename by appending
 * the SCN_SUFFIX to the input filename's extension.
 *   input_filename: e.g. "example.c"
 *   output_buf:     buffer to write result e.g. "example.cscn"
 *   buf_len:        size of output_buf
 */
void ow_build_output_filename(const char *input_filename, char *output_buf,
                              int buf_len);

/*
 * ow_write_token_file - writes the token list to the output .cscn file.
 *   tokens: the token list to write
 *   output_filename: path to the output file
 * Returns 0 on success, non-zero on error.
 */
int ow_write_token_file(const token_list_t *tokens, const char *output_filename);

#endif /* OUT_WRITER_H */
