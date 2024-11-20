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
 
#ifndef BOARDS_H
#define BOARDS_H

#include "nrf_gpio.h"  // 包含 nRF 芯片的 GPIO 操作相关的函数和宏定义，提供了对 GPIO（通用输入输出引脚）功能的控制。
#include "nordic_common.h"  // 包含 Nordic Semiconductor 通用宏和常用功能的定义，如位操作、模块启用检查等工具宏。

// 包含不同的开发板配置文件
#if defined(BOARD_NRF6310)
  #include "nrf6310.h"
#elif defined(BOARD_PCA10000)
  #include "pca10000.h"
#elif defined(BOARD_PCA10001)
  #include "pca10001.h"
#elif defined(BOARD_PCA10002)
  #include "pca10000.h"
#elif defined(BOARD_PCA10003)
  #include "pca10003.h"
#elif defined(BOARD_PCA20006)
  #include "pca20006.h"
#elif defined(BOARD_PCA10028)
  #include "pca10028.h"
#elif defined(BOARD_PCA10031)
  #include "pca10031.h"
#elif defined(BOARD_PCA10036)
  #include "pca10036.h"
#elif defined(BOARD_PCA10040)
  #include "pca10040.h"
#elif defined(BOARD_PCA10056)
  #include "pca10056.h"
#elif defined(BOARD_PCA10100)
  #include "pca10100.h"
#elif defined(BOARD_PCA10112)
  #include "pca10112.h"  
#elif defined(BOARD_PCA20020)
  #include "pca20020.h"
#elif defined(BOARD_PCA10059)
  #include "pca10059.h"
#elif defined(BOARD_WT51822)
  #include "wt51822.h"
#elif defined(BOARD_N5DK1)
  #include "n5_starterkit.h"
#elif defined(BOARD_D52DK1)
  #include "d52_starterkit.h"
#elif defined(BOARD_ARDUINO_PRIMO)
  #include "arduino_primo.h"
#elif defined(CUSTOM_BOARD_INC)
  #include STRINGIFY(CUSTOM_BOARD_INC.h)
#elif defined(BOARD_CUSTOM)
  #include "custom_board.h"
#else
#error "未定义板类型"  // 报错提示板类型未定义

#endif

