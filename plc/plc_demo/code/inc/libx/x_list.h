#ifndef __X_LIST_H__
#define __X_LIST_H__

struct xlist_head {
    struct xlist_head *next, *prev;
};

struct xhlist_head {
    struct xhlist_node *first;
};

struct xhlist_node {
    struct xhlist_node *next, **pprev;
};

#define __compiler_xoffsetof(a,b) __builtin_offsetof(a,b)
#define xoffsetof(TYPE,MEMBER) __compiler_xoffsetof(TYPE,MEMBER)

/**
 * xcontainer_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */

#define xcontainer_of(ptr, type, member) ((type *)((char *)ptr - xoffsetof(type, member)))

#if 0
#define xcontainer_of(ptr, type, member) ({         \
    const typeof(((type *)0)->member) * __mptr = (ptr); \
    (type *)((char *)__mptr - xoffsetof(type, member)); })
#endif

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

#define XLIST_HEAD_INIT(name) { &(name), &(name) }

#define XLIST_HEAD(name) \
    struct xlist_head name = XLIST_HEAD_INIT(name)

static inline void INIT_XLIST_HEAD(struct xlist_head *list)
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

static inline void __xlist_add(struct xlist_head *new,
                  struct xlist_head *prev,
                  struct xlist_head *next)
{
    new->next = next;
    new->prev = prev;
    //mb();
    prev->next = new;
    next->prev = new;
}


/**
 * xlist_add_head - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void xlist_add_head(struct xlist_head *new, struct xlist_head *head)
{
    __xlist_add(new, head, head->next);
}


/**
 * xlist_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void xlist_add_tail(struct xlist_head *new, struct xlist_head *head)
{
    __xlist_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __xlist_del(struct xlist_head * prev, struct xlist_head * next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * xlist_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: xlist_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void __xlist_del_entry(struct xlist_head *entry)
{
    __xlist_del(entry->prev, entry->next);
}

static inline void xlist_del(struct xlist_head *entry)
{
    __xlist_del(entry->prev, entry->next);
    //entry->next = NULL;
    //entry->prev = NULL;
}


/**
 * xlist_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void xlist_replace(struct xlist_head *old,
                struct xlist_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

static inline void xlist_replace_init(struct xlist_head *old,
                    struct xlist_head *new)
{
    xlist_replace(old, new);
    INIT_XLIST_HEAD(old);
}

/**
 * xlist_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void xlist_del_init(struct xlist_head *entry)
{
    __xlist_del_entry(entry);
    INIT_XLIST_HEAD(entry);
}

/**
 * xlist_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void xlist_move(struct xlist_head *list, struct xlist_head *head)
{
    __xlist_del_entry(list);
    xlist_add_head(list, head);
}

/**
 * xlist_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void xlist_move_tail(struct xlist_head *list,
                  struct xlist_head *head)
{
    __xlist_del_entry(list);
    xlist_add_tail(list, head);
}

/**
 * xlist_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int xlist_is_last(const struct xlist_head *list,
                const struct xlist_head *head)
{
    return list->next == head;
}

/**
 * xlist_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int xlist_empty(const struct xlist_head *head)
{
    return head->next == head;
}

/**
 * xlist_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using xlist_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is xlist_del_init(). Eg. it cannot be used
 * if another CPU could re-xlist_add() it.
 */
static inline int xlist_empty_careful(const struct xlist_head *head)
{
    struct xlist_head *next = head->next;
    return (next == head) && (next == head->prev);
}

/**
 * xlist_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void xlist_rotate_left(struct xlist_head *head)
{
    struct xlist_head *first;

    if (!xlist_empty(head)) {
        first = head->next;
        xlist_move_tail(first, head);
    }
}

/**
 * xlist_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int xlist_is_singular(const struct xlist_head *head)
{
    return !xlist_empty(head) && (head->next == head->prev);
}

static inline void __xlist_cut_position(struct xlist_head *list,
        struct xlist_head *head, struct xlist_head *entry)
{
    struct xlist_head *new_first = entry->next;
    list->next = head->next;
    list->next->prev = list;
    list->prev = entry;
    entry->next = list;
    head->next = new_first;
    new_first->prev = head;
}

/**
 * xlist_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *  and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void xlist_cut_position(struct xlist_head *list,
        struct xlist_head *head, struct xlist_head *entry)
{
    if (xlist_empty(head))
        return;
    if (xlist_is_singular(head) &&
        (head->next != entry && head != entry))
        return;
    if (entry == head)
        INIT_XLIST_HEAD(list);
    else
        __xlist_cut_position(list, head, entry);
}

static inline void __xlist_splice(const struct xlist_head *list,
                 struct xlist_head *prev,
                 struct xlist_head *next)
{
    struct xlist_head *first = list->next;
    struct xlist_head *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

/**
 * xlist_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void xlist_splice(const struct xlist_head *list,
                struct xlist_head *head)
{
    if (!xlist_empty(list))
        __xlist_splice(list, head, head->next);
}

/**
 * xlist_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void xlist_splice_tail(struct xlist_head *list,
                struct xlist_head *head)
{
    if (!xlist_empty(list))
        __xlist_splice(list, head->prev, head);
}

/**
 * xlist_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void xlist_splice_init(struct xlist_head *list,
                    struct xlist_head *head)
{
    if (!xlist_empty(list)) {
        __xlist_splice(list, head, head->next);
        INIT_XLIST_HEAD(list);
    }
}

/**
 * xlist_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void xlist_splice_tail_init(struct xlist_head *list,
                     struct xlist_head *head)
{
    if (!xlist_empty(list)) {
        __xlist_splice(list, head->prev, head);
        INIT_XLIST_HEAD(list);
    }
}

/**
 * xlist_entry - get the struct for this entry
 * @ptr:    the &struct xlist_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define xlist_entry(ptr, type, member) \
    xcontainer_of(ptr, type, member)

/**
 * xlist_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define xlist_first_entry(ptr, type, member) \
    xlist_entry((ptr)->next, type, member)

/**
 * xlist_for_each   -   iterate over a list
 * @pos:    the &struct xlist_head to use as a loop cursor.
 * @head:   the head for your list.
 */
