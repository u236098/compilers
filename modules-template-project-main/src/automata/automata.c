/*
 * -----------------------------------------------------------------------------
 * automata.c
 *
 * Scanner engine implementation. Uses a transition matrix to drive
 * character-by-character DFA-based lexical analysis.
 *
 * The scanner operates in a single pass:
 *   1. Peek at the next character to classify it.
 *   2. Look up the transition matrix to decide the next state.
 *   3. Consume the character (cs_get) and append it to the lexeme buffer.
 *   4. When an accepting/terminal condition is reached, emit a token.
 *   5. Repeat until EOF.
 *
 * Keyword recognition is a post-processing step after the IDENTIFIER
 * automaton completes (character-by-character match, no string library).
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "automata.h"
#include "../lang_spec/lang_spec.h"

/* ---- Transition matrix ----
 * Rows = current state, Columns = character class.
 * Value = next state.
 * The matrix drives the main scanner loop.
 *
 * Transitions that lead to ST_START mean "do not consume, re-evaluate".
 * ST_ACCEPT means "emit current token and return to start".
 */

/* Transition matrix: [state][char_class] -> next_state */
static const scan_state_t transition[ST_COUNT][CC_COUNT] = {
    /* ST_START: dispatch based on first character */
    /*              LETTER       DIGIT        QUOTE        OPERATOR     SPECIAL      SPACE        NEWLINE      EOF          OTHER     */
    [ST_START]   = {ST_IN_IDENT, ST_IN_NUMBER,ST_IN_LITERAL,ST_ACCEPT_OP,ST_ACCEPT_SC,ST_START,   ST_START,    ST_ACCEPT,   ST_IN_NONREC},

    /* ST_IN_NUMBER: keep reading digits */
    [ST_IN_NUMBER]= {ST_ACCEPT,  ST_IN_NUMBER,ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT,  ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT  },

    /* ST_IN_IDENT: keep reading letters/digits */
    [ST_IN_IDENT] = {ST_IN_IDENT,ST_IN_IDENT, ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT,  ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT  },

    /* ST_IN_LITERAL: consume everything until closing quote */
    /*              LETTER       DIGIT        QUOTE        OPERATOR     SPECIAL      SPACE        NEWLINE      EOF          OTHER     */
    [ST_IN_LITERAL]={ST_IN_LITERAL,ST_IN_LITERAL,ST_ACCEPT,ST_IN_LITERAL,ST_IN_LITERAL,ST_IN_LITERAL,ST_ERROR,ST_ERROR,     ST_IN_LITERAL},

    /* ST_ACCEPT_OP: single-char accept (operator) - should not be entered as ongoing state */
    [ST_ACCEPT_OP]= {ST_START,   ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START   },

    /* ST_ACCEPT_SC: single-char accept (special char) */
    [ST_ACCEPT_SC]= {ST_START,   ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START   },

    /* ST_IN_NONREC: group consecutive non-recognized characters */
    [ST_IN_NONREC]= {ST_ACCEPT,  ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT,   ST_ACCEPT,  ST_ACCEPT,   ST_ACCEPT,   ST_IN_NONREC},

    /* ST_ACCEPT: terminal - not used as ongoing state */
    [ST_ACCEPT]  = {ST_START,   ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START   },

    /* ST_ERROR: terminal error */
    [ST_ERROR]   = {ST_START,   ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START,    ST_START   },
};

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

/*
 * add_char_to_lexeme - appends a character to the lexeme buffer.
 */
static void add_char_to_lexeme(char *buf, int *len, int ch) {
    if (*len < MAX_LEXEME_LEN - 1) {
        buf[*len] = (char)ch;
        (*len)++;
        buf[*len] = '\0';
    }
}

/*
 * emit_token - creates a token and adds it to the list.
 * Determines the category based on the state that produced it.
 */
static void emit_token(token_list_t *tokens, const char *lexeme,
                       scan_state_t origin_state, int line, int col,
                       logger_t *lg) {
    token_t tok;
    token_category_t cat;

    switch (origin_state) {
        case ST_IN_NUMBER:
            cat = CAT_NUMBER;
            break;
        case ST_IN_IDENT:
            /* Post-process: check if it is a keyword */
            if (ls_is_keyword(lexeme)) {
                cat = CAT_KEYWORD;
            } else {
                cat = CAT_IDENTIFIER;
            }
            break;
        case ST_IN_LITERAL:
            cat = CAT_LITERAL;
            break;
        case ST_ACCEPT_OP:
            cat = CAT_OPERATOR;
            break;
        case ST_ACCEPT_SC:
            cat = CAT_SPECIALCHAR;
            break;
        case ST_IN_NONREC:
            cat = CAT_NONRECOGNIZED;
            break;
        default:
            cat = CAT_NONRECOGNIZED;
            break;
    }

    token_init(&tok, lexeme, cat, line, col);
    tl_add(tokens, &tok);
}

/*
 * report_nonrecognized - reports a non-recognized error.
 */
static void report_nonrecognized(logger_t *lg, int line, const char *lexeme) {
    err_report(logger_get_dest(lg), ERR_NONRECOGNIZED, ERR_STEP_SCANNER,
               line, lexeme);
}

/*
 * report_unterminated_literal - reports an unterminated literal error.
 */
static void report_unterminated_literal(logger_t *lg, int line,
                                        const char *lexeme) {
    err_report(logger_get_dest(lg), ERR_UNTERMINATED_LIT, ERR_STEP_SCANNER,
               line, lexeme);
}

