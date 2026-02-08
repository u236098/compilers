/*
 * -----------------------------------------------------------------------------
 * logger.c
 *
 * Logger implementation. Routes messages through fprintf to either
 * stdout or the output file based on the DEBUG_FLAG compile-time setting.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "logger.h"
#include <stdarg.h>

/*
 * logger_init - sets the destination stream.
 */
void logger_init(logger_t *lg, FILE *outfile) {
    if (DEBUG_FLAG == DEBUG_ON && outfile != NULL) {
        lg->dest = outfile;
    } else {
        lg->dest = stdout;
    }
}

/*
 * logger_get_dest - returns current destination.
 */
FILE* logger_get_dest(const logger_t *lg) {
    return lg->dest;
}

/*
 * logger_write - variable-argument message writer.
 */
void logger_write(const logger_t *lg, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(lg->dest, fmt, args);
    va_end(args);
}
