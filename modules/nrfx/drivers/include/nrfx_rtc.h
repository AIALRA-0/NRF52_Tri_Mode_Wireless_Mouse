/**
 * Copyright (c) 2014 - 2021, Nordic Semiconductor ASA
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

#ifndef NRFX_RTC_H__
#define NRFX_RTC_H__

#include <nrfx.h>
#include <hal/nrf_rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_rtc RTC 驱动
 * @{
 * @ingroup nrf_rtc
 * @brief   实时时钟（RTC）外设驱动。
 */

/** @brief 将微秒转换为计时器刻度的宏。 */
#define NRFX_RTC_US_TO_TICKS(us,freq) (((us) * (freq)) / 1000000U)

/** @brief RTC 驱动中断类型。 */
typedef enum
{
    NRFX_RTC_INT_COMPARE0 = 0, /**< 来自 COMPARE0 事件的中断。 */
    NRFX_RTC_INT_COMPARE1 = 1, /**< 来自 COMPARE1 事件的中断。 */
    NRFX_RTC_INT_COMPARE2 = 2, /**< 来自 COMPARE2 事件的中断。 */
    NRFX_RTC_INT_COMPARE3 = 3, /**< 来自 COMPARE3 事件的中断。 */
    NRFX_RTC_INT_TICK     = 4, /**< 来自 TICK 事件的中断。 */
    NRFX_RTC_INT_OVERFLOW = 5  /**< 来自溢出（OVERFLOW）事件的中断。 */
} nrfx_rtc_int_type_t;

/** @brief RTC 驱动实例结构体。 */
typedef struct
{
    NRF_RTC_Type  * p_reg;            /**< 实例寄存器集的指针。 */
    IRQn_Type       irq;              /**< 实例 IRQ ID。 */
    uint8_t         instance_id;      /**< 驱动实例的索引，仅供内部使用。 */
    uint8_t         cc_channel_count; /**< 捕获/比较通道数量。 */
} nrfx_rtc_t;

/** @brief 创建 RTC 驱动实例的宏。 */
#define NRFX_RTC_INSTANCE(id)                                   \
{                                                               \
    .p_reg            = NRFX_CONCAT_2(NRF_RTC, id),             \
    .irq              = NRFX_CONCAT_3(RTC, id, _IRQn),          \
    .instance_id      = NRFX_CONCAT_3(NRFX_RTC, id, _INST_IDX), \
    .cc_channel_count = NRF_RTC_CC_CHANNEL_COUNT(id),           \
}

#ifndef __NRFX_DOXYGEN__
enum {
#if NRFX_CHECK(NRFX_RTC0_ENABLED)
    NRFX_RTC0_INST_IDX,
#endif
#if NRFX_CHECK(NRFX_RTC1_ENABLED)
    NRFX_RTC1_INST_IDX,
#endif
#if NRFX_CHECK(NRFX_RTC2_ENABLED)
    NRFX_RTC2_INST_IDX,
#endif
    NRFX_RTC_ENABLED_COUNT
};
#endif

/** @brief RTC 驱动实例配置结构体。 */
typedef struct
{
    uint16_t prescaler;          /**< 预分频器。 */
    uint8_t  interrupt_priority; /**< 中断优先级。 */
    uint8_t  tick_latency;       /**< 中断处理器的最大时长（以刻度为单位，最大为 7.7 ms）。 */
    bool     reliable;           /**< 可靠模式标志。 */
} nrfx_rtc_config_t;

/** @brief RTC 实例默认配置。 */
#define NRFX_RTC_DEFAULT_CONFIG                                                     \
{                                                                                   \
    .prescaler          = RTC_FREQ_TO_PRESCALER(NRFX_RTC_DEFAULT_CONFIG_FREQUENCY), \
    .interrupt_priority = NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY,                     \
    .tick_latency       = NRFX_RTC_US_TO_TICKS(NRFX_RTC_MAXIMUM_LATENCY_US,         \
                                               NRFX_RTC_DEFAULT_CONFIG_FREQUENCY),  \
    .reliable           = NRFX_RTC_DEFAULT_CONFIG_RELIABLE,                         \
}

/** @brief RTC 驱动实例处理程序类型。 */
typedef void (*nrfx_rtc_handler_t)(nrfx_rtc_int_type_t int_type);

/**
 * @brief 初始化 RTC 驱动实例的函数。
 *
 * 初始化后，实例处于关闭状态。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 * @param[in] p_config   初始配置结构体的指针。
 * @param[in] handler    用户提供的事件处理程序。
 *                       不能为 NULL。
 *
 * @retval NRFX_SUCCESS             初始化成功。
 * @retval NRFX_ERROR_INVALID_STATE 实例已被初始化。
 */
nrfx_err_t nrfx_rtc_init(nrfx_rtc_t const * const  p_instance,
                         nrfx_rtc_config_t const * p_config,
                         nrfx_rtc_handler_t        handler);

/**
 * @brief 取消初始化 RTC 驱动实例的函数。
 *
 * 取消初始化后，实例处于空闲状态。硬件应恢复到初始化之前的状态。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 */
