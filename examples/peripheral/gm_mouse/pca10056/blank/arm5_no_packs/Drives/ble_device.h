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

/***************************************
 *              宏定义             		 *
 ***************************************/
// 设备信息定义
#define DEVICE_NAME                     "AIALRA_GAMING_MOUSE"                                /**< @brief 设备名称，将包含在广播数据中。 */
#define MANUFACTURER_NAME               "AIALRA"                       /**< @brief 制造商名称，将传递给设备信息服务。 */

// 供应商和产品信息定义
#define PNP_ID_VENDOR_ID_SOURCE         0x02                                        /**< @brief 供应商ID来源。 */
#define PNP_ID_VENDOR_ID                0x1915                                      /**< @brief 供应商ID。 */
#define PNP_ID_PRODUCT_ID               0xEEEE                                      /**< @brief 产品ID。 */
#define PNP_ID_PRODUCT_VERSION          0x0001                                      /**< @brief 产品版本。 */

// BLE配置参数定义
#define APP_BLE_OBSERVER_PRIO           3                                           /**< @brief 应用的BLE观察者优先级，通常无需修改。 */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< @brief SoftDevice BLE配置标识。 */

// 电池服务定义
#define BATTERY_LEVEL_MEAS_INTERVAL     APP_TIMER_TICKS(2000)                       /**< @brief 电池电量测量间隔（ticks）。 */
#define MIN_BATTERY_LEVEL               81                                          /**< @brief 模拟电池的最小电量。 */
#define MAX_BATTERY_LEVEL               100                                         /**< @brief 模拟电池的最大电量。 */
#define BATTERY_LEVEL_INCREMENT         1                                           /**< @brief 每次模拟电池电量测量的增量。 */

// 连接参数定义
/*lint -emacro(524, MIN_CONN_INTERVAL) // Loss of precision */
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)            /**< @brief 最小连接间隔 */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)             /**< @brief 最大连接间隔 */
#define SLAVE_LATENCY                   0                                          	/**< @brief 从设备延迟。 */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(3000, UNIT_10_MS)             /**< @brief 连接监督超时（3000毫秒）。 */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< @brief 从触发事件（连接或通知开始）到首次调用连接参数更新的时间（5秒）。 */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< @brief 首次连接参数更新后每次调用的时间间隔（30秒）。 */
#define MAX_CONN_PARAM_UPDATE_COUNT     5                                           /**< @brief 在放弃连接参数协商之前的最大尝试次数。 */

// Swift Pair功能定义
#define SWIFT_PAIR_SUPPORTED            1                                           /**< @brief 支持Swift Pair功能。 */
#if SWIFT_PAIR_SUPPORTED == 1
#define MICROSOFT_VENDOR_ID             0x0006                                      /**< @brief Microsoft供应商ID。 */
#define MICROSOFT_BEACON_ID             0x03                                        /**< @brief Microsoft Beacon ID，表示支持Swift Pair功能。 */
#define MICROSOFT_BEACON_SUB_SCENARIO   0x00                                        /**< @brief Microsoft Beacon子场景，用于指示如何通过Swift Pair配对。 */
#define RESERVED_RSSI_BYTE              0x80                                        /**< @brief 保留的RSSI字节，用于保持向前和向后兼容性。 */
#endif

// 广播参数定义
#define APP_ADV_DIRECT_INTERVAL         0x0020                                   	/**< @brief 直接广播间隔（单位为0.625毫秒，对应于20毫秒）。 */
#define APP_ADV_FAST_INTERVAL           0x0028                                      /**< @brief 快速广播间隔（单位为0.625毫秒，对应于25毫秒）。 */
#define APP_ADV_SLOW_INTERVAL           0x00A0                                      /**< @brief 慢速广播间隔（单位为0.625毫秒，对应于100毫秒）。 */
#define APP_ADV_DIRECT_DURATION         1000                                        /**< @brief 直接广播的持续时间，单位为10毫秒。 */
#define APP_ADV_FAST_DURATION           3000                                        /**< @brief 快速广播的持续时间，单位为10毫秒。 */
#define APP_ADV_SLOW_DURATION           18000                                       /**< @brief 慢速广播的持续时间，单位为10毫秒。 */

// 安全参数定义
#define SEC_PARAM_BOND                  1                                           /**< @brief 是否进行配对。 */
#define SEC_PARAM_MITM                  0                                           /**< @brief 是否需要中间人保护。 */
#define SEC_PARAM_LESC                  0                                           /**< @brief 是否启用LE安全连接。 */
#define SEC_PARAM_KEYPRESS              0                                           /**< @brief 是否启用按键通知。 */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< @brief 无I/O能力。 */
#define SEC_PARAM_OOB                   0                                           /**< @brief 无带外数据。 */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< @brief 最小加密密钥大小。 */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< @brief 最大加密密钥大小。 */

// HID鼠标配置定义
#define BASE_USB_HID_SPEC_VERSION       0x0101                                      /**< @brief 应用实现的基本USB HID规范版本号。 */
#define INPUT_REPORT_COUNT              3                                           /**< @brief 本应用中的输入报告数量。 */
#define INPUT_REP_BUTTONS_LEN           3                                           /**< @brief 包含按钮数据的鼠标输入报告的长度。 */
#define INPUT_REP_MOVEMENT_LEN          4                                           /**< @brief 包含运动数据的鼠标输入报告的长度。 */
#define INPUT_REP_MEDIA_PLAYER_LEN      1                                           /**< @brief 包含媒体播放器数据的鼠标输入报告的长度。 */
#define INPUT_REP_BUTTONS_INDEX         0                                           /**< @brief 包含按钮数据的鼠标输入报告的索引。 */
#define INPUT_REP_MOVEMENT_INDEX        1                                           /**< @brief 包含运动数据的鼠标输入报告的索引。 */
#define INPUT_REP_MPLAYER_INDEX         2                                           /**< @brief 包含媒体播放器数据的鼠标输入报告的索引。 */
#define INPUT_REP_REF_BUTTONS_ID        1                                           /**< @brief 包含按钮数据的鼠标输入报告的引用ID。 */
#define INPUT_REP_REF_MOVEMENT_ID       2                                           /**< @brief 包含运动数据的鼠标输入报告的引用ID。 */
#define INPUT_REP_REF_MPLAYER_ID        3                                           /**< @brief 包含媒体播放器数据的鼠标输入报告的引用ID。 */


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
