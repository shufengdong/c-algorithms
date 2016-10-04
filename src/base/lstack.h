#ifndef ALGORITHM_LSTACK_H
#define ALGORITHM_LSTACK_H
#ifdef __cplusplus
extern "C" {
#endif
#include "slist.h" 

/* 销毁一个链式栈 */
void lstack_free(SListEntry *lstack);

/* 在栈顶插入元素 */
SListEntry *lstack_push(SListEntry **lstack, SListValue data);

/* 从栈中删除栈顶元素 */
SListValue lstack_pop(SListEntry **lstack);

/* 读取栈顶元素 */
SListValue lstack_peek(SListEntry *lstack);

/* 判断栈是否为空 */
int lstack_is_empty(SListEntry *lstack);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef ALGORITHM_LSTACK_H */

