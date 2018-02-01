/* mbed definitions
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
#ifndef MBED_H
#define MBED_H

#define MBED_ASSERT(a)
#define MBED_DEPRECATED(m)
#define MBED_DEPRECATED_SINCE(v, m)
#define MBED_STATIC_ASSERT(a, m)
#define MBED_PRETTY_FUNCTION	__FUNCTION__
#define MBED_PRINTF_METHOD(x, y)
#define MBED_SCANF_METHOD(x, y)
#define MBED_UNREACHABLE	MBED_ASSERT(0)

#define MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE	9600

#define MBED_CONF_UBLOX_CELL_BAUD_RATE	\
	MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE

#define MBED_CONF_UBLOX_CELL_N2XX_BAUD_RATE	MBED_CONF_UBLOX_CELL_BAUD_RATE

#endif
