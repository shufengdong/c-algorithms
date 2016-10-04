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

#include <stdlib.h>
#include <string.h>

#include "arraylist.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

/* 创建自动调整长度的动态数组 */

ArrayList *arraylist_new(unsigned int length) {
	ArrayList *new_arraylist;

	/* 如果数组的长度不合法(小于等于0)，使用一个合理的默认值 */

	if (length <= 0) {
		length = 16;
	}

	/* 给一个新的动态数组分配空间，并且进行赋值 */

	new_arraylist = (ArrayList *) malloc(sizeof(ArrayList));

	if (new_arraylist == NULL) {
		return NULL;
	}

	new_arraylist->_alloced = length;
	new_arraylist->length = 0;

	/* 给data分配内存地址 */

	new_arraylist->data = malloc(length * sizeof(ArrayListValue));

	if (new_arraylist->data == NULL) {
		free(new_arraylist);
		return NULL;
	}

	return new_arraylist;
}

void arraylist_free(ArrayList *arraylist) {
	/* 不可对NULL释放内存 */

	if (arraylist != NULL) {
		free(arraylist->data);
		free(arraylist);
	}
}

static int arraylist_enlarge(ArrayList *arraylist) {
	ArrayListValue *data;
	unsigned int newsize;

	/* 将已分配的数组内存空间扩展为原来的2倍 */

	newsize = arraylist->_alloced * 2;

	/* 给数组重新分配新的内存空间 */

	data = realloc(arraylist->data, sizeof(ArrayListValue) * newsize);

	if (data == NULL) {
		return 0;
	} else {
		arraylist->data = data;
		arraylist->_alloced = newsize;

		return 1;
	}
}

int arraylist_insert(ArrayList *arraylist, unsigned int index,
                     ArrayListValue data) {
	/* 检查下标是否越界 */

	if (index > arraylist->length) {
		return 0;
	}

	/* 必要时扩展数组长度 */

	if (arraylist->length + 1 > arraylist->_alloced) {
		if (!arraylist_enlarge(arraylist)) {
			return 0;
		}
	}

	/* 把待插入位置及之后的数组内容后移一位 */

	memmove(&arraylist->data[index + 1],
	        &arraylist->data[index],
	        (arraylist->length - index) * sizeof(ArrayListValue));

	/*在下标为index的位置插入数据 */

	arraylist->data[index] = data;
	++arraylist->length;

	return 1;
}

/* 在数组数据尾部插入数据 */

int arraylist_append(ArrayList *arraylist, ArrayListValue data) {
	return arraylist_insert(arraylist, arraylist->length, data);
}

/* 在数组首插入数据 */
int arraylist_prepend(ArrayList *arraylist, ArrayListValue data) {
	return arraylist_insert(arraylist, 0, data);
}

/* 移除指定范围的内容 */

void arraylist_remove_range(ArrayList *arraylist, unsigned int index,
                            unsigned int length) {
	/* 检查范围是否合法 */

	if (index > arraylist->length || index + length > arraylist->length) {
		return;
	}

	/* 把移除范围之后数组的内容前移 */

	memmove(&arraylist->data[index],
	        &arraylist->data[index + length],
	        (arraylist->length - (index + length))
	            * sizeof(ArrayListValue));

	/* 新数组的长度 */

	arraylist->length -= length;
}

/* 移除指定下标的内容 */
void arraylist_remove(ArrayList *arraylist, unsigned int index) {
	arraylist_remove_range(arraylist, index, 1);
}

/* 查找指定元素 */
int arraylist_index_of(ArrayList *arraylist,
                       ArrayListEqualFunc callback,
                       ArrayListValue data) {
	unsigned int i;

	for (i=0; i<arraylist->length; ++i) {
		if (callback(arraylist->data[i], data) != 0)
			return (int) i;
	}

	return -1;
}

/* 清除数组 */
void arraylist_clear(ArrayList *arraylist) {
	/* 将数组长度设为0即可清理数组 */

	arraylist->length = 0;
}

/* 数组排序 */
static void arraylist_sort_internal(ArrayListValue *list_data,
                                    unsigned int list_length,
                                    ArrayListCompareFunc compare_func) {
	ArrayListValue pivot;
	ArrayListValue tmp;
	unsigned int i;
	unsigned int list1_length;
	unsigned int list2_length;

	/* 如果数据少于2个，则已经完成排序. */

	if (list_length <= 1) {
		return;
	}

	/* 将最后一个数据赋值给pivot. */

	pivot = list_data[list_length-1];

	/* 将数组分为两组:
	 *
	 * List 1 包含小于pivot的数据.
	 * List 2 包含大于pivot的数据.
	 *
	 * 两个组建立之后，它们在数组中顺序排列.
     * 亦即：list_data[list1_length-1]是组1中的最后一个数据，
     * list_data[list1_length]是组2中的第一个数据.
	 */

	list1_length = 0;

	for (i=0; i<list_length-1; ++i) {

		if (compare_func(list_data[i], pivot) < 0) {

			/* 数据应在list 1中，故其处于错误的位置，将此数据与紧跟list 1最后一项的数据互换 */

			tmp = list_data[i];
			list_data[i] = list_data[list1_length];
			list_data[list1_length] = tmp;

			++list1_length;

		} else {
			/* 数据应在list 2中，故其已经位于正确的位置 */
		}
	}

	/* list 2的长度可以计算得到 */

	list2_length = list_length - list1_length - 1;

	/* list_data[0 ~ list1_length-1]现在包含了所有排序在pivot之前的数据，
     * list_data[list1_length ~ list_length-2]包含了所有排序在pivot之后
     * 或者等于pivot的数据 */

	/* 通过互换pivot与list 2的第一个数据，将pivot移至正确的位置  */

	list_data[list_length-1] = list_data[list1_length];
	list_data[list1_length] = pivot;

	/* 使用递归进行排序 */

	arraylist_sort_internal(list_data, list1_length, compare_func);

	arraylist_sort_internal(&list_data[list1_length + 1], list2_length,
	                        compare_func);
}

void arraylist_sort(ArrayList *arraylist, ArrayListCompareFunc compare_func) {
	/* 执行递归排序 */

	arraylist_sort_internal(arraylist->data, arraylist->length,
	                        compare_func);
}

