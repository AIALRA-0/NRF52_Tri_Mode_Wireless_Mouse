/**
 * Copyright (c) 2011 - 2021, Nordic Semiconductor ASA
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

#ifndef _NRF_DELAY_H
#define _NRF_DELAY_H

#include <nrfx.h>  // ����nrfx�⣬�ṩ�ײ�Ӳ������

#ifdef __cplusplus
extern "C" {  // �����C++��������ʹ��C���Եķ�����������
#endif

/**
 * @brief �ӳ�ָ����΢�����ĺ�����
 *
 * �˺������ڽ�����ִ���ӳ�ָ����΢��������Ӳ������У�������Ҫ���ݵĵȴ�ʱ�䣬�Եȴ�ĳЩӲ��������ɡ�
 *
 * @param us_time Ҫ�ȴ���΢�������ò���ָ���ӳٵ�ʱ�䣬��λΪ΢�롣
 */
#define nrf_delay_us(us_time) NRFX_DELAY_US(us_time)  // ���õײ��NRFX_DELAY_US�꣬����΢�뼶�ӳ�

/**
 * @brief �ӳ�ָ���ĺ������ĺ�����
 *
 * �˺������ڽ�����ִ���ӳ�ָ���ĺ�������ͨ��ѭ������nrf_delay_us������ʵ�ֺ��뼶����ӳ١�
 *
 * @param ms_time Ҫ�ȴ��ĺ��������ò���ָ���ӳٵ�ʱ�䣬��λΪ���롣
 */
__STATIC_INLINE void nrf_delay_ms(uint32_t ms_time)
{
    if (ms_time == 0)  // ����ӳ�ʱ��Ϊ0���룬ֱ�ӷ��أ������ӳ�
    {
        return;
    }

    do {
        nrf_delay_us(1000);  // ÿ���ӳ�1���룬ͨ������nrf_delay_us(1000)ʵ��
    } while (--ms_time);  // ѭ������ms_time��ֱ��ms_timeΪ0����ζ���ӳ�ʱ�����
}

#ifdef __cplusplus
}
#endif

#endif  // _NRF_DELAY_H

