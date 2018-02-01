/* Platform-related definitions
 * Copyright (c) 2018 Francesco Lavra <francescolavra.fl@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#if __linux

#include <time.h>
#include <unistd.h>

#define wait_ms(msec)	usleep(msec * 1000)

static inline unsigned long millis(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static inline unsigned long micros(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

#elif defined FREERTOS

#include <FreeRTOS.h>
#include <task.h>

#define wait_ms(msec) vTaskDelay(msec / portTICK_PERIOD_MS)

#define millis() (xTaskGetTickCount() * portTICK_PERIOD_MS)
#define micros() (millis() * 1000)

#endif

#define core_util_critical_section_enter()
#define core_util_critical_section_exit()

typedef int PinName;
typedef unsigned long us_timestamp_t;

#endif
