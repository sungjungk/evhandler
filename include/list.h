#ifndef _LIST_H_
#define _LIST_H_


typedef struct list_head_s list_head_t;
typedef struct list_head_s list_t;
typedef struct list_head_s list_link_t;
struct list_head_s {
	list_t *next, *prev;
};

#define node_to_item(node, container, member) \
	(container *) (((char*) (node)) - offsetof(container, member))
#if 0
#define list_declare(name) \
	struct listnode name = { \
		.next = &name, \
		.prev = &name, \
	}
#endif
#define list_for_each(node, list) \
	for (node = (list)->next; node != (list); node = node->next)
#define list_for_each_reverse(node, list) \
	for (node = (list)->prev; node != (list); node = node->prev)
#define list_for_each_safe(node, n, list) \
	for (node = (list)->next, n = node->next; \
			node != (list); \
			node = n, n = node->next)
static inline void 
list_init(list_head_t *node)
{
	node->next = node;
	node->prev = node;
}

static inline void 
list_add_tail(list_head_t *head, list_t *item)
{
	item->next = head;
	item->prev = head->prev;
	head->prev->next = item;
	head->prev = item;
}

static inline void 
list_add_head(list_head_t *head, list_t *item)
{
	item->next = head->next;
	item->prev = head;
	head->next->prev = item;
	head->next = item;
}

static inline void 
list_del(list_head_t *item)
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
}

#define list_empty(list) ((list) == (list)->next)
#define list_head(list) ((list)->next)
#define list_tail(list) ((list)->prev)

#endif
