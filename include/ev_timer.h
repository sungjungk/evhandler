/**
 * @file    ev_timer.h
 * @brief   TODO brief documentation here.
 *
 * @history
 *  - 파일 생성 및 초기 구현 (2016.06 김성중)
 */

#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>

//#include "common.h"
#include <stdlib.h>
#include "list.h"

#include "minheap.h"
#include "atomic.h"

typedef struct event_base_s event_base_t;
struct event_base_s {
	struct monotonic_timer {
		struct timeval last_time;
		struct timeval adjust_monotonic_clock;
		int monotonic_clock;	/* monotonic timer 사용 유무 */
	} mono_timer;
	
	min_heap_t timeheap;
	//struct event_callback *current_event;

	list_t ev_list;
	atomic_lock_t lock;
};

/* Timer 처리 관련 함수 */
void get_event(struct timeval *time_sleep);
void get_monotonic_time(struct timeval *time_mono);
void get_time_sleep(struct timeval *time_sleep, struct timeval *time_tick);
void timeout_next(event_base_t *base, struct timeval **tv_p);
void timeout_process(event_base_t *base);

/* Event 처리 관련 함수 */
void event_init(event_base_t *ev_base);
event_t* event_assign(event_base_t *base, void (*callback)(short, void*), 
					  int is_permanent, timer_duration_t *duration, 
					  void *poll_msg);
void event_deassign(event_t *ev);
void event_add(event_t *ev, const struct timeval *tv);
void event_del(event_t *ev);
void event_start(event_base_t *ev_base, void (*timeout_cb)(short, void*), 
				 timer_duration_t *duration, void *poll_msg);
void event_end(event_t *ev);

#endif
