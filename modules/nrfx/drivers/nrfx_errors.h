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

#ifndef NRFX_ERRORS_H__
#define NRFX_ERRORS_H__

#if !NRFX_CHECK(NRFX_CUSTOM_ERROR_CODES) // 如果没有启用自定义错误代码，则使用默认的错误代码定义

/**
 * @defgroup nrfx_error_codes 全局错误代码
 * @{
 * @ingroup nrfx
 *
 * @brief 全局错误代码定义。
 */

/** @brief 错误代码的基地址，所有的全局错误代码都从这个基地址开始 */
#define NRFX_ERROR_BASE_NUM         0x0BAD0000

/** @brief 驱动程序错误代码的基地址，所有的驱动程序错误代码都从这个基地址开始 */
#define NRFX_ERROR_DRIVERS_BASE_NUM (NRFX_ERROR_BASE_NUM + 0x10000)

/** @brief 错误代码的枚举类型，程序可以通过这些错误码来标识不同类型的错误，方便在不同场景下进行错误处理和调试 */
typedef enum {
    NRFX_SUCCESS                    = (NRFX_ERROR_BASE_NUM + 0),  ///< 操作成功执行。
    NRFX_ERROR_INTERNAL             = (NRFX_ERROR_BASE_NUM + 1),  ///< 内部错误。
    NRFX_ERROR_NO_MEM               = (NRFX_ERROR_BASE_NUM + 2),  ///< 操作没有足够的内存。
    NRFX_ERROR_NOT_SUPPORTED        = (NRFX_ERROR_BASE_NUM + 3),  ///< 操作不支持。
    NRFX_ERROR_INVALID_PARAM        = (NRFX_ERROR_BASE_NUM + 4),  ///< 参数无效。
    NRFX_ERROR_INVALID_STATE        = (NRFX_ERROR_BASE_NUM + 5),  ///< 状态无效，在当前状态下不允许操作。
    NRFX_ERROR_INVALID_LENGTH       = (NRFX_ERROR_BASE_NUM + 6),  ///< 长度无效。
    NRFX_ERROR_TIMEOUT              = (NRFX_ERROR_BASE_NUM + 7),  ///< 操作超时。
    NRFX_ERROR_FORBIDDEN            = (NRFX_ERROR_BASE_NUM + 8),  ///< 操作被禁止。
    NRFX_ERROR_NULL                 = (NRFX_ERROR_BASE_NUM + 9),  ///< 空指针错误。
    NRFX_ERROR_INVALID_ADDR         = (NRFX_ERROR_BASE_NUM + 10), ///< 无效的内存地址。
    NRFX_ERROR_BUSY                 = (NRFX_ERROR_BASE_NUM + 11), ///< 设备忙。
    NRFX_ERROR_ALREADY_INITIALIZED  = (NRFX_ERROR_BASE_NUM + 12), ///< 模块已初始化。

    NRFX_ERROR_DRV_TWI_ERR_OVERRUN  = (NRFX_ERROR_DRIVERS_BASE_NUM + 0), ///< TWI/I2C 错误：溢出。
    NRFX_ERROR_DRV_TWI_ERR_ANACK    = (NRFX_ERROR_DRIVERS_BASE_NUM + 1), ///< TWI/I2C 错误：地址未被确认。
    NRFX_ERROR_DRV_TWI_ERR_DNACK    = (NRFX_ERROR_DRIVERS_BASE_NUM + 2)  ///< TWI/I2C 错误：数据未被确认。
} nrfx_err_t;

/** @} */

#endif // !NRFX_CHECK(NRFX_CUSTOM_ERROR_CODES)

#endif // NRFX_ERRORS_H__





