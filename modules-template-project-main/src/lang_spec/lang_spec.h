/*
 * -----------------------------------------------------------------------------
 * lang_spec.h
 *
 * Language specification constants for the scanner.
 * All language-dependent information is concentrated here so that
 * the rest of the modules remain language-independent.
 *
 * Supported language (subset of C):
 *   - Types: int, char, void (recognized as keywords)
 *   - Keywords: if, else, while, return, int, char, void
 *   - Operators: = > + *
 *   - Special characters: ( ) ; { } [ ] ,
 *   - Numbers: integers [0-9]+
 *   - Identifiers: [A-Za-z][A-Za-z0-9]*  (not a keyword)
 *   - Literals: "..." (double-quoted strings)
 *   - Non-recognized: any unsupported lexeme
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef LANG_SPEC_H
#define LANG_SPEC_H

/* ---- Token category enumeration ---- */
typedef enum {
    CAT_NUMBER       = 0,
    CAT_IDENTIFIER   = 1,
    CAT_KEYWORD      = 2,
    CAT_LITERAL      = 3,
    CAT_OPERATOR     = 4,
    CAT_SPECIALCHAR  = 5,
    CAT_NONRECOGNIZED = 6,
    CAT_COUNT        = 7   /* total number of categories */
} token_category_t;

/* ---- Category name strings (indexed by token_category_t) ---- */
#define CAT_NAME_NUMBER        "CAT_NUMBER"
#define CAT_NAME_IDENTIFIER    "CAT_IDENTIFIER"
#define CAT_NAME_KEYWORD       "CAT_KEYWORD"
#define CAT_NAME_LITERAL       "CAT_LITERAL"
#define CAT_NAME_OPERATOR      "CAT_OPERATOR"
#define CAT_NAME_SPECIALCHAR   "CAT_SPECIALCHAR"
#define CAT_NAME_NONRECOGNIZED "CAT_NONRECOGNIZED"

/* ---- Keyword table ---- */
#define NUM_KEYWORDS 7

#define KW_IF     "if"
#define KW_ELSE   "else"
#define KW_WHILE  "while"
#define KW_RETURN "return"
#define KW_INT    "int"
#define KW_CHAR   "char"
#define KW_VOID   "void"

/* ---- Operator characters ---- */
#define OP_ASSIGN '='
#define OP_GT     '>'
#define OP_PLUS   '+'
#define OP_STAR   '*'
#define NUM_OPERATORS 4

/* ---- Special characters ---- */
#define SC_LPAREN    '('
#define SC_RPAREN    ')'
#define SC_SEMICOLON ';'
#define SC_LBRACE    '{'
#define SC_RBRACE    '}'
#define SC_LBRACKET  '['
#define SC_RBRACKET  ']'
#define SC_COMMA     ','
#define NUM_SPECIALS 8

/* ---- Literal delimiters ---- */
#define LIT_QUOTE '"'

/* ---- Whitespace / delimiter characters ---- */
#define WS_SPACE  ' '
#define WS_TAB    '\t'
#define WS_CR     '\r'
#define WS_NL     '\n'

/* ---- Max lexeme length ---- */
#define MAX_LEXEME_LEN 1024

/* ---- Scanner output file suffix ---- */
#define SCN_SUFFIX "scn"

/* ---- Debug count output file suffix ---- */
#define DBGCNT_SUFFIX "dbgcnt"

/* ---- Helper function declarations ---- */

/*
 * ls_get_category_name - returns the printable name for a category.
 */
const char* ls_get_category_name(token_category_t cat);

/*
 * ls_is_keyword - checks whether a lexeme is a keyword.
 * Returns 1 if it is, 0 otherwise.
 * Uses character-by-character comparison (no string library for recognition).
 */
int ls_is_keyword(const char *lexeme);

/*
 * ls_is_operator - checks if a character is an operator.
 */
int ls_is_operator(char ch);

/*
 * ls_is_special_char - checks if a character is a special character.
 */
int ls_is_special_char(char ch);

/*
 * ls_is_whitespace - checks if a character is a whitespace/delimiter.
 */
int ls_is_whitespace(char ch);

/*
 * ls_is_letter - checks if a character is a letter [A-Za-z].
 */
int ls_is_letter(char ch);

/*
 * ls_is_digit - checks if a character is a digit [0-9].
 */
int ls_is_digit(char ch);

/*
 * ls_is_quote - checks if a character is a literal quote delimiter.
 */
int ls_is_quote(char ch);

#endif /* LANG_SPEC_H */
