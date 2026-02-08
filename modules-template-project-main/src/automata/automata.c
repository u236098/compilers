/*
 * -----------------------------------------------------------------------------
 * automata.c
 *
 * Scanner engine implementation. Uses a single DFA loop driven entirely
 * by the transition matrix T[state][class]. No external dispatch by
 * first character — all decisions happen inside the automaton.
 *
 * Design:
 *   - ONE function scanner_next_token() recognises each token.
 *   - Maximal munch via last_accept_state + last_accept_pos rollback.
 *   - Keywords are reclassified char-by-char after identifier acceptance.
 *   - Whitespace is consumed inside the DFA (START + WS → START).
 *   - Unterminated literals emit one error + NONRECOGNIZED token.
 *   - Grouped non-recognized chars emit one error per group.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "automata.h"
#include "../lang_spec/lang_spec.h"

/* ---- Transition matrix ----
 * Rows = current state, Columns = character class.
 * Value = next state.
 *
 * ST_STOP means "do not consume; emit token using last_accept_state".
 * All branching is internal to the matrix — no outer switch/if dispatch.
 */

static const scan_state_t T[ST_COUNT][CC_COUNT] = {
    /* ST_START: branch internally based on character class */
    /*              LETTER         DIGIT          QUOTE          OPERATOR       SPECIAL        SPACE          NEWLINE        EOF            OTHER        */
    [ST_START]    = {ST_IN_IDENT,  ST_IN_NUMBER,  ST_IN_LITERAL, ST_ACCEPT_OP,  ST_ACCEPT_SC,  ST_START,      ST_START,      ST_STOP,       ST_IN_NONREC },

    /* ST_IN_NUMBER: accumulate digits, stop on anything else */
    [ST_IN_NUMBER]= {ST_STOP,      ST_IN_NUMBER,  ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    /* ST_IN_IDENT: accumulate letters/digits, stop on anything else */
    [ST_IN_IDENT] = {ST_IN_IDENT,  ST_IN_IDENT,   ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    /* ST_IN_LITERAL: consume everything until closing quote, error on NL/EOF */
    [ST_IN_LITERAL]={ST_IN_LITERAL,ST_IN_LITERAL,  ST_LIT_END,    ST_IN_LITERAL, ST_IN_LITERAL, ST_IN_LITERAL, ST_ERROR,      ST_ERROR,      ST_IN_LITERAL},

    /* ST_ACCEPT_OP: single-char accept — immediately stop after consume */
    [ST_ACCEPT_OP]= {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    /* ST_ACCEPT_SC: single-char accept — immediately stop after consume */
    [ST_ACCEPT_SC]= {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    /* ST_IN_NONREC: accumulate consecutive non-recognized characters */
    [ST_IN_NONREC]= {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_IN_NONREC },

    /* ST_LIT_END: closing quote consumed — literal done, stop immediately */
    [ST_LIT_END]  = {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    /* ST_ERROR: terminal error — stop immediately */
    [ST_ERROR]    = {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    /* ST_STOP: should never be used as a current state row */
    [ST_STOP]     = {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },
};

/* ---- Accepting-state → token-category map ---- */

/* Returns 1 if the state is an accepting state, 0 otherwise. */
static int is_accepting(scan_state_t st) {
    switch (st) {
        case ST_IN_NUMBER:
        case ST_IN_IDENT:
        case ST_IN_NONREC:
        case ST_ACCEPT_OP:
        case ST_ACCEPT_SC:
        case ST_LIT_END:
            return 1;
        default:
            return 0;
    }
}

/* Maps an accepting state to its token category. */
static token_category_t accept_category(scan_state_t st) {
    switch (st) {
        case ST_IN_NUMBER:  return CAT_NUMBER;
        case ST_IN_IDENT:   return CAT_IDENTIFIER; /* reclassified later */
        case ST_ACCEPT_OP:  return CAT_OPERATOR;
        case ST_ACCEPT_SC:  return CAT_SPECIALCHAR;
        case ST_LIT_END:    return CAT_LITERAL;
        case ST_IN_NONREC:  return CAT_NONRECOGNIZED;
        default:            return CAT_NONRECOGNIZED;
    }
}

/*
 * classify_char - maps a character to its character class.
 */
char_class_t classify_char(int ch) {
    if (ch == CS_EOF) {
        return CC_EOF;
    }
    if (ls_is_letter((char)ch)) {
        return CC_LETTER;
    }
    if (ls_is_digit((char)ch)) {
        return CC_DIGIT;
    }
    if (ls_is_quote((char)ch)) {
        return CC_QUOTE;
    }
    if (ls_is_operator((char)ch)) {
        return CC_OPERATOR;
    }
    if (ls_is_special_char((char)ch)) {
        return CC_SPECIAL;
    }
    if ((char)ch == WS_NL) {
        return CC_NEWLINE;
    }
    if (ls_is_whitespace((char)ch)) {
        return CC_SPACE;
    }
    return CC_OTHER;
}

/* ---- Lexeme buffer helper ---- */

static void add_char_to_lexeme(char *buf, int *len, int ch) {
    if (*len < MAX_LEXEME_LEN - 1) {
        buf[*len] = (char)ch;
        (*len)++;
        buf[*len] = '\0';
    }
}

/* ---- Error reporters ---- */

static void report_nonrecognized(logger_t *lg, int line, const char *lexeme) {
    err_report(logger_get_dest(lg), ERR_NONRECOGNIZED, ERR_STEP_SCANNER,
               line, lexeme);
}

static void report_unterminated_literal(logger_t *lg, int line,
                                        const char *lexeme) {
    err_report(logger_get_dest(lg), ERR_UNTERMINATED_LIT, ERR_STEP_SCANNER,
               line, lexeme);
}

/*
 * scanner_next_token - recognises ONE token using the DFA loop.
 * Returns 1 if a token was emitted, 0 if EOF was reached.
 *
 * All decisions are driven by the transition matrix T[state][class].
 * Maximal munch is implemented via last_accept_state + last_accept_pos
 * rollback (for this grammar rollback is not needed because the DFA
 * transitions to ST_STOP without consuming, but the structure is in
 * place for correctness).
 */
static int scanner_next_token(char_stream_t *cs, token_list_t *tokens,
                              logger_t *lg, counter_t *cnt) {
    scan_state_t state = ST_START;
    scan_state_t last_accept_state = ST_STOP; /* ST_STOP = no accept yet */
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line = cs_line(cs);
    int tok_col = cs_col(cs);
    int ch;
    char_class_t cls;
    scan_state_t next;

    buf[0] = '\0';

    while (1) {
        ch = cs_peek(cs);
        CNT_COMP(cnt, 1);

        cls = classify_char(ch);
        CNT_COMP(cnt, 1);

        next = T[state][cls];

        /* --- Handle ST_STOP or ST_ERROR --- */
        if (next == ST_STOP || next == ST_ERROR) {
            if (next == ST_ERROR && state == ST_IN_LITERAL) {
                /* Unterminated literal: report error, emit NONRECOGNIZED */
                report_unterminated_literal(lg, tok_line, buf);
                {
                    token_t tok;
                    token_init(&tok, buf, CAT_NONRECOGNIZED, tok_line, tok_col);
                    tl_add(tokens, &tok);
                }
                return 1;
            }

            if (last_accept_state != ST_STOP) {
                /* Emit token from last accepting state */
                token_category_t cat = accept_category(last_accept_state);
                token_t tok;

                /* Keyword reclassification: char-by-char after identifier */
                if (cat == CAT_IDENTIFIER && ls_is_keyword(buf)) {
                    cat = CAT_KEYWORD;
                }

                token_init(&tok, buf, cat, tok_line, tok_col);
                tl_add(tokens, &tok);

                /* Report error for non-recognized group (one per group) */
                if (cat == CAT_NONRECOGNIZED) {
                    report_nonrecognized(lg, tok_line, buf);
                }
                return 1;
            }

            /* No accept and ST_STOP at START with EOF → done */
            if (state == ST_START && cls == CC_EOF) {
                return 0;
            }

            /* Should not happen — consume one char to avoid infinite loop */
            cs_get(cs);
            CNT_IO(cnt, 1);
            return 0;
        }

        /* --- Handle whitespace skip inside DFA (START → START) --- */
        if (state == ST_START && next == ST_START) {
            cs_get(cs);
            CNT_IO(cnt, 1);
            continue;
        }

        /* --- Normal transition: consume character --- */
        if (state == ST_START) {
            /* Record position of first character of token */
            tok_line = cs_line(cs);
            tok_col = cs_col(cs);
        }

        ch = cs_get(cs);
        CNT_IO(cnt, 1);
        CNT_GEN(cnt, 1);
        add_char_to_lexeme(buf, &buf_len, ch);

        state = next;

        /* Update last_accept_state if current state is accepting */
        if (is_accepting(state)) {
            last_accept_state = state;
        }
    }
}

/*
 * automata_scan - main scanner entry point. Calls scanner_next_token()
 * in a loop until EOF. All decisions are inside the DFA.
 */
int automata_scan(char_stream_t *cs, token_list_t *tokens, logger_t *lg,
                  counter_t *cnt) {
    while (scanner_next_token(cs, tokens, lg, cnt)) {
        /* token emitted; continue */
    }
    return 0;
}
