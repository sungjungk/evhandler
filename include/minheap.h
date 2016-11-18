/**
 * @file    minheap.h
 * @brief   TODO brief documentation here.
 *
 * @history
 *  - 파일 생성 및 초기 구현 (2016.06 김성중)
 *  - 현재 버전은 libevent (min-heap-internal.h 참고 했음)
 *    추후에 수정 예정
 */



#ifndef _MINHEAP_H
#define _MINHEAP_H

#include "ev_struct.h"

typedef struct min_heap_s min_heap_t;
struct min_heap_s {
	event_t** p;
	unsigned n, a;
};

#define min_heap_elem_greater(a, b) \
	(timercmp(&(a)->timeout, &(b)->timeout, >))

inline static void 
min_heap_ctor_(min_heap_t* s) 
{ 
	s->p = 0; s->n = 0; s->a = 0; 
}

inline static void
min_heap_elem_init_(event_t* ev) 
{
   	ev->min_heap_idx = -1; 
}

inline static event_t*
min_heap_top_(min_heap_t* s) 
{ 
	return s->n ? *s->p : 0; 
}

inline static int 
min_heap_empty_(min_heap_t* s) 
{ 
	return 0u == s->n;
}

inline static unsigned 
min_heap_size_(min_heap_t* s)
{
   	return s->n; 
}

inline static int 
min_heap_reserve_(min_heap_t* s, unsigned n)
{
	if (s->a < n) {
		event_t** p;
		unsigned a = s->a ? s->a * 2 : 8;
		if (a < n)
			a = n;
		if (!(p = (event_t**)realloc(s->p, a * sizeof *p)))
			return -1;
		s->p = p;
		s->a = a;
	}
	return 0;
}


inline static void 
min_heap_shift_up_unconditional_(min_heap_t* s, unsigned hole_index, event_t* e)
{
	unsigned parent = (hole_index - 1) / 2;

	do {
		(s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	} while (hole_index && min_heap_elem_greater(s->p[parent], e));

	(s->p[hole_index] = e)->min_heap_idx = hole_index;
}

inline static void 
min_heap_shift_up_(min_heap_t* s, unsigned hole_index, event_t* e)
{
	unsigned parent = (hole_index - 1) / 2;
	
	while (hole_index && min_heap_elem_greater(s->p[parent], e)) {
		(s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}

	(s->p[hole_index] = e)->min_heap_idx = hole_index;
}
inline static void
min_heap_shift_down_(min_heap_t* s, unsigned hole_index, event_t* e)
{
	unsigned min_child = 2 * (hole_index + 1);

	while (min_child <= s->n) {
		min_child -= min_child == s->n || min_heap_elem_greater(s->p[min_child], 
																s->p[min_child - 1]);

		if (!(min_heap_elem_greater(e, s->p[min_child])))
			break;

		(s->p[hole_index] = s->p[min_child])->min_heap_idx = hole_index;
		hole_index = min_child;
		min_child = 2 * (hole_index + 1);
	}

	(s->p[hole_index] = e)->min_heap_idx = hole_index;
}

inline static int 
min_heap_push_(min_heap_t* s, event_t* e)
{
	if (min_heap_reserve_(s, s->n + 1))
		return -1;

	min_heap_shift_up_(s, s->n++, e);

	return 0;
}

inline static int 
min_heap_erase_(min_heap_t* s, event_t* e)
{
	if (-1 != e->min_heap_idx) {
		event_t *last = s->p[--s->n];
		unsigned parent = (e->min_heap_idx - 1) / 2;

		if (e->min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], last))
			min_heap_shift_up_unconditional_(s, e->min_heap_idx, last);
		else
			min_heap_shift_down_(s, e->min_heap_idx, last);

		e->min_heap_idx = -1;

		return 0;
	}
	return -1;
}

inline static int 
min_heap_adjust_(min_heap_t *s, event_t *e)
{
	if (-1 == e->min_heap_idx) {
		return min_heap_push_(s, e);
	} else {
		unsigned parent = (e->min_heap_idx - 1) / 2;
		
		if (e->min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], e))
			min_heap_shift_up_unconditional_(s, e->min_heap_idx, e);
		else
			min_heap_shift_down_(s, e->min_heap_idx, e);

		return 0;
	}
}

#endif
