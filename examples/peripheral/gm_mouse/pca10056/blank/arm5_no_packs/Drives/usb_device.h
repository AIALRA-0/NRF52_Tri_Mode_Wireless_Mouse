#ifndef USB_DEVICE_H
#define USB_DEVICE_H

/***************************************
 *              库文件             		 *
 ***************************************/
#include <stdint.h>   // 标准整数类型定义
#include <stdbool.h>  // 布尔类型定义
#include <stddef.h>   // 标准定义（NULL等）
#include "nrf.h"      // nRF微控制器设备驱动头文件
#include "nrf_drv_usbd.h" // nRF USB驱动头文件
#include "nrf_drv_clock.h" // 时钟驱动头文件
#include "nrf_gpio.h"  // GPIO操作头文件
#include "nrf_drv_gpiote.h" // GPIOTE库
#include "nrf_delay.h" // 延迟函数库
#include "nrf_drv_power.h" // 电源管理驱动头文件
#include "nrfx_log.h"  // 日志模块头文件
#include "nrf_log_ctrl.h"  // 日志控制模块
#include "nrf_log_default_backends.h" // 默认日志后端
#include "app_timer.h" // 定时器应用程序接口
#include "app_error.h" // 错误处理接口
#include "bsp.h"       // 板级支持包
#include "FreeRTOS.h"	 // FreeRTOS库的核心头文件ss
#include "task.h"			 // FreeRTOS任务管理的头文件
#include "semphr.h"			// FreeRTOS信号量和互斥锁
#include "ws2812b.h"    // RGB指示灯

 /****************************************
 *            字符串描述符                *
 ****************************************/
#define USBD_STRING_LANG_IX 0x00 // 语言字符串索引
#define USBD_STRING_LANG \
    0x04, /* 描述符长度 -> 4字节 */\
    0x03, /* 描述符类型  -> 字符串描述符 */\
    0x09, 0x04 /* 支持的语言ID -> 美国英语 */

#define USBD_STRING_SERIAL_IX 0x00 // 无序列号字符串索引

#define USBD_STRING_MANUFACTURER_IX 0x01 // 厂商字符串索引
#define USBD_STRING_MANUFACTURER \
    0x0E, /* 描述符长度 -> 14字节  */\
    0x03, /* 描述符类型 -> 字符串描述符) */\
    'A', 0x00, \
    'I', 0x00, \
    'A', 0x00, \
    'L', 0x00, \
    'R', 0x00, \
    'A', 0x00
		
#define USBD_STRING_PRODUCT_IX 0x02 // 产品字符串索引
#define USBD_STRING_PRODUCT \
    0x14, /* 描述符长度 -> 20字节 */\
    0x03, /* 描述符类型 -> 字符串描述符 */\
    'H', 0x00, \
    'I', 0x00, \
    'D', 0x00, \
    ' ', 0x00, \
    'M', 0x00, \
    'O', 0x00, \
    'U', 0x00, \
    'S', 0x00, \
    'E', 0x00

 /****************************************
 *             设备描述符                *
 ****************************************/
#define EP0_MAXPACKETSIZE NRF_DRV_USBD_EPSIZE	// EP0最大传输包大小
		
/** @brief 设备描述符定义 
 *  大端字节序，高字节在前，低字节在后
 */
#define USBD_DEVICE_DESCRIPTOR \
    0x12, 											/* 描述符长度 -> 18 字节*/\
    0x01, 											/* 描述符类型 -> 设备描述符*/\
    0x00, 0x02, 								/* USB版本 -> 2.0 */\
    0x00, 											/* 设备类 -> 由接口描述符决定，每个接口都指定其自己的类信息，各个接口独立运行 */\
    0x00, 											/* 设备子类 -> 无特定设备子类 */\
    0x00, 											/* 设备协议 -> 无特定设备层协议*/\
    EP0_MAXPACKETSIZE, 					/* 最大包大小 */\
    0x15, 0x19, 								/* 厂商ID -> Nordic Semiconfuctor ASA 6421  */\
    0x0A, 0x52, 								/* 产品ID -> nRF52 HID鼠标) */\
    0x01, 0x00, 								/* 设备版本号 -> 0.01 */\
    USBD_STRING_MANUFACTURER_IX,/* 厂商字符串索引 */\
    USBD_STRING_PRODUCT_IX, 		/* 产品字符串索引 */\
    USBD_STRING_SERIAL_IX, 			/* 序列号字符串索引 */\
    0x01 /* 配置数量 */
		
 /****************************************
 *             配置描述符                *
 ****************************************/

