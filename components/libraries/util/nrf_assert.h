/**
 * Copyright (c) 2006 - 2021, Nordic Semiconductor ASA
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
 * @brief Utilities for verifying program logic
 */

#ifndef NRF_ASSERT_H_
#define NRF_ASSERT_H_

// �����������ͷ�ļ���NordicоƬ��ص�ͷ�ļ�
#include <stdint.h>
#include "nrf.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief ���ڴ�����Եĺ�����
 *
 * ����������ʱ�������ô˺�����
 * 
 * @note
 * �˺�������δ���汾�б��Ƴ����Ѳ����Ƽ�ʹ�á�
 * �Ƽ�ʹ�� `app_error_fault_handler` ����˺�����
 *
 * @post
 * ���������Ժ�����Ӳ������������״̬�����������ߵ磨Radio����Ҫ�رա�
 * ������Ϊ���ߵ���ܴ���ĳ��״̬��������ִ������ѭ����ASSERTʱ���ܻ�����������ݰ���
 *
 * @param line_num  �������Ե��кš�
 * @param file_name �������Ե��ļ���ָ�롣
 */
// ʹ��lint���߱��浱ǰ״̬�������ض����棨���Է���14���棩
void assert_nrf_callback(uint16_t line_num, const uint8_t *file_name);
// �ָ�lint���ߵ�״̬
// lint -save -esym(14, assert_nrf_callback)
// lint -restore

// ���������DEBUG_NRF����DEBUG_NRF_USER�������ö��Թ���
#if (defined(DEBUG_NRF) || defined(DEBUG_NRF_USER))
#define NRF_ASSERT_PRESENT 1  // ���Թ�����Ч
#else
#define NRF_ASSERT_PRESENT 0  // ���Թ�����Ч
#endif

/**
 * @brief ���Ժ꣬���ڼ����ʽ�Ƿ�Ϊtrue��
 * 
 * ������ʽΪfalse���򴥷����Դ������ `assert_nrf_callback`��
 * 
 * @param expr ��Ҫ���ı��ʽ��
 */

// ���ʹ����lint���ߣ�����򻯵Ķ����߼���
// ������ʽΪfalse��������һ������ѭ����
#ifdef _lint
#define ASSERT(expr)                                                          \
if (expr)                                                                     \
{                                                                             \
}                                                                             \
else                                                                          \
{                                                                             \
    while (1);                                                                \
}
#else //_lint
// ���ڷ�lint�������ʹ�ñ�׼�Ķ����߼���
// ���NRF_ASSERT_PRESENT������Ϊ1����ʾ���Թ�����Ч����������ʽ��
// ������ʽΪfalse������� `assert_nrf_callback`�����ݵ�ǰ�кź��ļ�����
// ���򣬲����κβ�����
#define ASSERT(expr)                                                          \
if (NRF_ASSERT_PRESENT)                                                       \
{                                                                             \
    if (expr)                                                                 \
    {                                                                         \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        assert_nrf_callback((uint16_t)__LINE__, (uint8_t *)__FILE__);         \
    }                                                                         \
}
#endif // _lint

#ifdef __cplusplus
}
#endif

#endif /* NRF_ASSERT_H_ */

