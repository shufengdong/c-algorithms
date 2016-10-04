#include<stdio.h>
#include <stdlib.h>
#include "lstack.h"
#include "slist.h"
#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

void lstack_free(SListEntry *lstack) {
	slist_free(lstack);
}

SListEntry *lstack_push(SListEntry **lstack, SListValue data) {
	return slist_prepend(lstack, data);
}

SListValue lstack_pop(SListEntry **lstack) {
	SListValue result = slist_nth_data(*lstack, 0);
	if(slist_remove_entry(lstack, *lstack))
		return result;
	return SLIST_NULL;
}

SListValue lstack_peek(SListEntry *lstack) {
	return slist_nth_data(lstack, 0);
}

int lstack_is_empty(SListEntry *lstack) {
	return slist_is_empty(lstack);
}
