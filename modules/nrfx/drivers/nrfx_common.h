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

#ifndef NRFX_COMMON_H__
#define NRFX_COMMON_H__

#include <stdint.h>    // �ṩ��׼�������Ͷ��壨�� int8_t, uint32_t �ȣ�
#include <stddef.h>    // ���峣�����ͺͺ꣨�� size_t, NULL �ȣ�
#include <stdbool.h>   // �ṩ��������֧�֣����� true �� false��

#include <nrf.h>       // �ṩ��Nordic SemiconductorоƬ��Ӳ���Ĵ���������ķ���
#include <nrf_peripherals.h>  // ����NordicоƬ�����趨�壬���������ض�Ӳ������

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_common ����ģ��
 * @{
 * @ingroup nrfx
 * @brief ����ģ�顣
 */

/**
 * @brief ���ڼ��ָ���ı�ʶ���Ƿ��Ѷ�������ֵ��Ϊ��ĺꡣ
 *
 * ͨ����Ԥ������������δ����ı�ʶ����ΪֵΪ�㡣
 * Ȼ����һЩ���ߣ��羲̬����������������������ʶ��ʱ���ܻᷢ�����档
 * �ú���Խ��ڴ˺����������ĵط����ƴ��ྯ�棬�����������������в���Ӱ�졣
 */
#define NRFX_CHECK(module_enabled)  (module_enabled)

/**
 * @brief �����ں�չ��ʱ����������ǵĺꡣ
 *
 * @note �ú������չ������ȷ����Ϊ�걾��ı���ںϲ�֮ǰ����ȫչ����
 *
 * @param[in] p1 ��һ����ǡ�
 * @param[in] p2 �ڶ�����ǡ�
 *
 * @return �ϲ���ı�ǣ����������޷��γ���Ч�ı�ǣ�����������£�Ԥ�������ᷢ�����沢�Ҳ���ִ�кϲ�����
 *
 * @sa NRFX_CONCAT_3
 */
#define NRFX_CONCAT_2(p1, p2)       NRFX_CONCAT_2_(p1, p2)

/** @brief �ڲ��꣬�� @ref NRFX_CONCAT_2 ��������չ���� */
#define NRFX_CONCAT_2_(p1, p2)      p1 ## p2

/**
 * @brief �����ں�չ��ʱ����������ǵĺꡣ
 *
 * @note �ú������չ������ȷ����Ϊ�걾��ı���ںϲ�֮ǰ����ȫչ����
 *
 * @param[in] p1 ��һ����ǡ�
 * @param[in] p2 �ڶ�����ǡ�
 * @param[in] p3 ��������ǡ�
 *
 * @return �ϲ���ı�ǣ����������޷��γ���Ч�ı�ǣ�����������£�Ԥ�������ᷢ�����沢�Ҳ���ִ�кϲ�����
 *
 * @sa NRFX_CONCAT_2
 */
#define NRFX_CONCAT_3(p1, p2, p3)   NRFX_CONCAT_3_(p1, p2, p3)

/** @brief �ڲ��꣬�� @ref NRFX_CONCAT_3 ��������չ���� */
#define NRFX_CONCAT_3_(p1, p2, p3)  p1 ## p2 ## p3

/**
 * @brief ����ִ�����������������������ضϽ����ԣ���
 *
 * @param[in] a ���ӡ�
 * @param[in] b ��ĸ��
 *
 * @return a ���� b ���������루�����������
 */
#define NRFX_ROUNDED_DIV(a, b)  (((a) + ((b) / 2)) / (b))

/**
 * @brief ����ִ������������ȷ���������ȡ���ĺꡣ
 *
 * @details �˺�ĵ���ʹ�ó����Ǽ�����Ҫ���ٴ�СΪ b �Ķ������洢 a ���ֽڡ�
 *
 * @param[in] a ���ӡ�
 * @param[in] b ��ĸ��
 *
 * @return a ���� b �����������������ȡ����
 */
#define NRFX_CEIL_DIV(a, b)  ((((a) - 1) / (b)) + 1)

/**
 * @brief ���ڻ�ȡ����Ԫ�������ĺꡣ
 *
 * @param[in] array ��������ơ�
 *
 * @return �����Ԫ��������
 */
#define NRFX_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/**
 * @brief ���ڻ�ȡָ�����ͽṹ���г�Ա���ֽ�ƫ�����ĺꡣ
 *
 * @param[in] type   �ṹ�����͡�
 * @param[in] member Ҫ����ƫ�����Ľṹ���Ա��
 *
 * @return ��Ա���ֽ�ƫ������
 */
#define NRFX_OFFSETOF(type, member)  ((size_t)&(((type *)0)->member))

/** @brief ���ڼ��EasyDMA���䳤���Ƿ񳬹�ָ���������Ƶĺꡣ
 *
 * @param[in] peripheral Ҫ��鳤�ȵ����衣
 * @param[in] length1    Ҫ���ĵ�һ�����ȡ�
 * @param[in] length2    Ҫ���ĵڶ������ȣ��粻��Ҫ���봫��0����
 *
 * @retval true  ����������û�г���ָ����������ơ�
 * @retval false ���������ȳ�����ָ����������ơ�
 */
#define NRFX_EASYDMA_LENGTH_VALIDATE(peripheral, length1, length2)            \
    (((length1) < (1U << NRFX_CONCAT_2(peripheral, _EASYDMA_MAXCNT_SIZE))) && \
     ((length2) < (1U << NRFX_CONCAT_2(peripheral, _EASYDMA_MAXCNT_SIZE))))

/**
 * @brief ���ڵȴ�ֱ�����������ĺꡣ
 *
 * @param[in]  condition Ҫ�����������
 * @param[in]  attempts  ����������Ĵ���������Ϊ0��
 * @param[in]  delay_us  �������֮����ӳ٣�΢�룩��
 * @param[out] result    �洢�ȴ����̽���Ĳ��������������������������Ϊtrue������Ϊfalse��
 */
#define NRFX_WAIT_FOR(condition, attempts, delay_us, result) \
do {                                                         \
    result =  false;                                         \
    uint32_t remaining_attempts = (attempts);                \
    do {                                                     \
           if (condition)                                    \
           {                                                 \
               result =  true;                               \
               break;                                        \
           }                                                 \
           NRFX_DELAY_US(delay_us);                          \
    } while (--remaining_attempts);                          \
} while(0)

/**
 * @brief ���ڻ�ȡָ������ID�ŵĺꡣ
 *
 * ����Nordic SoC�е����裬���ǵ�ID���������ֱַ����ء�
 * ����Ĳ�Ʒ����еġ�����ӿڡ��½ڣ�������ID�����֣���
 *
 * @param[in] base_addr �������ַ��ָ�롣
 *
 * @return ��ָ�����������ID�š�
 */
#define NRFX_PERIPHERAL_ID_GET(base_addr)  (uint8_t)((uint32_t)(base_addr) >> 12)

/**
 * @brief ���ڻ�ȡ�����ָ��������жϺŵĺꡣ
 *
 * ����Nordic SoC�е����裬����������IRQ�ŵ�����ID�š�
 * ����Ĳ�Ʒ����еġ�����ӿڡ��½ڣ�������ID���͡��жϡ����֣���
 *
 * @param[in] base_addr �������ַ��ָ�롣
 *
 * @return ��ָ������������жϺš�
 */
#define NRFX_IRQ_NUMBER_GET(base_addr)  NRFX_PERIPHERAL_ID_GET(base_addr)

/** @brief IRQ����������͡� */
typedef void (* nrfx_irq_handler_t)(void);

/** @brief ��������״̬�� */
typedef enum
{
    NRFX_DRV_STATE_UNINITIALIZED, ///< δ��ʼ����
    NRFX_DRV_STATE_INITIALIZED,   ///< �ѳ�ʼ����δͨ�硣
    NRFX_DRV_STATE_POWERED_ON,    ///< �ѳ�ʼ����ͨ�硣
} nrfx_drv_state_t;

/**
 * @brief ���ڼ������Ƿ�λ������RAM����ĺ�����
 *
 * ĳЩ���裨ʹ��EasyDMA�����裩Ҫ���仺����λ������RAM����
 * �˺��������ڼ��������Ƿ����㡣
 *
 * @param[in] p_object Ҫ�����λ�õĶ���ָ�롣
 *
 * @retval true  ָ��Ķ���λ������RAM����
 * @retval false ָ��Ķ���λ������RAM����
 */
__STATIC_INLINE bool nrfx_is_in_ram(void const * p_object);

/**
 * @brief ���ڼ������Ƿ���뵽32λ�ֵĺ�����
 *
 * ĳЩ���裨ʹ��EasyDMA�����裩Ҫ���仺�������뵽32λ�֡�
 * �˺��������ڼ��������Ƿ����㡣
 *
 * @param[in] p_object  Ҫ�����λ�õĶ���ָ�롣
 *
 * @retval true  ָ��Ķ�����뵽32λ�֡�
 * @retval false ָ��Ķ���δ���뵽32λ�֡�
 */
__STATIC_INLINE bool nrfx_is_word_aligned(void const * p_object);

/**
 * @brief ���ڻ�ȡָ��������жϺŵĺ�����
 *
 * @param[in] p_reg �������ַָ�롣
 *
 * @return ��ָ������������жϺš�
 */
__STATIC_INLINE IRQn_Type nrfx_get_irq_number(void const * p_reg);

/**
 * @brief ���ڽ�INTEN�Ĵ���λλ��ת��Ϊ��Ӧ�¼���ʶ���ĺ�����
 *
 * �¼���ʶ�����¼��Ĵ�����ַ���������ַ֮���ƫ���������ҵ���
 * HAL�е�ö�����ͣ�nrf_*_event_t���Ķ�Ӧֵ����˿���ֱ��ת������
 *
 * @param[in] bit INTEN�Ĵ���λλ�á�
 *
 * @return �¼���ʶ����
 *
 * @sa nrfx_event_to_bitpos
 */
__STATIC_INLINE uint32_t nrfx_bitpos_to_event(uint32_t bit);

/**
 * @brief ���ڽ��¼���ʶ��ת��Ϊ��ӦINTEN�Ĵ���λλ�õĺ�����
 *
 * �¼���ʶ�����¼��Ĵ�����ַ���������ַ֮���ƫ���������ҵ���
 * HAL�е�ö�����ͣ�nrf_*_event_t���Ķ�Ӧֵ����˿���ֱ��ת������
 *
 * @param[in] event �¼���ʶ����
 *
 * @return INTEN�Ĵ���λλ�á�
 *
 * @sa nrfx_bitpos_to_event
 */
__STATIC_INLINE uint32_t nrfx_event_to_bitpos(uint32_t event);


#ifndef SUPPRESS_INLINE_IMPLEMENTATION /** �˺����ڿ����Ƿ���������������ʵ�֣��������� SUPPRESS_INLINE_IMPLEMENTATION ʱ������������ʵ�ֽ������� */

/**
 * @brief �������Ƿ�λ������RAM����
 * 
 * �ú������ڼ�鴫��Ķ���ָ���Ƿ�λ���ڴ��ַ��Data RAM����
 * ��NordicоƬ�У�Data RAM����ĵ�ַ�ռ䷶Χ��0x20000000Ϊ��׼��
 * ͨ���Ե�ַ��λ�������Σ�& 0xE0000000u��������0x20000000u���бȽϣ�
 * ���ж϶����Ƿ���Data RAM����
 *
 * EasyDMA������Ҫ���仺����λ��Data RAM����
 *
 * @param[in] p_object Ҫ���Ķ���ָ�롣
 * 
 * @retval true  �������λ��Data RAM�����򷵻�true��
 * @retval false ���������Data RAM�����򷵻�false��
 */
__STATIC_INLINE bool nrfx_is_in_ram(void const * p_object)
{
    return ((((uint32_t)p_object) & 0xE0000000u) == 0x20000000u);
}

/**
 * @brief �������Ƿ���뵽32λ�֡�
 * 
 * �ú������ڼ�����ĵ�ַ�Ƿ���뵽32λ�֣�4�ֽڣ���
 * EasyDMA������Ҫ���仺�������뵽32λ������ַ����λ����Ϊ0����
 * 
 * @param[in] p_object Ҫ���Ķ���ָ�롣
 * 
 * @retval true  ����ĵ�ַ���뵽32λ�֡�
 * @retval false ����ĵ�ַδ���뵽32λ�֡�
 */
__STATIC_INLINE bool nrfx_is_word_aligned(void const * p_object)
{
    return ((((uint32_t)p_object) & 0x3u) == 0u);
}

/**
 * @brief ��ȡ������жϺš�
 * 
 * �ú���������ָ���������ַ�������IRQ�жϺš�
 * ����Nordic SoC��IRQ�ŵ�������ID�š�
 * 
 * @param[in] p_reg �������ַָ�롣
 * 
 * @return ��ص�IRQ�жϺš�
 */
__STATIC_INLINE IRQn_Type nrfx_get_irq_number(void const * p_reg)
{
    return (IRQn_Type)NRFX_IRQ_NUMBER_GET(p_reg);
}

/**
 * @brief ��INTEN�Ĵ����е�λλ��ת��Ϊ�¼���ʶ����
 * 
 * �ú�����INTEN�Ĵ����е�λλ��ת��Ϊ�¼��Ĵ����е��¼���ʶ����
 * ͨ�������¼���INTEN�Ĵ���λλ�õ�ӳ�䡣
 * 
 * @param[in] bit INTEN�Ĵ����е�λλ�á�
 * 
 * @return ��Ӧ���¼���ʶ����
 */
__STATIC_INLINE uint32_t nrfx_bitpos_to_event(uint32_t bit)
{
    static const uint32_t event_reg_offset = 0x100u;
    return event_reg_offset + (bit * sizeof(uint32_t));
}

/**
 * @brief ���¼���ʶ��ת��ΪINTEN�Ĵ����е�λλ�á�
 * 
 * �ú���ͨ�����¼��Ĵ����ĵ�ַƫ�����м�ȥ�¼��Ĵ����Ļ�ƫ���������
 * ��Ӧ��INTEN�Ĵ����е�λλ�á�
 * 
 * @param[in] event �¼���ʶ����
 * 
 * @return INTEN�Ĵ����е�λλ�á�
 */
__STATIC_INLINE uint32_t nrfx_event_to_bitpos(uint32_t event)
{
    static const uint32_t event_reg_offset = 0x100u;
    return (event - event_reg_offset) / sizeof(uint32_t);
}


#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_COMMON_H__
