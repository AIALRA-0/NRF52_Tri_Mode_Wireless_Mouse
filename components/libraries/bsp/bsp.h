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
 * @defgroup bsp �弶֧�ְ� (BSP)
 * @{
 * @ingroup app_common
 *
 * @brief BSP ģ�顣
 * @details ��ģ���ṩ�˶Կ�����ĳ���㡣
 *          �������û��Լ򵥵ķ�ʽͨ�� LED ָʾĳЩ״̬��
 *          ����ͨ������ BSP_SIMPLE ���޸�ģ�鹦�ܣ��Լ򻯸�ģ��Ĺ��ܣ�
 *          ֻ֧�����úͶ�ȡ��ť��״̬��
 */

#ifndef BSP_H__
#define BSP_H__

#include <stdint.h>
#include <stdbool.h>
#include "boards.h"

// ��������� BSP_DEFINES_ONLY����ôֻ������� BSP���弶֧�ְ���������صĴ��룬���������κζ���Ĺ���ʵ��
// BSP_SIMPLE ���ڼ��� BSP ģ��Ĺ��ܡ��������������꣬BSP ģ�齫��Ϊ��֧�ְ�ť״̬�Ķ�ȡ�Ͳ���������������� LED ״ָ̬ʾ�ȸ��ӹ���
#if !defined(BSP_DEFINES_ONLY) && !defined(BSP_SIMPLE)
#include "app_button.h"

#define BSP_BUTTON_ACTION_PUSH      (APP_BUTTON_PUSH)    /**< �����°�ť���μ� @ref bsp_button_action_t�� */
#define BSP_BUTTON_ACTION_RELEASE   (APP_BUTTON_RELEASE) /**< �����ͷŰ�ť���μ� @ref bsp_button_action_t�� */
#define BSP_BUTTON_ACTION_LONG_PUSH (2)                  /**< ��������ť������ʱ��Ϊ @ref BSP_LONG_PUSH_TIMEOUT_MS ���롣�μ� @ref bsp_button_action_t�� */

#endif

/* BSP_UART_SUPPORT
 * �ö��������� UART ֧��ģ�顣
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t bsp_button_action_t; /**< ��ͬ��ť����ִ�еĶ����� */

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
} /**< �μ� @ref examples_bsp_states���˽���Щ״̬����� PCA10028/PCA10040 ������� PCA10031 ���ܹ���ָʾ��*/


/**@brief BSP ״ָ̬ʾ��
 *
 * @details �μ� @ref examples_bsp_states���˽���Щ״̬����� PCA10028/PCA10040 ������� PCA10031 ���ܹ���ָʾ��
 */
typedef enum
{
    BSP_INDICATE_FIRST = 0,
    BSP_INDICATE_IDLE  = BSP_INDICATE_FIRST, /**< �μ� \ref BSP_INDICATE_IDLE��*/
    BSP_INDICATE_SCANNING,                   /**< �μ� \ref BSP_INDICATE_SCANNING��*/
    BSP_INDICATE_ADVERTISING,                /**< �μ� \ref BSP_INDICATE_ADVERTISING��*/
    BSP_INDICATE_ADVERTISING_WHITELIST,      /**< �μ� \ref BSP_INDICATE_ADVERTISING_WHITELIST��*/
    BSP_INDICATE_ADVERTISING_SLOW,           /**< �μ� \ref BSP_INDICATE_ADVERTISING_SLOW��*/
    BSP_INDICATE_ADVERTISING_DIRECTED,       /**< �μ� \ref BSP_INDICATE_ADVERTISING_DIRECTED��*/
    BSP_INDICATE_BONDING,                    /**< �μ� \ref BSP_INDICATE_BONDING��*/
    BSP_INDICATE_CONNECTED,                  /**< �μ� \ref BSP_INDICATE_CONNECTED��*/
    BSP_INDICATE_SENT_OK,                    /**< �μ� \ref BSP_INDICATE_SENT_OK��*/
    BSP_INDICATE_SEND_ERROR,                 /**< �μ� \ref BSP_INDICATE_SEND_ERROR��*/
    BSP_INDICATE_RCV_OK,                     /**< �μ� \ref BSP_INDICATE_RCV_OK��*/
    BSP_INDICATE_RCV_ERROR,                  /**< �μ� \ref BSP_INDICATE_RCV_ERROR��*/
    BSP_INDICATE_FATAL_ERROR,                /**< �μ� \ref BSP_INDICATE_FATAL_ERROR��*/
    BSP_INDICATE_ALERT_0,                    /**< �μ� \ref BSP_INDICATE_ALERT_0��*/
    BSP_INDICATE_ALERT_1,                    /**< �μ� \ref BSP_INDICATE_ALERT_1��*/
    BSP_INDICATE_ALERT_2,                    /**< �μ� \ref BSP_INDICATE_ALERT_2��*/
    BSP_INDICATE_ALERT_3,                    /**< �μ� \ref BSP_INDICATE_ALERT_3��*/
    BSP_INDICATE_ALERT_OFF,                  /**< �μ� \ref BSP_INDICATE_ALERT_OFF��*/
    BSP_INDICATE_USER_STATE_OFF,             /**< �μ� \ref BSP_INDICATE_USER_STATE_OFF��*/
    BSP_INDICATE_USER_STATE_0,               /**< �μ� \ref BSP_INDICATE_USER_STATE_0��*/
    BSP_INDICATE_USER_STATE_1,               /**< �μ� \ref BSP_INDICATE_USER_STATE_1��*/
    BSP_INDICATE_USER_STATE_2,               /**< �μ� \ref BSP_INDICATE_USER_STATE_2��*/
    BSP_INDICATE_USER_STATE_3,               /**< �μ� \ref BSP_INDICATE_USER_STATE_3��*/
    BSP_INDICATE_USER_STATE_ON,              /**< �μ� \ref BSP_INDICATE_USER_STATE_ON��*/
    BSP_INDICATE_LAST = BSP_INDICATE_USER_STATE_ON
} bsp_indication_t;

