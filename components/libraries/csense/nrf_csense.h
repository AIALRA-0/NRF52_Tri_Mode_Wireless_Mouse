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
#ifndef NRF_CSENSE_H__
#define NRF_CSENSE_H__

#include <stdint.h>
#include "nrf.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "nrf_drv_csense.h"
#include "nrf_csense_macros.h"
#include "app_util.h"

/** @file
 *
 * @defgroup nrf_csense ���ݴ�������
 * @{
 * @ingroup app_common
 *
 * @brief ��ģ������ʹ�õ��ݴ������⣬֧�ֶ�����顢��ת�ֺͰ�ťʵ����
 */

/**
 * @brief ���ر�����ַ�ĺꡣ
 */
#define NRF_CSENSE_GET_INSTANCE_ID(instance) (&instance)

/**
 * @brief ��̬������ݴ�����ʵ�����ڴ档
 *
 * @param[in,out] name          ��Ҫ�����ĵ��ݴ�����ʵ�������ơ�
 * @param[in] p1                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 */
#define NRF_CSENSE_BUTTON_DEF(name, p1) NRF_CSENSE_INTERNAL_BUTTON_DEF(name, p1)

/**
 * @brief ���ڴ���һ��2������Ļ���ʵ���ĺꡣ
 *
 * @param[in,out] name          ��Ҫ�����ĵ��ݴ�����ʵ�������ơ�
 * @param[in] steps_no          ��Դ�������������ζ���ڻ��鴦������У��û��齫���� (1, steps_no) ֮���ֵ��
 * @param[in] p1                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p2                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 */
#define NRF_CSENSE_SLIDER_2_DEF(name, steps_no, p1, p2) NRF_CSENSE_INTERNAL_SLIDER_2_DEF(name, steps_no, p1, p2)

/**
 * @brief ���ڴ���һ��3������Ļ���ʵ���ĺꡣ
 *
 * @param[in,out] name          ��Ҫ�����ĵ��ݴ�����ʵ�������ơ�
 * @param[in] steps_no          ��Դ�������������ζ���ڻ��鴦������У��û��齫���� (1, steps_no) ֮���ֵ��
 * @param[in] p1                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p2                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p3                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 */
#define NRF_CSENSE_SLIDER_3_DEF(name, steps_no, p1, p2, p3) NRF_CSENSE_INTERNAL_SLIDER_3_DEF(name, steps_no, p1, p2, p3)

/**
 * @brief ���ڴ���һ��4������Ļ���ʵ���ĺꡣ
 *
 * @param[in,out] name          ��Ҫ�����ĵ��ݴ�����ʵ�������ơ�
 * @param[in] steps_no          ��Դ�������������ζ���ڻ��鴦������У��û��齫���� (1, steps_no) ֮���ֵ��
 * @param[in] p1                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p2                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p3                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p4                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 */
#define NRF_CSENSE_SLIDER_4_DEF(name, steps_no, p1, p2, p3, p4) NRF_CSENSE_INTERNAL_SLIDER_4_DEF(name, steps_no, p1, p2, p3, p4)

/**
 * @brief ���ڴ���һ��5������Ļ���ʵ���ĺꡣ
 *
 * @param[in,out] name          ��Ҫ�����ĵ��ݴ�����ʵ�������ơ�
 * @param[in] steps_no          ��Դ�������������ζ���ڻ��鴦������У��û��齫���� (1, steps_no) ֮���ֵ��
 * @param[in] p1                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p2                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p3                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p4                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p5                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 */
#define NRF_CSENSE_SLIDER_5_DEF(name, steps_no, p1, p2, p3, p4, p5) NRF_CSENSE_INTERNAL_SLIDER_5_DEF(name, steps_no, p1, p2, p3, p4, p5)

/**
 * @brief ���ڴ���һ��3���������ת��ʵ���ĺꡣ
 *
 * @param[in,out] name          ��Ҫ�����ĵ��ݴ�����ʵ�������ơ�
 * @param[in] steps_no          ��Դ�������������ζ���ڻ��鴦������У����ֽ����� (1, steps_no) ֮���ֵ��
 * @param[in] p1                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p2                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p3                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 */
#define NRF_CSENSE_WHEEL_3_DEF(name, steps_no, p1, p2, p3) NRF_CSENSE_INTERNAL_WHEEL_3_DEF(name, steps_no, p1, p2, p3)

/**
 * @brief ���ڴ���һ��4���������ת��ʵ���ĺꡣ
 *
 * @param[in,out] name          ��Ҫ�����ĵ��ݴ�����ʵ�������ơ�
 * @param[in] steps_no          ��Դ�������������ζ���ڻ��鴦������У����ֽ����� (1, steps_no) ֮���ֵ��
 * @param[in] p1                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p2                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p3                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p4                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 */
