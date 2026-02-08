/*
 * -----------------------------------------------------------------------------
 * main.c
 *
 * Scanner driver (Practice 2 — Lexical Analysis).
 * This is a wrapper function that calls the scanner modules.
 *
 * Usage: ./scanner <input.c>
 *
 * Steps:
 *   1. Parse command-line arguments.
 *   2. Open the input file via char_stream.
 *   3. Run the automata scanner to produce the token list.
 *   4. Write the token list to the .cscn output file.
 *   5. (Future hook) Call the parser with the in-memory token list.
 *   6. Clean up resources.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "./main.h"

FILE* ofile = NULL; /* The output handler for the project run */

/*
 * print_usage - displays usage information when arguments are incorrect.
 */
static void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <input.c>\n", prog_name);
}

#ifdef COUNTCONFIG
#if OUTFORMAT == OUTFORMAT_DEBUG
/*
 * write_count_summary - routes total counter output based on COUNTOUT/COUNTFILE.
 */
static void write_count_summary(const char *input_filename,
                                const char *output_filename,
                                const counter_t *cnt) {
    FILE *dest = stdout;
    char count_filename[MAX_FILENAME_BUF];

    if (COUNTOUT == COUNTOUT_OUT) {
        if (COUNTFILE == COUNTFILE_DBGCNT) {
            ow_build_count_filename(input_filename, count_filename,
                                    MAX_FILENAME_BUF);
            dest = fopen(count_filename, "w");
        } else {
            dest = fopen(output_filename, "a");
        }
        if (dest == NULL) {
            dest = stdout;
        }
    }

    counter_print(cnt, dest, "run_scanner", 0);

    if (dest != stdout) {
        fclose(dest);
    }
}
#endif
#endif

/*
 * run_scanner - orchestrates the scanning phase.
 *   input_filename: path to the .c input file
 * Returns 0 on success, non-zero on error.
 */
static int run_scanner(const char *input_filename) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    FILE *debug_out = NULL;
    char output_filename[MAX_FILENAME_BUF];
    int result;

    /* Initialise subsystems */
    counter_init(&cnt);
    tl_init(&tokens);

    /* Build output filename: e.g. "example.c" -> "example.cscn" */
    ow_build_output_filename(input_filename, output_filename, MAX_FILENAME_BUF);

    /*
     * Initialise logger destination:
     * DEBUG_ON -> output file, DEBUG_OFF -> stdout.
     */
    if (DEBUG_FLAG == DEBUG_ON) {
        debug_out = fopen(output_filename, "w");
        if (debug_out == NULL) {
            tl_free(&tokens);
            return ERR_FILE_OUTPUT;
        }
        logger_init(&lg, debug_out);
    } else {
        logger_init(&lg, stdout);
    }

    /* Open input file */
    if (cs_open(&cs, input_filename) != 0) {
        err_report(logger_get_dest(&lg), ERR_FILE_OPEN, ERR_STEP_DRIVER,
                   0, input_filename);
        if (debug_out != NULL) {
            fclose(debug_out);
        }
        tl_free(&tokens);
        return ERR_FILE_OPEN;
    }

    fprintf(stdout, "Scanning: %s\n", input_filename);

    /* Run the scanner automaton */
    result = automata_scan(&cs, &tokens, &lg, &cnt);

    /* Close input */
    cs_close(&cs);

    if (debug_out != NULL) {
        fclose(debug_out);
        debug_out = NULL;
        logger_init(&lg, NULL);
    }

    /* Write the token list to the output .cscn file */
    if (ow_write_token_file_mode(&tokens, output_filename,
                                 (DEBUG_FLAG == DEBUG_ON)) != 0) {
        err_report(logger_get_dest(&lg), ERR_FILE_OUTPUT, ERR_STEP_DRIVER,
                   0, output_filename);
        tl_free(&tokens);
        return ERR_FILE_OUTPUT;
    }

    fprintf(stdout, "Output written to: %s\n", output_filename);
    fprintf(stdout, "Tokens found: %d\n", tl_count(&tokens));

#ifdef COUNTCONFIG
#if OUTFORMAT == OUTFORMAT_DEBUG
    write_count_summary(input_filename, output_filename, &cnt);
#endif
#endif

    /* (Future hook) Parser would be called here with the in-memory token list:
     *   result = parser_run(&tokens);
     */

    /* Clean up */
    tl_free(&tokens);

    return result;
}

/*
 * main - entry point. Wrapper that calls scanner functions.
 */
int main(int argc, char *argv[]) {
    int result;

    ofile = stdout; /* Default output to stdout */

    if (argc < MIN_ARGS) {
        print_usage(argv[0]);
        return ERR_FILE_OPEN;
    }

    result = run_scanner(argv[ARG_INPUT_FILE]);

    return result;
}
