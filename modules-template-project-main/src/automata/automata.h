/*
 * -----------------------------------------------------------------------------
 * automata.h
 *
 * Scanner engine using automaton-based lexical analysis.
 * Implements a transition matrix (DFA) for character-by-character
 * token recognition. This is the ONLY module that consumes input
 * characters (cs_get). All lexical decisions live here.
 *
 * Automata implemented:
 *   - NUMBER:      [0-9]+
 *   - IDENTIFIER:  [A-Za-z][A-Za-z0-9]*  (post-check for keywords)
 *   - LITERAL:     "...", any char except newline/EOF inside
 *   - OPERATOR:    single char  = > + *
 *   - SPECIALCHAR: single char  ( ) ; { } [ ] ,
 *   - NONRECOGNIZED: consecutive invalid chars grouped into one token
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "../char_stream/char_stream.h"
#include "../token_list/token_list.h"
#include "../error_mod/error_mod.h"
#include "../logger/logger.h"
#include "../counter/counter.h"

/* ---- DFA state definitions ---- */

/* Character class indices for the transition matrix */
typedef enum {
    CC_LETTER   = 0,   /* [A-Za-z]     */
    CC_DIGIT    = 1,   /* [0-9]        */
    CC_QUOTE    = 2,   /* "            */
    CC_OPERATOR = 3,   /* = > + *      */
    CC_SPECIAL  = 4,   /* ( ) ; { } [ ] , */
    CC_SPACE    = 5,   /* whitespace   */
    CC_NEWLINE  = 6,   /* \n           */
    CC_EOF      = 7,   /* end of file  */
    CC_OTHER    = 8,   /* anything else */
    CC_COUNT    = 9    /* total classes */
} char_class_t;

/* Scanner states */
typedef enum {
    ST_START       = 0,  /* initial / idle state          */
    ST_IN_NUMBER   = 1,  /* accumulating digits           */
    ST_IN_IDENT    = 2,  /* accumulating letters/digits   */
    ST_IN_LITERAL  = 3,  /* inside a quoted literal       */
    ST_ACCEPT_OP   = 4,  /* accepted an operator          */
    ST_ACCEPT_SC   = 5,  /* accepted a special char       */
    ST_IN_NONREC   = 6,  /* accumulating non-recognized   */
    ST_LIT_END     = 7,  /* accepted a literal (closing quote) */
    ST_ERROR       = 8,  /* error (e.g. unterminated lit) */
    ST_STOP        = 9,  /* stop / do not consume         */
    ST_COUNT       = 10  /* total states                  */
} scan_state_t;

/*
 * automata_scan - scans the entire input stream and fills the token list.
 *   cs:      initialised char stream
 *   tokens:  initialised (empty) token list to fill
 *   lg:      logger for error/debug messages
 *   cnt:     counter for operation counting (may be NULL if COUNTCONFIG off)
 * Returns 0 on success.
 */
int automata_scan(char_stream_t *cs, token_list_t *tokens, logger_t *lg,
                  counter_t *cnt);

/*
 * classify_char - returns the character class for a given character.
 */
char_class_t classify_char(int ch);

#endif /* AUTOMATA_H */