#define xlist_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * __xlist_for_each -   iterate over a list
 * @pos:    the &struct xlist_head to use as a loop cursor.
 * @head:   the head for your list.
 *
 * This variant doesn't differ from xlist_for_each() any more.
 * We don't do prefetching in either case.
 */
#define __xlist_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * xlist_for_each_prev  -   iterate over a list backwards
 * @pos:    the &struct xlist_head to use as a loop cursor.
 * @head:   the head for your list.
 */
#define xlist_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * xlist_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the &struct xlist_head to use as a loop cursor.
 * @n:      another &struct xlist_head to use as temporary storage
 * @head:   the head for your list.
 */
#define xlist_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * xlist_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:    the &struct xlist_head to use as a loop cursor.
 * @n:      another &struct xlist_head to use as temporary storage
 * @head:   the head for your list.
 */
#define xlist_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; \
         pos != (head); \
         pos = n, n = pos->prev)

/**
 * xlist_for_each_entry -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define xlist_for_each_entry(pos, head, member)             \
    for (pos = xlist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head);    \
         pos = xlist_entry(pos->member.next, typeof(*pos), member))

/**
 * xlist_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define xlist_for_each_entry_reverse(pos, head, member)         \
    for (pos = xlist_entry((head)->prev, typeof(*pos), member); \
         &pos->member != (head);    \
         pos = xlist_entry(pos->member.prev, typeof(*pos), member))

/**
 * xlist_prepare_entry - prepare a pos entry for use in xlist_for_each_entry_continue()
 * @pos:    the type * to use as a start point
 * @head:   the head of the list
 * @member: the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in xlist_for_each_entry_continue().
 */
#define xlist_prepare_entry(pos, head, member) \
    ((pos) ? : xlist_entry(head, typeof(*pos), member))