#define NRF_CSENSE_WHEEL_4_DEF(name, steps_no, p1, p2, p3, p4) NRF_CSENSE_INTERNAL_WHEEL_4_DEF(name, steps_no, p1, p2, p3, p4)

/**
 * @brief ���ڴ���һ��5���������ת��ʵ���ĺꡣ
 *
 * @param[in,out] name          ��Ҫ�����ĵ��ݴ�����ʵ�������ơ�
 * @param[in] steps_no          ��Դ�������������ζ���ڻ��鴦������У����ֽ����� (1, steps_no) ֮���ֵ��
 * @param[in] p1                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p2                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p3                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p4                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 * @param[in] p5                ������������ϣ���ֵ��ģ�������š�������(analog_input_number, threshold)��ʽ���ݡ�
 */
#define NRF_CSENSE_WHEEL_5_DEF(name, steps_no, p1, p2, p3, p4, p5) NRF_CSENSE_INTERNAL_WHEEL_5_DEF(name, steps_no, p1, p2, p3, p4, p5)

/**
 * @cond (NODOX)
 * @defgroup nrf_csense_internal �����ڲ���������
 * @brief �ڲ�ʹ��ģ�鶨�壬���޿��ڲ�ʹ�á�
 * @details ��Щ������û��ɼ�����������ֱ�ӷ������ǡ���ʹ��API���з��ʡ�
 * @{
 *
 */

    /*
     * @brief ���ݴ�����ʵ����ǰ��������
     */
    typedef struct nrf_csense_instance_s nrf_csense_instance_t;

    /*
     * @brief ���ݴ����������ǰ��������
     */
    typedef struct nrf_csense_pad_s nrf_csense_pad_t;

    /**
     * @brief ���������Ĵ�����Сֵ�����ֵ�Ľṹ�塣
     */
    typedef struct
    {
        uint16_t                   max_value;                //!< �����ϲ����������ֵ��
        uint16_t                   min_value;                //!< �����ϲ���������Сֵ��
    }nrf_csense_min_max_t;

    /**
     * @brief ��������ʵ�������Ľṹ�塣��������һ�������ť��
     */
    struct nrf_csense_instance_s
    {
        nrf_csense_instance_t *     p_next_instance;            //!< ָ����һ��ʵ����ָ�롣
        nrf_csense_pad_t *          p_nrf_csense_pad;           //!< ָ���ģ���һ�������ָ�롣
        nrf_csense_min_max_t *      min_max;                    //!< ���������ϲ���������С�����ֵ�Ľṹ��ָ�롣
        uint16_t                    steps;                      //!< ��Դ����������ڻ��鴦������У����齫���� (1, steps_no) ֮���ֵ��
        uint8_t                     number_of_pads;             //!< ʵ��ʹ�õĴ���������
        bool                        is_active;                  //!< ָʾʵ���Ƿ񼤻�ı�־��
        bool                        is_touched;                 //!< ָʾʵ���Ƿ񱻴����ı�־��
        void *                      p_context;                  //!< ͨ��ָ�룬���ڴ洢���������ݡ�
    };

    /* ��ʼ��ʱʹ�õĵ�����������ṹ�塣 */
    struct nrf_csense_pad_s
    {
        nrf_csense_pad_t *          p_next_pad;                 //!< ָ����һ�������ָ�롣
        uint16_t                    threshold;                  //!< �����ϵ���ֵ��ѹ/���ʱ�䣬��ȷ���Ƿ����˴��㡣
        uint8_t                     pad_index;                  //!< ����������š�
        uint8_t                     analog_input_number;        //!< �봥�����ӵ�ģ�������š�
    };

/** @}
 * @endcond
 */

/**
 * @brief ���ݴ������¼�����ö�١�
 */
typedef enum
{
    NRF_CSENSE_BTN_EVT_PRESSED,                                 //!< �������㰴���¼���
    NRF_CSENSE_BTN_EVT_RELEASED,                                //!< ���������ͷ��¼���
    NRF_CSENSE_SLIDER_EVT_PRESSED,                              //!< �������鰴���¼���
    NRF_CSENSE_SLIDER_EVT_RELEASED,                             //!< ���������ͷ��¼���
    NRF_CSENSE_SLIDER_EVT_DRAGGED,                              //!< ���������϶��¼���
}nrf_csense_evt_type_t;

/**
 * @brief ���������¼����ݣ������������Ĳ���ֵ���Ľṹ�塣
 */
typedef struct
{
    uint16_t step;                                              //!< �������Ĳ���ֵ��
} nrf_csense_slider_evt_t;

/**
 * @brief ���ݴ������¼������������塣
 */
typedef union
{
    nrf_csense_slider_evt_t slider;                             //!< ���������¼����ݣ������������Ĳ���ֵ���Ľṹ�塣
} nrf_csense_evt_param_t;

/**
 * @brief �����¼������Ľṹ�塣
 */