#define USBD_CONFIG_DESCRIPTOR_FULL_SIZE (9 + (9 + 9 + 7)) // 完整配置描述符的总大小，包括此配置返回的所有描述符（配置+接口+HID+端口）的总长度

/** @brief 配置描述符 */
#define USBD_CONFIG_DESCRIPTOR \
    0x09, /* 描述符长度 -> 9字节 */\
    0x02, /* 描述符类型 -> 配置描述符 */\
    USBD_CONFIG_DESCRIPTOR_FULL_SIZE, 0x00, /* 描述符总长度 */\
    0x01, /* 接口数量 -> 1*/\
    0x01, /* 选择配置值 -> 1*/\
    0x00, /* 配置字符串索引 -> 无字符串描述符用于描述这个配置*/\
    0x80 | (((DEVICE_SELF_POWERED) ? 1U:0U)<<6) | (((REMOTE_WU) ? 1U:0U)<<5), /* 自供电&远程唤醒属性 */\
    100 /* 最大功率 -> 100 * 2 = 200 mA */

 /****************************************
 *             接口描述符                *
 ****************************************/
/** @brief 接口描述符定义 */
#define USBD_INTERFACE0_DESCRIPTOR \
    0x09, /* 描述符长度 -> 9字节 */\
    0x04, /* 描述符类型 -> 接口描述符 */\
    0x00, /* 接口编号 -> 0*/\
    0x00, /* 备用设置 -> 无*/\
    0x01, /* 端点数量 -> 1*/\
    0x03, /* 接口类 -> HID */\
    0x00, /* 接口子类 -> 无 TODO:支持BOOT*/\
    0x02, /* 接口协议 -> 鼠标 */\
    0x00 /* 接口字符串索引 */

 /****************************************
 *             HID描述符                *
 ****************************************/
/**
 * @brief HID描述符定义
 */
#define USBD_HID0_DESCRIPTOR \
    0x09, /* 描述符长度 -> 9字节 */\
    0x21, /* HID描述符类型 -> HID */\
    0x11, 0x01, /* HID版本 -> 1.11 */\
    0x00, /* 目标国家代码 -> 无目标国家 */\
    0x01, /* 后续类描述符的数量 -> 1*/\
    0x22, /* 后续描述符1 类型 -> 报告描述符 */\
    (uint8_t)(USBD_MOUSE_REPORT_DESCRIPTOR_SIZE), /* 报告描述符长度低字节 */\
    (uint8_t)(USBD_MOUSE_REPORT_DESCRIPTOR_SIZE / 256) /* 报告描述符长度高字节 */
		
 /****************************************
 *             端点描述符                *
 ****************************************/
#define POLLLING_INTERVAL (1000 / REPORT_RATE) // 轮询间隔 (ms)

#define USBD_ENDPOINT1_DESCRIPTOR \
    0x07, /* 描述符长度 -> 7字节 */\
    0x05, /* 描述符类型 -> 端点描述符 */\
    0x81, /* 端点地址 -> 输入IN端点1 */\
    0x03, /* 端点属性 -> 中断传输 */\
    0x08, 0x00, /* 最大包大小 -> 8字节 TODO:高速同步端点微帧*/\
    POLLLING_INTERVAL /* 根据 Report_Rate 设置轮询间隔 */
		
 /****************************************
 *             报告描述符                *
 ****************************************/
