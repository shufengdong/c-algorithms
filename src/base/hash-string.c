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

#include <ctype.h>

#include "hash-string.h"

/* 字符串哈希函数 */

unsigned int string_hash(void *string)
{
	/* 这是djb2字符串哈希函数 */

	unsigned int result = 5381;
	unsigned char *p;

	p = (unsigned char *) string;

	while (*p != '\0') {
		result = (result << 5) + result + *p;   /* 相当于result * 33 + *p */
		++p;
	}

	return result;
}

/* The same function, with a tolower on every character so that
 * case is ignored.  This code is duplicated for performance. */
/* 相同的函数，使用tolower将每个字母变成小写，从而忽略字母的大小写 */

unsigned int string_nocase_hash(void *string)
{
	unsigned int result = 5381;
	unsigned char *p;

	p = (unsigned char *) string;

	while (*p != '\0') {
		result = (result << 5) + result + (unsigned int) tolower(*p);
		++p;
	}

	return result;
}

