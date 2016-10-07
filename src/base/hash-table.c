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

/* 散列表的实现 */

#include <stdlib.h>
#include <string.h>

#include "hash-table.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

struct _HashTableEntry {
	HashTableKey key;
	HashTableValue value;
	HashTableEntry *next;
};

struct _HashTable {
	HashTableEntry **table;
	unsigned int table_size;
	HashTableHashFunc hash_func;
	HashTableEqualFunc equal_func;
	HashTableKeyFreeFunc key_free_func;
	HashTableValueFreeFunc value_free_func;
	unsigned int entries;
	unsigned int prime_index;
};

/* This is a set of good hash table prime numbers, from:
 *   http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
 * Each prime is roughly double the previous value, and as far as
 * possible from the nearest powers of two. */
/* 用于作为散列表大小的素数表 */
static const unsigned int hash_table_primes[] = {
	193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
	196613, 393241, 786433, 1572869, 3145739, 6291469,
	12582917, 25165843, 50331653, 100663319, 201326611,
	402653189, 805306457, 1610612741,
};

static const unsigned int hash_table_num_primes
	= sizeof(hash_table_primes) / sizeof(int);

 /* 用来在创建散列表和扩大散列表时分配内存，是内部函数 */
static int hash_table_allocate_table(HashTable *hash_table)
{
	unsigned int new_table_size;

	/* Determine the table size based on the current prime index.
	 * An attempt is made here to ensure sensible behavior if the
	 * maximum prime is exceeded, but in practice other things are
	 * likely to break long before that happens. */
    /* 基于素数的序号决定散列表的大小 */
	if (hash_table->prime_index < hash_table_num_primes)
		new_table_size = hash_table_primes[hash_table->prime_index];
	else
		new_table_size = hash_table->entries * 10;
	hash_table->table_size = new_table_size;

    /* 给散列表分配内存空间，并初始化所有数据为NULL */
	hash_table->table = calloc(hash_table->table_size,
	                           sizeof(HashTableEntry *));
	return hash_table->table != NULL;
}

/* 释放一个结点（如果注册了free函数，调用注册的free函数） */
static void hash_table_free_entry(HashTable *hash_table, HashTableEntry *entry){

	/* 如果注册了free函数，调用注册的free函数来释放关键词 */
	if (hash_table->key_free_func != NULL)
		hash_table->key_free_func(entry->key);

	/* 对数据也类似 */
	if (hash_table->value_free_func != NULL)
		hash_table->value_free_func(entry->value);

	/* 释放数据结构占有的内存 */
	free(entry);
}

HashTable *hash_table_new(HashTableHashFunc hash_func,
                          HashTableEqualFunc equal_func){
	HashTable *hash_table;

	/* 分配一个内存空间 */
	hash_table = (HashTable *) malloc(sizeof(HashTable));
	if (hash_table == NULL)
		return NULL;
	hash_table->hash_func = hash_func;
	hash_table->equal_func = equal_func;
	hash_table->key_free_func = NULL;
	hash_table->value_free_func = NULL;
	hash_table->entries = 0;
	hash_table->prime_index = 0;

	/* 给每个结点分配内存空间，结点数量为table_size */
	if (!hash_table_allocate_table(hash_table)) {
		free(hash_table);
		return NULL;
	}
	return hash_table;
}

void hash_table_free(HashTable *hash_table)
{
	HashTableEntry *rover;
	HashTableEntry *next;
	unsigned int i;

	/* 释放所有链表的所有结点占用的内存 */
	for (i=0; i<hash_table->table_size; ++i) {
		rover = hash_table->table[i];
		while (rover != NULL) {
			next = rover->next;
			hash_table_free_entry(hash_table, rover);
			rover = next;
		}
	}

	/* 释放表头占用的内存 */
	free(hash_table->table);

	/* 释放散列表的结构 */
	free(hash_table);
}

void hash_table_register_free_functions(HashTable *hash_table,
                                        HashTableKeyFreeFunc key_free_func,
                                        HashTableValueFreeFunc value_free_func)
{
	hash_table->key_free_func = key_free_func;
	hash_table->value_free_func = value_free_func;
}


static int hash_table_enlarge(HashTable *hash_table)
{
	HashTableEntry **old_table;
	unsigned int old_table_size;
	unsigned int old_prime_index;
	HashTableEntry *rover;
	HashTableEntry *next;
	unsigned int index;
	unsigned int i;

	/* 复制一份旧表结构并存储 */
	old_table = hash_table->table;
	old_table_size = hash_table->table_size;
	old_prime_index = hash_table->prime_index;

	/* 给一个新的表分配更大的内存 */
	++hash_table->prime_index;

	if (!hash_table_allocate_table(hash_table)) {

		/* 为新表分配内存失败 */
		hash_table->table = old_table;
		hash_table->table_size = old_table_size;
		hash_table->prime_index = old_prime_index;
		return 0;
	}

	/* 把所有结点链接进新生成的表中 */
	for (i=0; i < old_table_size; ++i) {
		rover = old_table[i];

		while (rover != NULL) {
			next = rover->next;

			/* 在新的表中找到原关键词对应的链表序号 */
			index = hash_table->hash_func(rover->key)
			      % hash_table->table_size;

			/* 把结点接入序号是index的链表 */
			rover->next = hash_table->table[index];
			hash_table->table[index] = rover;

			/* 链表中的下一个 */
			rover = next;
		}
	}

	/* 释放旧表的占用的内存 */
	free(old_table);
	return 1;
}

