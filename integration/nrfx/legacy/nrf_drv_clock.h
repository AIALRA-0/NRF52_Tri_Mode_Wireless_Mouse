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
#ifndef NRF_DRV_CLOCK_H__
#define NRF_DRV_CLOCK_H__

// 引入时钟驱动的头文件
#include <nrfx_clock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_drv_clock Clock driver - legacy layer
 * @{
 * @ingroup nrf_clock
 *
 * @brief 该模块为兼容旧的API提供了支持。它基于 `nrfx_clock` 实现。
 */

/**
 * @brief 时钟事件类型。
 *
 * 这些枚举值表示系统中与时钟相关的不同事件。
 */
typedef enum
{
    NRF_DRV_CLOCK_EVT_HFCLK_STARTED, ///< 高频时钟已启动。
    NRF_DRV_CLOCK_EVT_LFCLK_STARTED, ///< 低频时钟已启动。
    NRF_DRV_CLOCK_EVT_CAL_DONE,      ///< 时钟校准完成。
    NRF_DRV_CLOCK_EVT_CAL_ABORTED,   ///< 时钟校准被中止。
} nrf_drv_clock_evt_type_t;

/**
 * @brief 时钟事件处理函数类型。
 *
 * 当时钟相关事件发生时，会调用此处理函数。
 *
 * @param[in] event 触发的时钟事件。
 */
typedef void (*nrf_drv_clock_event_handler_t)(nrf_drv_clock_evt_type_t event);

// 前向声明 `nrf_drv_clock_handler_item_t` 类型，用于表示事件处理程序结构体。
typedef struct nrf_drv_clock_handler_item_s nrf_drv_clock_handler_item_t;

/**
 * @brief 时钟事件处理程序项的结构体定义。
 *
 * 包含指向下一个事件处理程序的指针和当前事件处理程序的指针。
 */
struct nrf_drv_clock_handler_item_s
{
    nrf_drv_clock_handler_item_t * p_next;        ///< 指向下一个时钟处理程序的指针。
    nrf_drv_clock_event_handler_t  event_handler; ///< 当时钟启动时要调用的处理程序函数。
};

/**
 * @brief 检查时钟驱动是否已初始化。
 *
 * @retval true  时钟驱动已初始化。
 * @retval false 时钟驱动未初始化。
 */
bool nrf_drv_clock_init_check(void);

/**
 * @brief 初始化时钟驱动模块。
 *
 * 初始化后，模块处于关闭状态（时钟尚未请求）。
 *
 * @retval NRF_SUCCESS                          初始化成功。
 * @retval NRF_ERROR_MODULE_ALREADY_INITIALIZED 驱动已初始化。
 */
ret_code_t nrf_drv_clock_init(void);

/**
 * @brief 反初始化时钟模块。
 *
 * 该函数用于关闭和清理时钟驱动模块。
 */
void nrf_drv_clock_uninit(void);

/**
 * @brief 请求启动低频时钟 (LFCLK)。
 *
 * 不同模块或上下文可以请求低频时钟。驱动程序确保只有在第一次请求时才启动时钟。
 * 如果时钟尚未准备好，提供的处理程序会被添加到处理程序列表中，当时钟启动后将被通知。
 *
 * 如果已经启动低频时钟，用户的回调函数将立即从当前上下文调用。
 * 
 * @note 当SoftDevice启用时，低频时钟一直运行，驱动程序无法控制它。
 *
 * @param[in] p_handler_item 指向事件处理程序结构的指针。
 */
void nrf_drv_clock_lfclk_request(nrf_drv_clock_handler_item_t * p_handler_item);

/**
 * @brief 释放低频时钟 (LFCLK)。
 *
 * 如果没有其他模块请求时钟，LFCLK将被停止。
 *
 * @note 当SoftDevice启用时，低频时钟始终运行。
 */
void nrf_drv_clock_lfclk_release(void);

/**
 * @brief 检查低频时钟是否正在运行。
 *
 * @retval true  低频时钟正在运行。
 * @retval false 低频时钟未运行。
 */
bool nrf_drv_clock_lfclk_is_running(void);

/**
 * @brief 请求启动高精度时钟源 (HFCLK)。
 *
 * 不同模块或上下文可以请求高精度时钟。驱动程序确保只有在第一次请求时才启动时钟。
 * 
 * @note 如果SoftDevice正在运行，时钟由SoftDevice管理，所有请求由SoftDevice处理。
 *
 * @param[in] p_handler_item 指向事件处理程序结构的指针。
 */
void nrf_drv_clock_hfclk_request(nrf_drv_clock_handler_item_t * p_handler_item);

/**
 * @brief 释放高精度时钟源 (HFCLK)。
 *
 * 如果没有其他模块请求时钟，高精度时钟将被停止。
 */
void nrf_drv_clock_hfclk_release(void);

/**
 * @brief 检查高频时钟 (HFCLK) 是否正在运行。
 *
 * @retval true  高频时钟正在运行。
 * @retval false 高频时钟未运行。
 */
bool nrf_drv_clock_hfclk_is_running(void);

/**
 * @brief 启动一次时钟校准过程。
 *
 * 校准过程分为三个阶段：延迟（可选）、请求HFCLK和硬件支持的校准。
 *
 * @param[in] delay   校准开始的延迟时间（以0.25秒为单位）。
 * @param[in] handler 校准完成或中止时的回调函数。
 *
 * @retval NRF_SUCCESS             操作成功。
 * @retval NRF_ERROR_FORBIDDEN     如果SoftDevice存在或选择的LFCLK源不是RC振荡器。
 * @retval NRF_ERROR_INVALID_STATE 低频时钟关闭时。
 * @retval NRF_ERROR_BUSY          校准正在进行中。
 */
ret_code_t nrf_drv_clock_calibration_start(uint8_t delay, nrf_drv_clock_event_handler_t handler);

/**
 * @brief 中止正在进行的校准。
 *
 * 如果提供了处理程序，它将在校准中止完成后调用。
 *
 * @retval NRF_SUCCESS             操作成功。
 * @retval NRF_ERROR_FORBIDDEN     如果SoftDevice存在或选择的LFCLK源不是RC振荡器。
 */
ret_code_t nrf_drv_clock_calibration_abort(void);

/**
 * @brief 检查校准是否正在进行中。
 *
 * @param[out] p_is_calibrating 如果校准正在进行，则设置为true。
 *
 * @retval NRF_SUCCESS             操作成功。
 * @retval NRF_ERROR_FORBIDDEN     如果SoftDevice存在或选择的LFCLK源不是RC振荡器。
 */
ret_code_t nrf_drv_clock_is_calibrating(bool * p_is_calibrating);

/**
 * @brief 返回与时钟驱动模块相关的PPI任务地址。
 *
 * @param[in] task 时钟外设任务。
 *
 * @return 任务地址。
 */
__STATIC_INLINE uint32_t nrf_drv_clock_ppi_task_addr(nrf_clock_task_t task);

/**
 * @brief 返回与时钟驱动模块相关的PPI事件地址。
 *
 * @param[in] event 时钟外设事件。
 *
 * @return 事件地址。
 */
__STATIC_INLINE uint32_t nrf_drv_clock_ppi_event_addr(nrf_clock_event_t event);

#ifdef SOFTDEVICE_PRESENT
/**
 * @brief SoftDevice处理程序接收到 @ref NRF_SOC_EVTS 事件时调用此函数。
 *
 * @param[in] evt_id NRF_SOC_EVTS中的一个值。
 */
void nrf_drv_clock_on_soc_event(uint32_t evt_id);

/**
 * @brief SoftDevice启用后调用此函数。
 *
 * 该函数主要用于标记SoftDevice已请求LFCLK。
 */
void nrf_drv_clock_on_sd_enable(void);

/**
 * @brief SoftDevice禁用后调用此函数。
 *
 * 此函数用于：
 * 1. 释放SoftDevice的LFCLK资源。
 * 2. 在SoftDevice释放POWER_CLOCK_IRQ后重新初始化中断。
 */
void nrf_drv_clock_on_sd_disable(void);

#endif

#ifndef SUPPRESS_INLINE_IMPLEMENTATION
__STATIC_INLINE uint32_t nrf_drv_clock_ppi_task_addr(nrf_clock_task_t task)
{
    return nrf_clock_task_address_get(task); // 返回指定时钟任务的地址
}

__STATIC_INLINE uint32_t nrf_drv_clock_ppi_event_addr(nrf_clock_event_t event)
{
    return nrf_clock_event_address_get(event); // 返回指定时钟事件的地址
}
#endif // SUPPRESS_INLINE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // NRF_DRV_CLOCK_H__