// 如果定义了SHIELD_BSP_INC宏，则包含相应的头文件
#if defined (SHIELD_BSP_INC)
  #include STRINGIFY(SHIELD_BSP_INC.h)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup BSP_BOARD_INIT_FLAGS 开发板初始化标志，移位用于位掩码或标志位操作中。
 * @{ */
#define BSP_INIT_NONE    0        /**< 不初始化LED或按钮 (@ref bsp_board_init)。*/
#define BSP_INIT_LEDS    (1 << 0) /**< 初始化时启用LED (@ref bsp_board_init)。*/
#define BSP_INIT_BUTTONS (1 << 1) /**< 初始化时启用按钮 (@ref bsp_board_init)。*/
/**@} */

/**
 * 获取LED状态的函数。
 *
 * @param led_idx LED索引（从0开始），在特定开发板的头文件中定义。
 *
 * @return 如果LED打开，返回True。
 */
bool bsp_board_led_state_get(uint32_t led_idx);

/**
 * 打开LED的函数。
 *
 * @param led_idx LED索引（从0开始），在特定开发板的头文件中定义。
 */
void bsp_board_led_on(uint32_t led_idx);

/**
 * 关闭LED的函数。
 *
 * @param led_idx LED索引（从0开始），在特定开发板的头文件中定义。
 */
void bsp_board_led_off(uint32_t led_idx);

/**
 * 反转LED状态的函数。
 *
 * @param led_idx LED索引（从0开始），在特定开发板的头文件中定义。
 */
void bsp_board_led_invert(uint32_t led_idx);

/**
 * 关闭所有LED的函数。
 */
void bsp_board_leds_off(void);

/**
 * 打开所有LED的函数。
 */
void bsp_board_leds_on(void);

/**
 * 初始化开发板的BSP（Board Support Package）处理函数。
 *
 * @note 如果 @ref BOARDS_WITH_USB_DFU_TRIGGER 为1，这也会初始化USB DFU触发库。
 *
 * @param[in]  init_flags  初始化标志，指定需要初始化的内容（LED/按钮）。
 *                         参见 @ref BSP_BOARD_INIT_FLAGS。
 */
void bsp_board_init(uint32_t init_flags);

/**
 * 将引脚号转换为LED索引的函数。
 *
 * @param pin_number 引脚号。
 *
 * @return 引脚对应的LED索引，如果提供的引脚无效，返回0xFFFFFFFF。
 */
uint32_t bsp_board_pin_to_led_idx(uint32_t pin_number);

/**
 * 将LED索引转换为引脚号的函数。
 *
 * @param led_idx LED索引。
 *
 * @return 对应的引脚号。
 */
uint32_t bsp_board_led_idx_to_pin(uint32_t led_idx);

/**
 * 获取按钮状态的函数。
 *
 * @param button_idx 按钮索引（从0开始），在特定开发板的头文件中定义。
 *
 * @return 如果按钮按下，返回True。
 */
bool bsp_board_button_state_get(uint32_t button_idx);

/**
 * 将引脚号转换为按钮索引的函数。
 *
 * @param pin_number 引脚号。
 *
 * @return 引脚对应的按钮索引，如果提供的引脚无效，返回0xFFFFFFFF。
 */
uint32_t bsp_board_pin_to_button_idx(uint32_t pin_number);

/**
 * 将按钮索引转换为引脚号的函数。
 *
 * @param button_idx 按钮索引。
 *
 * @return 对应的引脚号。
 */
uint32_t bsp_board_button_idx_to_pin(uint32_t button_idx);

// LED和按钮的定义
#define BSP_BOARD_LED_0 0
#define BSP_BOARD_LED_1 1
#define BSP_BOARD_LED_2 2
#define BSP_BOARD_LED_3 3
#define BSP_BOARD_LED_4 4
#define BSP_BOARD_LED_5 5
#define BSP_BOARD_LED_6 6
#define BSP_BOARD_LED_7 7

// 用于生成引脚掩码的宏
#define PIN_MASK(_pin)  /*lint -save -e504 */                     \
                        (1u << (uint32_t)((_pin) & (~P0_PIN_NUM))) \
                        /*lint -restore    */

// 获取引脚所在的端口
#define PIN_PORT(_pin) (((_pin) >= P0_PIN_NUM) ? NRF_P1 : NRF_GPIO)

// 定义LED的掩码和端口（如果存在）
#ifdef BSP_LED_0
#define BSP_LED_0_MASK PIN_MASK(BSP_LED_0)
#define BSP_LED_0_PORT PIN_PORT(BSP_LED_0)
#else
#define BSP_LED_0_MASK 0
#define BSP_LED_0_PORT 0
#endif
#ifdef BSP_LED_1
#define BSP_LED_1_MASK PIN_MASK(BSP_LED_1)
#define BSP_LED_1_PORT PIN_PORT(BSP_LED_1)
#else
#define BSP_LED_1_MASK 0
#define BSP_LED_1_PORT 0
#endif
#ifdef BSP_LED_2
#define BSP_LED_2_MASK PIN_MASK(BSP_LED_2)
#define BSP_LED_2_PORT PIN_PORT(BSP_LED_2)
#else
#define BSP_LED_2_MASK 0
#define BSP_LED_2_PORT 0
#endif
#ifdef BSP_LED_3
#define BSP_LED_3_MASK PIN_MASK(BSP_LED_3)
#define BSP_LED_3_PORT PIN_PORT(BSP_LED_3)
#else
#define BSP_LED_3_MASK 0
#define BSP_LED_3_PORT 0
#endif
#ifdef BSP_LED_4
#define BSP_LED_4_MASK PIN_MASK(BSP_LED_4)
#define BSP_LED_4_PORT PIN_PORT(BSP_LED_4)
#else
#define BSP_LED_4_MASK 0
#define BSP_LED_4_PORT 0
#endif
#ifdef BSP_LED_5
#define BSP_LED_5_MASK PIN_MASK(BSP_LED_5)
#define BSP_LED_5_PORT PIN_PORT(BSP_LED_5)
#else
#define BSP_LED_5_MASK 0
#define BSP_LED_5_PORT 0
#endif
#ifdef BSP_LED_6
#define BSP_LED_6_MASK PIN_MASK(BSP_LED_6)
#define BSP_LED_6_PORT PIN_PORT(BSP_LED_6)
#else
#define BSP_LED_6_MASK 0
#define BSP_LED_6_PORT 0
#endif
#ifdef BSP_LED_7
#define BSP_LED_7_MASK PIN_MASK(BSP_LED_7)
#define BSP_LED_7_PORT PIN_PORT(BSP_LED_7)
#else
#define BSP_LED_7_MASK 0
#define BSP_LED_7_PORT 0
#endif

// 生成LED掩码
#define LEDS_MASK      (BSP_LED_0_MASK | BSP_LED_1_MASK | \
                        BSP_LED_2_MASK | BSP_LED_3_MASK | \
                        BSP_LED_4_MASK | BSP_LED_5_MASK | \
                        BSP_LED_6_MASK | BSP_LED_7_MASK)

