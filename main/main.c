/**
 * @file    main.c
 * @brief   TODO
 *
 * @authors	Seong-Joong Kim (sungjungk@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"

#include "ev_timer.h"
#include "ev_struct.h"

struct timeval lasttime;

typedef struct poll_msg_s poll_msg_t;
struct poll_msg_s {
	uint16_t interval;
	uint16_t type;

	char *msg;
};



void poll_attach_msg(event_base_t *ev_base);
void poll_detach_msg(event_t *ev);
poll_msg_t * gen_msg(void);
void event_append(event_base_t *base, poll_msg_t *poll_msg);


static void
timeout_callback(short event, void *arg)
{
	struct timeval newtime, difference;
	double elapsed;

	event_t *ev = arg;

	gettimeofday(&newtime, NULL);
	timersub(&newtime, &lasttime, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);
	
#if 1
	printf("timeout_cb called at %d: %.3f seconds elapsed\n",
			(int)newtime.tv_sec, elapsed);
#endif
	lasttime = newtime;

	/* TODO: Fill in the blank below space with your intention */

	if ((ev->duration.limit != -1) && (--ev->duration.limit <= 0)) {
		goto clean;
	} else {
		struct timeval tv;
		timerclear(&tv);
		tv.tv_sec = ev->duration.interval;
		atomic_lock(&ev->ev_base->lock);
		event_add(ev, &tv);
		atomic_unlock(&ev->ev_base->lock);
	}
	return;

clean:
	poll_detach_msg(ev);
}

int
main(int argc, char *argv[])
{
	event_base_t ev_base;
	struct timeval tv;
	struct timeval *tv_p;

	/* Event callback */
	event_init(&ev_base);

	poll_attach_msg(&ev_base);

	while (true) {
		tv_p = &tv;

		timeout_next(&ev_base, &tv_p);
		
		timeout_process(&ev_base);
	}

	printf("Event handler test done\n\n");

	return 0;
}

void
poll_attach_msg(event_base_t *ev_base)
{
	poll_msg_t *poll_msg;

	poll_msg = gen_msg();

	/* Append a message for event callback function */
	event_append(ev_base, poll_msg);
	
	return ;
}

void
poll_detach_msg(event_t *ev)
{
	if (!ev->poll_msg) {
		event_end(ev);
		return ;
	}

	/* free an msg. */
	free(((poll_msg_t*)ev->poll_msg)->msg);

	/* free a poll_msg */
	free(ev->poll_msg);

	/* arrnage an event */
	event_end(ev);
}

poll_msg_t *
gen_msg(void)
{
	poll_msg_t *poll_msg;
	char msg[20] = "Echo Msg";

	poll_msg = (poll_msg_t*) malloc (sizeof(*poll_msg));

	
	/* Type definitions are as follows:
	 *	0x01 : 
	 */
#define TYPE		0x0001 	/* TYPE  */
#define INTERVAL	10
#define MSG_LEN		20
	poll_msg->type = TYPE;
	poll_msg->interval = INTERVAL;
	poll_msg->msg = malloc(MSG_LEN);
	memcpy(poll_msg->msg, msg, MSG_LEN);

	return poll_msg;
}

void
event_append(event_base_t *base, poll_msg_t *poll_msg)
{
	timer_duration_t duration;

	/* Timer setup */
	duration.interval = poll_msg->interval;
	duration.limit = -1;
	event_start(base, timeout_callback, &duration, (void*)poll_msg);
}

/* end of main.c */
