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
#include "ws2812b.h"  // LED驱动

#include "app_scheduler.h" // app调度器

#include "nrf_drv_saadc.h"      // SAADC驱动库

/***************************************
 *             宏定义            	 	*
 ***************************************/
//SAADC
#define FILTER_WINDOW_SIZE 5     // 滑动窗口大小

// 调度器
#define SCHED_MAX_EVENT_DATA_SIZE       APP_TIMER_SCHED_EVENT_DATA_SIZE             /**< @brief 调度器事件的最大大小。 */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                80                                          /**< @brief 调度器队列中的最大事件数。对于序列化应用需要更多。 */
#else
#define SCHED_QUEUE_SIZE                40                                          /**< @brief 调度器队列中的最大事件数。 */
#endif

// 电池
# define BATTERY_HIGH_LIM 40
# define BATTERY_LOW_LIM 20

typedef enum {
    BATTERY_LOW,
    BATTERY_MIDDLE,
    BATTERY_HIGH
} battery_state_t; // 电池状态
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
void mouse_scheduler_init(void);

// 鼠标定时器函数
void ble_pairing_timeout_handler(void *p_context);
void mouse_timer_init(void);

// 电池电量函数
void mouse_saadc_init(void);
void mouse_saadc_callback(nrf_drv_saadc_evt_t const * p_event);
void mouse_battery_update_timer_handler(void *p_context);
uint8_t saadc_value_to_battery_percent(int16_t adc_value);
void mouse_battery_update_timer_start(void);

// 鼠标主轮询任务
void mouse_main_task(void);


#endif // MOUSE_H
