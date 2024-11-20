#ifndef ESB_DEVICE_H
#define ESB_DEVICE_H

/***************************************
 *              库文件             		 *
 ***************************************/
#include <stdint.h>   // 标准整数类型定义
#include <stdbool.h>  // 布尔类型定义
#include <stddef.h>   // 标准定义（NULL等）

// Nordic库
#include "nordic_common.h"  // 包含Nordic芯片的通用定义和功能，如工具宏等。
#include "nrf.h"            // 提供Nordic芯片的寄存器和内存映射定义。
#include "app_error.h"      // 提供错误检查和错误处理功能，便于调试和错误日志记录。
#include "nrf_delay.h"			// nrf延迟库

// ESB库
#include "nrf_esb.h"        // NRF增强型ShockBurst(ESB)协议库
#include "nrf_esb_error_codes.h" // ESB协议相关的错误代码

// 时钟库
#include "nrf_drv_clock.h"	// 时钟库

// 系统库
#include "app_util.h"       // 应用程序实用工具库

// 日志管理
#include "nrf_log.h"             // 日志模块，用于记录调试和运行信息。
#include "nrf_log_ctrl.h"        // 日志控制模块，用于控制日志记录的启动和停止。
#include "nrf_log_default_backends.h" // 日志后端管理，定义日志输出的具体位置，如串口或存储。


 /****************************************
 *                 函数  			           *
 ****************************************/
 // ESB函数
void esb_device_event_handler(nrf_esb_evt_t const * p_event);
uint32_t esb_module_init(void);
void esb_device_init(void);

#endif // ESB_DEVICE_H
