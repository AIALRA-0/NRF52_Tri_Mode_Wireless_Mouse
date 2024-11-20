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
 * @brief 初始化日志模块
 * 
 * 该函数独立完成日志模块和后端的初始化工作
 */
void system_log_init(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));  // 初始化日志模块
		NRF_LOG_DEFAULT_BACKENDS_INIT();  // 初始化日志后端
}

/**
 * @brief 初始化GPIO
 * 
 * 该函数独立完成GPIO模块初始化工作
 */
void system_gpio_init(void)
{
}

/**
 * @brief 初始化时钟模块
 * 
 * 该函数独立完成时钟模块初始化工作
 */
void system_clock_init(void)
{ 
		ret_code_t ret;
		
		// 初始化时钟
    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret); // 检查时钟初始化是否成功

    // 请求高频时钟和低频时钟
    nrf_drv_clock_hfclk_request(NULL); // 请求高频时钟
    nrf_drv_clock_lfclk_request(NULL); // 请求低频时钟
		
	
    // 等待时钟启动 
    while (!(nrf_drv_clock_hfclk_is_running() &&
             nrf_drv_clock_lfclk_is_running()))
    {
        // 等待时钟启动
    }

    // 初始化应用定时器
    ret = app_timer_init();
    APP_ERROR_CHECK(ret);
		
    // 避免在禁用断言时的警告
    UNUSED_VARIABLE(ret);
}

/**
 * @brief 初始化电源模块
 *
 * 初始化电源。
 */
void system_power_init(void)
{
    ret_code_t ret;
		
		// 初始化电源
		ret = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(ret); // 检查电源初始化是否成功
}

/***************************************
 *              主函数              		 *
 ***************************************/
/**
 * @brief 主函数，初始化SPI并持续发送/接收数据。
 */
int main(void)
{
		// WS2812B 指示灯珠初始化
		WS2812B_init();
	
		// 日志模块初始化
		system_log_init();
	
		// GPIO模块初始化
		system_gpio_init();
	
		// 时钟模块初始化
		system_clock_init();
	
		// 电源模块初始化
		system_power_init();
	
		// USB 设备初始化
		usb_device_init();
	
		// PAW3399-TQ4U 初始化
		paw3399_init();
	
		// 鼠标 初始化
		mouse_init();

    // 启动鼠标主处理循环
		mouse_main_task();
}