void nrfx_rtc_uninit(nrfx_rtc_t const * const p_instance);

/**
 * @brief 启用 RTC 驱动实例的函数。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 */
void nrfx_rtc_enable(nrfx_rtc_t const * const p_instance);

/**
 * @brief 禁用 RTC 驱动实例的函数。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 */
void nrfx_rtc_disable(nrfx_rtc_t const * const p_instance);

/**
 * @brief 设置比较通道的函数。
 *
 * 如果实例处于关闭状态，该函数将启动实例。
 *
 * 驱动在配置 RTC 时不进入临界区，这意味着它可以被抢占一段时间。
 * 如果驱动被抢占，且设置的值在短时间内，驱动可能会设置一个已经过时的比较值。
 * 如果为指定实例启用了可靠模式，将处理该风险。
 * 然而，为了检测请求的值是否已经过时，该模式做出以下假设：
 *  - 最大的抢占时间（以刻度为单位，8 位值）已知且小于 7.7 毫秒（当 prescaler = 0 且 RTC 频率为 32 kHz 时）。
 *  - 请求的绝对比较值不大于 (0x00FFFFFF)-tick_latency。用户负责确保这一点。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 * @param[in] channel    实例的一个通道。
 * @param[in] val        要设置的比较寄存器的绝对值。
 * @param[in] enable_irq True 表示启用中断，False 表示禁用中断。
 *
 * @retval NRFX_SUCCESS       操作成功。
 * @retval NRFX_ERROR_TIMEOUT 比较未设置，因为请求的值比当前计数器值小。只有在启用了可靠模式时，才可能报告此错误。
 */
nrfx_err_t nrfx_rtc_cc_set(nrfx_rtc_t const * const p_instance,
                           uint32_t                 channel,
                           uint32_t                 val,
                           bool                     enable_irq);

/**
 * @brief 禁用通道的函数。
 *
 * 此函数禁用通道事件和通道中断。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 * @param[in] channel    实例的一个通道。
 *
 * @retval NRFX_SUCCESS       操作成功。
 * @retval NRFX_ERROR_TIMEOUT 请求的通道上有挂起的中断。
 */
nrfx_err_t nrfx_rtc_cc_disable(nrfx_rtc_t const * const p_instance, uint32_t channel);

/**
 * @brief 启用 TICK 事件的函数。
 *
 * 此函数启用 tick 事件并可选择启用中断。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 * @param[in] enable_irq True 表示启用中断，False 表示禁用中断。
 */
void nrfx_rtc_tick_enable(nrfx_rtc_t const * const p_instance, bool enable_irq);

/**
 * @brief 禁用 TICK 事件的函数。
 *
 * 此函数禁用 TICK 事件和中断。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 */
void nrfx_rtc_tick_disable(nrfx_rtc_t const * const p_instance);

/**
 * @brief 启用溢出的函数。
 *
 * 此函数启用溢出事件并可选择启用中断。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 * @param[in] enable_irq True 表示启用中断，False 表示禁用中断。
 */
void nrfx_rtc_overflow_enable(nrfx_rtc_t const * const p_instance, bool enable_irq);

/**
 * @brief 禁用溢出的函数。
 *
 * 此函数禁用溢出事件和中断。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 */
void nrfx_rtc_overflow_disable(nrfx_rtc_t const * const p_instance);

/**
 * @brief 获取可以在比较通道中设置的最大相对刻度值的函数。
 *
 * 当使用了占用高优先级中断的堆栈（例如 SoftDevice）时，应用程序代码可能会随时被中断一段时间。
 * 如果启用了可靠模式，考虑了最大延迟，返回值会小于 RTC 计数器的分辨率。
 * 如果未启用可靠模式，返回值等于计数器分辨率。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 *
 * @return 最大的刻度值。
 */
uint32_t nrfx_rtc_max_ticks_get(nrfx_rtc_t const * const p_instance);

/**
 * @brief 禁用所有实例中断的函数。
  *
 * @param[in] p_instance 驱动实例结构体的指针。
 * @param[in] p_mask     指向保存掩码的位置的指针。
 */
__STATIC_INLINE void nrfx_rtc_int_disable(nrfx_rtc_t const * const p_instance,
                                          uint32_t               * p_mask);

/**
 * @brief 启用实例中断的函数。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 * @param[in] mask       要启用的中断掩码。
 */
__STATIC_INLINE void nrfx_rtc_int_enable(nrfx_rtc_t const * const p_instance, uint32_t mask);

/**
 * @brief 获取当前计数器值的函数。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 *
 * @return 计数器值。
 */
__STATIC_INLINE uint32_t nrfx_rtc_counter_get(nrfx_rtc_t const * const p_instance);

/**
 * @brief 清除计数器值的函数。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 */
__STATIC_INLINE void nrfx_rtc_counter_clear(nrfx_rtc_t const * const p_instance);

/**
 * @brief 返回 RTC 驱动实例的请求任务地址的函数。
 *
 * 任务地址可以由 PPI 模块使用。
 *
 * @param[in] p_instance 实例的指针。
 * @param[in] task       外设任务之一。
 *
 * @return 任务寄存器的地址。
 */
