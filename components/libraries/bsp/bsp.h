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
/** @file
 *
 * @defgroup bsp 板级支持包 (BSP)
 * @{
 * @ingroup app_common
 *
 * @brief BSP 模块。
 * @details 该模块提供了对开发板的抽象层。
 *          它允许用户以简单的方式通过 LED 指示某些状态。
 *          可以通过定义 BSP_SIMPLE 来修改模块功能，以简化该模块的功能，
 *          只支持启用和读取按钮的状态。
 */

#ifndef BSP_H__
#define BSP_H__

#include <stdint.h>
#include <stdbool.h>
#include "boards.h"

// 如果定义了 BSP_DEFINES_ONLY，那么只会包含与 BSP（板级支持包）定义相关的代码，而不包含任何额外的功能实现
// BSP_SIMPLE 用于减少 BSP 模块的功能。如果定义了这个宏，BSP 模块将简化为仅支持按钮状态的读取和操作，不会包含诸如 LED 状态指示等附加功能
#if !defined(BSP_DEFINES_ONLY) && !defined(BSP_SIMPLE)
#include "app_button.h"

#define BSP_BUTTON_ACTION_PUSH      (APP_BUTTON_PUSH)    /**< 代表按下按钮。参见 @ref bsp_button_action_t。 */
#define BSP_BUTTON_ACTION_RELEASE   (APP_BUTTON_RELEASE) /**< 代表释放按钮。参见 @ref bsp_button_action_t。 */
#define BSP_BUTTON_ACTION_LONG_PUSH (2)                  /**< 代表长按按钮，持续时间为 @ref BSP_LONG_PUSH_TIMEOUT_MS 毫秒。参见 @ref bsp_button_action_t。 */

#endif

/* BSP_UART_SUPPORT
 * 该定义启用了 UART 支持模块。
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t bsp_button_action_t; /**< 不同按钮可以执行的动作。 */

#define BSP_INDICATIONS_LIST {                \
        "BSP_INDICATE_IDLE",                  \
        "BSP_INDICATE_SCANNING",              \
        "BSP_INDICATE_ADVERTISING",           \
        "BSP_INDICATE_ADVERTISING_WHITELIST", \
        "BSP_INDICATE_ADVERTISING_SLOW",      \
        "BSP_INDICATE_ADVERTISING_DIRECTED",  \
        "BSP_INDICATE_BONDING",               \
        "BSP_INDICATE_CONNECTED",             \
        "BSP_INDICATE_SENT_OK",               \
        "BSP_INDICATE_SEND_ERROR",            \
        "BSP_INDICATE_RCV_OK",                \
        "BSP_INDICATE_RCV_ERROR",             \
        "BSP_INDICATE_FATAL_ERROR",           \
        "BSP_INDICATE_ALERT_0",               \
        "BSP_INDICATE_ALERT_1",               \
        "BSP_INDICATE_ALERT_2",               \
        "BSP_INDICATE_ALERT_3",               \
        "BSP_INDICATE_ALERT_OFF",             \
        "BSP_INDICATE_USER_STATE_OFF",        \
        "BSP_INDICATE_USER_STATE_0",          \
        "BSP_INDICATE_USER_STATE_1",          \
        "BSP_INDICATE_USER_STATE_2",          \
        "BSP_INDICATE_USER_STATE_3",          \
        "BSP_INDICATE_USER_STATE_ON"          \
} /**< 参见 @ref examples_bsp_states，了解这些状态如何在 PCA10028/PCA10040 开发板和 PCA10031 加密狗上指示。*/


/**@brief BSP 状态指示。
 *
 * @details 参见 @ref examples_bsp_states，了解这些状态如何在 PCA10028/PCA10040 开发板和 PCA10031 加密狗上指示。
 */
