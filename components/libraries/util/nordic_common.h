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
 * @brief Nordic Semiconductor �����Ĺ̼��е�ͨ�ö���ͺꡣ
 *
 * ����ļ�������һЩ�����ĺ꣬��������ģ�顢����λ��������ȡ��ֵ���ض�λ�����Ӷ�������ȹ��ܡ�
 */

#ifndef NORDIC_COMMON_H__
#define NORDIC_COMMON_H__

#ifdef __cplusplus
extern "C" {	//ָʾ���������� C �ķ�ʽ���з��Ž��������������������Σ�name mangling����������, C++ �У���������Ժ����ͱ����������������Σ���֧�ֺ������ص����ԣ��� C ��������û���������η�ʽ
#endif

/**
 * @brief �����ѡģ���Ƿ����á�
 *
 * ����һ����ȫ�������������ü�麯����
 * �Ӵ��뾲̬�������ߣ���Lint���ĽǶ�������������ȷ�ģ���ʹ��Ĭ�ϵ�δ����ֵ����
 *
 * ʹ��ʾ����
 * @code
   #if NRF_MODULE_ENABLED(UART)
   ...
   #endif
 * @endcode
 *
 * @param module ģ�����ơ�
 *
 * @retval 1 �� <module>_ENABLED �����岢�ҷ��㡣
 * @retval 0 �� <module>_ENABLED δ���������㡣
 *
 * @note �ú���ⲻʵ�ֵڶ�����չ��Ҫģ������Ʊ���ֱ����Ϊ�����ṩ�������Ǵ���һ������������������ӱ�ʾģ�����ƣ�������ֱ���� `ENABLED` ��׺���ӣ���������ֵ����������
 */
// lint -emacro(491,NRF_MODULE_ENABLED) // ��ֹLint����491 ��defined�������ķǱ�׼ʹ�á�
#ifdef NRF_MODULE_ENABLE_ALL
#warning "������ʵ�ʹ�����ʹ��NRF_MODULE_ENABLE_ALL��"
#define NRF_MODULE_ENABLED(module) 1
#else
#define NRF_MODULE_ENABLED(module) \
    ((defined(module ## _ENABLED) && (module ## _ENABLED)) ? 1 : 0)
#endif

/** ��ȡ32λֵ�ĸ�8λ */
// lint -emacro(572,MSB_32) // ��ֹLint����572 ���������λֵ��
#define MSB_32(a) (((a) & 0xFF000000) >> 24)

/** ��ȡ32λֵ�ĵ�8λ */
#define LSB_32(a) ((a) & 0x000000FF)

/** ��ȡ16λֵ�ĸ�8λ */
// lint -emacro(572,MSB_16) // ��ֹLint����572 ���������λֵ��
#define MSB_16(a) (((a) & 0xFF00) >> 8)

/** ��ȡ16λֵ�ĵ�8λ */
#define LSB_16(a) ((a) & 0x00FF)

/** ��������32λ�����е���Сֵ */
/*lint -emacro(506, MIN) */ /* ��ֹLint���� ������ֵΪ������ */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/** ��������32λ�����е����ֵ */
/*lint -emacro(506, MAX) */ /* ��ֹLint���� ������ֵΪ������ */
#define MAX(a, b) ((a) < (b) ? (b) : (a))

/**@brief ����������������һ��
 *
 * ʵ����������չ����ȷ������������֮ǰ�����в�������չ����
 *
 * @param p1 ��һ��������
 * @param p2 �ڶ���������
 *
 * @return ����������������һ�����Ǳ���������Ч��C���Է��ţ���������Ԥ����������
 *
 * @sa CONCAT_3
 */
#define CONCAT_2(p1, p2)      CONCAT_2_(p1, p2)
/** CONCAT_2�ĸ����� */
#define CONCAT_2_(p1, p2)     p1##p2

/**@brief ����������������һ��
 *
 * ʵ����������չ����ȷ������������֮ǰ�����в�������չ����
 *
 * @param p1 ��һ��������
 * @param p2 �ڶ���������
 * @param p3 ������������
 *
 * @return ����������������һ�����Ǳ���������Ч��C���Է��ţ���������Ԥ����������
 *
 * @sa CONCAT_2
 */
#define CONCAT_3(p1, p2, p3)  CONCAT_3_(p1, p2, p3)
/** CONCAT_3�ĸ����� */
#define CONCAT_3_(p1, p2, p3) p1##p2##p3

/**@brief �������ת��Ϊ�ַ���������
 */
#define STRINGIFY_(val) #val
#define STRINGIFY(val)  STRINGIFY_(val)

/**@brief ���������е�Ԫ�ظ�����
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**@brief ����32λ���е�ָ��λ��
 *
 * @param[in] W Ҫ����λ���֡�
 * @param[in] B Ҫ���õ�λ�š�
 */
#define SET_BIT(W, B)  ((W) |= (uint32_t)(1U << (B)))

/**@brief ���32λ���е�ָ��λ��
 *
 * @param[in] W Ҫ���λ���֡�
 * @param[in] B Ҫ�����λ�š�
 */
#define CLR_BIT(W, B) ((W) &= (~(uint32_t)(1U << (B))))

/**@brief ���λ�Ƿ������á�
 *
 * @param[in] W Ҫ���λ���֡�
 * @param[in] B Ҫ����λ�š�
 *
 * @retval 1 ���λ�����á�
 * @retval 0 ���λδ���á�
 */
#define IS_SET(W, B) (((W) >> (B)) & 1)

/* ����λ����ֵ */
#define BIT_0 0x01 /**< λ0��ֵ */
#define BIT_1 0x02 /**< λ1��ֵ */
#define BIT_2 0x04 /**< λ2��ֵ */
#define BIT_3 0x08 /**< λ3��ֵ */
#define BIT_4 0x10 /**< λ4��ֵ */
#define BIT_5 0x20 /**< λ5��ֵ */
#define BIT_6 0x40 /**< λ6��ֵ */
#define BIT_7 0x80 /**< λ7��ֵ */
#define BIT_8 0x0100 /**< λ8��ֵ */
#define BIT_9 0x0200 /**< λ9��ֵ */
#define BIT_10 0x0400 /**< λ10��ֵ */
#define BIT_11 0x0800 /**< λ11��ֵ */
#define BIT_12 0x1000 /**< λ12��ֵ */
#define BIT_13 0x2000 /**< λ13��ֵ */
#define BIT_14 0x4000 /**< λ14��ֵ */
#define BIT_15 0x8000 /**< λ15��ֵ */
#define BIT_16 0x00010000 /**< λ16��ֵ */
#define BIT_17 0x00020000 /**< λ17��ֵ */
#define BIT_18 0x00040000 /**< λ18��ֵ */
#define BIT_19 0x00080000 /**< λ19��ֵ */
#define BIT_20 0x00100000 /**< λ20��ֵ */
#define BIT_21 0x00200000 /**< λ21��ֵ */
#define BIT_22 0x00400000 /**< λ22��ֵ */
#define BIT_23 0x00800000 /**< λ23��ֵ */
#define BIT_24 0x01000000 /**< λ24��ֵ */
#define BIT_25 0x02000000 /**< λ25��ֵ */
#define BIT_26 0x04000000 /**< λ26��ֵ */
#define BIT_27 0x08000000 /**< λ27��ֵ */
#define BIT_28 0x10000000 /**< λ28��ֵ */
#define BIT_29 0x20000000 /**< λ29��ֵ */
#define BIT_30 0x40000000 /**< λ30��ֵ */
#define BIT_31 0x80000000 /**< λ31��ֵ */

/* ���ñ����������ͷ���ֵ�ĺ� */
#define UNUSED_VARIABLE(X)  ((void)(X))	/** ʹ�÷�����UNUSED_VARIABLE(������) */
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X) UNUSED_VARIABLE(X)

#ifdef __cplusplus
}
#endif

#endif // NORDIC_COMMON_H__
