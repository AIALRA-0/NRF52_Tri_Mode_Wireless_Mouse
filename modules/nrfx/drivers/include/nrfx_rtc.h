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

#ifndef NRFX_RTC_H__
#define NRFX_RTC_H__

#include <nrfx.h>
#include <hal/nrf_rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_rtc RTC ����
 * @{
 * @ingroup nrf_rtc
 * @brief   ʵʱʱ�ӣ�RTC������������
 */

/** @brief ��΢��ת��Ϊ��ʱ���̶ȵĺꡣ */
#define NRFX_RTC_US_TO_TICKS(us,freq) (((us) * (freq)) / 1000000U)

/** @brief RTC �����ж����͡� */
typedef enum
{
    NRFX_RTC_INT_COMPARE0 = 0, /**< ���� COMPARE0 �¼����жϡ� */
    NRFX_RTC_INT_COMPARE1 = 1, /**< ���� COMPARE1 �¼����жϡ� */
    NRFX_RTC_INT_COMPARE2 = 2, /**< ���� COMPARE2 �¼����жϡ� */
    NRFX_RTC_INT_COMPARE3 = 3, /**< ���� COMPARE3 �¼����жϡ� */
    NRFX_RTC_INT_TICK     = 4, /**< ���� TICK �¼����жϡ� */
    NRFX_RTC_INT_OVERFLOW = 5  /**< ���������OVERFLOW���¼����жϡ� */
} nrfx_rtc_int_type_t;

/** @brief RTC ����ʵ���ṹ�塣 */
typedef struct
{
    NRF_RTC_Type  * p_reg;            /**< ʵ���Ĵ�������ָ�롣 */
    IRQn_Type       irq;              /**< ʵ�� IRQ ID�� */
    uint8_t         instance_id;      /**< ����ʵ���������������ڲ�ʹ�á� */
    uint8_t         cc_channel_count; /**< ����/�Ƚ�ͨ�������� */
} nrfx_rtc_t;

/** @brief ���� RTC ����ʵ���ĺꡣ */
#define NRFX_RTC_INSTANCE(id)                                   \
{                                                               \
    .p_reg            = NRFX_CONCAT_2(NRF_RTC, id),             \
    .irq              = NRFX_CONCAT_3(RTC, id, _IRQn),          \
    .instance_id      = NRFX_CONCAT_3(NRFX_RTC, id, _INST_IDX), \
    .cc_channel_count = NRF_RTC_CC_CHANNEL_COUNT(id),           \
}

#ifndef __NRFX_DOXYGEN__
enum {
#if NRFX_CHECK(NRFX_RTC0_ENABLED)
    NRFX_RTC0_INST_IDX,
#endif
#if NRFX_CHECK(NRFX_RTC1_ENABLED)
    NRFX_RTC1_INST_IDX,
#endif
#if NRFX_CHECK(NRFX_RTC2_ENABLED)
    NRFX_RTC2_INST_IDX,
#endif
    NRFX_RTC_ENABLED_COUNT
};
#endif

/** @brief RTC ����ʵ�����ýṹ�塣 */
typedef struct
{
    uint16_t prescaler;          /**< Ԥ��Ƶ���� */
    uint8_t  interrupt_priority; /**< �ж����ȼ��� */
    uint8_t  tick_latency;       /**< �жϴ����������ʱ�����Կ̶�Ϊ��λ�����Ϊ 7.7 ms���� */
    bool     reliable;           /**< �ɿ�ģʽ��־�� */
} nrfx_rtc_config_t;

/** @brief RTC ʵ��Ĭ�����á� */
#define NRFX_RTC_DEFAULT_CONFIG                                                     \
{                                                                                   \
    .prescaler          = RTC_FREQ_TO_PRESCALER(NRFX_RTC_DEFAULT_CONFIG_FREQUENCY), \
    .interrupt_priority = NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY,                     \
    .tick_latency       = NRFX_RTC_US_TO_TICKS(NRFX_RTC_MAXIMUM_LATENCY_US,         \
                                               NRFX_RTC_DEFAULT_CONFIG_FREQUENCY),  \
    .reliable           = NRFX_RTC_DEFAULT_CONFIG_RELIABLE,                         \
}

/** @brief RTC ����ʵ������������͡� */
typedef void (*nrfx_rtc_handler_t)(nrfx_rtc_int_type_t int_type);

/**
 * @brief ��ʼ�� RTC ����ʵ���ĺ�����
 *
 * ��ʼ����ʵ�����ڹر�״̬��
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 * @param[in] p_config   ��ʼ���ýṹ���ָ�롣
 * @param[in] handler    �û��ṩ���¼��������
 *                       ����Ϊ NULL��
 *
 * @retval NRFX_SUCCESS             ��ʼ���ɹ���
 * @retval NRFX_ERROR_INVALID_STATE ʵ���ѱ���ʼ����
 */
nrfx_err_t nrfx_rtc_init(nrfx_rtc_t const * const  p_instance,
                         nrfx_rtc_config_t const * p_config,
                         nrfx_rtc_handler_t        handler);

/**
 * @brief ȡ����ʼ�� RTC ����ʵ���ĺ�����
 *
 * ȡ����ʼ����ʵ�����ڿ���״̬��Ӳ��Ӧ�ָ�����ʼ��֮ǰ��״̬��
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 */
void nrfx_rtc_uninit(nrfx_rtc_t const * const p_instance);

/**
 * @brief ���� RTC ����ʵ���ĺ�����
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 */
void nrfx_rtc_enable(nrfx_rtc_t const * const p_instance);

/**
 * @brief ���� RTC ����ʵ���ĺ�����
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 */
void nrfx_rtc_disable(nrfx_rtc_t const * const p_instance);

/**
 * @brief ���ñȽ�ͨ���ĺ�����
 *
 * ���ʵ�����ڹر�״̬���ú���������ʵ����
 *
 * ���������� RTC ʱ�������ٽ���������ζ�������Ա���ռһ��ʱ�䡣
 * �����������ռ�������õ�ֵ�ڶ�ʱ���ڣ��������ܻ�����һ���Ѿ���ʱ�ıȽ�ֵ��
 * ���Ϊָ��ʵ�������˿ɿ�ģʽ��������÷��ա�
 * Ȼ����Ϊ�˼�������ֵ�Ƿ��Ѿ���ʱ����ģʽ�������¼��裺
 *  - ������ռʱ�䣨�Կ̶�Ϊ��λ��8 λֵ����֪��С�� 7.7 ���루�� prescaler = 0 �� RTC Ƶ��Ϊ 32 kHz ʱ����
 *  - ����ľ��ԱȽ�ֵ������ (0x00FFFFFF)-tick_latency���û�����ȷ����һ�㡣
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 * @param[in] channel    ʵ����һ��ͨ����
 * @param[in] val        Ҫ���õıȽϼĴ����ľ���ֵ��
 * @param[in] enable_irq True ��ʾ�����жϣ�False ��ʾ�����жϡ�
 *
 * @retval NRFX_SUCCESS       �����ɹ���
 * @retval NRFX_ERROR_TIMEOUT �Ƚ�δ���ã���Ϊ�����ֵ�ȵ�ǰ������ֵС��ֻ���������˿ɿ�ģʽʱ���ſ��ܱ���˴���
 */
nrfx_err_t nrfx_rtc_cc_set(nrfx_rtc_t const * const p_instance,
                           uint32_t                 channel,
                           uint32_t                 val,
                           bool                     enable_irq);

/**
 * @brief ����ͨ���ĺ�����
 *
 * �˺�������ͨ���¼���ͨ���жϡ�
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 * @param[in] channel    ʵ����һ��ͨ����
 *
 * @retval NRFX_SUCCESS       �����ɹ���
 * @retval NRFX_ERROR_TIMEOUT �����ͨ�����й�����жϡ�
 */
nrfx_err_t nrfx_rtc_cc_disable(nrfx_rtc_t const * const p_instance, uint32_t channel);

/**
 * @brief ���� TICK �¼��ĺ�����
 *
 * �˺������� tick �¼�����ѡ�������жϡ�
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 * @param[in] enable_irq True ��ʾ�����жϣ�False ��ʾ�����жϡ�
 */
void nrfx_rtc_tick_enable(nrfx_rtc_t const * const p_instance, bool enable_irq);

/**
 * @brief ���� TICK �¼��ĺ�����
 *
 * �˺������� TICK �¼����жϡ�
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 */
void nrfx_rtc_tick_disable(nrfx_rtc_t const * const p_instance);

/**
 * @brief ��������ĺ�����
 *
 * �˺�����������¼�����ѡ�������жϡ�
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 * @param[in] enable_irq True ��ʾ�����жϣ�False ��ʾ�����жϡ�
 */
void nrfx_rtc_overflow_enable(nrfx_rtc_t const * const p_instance, bool enable_irq);

/**
 * @brief ��������ĺ�����
 *
 * �˺�����������¼����жϡ�
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 */
void nrfx_rtc_overflow_disable(nrfx_rtc_t const * const p_instance);

/**
 * @brief ��ȡ�����ڱȽ�ͨ�������õ������Կ̶�ֵ�ĺ�����
 *
 * ��ʹ����ռ�ø����ȼ��жϵĶ�ջ������ SoftDevice��ʱ��Ӧ�ó��������ܻ���ʱ���ж�һ��ʱ�䡣
 * ��������˿ɿ�ģʽ������������ӳ٣�����ֵ��С�� RTC �������ķֱ��ʡ�
 * ���δ���ÿɿ�ģʽ������ֵ���ڼ������ֱ��ʡ�
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 *
 * @return ���Ŀ̶�ֵ��
 */
uint32_t nrfx_rtc_max_ticks_get(nrfx_rtc_t const * const p_instance);

/**
 * @brief ��������ʵ���жϵĺ�����
  *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 * @param[in] p_mask     ָ�򱣴������λ�õ�ָ�롣
 */
__STATIC_INLINE void nrfx_rtc_int_disable(nrfx_rtc_t const * const p_instance,
                                          uint32_t               * p_mask);

/**
 * @brief ����ʵ���жϵĺ�����
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 * @param[in] mask       Ҫ���õ��ж����롣
 */
__STATIC_INLINE void nrfx_rtc_int_enable(nrfx_rtc_t const * const p_instance, uint32_t mask);

/**
 * @brief ��ȡ��ǰ������ֵ�ĺ�����
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 *
 * @return ������ֵ��
 */
__STATIC_INLINE uint32_t nrfx_rtc_counter_get(nrfx_rtc_t const * const p_instance);

/**
 * @brief ���������ֵ�ĺ�����
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 */
__STATIC_INLINE void nrfx_rtc_counter_clear(nrfx_rtc_t const * const p_instance);

/**
 * @brief ���� RTC ����ʵ�������������ַ�ĺ�����
 *
 * �����ַ������ PPI ģ��ʹ�á�
 *
 * @param[in] p_instance ʵ����ָ�롣
 * @param[in] task       ��������֮һ��
 *
 * @return ����Ĵ����ĵ�ַ��
 */
__STATIC_INLINE uint32_t nrfx_rtc_task_address_get(nrfx_rtc_t const * const p_instance,
                                                   nrf_rtc_task_t           task);

/**
 * @brief ���� RTC ����ʵ���������¼���ַ�ĺ�����
 *
 * �¼���ַ������ PPI ģ��ʹ�á�
 *
 * @param[in] p_instance ����ʵ���ṹ���ָ�롣
 * @param[in] event      �����¼�֮һ��
 *
 * @return �¼��Ĵ����ĵ�ַ��
 */
__STATIC_INLINE uint32_t nrfx_rtc_event_address_get(nrfx_rtc_t const * const p_instance,
                                                    nrf_rtc_event_t          event);

#ifndef SUPPRESS_INLINE_IMPLEMENTATION

/**
 * @brief ���� RTC ʵ�����жϡ�
 *
 * �˺���������ʵ���� TICK������Լ����бȽ�ͨ�����жϡ�
 *
 * @param[in] p_instance RTC ����ʵ����ָ�롣
 * @param[out] p_mask    ���ڱ��浱ǰ�����жϵ����롣
 */
__STATIC_INLINE void nrfx_rtc_int_disable(nrfx_rtc_t const * const p_instance,
                                          uint32_t               * p_mask)
{
    *p_mask = nrf_rtc_int_get(p_instance->p_reg);
    nrf_rtc_int_disable(p_instance->p_reg, NRF_RTC_INT_TICK_MASK |
                                           NRF_RTC_INT_OVERFLOW_MASK |
                                           NRF_RTC_INT_COMPARE0_MASK |
                                           NRF_RTC_INT_COMPARE1_MASK |
                                           NRF_RTC_INT_COMPARE2_MASK |
                                           NRF_RTC_INT_COMPARE3_MASK);
}

/**
 * @brief ���� RTC ʵ�����жϡ�
 *
 * �˺������ݴ��������������Ӧ���жϡ�
 *
 * @param[in] p_instance RTC ����ʵ����ָ�롣
 * @param[in] mask       Ҫ���õ��жϵ����롣
 */
__STATIC_INLINE void nrfx_rtc_int_enable(nrfx_rtc_t const * const p_instance, uint32_t mask)
{
    nrf_rtc_int_enable(p_instance->p_reg, mask);
}

/**
 * @brief ��ȡ RTC ʵ���ĵ�ǰ������ֵ��
 *
 * �˺������ص�ǰ RTC ��������ֵ��
 *
 * @param[in] p_instance RTC ����ʵ����ָ�롣
 *
 * @return ��ǰ��������ֵ��
 */
__STATIC_INLINE uint32_t nrfx_rtc_counter_get(nrfx_rtc_t const * const p_instance)
{
    return nrf_rtc_counter_get(p_instance->p_reg);
}

/**
 * @brief ��� RTC ʵ���ļ�����ֵ��
 *
 * �˺��������� RTC ���������������������������λΪ 0��
 *
 * @param[in] p_instance RTC ����ʵ����ָ�롣
 */
__STATIC_INLINE void nrfx_rtc_counter_clear(nrfx_rtc_t const * const p_instance)
{
    nrf_rtc_task_trigger(p_instance->p_reg, NRF_RTC_TASK_CLEAR);
}

/**
 * @brief ��ȡ RTC ʵ���������ַ��
 *
 * �˺������� RTC ʵ����ָ������ļĴ�����ַ�������� PPI ģ�顣
 *
 * @param[in] p_instance RTC ����ʵ����ָ�롣
 * @param[in] task       RTC ��������֮һ��
 *
 * @return ָ������ļĴ�����ַ��
 */
__STATIC_INLINE uint32_t nrfx_rtc_task_address_get(nrfx_rtc_t const * const p_instance,
                                                   nrf_rtc_task_t           task)
{
    return nrf_rtc_task_address_get(p_instance->p_reg, task);
}

/**
 * @brief ��ȡ RTC ʵ�����¼���ַ��
 *
 * �˺������� RTC ʵ����ָ���¼��ļĴ�����ַ�������� PPI ģ�顣
 *
 * @param[in] p_instance RTC ����ʵ����ָ�롣
 * @param[in] event      RTC �����¼�֮һ��
 *
 * @return ָ���¼��ļĴ�����ַ��
 */
__STATIC_INLINE uint32_t nrfx_rtc_event_address_get(nrfx_rtc_t const * const p_instance,
                                                    nrf_rtc_event_t          event)
{
    return nrf_rtc_event_address_get(p_instance->p_reg, event);
}

#endif // SUPPRESS_INLINE_IMPLEMENTATION

/** @} */

/**
 * @brief RTC 0 �жϴ�������
 *
 * ������ RTC 0 ʵ����ص��жϡ�
 */
void nrfx_rtc_0_irq_handler(void);

/**
 * @brief RTC 1 �жϴ�������
 *
 * ������ RTC 1 ʵ����ص��жϡ�
 */
void nrfx_rtc_1_irq_handler(void);

/**
 * @brief RTC 2 �жϴ�������
 *
 * ������ RTC 2 ʵ����ص��жϡ�
 */
void nrfx_rtc_2_irq_handler(void);



#ifdef __cplusplus
}
#endif

#endif // NRFX_RTC_H__
