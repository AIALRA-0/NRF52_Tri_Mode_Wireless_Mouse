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
 * @brief Nordic Semiconductor 开发的固件中的通用定义和宏。
 *
 * 这个文件定义了一些常见的宏，用于启用模块、处理位操作、获取数值的特定位、连接多个参数等功能。
 */

#ifndef NORDIC_COMMON_H__
#define NORDIC_COMMON_H__

#ifdef __cplusplus
extern "C" {	//指示编译器按照 C 的方式进行符号解析（即不进行名字修饰（name mangling））的声明, C++ 中，编译器会对函数和变量名进行名字修饰，以支持函数重载等特性，而 C 语言中则没有这种修饰方式
#endif

/**
 * @brief 检查所选模块是否启用。
 *
 * 这是一个安全的驱动程序启用检查函数。
 * 从代码静态分析工具（如Lint）的角度来看，它是正确的（不使用默认的未定义值）。
 *
 * 使用示例：
 * @code
   #if NRF_MODULE_ENABLED(UART)
   ...
   #endif
 * @endcode
 *
 * @param module 模块名称。
 *
 * @retval 1 宏 <module>_ENABLED 被定义并且非零。
 * @retval 0 宏 <module>_ENABLED 未定义或等于零。
 *
 * @note 该宏故意不实现第二级扩展。要模块的名称必须直接作为参数提供，而不是传递一个变量或其他宏来间接表示模块名称，参数会直接与 `ENABLED` 后缀连接，而不对其值进行评估。
 */
// lint -emacro(491,NRF_MODULE_ENABLED) // 禁止Lint警告491 “defined操作符的非标准使用”
#ifdef NRF_MODULE_ENABLE_ALL
#warning "请勿在实际构建中使用NRF_MODULE_ENABLE_ALL。"
#define NRF_MODULE_ENABLED(module) 1
#else
#define NRF_MODULE_ENABLED(module) \
    ((defined(module ## _ENABLED) && (module ## _ENABLED)) ? 1 : 0)
#endif

/** 获取32位值的高8位 */
// lint -emacro(572,MSB_32) // 禁止Lint警告572 “过大的移位值”
#define MSB_32(a) (((a) & 0xFF000000) >> 24)

/** 获取32位值的低8位 */
#define LSB_32(a) ((a) & 0x000000FF)

/** 获取16位值的高8位 */
// lint -emacro(572,MSB_16) // 禁止Lint警告572 “过大的移位值”
#define MSB_16(a) (((a) & 0xFF00) >> 8)

/** 获取16位值的低8位 */
#define LSB_16(a) ((a) & 0x00FF)

/** 返回两个32位参数中的最小值 */
/*lint -emacro(506, MIN) */ /* 禁止Lint警告 “布尔值为常量” */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/** 返回两个32位参数中的最大值 */
/*lint -emacro(506, MAX) */ /* 禁止Lint警告 “布尔值为常量” */
#define MAX(a, b) ((a) < (b) ? (b) : (a))

/**@brief 将两个参数连接在一起。
 *
 * 实现了两级扩展，以确保在连接它们之前，所有参数都已展开。
 *
 * @param p1 第一个参数。
 * @param p2 第二个参数。
 *
 * @return 将两个参数连接在一起。它们必须生成有效的C语言符号，否则会产生预处理器错误。
 *
 * @sa CONCAT_3
 */
#define CONCAT_2(p1, p2)      CONCAT_2_(p1, p2)
/** CONCAT_2的辅助宏 */
#define CONCAT_2_(p1, p2)     p1##p2

/**@brief 将三个参数连接在一起。
 *
 * 实现了两级扩展，以确保在连接它们之前，所有参数都已展开。
 *
 * @param p1 第一个参数。
 * @param p2 第二个参数。
 * @param p3 第三个参数。
 *
 * @return 将三个参数连接在一起。它们必须生成有效的C语言符号，否则会产生预处理器错误。
 *
 * @sa CONCAT_2
 */
#define CONCAT_3(p1, p2, p3)  CONCAT_3_(p1, p2, p3)
/** CONCAT_3的辅助宏 */
#define CONCAT_3_(p1, p2, p3) p1##p2##p3

/**@brief 将宏参数转换为字符串常量。
 */
#define STRINGIFY_(val) #val
#define STRINGIFY(val)  STRINGIFY_(val)

/**@brief 计算数组中的元素个数。
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**@brief 设置32位字中的指定位。
 *
 * @param[in] W 要设置位的字。
 * @param[in] B 要设置的位号。
 */
#define SET_BIT(W, B)  ((W) |= (uint32_t)(1U << (B)))

/**@brief 清除32位字中的指定位。
 *
 * @param[in] W 要清除位的字。
 * @param[in] B 要清除的位号。
 */
#define CLR_BIT(W, B) ((W) &= (~(uint32_t)(1U << (B))))

/**@brief 检查位是否已设置。
 *
 * @param[in] W 要检查位的字。
 * @param[in] B 要检查的位号。
 *
 * @retval 1 如果位已设置。
 * @retval 0 如果位未设置。
 */
#define IS_SET(W, B) (((W) >> (B)) & 1)

/* 定义位的数值 */
#define BIT_0 0x01 /**< 位0的值 */
#define BIT_1 0x02 /**< 位1的值 */
#define BIT_2 0x04 /**< 位2的值 */
#define BIT_3 0x08 /**< 位3的值 */
#define BIT_4 0x10 /**< 位4的值 */
#define BIT_5 0x20 /**< 位5的值 */
#define BIT_6 0x40 /**< 位6的值 */
#define BIT_7 0x80 /**< 位7的值 */
#define BIT_8 0x0100 /**< 位8的值 */
#define BIT_9 0x0200 /**< 位9的值 */
#define BIT_10 0x0400 /**< 位10的值 */
#define BIT_11 0x0800 /**< 位11的值 */
#define BIT_12 0x1000 /**< 位12的值 */
#define BIT_13 0x2000 /**< 位13的值 */
#define BIT_14 0x4000 /**< 位14的值 */
#define BIT_15 0x8000 /**< 位15的值 */
#define BIT_16 0x00010000 /**< 位16的值 */
#define BIT_17 0x00020000 /**< 位17的值 */
#define BIT_18 0x00040000 /**< 位18的值 */
#define BIT_19 0x00080000 /**< 位19的值 */
#define BIT_20 0x00100000 /**< 位20的值 */
#define BIT_21 0x00200000 /**< 位21的值 */
#define BIT_22 0x00400000 /**< 位22的值 */
#define BIT_23 0x00800000 /**< 位23的值 */
#define BIT_24 0x01000000 /**< 位24的值 */
#define BIT_25 0x02000000 /**< 位25的值 */
#define BIT_26 0x04000000 /**< 位26的值 */
#define BIT_27 0x08000000 /**< 位27的值 */
#define BIT_28 0x10000000 /**< 位28的值 */
#define BIT_29 0x20000000 /**< 位29的值 */
#define BIT_30 0x40000000 /**< 位30的值 */
#define BIT_31 0x80000000 /**< 位31的值 */

/* 无用变量、参数和返回值的宏 */
#define UNUSED_VARIABLE(X)  ((void)(X))	/** 使用方法：UNUSED_VARIABLE(变量名) */
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X) UNUSED_VARIABLE(X)

#ifdef __cplusplus
}
#endif

#endif // NORDIC_COMMON_H__
