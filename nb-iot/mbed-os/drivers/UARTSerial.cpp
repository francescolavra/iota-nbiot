/* mbed Microcontroller Library
 * Copyright (c) 2006-2017 ARM Limited
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

#include <errno.h>
#include "UARTSerial.h"
#include "platform/mbed_poll.h"

#if MBED_CONF_RTOS_PRESENT
#include "rtos/Thread.h"
#else
#endif

namespace mbed {

UARTSerial::UARTSerial(PinName tx, PinName rx, int baud) :
        SerialBase(tx, rx, baud),
        _blocking(true),
        _tx_irq_enabled(false),
        _rx_irq_enabled(true)
{
    /* Attatch IRQ routines to the serial device. */
    SerialBase::attach(callback(this, &UARTSerial::rx_irq), RxIrq);
}

UARTSerial::~UARTSerial()
{
}

void UARTSerial::dcd_irq()
{
    wake();
}

void UARTSerial::set_baud(int baud)
{
    SerialBase::baud(baud);
}

void UARTSerial::set_data_carrier_detect(PinName dcd_pin, bool active_high)
{
}

void UARTSerial::set_format(int bits, Parity parity, int stop_bits)
{
    api_lock();
    SerialBase::format(bits, parity, stop_bits);
    api_unlock();
}

#if DEVICE_SERIAL_FC
void UARTSerial::set_flow_control(Flow type, PinName flow1, PinName flow2)
{
    api_lock();
    SerialBase::set_flow_control(type, flow1, flow2);
    api_unlock();
}
#endif

int UARTSerial::close()
{
    /* Does not let us pass a file descriptor. So how to close ?
     * Also, does it make sense to close a device type file descriptor*/
    return 0;
}

int UARTSerial::isatty()
{
    return 1;

}

off_t UARTSerial::seek(off_t offset, int whence)
{
    /*XXX lseek can be done theoratically, but is it sane to mark positions on a dynamically growing/shrinking
     * buffer system (from an interrupt context) */
    return -ESPIPE;
}

int UARTSerial::sync()
{
    api_lock();
    api_unlock();

    return 0;
}

void UARTSerial::sigio(Callback<void()> func) {
    core_util_critical_section_enter();
    _sigio_cb = func;
    if (_sigio_cb) {
        short current_events = poll(0x7FFF);
        if (current_events) {
            _sigio_cb();
        }
    }
    core_util_critical_section_exit();
}

ssize_t UARTSerial::write(const void* buffer, size_t length)
{
    size_t data_written = 0;
    const char *buf_ptr = static_cast<const char *>(buffer);

    if (length == 0) {
        return 0;
    }

    api_lock();

    // Unlike read, we should write the whole thing if blocking. POSIX only
    // allows partial as a side-effect of signal handling; it normally tries to
    // write everything if blocking. Without signals we can always write all.
    while (data_written < length) {
#if __linux
        if (_fd >= 0) {
            int ret = ::write(_fd, buffer, length);

            if (ret < 0) {
                break;
            }
            data_written += ret;
        }
        else {
            break;
        }
#endif
        core_util_critical_section_enter();
        if (!_tx_irq_enabled) {
            UARTSerial::tx_irq();                // only write to hardware in one place
        }
        core_util_critical_section_exit();
    }

    api_unlock();

    return data_written != 0 ? (ssize_t) data_written : (ssize_t) -EAGAIN;
}

ssize_t UARTSerial::read(void* buffer, size_t length)
{
    size_t data_read = 0;

    char *ptr = static_cast<char *>(buffer);

    if (length == 0) {
        return 0;
    }

    api_lock();
#if __linux
 	if (_fd >= 0) {
        int ret = ::read(_fd, buffer, length);

        if (ret >= 0) {
            data_read += ret;
        }
    }
#endif

    core_util_critical_section_enter();
    if (!_rx_irq_enabled) {
        UARTSerial::rx_irq();               // only read from hardware in one place
    }
    core_util_critical_section_exit();

    api_unlock();

    return data_read;
}

bool UARTSerial::hup() const
{
    return false;
}

void UARTSerial::wake()
{
    if (_sigio_cb) {
        _sigio_cb();
    }
}

short UARTSerial::poll(short events) const {

    short revents = 0;
    /* Check the Circular Buffer if space available for writing out */


    /* POLLHUP and POLLOUT are mutually exclusive */
    if (hup()) {
        revents |= POLLHUP;
    }
    else {
#if __linux
        if (_fd >= 0) {
            struct pollfd fds;

            fds.fd = _fd;
            fds.events = events;
            if (::poll(&fds, 1, 0) > 0) {
                revents |= fds.revents;
            }
        }
#endif
    }

    /*TODO Handle other event types */

    return revents;
}

void UARTSerial::lock()
{
    // This is the override for SerialBase.
    // No lock required as we only use SerialBase from interrupt or from
    // inside our own critical section.
}

void UARTSerial::unlock()
{
    // This is the override for SerialBase.
}

void UARTSerial::api_lock(void)
{
}

void UARTSerial::api_unlock(void)
{
}

void UARTSerial::rx_irq(void)
{
}

// Also called from write to start transfer
void UARTSerial::tx_irq(void)
{
}
} //namespace mbed