// 按钮定义与掩码生成
#define BSP_BOARD_BUTTON_0 0
#define BSP_BOARD_BUTTON_1 1
#define BSP_BOARD_BUTTON_2 2
#define BSP_BOARD_BUTTON_3 3
#define BSP_BOARD_BUTTON_4 4
#define BSP_BOARD_BUTTON_5 5
#define BSP_BOARD_BUTTON_6 6
#define BSP_BOARD_BUTTON_7 7

#ifdef BSP_BUTTON_0
#define BSP_BUTTON_0_MASK (1<<BSP_BUTTON_0)
#else
#define BSP_BUTTON_0_MASK 0
#endif

#ifdef BSP_BUTTON_1
#define BSP_BUTTON_1_MASK (1<<BSP_BUTTON_1)
#else
#define BSP_BUTTON_1_MASK 0
#endif

// 生成按钮掩码
#define BUTTONS_MASK   (BSP_BUTTON_0_MASK | BSP_BUTTON_1_MASK | \
                        BSP_BUTTON_2_MASK | BSP_BUTTON_3_MASK | \
                        BSP_BUTTON_4_MASK | BSP_BUTTON_5_MASK | \
                        BSP_BUTTON_6_MASK | BSP_BUTTON_7_MASK)

// LED操作宏定义，仅支持P0端口
#define LEDS_OFF(leds_mask) do { /* 关闭对应掩码的LED */ \
												ASSERT(sizeof(leds_mask) == 4);                      												/* 检查 leds_mask 的大小是否为 4 字节（32 位），确保其为 32 位的数值，使用 ASSERT 宏进行调试检查 */ \
                        NRF_GPIO->OUTSET = (leds_mask) & (LEDS_MASK & LEDS_INV_MASK);  							/* 需要关闭的引脚设置为高电平，将 leds_mask 和 LEDS_MASK 的按位与结果设置到 OUTSET 寄存器 */ \
                        NRF_GPIO->OUTCLR = (leds_mask) & (LEDS_MASK & ~LEDS_INV_MASK); } while (0)	/* 不需要关闭的引脚设置为低电平，将 leds_mask 和 LEDS_MASK 的按位与结果设置到 OUTCLR 寄存器 */ 

#define LEDS_ON(leds_mask) do {  ASSERT(sizeof(leds_mask) == 4);                     /* 打开对应掩码的LED */ \
                       NRF_GPIO->OUTCLR = (leds_mask) & (LEDS_MASK & LEDS_INV_MASK); \
                       NRF_GPIO->OUTSET = (leds_mask) & (LEDS_MASK & ~LEDS_INV_MASK); } while (0)

#define LED_IS_ON(leds_mask) ((leds_mask) & (NRF_GPIO->OUT ^ LEDS_INV_MASK) )	/* 检查LED的状态，通过按位与操作和 GPIO 的状态进行比较，返回是否为亮灯状态 */

#define LEDS_INVERT(leds_mask) do { uint32_t gpio_state = NRF_GPIO->OUT;      /* 反转对应掩码的LED */ \
                              ASSERT(sizeof(leds_mask) == 4);                 \
                              NRF_GPIO->OUTSET = ((leds_mask) & ~gpio_state); \
                              NRF_GPIO->OUTCLR = ((leds_mask) & gpio_state); } while (0)

#define LEDS_CONFIGURE(leds_mask) do { uint32_t pin;                  /* 将由 leds_mask 掩码指定的 LED 引脚配置为输出模式 */ \
                                  ASSERT(sizeof(leds_mask) == 4);     /* 遍历每个引脚（假设32个引脚） */ \
                                  for (pin = 0; pin < 32; pin++)      /* 如果 leds_mask 的某个位为1，则配置该引脚 */ \
                                      if ( (leds_mask) & (1 << pin) ) /* 将相应的引脚配置为输出模式 */ \
                                          nrf_gpio_cfg_output(pin); } while (0)

#ifdef __cplusplus
}
#endif

#endif
