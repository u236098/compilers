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
    char output_filename[MAX_FILENAME_BUF];
    int result;

    /* Initialise subsystems */
    counter_init(&cnt);
    tl_init(&tokens);

    /* Build output filename: e.g. "example.c" -> "example.cscn" */
    ow_build_output_filename(input_filename, output_filename, MAX_FILENAME_BUF);

    /* Initialise logger — use stdout for now (output file not opened yet) */
    logger_init(&lg, stdout);

    /* Open input file */
    if (cs_open(&cs, input_filename) != 0) {
        err_report(logger_get_dest(&lg), ERR_FILE_OPEN, ERR_STEP_DRIVER,
                   0, input_filename);
        tl_free(&tokens);
        return ERR_FILE_OPEN;
    }

    fprintf(stdout, "Scanning: %s\n", input_filename);

    /* Run the scanner automaton */
    result = automata_scan(&cs, &tokens, &lg, &cnt);

    /* Close input */
    cs_close(&cs);

    /* Write the token list to the output .cscn file */
    if (ow_write_token_file(&tokens, output_filename) != 0) {
        err_report(logger_get_dest(&lg), ERR_FILE_OUTPUT, ERR_STEP_DRIVER,
                   0, output_filename);
        tl_free(&tokens);
        return ERR_FILE_OUTPUT;
    }

    fprintf(stdout, "Output written to: %s\n", output_filename);
    fprintf(stdout, "Tokens found: %d\n", tl_count(&tokens));

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