#ifndef BLE_DEVICE_H
#define BLE_DEVICE_H

/***************************************
 *              库文件             		 *
 ***************************************/
#include <stdint.h>   // 标准整数类型定义
#include <stdbool.h>  // 布尔类型定义
#include <stddef.h>   // 标准定义（NULL等）

// Nordic库
#include "nordic_common.h"  // 包含Nordic芯片的通用定义和功能，如工具宏等。
#include "nrf.h"            // 提供Nordic芯片的寄存器和内存映射定义。
#include "nrf_sdm.h"        // 提供SoftDevice（Nordic BLE协议栈）的管理功能。
#include "app_error.h"      // 提供错误检查和错误处理功能，便于调试和错误日志记录。
#include "nrf_delay.h"			// nrf延迟库

// BLE基础库
#include "ble.h"            // 包含BLE协议栈的基础API，提供BLE事件和结构定义。
#include "ble_err.h"        // 定义BLE错误码，用于错误检测和处理。
#include "ble_hci.h"        // 定义HCI（主机控制接口）协议的错误码。
#include "ble_srv_common.h" // BLE服务的通用接口，用于创建和管理BLE服务。
#include "ble_advdata.h"    // 提供BLE广播数据的设置功能。

// BLE服务库
#include "ble_hids.h"       // BLE HID（Human Interface Device）服务的接口，用于实现键盘或鼠标等HID设备。
#include "ble_bas.h"        // BLE电池服务接口，提供设备电池电量信息。
#include "ble_dis.h"        // BLE设备信息服务接口，提供设备的基本信息（如制造商、型号等）。

// BLE管理库
#include "ble_conn_params.h"// 管理BLE连接参数，包括连接间隔、超时等。
#include "bsp_btn_ble.h"    // 管理与BLE相关的按键事件，比如连接、断开等操作。
#include "app_scheduler.h"  // 应用程序调度器，用于将任务放入队列并逐步执行。
#include "nrf_sdh.h"        // SoftDevice handler管理，提供SoftDevice初始化和事件处理。
#include "nrf_sdh_soc.h"    // 提供SoC（系统级芯片）相关的事件管理功能。
#include "nrf_sdh_ble.h"    // SoftDevice的BLE管理接口，初始化BLE、处理BLE事件等。
#include "ble_advertising.h"// BLE广播功能管理，设置广播内容并控制广播状态。
#include "ble_conn_state.h" // 连接状态管理，提供连接状态查询等功能。
#include "peer_manager.h"   // 管理BLE设备的配对和加密连接。

// 系统管理库
#include "app_timer.h"      // 应用程序定时器接口，提供定时器功能支持。
#include "fds.h"            // Flash数据存储接口，用于在闪存中存储配置信息或数据。

// GATT管理 
#include "nrf_ble_gatt.h"   // GATT模块管理，提供GATT连接的设置和数据传输。
#include "nrf_ble_qwr.h"    // 队列写入模块，简化GATT的写操作，处理写入请求。

// 电源管理
#include "nrf_pwr_mgmt.h"   // 电源管理模块，提供低功耗和电源管理功能。

// 配对管理
#include "peer_manager_handler.h"  // 配对管理处理程序，处理配对和安全性相关事件。

// 日志管理
#include "nrf_log.h"             // 日志模块，用于记录调试和运行信息。
#include "nrf_log_ctrl.h"        // 日志控制模块，用于控制日志记录的启动和停止。
#include "nrf_log_default_backends.h" // 日志后端管理，定义日志输出的具体位置，如串口或存储。

// 测试管理
#include "sensorsim.h"      // 模拟传感器数据生成，用于测试传感器相关应用。

// LED指示灯
#include "ws2812b.h" // WS2812B灯珠

 /****************************************
 *                 函数  			           *
 ****************************************/
// BLE 标志操作函数 
void ble_device_set_mark_in_boot_mode(bool boot_mode);
bool ble_device_get_mark_in_boot_mode(void);
bool ble_device_get_mark_device_disconnected(void);
void ble_device_set_mark_device_disconnected(bool is_disconncted);
bool ble_device_get_mark_need_host_delete(void);
void ble_device_set_mark_need_host_delete(bool need_delete);

// BLE 模块操作函数 
ble_hids_t* ble_device_get_module_hid_service(void);
uint16_t ble_device_get_module_connection_handle(void);
const app_timer_id_t* ble_device_get_module_led_timer_handle(void);

// BLE 电池函数
void ble_device_battery_timer_timeout_handler(void * p_context);
void ble_device_battery_level_update(uint8_t  battery_level);
void ble_device_battery_service_init(void);
void ble_device_battery_sensor_simulator_init(void);  

// BLE 功率管理函数
void ble_device_power_management_init(void);

// BLE 堆栈和协议模块函数
void ble_device_ble_stack_init(void);
void ble_device_observer_event_handler(ble_evt_t const * p_ble_evt, void * p_context);
void ble_device_gap_init(void);
void ble_device_gatt_init(void);
void ble_device_connection_parameters_init(void);
void ble_device_connection_parameters_error_handler(uint32_t nrf_error);
	
// BLE 模式按钮和LED函数 
void ble_device_disconnect(void);
void ble_device_enter_sleep_mode(void);
void ble_device_led_timer_handler(void *p_context);
void ble_device_led_timer_stop(void);

// BLE 广播函数
void ble_device_advertising_init(void);
void ble_device_advertising_event_handler(ble_adv_evt_t ble_adv_evt);
void ble_device_advertising_error_handler(uint32_t nrf_error);
void ble_device_advertising_start(bool erase_bonds, bool use_whitelist);
bool has_valid_peer(void);
bool has_device_identities(void);

// BLE 白名单函数
void ble_device_set_filtered_device_identity_list(pm_peer_id_list_skip_t skip);
void ble_device_set_filtered_white_list(pm_peer_id_list_skip_t skip);

// BLE 配对模块函数
void ble_device_peer_manager_init(void);
void ble_device_peer_manager_event_handler(pm_evt_t const * p_evt);
void ble_device_delete_bonds(void);
void ble_device_stop_advertising(void);
void ble_device_generate_random_private_address(void);
uint32_t ble_device_get_peer_count(void);

// BLE 服务模块函数 
void ble_device_queue_write_init(void);
void ble_device_queue_write_error_handler(uint32_t nrf_error);
void ble_device_info_system_init(void);

// BLE HID模块函数
void ble_device_hid_service_init(void);
void ble_device_hid_service_event_handler(ble_hids_t * p_hids, ble_hids_evt_t * p_evt);
void ble_device_hid_service_error_handler(uint32_t nrf_error);

// BLE 初始化函数
void ble_device_timer_init(void);
void ble_device_idle_state_handle(void);
void ble_device_init(void);
	
#endif // BLE_DEVICE_H