/**@brief BSP �¼���
 *
 * @note BSP_EVENT_KEY_0 �� BSP_EVENT_KEY_LAST ���¼�Ĭ�Ϸ������ť��
 */
typedef enum
{
    BSP_EVENT_NOTHING = 0,                  /**< �����¼�������������Է�ֹ�����¼������ò�������*/
    BSP_EVENT_DEFAULT,                      /**< �����¼�������������Է���Ĭ���¼���������*/
    BSP_EVENT_CLEAR_BONDING_DATA,           /**< �־õİ�����Ӧ�ñ������*/
    BSP_EVENT_CLEAR_ALERT,                  /**< Ӧ�����������*/
    BSP_EVENT_DISCONNECT,                   /**< Ӧ�öϿ����ӡ�*/
    BSP_EVENT_ADVERTISING_START,            /**< �豸Ӧ�ÿ�ʼ�㲥��*/
    BSP_EVENT_ADVERTISING_STOP,             /**< �豸Ӧ��ֹͣ�㲥��*/
    BSP_EVENT_WHITELIST_OFF,                /**< �豸Ӧ���Ƴ���㲥��������*/
    BSP_EVENT_BOND,                         /**< �豸Ӧ���뵱ǰ���ӵĶԵ��豸�󶨡�*/
    BSP_EVENT_RESET,                        /**< �豸Ӧ�ø�λ��*/
    BSP_EVENT_SLEEP,                        /**< �豸Ӧ�ý���˯��ģʽ��*/
    BSP_EVENT_WAKEUP,                       /**< �豸Ӧ�ô�˯��ģʽ���ѡ�*/
    BSP_EVENT_SYSOFF,                       /**< �豸Ӧ�ý���ϵͳ�ر�ģʽ��û�л��ѣ���*/
    BSP_EVENT_DFU,                          /**< �豸Ӧ�ý��� DFU ģʽ��*/
    BSP_EVENT_KEY_0,                        /**< BSP_BUTTON_0 ��ť���Ͷ�����Ĭ���¼��������ð�ť����ʱ����*/
    BSP_EVENT_KEY_1,                        /**< BSP_BUTTON_1 ��ť���Ͷ�����Ĭ���¼��������ð�ť����ʱ����*/
    BSP_EVENT_KEY_2,                        /**< BSP_BUTTON_2 ��ť���Ͷ�����Ĭ���¼��������ð�ť����ʱ����*/
    BSP_EVENT_KEY_3,                        /**< BSP_BUTTON_3 ��ť���Ͷ�����Ĭ���¼��������ð�ť����ʱ����*/
    BSP_EVENT_KEY_4,                        /**< BSP_BUTTON_4 ��ť���Ͷ�����Ĭ���¼��������ð�ť����ʱ����*/
    BSP_EVENT_KEY_5,                        /**< BSP_BUTTON_5 ��ť���Ͷ�����Ĭ���¼��������ð�ť����ʱ����*/
    BSP_EVENT_KEY_6,                        /**< BSP_BUTTON_6 ��ť���Ͷ�����Ĭ���¼��������ð�ť����ʱ����*/
    BSP_EVENT_KEY_7,                        /**< BSP_BUTTON_7 ��ť���Ͷ�����Ĭ���¼��������ð�ť����ʱ����*/
    BSP_EVENT_KEY_LAST = BSP_EVENT_KEY_7,
} bsp_event_t;


typedef struct
{
    bsp_event_t push_event;      /**< ��ť���水�¶���ʱ�������¼��� */
    bsp_event_t long_push_event; /**< ��ť��������ʱ�������¼��� */
    bsp_event_t release_event;   /**< ��ť�ͷ�ʱ�������¼��� */
} bsp_button_event_cfg_t;

/**@brief BSP ģ���¼��ص��������͡�
 *
 * @details �� BSP ģ���з����¼�ʱ�������øûص�������֪ͨӦ�ó���
 *
 * @param[in]   bsp_event_t BSP �¼����͡�
 */
typedef void (* bsp_event_callback_t)(bsp_event_t);