/*
 * scan_single_char_token - handles operator and special char tokens
 * (single character, immediate accept).
 */
static void scan_single_char_token(char_stream_t *cs, token_list_t *tokens,
                                    scan_state_t state, logger_t *lg,
                                    counter_t *cnt) {
    char buf[2];  /* single char + null terminator */
    int ch;
    int line = cs_line(cs);
    int col = cs_col(cs);

    ch = cs_get(cs);
    CNT_IO(*cnt, 1);
    CNT_GEN(*cnt, 1);

    buf[0] = (char)ch;
    buf[1] = '\0';

    emit_token(tokens, buf, state, line, col, lg);
}

/*
 * scan_multi_char_token - handles tokens that span multiple characters
 * (numbers, identifiers, non-recognized).
 * Uses the transition matrix with lookahead.
 */
static void scan_multi_char_token(char_stream_t *cs, token_list_t *tokens,
                                   scan_state_t state, logger_t *lg,
                                   counter_t *cnt) {
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int ch;
    int line = cs_line(cs);
    int col = cs_col(cs);
    char_class_t cc;
    scan_state_t next;

    /* Consume the first character that brought us to this state */
    ch = cs_get(cs);
    CNT_IO(*cnt, 1);
    CNT_GEN(*cnt, 1);
    add_char_to_lexeme(buf, &buf_len, ch);

    /* Continue consuming while the transition says to stay in same state */
    while (1) {
        int peek = cs_peek(cs);
        CNT_COMP(*cnt, 1);

        cc = classify_char(peek);
        CNT_COMP(*cnt, 1);

        next = transition[state][cc];

        if (next == state) {
            /* Stay in current state: consume and continue */
            ch = cs_get(cs);
            CNT_IO(*cnt, 1);
            CNT_GEN(*cnt, 1);
            add_char_to_lexeme(buf, &buf_len, ch);
        } else {
            /* Transition out: stop without consuming */
            break;
        }
    }

    /* Emit the token */
    emit_token(tokens, buf, state, line, col, lg);

    /* If it was non-recognized, report the error */
    if (state == ST_IN_NONREC) {
        report_nonrecognized(lg, line, buf);
    }
}

/*
 * scan_literal_token - handles string literal scanning.
 * Reads from opening quote to closing quote or error.
 */
static void scan_literal_token(char_stream_t *cs, token_list_t *tokens,
                                logger_t *lg, counter_t *cnt) {
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int ch;
    int line = cs_line(cs);
    int col = cs_col(cs);
    char_class_t cc;
    scan_state_t next;
    scan_state_t state = ST_IN_LITERAL;

    /* Consume opening quote */
    ch = cs_get(cs);
    CNT_IO(*cnt, 1);
    CNT_GEN(*cnt, 1);
    add_char_to_lexeme(buf, &buf_len, ch);

    /* Read until closing quote, newline, or EOF */
    while (1) {
        int peek = cs_peek(cs);
        CNT_COMP(*cnt, 1);

        cc = classify_char(peek);
        CNT_COMP(*cnt, 1);

        next = transition[state][cc];

        if (next == ST_IN_LITERAL) {
            /* Continue reading literal content */
            ch = cs_get(cs);
            CNT_IO(*cnt, 1);
            CNT_GEN(*cnt, 1);
            add_char_to_lexeme(buf, &buf_len, ch);
        } else if (next == ST_ACCEPT) {
            /* Closing quote found */
            ch = cs_get(cs);
            CNT_IO(*cnt, 1);
            CNT_GEN(*cnt, 1);
            add_char_to_lexeme(buf, &buf_len, ch);
            emit_token(tokens, buf, ST_IN_LITERAL, line, col, lg);
            return;
        } else {
            /* Error: unterminated literal (newline or EOF) */
            report_unterminated_literal(lg, line, buf);
            emit_token(tokens, buf, ST_IN_NONREC, line, col, lg);
            return;
        }
    }
}

/*
 * automata_scan - main scanner loop. Processes the entire input.
 */
int automata_scan(char_stream_t *cs, token_list_t *tokens, logger_t *lg,
                  counter_t *cnt) {
    int peek;
    char_class_t cc;
    scan_state_t next;

    while (1) {
        peek = cs_peek(cs);
        CNT_COMP(*cnt, 1);

        if (peek == CS_EOF) {
            break;
        }

        cc = classify_char(peek);
        CNT_COMP(*cnt, 1);

        /* Use transition from ST_START to decide what to do */
        next = transition[ST_START][cc];

        switch (next) {
            case ST_IN_NUMBER:
            case ST_IN_IDENT:
            case ST_IN_NONREC:
                scan_multi_char_token(cs, tokens, next, lg, cnt);
                break;

            case ST_IN_LITERAL:
                scan_literal_token(cs, tokens, lg, cnt);
                break;

            case ST_ACCEPT_OP:
            case ST_ACCEPT_SC:
                scan_single_char_token(cs, tokens, next, lg, cnt);
                break;

            case ST_START:
                /* Whitespace / newline: skip */
                cs_get(cs);
                CNT_IO(*cnt, 1);
                break;

            default:
                /* Should not happen */
                cs_get(cs);
                CNT_IO(*cnt, 1);
                break;
        }
    }

    return 0;
}