/**
 * xlist_for_each_entry_continue - continue iteration over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define xlist_for_each_entry_continue(pos, head, member)        \
    for (pos = xlist_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head);    \
         pos = xlist_entry(pos->member.next, typeof(*pos), member))

/**
 * xlist_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define xlist_for_each_entry_continue_reverse(pos, head, member)        \
    for (pos = xlist_entry(pos->member.prev, typeof(*pos), member); \
         &pos->member != (head);    \
         pos = xlist_entry(pos->member.prev, typeof(*pos), member))

/**
 * xlist_for_each_entry_from - iterate over list of given type from the current point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define xlist_for_each_entry_from(pos, head, member)            \
    for (; &pos->member != (head);  \
         pos = xlist_entry(pos->member.next, typeof(*pos), member))

/**
 * xlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define xlist_for_each_entry_safe(pos, n, head, member)         \
    for (pos = xlist_entry((head)->next, typeof(*pos), member), \
        n = xlist_entry(pos->member.next, typeof(*pos), member);    \
         &pos->member != (head);                    \
         pos = n, n = xlist_entry(n->member.next, typeof(*n), member))

/**
 * xlist_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define xlist_for_each_entry_safe_continue(pos, n, head, member)        \
    for (pos = xlist_entry(pos->member.next, typeof(*pos), member),         \
        n = xlist_entry(pos->member.next, typeof(*pos), member);        \
         &pos->member != (head);                        \
         pos = n, n = xlist_entry(n->member.next, typeof(*n), member))

/**
 * xlist_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define xlist_for_each_entry_safe_from(pos, n, head, member)            \
    for (n = xlist_entry(pos->member.next, typeof(*pos), member);       \
         &pos->member != (head);                        \
         pos = n, n = xlist_entry(n->member.next, typeof(*n), member))

/**
 * xlist_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define xlist_for_each_entry_safe_reverse(pos, n, head, member)     \
    for (pos = xlist_entry((head)->prev, typeof(*pos), member), \
        n = xlist_entry(pos->member.prev, typeof(*pos), member);    \
         &pos->member != (head);                    \
         pos = n, n = xlist_entry(n->member.prev, typeof(*n), member))

/**
 * xlist_safe_reset_next - reset a stale xlist_for_each_entry_safe loop
 * @pos:    the loop cursor used in the xlist_for_each_entry_safe loop
 * @n:      temporary storage used in xlist_for_each_entry_safe
 * @member: the name of the list_struct within the struct.
 *
 * xlist_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and xlist_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define xlist_safe_reset_next(pos, n, member)               \
    n = xlist_entry(pos->member.next, typeof(*pos), member)

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

#define XHLIST_HEAD_INIT { .first = NULL }
#define XHLIST_HEAD(name) struct xhlist_head name = {  .first = NULL }
#define INIT_XHLIST_HEAD(ptr) ((ptr)->first = NULL)
static inline void INIT_XHLIST_NODE(struct xhlist_node *h)
{
    h->next = NULL;
    h->pprev = NULL;
}

static inline int xhlist_unhashed(const struct xhlist_node *h)
{
    return !h->pprev;
}

static inline int xhlist_empty(const struct xhlist_head *h)
{
    return !h->first;
}

static inline void __xhlist_del(struct xhlist_node *n)
{
    struct xhlist_node *next = n->next;
    struct xhlist_node **pprev = n->pprev;
    *pprev = next;
    if (next)
        next->pprev = pprev;
}

static inline void xhlist_del(struct xhlist_node *n)
{
    __xhlist_del(n);
    n->next = NULL;
    n->pprev = NULL;
}

static inline void xhlist_del_init(struct xhlist_node *n)
{
    if (!xhlist_unhashed(n)) {
        __xhlist_del(n);
        INIT_XHLIST_NODE(n);
    }
}

static inline void xhlist_add_head(struct xhlist_node *n, struct xhlist_head *h)
{
    struct xhlist_node *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}

/* next must be != NULL */
static inline void xhlist_add_before(struct xhlist_node *n,
                    struct xhlist_node *next)
{
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

static inline void xhlist_add_after(struct xhlist_node *n,
                    struct xhlist_node *next)
{
    next->next = n->next;
    n->next = next;
    next->pprev = &n->next;

    if(next->next)
        next->next->pprev  = &next->next;
}

/* after that we'll appear to be on some hlist and xhlist_del will work */
static inline void xhlist_add_fake(struct xhlist_node *n)
{
    n->pprev = &n->next;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static inline void xhlist_move_list(struct xhlist_head *old,
                   struct xhlist_head *new)
{
    new->first = old->first;
    if (new->first)
        new->first->pprev = &new->first;
    old->first = NULL;
}

#define xhlist_entry(ptr, type, member) xcontainer_of(ptr,type,member)

#define xhlist_for_each(pos, head) \
    for (pos = (head)->first; pos ; pos = pos->next)

#define xhlist_for_each_safe(pos, n, head) \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
         pos = n)

/**
 * xhlist_for_each_entry    - iterate over list of given type
 * @tpos:   the type * to use as a loop cursor.
 * @pos:    the &struct xhlist_node to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the xhlist_node within the struct.
 */
#define xhlist_for_each_entry(tpos, pos, head, member)           \
    for (pos = (head)->first;                    \
         pos &&                          \
        ({ tpos = xhlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)

/**
 * xhlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @tpos:   the type * to use as a loop cursor.
 * @pos:    the &struct xhlist_node to use as a loop cursor.
 * @member: the name of the xhlist_node within the struct.
 */
#define xhlist_for_each_entry_continue(tpos, pos, member)        \
    for (pos = (pos)->next;                      \
         pos &&                          \
        ({ tpos = xhlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)

/**
 * xhlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @tpos:   the type * to use as a loop cursor.
 * @pos:    the &struct xhlist_node to use as a loop cursor.
 * @member: the name of the xhlist_node within the struct.
 */
#define xhlist_for_each_entry_from(tpos, pos, member)            \
    for (; pos &&                            \
        ({ tpos = xhlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)

/**
 * xhlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @tpos:   the type * to use as a loop cursor.
 * @pos:    the &struct xhlist_node to use as a loop cursor.
 * @n:      another &struct xhlist_node to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the xhlist_node within the struct.
 */
#define xhlist_for_each_entry_safe(tpos, pos, n, head, member)       \
    for (pos = (head)->first;                    \
         pos && ({ n = pos->next; 1; }) &&               \
        ({ tpos = xhlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = n)

#endif


