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

#ifndef NRFX_SYSTICK_H__
#define NRFX_SYSTICK_H__

#include <nrfx.h>
#include <hal/nrf_systick.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_systick ARM(R) SysTick ��������
 * @{
 * @ingroup nrf_systick
 *
 * @brief   ARM(R) SysTick ��������
 *
 * ���������� ARM(R) SysTick ����Ϊ�������еĶ�ʱ����
 * �ö�ʱ�����������ӳٺ���ѯ��ʱ��
 * ��֧����Խ϶̵ĳ�ʱ��
 * SysTick �� 64MHz ���У������� 24 λ��
 * ����ζ������Լÿ����� 4 �Σ�����֧�ֵ��ʱ��ԼΪ 250 ΢�롣
 * ���ڲ�ʹ���жϺ��Ѽ���Ƿ�����������
 * ��˳��ڰ�ȫԭ������ӳٷ�Χ�����롣
 */

/**
 * @brief ���� SysTick ״̬��ֵ���͡�
 *
 * �˱������ڼ�������ĳ�ʱ��
 * @sa nrfx_systick_get
 */
typedef struct {
    uint32_t time; //!< ��¼��ʱ��ֵ��
} nrfx_systick_state_t;

/**
 * @brief ���ò�������ʱ���ĺ�����
 *
 * �ú����� SysTick ����Ϊ���жϵ��������ж�ʱ����
 */
void nrfx_systick_init(void);

/**
 * @brief ��ȡ��ǰ SysTick ״̬�ĺ�����
 *
 * �ú�����ȡ SysTick ��ʱ���ĵ�ǰ״̬��
 * ������ͨ�� @ref nrfx_systick_test ��鳬ʱ��
 *
 * @param[out] p_state Ҫ����״̬����ָ�롣
 */
void nrfx_systick_get(nrfx_systick_state_t * p_state);

/**
 * @brief ���Ե�ǰʱ���Ƿ���ڼ�¼��״̬�ĺ�����
 *
 * @param[in] p_state �� @ref nrfx_systick_get ���õļ�¼״̬��
 * @param[in] us      ����ĳ�ʱʱ�䡣
 *
 * @retval true  ��ǰʱ�����ָ��״̬���ϸ�����ʱʱ�䡣
 * @retval false ��ǰʱ��С��ָ��״̬���ϸ�����ʱʱ�䡣
 */
bool nrfx_systick_test(nrfx_systick_state_t const * p_state, uint32_t us);

/**
 * @brief ����ִ��ָ�� CPU ʱ���������ĺ�����
 *
 * @param[in] ticks ����ִ�е� CPU ʱ����������
 */
void nrfx_systick_delay_ticks(uint32_t ticks);

/**
 * @brief ����ִ��ָ��΢�����ĺ�����
 *
 * @param[in] us ����ִ�е�΢������
 */
void nrfx_systick_delay_us(uint32_t us);

/**
 * @brief ����ִ��ָ���������ĺ�����
 *
 * ���ӳٺ���ȥ�����������ӳ�ֵ�����ơ�
 *
 * @param[in] ms ����ִ�еĺ�������
 */
void nrfx_systick_delay_ms(uint32_t ms);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRFX_SYSTICK_H__ */

