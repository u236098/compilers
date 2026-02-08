/*
 * -----------------------------------------------------------------------------
 * error_mod.h
 *
 * Centralized error catalog and reporter for the compiler.
 * Each error has a unique ID and a step (phase) identifier.
 * Error messages are formatted with context parameters and
 * routed through the logger.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef ERROR_MOD_H
#define ERROR_MOD_H

#include <stdio.h>

/* ---- Step / phase identifiers ---- */
#define ERR_STEP_SCANNER  "SCANNER"
#define ERR_STEP_PARSER   "PARSER"
#define ERR_STEP_DRIVER   "DRIVER"

/* ---- Error IDs (unique across all phases) ---- */
#define ERR_NONE               0    /* no error                              */
#define ERR_FILE_OPEN          1    /* cannot open input file                */
#define ERR_FILE_OUTPUT        2    /* cannot create output file             */
#define ERR_UNTERMINATED_LIT   3    /* unterminated literal (missing close ") */
#define ERR_NONRECOGNIZED      4    /* non-recognized character(s)           */
#define ERR_INTERNAL           5    /* internal / unexpected error           */

/* ---- Error message templates ---- */
#define ERR_MSG_FILE_OPEN        "Cannot open input file"
#define ERR_MSG_FILE_OUTPUT      "Cannot create output file"
#define ERR_MSG_UNTERMINATED_LIT "Unterminated literal"
#define ERR_MSG_NONRECOGNIZED    "Non-recognized character(s)"
#define ERR_MSG_INTERNAL         "Internal error"

/*
 * err_report - formats and writes an error message.
 *   dest: output stream (file or stdout, decided by caller/logger)
 *   err_id: error identifier
 *   step: compiler phase string
 *   line: source line where error occurred
 *   context: additional text (e.g. the offending lexeme); may be NULL
 */
void err_report(FILE *dest, int err_id, const char *step, int line,
                const char *context);

/*
 * err_get_message - returns the template message string for an error ID.
 */
const char* err_get_message(int err_id);

#endif /* ERROR_MOD_H */
