/**
 * Copyright (c) 2015 - 2021, Nordic Semiconductor ASA
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
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS	esb_device_init(); INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
 /***************************************
 *              ���ļ�             		 *
  ***************************************/
#include <string.h>             // �ַ��������

#include "mouse.h"							// �������

#include "bsp.h"								// ����BSPģ��
#include "boards.h"             // �弶֧�ֿ�
#include "nrf_gpio.h"           // GPIO ���ƿ�
#include "nrf_delay.h"          // ��ʱ��
#include "app_util_platform.h"  // ƽ̨���߿�
#include "app_error.h"          // �������

#include "nrf_log.h"            // ��־��
#include "nrf_log_ctrl.h"       // ��־���ƿ�
#include "nrf_log_default_backends.h"  // ��־��˿�

/***************************************
 *            ��ʼ������             	*
 ***************************************/

/**
 * @brief ��ʼ����־ģ��
 * 
 * �ú������������־ģ��ͺ�˵ĳ�ʼ������
 */
void system_log_init(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));  // ��ʼ����־ģ��
		NRF_LOG_DEFAULT_BACKENDS_INIT();  // ��ʼ����־���
}

/**
 * @brief ��ʼ��GPIO
 * 
 * �ú����������GPIOģ���ʼ������
 */
void system_gpio_init(void)
{
}

/**
 * @brief ��ʼ��ʱ��ģ��
 * 
 * �ú����������ʱ��ģ���ʼ������
 */
void system_clock_init(void)
{ 
		ret_code_t ret;
		
		// ��ʼ��ʱ��
    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret); // ���ʱ�ӳ�ʼ���Ƿ�ɹ�

    // �����Ƶʱ�Ӻ͵�Ƶʱ��
    nrf_drv_clock_hfclk_request(NULL); // �����Ƶʱ��
    nrf_drv_clock_lfclk_request(NULL); // �����Ƶʱ��
		
	
    // �ȴ�ʱ������ 
    while (!(nrf_drv_clock_hfclk_is_running() &&
             nrf_drv_clock_lfclk_is_running()))
    {
        // �ȴ�ʱ������
    }

    // ��ʼ��Ӧ�ö�ʱ��
    ret = app_timer_init();
    APP_ERROR_CHECK(ret);
		
    // �����ڽ��ö���ʱ�ľ���
    UNUSED_VARIABLE(ret);
}

/**
 * @brief ��ʼ����Դģ��
 *
 * ��ʼ����Դ��
 */
void system_power_init(void)
{
    ret_code_t ret;
		
		// ��ʼ����Դ
		ret = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(ret); // ����Դ��ʼ���Ƿ�ɹ�
}

/***************************************
 *              ������              		 *
 ***************************************/
/**
 * @brief ����������ʼ��SPI����������/�������ݡ�
 */
int main(void)
{
		// WS2812B ָʾ�����ʼ��
		WS2812B_init();
	
		// ��־ģ���ʼ��
		system_log_init();
	
		// GPIOģ���ʼ��
		system_gpio_init();
	
		// ʱ��ģ���ʼ��
		system_clock_init();
	
		// ��Դģ���ʼ��
		system_power_init();
	
		// USB �豸��ʼ��
		usb_device_init();
	
		// PAW3399-TQ4U ��ʼ��
		paw3399_init();
	
		// ��� ��ʼ��
		mouse_init();

    // �������������ѭ��
		mouse_main_task();
}