/**@brief       ��ʼ�� BSP �ĺ�����
 *
 * @details     �ú�����ʼ���弶֧�ְ���������״ָ̬ʾ�Ͱ�ť��Ӧ��Ĭ���¼��ᱻ�������ť��
 * @note        �ڵ��ô˺���֮ǰ�������ʼ����������ģ�飺
 *              - @ref app_timer ���� LED ֧��
 *              - @ref app_gpiote ���ڰ�ť֧��
 *
 * @param[in]   type               ʹ�õ��������͡�
 * @param[in]   callback           �ڼ�⵽��ť����/�¼�ʱ���õĺ�����
 *
 * @retval      NRF_SUCCESS               ��� BSP ģ��ɹ���ʼ����
 * @retval      NRF_ERROR_INVALID_STATE   ���Ӧ�ó����ʱ��ģ��δ��ʼ����
 * @retval      NRF_ERROR_NO_MEM          ����Ѵﵽ����ʱ��������
 * @retval      NRF_ERROR_INVALID_PARAM   ��� GPIOTE �û����ࡣ
 * @retval      NRF_ERROR_INVALID_STATE   �����ť�� GPIOTE δ��ʼ����
 */
uint32_t bsp_init(uint32_t type, bsp_event_callback_t callback);


/**@brief       ��鰴ť״̬�ĺ�����
 *
 * @details     �˺�����鰴ť�Ƿ񱻰��¡������ť ID ������Χ���������� false��
 *
 * @param[in]   button                  Ҫ���İ�ť ID��
 *
 * @retval      true                    �����ť�����¡�
 * @retval      false                   �����ťû�б����¡�
 */
bool bsp_button_is_pressed(uint32_t button);


/**@brief       Ϊ��ť�����ض��¼��ĺ�����
 *
 * @details     �˺����������¶���������ť�ı�׼�¼���Ҫȡ�������¼������ṩ�¼� @ref BSP_EVENT_NOTHING��
 *
 * @param[in]   button                   Ҫ���¶���İ�ť ID��
 * @param[in]   action                   Ҫ�����¼��İ�ť������
 * @param[in]   event                    Ҫ�������ť���¼���
 *
 * @retval      NRF_SUCCESS              ����¼��ɹ��������ť��
 * @retval      NRF_ERROR_INVALID_PARAM  �����ť ID ��ť������Ч��
 */
uint32_t bsp_event_to_button_action_assign(uint32_t button, bsp_button_action_t action, bsp_event_t event);


/**@brief       ����ָʾ��������״̬�ĺ�����
 *
 * @details     �˺���ͨ�� LED��������ã�ָʾ�����״̬��
 *
 * @note        �����Ƕ���ָʾ�ġ�
 *
 * @param[in]   indicate   Ҫָʾ��״̬��
 *
 * @retval      NRF_SUCCESS               ���״̬�ɹ�ָʾ��
 * @retval      NRF_ERROR_NO_MEM          ����ڲ���ʱ����������������
 * @retval      NRF_ERROR_INVALID_STATE   ���Ӧ�ó����ʱ��ģ��δ��ʼ�������ڲ���ʱ��δ������
 */
uint32_t bsp_indication_set(bsp_indication_t indicate);


/**@brief       �������а�ť�Ĺ��ܡ�
 *
 * @details     ���ô˺��������а�ť�ڰ���ʱ�������¼����������а�ť�����Ի���ϵͳ��
 *
 * @retval      NRF_SUCCESS              �����ť�ɹ����á�
 * @retval      NRF_ERROR_NOT_SUPPORTED  �������û�а�ť������ BSP_SIMPLE��
 * @return      ���ݵĴ�����롣
 */
uint32_t bsp_buttons_enable(void);


/**@brief       �������а�ť�Ĺ��ܡ�
 *
 * @details     ���ô˺����󣬰��°�ť���������¼������Ұ�ť�޷�����ϵͳ��
 *
 * @retval      NRF_SUCCESS              �����ť�ɹ����á�
 * @retval      NRF_ERROR_NOT_SUPPORTED  �������û�а�ť������ BSP_SIMPLE��
 * @return      ���ݵĴ�����롣
 */
uint32_t bsp_buttons_disable(void);


/**@brief       ���ø�����ť�� SYSTEM OFF ���ѹ��ܡ�
 *
 * @details     ���ô˺����󣬿���ʹ�ð�ť����оƬ���˺���Ӧ�ڽ���˯��ǰ�������á�
 *
 * @param[in]   button_idx  ��ť��������
 *
 * @retval      NRF_SUCCESS              �����ť�ɹ����á�
 * @retval      NRF_ERROR_NOT_SUPPORTED  �������û�а�ť������ BSP_SIMPLE��
 */
uint32_t bsp_wakeup_button_enable(uint32_t button_idx);


/**@brief       ���ø�����ť�Ļ��ѹ��ܡ�
 *
 * @param[in]   button_idx  ��ť��������
 *
 * @retval      NRF_SUCCESS              �����ť�ɹ����á�
 * @retval      NRF_ERROR_NOT_SUPPORTED  �������û�а�ť������ BSP_SIMPLE��
 */
uint32_t bsp_wakeup_button_disable(uint32_t button_idx);


#ifdef __cplusplus
}
#endif

#endif // BSP_H__

/** @} */