typedef enum
{
    BSP_INDICATE_FIRST = 0,
    BSP_INDICATE_IDLE  = BSP_INDICATE_FIRST, /**< 参见 \ref BSP_INDICATE_IDLE。*/
    BSP_INDICATE_SCANNING,                   /**< 参见 \ref BSP_INDICATE_SCANNING。*/
    BSP_INDICATE_ADVERTISING,                /**< 参见 \ref BSP_INDICATE_ADVERTISING。*/
    BSP_INDICATE_ADVERTISING_WHITELIST,      /**< 参见 \ref BSP_INDICATE_ADVERTISING_WHITELIST。*/
    BSP_INDICATE_ADVERTISING_SLOW,           /**< 参见 \ref BSP_INDICATE_ADVERTISING_SLOW。*/
    BSP_INDICATE_ADVERTISING_DIRECTED,       /**< 参见 \ref BSP_INDICATE_ADVERTISING_DIRECTED。*/
    BSP_INDICATE_BONDING,                    /**< 参见 \ref BSP_INDICATE_BONDING。*/
    BSP_INDICATE_CONNECTED,                  /**< 参见 \ref BSP_INDICATE_CONNECTED。*/
    BSP_INDICATE_SENT_OK,                    /**< 参见 \ref BSP_INDICATE_SENT_OK。*/
    BSP_INDICATE_SEND_ERROR,                 /**< 参见 \ref BSP_INDICATE_SEND_ERROR。*/
    BSP_INDICATE_RCV_OK,                     /**< 参见 \ref BSP_INDICATE_RCV_OK。*/
    BSP_INDICATE_RCV_ERROR,                  /**< 参见 \ref BSP_INDICATE_RCV_ERROR。*/
    BSP_INDICATE_FATAL_ERROR,                /**< 参见 \ref BSP_INDICATE_FATAL_ERROR。*/
    BSP_INDICATE_ALERT_0,                    /**< 参见 \ref BSP_INDICATE_ALERT_0。*/
    BSP_INDICATE_ALERT_1,                    /**< 参见 \ref BSP_INDICATE_ALERT_1。*/
    BSP_INDICATE_ALERT_2,                    /**< 参见 \ref BSP_INDICATE_ALERT_2。*/
    BSP_INDICATE_ALERT_3,                    /**< 参见 \ref BSP_INDICATE_ALERT_3。*/
    BSP_INDICATE_ALERT_OFF,                  /**< 参见 \ref BSP_INDICATE_ALERT_OFF。*/
    BSP_INDICATE_USER_STATE_OFF,             /**< 参见 \ref BSP_INDICATE_USER_STATE_OFF。*/
    BSP_INDICATE_USER_STATE_0,               /**< 参见 \ref BSP_INDICATE_USER_STATE_0。*/
    BSP_INDICATE_USER_STATE_1,               /**< 参见 \ref BSP_INDICATE_USER_STATE_1。*/
    BSP_INDICATE_USER_STATE_2,               /**< 参见 \ref BSP_INDICATE_USER_STATE_2。*/
    BSP_INDICATE_USER_STATE_3,               /**< 参见 \ref BSP_INDICATE_USER_STATE_3。*/
    BSP_INDICATE_USER_STATE_ON,              /**< 参见 \ref BSP_INDICATE_USER_STATE_ON。*/
    BSP_INDICATE_LAST = BSP_INDICATE_USER_STATE_ON
} bsp_indication_t;

/**@brief BSP 事件。
 *
 * @note BSP_EVENT_KEY_0 到 BSP_EVENT_KEY_LAST 的事件默认分配给按钮。
 */
