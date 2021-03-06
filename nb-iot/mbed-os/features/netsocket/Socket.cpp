/* Socket
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

#include "Socket.h"
#include "mbed.h"

using namespace mbed;

Socket::Socket()
    : _stack(0)
    , _socket(0)
    , _timeout(-1)
{
}

nsapi_error_t Socket::open(NetworkStack *stack)
{

    if (_stack != NULL || stack == NULL) {
        return NSAPI_ERROR_PARAMETER;
    }

    nsapi_socket_t socket;
    nsapi_error_t err = stack->socket_open(&socket, get_proto());
    if (err) {
        return err;
    }

    _stack = stack;
    _socket = socket;
    _event = callback(this, &Socket::event);
    _stack->socket_attach(_socket, Callback<void()>::thunk, &_event);

    return NSAPI_ERROR_OK;
}

nsapi_error_t Socket::close()
{

    nsapi_error_t ret = NSAPI_ERROR_OK;
    if (_socket) {
        _stack->socket_attach(_socket, 0, 0);
        nsapi_socket_t socket = _socket;
        _socket = 0;
        ret = _stack->socket_close(socket);
    }
    _stack = 0;

    // Wakeup anything in a blocking operation
    // on this socket
    event();

    return ret;
}

int Socket::modify_multicast_group(const SocketAddress &address, nsapi_socket_option_t socketopt)
{
    nsapi_ip_mreq_t mreq;

    // Set up group address
    mreq.imr_multiaddr = address.get_addr();
    mreq.imr_interface = nsapi_addr_t();   // Default address, NSAPI_UNSPEC

    return this->setsockopt(NSAPI_SOCKET, socketopt, &mreq, sizeof(mreq));
}

int Socket::join_multicast_group(const SocketAddress &address)
{
    return modify_multicast_group(address, NSAPI_ADD_MEMBERSHIP);
}

int Socket::leave_multicast_group(const SocketAddress &address)
{
    return modify_multicast_group(address, NSAPI_DROP_MEMBERSHIP);
}


nsapi_error_t Socket::bind(uint16_t port)
{
    // Underlying bind is thread safe
    SocketAddress addr(0, port);
    return bind(addr);
}

nsapi_error_t Socket::bind(const char *address, uint16_t port)
{
    // Underlying bind is thread safe
    SocketAddress addr(address, port);
    return bind(addr);
}

nsapi_error_t Socket::bind(const SocketAddress &address)
{
    nsapi_error_t ret;

    if (!_socket) {
        ret = NSAPI_ERROR_NO_SOCKET;
    } else {
        ret = _stack->socket_bind(_socket, address);
    }

    return ret;
}

void Socket::set_blocking(bool blocking)
{
    // Socket::set_timeout is thread safe
    set_timeout(blocking ? -1 : 0);
}

void Socket::set_timeout(int timeout)
{
    if (timeout >= 0) {
        _timeout = (uint32_t)timeout;
    } else {
        _timeout = -1;
    }
}

nsapi_error_t Socket::setsockopt(int level, int optname, const void *optval, unsigned optlen)
{
    nsapi_error_t ret;

    if (!_socket) {
        ret = NSAPI_ERROR_NO_SOCKET;
    } else {
        ret = _stack->setsockopt(_socket, level, optname, optval, optlen);
    }

    return ret;
}

nsapi_error_t Socket::getsockopt(int level, int optname, void *optval, unsigned *optlen)
{
    nsapi_error_t ret;

    if (!_socket) {
        ret = NSAPI_ERROR_NO_SOCKET;
    } else {
        ret = _stack->getsockopt(_socket, level, optname, optval, optlen);
    }

    return ret;

}

void Socket::sigio(Callback<void()> callback)
{
    _callback = callback;
}

void Socket::attach(Callback<void()> callback)
{
    sigio(callback);
}
