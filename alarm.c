#include <stdlib.h>
#include <pthread.h>
#include "alarm.h"
#include <string.h>

static alarm_t get_alarm(alarm_queue_t *queue)
{
	pthread_mutex_lock(&(queue->mutex));
	alarm_t alarm;
	//while (queue->size == 0)
			//pthread_cond_wait(&(queue->empty), &(queue->mutex));
	if(queue->size == 0)
	{
		alarm.open = 0;
		strcpy(alarm.msg, "");
	} else {
		alarm = *queue->queue[queue->out];
		-- queue->size;
		queue->out++;
		queue->out %= 100;
	}
	pthread_mutex_unlock(&(queue->mutex));
	pthread_cond_broadcast(&(queue->full));
	return alarm;
}

static void add_alarm(alarm_queue_t *queue, int runonce, int level, int freq, int escfreq, char msg[256])
{
	pthread_mutex_lock(&(queue->mutex));
	while (queue->size == 100)
		pthread_cond_wait(&(queue->full), &(queue->mutex));
	printf("Added message: %s\n", msg);
	alarm_t *alarm = queue->queue[queue->in];
	alarm->open = 1;
	alarm->runonce = runonce;
	alarm->level = level;
	alarm->levelmax = 5;
	alarm->freq = freq;
	alarm->escfreq = escfreq;
	strcpy(alarm->msg, msg);
	queue->size++;
	queue->in++;
	queue->in %= 100;
	pthread_mutex_unlock(&(queue->mutex));
	pthread_cond_broadcast(&(queue->empty));
}

static void init_alarmqueue(alarm_queue_t *queue)
{
	alarm_t *alarmbuffer[100];
	alarm_queue_t *alarmqueue;
	int i;
	for (i = 0; i < 100; i++)
	{
		alarmbuffer[i] = malloc(sizeof(alarm_t));
	}
	alarmqueue = malloc(sizeof(alarm_queue_t) * 100);
	alarmqueue->in = 0;
	alarmqueue->out = 0;
	alarmqueue->size = 0;
	//alarmqueue->empty = PTHREAD_COND_INITIALIZER();
	//alarmqueue->full = PTHREAD_COND_INITIALIZER();
	//alarmqueue->mutex = PTHREAD_MUTEX_INITIALIZER();
	memcpy(alarmqueue->queue, alarmbuffer, sizeof(alarmbuffer));
}