typedef struct
{
    nrf_csense_evt_type_t       nrf_csense_evt_type;            //!< �¼����͡�
    nrf_csense_instance_t *     p_instance;                     //!< ָ��ʵ����ָ�롣
    nrf_csense_evt_param_t      params;                         //!< ���ݴ������¼������������塣
}nrf_csense_evt_t;

/**
 * @brief ���ݴ������¼����������͡�
 */
typedef void (* nrf_csense_event_handler_t)(nrf_csense_evt_t * p_evt);

/**
 * @brief ����ʵ���������ĺ�����
 *
 * @param [in] p_instance                           ָ��Ҫ�޸Ĳ���ֵ��ʵ����ָ�롣
 * @param [in] p_context                            ͨ��ָ�롣�����ڻص������д��ݡ�
 */
__STATIC_INLINE void nrf_csense_instance_context_set(nrf_csense_instance_t * const p_instance, void * p_context)
{
    p_instance->p_context = p_context;
}

/**
 * @brief ��ʼ��ģ��ĺ�������ʼ����û���κ�ʵ�������á�
 *
 * @param [in] event_handler                          ���ݴ�����ģ����¼���������
 * @param [in] ticks                                  ����һ��ת��֮���Ӧ�ó���ʱ���ĵδ�����
 *
 * @retval NRF_ERROR_INVALID_PARAM                    ����ṩ����Ч������
 * @retval NRF_ERROR_INVALID_STATE                    ���ʹ�õ�ģ�鴦����Ч״̬��
 * @retval NRF_ERROR_INTERNAL                         ����ڳ�ʼ�����ݴ�������ʹ�õ�ģ��֮һʱ�����˴���
 * @retval NRF_SUCCESS                                ���ģ���ʼ���ɹ���
 */
ret_code_t nrf_csense_init(nrf_csense_event_handler_t event_handler, uint32_t ticks);

/**
 * @brief ����ʼ��ģ��ĺ�����
 *
 * @return @ref nrf_drv_csense_uninit �� @ref app_timer_stop ���ص�ֵ��
 */
ret_code_t nrf_csense_uninit(void);

/**
 * @brief �����ݴ�����ʵ����ӵ�����ĺ�����
 *
 * �ú������� @ref nrf_csense_enable ������ӵ������е�ʵ����
 *
 * @param [in] p_instance                            ָ����ݴ�����ʵ����ָ�롣������ģ�鱣�棬�������ø�ʵ��ʱʹ�á�
 *
 * @return @ref nrf_csense_enable ���ص�ֵ��
 */
ret_code_t nrf_csense_add(nrf_csense_instance_t * const p_instance);

/**
 * @brief ���õ���ʵ���ĺ�����
 *
 * @param [in,out] p_instance                        ָ����ݴ�����ʵ����ָ�롣������ģ�鱣�棬�������ø�ʵ��ʱʹ�á�
 *
 * @return @ref app_timer_start ���ص�ֵ��
 */
ret_code_t nrf_csense_enable(nrf_csense_instance_t * const p_instance);

/**
 * @brief ����ʵ���ĺ�����
 *
 * @param [in] p_instance                              ָ��Ҫ���õ�ʵ����ָ�롣
 *
 * @retval NRF_ERROR_INVALID_PARAM                     ���ʵ���Ѿ������á�
 * @retval NRF_SUCCESS                                 ���ʵ���ɹ����á�
 */
ret_code_t nrf_csense_disable(nrf_csense_instance_t * const p_instance);

/**
 * @brief ������һ�β���֮��ĵδ����ĺ�����
 *
 * @param [in] ticks                                 Ӧ�ó���ʱ���ĵδ�����
 *
 * @retval NRF_ERROR_BUSY                             ������ݴ�������æ��
 * @retval NRF_ERROR_INVALID_PARAM                    ����ṩ����Ч�Ĳ�����
 * @retval NRF_ERROR_INVALID_STATE                    ��� app_timer ������Ч״̬��
 * @retval NRF_SUCCESS                                ����δ����ɹ����á�
 */
ret_code_t nrf_csense_ticks_set(uint32_t ticks);

/**
 * @brief ����ʵ������ֵ�ĺ�����
 *
 * ע�⣺�ڸ���ʵ���Ĳ���ֵ֮ǰ��������ø�ʵ����
 *
 * @param [in] p_instance                           ָ��Ҫ�޸Ĳ���ֵ��ʵ����ָ�롣
 * @param [in] steps                                �µĲ���ֵ��
 *
 * @retval NRF_ERROR_BUSY                           ������ݴ�������æ��
 * @retval NRF_SUCCESS                              �������ֵ���óɹ���
 */
ret_code_t nrf_csense_steps_set(nrf_csense_instance_t  * const p_instance, uint16_t steps);


/** @} */

#endif //NRF_CSENSE_H__
