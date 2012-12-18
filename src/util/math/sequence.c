/**
 * sequence.c
 * clockwork
 * December 02, 2012
 * Brandon Surmanski
 *
 * Special mathmatics sequences
 */

#include <math.h>
#include "const.h"
#include "sequence.h"

/**
 * retrieve the Fibonacci number through the 'Binet' formula
 */
int sequence_fibonacci(int n)
{
    return (int) round((pow(AURATIO, n) - pow(-AURATIO, -n)) / SQRT5);
}
