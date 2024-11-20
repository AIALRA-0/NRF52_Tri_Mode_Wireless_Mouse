/**
 * Copyright (c) 2016 - 2021, Nordic Semiconductor ASA
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

#ifndef NRFX_SYSTICK_H__
#define NRFX_SYSTICK_H__

#include <nrfx.h>
#include <hal/nrf_systick.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_systick ARM(R) SysTick 驱动程序
 * @{
 * @ingroup nrf_systick
 *
 * @brief   ARM(R) SysTick 驱动程序。
 *
 * 此驱动程序将 ARM(R) SysTick 配置为自由运行的定时器。
 * 该定时器用于生成延迟和轮询超时。
 * 仅支持相对较短的超时。
 * SysTick 以 64MHz 运行，并且是 24 位宽。
 * 这意味着它大约每秒溢出 4 次，库中支持的最长时间约为 250 微秒。
 * 由于不使用中断很难检测是否产生了溢出，
 * 因此出于安全原因，最大延迟范围被减半。
 */

/**
 * @brief 保存 SysTick 状态的值类型。
 *
 * 此变量用于计数请求的超时。
 * @sa nrfx_systick_get
 */
typedef struct {
    uint32_t time; //!< 记录的时间值。
} nrfx_systick_state_t;

/**
 * @brief 配置并启动定时器的函数。
 *
 * 该函数将 SysTick 配置为无中断的自由运行定时器。
 */
void nrfx_systick_init(void);

/**
 * @brief 获取当前 SysTick 状态的函数。
 *
 * 该函数获取 SysTick 定时器的当前状态。
 * 它可以通过 @ref nrfx_systick_test 检查超时。
 *
 * @param[out] p_state 要填充的状态变量指针。
 */
void nrfx_systick_get(nrfx_systick_state_t * p_state);

/**
 * @brief 测试当前时间是否高于记录的状态的函数。
 *
 * @param[in] p_state 由 @ref nrfx_systick_get 设置的记录状态。
 * @param[in] us      所需的超时时间。
 *
 * @retval true  当前时间大于指定状态加上给定超时时间。
 * @retval false 当前时间小于指定状态加上给定超时时间。
 */
bool nrfx_systick_test(nrfx_systick_state_t const * p_state, uint32_t us);

/**
 * @brief 阻塞执行指定 CPU 时钟周期数的函数。
 *
 * @param[in] ticks 阻塞执行的 CPU 时钟周期数。
 */
void nrfx_systick_delay_ticks(uint32_t ticks);

/**
 * @brief 阻塞执行指定微秒数的函数。
 *
 * @param[in] us 阻塞执行的微秒数。
 */
void nrfx_systick_delay_us(uint32_t us);

/**
 * @brief 阻塞执行指定毫秒数的函数。
 *
 * 该延迟函数去除了最大可能延迟值的限制。
 *
 * @param[in] ms 阻塞执行的毫秒数。
 */
void nrfx_systick_delay_ms(uint32_t ms);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRFX_SYSTICK_H__ */

