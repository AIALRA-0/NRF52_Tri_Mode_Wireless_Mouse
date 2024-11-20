/**
 * Copyright (c) 2006 - 2021, Nordic Semiconductor ASA
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
 * @brief Utilities for verifying program logic
 */

#ifndef NRF_ASSERT_H_
#define NRF_ASSERT_H_

// 引入基本类型头文件和Nordic芯片相关的头文件
#include <stdint.h>
#include "nrf.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief 用于处理断言的函数。
 *
 * 当触发断言时，将调用此函数。
 * 
 * @note
 * 此函数将在未来版本中被移除，已不再推荐使用。
 * 推荐使用 `app_error_fault_handler` 代替此函数。
 *
 * @post
 * 当触发断言后，所有硬件都会进入空闲状态，尤其是无线电（Radio）需要关闭。
 * 这是因为无线电可能处于某种状态，导致在执行无限循环的ASSERT时可能会持续发送数据包。
 *
 * @param line_num  触发断言的行号。
 * @param file_name 触发断言的文件名指针。
 */
// 使用lint工具保存当前状态，忽略特定警告（忽略符号14警告）
void assert_nrf_callback(uint16_t line_num, const uint8_t *file_name);
// 恢复lint工具的状态
// lint -save -esym(14, assert_nrf_callback)
// lint -restore

// 如果定义了DEBUG_NRF或者DEBUG_NRF_USER，则启用断言功能
#if (defined(DEBUG_NRF) || defined(DEBUG_NRF_USER))
#define NRF_ASSERT_PRESENT 1  // 断言功能有效
#else
#define NRF_ASSERT_PRESENT 0  // 断言功能无效
#endif

/**
 * @brief 断言宏，用于检查表达式是否为true。
 * 
 * 如果表达式为false，则触发断言处理程序 `assert_nrf_callback`。
 * 
 * @param expr 需要检查的表达式。
 */

// 如果使用了lint工具，定义简化的断言逻辑，
// 如果表达式为false，将进入一个无限循环。
#ifdef _lint
#define ASSERT(expr)                                                          \
if (expr)                                                                     \
{                                                                             \
}                                                                             \
else                                                                          \
{                                                                             \
    while (1);                                                                \
}
#else //_lint
// 对于非lint的情况，使用标准的断言逻辑。
// 如果NRF_ASSERT_PRESENT被设置为1（表示断言功能有效），则检查表达式。
// 如果表达式为false，则调用 `assert_nrf_callback`，传递当前行号和文件名。
// 否则，不做任何操作。
#define ASSERT(expr)                                                          \
if (NRF_ASSERT_PRESENT)                                                       \
{                                                                             \
    if (expr)                                                                 \
    {                                                                         \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        assert_nrf_callback((uint16_t)__LINE__, (uint8_t *)__FILE__);         \
    }                                                                         \
}
#endif // _lint

#ifdef __cplusplus
}
#endif

#endif /* NRF_ASSERT_H_ */

