/*
 * -----------------------------------------------------------------------------
 * counter.h
 *
 * Operation counting system controlled by preprocessor flags.
 * When COUNTCONFIG is defined, the macros expand to increment calls;
 * otherwise they compile to nothing (zero overhead in RELEASE).
 *
 * Counters:
 *   COUNTCOMP - number of comparisons
 *   COUNTIO   - number of I/O characters read/written
 *   COUNTGEN  - general instruction count
 *
 * Output controlled by COUNTOUT:
 *   OUT=1  -> messages to output file
 *   DBGCOUNT=0 -> messages to <filename>.<ext>dbgcnt file
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef COUNTER_H
#define COUNTER_H

#include <stdio.h>

/* ---- Counter state ---- */
typedef struct {
    long comp;     /* comparison counter        */
    long io;       /* I/O character counter      */
    long gen;      /* general instruction counter */
} counter_t;

/* ---- Count output routing configuration ---- */
#define COUNTOUT_STDOUT 0
#define COUNTOUT_OUT    1

#ifndef COUNTOUT
#define COUNTOUT COUNTOUT_STDOUT
#endif

#define COUNTFILE_DBGCNT 0
#define COUNTFILE_OUTPUT 1

#ifndef COUNTFILE
#define COUNTFILE COUNTFILE_DBGCNT
#endif

/*
 * counter_init - reset all counters to zero.
 */
void counter_init(counter_t *cnt);

/*
 * counter_add_comp - increment comparison counter by amount.
 */
void counter_add_comp(counter_t *cnt, long amount);

/*
 * counter_add_io - increment I/O counter by amount.
 */
void counter_add_io(counter_t *cnt, long amount);

/*
 * counter_add_gen - increment general counter by amount.
 */
void counter_add_gen(counter_t *cnt, long amount);

/*
 * counter_print - prints current counter values to the given stream.
 *   func_name: name of the function reporting
 *   line: source line being processed
 */
void counter_print(const counter_t *cnt, FILE *dest, const char *func_name,
                   int line);

/* ---- Preprocessor macros for zero-overhead counting ---- */
#ifdef COUNTCONFIG

#define CNT_COMP(cnt_ptr, n)  counter_add_comp((cnt_ptr), (n))
#define CNT_IO(cnt_ptr, n)    counter_add_io((cnt_ptr), (n))
#define CNT_GEN(cnt_ptr, n)   counter_add_gen((cnt_ptr), (n))

#else

#define CNT_COMP(cnt, n)  /* no-op */
#define CNT_IO(cnt, n)    /* no-op */
#define CNT_GEN(cnt, n)   /* no-op */

#endif /* COUNTCONFIG */

#endif /* COUNTER_H */
