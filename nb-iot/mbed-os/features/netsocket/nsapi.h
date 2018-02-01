
/** \addtogroup netsocket */
/** @{*/
/* nsapi.h - The network socket API
 * Copyright (c) 2015 ARM Limited
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

#ifndef NSAPI_H
#define NSAPI_H


// entry point for nsapi types
#include "nsapi_types.h"

#ifdef __cplusplus

// entry point for C++ api
#include "netsocket/SocketAddress.h"
#include "netsocket/NetworkStack.h"

#include "netsocket/NetworkInterface.h"
#include "netsocket/CellularInterface.h"

#include "netsocket/Socket.h"
#include "netsocket/UDPSocket.h"

#endif


#endif

/** @}*/
