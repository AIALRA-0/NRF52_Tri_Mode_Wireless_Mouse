/**
 * Copyright (c) 2017 - 2021, Nordic Semiconductor ASA
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

#ifndef NRFX_GLUE_H__
#define NRFX_GLUE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_glue nrfx_glue.h
 * @{
 * @ingroup nrfx
 *
 * @brief 此文件包含一些根据@em nrfx 集成到宿主环境的需要实现的宏。
 */

#include <legacy/apply_old_config.h>  // 引入旧配置的应用文件，用于兼容老版本配置。

#include <soc/nrfx_irqs.h>  // 引入nrfx中与中断请求（IRQ）相关的头文件，处理SoC的IRQ配置。

//------------------------------------------------------------------------------

// 包含断言功能的头文件
#include <nrf_assert.h>
/**
 * @brief 放置运行时断言的宏。
 *
 * @param expression 要求评估的表达式。
 */
#define NRFX_ASSERT(expression)     ASSERT(expression)

// 包含实用功能的头文件
#include <app_util.h>
/**
 * @brief 放置编译时断言的宏。
 *
 * @param expression 要求评估的表达式。
 */
#define NRFX_STATIC_ASSERT(expression)  STATIC_ASSERT(expression)

//------------------------------------------------------------------------------

// SoftDevice 是 Nordic Semiconductor 提供的可选蓝牙协议栈或无线协议栈，作为固件在其芯片上运行，用于管理无线通信
#ifdef NRF51
#ifdef SOFTDEVICE_PRESENT
// 对于NRF51平台且存在SoftDevice时，检查中断优先级是否有效。
// SoftDevice保留优先级1和3，因此只有这两个优先级是有效的。
#define INTERRUPT_PRIORITY_IS_VALID(pri) (((pri) == 1) || ((pri) == 3))
#else
// 对于NRF51平台且没有SoftDevice时，任何小于4的优先级都是有效的。
#define INTERRUPT_PRIORITY_IS_VALID(pri) ((pri) < 4)
#endif // SOFTDEVICE_PRESENT
#else
#ifdef SOFTDEVICE_PRESENT
// 对于非NRF51平台且存在SoftDevice时，优先级2、3，5到7是有效的。
// 这意味着中断优先级不能为1和4，因为这两个优先级被SoftDevice保留。
#define INTERRUPT_PRIORITY_IS_VALID(pri) ((((pri) > 1) && ((pri) < 4)) || \
                                          (((pri) > 4) && ((pri) < 8)))
#else
// 对于非NRF51平台且没有SoftDevice时，任何小于8的优先级都是有效的。
#define INTERRUPT_PRIORITY_IS_VALID(pri) ((pri) < 8)
#endif // SOFTDEVICE_PRESENT
#endif // NRF52


/**
 * @brief 设置特定IRQ优先级的宏。
 *
 * @param irq_number  IRQ编号。
 * @param priority    要设置的优先级。
 */
#define NRFX_IRQ_PRIORITY_SET(irq_number, priority) \
    _NRFX_IRQ_PRIORITY_SET(irq_number, priority)
static inline void _NRFX_IRQ_PRIORITY_SET(IRQn_Type irq_number,
                                          uint8_t   priority)
{
    ASSERT(INTERRUPT_PRIORITY_IS_VALID(priority));
    NVIC_SetPriority(irq_number, priority);
}

/**
 * @brief 启用特定IRQ的宏。
 *
 * @param irq_number  IRQ编号。
 */
#define NRFX_IRQ_ENABLE(irq_number)  _NRFX_IRQ_ENABLE(irq_number)
static inline void _NRFX_IRQ_ENABLE(IRQn_Type irq_number)
{
    NVIC_EnableIRQ(irq_number);
}

/**
 * @brief 检查特定IRQ是否启用的宏。
 *
 * @param irq_number  IRQ编号。
 *
 * @retval true  如果IRQ已启用。
 * @retval false 否则。
 */
#define NRFX_IRQ_IS_ENABLED(irq_number)  _NRFX_IRQ_IS_ENABLED(irq_number)
static inline bool _NRFX_IRQ_IS_ENABLED(IRQn_Type irq_number)
{
    return 0 != (NVIC->ISER[irq_number / 32] & (1UL << (irq_number % 32)));
}

/**
 * @brief 禁用特定IRQ的宏。
 *
 * @param irq_number  IRQ编号。
 */
