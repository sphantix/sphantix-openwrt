/***********************************************************************
 *
 *  Copyright (c) 2015 sphantix
 *  All Rights Reserved
 *
# 
# 
# 
#
 * 
 ************************************************************************/

#ifndef __UTL_LIST_H__
#define __UTL_LIST_H__

#include "utl.h"

/*!\file cms_dlist.h
 * \brief Header file for doubly linked list manipulation functions.
 *
 * These functions implement doubly linked list.
 *
 */

/*! \brief structure that must be placed at the begining of any structure
 *         that is to be put into the linked list.
 */
typedef struct dlist_node {
    struct dlist_node *next;   /**< next pointer */
    struct dlist_node *prev;   /**< previous pointer */
} DlistNode;


/** Initialize a field in a structure that is used as the head of a dlist */
#define DLIST_HEAD_IN_STRUCT_INIT(field) do {\
    (field).next = &(field);               \
    (field).prev = &(field);               \
} while (0)

/** Initialize a standalone variable that is the head of a dlist */
#define DLIST_HEAD_INIT(name) { &(name), &(name) }

/** Declare a standalone variable that is the head of the dlist */
#define DLIST_HEAD(name) \
    struct dlist_node name = DLIST_HEAD_INIT(name)


static inline void INIT_DLIST_HEAD(struct dlist_node *list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __dlist_add(struct dlist_node *new, struct dlist_node *prev, struct dlist_node *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * dlist_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void dlist_add(struct dlist_node *new, struct dlist_node *head)
{
	__dlist_add(new, head, head->next);
}

/**
 * dlist_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void dlist_add_tail(struct dlist_node *new, struct dlist_node *head)
{
	__dlist_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __dlist_del(struct dlist_node * prev, struct dlist_node * next)
{
	next->prev = prev;
	prev->next = next;
}

/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

static inline void dlist_del(struct dlist_node *entry)
{
	__dlist_del(entry->prev, entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

static inline void __dlist_del_entry(struct dlist_node *entry)
{
	__dlist_del(entry->prev, entry->next);
}

/**
 * dlist_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void dlist_replace(struct dlist_node *old, struct dlist_node *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void dlist_replace_init(struct dlist_node *old, struct dlist_node *new)
{
	dlist_replace(old, new);
	INIT_DLIST_HEAD(old);
}

/**
 * dlist_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void dlist_del_init(struct dlist_node *entry)
{
	__dlist_del_entry(entry);
	INIT_DLIST_HEAD(entry);
}

/**
 * dlist_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void dlist_move(struct dlist_node *list, struct dlist_node *head)
{
	__dlist_del_entry(list);
	dlist_add(list, head);
}

/**
 * dlist_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void dlist_move_tail(struct dlist_node *list, struct dlist_node *head)
{
	__dlist_del_entry(list);
	dlist_add_tail(list, head);
}

/**
 * dlist_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int dlist_is_last(const struct dlist_node *list, const struct dlist_node *head)
{
	return list->next == head;
}

/** Return true if the dlist is empty.
 *
 * @param head pointer to the head of the dlist.
 */
static inline int dlist_empty(const struct dlist_node *head)
{
    return ((head->next == head) && (head->prev == head));
}

/**
 * dlist_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void dlist_rotate_left(struct dlist_node *head)
{
    struct dlist_node *first;

    if (!dlist_empty(head)) {
        first = head->next;
        dlist_move_tail(first, head);
    }
}

/**
 * dlist_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int dlist_is_singular(const struct dlist_node *head)
{
	return !dlist_empty(head) && (head->next == head->prev);
}

static inline void __dlist_cut_position(struct dlist_node *list, struct dlist_node *head, struct dlist_node *entry)
{
	struct dlist_node *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

/**
 * dlist_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void dlist_cut_position(struct dlist_node *list, struct dlist_node *head, struct dlist_node *entry)
{
	if (dlist_empty(head))
		return;
	if (dlist_is_singular(head) &&
		(head->next != entry && head != entry))
		return;
	if (entry == head)
		INIT_DLIST_HEAD(list);
	else
		__dlist_cut_position(list, head, entry);
}

static inline void __dlist_splice(const struct dlist_node *list, struct dlist_node *prev, struct dlist_node *next)
{
	struct dlist_node *first = list->next;
	struct dlist_node *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * dlist_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void dlist_splice(const struct dlist_node *list, struct dlist_node *head)
{
	if (!dlist_empty(list))
		__dlist_splice(list, head, head->next);
}

/**
 * dlist_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void dlist_splice_tail(struct dlist_node *list, struct dlist_node *head)
{
	if (!dlist_empty(list))
		__dlist_splice(list, head->prev, head);
}

/**
 * dlist_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void dlist_splice_init(struct dlist_node *list, struct dlist_node *head)
{
	if (!dlist_empty(list)) {
		__dlist_splice(list, head, head->next);
		INIT_DLIST_HEAD(list);
	}
}

/**
 * dlist_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void dlist_splice_tail_init(struct dlist_node *list, struct dlist_node *head)
{
	if (!dlist_empty(list)) {
		__dlist_splice(list, head->prev, head);
		INIT_DLIST_HEAD(list);
	}
}

/** add a new entry after an existing list element
 *
 * @param new       new entry to be added
 * @param existing  list element to add the new entry after.  This could
 *                  be the list head or it can be any element in the dlist.
 *
 */
static inline void dlist_append(struct dlist_node *new_node, struct dlist_node *existing)
{
    existing->next->prev = new_node;
    new_node->next = existing->next;
    new_node->prev = existing;
    existing->next = new_node;
}


/** add a new entry in front of an existing list element
 *
 * @param new       new entry to be added
 * @param existing  list element to add the new entry in front of.  This could
 *                  be the list head or it can be any element in the dlist.
 *
 */
static inline void dlist_prepend(struct dlist_node *new_node, struct dlist_node *existing)
{
    existing->prev->next = new_node;
    new_node->next = existing;
    new_node->prev = existing->prev;
    existing->prev = new_node;
}


#define dlist_entry(ptr, type, member) \
    container_of(ptr, type, member)


/**
 * dlist_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define dlist_first_entry(ptr, type, member) \
	dlist_entry((ptr)->next, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define dlist_first_entry_or_null(ptr, type, member) \
	(!dlist_empty(ptr) ? dlist_first_entry(ptr, type, member) : NULL)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_struct within the struct.
 */
#define dlist_next_entry(pos, member) \
	dlist_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_struct within the struct.
 */
#define dlist_prev_entry(pos, member) \
	dlist_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * dlist_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define dlist_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * dlist_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define dlist_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * dlist_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define dlist_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * dlist_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define dlist_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/** Create a for loop over all entries in the dlist.
 *
 * @param pos A variable that is the type of the structure which
 *            contains the DlistNode.
 * @param head Pointer to the head of the dlist.
 * @param member The field name of the DlistNode field in the
 *               containing structure.
 *
 */
#define dlist_for_each_entry(pos, head, member)				\
    for (pos = dlist_entry((head)->next, typeof(*pos), member);	\
            &pos->member != (head); 					\
            pos = dlist_entry(pos->member.next, typeof(*pos), member))

/**
 * dlist_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define dlist_for_each_entry_reverse(pos, head, member)			\
	for (pos = dlist_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = dlist_entry(pos->member.prev, typeof(*pos), member))

/**
 * dlist_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define dlist_prepare_entry(pos, head, member) \
	((pos) ? : dlist_entry(head, typeof(*pos), member))

/**
 * dlist_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define dlist_for_each_entry_continue(pos, head, member) 		\
	for (pos = dlist_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = dlist_entry(pos->member.next, typeof(*pos), member))

/**
 * dlist_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define dlist_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = dlist_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = dlist_entry(pos->member.prev, typeof(*pos), member))

/**
 * dlist_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define dlist_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = dlist_entry(pos->member.next, typeof(*pos), member))

/**
 * dlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define dlist_for_each_entry_safe(pos, n, head, member)			\
	for (pos = dlist_entry((head)->next, typeof(*pos), member),	\
		n = dlist_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = dlist_entry(n->member.next, typeof(*n), member))

/**
 * dlist_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define dlist_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = dlist_entry(pos->member.next, typeof(*pos), member), 		\
		n = dlist_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = dlist_entry(n->member.next, typeof(*n), member))

/**
 * dlist_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define dlist_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = dlist_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = dlist_entry(n->member.next, typeof(*n), member))

/**
 * dlist_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define dlist_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = dlist_entry((head)->prev, typeof(*pos), member),	\
		n = dlist_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = dlist_entry(n->member.prev, typeof(*n), member))

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = {  .first = NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

static inline void INIT_HLIST_NODE(struct hlist_node *h)
{
	h->next = NULL;
	h->pprev = NULL;
}

static inline int hlist_unhashed(const struct hlist_node *h)
{
	return !h->pprev;
}

static inline int hlist_empty(const struct hlist_head *h)
{
	return !h->first;
}

static inline void __hlist_del(struct hlist_node *n)
{
	struct hlist_node *next = n->next;
	struct hlist_node **pprev = n->pprev;
	*pprev = next;
	if (next)
		next->pprev = pprev;
}

static inline void hlist_del(struct hlist_node *n)
{
	__hlist_del(n);
	n->next = LIST_POISON1;
	n->pprev = LIST_POISON2;
}

static inline void hlist_del_init(struct hlist_node *n)
{
	if (!hlist_unhashed(n)) {
		__hlist_del(n);
		INIT_HLIST_NODE(n);
	}
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
	struct hlist_node *first = h->first;
	n->next = first;
	if (first)
		first->pprev = &n->next;
	h->first = n;
	n->pprev = &h->first;
}

/* next must be != NULL */
static inline void hlist_add_before(struct hlist_node *n,
					struct hlist_node *next)
{
	n->pprev = next->pprev;
	n->next = next;
	next->pprev = &n->next;
	*(n->pprev) = n;
}

static inline void hlist_add_after(struct hlist_node *n,
					struct hlist_node *next)
{
	next->next = n->next;
	n->next = next;
	next->pprev = &n->next;

	if(next->next)
		next->next->pprev  = &next->next;
}

/* after that we'll appear to be on some hlist and hlist_del will work */
static inline void hlist_add_fake(struct hlist_node *n)
{
	n->pprev = &n->next;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static inline void hlist_move_list(struct hlist_head *old,
				   struct hlist_head *new)
{
	new->first = old->first;
	if (new->first)
		new->first->pprev = &new->first;
	old->first = NULL;
}

#define hlist_entry(ptr, type, member) container_of(ptr,type,member)

#define hlist_for_each(pos, head) \
	for (pos = (head)->first; pos ; pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
	     pos = n)

#define hlist_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   ____ptr ? hlist_entry(____ptr, type, member) : NULL; \
	})

/**
 * hlist_for_each_entry	- iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(pos, head, member)				\
	for (pos = hlist_entry_safe((head)->first, typeof(*(pos)), member);\
	     pos;							\
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @pos:	the type * to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_continue(pos, member)			\
	for (pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member);\
	     pos;							\
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @pos:	the type * to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_from(pos, member)				\
	for (; pos;							\
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another &struct hlist_node to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_safe(pos, n, head, member) 		\
	for (pos = hlist_entry_safe((head)->first, typeof(*pos), member);\
	     pos && ({ n = pos->member.next; 1; });			\
	     pos = hlist_entry_safe(n, typeof(*pos), member))

#endif  /*__UTL_LIST_H__ */
