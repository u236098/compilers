/*
 * -----------------------------------------------------------------------------
 * test_scanner.c
 *
 * Autonomous test program for the scanner (lexical analysis) modules.
 * Tests the lang_spec, char_stream, token, token_list, automata, and
 * out_writer modules.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "test_scanner.h"

/* ---- Test: Language Specification ---- */

/*
 * test_lang_spec - verifies keyword detection, char classification,
 * and category naming.
 */
static void test_lang_spec(void) {
    printf("  Testing lang_spec...\n");

    /* Keywords must be recognized */
    assert(ls_is_keyword(KW_IF) == 1);
    assert(ls_is_keyword(KW_ELSE) == 1);
    assert(ls_is_keyword(KW_WHILE) == 1);
    assert(ls_is_keyword(KW_RETURN) == 1);
    assert(ls_is_keyword(KW_INT) == 1);
    assert(ls_is_keyword(KW_CHAR) == 1);
    assert(ls_is_keyword(KW_VOID) == 1);

    /* Non-keywords must NOT be recognized */
    assert(ls_is_keyword("foo") == 0);
    assert(ls_is_keyword("iff") == 0);
    assert(ls_is_keyword("whil") == 0);

    /* Character classification */
    assert(ls_is_letter('a') == 1);
    assert(ls_is_letter('Z') == 1);
    assert(ls_is_letter('5') == 0);
    assert(ls_is_digit('0') == 1);
    assert(ls_is_digit('9') == 1);
    assert(ls_is_digit('a') == 0);
    assert(ls_is_operator(OP_ASSIGN) == 1);
    assert(ls_is_operator(OP_GT) == 1);
    assert(ls_is_operator(OP_PLUS) == 1);
    assert(ls_is_operator(OP_STAR) == 1);
    assert(ls_is_operator('x') == 0);
    assert(ls_is_special_char(SC_LPAREN) == 1);
    assert(ls_is_special_char(SC_SEMICOLON) == 1);
    assert(ls_is_special_char('x') == 0);
    assert(ls_is_whitespace(WS_SPACE) == 1);
    assert(ls_is_whitespace(WS_TAB) == 1);
    assert(ls_is_whitespace('a') == 0);
    assert(ls_is_quote(LIT_QUOTE) == 1);
    assert(ls_is_quote('a') == 0);

    /* Category names */
    assert(ls_get_category_name(CAT_NUMBER) != NULL);
    assert(ls_get_category_name(CAT_KEYWORD) != NULL);

    printf("  lang_spec tests PASSED\n");
}

/* ---- Test: Token and Token List ---- */

/*
 * test_token_list - verifies token creation and list operations.
 */
static void test_token_list(void) {
    token_list_t list;
    token_t tok;
    const token_t *retrieved;

    printf("  Testing token + token_list...\n");

    tl_init(&list);
    assert(tl_count(&list) == 0);

    token_init(&tok, "hello", CAT_IDENTIFIER, 1, 1);
    tl_add(&list, &tok);
    assert(tl_count(&list) == 1);

    retrieved = tl_get(&list, 0);
    assert(retrieved != NULL);
    assert(retrieved->category == CAT_IDENTIFIER);
    assert(retrieved->line == 1);

    /* Out of bounds returns NULL */
    assert(tl_get(&list, 5) == NULL);

    tl_free(&list);
    assert(tl_count(&list) == 0);

    printf("  token + token_list tests PASSED\n");
}

/* ---- Test: Scanner with test input file ---- */

/*
 * write_test_file - creates a temporary test input file.
 */
static void write_test_file(void) {
    FILE *fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "if(x > 3)\n");
    fprintf(fp, " printf(\"true\");\n");
    fprintf(fp, "else\n");
    fprintf(fp, " printf(\"false\");\n");
    fclose(fp);
}

/*
 * test_scanner_scan - runs the full scanner on the test input and
 * verifies the token count and categories.
 */
static void test_scanner_scan(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    int result;

    printf("  Testing scanner (automata)...\n");

    write_test_file();

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);

    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);

    cs_close(&cs);

    /* Verify token count */
    assert(tl_count(&tokens) == TEST_BASIC_EXPECTED_TOKENS);

    /* Verify first token: "if" should be CAT_KEYWORD */
    tok = tl_get(&tokens, 0);
    assert(tok != NULL);
    assert(tok->category == CAT_KEYWORD);

    /* Verify "(" token: should be CAT_SPECIALCHAR */
    tok = tl_get(&tokens, 1);
    assert(tok != NULL);
    assert(tok->category == CAT_SPECIALCHAR);

    /* Verify "x" token: should be CAT_IDENTIFIER */
    tok = tl_get(&tokens, 2);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);

    /* Verify ">" token: should be CAT_OPERATOR */
    tok = tl_get(&tokens, 3);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);

    /* Verify "3" token: should be CAT_NUMBER */
    tok = tl_get(&tokens, 4);
    assert(tok != NULL);
    assert(tok->category == CAT_NUMBER);

    /* Verify literal "true" token */
    tok = tl_get(&tokens, 8);
    assert(tok != NULL);
    assert(tok->category == CAT_LITERAL);

    /* Verify "else" keyword */
    tok = tl_get(&tokens, 11);
    assert(tok != NULL);
    assert(tok->category == CAT_KEYWORD);

    tl_free(&tokens);

    printf("  scanner tests PASSED\n");
}

/*
 * test_output_filename - verifies the output filename construction.
 */
static void test_output_filename(void) {
    char buf[256];

    printf("  Testing output filename construction...\n");

    ow_build_output_filename("example.c", buf, 256);
    assert(strcmp(buf, "example.cscn") == 0);

    ow_build_output_filename("test/path/file.c", buf, 256);
    assert(strcmp(buf, "test/path/file.cscn") == 0);

    printf("  output filename tests PASSED\n");
}

/*
 * test_output_writer - verifies the .cscn file is written correctly.
 */
static void test_output_writer(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    char output_filename[256];
    FILE *fp;
    int result;

    printf("  Testing output writer...\n");

    write_test_file();
    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    automata_scan(&cs, &tokens, &lg, &cnt);
    cs_close(&cs);

    ow_build_output_filename(TEST_INPUT_FILE, output_filename, 256);
    result = ow_write_token_file(&tokens, output_filename);
    assert(result == 0);

    /* Verify the file exists and is non-empty */
    fp = fopen(output_filename, "r");
    assert(fp != NULL);
    fseek(fp, 0, SEEK_END);
    assert(ftell(fp) > 0);
    fclose(fp);

    tl_free(&tokens);

    printf("  output writer tests PASSED\n");
}

/* ---- Main ---- */

int main(void) {
    printf("Running scanner tests...\n");

    test_lang_spec();
    test_token_list();
    test_scanner_scan();
    test_output_filename();
    test_output_writer();

    printf("All scanner tests PASSED!\n");
    return 0;
}
