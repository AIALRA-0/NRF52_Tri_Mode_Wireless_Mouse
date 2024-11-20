/**
 * Copyright (c) 2011 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _NRF_DELAY_H
#define _NRF_DELAY_H

#include <nrfx.h>  // 包含nrfx库，提供底层硬件抽象

#ifdef __cplusplus
extern "C" {  // 如果是C++编译器，使用C语言的符号命名规则
#endif

/**
 * @brief 延迟指定的微秒数的函数。
 *
 * 此函数用于将程序执行延迟指定的微秒数。在硬件编程中，可能需要短暂的等待时间，以等待某些硬件操作完成。
 *
 * @param us_time 要等待的微秒数。该参数指定延迟的时间，单位为微秒。
 */
#define nrf_delay_us(us_time) NRFX_DELAY_US(us_time)  // 调用底层的NRFX_DELAY_US宏，进行微秒级延迟

/**
 * @brief 延迟指定的毫秒数的函数。
 *
 * 此函数用于将程序执行延迟指定的毫秒数。通过循环调用nrf_delay_us函数来实现毫秒级别的延迟。
 *
 * @param ms_time 要等待的毫秒数。该参数指定延迟的时间，单位为毫秒。
 */
__STATIC_INLINE void nrf_delay_ms(uint32_t ms_time)
{
    if (ms_time == 0)  // 如果延迟时间为0毫秒，直接返回，不做延迟
    {
        return;
    }

    do {
        nrf_delay_us(1000);  // 每次延迟1毫秒，通过调用nrf_delay_us(1000)实现
    } while (--ms_time);  // 循环减少ms_time，直到ms_time为0，意味着延迟时间结束
}

#ifdef __cplusplus
}
#endif

#endif  // _NRF_DELAY_H