typedef enum
{
    BSP_EVENT_NOTHING = 0,                  /**< 将此事件分配给操作，以防止生成事件（禁用操作）。*/
    BSP_EVENT_DEFAULT,                      /**< 将此事件分配给操作，以分配默认事件给操作。*/
    BSP_EVENT_CLEAR_BONDING_DATA,           /**< 持久的绑定数据应该被清除。*/
    BSP_EVENT_CLEAR_ALERT,                  /**< 应该清除警报。*/
    BSP_EVENT_DISCONNECT,                   /**< 应该断开连接。*/
    BSP_EVENT_ADVERTISING_START,            /**< 设备应该开始广播。*/
    BSP_EVENT_ADVERTISING_STOP,             /**< 设备应该停止广播。*/
    BSP_EVENT_WHITELIST_OFF,                /**< 设备应该移除其广播白名单。*/
    BSP_EVENT_BOND,                         /**< 设备应该与当前连接的对等设备绑定。*/
    BSP_EVENT_RESET,                        /**< 设备应该复位。*/
    BSP_EVENT_SLEEP,                        /**< 设备应该进入睡眠模式。*/
    BSP_EVENT_WAKEUP,                       /**< 设备应该从睡眠模式唤醒。*/
    BSP_EVENT_SYSOFF,                       /**< 设备应该进入系统关闭模式（没有唤醒）。*/
    BSP_EVENT_DFU,                          /**< 设备应该进入 DFU 模式。*/
    BSP_EVENT_KEY_0,                        /**< BSP_BUTTON_0 按钮推送动作的默认事件（仅当该按钮存在时）。*/
    BSP_EVENT_KEY_1,                        /**< BSP_BUTTON_1 按钮推送动作的默认事件（仅当该按钮存在时）。*/
    BSP_EVENT_KEY_2,                        /**< BSP_BUTTON_2 按钮推送动作的默认事件（仅当该按钮存在时）。*/
    BSP_EVENT_KEY_3,                        /**< BSP_BUTTON_3 按钮推送动作的默认事件（仅当该按钮存在时）。*/
    BSP_EVENT_KEY_4,                        /**< BSP_BUTTON_4 按钮推送动作的默认事件（仅当该按钮存在时）。*/
    BSP_EVENT_KEY_5,                        /**< BSP_BUTTON_5 按钮推送动作的默认事件（仅当该按钮存在时）。*/
    BSP_EVENT_KEY_6,                        /**< BSP_BUTTON_6 按钮推送动作的默认事件（仅当该按钮存在时）。*/
    BSP_EVENT_KEY_7,                        /**< BSP_BUTTON_7 按钮推送动作的默认事件（仅当该按钮存在时）。*/
    BSP_EVENT_KEY_LAST = BSP_EVENT_KEY_7,
} bsp_event_t;


typedef struct
{
    bsp_event_t push_event;      /**< 按钮常规按下动作时触发的事件。 */
    bsp_event_t long_push_event; /**< 按钮长按动作时触发的事件。 */
    bsp_event_t release_event;   /**< 按钮释放时触发的事件。 */
} bsp_button_event_cfg_t;

/**@brief BSP 模块事件回调函数类型。
 *
 * @details 当 BSP 模块中发生事件时，将调用该回调函数来通知应用程序。
 *
 * @param[in]   bsp_event_t BSP 事件类型。
 */
typedef void (* bsp_event_callback_t)(bsp_event_t);


/**@brief       初始化 BSP 的函数。
 *
 * @details     该函数初始化板级支持包，以允许状态指示和按钮反应。默认事件会被分配给按钮。
 * @note        在调用此函数之前，必须初始化以下所需模块：
 *              - @ref app_timer 用于 LED 支持
 *              - @ref app_gpiote 用于按钮支持
 *
 * @param[in]   type               使用的外设类型。
 * @param[in]   callback           在检测到按钮按下/事件时调用的函数。
 *
 * @retval      NRF_SUCCESS               如果 BSP 模块成功初始化。
 * @retval      NRF_ERROR_INVALID_STATE   如果应用程序计时器模块未初始化。
 * @retval      NRF_ERROR_NO_MEM          如果已达到最大计时器数量。
 * @retval      NRF_ERROR_INVALID_PARAM   如果 GPIOTE 用户过多。
 * @retval      NRF_ERROR_INVALID_STATE   如果按钮或 GPIOTE 未初始化。
 */
