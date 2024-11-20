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
/**
 * @file
 * @defgroup app_usbd_hid_mouse USB HID鼠标类
 * @{
 * @ingroup app_usbd_hid
 * @brief 提供USB HID鼠标类的类型、定义和API。
 *
 * @details 该模块实现了HID鼠标类的初始化、控制、以及与USB通信的基本接口。
 */

#ifndef APP_USBD_HID_MOUSE_H__
#define APP_USBD_HID_MOUSE_H__

#include <stdint.h>   // 标准整数类型
#include <stdbool.h>  // 标准布尔类型

#include "nrf_drv_usbd.h"             // USB驱动库
#include "app_usbd_class_base.h"      // 基本的USBD类定义
#include "app_usbd_hid_types.h"       // HID类型定义
#include "app_usbd_hid.h"             // HID基础接口
#include "app_usbd.h"                 // USB设备核心接口
#include "app_usbd_core.h"            // USB核心实现
#include "app_usbd_descriptor.h"      // USB描述符
#include "app_usbd_hid_mouse_desc.h"  // HID鼠标描述符
#include "app_usbd_hid_mouse_internal.h"  // HID鼠标内部定义

#ifdef DOXYGEN
/**
 * @brief HID鼠标类实例类型。
 *
 * @ref APP_USBD_CLASS_TYPEDEF 宏定义了此类型。
 */
typedef struct { } app_usbd_hid_mouse_t;
#else
/*lint -save -e10 -e26 -e123 -e505 */
APP_USBD_CLASS_TYPEDEF(app_usbd_hid_mouse,                      \
            APP_USBD_HID_MOUSE_CONFIG(0, NRF_DRV_USBD_EPIN1),   \
            APP_USBD_HID_MOUSE_INSTANCE_SPECIFIC_DEC,           \
            APP_USBD_HID_MOUSE_DATA_SPECIFIC_DEC                \
);
/*lint -restore*/
#endif

/**
 * @brief HID鼠标类实例的全局定义宏
 *
 * @param instance_name     实例名称
 * @param interface_number  接口编号
 * @param endpoint          输入端点（@ref nrf_drv_usbd_ep_t类型）
 * @param bcnt              鼠标按键数（1到8）
 * @param user_ev_handler   用户事件处理函数（可选）
 * @param subclass_boot     子类启动模式（@ref app_usbd_hid_subclass_t类型）
 *
 * @code
 * APP_USBD_HID_MOUSE_GLOBAL_DEF(my_awesome_mouse, 0, NRF_DRV_USBD_EPIN1, 3, NULL);
 * @endcode
 */
#define APP_USBD_HID_MOUSE_GLOBAL_DEF(instance_name,                                                    \
                                      interface_number,                                                 \
                                      endpoint,                                                         \
                                      bcnt,                                                             \
                                      user_ev_handler,                                                  \
                                      subclass_boot)                                                    \
    APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(mouse_desc, APP_USBD_HID_MOUSE_REPORT_DSC_BUTTON(bcnt));  \
    static const app_usbd_hid_subclass_desc_t * mouse_descs[] = {&mouse_desc};                          \
    APP_USBD_HID_MOUSE_GLOBAL_DEF_INTERNAL(instance_name,                                               \
                                           interface_number,                                            \
                                           endpoint,                                                    \
                                           bcnt,                                                        \
                                           user_ev_handler,                                             \
                                           subclass_boot)

/**
 * @brief 从HID鼠标内部获取类实例的帮助函数。
 *
 * @param[in] p_mouse HID鼠标实例（由@ref APP_USBD_HID_MOUSE_GLOBAL_DEF定义）。
 *
 * @return 基类实例。
 */
static inline app_usbd_class_inst_t const *
app_usbd_hid_mouse_class_inst_get(app_usbd_hid_mouse_t const * p_mouse)
{
    return &p_mouse->base;
}

/**
 * @brief 从基类实例获取HID鼠标实例的帮助函数。
 *
 * @param[in] p_inst 基类实例。
 *
 * @return HID鼠标类实例句柄。
 */
static inline app_usbd_hid_mouse_t const *
app_usbd_hid_mouse_class_get(app_usbd_class_inst_t const * p_inst)
{
    return (app_usbd_hid_mouse_t const *)p_inst;
}

/**
 * @brief 移动鼠标在X轴上的位置。
 *
 * @param[in] p_mouse HID鼠标实例（由@ref APP_USBD_HID_MOUSE_GLOBAL_DEF定义）。
 * @param[in] offset  鼠标X轴的相对移动位置（支持int8_t的完整范围）。
 *
 * @return 标准错误码。
 */
ret_code_t app_usbd_hid_mouse_x_move(app_usbd_hid_mouse_t const * p_mouse, int8_t offset);

/**
 * @brief 移动鼠标在Y轴上的位置。
 *
 * @param[in] p_mouse HID鼠标实例（由@ref APP_USBD_HID_MOUSE_GLOBAL_DEF定义）。
 * @param[in] offset  鼠标Y轴的相对移动位置（支持int8_t的完整范围）。
 *
 * @return 标准错误码。
 */
ret_code_t app_usbd_hid_mouse_y_move(app_usbd_hid_mouse_t const * p_mouse, int8_t offset);

/**
 * @brief 鼠标滚轮移动。
 *
 * @param[in] p_mouse HID鼠标实例（由@ref APP_USBD_HID_MOUSE_GLOBAL_DEF定义）。
 * @param[in] offset  滚轮的相对移动位置（支持int8_t的完整范围）。
 *
 * @return 标准错误码。
 */
ret_code_t app_usbd_hid_mouse_scroll_move(app_usbd_hid_mouse_t const * p_mouse, int8_t offset);

/**
 * @brief 设置鼠标按键状态。
 *
 * @param[in] p_mouse    HID鼠标实例（由@ref APP_USBD_HID_MOUSE_GLOBAL_DEF定义）。
 * @param[in] button_id  按键编号（0到7）。
 * @param[in] state      按键状态：true 表示按下，false 表示释放。
 *
 * @return 标准错误码。
 */
ret_code_t app_usbd_hid_mouse_button_state(app_usbd_hid_mouse_t const * p_mouse,
                                           uint8_t button_id,
                                           bool state);

/**
 * @brief 处理SET_PROTOCOL命令的函数。
 *
 * @param[in] p_mouse HID鼠标实例。
 * @param[in] ev      用户事件。
 *
 * @return 标准错误码。
 */
ret_code_t hid_mouse_on_set_protocol(app_usbd_hid_mouse_t const * p_mouse,
                                     app_usbd_hid_user_event_t ev);

/**
 * @brief 清除HID鼠标缓冲区并发送空报告。
 *
 * @param[in] p_inst 基类实例。
 *
 * @return 标准错误码。
 */
ret_code_t hid_mouse_clear_buffer(app_usbd_class_inst_t const * p_inst);

/** @} */

#endif /* APP_USBD_HID_MOUSE_H__ */
