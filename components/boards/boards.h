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

#include "nrf_gpio.h"  // ���� nRF оƬ�� GPIO ������صĺ����ͺ궨�壬�ṩ�˶� GPIO��ͨ������������ţ����ܵĿ��ơ�
#include "nordic_common.h"  // ���� Nordic Semiconductor ͨ�ú�ͳ��ù��ܵĶ��壬��λ������ģ�����ü��ȹ��ߺꡣ

// ������ͬ�Ŀ����������ļ�
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
#error "δ���������"  // ������ʾ������δ����

#endif

// ���������SHIELD_BSP_INC�꣬�������Ӧ��ͷ�ļ�
#if defined (SHIELD_BSP_INC)
  #include STRINGIFY(SHIELD_BSP_INC.h)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup BSP_BOARD_INIT_FLAGS �������ʼ����־����λ����λ������־λ�����С�
 * @{ */
#define BSP_INIT_NONE    0        /**< ����ʼ��LED��ť (@ref bsp_board_init)��*/
#define BSP_INIT_LEDS    (1 << 0) /**< ��ʼ��ʱ����LED (@ref bsp_board_init)��*/
#define BSP_INIT_BUTTONS (1 << 1) /**< ��ʼ��ʱ���ð�ť (@ref bsp_board_init)��*/
/**@} */

/**
 * ��ȡLED״̬�ĺ�����
 *
 * @param led_idx LED��������0��ʼ�������ض��������ͷ�ļ��ж��塣
 *
 * @return ���LED�򿪣�����True��
 */
bool bsp_board_led_state_get(uint32_t led_idx);

/**
 * ��LED�ĺ�����
 *
 * @param led_idx LED��������0��ʼ�������ض��������ͷ�ļ��ж��塣
 */
void bsp_board_led_on(uint32_t led_idx);

/**
 * �ر�LED�ĺ�����
 *
 * @param led_idx LED��������0��ʼ�������ض��������ͷ�ļ��ж��塣
 */
void bsp_board_led_off(uint32_t led_idx);

/**
 * ��תLED״̬�ĺ�����
 *
 * @param led_idx LED��������0��ʼ�������ض��������ͷ�ļ��ж��塣
 */
void bsp_board_led_invert(uint32_t led_idx);

/**
 * �ر�����LED�ĺ�����
 */
void bsp_board_leds_off(void);

/**
 * ������LED�ĺ�����
 */
void bsp_board_leds_on(void);

/**
 * ��ʼ���������BSP��Board Support Package������������
 *
 * @note ��� @ref BOARDS_WITH_USB_DFU_TRIGGER Ϊ1����Ҳ���ʼ��USB DFU�����⡣
 *
 * @param[in]  init_flags  ��ʼ����־��ָ����Ҫ��ʼ�������ݣ�LED/��ť����
 *                         �μ� @ref BSP_BOARD_INIT_FLAGS��
 */
void bsp_board_init(uint32_t init_flags);

/**
 * �����ź�ת��ΪLED�����ĺ�����
 *
 * @param pin_number ���źš�
 *
 * @return ���Ŷ�Ӧ��LED����������ṩ��������Ч������0xFFFFFFFF��
 */
uint32_t bsp_board_pin_to_led_idx(uint32_t pin_number);

/**
 * ��LED����ת��Ϊ���źŵĺ�����
 *
 * @param led_idx LED������
 *
 * @return ��Ӧ�����źš�
 */
uint32_t bsp_board_led_idx_to_pin(uint32_t led_idx);

/**
 * ��ȡ��ť״̬�ĺ�����
 *
 * @param button_idx ��ť��������0��ʼ�������ض��������ͷ�ļ��ж��塣
 *
 * @return �����ť���£�����True��
 */
bool bsp_board_button_state_get(uint32_t button_idx);

/**
 * �����ź�ת��Ϊ��ť�����ĺ�����
 *
 * @param pin_number ���źš�
 *
 * @return ���Ŷ�Ӧ�İ�ť����������ṩ��������Ч������0xFFFFFFFF��
 */
uint32_t bsp_board_pin_to_button_idx(uint32_t pin_number);

/**
 * ����ť����ת��Ϊ���źŵĺ�����
 *
 * @param button_idx ��ť������
 *
 * @return ��Ӧ�����źš�
 */
uint32_t bsp_board_button_idx_to_pin(uint32_t button_idx);

// LED�Ͱ�ť�Ķ���
#define BSP_BOARD_LED_0 0
#define BSP_BOARD_LED_1 1
#define BSP_BOARD_LED_2 2
#define BSP_BOARD_LED_3 3
#define BSP_BOARD_LED_4 4
#define BSP_BOARD_LED_5 5
#define BSP_BOARD_LED_6 6
#define BSP_BOARD_LED_7 7

// ����������������ĺ�
#define PIN_MASK(_pin)  /*lint -save -e504 */                     \
                        (1u << (uint32_t)((_pin) & (~P0_PIN_NUM))) \
                        /*lint -restore    */

// ��ȡ�������ڵĶ˿�
#define PIN_PORT(_pin) (((_pin) >= P0_PIN_NUM) ? NRF_P1 : NRF_GPIO)

// ����LED������Ͷ˿ڣ�������ڣ�
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

// ����LED����
#define LEDS_MASK      (BSP_LED_0_MASK | BSP_LED_1_MASK | \
                        BSP_LED_2_MASK | BSP_LED_3_MASK | \
                        BSP_LED_4_MASK | BSP_LED_5_MASK | \
                        BSP_LED_6_MASK | BSP_LED_7_MASK)

