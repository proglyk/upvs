/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifndef __MQTT_INTERFACE_H_
#define __MQTT_INTERFACE_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define MQTT_TASK

// TImer

typedef struct Timer 
{
	TickType_t xTicksToWait;
	TimeOut_t xTimeOut;
} Timer;

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

// Multitasking

typedef struct Thread
{
	TaskHandle_t task;
} Thread;

typedef struct Mutex
{
	SemaphoreHandle_t sem;
} Mutex;

int ThreadStart(Thread*, void (*fn)(void*), void* arg);
void MutexInit(Mutex*);
int MutexLock(Mutex*);
int MutexUnlock(Mutex*);

// Network

typedef struct Network Network;
struct Network
{
	int socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

#endif //__MQTT_INTERFACE_H_