int hash_table_insert(HashTable *hash_table, HashTableKey key,
                      HashTableValue value){
	HashTableEntry *rover;
	HashTableEntry *newentry;
	unsigned int index;

	/* 如果表中的结点过多，冲突的可能性增大，散列表的查找效率下降，此时扩大表的大小 */
	/* 当已使用的结点数量超过散列表大小的1/3 */
	if ((hash_table->entries * 3) / hash_table->table_size > 0)
		if (!hash_table_enlarge(hash_table))

			/* 分配内存失败 */
			return 0;

	/* 根据关键词找到对应链表头结点的下标 */
	index = hash_table->hash_func(key) % hash_table->table_size;

	/* 遍历整个链表来查找是否有相同关键词的结点，有则覆写，否则添加 */
	rover = hash_table->table[index];
	while (rover != NULL) {
		if (hash_table->equal_func(rover->key, key) != 0) {

			/* 若找到相同关键词，用新数据覆写结点 */
			/* 如果有释放数值和关键词内存的函数，那么释放旧内存，没有则跳过 */
			if (hash_table->value_free_func != NULL)
				hash_table->value_free_func(rover->value);
			if (hash_table->key_free_func != NULL)
				hash_table->key_free_func(rover->key);
			rover->key = key;
			rover->value = value;

			/* 覆写完成 */
			return 1;
		}
		rover = rover->next;
	}

	/* 没有关键词为key的结点，那么在新建一个并加入链表头部 */
	newentry = (HashTableEntry *) malloc(sizeof(HashTableEntry));
	if (newentry == NULL)
		return 0;
	newentry->key = key;
	newentry->value = value;

	/* 插入序号为index的链表头部 */
	newentry->next = hash_table->table[index];
	hash_table->table[index] = newentry;

	/* 链表中结点的数量增加1 */
	hash_table->entries++;

	/* 添加完成 */
	return 1;
}

HashTableValue hash_table_lookup(HashTable *hash_table, HashTableKey key){
	HashTableEntry *rover;
	unsigned int index;

	/* 根据关键词找到对应链表头结点的下标 */
	index = hash_table->hash_func(key) % hash_table->table_size;

	/* 遍历下标为index的链表直到找到关键词为key的结点 */
	rover = hash_table->table[index];
	while (rover != NULL) {
		if (hash_table->equal_func(key, rover->key) != 0)

			/* 找到目标结点，返回数据. */
			return rover->value;
		rover = rover->next;
	}

	/* 未找到结点 */
	return HASH_TABLE_NULL;
}

int hash_table_remove(HashTable *hash_table, HashTableKey key){
	HashTableEntry **rover;
	HashTableEntry *entry;
	unsigned int index;
	int result;

	/* 根据关键词找到对应链表头结点的下标 */
	index = hash_table->hash_func(key) % hash_table->table_size;
	result = 0;
	rover = &hash_table->table[index];
	while (*rover != NULL) {
		if (hash_table->equal_func(key, (*rover)->key) != 0) {

			/* 找到将要被移除的结点 */
			entry = *rover;

			/* 从链表中脱离 */
			*rover = entry->next;

			/* 销毁结点结构 */
			hash_table_free_entry(hash_table, entry);
			hash_table->entries--;
			result = 1;
			break;
		}

		/* 探查链表的下一个结点 */
		rover = &((*rover)->next);
	}
	return result;
}

unsigned int hash_table_num_entries(HashTable *hash_table)
{
	return hash_table->entries;
}

/* 初始化迭代器 */
void hash_table_iterate(HashTable *hash_table, HashTableIterator *iterator){
	unsigned int chain;
	iterator->hash_table = hash_table;
	iterator->next_entry = NULL;

	/* 找到第一个结点 */
	for (chain = 0; chain < hash_table->table_size; ++chain)
		if (hash_table->table[chain] != NULL) {
			iterator->next_entry = hash_table->table[chain];
			iterator->next_chain = chain;
			break;
		}
}

/* 判断目前的结点在同一链表中是否还有后继结点 */
int hash_table_iter_has_more(HashTableIterator *iterator){
	return iterator->next_entry != NULL;
}

/* 迭代器进入下一个结点，返回当前结点的数据 */
HashTableValue hash_table_iter_next(HashTableIterator *iterator){
	HashTableEntry *current_entry;
	HashTable *hash_table;
	HashTableValue result;
	unsigned int chain;
	hash_table = iterator->hash_table;

	/* 所有结点都已遍历完 */
	if (iterator->next_entry == NULL)
		return HASH_TABLE_NULL;

	/* 获得当前结点的数据 */
	current_entry = iterator->next_entry;
	result = current_entry->value;

	/* 判断该链表中是否还有未被遍历的结点 */
	if (current_entry->next != NULL)

		/* 继续在当前链表遍历 */
		iterator->next_entry = current_entry->next;
	else {

		/* 该链表中所有结点都被遍历，进入下一个链表 */
		chain = iterator->next_chain + 1;

		/* 初始化指针 */
		iterator->next_entry = NULL;
		while (chain < hash_table->table_size) {

			/* 判断链表中是否有结点 */
			if (hash_table->table[chain] != NULL) {
				iterator->next_entry = hash_table->table[chain];
				break;
			}

			/* 尝试下一个链表 */
			chain++;
		}
		iterator->next_chain = chain;
	}
	return result;
}

