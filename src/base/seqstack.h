/*

Copyright (c) 2005-2008, Simon Howard

Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided
that the above copyright notice and this permission notice appear
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 */

/**
 * @file seqstack.h
 *
 * @brief seqstack.
 *
 * seqstack stores a list of values in order.  New values
 * can be added and removed from  head of the seqstack.
 *
 * To create a new seqstack, use @ref seqstack_new.  To destroy a seqstack, use
 * @ref seqstack_free.
 *
 * To add values to a seqstack, use @ref seqstack_push and
 *
 * To read values from the head of a seqstack, use @ref seqstack_pop. To 
 * examine the ends without removing values from the seqstack, use 
 * @ref seqstack_peek.
 *
 */

#ifndef ALGORITHM_SEQSTACK_H
#define ALGORITHM_SEQSTACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "arraylist.h"

/**
 * Create a new double-ended seqstack.
 *
 * @return           A new seqstack, or NULL if it was not possible to allocate
 *                   the memory.
 */

ArrayList *seqstack_new(unsigned int length);

/**
 * Destroy a seqstack.
 *
 * @param seqstack      The seqstack to destroy.
 */

void seqstack_free(ArrayList *seqstack);

/**
 * Add a value to the head of a seqstack.
 *
 * @param seqstack      The seqstack.
 * @param data       The value to add.
 * @return           Non-zero if the value was added successfully, or zero
 *                   if it was not possible to allocate the memory for the
 *                   new entry.
 */

int seqstack_push(ArrayList *seqstack, ArrayListValue data);

/**
 * Remove a value from the head of a seqstack.
 *
 * @param seqstack      The seqstack.
 * @return           Value that was at the head of the seqstack, or
 *                   @ref QUEUE_NULL if the seqstack is empty.
 */

ArrayListValue seqstack_pop(ArrayList *seqstack);

/**
 * Read value from the head of a seqstack, without removing it from
 * the seqstack.
 *
 * @param seqstack      The seqstack.
 * @return           Value at the head of the seqstack, or @ref QUEUE_NULL if the
 *                   seqstack is empty.
 */

ArrayListValue seqstack_peek(ArrayList *seqstack);

/**
 * Add a value to the tail of a seqstack.
 *
 * @param seqstack      The seqstack.
 * @param data       The value to add.
 * @return           Non-zero if the value was added successfully, or zero
 *                   if it was not possible to allocate the memory for the
 *                   new entry.
 */

int seqstack_is_empty(ArrayList *seqstack);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_SEQSTACK_H */

