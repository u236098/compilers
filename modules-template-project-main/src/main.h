#ifndef MAIN_H
#define MAIN_H
 
#include <stdio.h>
#include "./utils_files.h"
#include "./module_args/module_args.h"
#include "./module_2/module_2.h"

/* Scanner module includes */
#include "./lang_spec/lang_spec.h"
#include "./char_stream/char_stream.h"
#include "./token/token.h"
#include "./token_list/token_list.h"
#include "./automata/automata.h"
#include "./out_writer/out_writer.h"
#include "./error_mod/error_mod.h"
#include "./logger/logger.h"
#include "./counter/counter.h"

/* Output file of project run: either a stdout or a filename with log extension */
#define PROJOUTFILENAME "./proj_modules_template.log"

/* Minimum number of arguments expected: program name + input file */
#define MIN_ARGS 2

/* Index of the input filename in argv */
#define ARG_INPUT_FILE 1

/* Maximum filename buffer size */
#define MAX_FILENAME_BUF 512

extern FILE* ofile; /* The output handler for the project run */

#endif /* MAIN_H */
