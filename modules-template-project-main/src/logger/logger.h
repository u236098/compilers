/*
 * -----------------------------------------------------------------------------
 * logger.h
 *
 * Centralized debug/message router. All output from the scanner goes
 * through this module so that the DEBUG flag can route messages to
 * stdout or the output file.
 *
 * DEBUG ON  (1) = messages written to the output file
 * DEBUG OFF (0) = messages written to stdout
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

/* ---- DEBUG configuration (compile-time) ---- */
#ifndef DEBUG_FLAG
#define DEBUG_FLAG 0  /* default OFF: messages go to stdout */
#endif

/* ---- Debug ON / OFF values ---- */
#define DEBUG_ON  1
#define DEBUG_OFF 0

/* Logger state: holds the destination stream */
typedef struct {
    FILE *dest;  /* current output destination (stdout or file) */
} logger_t;

/*
 * logger_init - initialise the logger.
 *   outfile: the scanner output file handle (used when DEBUG is ON).
 *   Chooses dest based on DEBUG_FLAG.
 */
void logger_init(logger_t *lg, FILE *outfile);

/*
 * logger_get_dest - returns the current destination stream.
 */
FILE* logger_get_dest(const logger_t *lg);

/*
 * logger_write - writes a formatted message to the current destination.
 */
void logger_write(const logger_t *lg, const char *fmt, ...);

#endif /* LOGGER_H */
