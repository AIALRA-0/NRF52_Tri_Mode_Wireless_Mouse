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

#ifndef NRFX_ERRORS_H__
#define NRFX_ERRORS_H__

#if !NRFX_CHECK(NRFX_CUSTOM_ERROR_CODES) // ���û�������Զ��������룬��ʹ��Ĭ�ϵĴ�����붨��

/**
 * @defgroup nrfx_error_codes ȫ�ִ������
 * @{
 * @ingroup nrfx
 *
 * @brief ȫ�ִ�����붨�塣
 */

/** @brief �������Ļ���ַ�����е�ȫ�ִ�����붼���������ַ��ʼ */
#define NRFX_ERROR_BASE_NUM         0x0BAD0000

/** @brief ��������������Ļ���ַ�����е��������������붼���������ַ��ʼ */
#define NRFX_ERROR_DRIVERS_BASE_NUM (NRFX_ERROR_BASE_NUM + 0x10000)

/** @brief ��������ö�����ͣ��������ͨ����Щ����������ʶ��ͬ���͵Ĵ��󣬷����ڲ�ͬ�����½��д�����͵��� */
typedef enum {
    NRFX_SUCCESS                    = (NRFX_ERROR_BASE_NUM + 0),  ///< �����ɹ�ִ�С�
    NRFX_ERROR_INTERNAL             = (NRFX_ERROR_BASE_NUM + 1),  ///< �ڲ�����
    NRFX_ERROR_NO_MEM               = (NRFX_ERROR_BASE_NUM + 2),  ///< ����û���㹻���ڴ档
    NRFX_ERROR_NOT_SUPPORTED        = (NRFX_ERROR_BASE_NUM + 3),  ///< ������֧�֡�
    NRFX_ERROR_INVALID_PARAM        = (NRFX_ERROR_BASE_NUM + 4),  ///< ������Ч��
    NRFX_ERROR_INVALID_STATE        = (NRFX_ERROR_BASE_NUM + 5),  ///< ״̬��Ч���ڵ�ǰ״̬�²����������
    NRFX_ERROR_INVALID_LENGTH       = (NRFX_ERROR_BASE_NUM + 6),  ///< ������Ч��
    NRFX_ERROR_TIMEOUT              = (NRFX_ERROR_BASE_NUM + 7),  ///< ������ʱ��
    NRFX_ERROR_FORBIDDEN            = (NRFX_ERROR_BASE_NUM + 8),  ///< ��������ֹ��
    NRFX_ERROR_NULL                 = (NRFX_ERROR_BASE_NUM + 9),  ///< ��ָ�����
    NRFX_ERROR_INVALID_ADDR         = (NRFX_ERROR_BASE_NUM + 10), ///< ��Ч���ڴ��ַ��
    NRFX_ERROR_BUSY                 = (NRFX_ERROR_BASE_NUM + 11), ///< �豸æ��
    NRFX_ERROR_ALREADY_INITIALIZED  = (NRFX_ERROR_BASE_NUM + 12), ///< ģ���ѳ�ʼ����

    NRFX_ERROR_DRV_TWI_ERR_OVERRUN  = (NRFX_ERROR_DRIVERS_BASE_NUM + 0), ///< TWI/I2C ���������
    NRFX_ERROR_DRV_TWI_ERR_ANACK    = (NRFX_ERROR_DRIVERS_BASE_NUM + 1), ///< TWI/I2C ���󣺵�ַδ��ȷ�ϡ�
    NRFX_ERROR_DRV_TWI_ERR_DNACK    = (NRFX_ERROR_DRIVERS_BASE_NUM + 2)  ///< TWI/I2C ��������δ��ȷ�ϡ�
} nrfx_err_t;

/** @} */

#endif // !NRFX_CHECK(NRFX_CUSTOM_ERROR_CODES)

#endif // NRFX_ERRORS_H__