#define USBD_MOUSE_REPORT_DESCRIPTOR_SIZE  60
#define USBD_MOUSE_REPORT_DESCRIPTOR /* 使用Waratah自动生成 */\
    0x05, 0x01,     /* usage page (generic desktop). Global item, applies to all subsequent items   */\
    0x09, 0x02,     /* usage (mouse). Local item                                                    */\
    0xA1, 0x01,     /* collection (application)                                                     */\
    0x09, 0x01,     		/* usage (pointer)                                                              */\
    0xA1, 0x00,     		/* collection (physical)                                                        */\
		0x05, 0x09,     				/*   usage page (buttons). Global item, applies to all subsequent items         */\
    0x19, 0x01,     						/*   usage minimum (1)                                                          */\
    0x29, 0x08,     						/*   usage maximum (8)                                                          */\
    0x15, 0x00,     						/*   logical minimum (0)                                                        */\
    0x25, 0x01,     						/*   logical maximum (1)                                                        */\
    0x95, 0x08,     						/*   report count (8)                                                           */\
    0x75, 0x01,     						/*   report size (1)                                                            */\
    0x81, 0x02,     						/*   input (data, var, abs)                                                     */\
    0x05, 0x01,     				/*   usage page (generic desktop). Global item, applies to all subsequent items */\
		0x16, 0x00, 0x80,    				/*   LogicalMinimum(-32,768)										*/\
		0x26, 0xFF, 0x7F,    				/*   LogicalMaximum(32,767)										*/\
    0x75, 0x10,     						/*   report size (16)                                                            */\
    0x09, 0x30,     						/*   usage (X)                                                                  */\
    0x09, 0x31,     						/*   usage (Y)                                                                  */\
    0x95, 0x02,     						/*   report count (2)                                                           */\
    0x81, 0x06,     						/*   input 	(Data, Variable, Relative)																          */\
		0x05, 0x01,     				/*   usage page (generic desktop). Global item, applies to all subsequent items */\
		0x15, 0x81,     						/*   logical minimum (-127)                                                     */\
    0x25, 0x7F,     						/*   logical maximum (127)                                                      */\
    0x75, 0x08,     						/*   report size (8)                                                            */\
		0x09, 0x38,     						/*   usage wheel                                                                */\
    0x95, 0x01,     						/*   report count (1)                                                           */\
    0x81, 0x06,     						/*   input 	(Data, Variable, Relative)																          */\
    0xC0,           		/* end collection                                                               */\
    0xC0            /* End Collection 																																	*/

 /****************************************
 *                 函数  			           *
 ****************************************/
// USB标志操作函数
bool usb_device_get_mark_usbd_configured(void); 
void usb_device_set_mark_usbd_configured(bool value); 
bool usb_device_get_mark_usbd_suspended(void); 
void usb_device_set_mark_usbd_suspended(bool value); 
bool usb_device_get_mark_usbd_remote_wakeup_enabled(void); 
void usb_device_set_mark_usbd_remote_wakeup_enabled(bool value); 
bool usb_device_get_mark_send_device_data(void); 
void usb_device_set_mark_send_device_data(bool value); 
bool usb_device_get_mark_usbd_suspend_state_request(void); 
void usb_device_set_mark_usbd_suspend_state_request(bool value); 
bool usb_device_get_mark_device_has_motion(void);
void usb_device_set_mark_device_has_motion(bool value);
	
// USBD 配置函数
ret_code_t usb_device_configure_endpoint(uint8_t index, nrf_drv_usbd_ep_t ep);

// USBD 请求响应函数
void usb_device_respond_setup_request(nrf_drv_usbd_setup_t const * const p_setup, void const * p_data, size_t size);
void usb_device_respond_get_status_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_clear_feature_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_feature_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_get_descriptor_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_get_config_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_config_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_idle_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_interface_request(nrf_drv_usbd_setup_t const * const p_setup);
void usb_device_respond_set_protocol_request(nrf_drv_usbd_setup_t const * const p_setup);

// USBD 事件处理函数
void usb_device_event_handler(nrf_drv_usbd_evt_t const * const p_event);

// USBD 电源函数
void usb_device_power_event_handler(nrf_drv_power_usb_evt_t event);

// GPIO 函数
void usb_device_gpio_init(void);
void usb_device_gpiote_init(void);

// 复位函数
void usb_device_log_reset_reason(void);

// 挂起函数
void usb_device_force_suspend(void);
void usb_device_force_leaving_suspend(void);

// USBD 初始化函数
void usb_device_init(void);

#endif // USB_DEVICE_H