#define NRFX_IRQ_DISABLE(irq_number)  _NRFX_IRQ_DISABLE(irq_number)
static inline void _NRFX_IRQ_DISABLE(IRQn_Type irq_number)
{
    NVIC_DisableIRQ(irq_number);
}

/**
 * @brief 设置特定IRQ为挂起状态的宏。
 *
 * @param irq_number  IRQ编号。
 */
#define NRFX_IRQ_PENDING_SET(irq_number) _NRFX_IRQ_PENDING_SET(irq_number)
static inline void _NRFX_IRQ_PENDING_SET(IRQn_Type irq_number)
{
    NVIC_SetPendingIRQ(irq_number);
}

/**
 * @brief 清除特定IRQ挂起状态的宏。
 *
 * @param irq_number  IRQ编号。
 */
#define NRFX_IRQ_PENDING_CLEAR(irq_number) _NRFX_IRQ_PENDING_CLEAR(irq_number)
static inline void _NRFX_IRQ_PENDING_CLEAR(IRQn_Type irq_number)
{
    NVIC_ClearPendingIRQ(irq_number);
}

/**
 * @brief 检查特定IRQ是否处于挂起状态的宏。
 *
 * @retval true  如果IRQ处于挂起状态。
 * @retval false 否则。
 */
#define NRFX_IRQ_IS_PENDING(irq_number) _NRFX_IRQ_IS_PENDING(irq_number)
static inline bool _NRFX_IRQ_IS_PENDING(IRQn_Type irq_number)
{
    return (NVIC_GetPendingIRQ(irq_number) == 1);
}

#include <nordic_common.h>     // 包含与Nordic Semiconductor设备相关的常见定义和实用工具函数。
#include <app_util_platform.h> // 包含平台相关的应用程序实用工具，例如断言、临界区处理等。

/**
 * @brief 进入临界区的宏，临界区是指一段代码在任意时刻只能被一个线程或中断处理程序执行，以防止并发访问共享资源导致的数据不一致或冲突，进入临界区导致程序在执行某段代码时，暂时禁止其他中断或任务访问共享资源，以确保当前代码块可以独占地访问这些资源。
 */
#define NRFX_CRITICAL_SECTION_ENTER()   CRITICAL_REGION_ENTER()

/**
 * @brief 退出临界区的宏。
 */
#define NRFX_CRITICAL_SECTION_EXIT()    CRITICAL_REGION_EXIT()

//------------------------------------------------------------------------------

/**
 * @brief 当设置为非零值时，此宏指定 @ref nrfx_coredep_delay_us 使用基于DWT的精确延迟解决方案。
 *        如果使用的SoC中没有DWT单元，将生成编译错误。
 */
#define NRFX_DELAY_DWT_BASED 0

#include <soc/nrfx_coredep.h> // 包含与处理器核心相关的依赖性定义和实用函数。

#define NRFX_DELAY_US(us_time) nrfx_coredep_delay_us(us_time)

//------------------------------------------------------------------------------

#include <soc/nrfx_atomic.h>  // 包含用于原子操作的函数和定义，确保多线程或中断环境下的安全操作。

/**
 * @brief 原子32位无符号类型。
 */
#define nrfx_atomic_t               nrfx_atomic_u32_t

/**
 * @brief 将值存储到原子对象，并返回先前存储的值。
 *
 * @param[in] p_data  原子内存指针。
 * @param[in] value   要存储的值。
 *
 * @return 存储到原子对象中的旧值。
 */
#define NRFX_ATOMIC_FETCH_STORE(p_data, value) nrfx_atomic_u32_fetch_store(p_data, value)

/**
 * @brief 对原子对象执行逻辑OR操作，并返回先前存储的值。
 *
 * @param[in] p_data  原子内存指针。
 * @param[in] value   OR操作的第二个操作数的值。
 *
 * @return 存储到原子对象中的旧值。
 */
#define NRFX_ATOMIC_FETCH_OR(p_data, value)   nrfx_atomic_u32_fetch_or(p_data, value)

/**
 * @brief 对原子对象执行逻辑AND操作，并返回先前存储的值。
 *
 * @param[in] p_data  原子内存指针。
 * @param[in] value   AND操作的第二个操作数的值。
 *
 * @return 存储到原子对象中的旧值。
 */
#define NRFX_ATOMIC_FETCH_AND(p_data, value)   nrfx_atomic_u32_fetch_and(p_data, value)

