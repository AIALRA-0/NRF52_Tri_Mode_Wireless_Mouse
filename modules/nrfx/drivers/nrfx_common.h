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

#ifndef NRFX_COMMON_H__
#define NRFX_COMMON_H__

#include <stdint.h>    // 提供标准整数类型定义（如 int8_t, uint32_t 等）
#include <stddef.h>    // 定义常用类型和宏（如 size_t, NULL 等）
#include <stdbool.h>   // 提供布尔类型支持（定义 true 和 false）

#include <nrf.h>       // 提供对Nordic Semiconductor芯片的硬件寄存器和外设的访问
#include <nrf_peripherals.h>  // 包含Nordic芯片的外设定义，帮助访问特定硬件功能

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_common 公共模块
 * @{
 * @ingroup nrfx
 * @brief 公共模块。
 */

/**
 * @brief 用于检查指定的标识符是否已定义且其值不为零的宏。
 *
 * 通常，预处理器将所有未定义的标识符视为值为零。
 * 然而，一些工具，如静态代码分析器，在评估此类标识符时可能会发出警告。
 * 该宏可以仅在此宏用于评估的地方抑制此类警告，而不会在整个代码中产生影响。
 */
#define NRFX_CHECK(module_enabled)  (module_enabled)

/**
 * @brief 用于在宏展开时连接两个标记的宏。
 *
 * @note 该宏分两步展开，以确保作为宏本身的标记在合并之前被完全展开。
 *
 * @param[in] p1 第一个标记。
 * @param[in] p2 第二个标记。
 *
 * @return 合并后的标记，除非它们无法形成有效的标记（在这种情况下，预处理器会发出警告并且不会执行合并）。
 *
 * @sa NRFX_CONCAT_3
 */
#define NRFX_CONCAT_2(p1, p2)       NRFX_CONCAT_2_(p1, p2)

/** @brief 内部宏，供 @ref NRFX_CONCAT_2 用于两步展开。 */
#define NRFX_CONCAT_2_(p1, p2)      p1 ## p2

/**
 * @brief 用于在宏展开时连接三个标记的宏。
 *
 * @note 该宏分两步展开，以确保作为宏本身的标记在合并之前被完全展开。
 *
 * @param[in] p1 第一个标记。
 * @param[in] p2 第二个标记。
 * @param[in] p3 第三个标记。
 *
 * @return 合并后的标记，除非它们无法形成有效的标记（在这种情况下，预处理器会发出警告并且不会执行合并）。
 *
 * @sa NRFX_CONCAT_2
 */
#define NRFX_CONCAT_3(p1, p2, p3)   NRFX_CONCAT_3_(p1, p2, p3)

/** @brief 内部宏，供 @ref NRFX_CONCAT_3 用于两步展开。 */
#define NRFX_CONCAT_3_(p1, p2, p3)  p1 ## p2 ## p3

/**
 * @brief 用于执行四舍五入的整数除法（与截断结果相对）。
 *
 * @param[in] a 分子。
 * @param[in] b 分母。
 *
 * @return a 除以 b 的四舍五入（整数）结果。
 */
#define NRFX_ROUNDED_DIV(a, b)  (((a) + ((b) / 2)) / (b))

/**
 * @brief 用于执行整数除法，确保结果向上取整的宏。
 *
 * @details 此宏的典型使用场景是计算需要多少大小为 b 的对象来存储 a 个字节。
 *
 * @param[in] a 分子。
 * @param[in] b 分母。
 *
 * @return a 除以 b 的整数结果，并向上取整。
 */
#define NRFX_CEIL_DIV(a, b)  ((((a) - 1) / (b)) + 1)

/**
 * @brief 用于获取数组元素数量的宏。
 *
 * @param[in] array 数组的名称。
 *
 * @return 数组的元素数量。
 */
#define NRFX_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/**
 * @brief 用于获取指定类型结构体中成员的字节偏移量的宏。
 *
 * @param[in] type   结构体类型。
 * @param[in] member 要查找偏移量的结构体成员。
 *
 * @return 成员的字节偏移量。
 */
#define NRFX_OFFSETOF(type, member)  ((size_t)&(((type *)0)->member))

/** @brief 用于检查EasyDMA传输长度是否超过指定外设限制的宏。
 *
 * @param[in] peripheral 要检查长度的外设。
 * @param[in] length1    要检查的第一个长度。
 * @param[in] length2    要检查的第二个长度（如不需要，请传递0）。
 *
 * @retval true  缓冲区长度没有超过指定外设的限制。
 * @retval false 缓冲区长度超过了指定外设的限制。
 */
#define NRFX_EASYDMA_LENGTH_VALIDATE(peripheral, length1, length2)            \
    (((length1) < (1U << NRFX_CONCAT_2(peripheral, _EASYDMA_MAXCNT_SIZE))) && \
     ((length2) < (1U << NRFX_CONCAT_2(peripheral, _EASYDMA_MAXCNT_SIZE))))

/**
 * @brief 用于等待直到满足条件的宏。
 *
 * @param[in]  condition 要满足的条件。
 * @param[in]  attempts  最大检查条件的次数。不得为0。
 * @param[in]  delay_us  连续检查之间的延迟（微秒）。
 * @param[out] result    存储等待过程结果的布尔变量。如果条件满足则设置为true，否则为false。
 */
#define NRFX_WAIT_FOR(condition, attempts, delay_us, result) \
do {                                                         \
    result =  false;                                         \
    uint32_t remaining_attempts = (attempts);                \
    do {                                                     \
           if (condition)                                    \
           {                                                 \
               result =  true;                               \
               break;                                        \
           }                                                 \
           NRFX_DELAY_US(delay_us);                          \
    } while (--remaining_attempts);                          \
} while(0)

/**
 * @brief 用于获取指定外设ID号的宏。
 *
 * 对于Nordic SoC中的外设，它们的ID号与其基地址直接相关。
 * 请参阅产品规格中的“外设接口”章节（“外设ID”部分）。
 *
 * @param[in] base_addr 外设基地址或指针。
 *
 * @return 与指定外设关联的ID号。
 */
#define NRFX_PERIPHERAL_ID_GET(base_addr)  (uint8_t)((uint32_t)(base_addr) >> 12)

/**
 * @brief 用于获取分配给指定外设的中断号的宏。
 *
 * 对于Nordic SoC中的外设，分配给外设的IRQ号等于其ID号。
 * 请参阅产品规格中的“外设接口”章节（“外设ID”和“中断”部分）。
 *
 * @param[in] base_addr 外设基地址或指针。
 *
 * @return 与指定外设关联的中断号。
 */
#define NRFX_IRQ_NUMBER_GET(base_addr)  NRFX_PERIPHERAL_ID_GET(base_addr)

/** @brief IRQ处理程序类型。 */
typedef void (* nrfx_irq_handler_t)(void);

/** @brief 驱动程序状态。 */
typedef enum
{
    NRFX_DRV_STATE_UNINITIALIZED, ///< 未初始化。
    NRFX_DRV_STATE_INITIALIZED,   ///< 已初始化但未通电。
    NRFX_DRV_STATE_POWERED_ON,    ///< 已初始化并通电。
} nrfx_drv_state_t;

/**
 * @brief 用于检查对象是否位于数据RAM区域的函数。
 *
 * 某些外设（使用EasyDMA的外设）要求传输缓冲区位于数据RAM区域。
 * 此函数可用于检查此条件是否满足。
 *
 * @param[in] p_object 要检查其位置的对象指针。
 *
 * @retval true  指向的对象位于数据RAM区域。
 * @retval false 指向的对象不位于数据RAM区域。
 */
__STATIC_INLINE bool nrfx_is_in_ram(void const * p_object);

/**
 * @brief 用于检查对象是否对齐到32位字的函数。
 *
 * 某些外设（使用EasyDMA的外设）要求传输缓冲区对齐到32位字。
 * 此函数可用于检查此条件是否满足。
 *
 * @param[in] p_object  要检查其位置的对象指针。
 *
 * @retval true  指向的对象对齐到32位字。
 * @retval false 指向的对象未对齐到32位字。
 */
__STATIC_INLINE bool nrfx_is_word_aligned(void const * p_object);

/**
 * @brief 用于获取指定外设的中断号的函数。
 *
 * @param[in] p_reg 外设基地址指针。
 *
 * @return 与指定外设关联的中断号。
 */
__STATIC_INLINE IRQn_Type nrfx_get_irq_number(void const * p_reg);

/**
 * @brief 用于将INTEN寄存器位位置转换为对应事件标识符的函数。
 *
 * 事件标识符是事件寄存器地址与外设基地址之间的偏移量，并且等于
 * HAL中的枚举类型（nrf_*_event_t）的对应值（因此可以直接转换）。
 *
 * @param[in] bit INTEN寄存器位位置。
 *
 * @return 事件标识符。
 *
 * @sa nrfx_event_to_bitpos
 */
__STATIC_INLINE uint32_t nrfx_bitpos_to_event(uint32_t bit);

/**
 * @brief 用于将事件标识符转换为对应INTEN寄存器位位置的函数。
 *
 * 事件标识符是事件寄存器地址与外设基地址之间的偏移量，并且等于
 * HAL中的枚举类型（nrf_*_event_t）的对应值（因此可以直接转换）。
 *
 * @param[in] event 事件标识符。
 *
 * @return INTEN寄存器位位置。
 *
 * @sa nrfx_bitpos_to_event
 */
__STATIC_INLINE uint32_t nrfx_event_to_bitpos(uint32_t event);


#ifndef SUPPRESS_INLINE_IMPLEMENTATION /** 此宏用于控制是否抑制内联函数的实现，当定义了 SUPPRESS_INLINE_IMPLEMENTATION 时，内联函数的实现将被抑制 */

/**
 * @brief 检查对象是否位于数据RAM区域。
 * 
 * 该函数用于检查传入的对象指针是否位于内存地址的Data RAM区域。
 * 在Nordic芯片中，Data RAM区域的地址空间范围以0x20000000为基准。
 * 通过对地址高位进行屏蔽（& 0xE0000000u），再与0x20000000u进行比较，
 * 来判断对象是否在Data RAM区域。
 *
 * EasyDMA等外设要求传输缓冲区位于Data RAM区域。
 *
 * @param[in] p_object 要检查的对象指针。
 * 
 * @retval true  如果对象位于Data RAM区域，则返回true。
 * @retval false 如果对象不在Data RAM区域，则返回false。
 */
__STATIC_INLINE bool nrfx_is_in_ram(void const * p_object)
{
    return ((((uint32_t)p_object) & 0xE0000000u) == 0x20000000u);
}

/**
 * @brief 检查对象是否对齐到32位字。
 * 
 * 该函数用于检查对象的地址是否对齐到32位字（4字节）。
 * EasyDMA等外设要求传输缓冲区对齐到32位（即地址低两位必须为0）。
 * 
 * @param[in] p_object 要检查的对象指针。
 * 
 * @retval true  对象的地址对齐到32位字。
 * @retval false 对象的地址未对齐到32位字。
 */
__STATIC_INLINE bool nrfx_is_word_aligned(void const * p_object)
{
    return ((((uint32_t)p_object) & 0x3u) == 0u);
}

/**
 * @brief 获取外设的中断号。
 * 
 * 该函数返回与指定外设基地址相关联的IRQ中断号。
 * 对于Nordic SoC，IRQ号等于外设ID号。
 * 
 * @param[in] p_reg 外设基地址指针。
 * 
 * @return 相关的IRQ中断号。
 */
__STATIC_INLINE IRQn_Type nrfx_get_irq_number(void const * p_reg)
{
    return (IRQn_Type)NRFX_IRQ_NUMBER_GET(p_reg);
}

/**
 * @brief 将INTEN寄存器中的位位置转换为事件标识符。
 * 
 * 该函数将INTEN寄存器中的位位置转换为事件寄存器中的事件标识符，
 * 通常用于事件到INTEN寄存器位位置的映射。
 * 
 * @param[in] bit INTEN寄存器中的位位置。
 * 
 * @return 对应的事件标识符。
 */
__STATIC_INLINE uint32_t nrfx_bitpos_to_event(uint32_t bit)
{
    static const uint32_t event_reg_offset = 0x100u;
    return event_reg_offset + (bit * sizeof(uint32_t));
}

/**
 * @brief 将事件标识符转换为INTEN寄存器中的位位置。
 * 
 * 该函数通过从事件寄存器的地址偏移量中减去事件寄存器的基偏移量来获得
 * 相应的INTEN寄存器中的位位置。
 * 
 * @param[in] event 事件标识符。
 * 
 * @return INTEN寄存器中的位位置。
 */
__STATIC_INLINE uint32_t nrfx_event_to_bitpos(uint32_t event)
{
    static const uint32_t event_reg_offset = 0x100u;
    return (event - event_reg_offset) / sizeof(uint32_t);
}


#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_COMMON_H__
