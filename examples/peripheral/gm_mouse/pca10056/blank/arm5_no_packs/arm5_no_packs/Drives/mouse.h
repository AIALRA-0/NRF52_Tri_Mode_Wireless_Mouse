#ifndef MOUSE_H
#define MOUSE_H

/***************************************
 *              库文件             		 *
 ***************************************/
#include <stdint.h>   // 标准整数类型定义
#include <stdbool.h>  // 布尔类型定义
#include <stddef.h>   // 标准定义（NULL等）
#include "nrf_gpio.h"  // GPIO操作头文件
#include "paw3399.h"	// paw3399 传感器库
#include "usb_device.h" // USB 设备实例
#include "ble_device.h" // BLE 设备实例
#include "esb_device.h" // BLE 设备实例

/***************************************
 *             函数定义            	 *
 ***************************************/
 
// 鼠标辅助函数
void mouse_button_debounce(bool button_state, uint8_t *button_count, uint8_t *button_bit);
void mouse_dpi_update(bool increase);
void mouse_connection_mode_update(void);
void mouse_wait_for_ble_connection(void);
void mouse_ble_pairing_handler(void);
void mouse_ble_host_disconnect_handler(void);
void mouse_system_status_update(void);
void mouse_data_update(void);
void mouse_usb_data_send(void);
void mouse_ble_data_send(void);
void mouse_esb_data_send(void);
void mouse_suspend_status_update(void);

// 鼠标初始化函数
void mouse_init(void);
void mouse_wheel_data_init(void);

// 鼠标主轮询任务
void mouse_main_task(void);


#endif // MOUSE_H
