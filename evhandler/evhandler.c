/**
 * @file    ev_timer.c
 * @brief   TODO brief documentation here.
 *
 * @history
 *  - 파일 생성 및 초기 구현 (2016.06 김성중)
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include "ev_timer.h"
#include "ev_struct.h"
#include "minheap.h"

#include "atomic.h"

void
event_init(event_base_t *ev_base)
{
	event_base_t *base = ev_base;

	min_heap_ctor_(&base->timeheap);

	list_init(&base->ev_list);
	
	get_monotonic_time(&base->mono_timer.last_time);

	atomic_lock_init(&base->lock);
}

void 
event_add(event_t *ev, const struct timeval *tv)
{
	struct timeval now;
	event_base_t *base = ev->ev_base;

	if (!ev)
		assert(0);

	if (tv != NULL && (ev->ev_flags & EVLIST_TIMEOUT))
		if (min_heap_reserve_(&base->timeheap, 1 + min_heap_size_(&base->timeheap)) < 0)
			return ;

	get_monotonic_time(&now);

	timeradd(&now, tv, &ev->timeout);

	list_add_tail(&ev->link, &base->ev_list);

	if (ev->ev_flags & EVLIST_TIMEOUT) 
		min_heap_adjust_(&base->timeheap, ev);
	else
		min_heap_push_(&base->timeheap, ev);

	ev->ev_flags |= EVLIST_TIMEOUT;
}

void
event_del(event_t *ev)
{
	event_base_t *base = ev->ev_base;
	
	assert(ev->ev_flags & EVLIST_TIMEOUT);

	ev->ev_flags &= ~EVLIST_TIMEOUT;

	min_heap_erase_(&base->timeheap, ev);

	list_del(&ev->link);
}

event_t *
event_assign(event_base_t *base, void (*callback)(short, void*), int is_permanent,
		 	 timer_duration_t *duration, void *poll_msg)
{
	event_t *ev = NULL;

	if ((ev = malloc(sizeof(*ev))) < 0)
		assert(0);

	memset(ev, '\0', sizeof(ev));

	ev->ev_base = base;
	ev->ev_callback.cb_union.callback = callback;
	ev->ev_flags = EVLIST_INIT;
	memcpy(&ev->duration, duration, sizeof(*duration));

	ev->poll_msg = poll_msg;
	
	min_heap_elem_init_(ev);

	list_init(&ev->link);

	return ev;
}

void
event_deassign(event_t *ev)
{
	free(ev);
}

void
event_start(event_base_t *ev_base, void (*timeout_callback)(short, void*), 
			timer_duration_t *duration, void *poll_msg)
{
	event_t *ev;
	struct timeval timeout;

	ev = event_assign(ev_base, timeout_callback, 1, duration, poll_msg);
	timerclear(&timeout);
	timeout.tv_sec = duration->interval; timeout.tv_usec = 0;

	atomic_lock(&ev->ev_base->lock);
	event_add(ev, &timeout);
	atomic_unlock(&ev->ev_base->lock);
}

void
event_end(event_t *ev)
{
	event_deassign(ev);
}


void
get_monotonic_time(struct timeval *time_mono)
{
	struct timespec time_nsec;
	int rc;

	rc = clock_gettime(CLOCK_MONOTONIC, &time_nsec);
	if (rc < 0) {
		fprintf(stderr, "clock_gettime() failed: %s\n", strerror(errno));
		assert(0);
	}
	time_mono->tv_sec = time_nsec.tv_sec;
	time_mono->tv_usec = time_nsec.tv_nsec / 1000;
}

void
timeout_next(event_base_t *base, struct timeval **tv_p)
{
	struct timeval now;
	event_t *ev;
	struct timeval *tv = *tv_p;

	ev = min_heap_top_(&base->timeheap);
	if (ev == NULL) {
		*tv_p = NULL;
		return ;
	}

	get_monotonic_time(&now);
	if (timercmp(&ev->timeout, &now, <=)) {
		timerclear(tv);
		return ;
	}

	timersub(&ev->timeout, &now, tv);
	return ;
}

void
timeout_process(event_base_t *base)
{
	struct timeval now;
	event_t *ev;

	if (min_heap_empty_(&base->timeheap)) 
		return ;

	get_monotonic_time(&now);

	while ((ev = min_heap_top_(&base->timeheap))) {
		if (timercmp(&ev->timeout, &now, >))
			break;
		
		ev->timeout.tv_sec = ev->duration.interval;

		event_del(ev);
		//printf("size: %d\n", min_heap_size_(&base->timeheap));
		/* TODO: del event */
		ev->ev_callback.cb_union.callback(0, ev);
	}
}

/* end of ev_timer.c */
