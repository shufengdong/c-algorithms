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
 * @file arraylist.h
 *
 * @brief Automatically resizing array
 *
 * ArrayLists are arrays of pointers which automatically increase in
 * size.
 *
 * To create an ArrayList, use @ref arraylist_new.
 * To destroy an ArrayList, use @ref arraylist_free.
 *
 * To add a value to an ArrayList, use @ref arraylist_prepend,
 * @ref arraylist_append, or @ref arraylist_insert.
 *
 * To remove a value from an ArrayList, use @ref arraylist_remove
 * or @ref arraylist_remove_range.
 */

#ifndef ALGORITHM_ARRAYLIST_H
#define ALGORITHM_ARRAYLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 指向数组中数据类型的指针，参考@ref.
 */

typedef void * ArrayListValue;

/**
 * 动态数组结构，可以使用arraylist_new函数来创建新的动态数组.
 *
 * 参考@see arraylist_new
 */

typedef struct _ArrayList ArrayList;

/**
 * 定义动态数组结构，参考@ref.
 */

struct _ArrayList {

	/** 数组的访问入口. */

	ArrayListValue *data;

	/** 数组中已存储的数据数. */

	unsigned int length;

	/** 数组的最大长度 */

	unsigned int _alloced;
};

/**
 * 比较数组中的两个数据是否相等.
 *
 * ����ֵ 相等返回非零值，不等返回零.
 */

typedef int (*ArrayListEqualFunc)(ArrayListValue value1,
                                  ArrayListValue value2);

/**
 *比较数组中的两个数据，在arraylist_sort数据排序中使用，参考@ref.
 *
 * ���� value1              第一个数据.
 * ���� value2              第二个数据.
 * ����ֵ                     若value1应排在value2之前，返回负值；若value1等于
 *                           value2，返回零；若value2应排在value1之前，返回正值
 */

typedef int (*ArrayListCompareFunc)(ArrayListValue value1,
                                    ArrayListValue value2);

/**
 * 创建一个新的动态数组以供使用.
 *
 * @param length         提示初始化函数最初分配给动态数组的内存大小，若
 *						 length被赋予零或负值，就使用一个合理的默认大小.
 * @return               一个新的动态数组.如果无法分配内存，返回空指针.
 * @see arraylist_free
 */

ArrayList *arraylist_new(unsigned int length);

/**
 * 销毁动态数组并且释放其占用的内存.
 *
 * @param arraylist      被销毁的动态数组.
 */

void arraylist_free(ArrayList *arraylist);

/**
 * 在动态数组的尾部添加一个数据.
 *
 * @param arraylist      动态数组.
 * @param data           添加到末尾的数据.
 * @return               添加成功返回非零值，如果无法分配更多内存空间返回零.
 */

int arraylist_append(ArrayList *arraylist, ArrayListValue data);

/**
 * 在动态数组的头部添加一个数据.
 *
 * @param arraylist      动态数组.
 * @param data           添加到头部的数据.
 * @return               添加成功返回非零值，如果无法分配更多内存空间返回零.
 */

int arraylist_prepend(ArrayList *arraylist, ArrayListValue data);

/**
 * 清除动态数组中指定下标的内容.
 *
 * @param arraylist      动态数组.
 * @param index          被清除内容所在的下标.
 */

void arraylist_remove(ArrayList *arraylist, unsigned int index);

/**
 * 清除动态数组中给定范围的内容.
 *
 * @param arraylist      动态数组.
 * @param index          被清除的范围在数组中的下标起始点.
 * @param length         被清除的范围的长度.
 */

void arraylist_remove_range(ArrayList *arraylist, unsigned int index,
                            unsigned int length);

/**
 * 在指定的下标位置插入一个数据.
 * 插入点的下标受动态数组的大小限制.
 *
 * @param arraylist      动态数组.
 * @param index          插入数据的下标.
 * @param data           数据.
 * @return               如果插入失败(由于下标无效，或者分配内存失败)
 *                       返回零值；成功返回非零值.
 */

int arraylist_insert(ArrayList *arraylist, unsigned int index,
                     ArrayListValue data);

/**
 * 在动态数组中找到特定数据的最小下标.
 *
 * @param arraylist      动态数组.
 * @param callback       Callback函数被用来比较组中的数据与被搜索数据.
 * @param data           被搜索的数据.
 * @return               如果找到对应数据，返回下标；未找到数据，返回-1.
 */

int arraylist_index_of(ArrayList *arraylist,
                       ArrayListEqualFunc callback,
                       ArrayListValue data);

/**
 * 移除动态数组中的所有内容.
 *
 * @param arraylist      动态数组.
 */

void arraylist_clear(ArrayList *arraylist);

/**
 * 对动态数组进行排序.
 *
 * @param arraylist      动态数组.
 * @param compare_func   排序过程中用来比较数据的函数.
 */

void arraylist_sort(ArrayList *arraylist, ArrayListCompareFunc compare_func);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_ARRAYLIST_H */

