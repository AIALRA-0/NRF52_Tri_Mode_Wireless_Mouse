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
#ifndef NRF_DRV_CLOCK_H__
#define NRF_DRV_CLOCK_H__

// ����ʱ��������ͷ�ļ�
#include <nrfx_clock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_drv_clock Clock driver - legacy layer
 * @{
 * @ingroup nrf_clock
 *
 * @brief ��ģ��Ϊ���ݾɵ�API�ṩ��֧�֡������� `nrfx_clock` ʵ�֡�
 */

/**
 * @brief ʱ���¼����͡�
 *
 * ��Щö��ֵ��ʾϵͳ����ʱ����صĲ�ͬ�¼���
 */
typedef enum
{
    NRF_DRV_CLOCK_EVT_HFCLK_STARTED, ///< ��Ƶʱ����������
    NRF_DRV_CLOCK_EVT_LFCLK_STARTED, ///< ��Ƶʱ����������
    NRF_DRV_CLOCK_EVT_CAL_DONE,      ///< ʱ��У׼��ɡ�
    NRF_DRV_CLOCK_EVT_CAL_ABORTED,   ///< ʱ��У׼����ֹ��
} nrf_drv_clock_evt_type_t;

/**
 * @brief ʱ���¼����������͡�
 *
 * ��ʱ������¼�����ʱ������ô˴�������
 *
 * @param[in] event ������ʱ���¼���
 */
typedef void (*nrf_drv_clock_event_handler_t)(nrf_drv_clock_evt_type_t event);

// ǰ������ `nrf_drv_clock_handler_item_t` ���ͣ����ڱ�ʾ�¼��������ṹ�塣
typedef struct nrf_drv_clock_handler_item_s nrf_drv_clock_handler_item_t;

/**
 * @brief ʱ���¼����������Ľṹ�嶨�塣
 *
 * ����ָ����һ���¼���������ָ��͵�ǰ�¼���������ָ�롣
 */
struct nrf_drv_clock_handler_item_s
{
    nrf_drv_clock_handler_item_t * p_next;        ///< ָ����һ��ʱ�Ӵ�������ָ�롣
    nrf_drv_clock_event_handler_t  event_handler; ///< ��ʱ������ʱҪ���õĴ����������
};

/**
 * @brief ���ʱ�������Ƿ��ѳ�ʼ����
 *
 * @retval true  ʱ�������ѳ�ʼ����
 * @retval false ʱ������δ��ʼ����
 */
bool nrf_drv_clock_init_check(void);

/**
 * @brief ��ʼ��ʱ������ģ�顣
 *
 * ��ʼ����ģ�鴦�ڹر�״̬��ʱ����δ���󣩡�
 *
 * @retval NRF_SUCCESS                          ��ʼ���ɹ���
 * @retval NRF_ERROR_MODULE_ALREADY_INITIALIZED �����ѳ�ʼ����
 */
ret_code_t nrf_drv_clock_init(void);

/**
 * @brief ����ʼ��ʱ��ģ�顣
 *
 * �ú������ڹرպ�����ʱ������ģ�顣
 */
void nrf_drv_clock_uninit(void);

/**
 * @brief ����������Ƶʱ�� (LFCLK)��
 *
 * ��ͬģ��������Ŀ��������Ƶʱ�ӡ���������ȷ��ֻ���ڵ�һ������ʱ������ʱ�ӡ�
 * ���ʱ����δ׼���ã��ṩ�Ĵ������ᱻ��ӵ���������б��У���ʱ�������󽫱�֪ͨ��
 *
 * ����Ѿ�������Ƶʱ�ӣ��û��Ļص������������ӵ�ǰ�����ĵ��á�
 * 
 * @note ��SoftDevice����ʱ����Ƶʱ��һֱ���У����������޷���������
 *
 * @param[in] p_handler_item ָ���¼��������ṹ��ָ�롣
 */
void nrf_drv_clock_lfclk_request(nrf_drv_clock_handler_item_t * p_handler_item);

/**
 * @brief �ͷŵ�Ƶʱ�� (LFCLK)��
 *
 * ���û������ģ������ʱ�ӣ�LFCLK����ֹͣ��
 *
 * @note ��SoftDevice����ʱ����Ƶʱ��ʼ�����С�
 */
void nrf_drv_clock_lfclk_release(void);

/**
 * @brief ����Ƶʱ���Ƿ��������С�
 *
 * @retval true  ��Ƶʱ���������С�
 * @retval false ��Ƶʱ��δ���С�
 */
bool nrf_drv_clock_lfclk_is_running(void);

/**
 * @brief ���������߾���ʱ��Դ (HFCLK)��
 *
 * ��ͬģ��������Ŀ�������߾���ʱ�ӡ���������ȷ��ֻ���ڵ�һ������ʱ������ʱ�ӡ�
 * 
 * @note ���SoftDevice�������У�ʱ����SoftDevice��������������SoftDevice����
 *
 * @param[in] p_handler_item ָ���¼��������ṹ��ָ�롣
 */
void nrf_drv_clock_hfclk_request(nrf_drv_clock_handler_item_t * p_handler_item);

/**
 * @brief �ͷŸ߾���ʱ��Դ (HFCLK)��
 *
 * ���û������ģ������ʱ�ӣ��߾���ʱ�ӽ���ֹͣ��
 */
void nrf_drv_clock_hfclk_release(void);

/**
 * @brief ����Ƶʱ�� (HFCLK) �Ƿ��������С�
 *
 * @retval true  ��Ƶʱ���������С�
 * @retval false ��Ƶʱ��δ���С�
 */
bool nrf_drv_clock_hfclk_is_running(void);

/**
 * @brief ����һ��ʱ��У׼���̡�
 *
 * У׼���̷�Ϊ�����׶Σ��ӳ٣���ѡ��������HFCLK��Ӳ��֧�ֵ�У׼��
 *
 * @param[in] delay   У׼��ʼ���ӳ�ʱ�䣨��0.25��Ϊ��λ����
 * @param[in] handler У׼��ɻ���ֹʱ�Ļص�������
 *
 * @retval NRF_SUCCESS             �����ɹ���
 * @retval NRF_ERROR_FORBIDDEN     ���SoftDevice���ڻ�ѡ���LFCLKԴ����RC������
 * @retval NRF_ERROR_INVALID_STATE ��Ƶʱ�ӹر�ʱ��
 * @retval NRF_ERROR_BUSY          У׼���ڽ����С�
 */
ret_code_t nrf_drv_clock_calibration_start(uint8_t delay, nrf_drv_clock_event_handler_t handler);

/**
 * @brief ��ֹ���ڽ��е�У׼��
 *
 * ����ṩ�˴������������У׼��ֹ��ɺ���á�
 *
 * @retval NRF_SUCCESS             �����ɹ���
 * @retval NRF_ERROR_FORBIDDEN     ���SoftDevice���ڻ�ѡ���LFCLKԴ����RC������
 */
ret_code_t nrf_drv_clock_calibration_abort(void);

/**
 * @brief ���У׼�Ƿ����ڽ����С�
 *
 * @param[out] p_is_calibrating ���У׼���ڽ��У�������Ϊtrue��
 *
 * @retval NRF_SUCCESS             �����ɹ���
 * @retval NRF_ERROR_FORBIDDEN     ���SoftDevice���ڻ�ѡ���LFCLKԴ����RC������
 */
ret_code_t nrf_drv_clock_is_calibrating(bool * p_is_calibrating);

/**
 * @brief ������ʱ������ģ����ص�PPI�����ַ��
 *
 * @param[in] task ʱ����������
 *
 * @return �����ַ��
 */
__STATIC_INLINE uint32_t nrf_drv_clock_ppi_task_addr(nrf_clock_task_t task);

/**
 * @brief ������ʱ������ģ����ص�PPI�¼���ַ��
 *
 * @param[in] event ʱ�������¼���
 *
 * @return �¼���ַ��
 */
__STATIC_INLINE uint32_t nrf_drv_clock_ppi_event_addr(nrf_clock_event_t event);

#ifdef SOFTDEVICE_PRESENT
/**
 * @brief SoftDevice���������յ� @ref NRF_SOC_EVTS �¼�ʱ���ô˺�����
 *
 * @param[in] evt_id NRF_SOC_EVTS�е�һ��ֵ��
 */
void nrf_drv_clock_on_soc_event(uint32_t evt_id);

/**
 * @brief SoftDevice���ú���ô˺�����
 *
 * �ú�����Ҫ���ڱ��SoftDevice������LFCLK��
 */
void nrf_drv_clock_on_sd_enable(void);

/**
 * @brief SoftDevice���ú���ô˺�����
 *
 * �˺������ڣ�
 * 1. �ͷ�SoftDevice��LFCLK��Դ��
 * 2. ��SoftDevice�ͷ�POWER_CLOCK_IRQ�����³�ʼ���жϡ�
 */
void nrf_drv_clock_on_sd_disable(void);

#endif

#ifndef SUPPRESS_INLINE_IMPLEMENTATION
__STATIC_INLINE uint32_t nrf_drv_clock_ppi_task_addr(nrf_clock_task_t task)
{
    return nrf_clock_task_address_get(task); // ����ָ��ʱ������ĵ�ַ
}

__STATIC_INLINE uint32_t nrf_drv_clock_ppi_event_addr(nrf_clock_event_t event)
{
    return nrf_clock_event_address_get(event); // ����ָ��ʱ���¼��ĵ�ַ
}
#endif // SUPPRESS_INLINE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // NRF_DRV_CLOCK_H__
