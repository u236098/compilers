/*
 * -----------------------------------------------------------------------------
 * counter.c
 *
 * Counter implementation. Increments and prints operation counts.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "counter.h"

/*
 * counter_init - zeros all counters.
 */
void counter_init(counter_t *cnt) {
    cnt->comp = 0;
    cnt->io = 0;
    cnt->gen = 0;
}

/*
 * counter_add_comp - add to comparison counter.
 */
void counter_add_comp(counter_t *cnt, long amount) {
    cnt->comp += amount;
}

/*
 * counter_add_io - add to I/O counter.
 */
void counter_add_io(counter_t *cnt, long amount) {
    cnt->io += amount;
}

/*
 * counter_add_gen - add to general counter.
 */
void counter_add_gen(counter_t *cnt, long amount) {
    cnt->gen += amount;
}

/*
 * counter_print - prints counter values to the given stream.
 */
void counter_print(const counter_t *cnt, FILE *dest, const char *func_name,
                   int line) {
    fprintf(dest, "[COUNTER] Line %d | Func: %s | COMP=%ld IO=%ld GEN=%ld\n",
            line, func_name, cnt->comp, cnt->io, cnt->gen);
}
