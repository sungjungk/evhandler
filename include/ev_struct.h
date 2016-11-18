/**
 * @file    ev_struct.h
 * @brief   TODO brief documentation here.
 *
 * @history
 *  - ���� ���� �� �ʱ� ���� (2016.06 �輺��)
 */

#ifndef _EVENT_STRUCT_H
#define _EVENT_STRUCT_H

#include "ev_timer.h"

#define EVLIST_TIMEOUT	0x01
#define EVLIST_INIT		0x80

typedef struct event_callback_s event_callback_t;
struct event_callback_s {
	void *arg;
	union {
		void (*callback)(short, void*);
	} cb_union;
};

typedef struct timer_duration_s timer_duration_t;
struct timer_duration_s {
	long interval;	/* Timer interval �� */
	int limit;		/* Timer duration ���� �� */
};

typedef struct event_base_s event_base_t;
typedef struct event_s event_t;
struct event_s {
	/* TODO: Multiple callback & priority �߰� �ʿ�*/
	event_callback_t ev_callback;
	event_base_t *ev_base;

	int min_heap_idx;
	int ev_flags;

	struct timeval timeout;
	timer_duration_t duration;

	void *poll_msg;

	list_t link;
};



#endif