// ��ť��������������
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

// ���ɰ�ť����
#define BUTTONS_MASK   (BSP_BUTTON_0_MASK | BSP_BUTTON_1_MASK | \
                        BSP_BUTTON_2_MASK | BSP_BUTTON_3_MASK | \
                        BSP_BUTTON_4_MASK | BSP_BUTTON_5_MASK | \
                        BSP_BUTTON_6_MASK | BSP_BUTTON_7_MASK)

// LED�����궨�壬��֧��P0�˿�
#define LEDS_OFF(leds_mask) do { /* �رն�Ӧ�����LED */ \
												ASSERT(sizeof(leds_mask) == 4);                      												/* ��� leds_mask �Ĵ�С�Ƿ�Ϊ 4 �ֽڣ�32 λ����ȷ����Ϊ 32 λ����ֵ��ʹ�� ASSERT ����е��Լ�� */ \
                        NRF_GPIO->OUTSET = (leds_mask) & (LEDS_MASK & LEDS_INV_MASK);  							/* ��Ҫ�رյ���������Ϊ�ߵ�ƽ���� leds_mask �� LEDS_MASK �İ�λ�������õ� OUTSET �Ĵ��� */ \
                        NRF_GPIO->OUTCLR = (leds_mask) & (LEDS_MASK & ~LEDS_INV_MASK); } while (0)	/* ����Ҫ�رյ���������Ϊ�͵�ƽ���� leds_mask �� LEDS_MASK �İ�λ�������õ� OUTCLR �Ĵ��� */ 

#define LEDS_ON(leds_mask) do {  ASSERT(sizeof(leds_mask) == 4);                     /* �򿪶�Ӧ�����LED */ \
                       NRF_GPIO->OUTCLR = (leds_mask) & (LEDS_MASK & LEDS_INV_MASK); \
                       NRF_GPIO->OUTSET = (leds_mask) & (LEDS_MASK & ~LEDS_INV_MASK); } while (0)

#define LED_IS_ON(leds_mask) ((leds_mask) & (NRF_GPIO->OUT ^ LEDS_INV_MASK) )	/* ���LED��״̬��ͨ����λ������� GPIO ��״̬���бȽϣ������Ƿ�Ϊ����״̬ */

#define LEDS_INVERT(leds_mask) do { uint32_t gpio_state = NRF_GPIO->OUT;      /* ��ת��Ӧ�����LED */ \
                              ASSERT(sizeof(leds_mask) == 4);                 \
                              NRF_GPIO->OUTSET = ((leds_mask) & ~gpio_state); \
                              NRF_GPIO->OUTCLR = ((leds_mask) & gpio_state); } while (0)

#define LEDS_CONFIGURE(leds_mask) do { uint32_t pin;                  /* ���� leds_mask ����ָ���� LED ��������Ϊ���ģʽ */ \
                                  ASSERT(sizeof(leds_mask) == 4);     /* ����ÿ�����ţ�����32�����ţ� */ \
                                  for (pin = 0; pin < 32; pin++)      /* ��� leds_mask ��ĳ��λΪ1�������ø����� */ \
                                      if ( (leds_mask) & (1 << pin) ) /* ����Ӧ����������Ϊ���ģʽ */ \
                                          nrf_gpio_cfg_output(pin); } while (0)

#ifdef __cplusplus
}
#endif

#endif