/**
 * @brief 对原子对象执行逻辑XOR操作，并返回先前存储的值。
 *
 * @param[in] p_data  原子内存指针。
 * @param[in] value   XOR操作的第二个操作数的值。
 *
 * @return 存储到原子对象中的旧值。
 */
#define NRFX_ATOMIC_FETCH_XOR(p_data, value)   nrfx_atomic_u32_fetch_xor(p_data, value)

/**
 * @brief 对原子对象执行逻辑ADD操作，并返回先前存储的值。
 *
 * @param[in] p_data  原子内存指针。
 * @param[in] value   ADD操作的第二个操作数的值。
 *
 * @return 存储到原子对象中的旧值。
 */
#define NRFX_ATOMIC_FETCH_ADD(p_data, value)   nrfx_atomic_u32_fetch_add(p_data, value)

/**
 * @brief 对原子对象执行逻辑SUB操作，并返回先前存储的值。
 *
 * @param[in] p_data  原子内存指针。
 * @param[in] value   SUB操作的第二个操作数的值。
 *
 * @return 存储到原子对象中的旧值。
 */
#define NRFX_ATOMIC_FETCH_SUB(p_data, value)   nrfx_atomic_u32_fetch_sub(p_data, value)

//------------------------------------------------------------------------------
#ifndef NRFX_CUSTOM_ERROR_CODES

#include <sdk_errors.h> // 包含SDK中常见错误代码的定义和错误处理函数。
/**
 * @brief 当设置为非零值时，此宏指定@ref nrfx_error_codes 和 @ref ret_code_t 类型定义以自定义方式处理，
 *        而不是使用@c <nrfx_error.h>中的默认定义。
 */
#define NRFX_CUSTOM_ERROR_CODES 1

typedef ret_code_t nrfx_err_t;

#define NRFX_SUCCESS                    NRF_SUCCESS
#define NRFX_ERROR_INTERNAL             NRF_ERROR_INTERNAL
#define NRFX_ERROR_NO_MEM               NRF_ERROR_NO_MEM
#define NRFX_ERROR_NOT_SUPPORTED        NRF_ERROR_NOT_SUPPORTED
#define NRFX_ERROR_INVALID_PARAM        NRF_ERROR_INVALID_PARAM
#define NRFX_ERROR_INVALID_STATE        NRF_ERROR_INVALID_STATE
#define NRFX_ERROR_INVALID_LENGTH       NRF_ERROR_INVALID_LENGTH
#define NRFX_ERROR_TIMEOUT              NRF_ERROR_TIMEOUT
#define NRFX_ERROR_FORBIDDEN            NRF_ERROR_FORBIDDEN
#define NRFX_ERROR_NULL                 NRF_ERROR_NULL
#define NRFX_ERROR_INVALID_ADDR         NRF_ERROR_INVALID_ADDR
#define NRFX_ERROR_BUSY                 NRF_ERROR_BUSY
#define NRFX_ERROR_ALREADY_INITIALIZED  NRF_ERROR_MODULE_ALREADY_INITIALIZED

#define NRFX_ERROR_DRV_TWI_ERR_OVERRUN  NRF_ERROR_DRV_TWI_ERR_OVERRUN
#define NRFX_ERROR_DRV_TWI_ERR_ANACK    NRF_ERROR_DRV_TWI_ERR_ANACK
#define NRFX_ERROR_DRV_TWI_ERR_DNACK    NRF_ERROR_DRV_TWI_ERR_DNACK

#endif // NRFX_CUSTOM_ERROR_CODES
//------------------------------------------------------------------------------

#include <sdk_resources.h>
/**
 * @brief 定义保留用于nrfx以外用途的PPI通道的位掩码。
 */
#define NRFX_PPI_CHANNELS_USED  NRF_PPI_CHANNELS_USED

/**
 * @brief 定义保留用于nrfx以外用途的PPI组的位掩码。
 */
#define NRFX_PPI_GROUPS_USED    NRF_PPI_GROUPS_USED

/**
 * @brief 定义保留用于nrfx以外用途的SWI实例的位掩码。
 */
#define NRFX_SWI_USED           NRF_SWI_USED

/**
 * @brief 定义保留用于nrfx以外用途的TIMER实例的位掩码。
 */
#define NRFX_TIMERS_USED        NRF_TIMERS_USED

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_GLUE_H__