uint32_t bsp_init(uint32_t type, bsp_event_callback_t callback);


/**@brief       检查按钮状态的函数。
 *
 * @details     此函数检查按钮是否被按下。如果按钮 ID 超出范围，函数返回 false。
 *
 * @param[in]   button                  要检查的按钮 ID。
 *
 * @retval      true                    如果按钮被按下。
 * @retval      false                   如果按钮没有被按下。
 */
bool bsp_button_is_pressed(uint32_t button);


/**@brief       为按钮分配特定事件的函数。
 *
 * @details     此函数允许重新定义分配给按钮的标准事件。要取消分配事件，请提供事件 @ref BSP_EVENT_NOTHING。
 *
 * @param[in]   button                   要重新定义的按钮 ID。
 * @param[in]   action                   要分配事件的按钮动作。
 * @param[in]   event                    要分配给按钮的事件。
 *
 * @retval      NRF_SUCCESS              如果事件成功分配给按钮。
 * @retval      NRF_ERROR_INVALID_PARAM  如果按钮 ID 或按钮动作无效。
 */
uint32_t bsp_event_to_button_action_assign(uint32_t button, bsp_button_action_t action, bsp_event_t event);


/**@brief       配置指示器到所需状态的函数。
 *
 * @details     此函数通过 LED（如果启用）指示所需的状态。
 *
 * @note        警报是独立指示的。
 *
 * @param[in]   indicate   要指示的状态。
 *
 * @retval      NRF_SUCCESS               如果状态成功指示。
 * @retval      NRF_ERROR_NO_MEM          如果内部计时器操作队列已满。
 * @retval      NRF_ERROR_INVALID_STATE   如果应用程序计时器模块未初始化，或内部计时器未创建。
 */
uint32_t bsp_indication_set(bsp_indication_t indicate);


/**@brief       启用所有按钮的功能。
 *
 * @details     调用此函数后，所有按钮在按下时会生成事件，并且所有按钮都可以唤醒系统。
 *
 * @retval      NRF_SUCCESS              如果按钮成功启用。
 * @retval      NRF_ERROR_NOT_SUPPORTED  如果板上没有按钮或定义了 BSP_SIMPLE。
 * @return      传递的错误代码。
 */
uint32_t bsp_buttons_enable(void);


/**@brief       禁用所有按钮的功能。
 *
 * @details     调用此函数后，按下按钮不会生成事件，并且按钮无法唤醒系统。
 *
 * @retval      NRF_SUCCESS              如果按钮成功禁用。
 * @retval      NRF_ERROR_NOT_SUPPORTED  如果板上没有按钮或定义了 BSP_SIMPLE。
 * @return      传递的错误代码。
 */
uint32_t bsp_buttons_disable(void);


/**@brief       启用给定按钮的 SYSTEM OFF 唤醒功能。
 *
 * @details     调用此函数后，可以使用按钮唤醒芯片。此函数应在进入睡眠前立即调用。
 *
 * @param[in]   button_idx  按钮的索引。
 *
 * @retval      NRF_SUCCESS              如果按钮成功启用。
 * @retval      NRF_ERROR_NOT_SUPPORTED  如果板上没有按钮或定义了 BSP_SIMPLE。
 */
uint32_t bsp_wakeup_button_enable(uint32_t button_idx);


/**@brief       禁用给定按钮的唤醒功能。
 *
 * @param[in]   button_idx  按钮的索引。
 *
 * @retval      NRF_SUCCESS              如果按钮成功禁用。
 * @retval      NRF_ERROR_NOT_SUPPORTED  如果板上没有按钮或定义了 BSP_SIMPLE。
 */
uint32_t bsp_wakeup_button_disable(uint32_t button_idx);


#ifdef __cplusplus
}
#endif

#endif // BSP_H__

/** @} */
