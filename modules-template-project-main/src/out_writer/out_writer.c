/*
 * -----------------------------------------------------------------------------
 * out_writer.c
 *
 * Output writer implementation. Builds the .cscn filename and writes
 * the formatted token list, respecting the OUTFORMAT setting.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "out_writer.h"
#include "../lang_spec/lang_spec.h"
#include <stdio.h>

/* ---- Internal helpers ---- */

/*
 * ow_build_output_filename - appends SCN_SUFFIX to the input extension.
 * Example: "example.c" -> "example.cscn"
 */
void ow_build_output_filename(const char *input_filename, char *output_buf,
                              int buf_len) {
    int i = 0;
    /* Copy entire input filename */
    while (input_filename[i] != '\0' && i < buf_len - 1) {
        output_buf[i] = input_filename[i];
        i++;
    }
    /* Append SCN_SUFFIX */
    {
        const char *suffix = SCN_SUFFIX;
        int s = 0;
        while (suffix[s] != '\0' && i < buf_len - 1) {
            output_buf[i] = suffix[s];
            i++;
            s++;
        }
    }
    output_buf[i] = '\0';
}

/*
 * write_token_formatted - writes a single token in <lexeme, CATEGORY> format.
 */
static void write_token_formatted(FILE *fp, const token_t *tok) {
    const char *cat_name = ls_get_category_name(tok->category);
    fprintf(fp, "%c%s%c %s%c", TOK_FMT_OPEN, tok->lexeme, TOK_FMT_SEP,
            cat_name, TOK_FMT_CLOSE);
}

/*
 * ow_write_token_file - writes the complete token list.
 */
int ow_write_token_file(const token_list_t *tokens, const char *output_filename) {
    FILE *fp;
    int i;
    int count;
    int current_line;
    int first_on_line;
    const token_t *tok;

    fp = fopen(output_filename, "w");
    if (fp == NULL) {
        return -1;
    }

    count = tl_count(tokens);
    if (count == 0) {
        fclose(fp);
        return 0;
    }

    current_line = -1;
    first_on_line = 1;

    for (i = 0; i < count; i++) {
        tok = tl_get(tokens, i);
        if (tok == NULL) {
            continue;
        }

        if (tok->line != current_line) {
            /* New source line — start a new output line */
            if (current_line != -1) {
                /* End previous line */
                fprintf(fp, "\n");
#if OUTFORMAT == OUTFORMAT_DEBUG
                /* Empty separator line in DEBUG mode */
                fprintf(fp, "\n");
#endif
            }
#if OUTFORMAT == OUTFORMAT_DEBUG
            /* Print source line number at the start */
            fprintf(fp, "%d ", tok->line);
#endif
            current_line = tok->line;
            first_on_line = 1;
        }

        if (!first_on_line) {
            fprintf(fp, " ");
        }
        write_token_formatted(fp, tok);
        first_on_line = 0;
    }

    /* Final newline */
    fprintf(fp, "\n");

    fclose(fp);
    return 0;
}
