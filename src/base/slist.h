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
 * @file slist.h
 *
 * Singly-linked list.
 *
 * A singly-linked list stores a collection of values.  Each
 * entry in the list (represented by a pointer to a @ref SListEntry
 * structure) contains a link to the next entry.  It is only
 * possible to iterate over entries in a singly linked list in one
 * direction.
 *
 * To create a new singly-linked list, create a variable which is
 * a pointer to a @ref SListEntry, and initialise it to NULL.
 *
 * To destroy a singly linked list, use @ref slist_free.
 *
 * To add a new value at the start of a list, use @ref slist_prepend.
 * To add a new value at the end of a list, use @ref slist_append.
 *
 * To find the length of a list, use @ref slist_length.
 *
 * To access a value in a list by its index in the list, use
 * @ref slist_nth_data.
 *
 * To search a list for a value, use @ref slist_find_data.
 *
 * To sort a list into an order, use @ref slist_sort.
 *
 * To find a particular entry in a list by its index, use
 * @ref slist_nth_entry.
 *
 * To iterate over each value in a list, use @ref slist_iterate to
 * initialise a @ref SListIterator structure, with @ref slist_iter_next
 * and @ref slist_iter_has_more to retrieve each value in turn.
 * @ref slist_iter_remove can be used to efficiently remove the
 * current entry from the list.
 *
 * Given a particular entry in a list (@ref SListEntry):
 *
 * @li To find the next entry, use @ref slist_next.
 * @li To access the value stored at the entry, use @ref slist_data.
 * @li To remove the entry, use @ref slist_remove_entry.
 *
 */

#ifndef ALGORITHM_SLIST_H
#define ALGORITHM_SLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 单链表结构.
 */

typedef struct _SListEntry SListEntry;

/**
 * 链表迭代器结构，用于遍历链表.
 */

typedef struct _SListIterator SListIterator;

/**
 * 指向链表中存储的数据的指针
 */

typedef void *SListValue;

/**
 * 定义链表迭代器结构，参考@ref SListIterator.
 */

struct _SListIterator {
	SListEntry **prev_next;
	SListEntry *current;
};

/**
 * 定义链表中数据的空指针，参考@ref.
 */

#define SLIST_NULL ((void *) 0)

/**
 * 在排序时比较链表中两个元素数据的函数.
 *
 * @return    如果value1应排在value2之前，返回正数,
 *            如果value1应排在value2后面，返回负数,
 *            如果value1 and value2相等，返回0.
 */

typedef int (*SListCompareFunc)(SListValue value1, SListValue value2);

/**
 * 判断链表中两个元素数据是否相等.
 *
 * @return   如果value1 and value2相等,返回非零值，如果不相等返回0.
 */

typedef int (*SListEqualFunc)(SListValue value1, SListValue value2);

/**
 * 释放整个链表内存.
 *
 * @param list      表头指针.
 */

void slist_free(SListEntry *list);

/**
 * 在链表头插入结点.
 *
 * @param list      表头指针.
 * @param data      待插入的数据.
 * @return          返回新结点的指针，如果新结点内存分配失败，返回NULL.
 */

SListEntry *slist_prepend(SListEntry **list, SListValue data);

/**
 * 在链表尾插入结点.
 *
 * @param list      表头指针.
 * @param data      待插入的数据.
 * @return          返回新结点的指针，如果新结点内存分配失败，返回NULL.
 */

SListEntry *slist_append(SListEntry **list, SListValue data);

/**
 * 获取链表中下一个结点的指针.
 *
 * @param listentry    结点指针.
 * @return             返回下一个结点的指针，若当前为尾结点则返回NULL.
 */

SListEntry *slist_next(SListEntry *listentry);

/**
 * 返回结点中存储的数据.
 *
 * @param listentry    结点指针.
 * @return             返回结点中的数据.
 */

SListValue slist_data(SListEntry *listentry);

/**
 * 得到指向第n个结点的指针.
 *
 * @param list       表头指针.
 * @param n          序号.
 * @return           返回序号为n的结点指针，如果序号超出范围返回NULL.
 */

SListEntry *slist_nth_entry(SListEntry *list, unsigned int n);

/**
 * 获取第n个结点的数据.
 *
 * @param list       表头指针.
 * @param n          序号.
 * @return           返回序号为n的结点的数据，如果失败返回SLIST_NULL，参考@ref.
 */

SListValue slist_nth_data(SListEntry *list, unsigned int n);

/**
 * 得到链表的长度
 *
 * @param list       表头指针.
 * @return           返回链表中结点的个数.
 */

unsigned int slist_length(SListEntry *list);

/**
 * 创建一个包含链表中内容的数组.
 *
 * @param list       表头指针.
 * @return           返回新建的数组，如果数组内存申请失败返回NULL.
 *                   数组的长度和链表的长度相等(参考@ref slist_length).
 */

SListValue *slist_to_array(SListEntry *list);

/**
 * 删除一个结点.
 *
 * @param list       表头指针.
 * @param entry      待删除结点.
 * @return           成功删除返回非零数，结点不存在返回零.
 */

int slist_remove_entry(SListEntry **list, SListEntry *entry);

/**
 * 删除所有存有特定数据的结点.
 *
 * @param list       表头指针.
 * @param callback   Callback函数用于比较表中数据与待删除数据.
 * @param data       需要删除的数据.
 * @return           返回删除的结点数.
 */

unsigned int slist_remove_data(SListEntry **list,
                               SListEqualFunc callback,
                               SListValue data);

/**
 * 对链表排序
 *
 * @param list          表头指针.
 * @param compare_func  函数用于比较结点中的数据，由用户定义.
 */

void slist_sort(SListEntry **list, SListCompareFunc compare_func);

/**
 *查找存有特定数据的结点
 *
 * @param list           表头指针.
 * @param callback       Callback函数用于比较表中数据与待查找的数据是否相等.
 * @param data           待查找数据.
 * @return               返回找到的第一个结点，若未找到返回NULL.
 */

SListEntry *slist_find_data(SListEntry *list,
                            SListEqualFunc callback,
                            SListValue data);

/**
 * 初始化一个链表迭代器，用于遍历链表，参考@ref.
 *
 * @param list           表头指针.
 * @param iter           指向初始化的链表迭代器的指针，参考@ref.
 */

void slist_iterate(SListEntry **list, SListIterator *iter);

/**
 * 判断链表中是否还有更多数据待遍历.没有返回0，有则返回非零数
 *
 * @param iterator       链表迭代器.
 * @return               还有更多的数据待遍历则返回非零数，没有则返回0.
 */

int slist_iter_has_more(SListIterator *iterator);

/**
 * 用链表迭代器获取链表中的下一个数据.
 *
 * @param iterator       链表迭代器.
 * @return               返回下一个数据，如果不存在则返回LIST_NULL.
 */

SListValue slist_iter_next(SListIterator *iterator);

/**
 * 删除当前遍历到的位置的结点(最后一次从 slist_iter_next返回的数据，参考@ref)
 *
 * @param iterator       链表迭代器.
 */

void slist_iter_remove(SListIterator *iterator);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_SLIST_H */