__STATIC_INLINE uint32_t nrfx_rtc_task_address_get(nrfx_rtc_t const * const p_instance,
                                                   nrf_rtc_task_t           task);

/**
 * @brief 返回 RTC 驱动实例的请求事件地址的函数。
 *
 * 事件地址可以由 PPI 模块使用。
 *
 * @param[in] p_instance 驱动实例结构体的指针。
 * @param[in] event      外设事件之一。
 *
 * @return 事件寄存器的地址。
 */
__STATIC_INLINE uint32_t nrfx_rtc_event_address_get(nrfx_rtc_t const * const p_instance,
                                                    nrf_rtc_event_t          event);

#ifndef SUPPRESS_INLINE_IMPLEMENTATION

/**
 * @brief 禁用 RTC 实例的中断。
 *
 * 此函数将禁用实例的 TICK、溢出以及所有比较通道的中断。
 *
 * @param[in] p_instance RTC 驱动实例的指针。
 * @param[out] p_mask    用于保存当前启用中断的掩码。
 */
__STATIC_INLINE void nrfx_rtc_int_disable(nrfx_rtc_t const * const p_instance,
                                          uint32_t               * p_mask)
{
    *p_mask = nrf_rtc_int_get(p_instance->p_reg);
    nrf_rtc_int_disable(p_instance->p_reg, NRF_RTC_INT_TICK_MASK |
                                           NRF_RTC_INT_OVERFLOW_MASK |
                                           NRF_RTC_INT_COMPARE0_MASK |
                                           NRF_RTC_INT_COMPARE1_MASK |
                                           NRF_RTC_INT_COMPARE2_MASK |
                                           NRF_RTC_INT_COMPARE3_MASK);
}

/**
 * @brief 启用 RTC 实例的中断。
 *
 * 此函数根据传入的掩码启用相应的中断。
 *
 * @param[in] p_instance RTC 驱动实例的指针。
 * @param[in] mask       要启用的中断的掩码。
 */
__STATIC_INLINE void nrfx_rtc_int_enable(nrfx_rtc_t const * const p_instance, uint32_t mask)
{
    nrf_rtc_int_enable(p_instance->p_reg, mask);
}

/**
 * @brief 获取 RTC 实例的当前计数器值。
 *
 * 此函数返回当前 RTC 计数器的值。
 *
 * @param[in] p_instance RTC 驱动实例的指针。
 *
 * @return 当前计数器的值。
 */
__STATIC_INLINE uint32_t nrfx_rtc_counter_get(nrfx_rtc_t const * const p_instance)
{
    return nrf_rtc_counter_get(p_instance->p_reg);
}

/**
 * @brief 清除 RTC 实例的计数器值。
 *
 * 此函数将触发 RTC 任务来清除计数器，将计数器复位为 0。
 *
 * @param[in] p_instance RTC 驱动实例的指针。
 */
__STATIC_INLINE void nrfx_rtc_counter_clear(nrfx_rtc_t const * const p_instance)
{
    nrf_rtc_task_trigger(p_instance->p_reg, NRF_RTC_TASK_CLEAR);
}

/**
 * @brief 获取 RTC 实例的任务地址。
 *
 * 此函数返回 RTC 实例中指定任务的寄存器地址，可用于 PPI 模块。
 *
 * @param[in] p_instance RTC 驱动实例的指针。
 * @param[in] task       RTC 外设任务之一。
 *
 * @return 指定任务的寄存器地址。
 */
__STATIC_INLINE uint32_t nrfx_rtc_task_address_get(nrfx_rtc_t const * const p_instance,
                                                   nrf_rtc_task_t           task)
{
    return nrf_rtc_task_address_get(p_instance->p_reg, task);
}

/**
 * @brief 获取 RTC 实例的事件地址。
 *
 * 此函数返回 RTC 实例中指定事件的寄存器地址，可用于 PPI 模块。
 *
 * @param[in] p_instance RTC 驱动实例的指针。
 * @param[in] event      RTC 外设事件之一。
 *
 * @return 指定事件的寄存器地址。
 */
__STATIC_INLINE uint32_t nrfx_rtc_event_address_get(nrfx_rtc_t const * const p_instance,
                                                    nrf_rtc_event_t          event)
{
    return nrf_rtc_event_address_get(p_instance->p_reg, event);
}

#endif // SUPPRESS_INLINE_IMPLEMENTATION

/** @} */

/**
 * @brief RTC 0 中断处理函数。
 *
 * 处理与 RTC 0 实例相关的中断。
 */
void nrfx_rtc_0_irq_handler(void);

/**
 * @brief RTC 1 中断处理函数。
 *
 * 处理与 RTC 1 实例相关的中断。
 */
void nrfx_rtc_1_irq_handler(void);

/**
 * @brief RTC 2 中断处理函数。
 *
 * 处理与 RTC 2 实例相关的中断。
 */
void nrfx_rtc_2_irq_handler(void);



#ifdef __cplusplus
}
#endif

#endif // NRFX_RTC_H__
