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
#ifndef APP_USBD_HID_MOUSE_DESC_H__
#define APP_USBD_HID_MOUSE_DESC_H__

/**
 * @file
 * @defgroup app_usbd_hid_mouse_desc USB HID鼠标描述符
 * @{
 * @ingroup app_usbd_hid_mouse
 *
 * @brief 提供HID鼠标类的描述符初始化宏。
 */

/**
 * @brief HID鼠标类的接口描述符初始化
 *
 * @param interface_number 接口编号
 */
#define APP_USBD_HID_MOUSE_INTERFACE_DSC(interface_number)      \
        APP_USBD_HID_INTERFACE_DSC(interface_number,            \
                                   1,                           \
                                   APP_USBD_HID_SUBCLASS_BOOT,  \
                                   APP_USBD_HID_PROTO_MOUSE)

/**
 * @brief HID鼠标类的HID描述符初始化
 *
 * @param ... 描述符列表
 */
#define APP_USBD_HID_MOUSE_HID_DSC(...)     \
        APP_USBD_HID_HID_DSC(__VA_ARGS__)

/**
 * @brief HID鼠标类的端点描述符初始化
 *
 * @param endpoint_number 端点编号
 */
#define APP_USBD_HID_MOUSE_EP_DSC(endpoint_number)  \
        APP_USBD_HID_EP_DSC(endpoint_number, 8, 1)

/**
 * @brief USB HID鼠标报告描述符示例，用于n个按钮的鼠标
 *
 * @param bcnt 按钮数量，允许的取值范围为1到8
 */
#define APP_USBD_HID_MOUSE_REPORT_DSC_BUTTON(bcnt) {                \
    0x05, 0x01,       /* 使用页面（通用桌面控制）          */     \
    0x09, 0x02,       /* 使用项（鼠标）                   */     \
    0xA1, 0x01,       /* 集合（应用）                     */     \
    0x09, 0x01,       /* 使用项（指针）                   */     \
    0xA1, 0x00,       /* 集合（物理）                     */     \
    0x05, 0x09,       /* 使用页面（按钮）                 */     \
    0x19, 0x01,       /* 使用最小值（01）                 */     \
    0x29, bcnt,       /* 使用最大值（bcnt）               */     \
    0x15, 0x00,       /* 逻辑最小值（0）                 */     \
    0x25, 0x01,       /* 逻辑最大值（1）                 */     \
    0x75, 0x01,       /* 报告大小（1）                   */     \
    0x95, bcnt,       /* 报告计数（bcnt）                */     \
    0x81, 0x02,       /* 输入（数据，变量，绝对）         */     \
    0x75, (8-(bcnt)), /* 报告大小（8-(bcnt)）            */     \
    0x95, 0x01,       /* 报告计数（1）                   */     \
    0x81, 0x01,       /* 输入（常量）                    */     \
    0x05, 0x01,       /* 使用页面（通用桌面控制）         */     \
    0x09, 0x30,       /* 使用项（X）                     */     \
    0x09, 0x31,       /* 使用项（Y）                     */     \
    0x09, 0x38,       /* 使用项（滚动）                  */     \
    0x15, 0x81,       /* 逻辑最小值（-127）              */     \
    0x25, 0x7F,       /* 逻辑最大值（127）               */     \
    0x75, 0x08,       /* 报告大小（8）                   */     \
    0x95, 0x03,       /* 报告计数（3）                   */     \
    0x81, 0x06,       /* 输入（数据，变量， 相对）        */     \
    0xC0,             /* 结束集合                        */     \
    0xC0,             /* 结束集合                        */     \
}

/** @} */

#endif /* APP_USBD_HID_MOUSE_DESC_H__ */
