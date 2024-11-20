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
 *              库文件             		 *
  ***************************************/
#include <string.h>             // 字符串处理库

#include "mouse.h"							// 鼠标驱动

#include "bsp.h"								// 包含BSP模块
#include "boards.h"             // 板级支持库
#include "nrf_gpio.h"           // GPIO 控制库
#include "nrf_delay.h"          // 延时库
#include "app_util_platform.h"  // 平台工具库
#include "app_error.h"          // 错误处理库

#include "nrf_log.h"            // 日志库
#include "nrf_log_ctrl.h"       // 日志控制库
#include "nrf_log_default_backends.h"  // 日志后端库

/***************************************
 *            初始化函数             	*
 ***************************************/
/**
 * @brief 通用GPIO初始化函数，用于初始化
 * 
 */
void gpio_init(void)
{
}

/**
 * @brief 初始化日志模块
 * 
 * 该函数独立完成日志模块和后端的初始化工作
 */
void log_init(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));  // 初始化日志模块
		NRF_LOG_DEFAULT_BACKENDS_INIT();  // 初始化日志后端
}

/***************************************
 *             打印函数             		 *
 ***************************************/
/**
 * @brief 倒计时函数，以秒为单位进行倒计时，并在每秒输出日志。
 *
 * @param[in] seconds 倒计时的秒数。
 */
void nrf_dealy_countdown(int seconds)
{
    for (int i = seconds; i > 0; i--)
    {
        NRF_LOG_INFO("%d...", i);
        NRF_LOG_FLUSH();
        nrf_delay_ms(1000);  // 延时1秒
    }
}

/***************************************
 *              主函数              		 *
 ***************************************/
/**
 * @brief 主函数，初始化SPI并持续发送/接收数据。
 */
int main(void)
{
		// 主函数初始化
		log_init();
		gpio_init();
	
		usb_device_init();

    esb_device_init();
	
    // 主循环，处理日志和功耗管理
    while (true)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            // 检查是否需要改变USB挂起状态
						if (get_mark_usbd_suspended() != get_mark_usbd_suspend_state_request())
						{
								if (get_mark_usbd_suspend_state_request()) // 如果请求USB进入挂起状态
								{
										usb_device_force_suspend();
								}
								else // 如果请求退出USB挂起状态
								{
										usb_device_force_leaving_suspend();
								}
						}
        }
    }
}
