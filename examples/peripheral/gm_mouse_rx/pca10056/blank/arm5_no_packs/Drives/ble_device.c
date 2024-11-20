/***************************************
 *              库文件             		 *
 ***************************************/
#include "ble_device.h"

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
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)            /**< @brief 最小连接间隔（7.5毫秒）。 */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(15, UNIT_1_25_MS)             /**< @brief 最大连接间隔（15毫秒）。 */
#define SLAVE_LATENCY                   20                                          /**< @brief 从设备延迟。 */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(3000, UNIT_10_MS)             /**< @brief 连接监督超时（3000毫秒）。 */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< @brief 从触发事件（连接或通知开始）到首次调用连接参数更新的时间（5秒）。 */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< @brief 首次连接参数更新后每次调用的时间间隔（30秒）。 */
#define MAX_CONN_PARAM_UPDATE_COUNT     3                                           /**< @brief 在放弃连接参数协商之前的最大尝试次数。 */


// Swift Pair功能定义
#define SWIFT_PAIR_SUPPORTED            1                                           /**< @brief 支持Swift Pair功能。 */
#if SWIFT_PAIR_SUPPORTED == 1
#define MICROSOFT_VENDOR_ID             0x0006                                      /**< @brief Microsoft供应商ID。 */
#define MICROSOFT_BEACON_ID             0x03                                        /**< @brief Microsoft Beacon ID，表示支持Swift Pair功能。 */
#define MICROSOFT_BEACON_SUB_SCENARIO   0x00                                        /**< @brief Microsoft Beacon子场景，用于指示如何通过Swift Pair配对。 */
#define RESERVED_RSSI_BYTE              0x80                                        /**< @brief 保留的RSSI字节，用于保持向前和向后兼容性。 */
#endif

// 广播参数定义
#define APP_ADV_FAST_INTERVAL           0x0028                                      /**< @brief 快速广播间隔（单位为0.625毫秒，对应于25毫秒）。 */
#define APP_ADV_SLOW_INTERVAL           0x00A0                                      /**< @brief 慢速广播间隔（单位为0.625毫秒，对应于100毫秒）。 */
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
#define INPUT_REP_BUTTONS_LEN           6                                           /**< @brief 包含按钮数据的鼠标输入报告的长度。 */
#define INPUT_REP_MOVEMENT_LEN          4                                           /**< @brief 包含运动数据的鼠标输入报告的长度。 */
#define INPUT_REP_MEDIA_PLAYER_LEN      1                                           /**< @brief 包含媒体播放器数据的鼠标输入报告的长度。 */
#define INPUT_REP_BUTTONS_INDEX         0                                           /**< @brief 包含按钮数据的鼠标输入报告的索引。 */
#define INPUT_REP_MOVEMENT_INDEX        1                                           /**< @brief 包含运动数据的鼠标输入报告的索引。 */
#define INPUT_REP_MPLAYER_INDEX         2                                           /**< @brief 包含媒体播放器数据的鼠标输入报告的索引。 */
#define INPUT_REP_REF_BUTTONS_ID        1                                           /**< @brief 包含按钮数据的鼠标输入报告的引用ID。 */
#define INPUT_REP_REF_MOVEMENT_ID       2                                           /**< @brief 包含运动数据的鼠标输入报告的引用ID。 */
#define INPUT_REP_REF_MPLAYER_ID        3                                           /**< @brief 包含媒体播放器数据的鼠标输入报告的引用ID。 */

// 调度器配置定义
#define SCHED_MAX_EVENT_DATA_SIZE       APP_TIMER_SCHED_EVENT_DATA_SIZE             /**< @brief 调度器事件的最大大小。 */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                20                                          /**< @brief 调度器队列中的最大事件数。对于序列化应用需要更多。 */
#else
#define SCHED_QUEUE_SIZE                10                                          /**< @brief 调度器队列中的最大事件数。 */
#endif

/***************************************
 *              BLE 标志           	  *	
 **************************************/ 
// HID模块
static bool              mark_in_boot_mode = false;                                    /**< @brief 当前协议模式，指示是否处于引导模式。 */

/***************************************
 *           BLE 标志操作函数           *	
 **************************************/ 
 /**@brief 设置 mark_in_boot_mode 标志的函数。
 *
 * @param[in] boot_mode  布尔值，true 表示进入 Boot 模式，false 表示退出 Boot 模式。
 */
void ble_device_set_mark_in_boot_mode(bool boot_mode)
{
    mark_in_boot_mode = boot_mode;
}

/**@brief 获取 mark_in_boot_mode 标志的函数。
 *
 * @return 当前 mark_in_boot_mode 标志的布尔值，true 表示处于 Boot 模式，false 表示处于 Report 模式。
 */
bool ble_device_get_mark_in_boot_mode(void)
{
    return mark_in_boot_mode;
}

/***************************************
 *              BLE 模块           	  *	
 **************************************/ 
 
// BLE电池电量模块
APP_TIMER_DEF(module_battery_timer);								/**< @brief 定义并配置一个具有日志功能的应用定时器实例，使得定时器的创建和日志管理变得更加简洁和一致 */
static sensorsim_cfg_t   module_battery_simulation_cfg;         /**< @brief 电池电量传感器模拟器的配置。 */
static sensorsim_state_t module_battery_simulation_state;       /**< @brief 电池电量传感器模拟器的状态。 */
BLE_BAS_DEF(module_ble_battery_service);                   /**< @brief 电池服务实例，用于与BLE设备交互电池信息。 */

// BLE链接模块
static uint16_t          module_connection_handle  = BLE_CONN_HANDLE_INVALID;                  /**< @brief 当前连接的句柄，BLE_CONN_HANDLE_INVALID表示无连接。 */
NRF_BLE_GATT_DEF(module_gatt);                                                           /**< @brief GATT模块实例，用于处理通用属性配置文件的连接。 */

// 队列写入模块
NRF_BLE_QWR_DEF(module_queue_write);                                                             /**< @brief 队列写入模块的上下文，用于管理写请求。 */

// BLE广播模块
BLE_ADVERTISING_DEF(module_ble_advertising);                                                 /**< @brief 广播模块实例，用于管理BLE广播。 */
static ble_uuid_t        module_advertising_uuids[] =                                            /**< @brief 广播中使用的服务UUID（通用唯一标识符）。 */
{
    {BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE, BLE_UUID_TYPE_BLE}                    /**< @brief 人机接口设备服务的UUID。 */
};
BLE_ADVERTISING_DEF(module_advertising);                                                 /**< @brief 广播模块实例，用于管理BLE广播。 */

// BLE配对模块
static pm_peer_id_t      module_peer_device_id;                                                 /**< @brief 当前已绑定的中心设备的设备引用句柄。 */
 
// SWIFT PAIR 配对模块，这是微软的一种蓝牙配对功能，允许设备快速配对
#if SWIFT_PAIR_SUPPORTED == 1
static uint8_t module_swift_pair_payload[] =                                                     /**< @brief 微软Swift Pair功能的广播数据结构负载。 */
{
    MICROSOFT_BEACON_ID,                                                            /**< @brief 微软信标 ID，用于标识这是一个支持 Swift Pair 功能的设备。具体值由微软定义。 */
    MICROSOFT_BEACON_SUB_SCENARIO,                                                  /**< @brief 微软信标的子场景，进一步描述该信标的作用或类型。该值帮助设备根据场景（如鼠标、键盘）做出相应的配对提示。 */
    RESERVED_RSSI_BYTE                                                              /**< @brief 保留的 RSSI 字节，用于表示接收信号强度指示（RSSI）。它通常是一个固定值，以供 Windows 使用。 */
};
static ble_advdata_manuf_data_t module_swift_pair_manufacturer_advertising_data =                                /**< @brief 微软Swift Pair功能的广播数据结构。 */
{
    .company_identifier = MICROSOFT_VENDOR_ID,                                      /**< @brief 微软公司标识符。 */
    .data               =
    {
        .size   = sizeof(module_swift_pair_payload),                                             /**< @brief 数据的大小（字节数） */
        .p_data = &module_swift_pair_payload[0]                                                  /**< @brief 指向广播数据负载的指针。 */
    }
};
static ble_advdata_t module_swift_pair_advertising_data;                                                  /**< @brief Swift Pair的BLE广播数据。 */
#endif

// BLE HID模块
BLE_HIDS_DEF(module_hid_service,                                     /**< HID服务实例，用于处理输入设备的数据传输。 */
             NRF_SDH_BLE_TOTAL_LINK_COUNT,                                          /**< 支持的BLE连接总数。 */
             INPUT_REP_BUTTONS_LEN,                                                 /**< 输入报告中按钮数据的长度。 */
             INPUT_REP_MOVEMENT_LEN,                                                /**< 输入报告中移动数据的长度。 */
             INPUT_REP_MEDIA_PLAYER_LEN);                                           /**< 输入报告中媒体播放器数据的长度。 */

/***************************************
 *           BLE 模块操作函数           *	
 **************************************/ 
/**@brief 获取 module_hid_service 实例的函数。
 *
 * @return 返回 BLE HID 服务实例的指针。
 */
ble_hids_t* ble_device_get_module_hid_service(void)
{
    return &module_hid_service;
}


/**@brief 获取当前连接句柄的函数。
 *
 * @return 返回当前连接句柄的值。如果无连接，则返回 BLE_CONN_HANDLE_INVALID。
 */
uint16_t ble_device_get_module_connection_handle(void)
{
    return module_connection_handle;
}

 /****************************************
 *             BLED 初始化函数  			     *
 ****************************************/
 /**
 * @brief 该函数执行系统初始化、蓝牙堆栈设置和必要的服务启动，随后进入主循环，处理空闲状态。
 * 
 * @return int 程序返回状态，通常不会返回值。
 */
void ble_device_init(void)
{
    bool erase_bonds;  /**< 指示是否需要擦除已绑定的设备信息。 */

    // 电池监控模块初始化
		ble_device_battery_timer_init();              /**< 初始化定时器模块，为系统定时事件提供支持。 */
		ble_device_battery_sensor_simulator_init();    									/**< 初始化传感器模拟器，生成模拟传感器数据（如电池电量）。 */
	
		// 功率管理模块初始化
		ble_device_power_management_init();    /**< 初始化电源管理模块，用于控制设备的低功耗状态。 */
	
		// BLE模式按钮和LED初始化
		ble_device_buttons_leds_init(&erase_bonds);				/**< 初始化按钮和LED模块，包含用户输入和指示灯控制功能，检查是否需要擦除绑定。 */
	
		// BLE堆栈和协议模块初始化
		ble_device_ble_stack_init();           /**< 初始化BLE堆栈，开启蓝牙功能。 */
		ble_device_gap_init();					 /**< 设置GAP（通用访问配置文件）参数，例如设备名称、可见性和连接间隔。 */
		ble_device_gatt_init();                /**< 初始化GATT（通用属性配置文件）模块，处理属性和服务的操作, 规定了数据通信的框架。 */
	  ble_device_connection_parameters_init(); /**< 初始化连接参数模块，确保BLE连接的稳定性。 */
	
		// 广播和配对模块初始化
		ble_device_advertising_init();         /**< 初始化广播模块，管理设备的广播信息。 */
	  ble_device_peer_manager_init();				/**< 初始化Peer Manager模块，管理设备配对、绑定和加密功能。 */
	
		// 服务模块初始化
		ble_device_queue_write_init();   /**< 初始化Queued Write模块，支持长写操作。 */
    ble_device_info_system_init();   /**< 初始化设备信息服务（DIS），提供设备基本信息。 */
		ble_device_battery_service_init();  /**< 初始化电池服务（BAS），用于监测和报告电池电量。 */
		
		// HID模块初始化
		ble_device_hid_service_init();
		
		// 事件调度模块初始化
		ble_device_scheduler_init();
		
		// 开启扫描窗口
		NRF_LOG_INFO("BLE device initialized. Starting advertising..."); /**< 日志输出：HID鼠标示例已启动。 */
    ble_device_battery_timer_start();         /**< 启动电池报告定时器事件。 */
    ble_device_advertising_start(erase_bonds); /**< 开始广播，是否擦除绑定取决于erase_bonds标志。 */
		NRF_LOG_INFO("Waiting for pairing and connection...");
}

/**@brief 处理空闲状态（主循环）函数。
 *
 * @details 该函数用于处理空闲状态下的任务调度。如果没有待处理的日志操作，则进入低功耗模式，
 *          使设备进入睡眠状态，直到下一个事件发生。
 */
void ble_device_idle_state_handle(void)
{
    app_sched_execute(); /**< 执行调度器中的待处理任务。 */

}

 /***************************************
 *            BLE 电池函数          *	
 **************************************/
/**@brief 定时器初始化函数。
 *
 * @details 该函数负责初始化定时器模块，并创建一个用于电池电量测量的定时器。
 */
void ble_device_battery_timer_init(void)
{
    ret_code_t err_code;  /**< 用于存储函数返回的错误代码。 */

    err_code = app_timer_init();  /**< 初始化应用程序定时器模块。 */
    APP_ERROR_CHECK(err_code);     /**< 检查定时器初始化是否成功，出错则触发错误处理。 */

    // 创建电池定时器。
    err_code = app_timer_create(&module_battery_timer,               /**< 定时器句柄，用于引用电池定时器。 */
                                APP_TIMER_MODE_REPEATED,           /**< 定时器模式，设置为重复模式。 */
                                ble_device_battery_timer_timeout_handler); /**< 定时器超时处理函数，当定时器超时时调用该函数。 */
    APP_ERROR_CHECK(err_code);     /**< 检查定时器创建是否成功，出错则触发错误处理。 */
}

/**@brief 电池测量定时器超时处理函数。
 *
 * @details 每当电池电量测量定时器到期时，将调用此函数。函数内部执行电池电量更新操作。
 *
 * @param[in]   p_context   指向上下文的指针，用于从app_start_timer()调用传递任意信息，
 *                          在本函数中未使用此参数。
 */
void ble_device_battery_timer_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);     /**< 标记p_context为未使用参数，避免编译器警告。 */
    ble_device_battery_level_update();          /**< 调用电池电量更新函数，将当前电量信息发送至中心设备。 */
}

/**@brief 执行电池测量并更新电池服务中的电池电量特征值。
 *
 * @details 该函数通过传感器模拟器获取当前电池电量值，并将其更新到电池服务（Battery Service）的电池电量特征值中。
 *          如果更新过程中出现错误（非预期的错误），将调用错误处理程序。
 */
void ble_device_battery_level_update(void)
{
    ret_code_t err_code;             /**< 用于存储函数返回的错误代码。 */
    uint8_t  battery_level;          /**< 当前电池电量值，单位为百分比（0-100）。 */

    battery_level = (uint8_t)sensorsim_measure(&module_battery_simulation_state, &module_battery_simulation_cfg); /**< 通过传感器模拟器测量电池电量。 */

    err_code = ble_bas_battery_level_update(&module_ble_battery_service, battery_level, BLE_CONN_HANDLE_ALL);  /**< 将测量的电池电量更新到电池服务特征值中。 */
    if ((err_code != NRF_SUCCESS) &&                                                     /**< 检查更新操作是否成功或处于合理状态。 */
        (err_code != NRF_ERROR_BUSY) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != NRF_ERROR_FORBIDDEN) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       )
    {
        APP_ERROR_HANDLER(err_code);  /**< 若发生非预期错误，调用错误处理程序。 */
    }
}

/**@brief 初始化电池服务（Battery Service）的函数。
 *
 * @details 该函数配置并初始化电池服务，用于向中心设备报告当前电池电量，并支持电池电量通知功能。
 */
void ble_device_battery_service_init(void)
{
    ret_code_t     err_code;       /**< 用于存储函数返回的错误代码。 */
    ble_bas_init_t bas_init_obj;   /**< 电池服务初始化结构体，用于配置电池服务。 */

    memset(&bas_init_obj, 0, sizeof(bas_init_obj)); /**< 清零电池服务初始化结构体。 */

    bas_init_obj.evt_handler          = NULL;           /**< 不使用事件处理程序。 */
    bas_init_obj.support_notification = true;           /**< 启用电池电量通知功能。 */
    bas_init_obj.p_report_ref         = NULL;           /**< 不使用报告引用。 */
    bas_init_obj.initial_batt_level   = 100;            /**< 设置初始电池电量为100%。 */

    // 配置电池服务的安全等级
    bas_init_obj.bl_rd_sec        = SEC_JUST_WORKS;     /**< 设置电池电量读取的安全级别。 */
    bas_init_obj.bl_cccd_wr_sec   = SEC_JUST_WORKS;     /**< 设置CCCD（客户端特征配置描述符）写入的安全级别。 */
    bas_init_obj.bl_report_rd_sec = SEC_JUST_WORKS;     /**< 设置电池报告读取的安全级别。 */

    err_code = ble_bas_init(&module_ble_battery_service, &bas_init_obj);     /**< 初始化电池服务模块。 */
    APP_ERROR_CHECK(err_code);                          /**< 检查电池服务初始化是否成功，出错则触发错误处理。 */
}

/**@brief 初始化电池传感器模拟器的函数。
 *
 * @details 该函数配置电池电量模拟器的参数，包括最小电量、最大电量和电量增量等，
 *          用于模拟电池电量的变化。
 */
void ble_device_battery_sensor_simulator_init(void)
{
    module_battery_simulation_cfg.min          = MIN_BATTERY_LEVEL;      /**< 设置模拟电池电量的最小值。 */
    module_battery_simulation_cfg.max          = MAX_BATTERY_LEVEL;      /**< 设置模拟电池电量的最大值。 */
    module_battery_simulation_cfg.incr         = BATTERY_LEVEL_INCREMENT; /**< 设置电池电量的增量，每次更新时电量变化的值。 */
    module_battery_simulation_cfg.start_at_max = true;                   /**< 指定模拟器从最大电量开始。 */

    sensorsim_init(&module_battery_simulation_state, &module_battery_simulation_cfg); /**< 初始化电池模拟器状态，应用配置的参数。 */
}

/**@brief 启动定时器的函数。
 *
 * @details 该函数启动用于电池电量测量的定时器，使其按设定的间隔触发电池电量更新事件。
 */
void ble_device_battery_timer_start(void)
{
    ret_code_t err_code; /**< 用于存储函数返回的错误代码。 */

    err_code = app_timer_start(module_battery_timer, BATTERY_LEVEL_MEAS_INTERVAL, NULL); /**< 启动电池测量定时器，按照指定的测量间隔触发。 */
    APP_ERROR_CHECK(err_code); /**< 检查定时器启动是否成功，出错则触发错误处理。 */
}


 /***************************************
 *            BLE 功率管理函数          *	
 **************************************/ 
/**@brief 初始化电源管理模块的函数。
 *
 * @details 该函数负责初始化电源管理模块（Power Management），
 *          以便设备在空闲时可以进入低功耗模式，从而节省电量。
 *          如果初始化失败，将调用错误处理程序。
 */
void ble_device_power_management_init(void)
{
    ret_code_t err_code;                /**< 用于存储函数返回的错误代码。 */
    err_code = nrf_pwr_mgmt_init();     /**< 初始化电源管理模块，启用低功耗功能。 */
    APP_ERROR_CHECK(err_code);          /**< 检查电源管理模块是否初始化成功，出错则触发错误处理。 */
}

 /***************************************
 *        BLE 模式按钮和LED函数(TODO)         *	
 **************************************/ 

/**@brief 按钮和LED的初始化函数。
 *
 * @details 该函数初始化板载的按钮和LED，以便在应用中使用。同时会检查是否按下了用于清除绑定数据的按钮。
 *          如果在唤醒时按下了清除绑定按钮，将通过参数`p_erase_bonds`返回该信息。
 *
 * @param[out] p_erase_bonds  如果按下了清除绑定数据的按钮，该参数将返回true，表示需要清除设备的绑定信息。
 */
void ble_device_buttons_leds_init(bool * p_erase_bonds)
{
    ret_code_t err_code;               /**< 用于存储函数返回的错误代码。 */
    bsp_event_t startup_event;         /**< 用于存储按钮的启动事件，例如清除绑定数据事件。 */

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, ble_device_buttons_event_handler);  /**< 初始化LED和按钮，设置事件处理函数。 */
    APP_ERROR_CHECK(err_code);                                                /**< 检查按钮和LED初始化是否成功，出错则触发错误处理。 */

    err_code = bsp_btn_ble_init(NULL, &startup_event);                         /**< 初始化BLE按钮功能，检测是否有启动事件（如清除绑定）。 */
    APP_ERROR_CHECK(err_code);                                                /**< 检查BLE按钮初始化是否成功，出错则触发错误处理。 */

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);         /**< 如果检测到清除绑定事件，则设置p_erase_bonds为true。 */
}

/**@brief 处理来自按钮模块的事件的函数。
 *
 * @details 该函数根据不同的按钮事件执行相应的操作，包括进入睡眠模式、断开连接、重启广播以及发送鼠标移动信号等。
 *
 * @param[in]   event   按钮按下产生的事件类型。
 */
void ble_device_buttons_event_handler(bsp_event_t event)
{
    ret_code_t err_code;  /**< 用于存储函数返回的错误代码。 */

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            ble_device_enter_sleep_mode(); /**< 进入睡眠模式以节省功耗。 */
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(module_connection_handle, /**< 断开当前BLE连接。 */
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)        /**< 检查是否在无效状态下调用了断开操作。 */
            {
                APP_ERROR_CHECK(err_code); /**< 检查断开连接操作是否成功，出错则触发错误处理。 */
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (module_connection_handle == BLE_CONN_HANDLE_INVALID)   /**< 若设备当前未连接，则重启不带白名单的广播。 */
            {
                err_code = ble_advertising_restart_without_whitelist(&module_ble_advertising); /**< 重启广播，不使用白名单。 */
                if (err_code != NRF_ERROR_INVALID_STATE)                             /**< 检查广播重启是否在有效状态下。 */
                {
                    APP_ERROR_CHECK(err_code); /**< 检查广播重启操作是否成功，出错则触发错误处理。 */
                }
            }
            break;

        default:
            break;
    }
}

/**@brief 使芯片进入睡眠模式的函数。
 *
 * @details 该函数将芯片置于系统关闭（system-off）模式，以实现最低功耗。
 *          唤醒时会导致芯片复位，从而重新启动应用程序。
 *
 * @note 该函数不会返回，一旦进入system-off模式，设备只能通过外部事件（如按钮唤醒）恢复。
 */
void ble_device_enter_sleep_mode(void)
{
    ret_code_t err_code; /**< 用于存储函数返回的错误代码。 */

    err_code = bsp_indication_set(BSP_INDICATE_IDLE); /**< 设置设备的空闲指示（LED等指示灯）。 */
    APP_ERROR_CHECK(err_code);

    // 准备唤醒按钮。
    err_code = bsp_btn_ble_sleep_mode_prepare(); /**< 配置按钮以允许在睡眠模式下唤醒设备。 */
    APP_ERROR_CHECK(err_code);

    // 进入系统关闭模式（此函数不会返回，唤醒将导致设备复位）。
    err_code = sd_power_system_off(); /**< 调用SoftDevice函数进入system-off模式。 */
    APP_ERROR_CHECK(err_code);
}


 /***************************************
 *         BLE 堆栈和协议模块函数        *	
 **************************************/ 

/**@brief 初始化BLE堆栈的函数。
 *
 * @details 该函数负责启用SoftDevice（Nordic的BLE协议栈），配置BLE堆栈的默认设置，并注册BLE事件中断处理程序。
 */
void ble_device_ble_stack_init(void)
{
    ret_code_t err_code;  /**< 用于存储函数返回的错误代码。 */

    err_code = nrf_sdh_enable_request();  /**< 启用SoftDevice以激活BLE协议栈。 */
    APP_ERROR_CHECK(err_code);            /**< 检查SoftDevice是否成功启用，出错则触发错误处理。 */

    // 配置BLE堆栈，使用默认配置。获取应用程序RAM的起始地址。
    uint32_t ram_start = 0;               /**< 应用程序RAM的起始地址，供BLE堆栈配置使用。 */
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);  /**< 设置默认BLE连接配置，并调整RAM起始地址。 */
    APP_ERROR_CHECK(err_code);            /**< 检查BLE堆栈配置是否成功，出错则触发错误处理。 */

    // 启用BLE堆栈。
    err_code = nrf_sdh_ble_enable(&ram_start);  /**< 根据调整后的RAM起始地址启用BLE堆栈。 */
    APP_ERROR_CHECK(err_code);                  /**< 检查BLE堆栈是否成功启用，出错则触发错误处理。 */

    // 注册BLE事件处理程序。
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_device_observer_event_handler, NULL);  /**< 注册BLE事件观察者，设置事件优先级和处理函数。 */
}

/**@brief 处理BLE事件的函数。
 *
 * @details 该函数根据不同的BLE事件类型执行相应的操作，包括连接、断开连接、PHY更新请求和GATT超时处理等。
 *
 * @param[in]   p_ble_evt   指向BLE堆栈事件的指针，包含事件类型及相关信息。
 * @param[in]   p_context   未使用的参数。
 */
void ble_device_observer_event_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;  /**< 用于存储函数返回的错误代码。 */

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");  /**< 日志信息：已连接。 */
						NRF_LOG_FLUSH();
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED); /**< 设置连接状态的LED指示。 */
            APP_ERROR_CHECK(err_code);  /**< 检查LED指示是否成功，出错则触发错误处理。 */

            module_connection_handle = p_ble_evt->evt.gap_evt.conn_handle; /**< 保存当前连接句柄。 */

            err_code = nrf_ble_qwr_conn_handle_assign(&module_queue_write, module_connection_handle); /**< 将连接句柄分配给Queued Write模块。 需要发送的数据超过限制时，需要使用长写来分段传输*/
            APP_ERROR_CHECK(err_code); /**< 检查分配操作是否成功，出错则触发错误处理。 */
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");  /**< 日志信息：已断开连接。 */
            // LED指示将在广播启动时更改。

            module_connection_handle = BLE_CONN_HANDLE_INVALID; /**< 将连接句柄设置为无效，表示当前无连接。 */
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request."); /**< 日志信息：PHY更新请求。 */
            ble_gap_phys_t const phys =          /**< 设置PHY参数为自动选择模式。 */
            {
                .rx_phys = BLE_GAP_PHY_AUTO,     /**< 接收PHY设置为自动选择。 */
                .tx_phys = BLE_GAP_PHY_AUTO,     /**< 发送PHY设置为自动选择。 */
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys); /**< 发送PHY更新请求。 */
            APP_ERROR_CHECK(err_code); /**< 检查PHY更新请求是否成功，出错则触发错误处理。 */
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            NRF_LOG_DEBUG("GATT Client Timeout."); /**< 日志信息：GATT客户端超时。 */
            // GATT客户端超时事件，触发断开连接。
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code); /**< 检查断开连接操作是否成功，出错则触发错误处理。 */
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            NRF_LOG_DEBUG("GATT Server Timeout."); /**< 日志信息：GATT服务器超时。 */
            // GATT服务器超时事件，触发断开连接。
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code); /**< 检查断开连接操作是否成功，出错则触发错误处理。 */
            break;

        default:
            // 无需实现其他事件。
            break;
    }
}

/**@brief 初始化GAP（通用访问配置文件）参数的函数。
 *
 * @details 该函数设置设备的GAP参数，包括设备名称、外观以及首选连接参数。
 */
void ble_device_gap_init(void)
{
    ret_code_t              err_code;          /**< 用于存储函数返回的错误代码。 */
    ble_gap_conn_params_t   gap_conn_params;   /**< GAP连接参数结构体。 */
    ble_gap_conn_sec_mode_t sec_mode;          /**< GAP连接安全模式。 */

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode); /**< 设置连接安全模式为开放，无需配对即可连接。 */

    err_code = sd_ble_gap_device_name_set(&sec_mode,               /**< 设置softdevice设备名称的安全模式。 */
                                          (const uint8_t *)DEVICE_NAME, /**< 设置softdevice设备名称。 */
                                          strlen(DEVICE_NAME));         /**< 设置softdevice设备名称的长度。 */
    APP_ERROR_CHECK(err_code);         /**< 检查设备名称设置是否成功，出错则触发错误处理。 */

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HID_MOUSE); /**< 设置设备外观为HID鼠标。 */
    APP_ERROR_CHECK(err_code);         /**< 检查外观设置是否成功，出错则触发错误处理。 */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params)); /**< 将连接参数结构体清零以初始化。 */

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL; /**< 设置最小连接间隔。 */
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL; /**< 设置最大连接间隔。 */
    gap_conn_params.slave_latency     = SLAVE_LATENCY;     /**< 设置从设备延迟，以减少通信频率。 */
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;  /**< 设置连接超时，定义连接断开时间。 */

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);      /**< 设置首选连接参数（PPCP），供中心设备参考。BLE 从设备可以通过 PPCP 建议连接参数，但主设备不一定严格遵循这些参数 */
    APP_ERROR_CHECK(err_code);         /**< 检查连接参数设置是否成功，出错则触发错误处理。 */
}

/**@brief 初始化GATT（通用属性配置文件）模块的函数。
 *
 * @details 该函数负责初始化GATT模块，以便设备可以使用BLE的GATT功能进行数据传输。
 *          初始化过程中未指定自定义事件处理函数，因此GATT模块将使用默认配置。
 */
void ble_device_gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&module_gatt, NULL); /**< 初始化GATT模块，第二个参数为事件处理程序，此处为NULL表示使用默认设置。 */
    APP_ERROR_CHECK(err_code);                             /**< 检查GATT初始化是否成功，出错则触发错误处理。 */
}

/**@brief 初始化连接参数模块的函数。
 *
 * @details 该函数配置和初始化连接参数模块，以便在连接期间动态调整连接参数。
 *          如果连接参数的更新请求失败，设备不会断开连接，而是继续使用现有的参数。
*						连接建立后再更新参数，可以根据设备的当前状态（如传输数据、低功耗等）调整到最合适的连接参数，以便在传输速率和功耗之间找到最佳平衡
 */
void ble_device_connection_parameters_init(void)
{
    ret_code_t             err_code;    /**< 用于存储函数返回的错误代码。 */
    ble_conn_params_init_t cp_init;     /**< 连接参数初始化结构体，用于配置连接参数模块。 */

    memset(&cp_init, 0, sizeof(cp_init)); /**< 将连接参数初始化结构体清零。 */

    cp_init.p_conn_params                  = NULL;                     /**< 指向自定义连接参数的指针。如果设置为 NULL，则使用 BLE 协议栈中的默认连接参数。 */
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY; /**< 初次更新连接参数的延迟时间,设备在连接后何时首次尝试更新连接参数。 */
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;  /**< 后续更新连接参数的延迟时间, 如果第一次更新失败，模块会在这个延迟时间后再次尝试更新连接参数。 */
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAM_UPDATE_COUNT;   /**< 最大连接参数更新尝试次数, 如果超过这个次数仍然失败，设备将不再尝试更新参。 */
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;       /**< 设置为无效句柄，表示不依赖通知的启动，连接参数更新将在其他设置的时机发生。 */
    cp_init.disconnect_on_fail             = false;                         /**< 若更新失败则不断开连接。 */
    cp_init.evt_handler                    = NULL;                          /**< 不使用自定义事件处理函数。 */
    cp_init.error_handler                  = ble_device_connection_parameters_error_handler;     /**< 指定错误处理函数。 */

    err_code = ble_conn_params_init(&cp_init); /**< 初始化连接参数模块。 */
    APP_ERROR_CHECK(err_code);                 /**< 检查连接参数初始化是否成功，出错则触发错误处理。 */
}

/**@brief 处理连接参数模块错误的函数。
 *
 * @param[in]   nrf_error   包含错误信息的错误代码。
 *
 * @details 如果连接参数模块初始化或运行过程中出现错误，此函数将处理该错误。
 */
void ble_device_connection_parameters_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error); /**< 调用错误处理程序并传入错误代码。 */
}

 /***************************************
 *         BLE 广播模块函数        		*	
 **************************************/ 
/**@brief 初始化BLE广播功能的函数。
 *
 * @details 该函数配置广播数据、广播模式和定时器等参数，以启动和管理BLE设备的广播。
 */
void ble_device_advertising_init(void)
{
    ret_code_t             err_code;     /**< 用于存储函数返回的错误代码。 */
    uint8_t                adv_flags;    /**< 广播标志，用于指示广播模式和设备可见性。 */
    ble_advertising_init_t init;         /**< 广播初始化结构体，包含广播配置和数据。 */

    memset(&init, 0, sizeof(init));      /**< 将广播初始化结构体清零。 */

    adv_flags                            = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE; /**< 设置广播标志，仅限低功耗BLE模式。 */
    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;        /**< 在广播数据中包含设备的完整名称。 */
    init.advdata.include_appearance      = true;                         /**< 在广播数据中包含设备外观。 */
    init.advdata.flags                   = adv_flags;                    /**< 设置广播标志。 */
    init.advdata.uuids_complete.uuid_cnt = sizeof(module_advertising_uuids) / sizeof(module_advertising_uuids[0]); /**< 广播中包含的UUID数量。 */
    init.advdata.uuids_complete.p_uuids  = module_advertising_uuids;                  /**< 广播中包含的UUID列表。 */

#if SWIFT_PAIR_SUPPORTED == 1
    init.advdata.p_manuf_specific_data = &module_swift_pair_manufacturer_advertising_data;            /**< 如果支持Swift Pair，将厂家自定义数据添加到广播中。 */
    memcpy(&module_swift_pair_advertising_data, &init.advdata, sizeof(module_swift_pair_advertising_data));          /**< 复制广播数据结构体。 */
#endif

    // 配置广播模式。
    init.config.ble_adv_whitelist_enabled          = true;               /**< 启用白名单广播模式。只有在白名单中的设备（即预先配对或允许的设备）才能扫描到该广播 */
    init.config.ble_adv_directed_high_duty_enabled = true;               /**< 启用高占空比定向广播，用于快速重连，此模式的广播频率很高，能够快速被上一个连接设备识别并建立连接 */
    init.config.ble_adv_directed_enabled           = false;              /**< 禁用普通定向广播，普通定向广播是一种较低频率的广播模式，广播目标是上一个连接的设备。 */
    init.config.ble_adv_directed_interval          = 0;                  /**< 定向广播的间隔（若启用）。 */
    init.config.ble_adv_directed_timeout           = 0;                  /**< 定向广播的超时时间（若启用）。 */
    init.config.ble_adv_fast_enabled               = true;               /**< 启用快速广播模式，快速广播模式的广播间隔较短，使得设备更容易被扫描到和连接。通常在设备刚启动或需要快速建立连接时使用。 */
    init.config.ble_adv_fast_interval              = APP_ADV_FAST_INTERVAL; /**< 设置快速广播的间隔。 */
    init.config.ble_adv_fast_timeout               = APP_ADV_FAST_DURATION; /**< 设置快速广播的持续时间。 */
    init.config.ble_adv_slow_enabled               = true;               /**< 启用慢速广播模式，适用于在快速广播超时后仍希望设备继续被发现的情况。慢速广播能显著降低功耗。 */
    init.config.ble_adv_slow_interval              = APP_ADV_SLOW_INTERVAL; /**< 设置慢速广播的间隔。 */
    init.config.ble_adv_slow_timeout               = APP_ADV_SLOW_DURATION; /**< 设置慢速广播的持续时间。 */

    init.evt_handler   = ble_device_advertising_event_handler;                /**< 设置广播事件处理程序。 */
    init.error_handler = ble_device_advertising_error_handler; /**< 设置广播错误处理程序。 */

    err_code = ble_advertising_init(&module_advertising, &init); /**< 初始化广播模块并配置广播数据。 */
    APP_ERROR_CHECK(err_code);                              /**< 检查广播初始化是否成功，出错则触发错误处理。 */

    ble_advertising_conn_cfg_tag_set(&module_advertising, APP_BLE_CONN_CFG_TAG); /**< 设置连接配置标签，以匹配连接配置。 */
}

/**@brief 处理广播事件的函数。
 *
 * @details 该函数会在各种广播事件触发时被调用，执行相应的广播指示和操作，包括不同模式的广播以及空闲处理。
 *
 * @param[in] ble_adv_evt  广播事件的类型。
 */
void ble_device_advertising_event_handler(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code; /**< 用于存储函数返回的错误代码。 */

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_DIRECTED_HIGH_DUTY: /**< 高占空比定向广播事件，用于快速重连到上一个已连接的设备 */
            NRF_LOG_INFO("Directed advertising."); /**< 日志信息：定向广播。 */
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED); /**< 设置定向广播指示。 */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_FAST:		/**< 快速广播事件，用于设备启动时吸引周围设备快速连接 */
            NRF_LOG_INFO("Fast advertising."); /**< 日志信息：快速广播。 */
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING); /**< 设置快速广播指示。 */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_SLOW:	/**< 慢速广播事件，降低广播频率以节省功耗 */
            NRF_LOG_INFO("Slow advertising."); /**< 日志信息：慢速广播。 */
#if SWIFT_PAIR_SUPPORTED == 1
            module_swift_pair_advertising_data.p_manuf_specific_data = NULL; /**< 清除Swift Pair的厂家数据。 */
            err_code = ble_advertising_advdata_update(&module_advertising, &module_swift_pair_advertising_data, NULL); /**< 更新广播数据。 */
            APP_ERROR_CHECK(err_code);
#endif
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW); /**< 设置慢速广播指示。 */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_FAST_WHITELIST: /**< 快速白名单广播事件，仅广播给白名单中的设备 */
            NRF_LOG_INFO("Fast advertising with whitelist."); /**< 日志信息：快速白名单广播。 */
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST); /**< 设置快速白名单广播指示。 */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_SLOW_WHITELIST: /**< 慢速白名单广播事件，仅广播给白名单中的设备 */
            NRF_LOG_INFO("Slow advertising with whitelist."); /**< 日志信息：慢速白名单广播。 */
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST); /**< 设置慢速白名单广播指示。 */
            APP_ERROR_CHECK(err_code);
            err_code = ble_advertising_restart_without_whitelist(&module_advertising); /**< 重启不带白名单的广播。 */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:	 /**< 广播停止事件 */
            err_code = bsp_indication_set(BSP_INDICATE_IDLE); /**< 设置设备空闲指示。 */
            APP_ERROR_CHECK(err_code);
            ble_device_enter_sleep_mode(); /**< 进入睡眠模式以节省电量。 */
            break;

        case BLE_ADV_EVT_WHITELIST_REQUEST: /**< 事件：广播请求白名单地址和 IRK（Identity Resolving Key）,使用过滤条件设置身份列表，排除没有 IRK 的设备。将更新的白名单列表应用于广播模块 */
				{
						// 当 BLE 设备启用隐私模式时，它会周期性地更换自己的设备地址（称为随机私有地址），使得其他设备难以跟踪到它的真实身份
						// IRK 的作用——它允许已配对的设备通过解析私有地址来识别对方的身份，从而重新建立连接
						ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT]; /**< 定义白名单地址数组，存储允许连接的设备地址。 */
						ble_gap_irk_t  whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];  /**< 定义白名单 IRK 数组，用于存储身份解析密钥。 */
						uint32_t       addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;       /**< 变量：记录白名单中的地址数量，初始化为最大数量。 */
						uint32_t       irk_cnt  = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;       /**< 变量：记录白名单中的 IRK 数量，初始化为最大数量。 */
				
						err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt, whitelist_irks, &irk_cnt); /**< 获取白名单中的地址和 IRK，存储到数组中。 */
						APP_ERROR_CHECK(err_code); /**< 检查获取白名单数据是否成功，若出错则触发错误处理。 */
				
						NRF_LOG_DEBUG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist", addr_cnt, irk_cnt); /**< 打印调试信息，输出白名单中地址和 IRK 的数量。 */
				
						ble_device_set_filtered_device_identity_list(PM_PEER_ID_LIST_SKIP_NO_IRK); /**< 设置身份列表，过滤掉无 IRK 的设备。 */
				
						err_code = ble_advertising_whitelist_reply(&module_advertising, whitelist_addrs, addr_cnt, whitelist_irks, irk_cnt); /**< 将获取到的白名单数据应用于广播模块。 */
						APP_ERROR_CHECK(err_code); /**< 检查白名单应用是否成功，若出错则触发错误处理。 */
				}
				break;


        case BLE_ADV_EVT_PEER_ADDR_REQUEST: /**< 事件：在广播请求绑定设备地址时，加载并设置已绑定设备的定向地址，使设备能够执行定向广播 */
				{
						pm_peer_data_bonding_t peer_bonding_data; /**< 定义绑定数据结构体，用于存储已绑定设备的配对信息。 */
				
						if (module_peer_device_id != PM_PEER_ID_INVALID) /**< 检查当前绑定设备 ID 是否有效。 */
						{
								err_code = pm_peer_data_bonding_load(module_peer_device_id, &peer_bonding_data); /**< 从 Peer Manager 加载已绑定设备的配对数据。 */
								if (err_code != NRF_ERROR_NOT_FOUND) /**< 检查是否成功加载已绑定设备数据，确保数据存在。 */
								{
										APP_ERROR_CHECK(err_code); /**< 若加载数据出错则触发错误处理。 */
				
										ble_device_set_filtered_device_identity_list(PM_PEER_ID_LIST_SKIP_ALL); /**< 设置过滤条件，排除所有无中央地址解析的设备。 */
				
										ble_gap_addr_t * p_peer_addr = &(peer_bonding_data.peer_ble_id.id_addr_info); /**< 获取已绑定设备的地址信息。 */
										err_code = ble_advertising_peer_addr_reply(&module_advertising, p_peer_addr); /**< 将绑定设备地址传递给广播模块以执行定向广播。 */
										APP_ERROR_CHECK(err_code); /**< 检查广播模块地址设置是否成功，若出错则触发错误处理。 */
								}
						}
						break;
				}


        default:
            break;
    }
}

/**@brief 处理广播模块错误的函数。
 *
 * @details 该函数在广播模块发生错误时被调用，处理错误并传递错误代码给通用错误处理程序。
 *
 * @param[in] nrf_error  包含错误信息的错误代码。
 */
void ble_device_advertising_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error); /**< 调用通用错误处理程序并传入错误代码。 */
}

/**@brief 启动广播的函数。
 *
 * @details 该函数根据是否需要清除绑定信息来启动广播。
 *          如果 `erase_bonds` 为 true，则删除所有绑定信息，广播将在绑定删除成功的事件（PM_EVT_PEERS_DELETE_SUCCEEDED）中启动。
 *          否则，直接设置白名单并开始快速广播。
 *
 * @param[in] erase_bonds 如果为 true，则先删除绑定信息再启动广播。
 */
void ble_device_advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        ble_device_delete_bonds(); /**< 删除所有绑定信息。 */
        // 广播将在 PM_EVT_PEERS_DELETE_SUCCEEDED 事件触发时启动。
    }
    else
    {
        ble_device_set_filtered_white_list(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR); /**< 设置白名单以允许指定的设备连接。 */

        ret_code_t ret = ble_advertising_start(&module_advertising, BLE_ADV_MODE_FAST); /**< 启动快速广播模式。 */
        APP_ERROR_CHECK(ret); /**< 检查广播启动是否成功，出错则触发错误处理。 */
    }
}

 /***************************************
 *         		BLE 白名单函数       			*	
 **************************************/ 
/**@brief 设置过滤后的设备身份列表的函数。
 *
 * @details 该函数根据指定的过滤条件，获取满足条件的设备身份ID列表，并将其设置为当前的设备身份列表。
 *          这使得设备可以使用特定的白名单策略，允许或排除特定设备的连接。
 *
 * @param[in] skip  过滤器参数，用于过滤Peer Manager中的设备ID列表。
 */
void ble_device_set_filtered_device_identity_list(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT]; /**< 设备身份ID数组。 */
    uint32_t     peer_id_count = BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT; /**< 设备身份ID数组的大小。 */

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip); /**< 获取符合条件的设备ID列表。 */
    APP_ERROR_CHECK(err_code); /**< 检查设备ID列表获取是否成功，出错则触发错误处理。 */

    err_code = pm_device_identities_list_set(peer_ids, peer_id_count); /**< 设置当前的设备身份列表，用于连接过滤。 */
    APP_ERROR_CHECK(err_code); /**< 检查设备身份列表设置是否成功，出错则触发错误处理。 */
}

/**@brief 设置过滤后的白名单的函数。
 *
 * @details 该函数根据指定的过滤条件获取符合条件的设备ID列表，并将其设置为白名单。
 *          白名单中的设备可以在BLE连接中自动配对和重连，以提高连接的安全性和效率。
 *
 * @param[in] skip  过滤器参数，用于过滤Peer Manager中的设备ID列表。
 */
void ble_device_set_filtered_white_list(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_WHITELIST_ADDR_MAX_COUNT]; /**< 白名单设备ID数组。 */
    uint32_t     peer_id_count = BLE_GAP_WHITELIST_ADDR_MAX_COUNT; /**< 白名单设备ID数组的大小。 */

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip); /**< 获取符合条件的设备ID列表。 */
    APP_ERROR_CHECK(err_code); /**< 检查设备ID列表获取是否成功，出错则触发错误处理。 */

    NRF_LOG_INFO("\tm_whitelist_peer_cnt %d, MAX_PEERS_WLIST %d", /**< 输出白名单设备数量信息，用于调试。 */
                 peer_id_count + 1,
                 BLE_GAP_WHITELIST_ADDR_MAX_COUNT);

    err_code = pm_whitelist_set(peer_ids, peer_id_count); /**< 设置白名单，以允许符合条件的设备连接。 */
    APP_ERROR_CHECK(err_code); /**< 检查白名单设置是否成功，出错则触发错误处理。 */
}

 /***************************************
 *         		BLE 配对模块函数        *	
 **************************************/ 
/**@brief 初始化Peer Manager模块的函数。
 *
 * @details 该函数初始化Peer Manager模块，并配置设备的安全参数（如配对和加密要求），
 *          用于管理与中心设备的配对、绑定和安全连接。
 */
void ble_device_peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;    /**< 安全参数结构体，包含配对和加密要求。 */
    ret_code_t           err_code;     /**< 用于存储函数返回的错误代码。 */

    err_code = pm_init();              /**< 初始化Peer Manager模块。 */
    APP_ERROR_CHECK(err_code);         /**< 检查Peer Manager初始化是否成功，出错则触发错误处理。 */

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t)); /**< 将安全参数结构体清零。 */

    // 配置用于所有安全过程的安全参数。
    sec_param.bond           = SEC_PARAM_BOND;          /**< 配置是否绑定，首次配对后保存配对信息，这样下次连接时可以直接使用之前的加密密钥，无需重新配对。 */
    sec_param.mitm           = SEC_PARAM_MITM;          /**< 是否启用中间人攻击保护，启用 MITM 保护可以防止中间人攻击，通过验证配对双方身份，确保数据传输的安全性 */
    sec_param.lesc           = SEC_PARAM_LESC;          /**< 是否启用LE安全连接（LE Secure Connections），LE Secure Connections 是一种更高级的加密方式，使用椭圆曲线加密（ECC）来增强配对的安全性 */
    sec_param.keypress       = SEC_PARAM_KEYPRESS;      /**< 是否启用按键通知，按键通知允许设备在配对过程中传递按键输入的状态信息，通常用于配对时需要用户确认的场景，如输入 PIN 码 */
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES; /**< 设置设备的输入输出能力，用输入输出能力用于确定配对的安全级别，决定了配对过程中是否需要用户交互（如输入 PIN 码或按键确认） */
    sec_param.oob            = SEC_PARAM_OOB;           /**< 是否使用OOB（Out-of-Band）数据进行配对，OOB 配对是在 BLE 之外的通道（如 NFC）中交换加密信息，避免数据被监听，提高配对安全性 */
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;  /**< 配置加密密钥的最小长度 ，限制配对过程中生成的加密密钥的长度，以确保加密的强度。密钥越长，安全性越高*/
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;  /**< 配置加密密钥的最大长度。 */
    sec_param.kdist_own.enc  = 1;                       /**< 配置设备自己分发加密密钥，分发加密密钥，允许设备将自己的加密密钥发送给中心设备，以便建立加密连接 */
    sec_param.kdist_own.id   = 1;                       /**< 配置设备自己分发身份信息，分发身份信息，允许设备将身份信息发送给中心设备，以便设备在连接时进行身份验证 */
    sec_param.kdist_peer.enc = 1;                       /**< 配置中心设备分发加密密钥，要求中心设备分发加密密钥，确保设备能够解密传输的数据 */
    sec_param.kdist_peer.id  = 1;                       /**< 配置中心设备分发身份信息，求中心设备分发身份信息，以便设备可以识别和验证中心设备的身份 */

    err_code = pm_sec_params_set(&sec_param);           /**< 设置Peer Manager的安全参数。 */
    APP_ERROR_CHECK(err_code);                          /**< 检查安全参数设置是否成功，出错则触发错误处理。 */

    err_code = pm_register(ble_device_peer_manager_event_handler);             /**< 注册Peer Manager事件处理函数。 */
    APP_ERROR_CHECK(err_code);                          /**< 检查事件处理函数注册是否成功，出错则触发错误处理。 */
}

/**@brief 处理Peer Manager事件的函数。
 *
 * @details 该函数根据Peer Manager事件类型执行相应操作，包括处理连接安全成功、删除所有绑定数据以及更新绑定数据等事件。
 *
 * @param[in] p_evt  Peer Manager事件结构体指针，包含事件类型和相关数据。
 */
void ble_device_peer_manager_event_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);                   /**< 处理通用Peer Manager事件。 */
    pm_handler_disconnect_on_sec_failure(p_evt);   /**< 如果安全连接失败则断开连接。 */
    pm_handler_flash_clean(p_evt);                 /**< 在需要时清理Flash存储空间。 */

    switch (p_evt->evt_id)
    {
        case PM_EVT_CONN_SEC_SUCCEEDED:
            module_peer_device_id = p_evt->peer_id; /**< 连接安全认证成功后，存储当前设备的peer ID。 */
            break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            ble_device_advertising_start(false); /**< 删除所有绑定数据成功后，重新启动广播。 */
            break;

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            if (p_evt->params.peer_data_update_succeeded.flash_changed &&
                (p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING))
            {
                NRF_LOG_INFO("New Bond, add the peer to the whitelist if possible"); /**< 新建绑定后，将设备添加到白名单（如有需要）。 */
                
                 ble_device_set_filtered_white_list(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR); /**< 设置白名单以允许绑定的设备连接。 */
            }
            break;

        default:
            break;
    }
}

/**@brief 从持久化存储中清除绑定信息的函数。
 *
 * @details 该函数删除设备与所有已配对中心设备的绑定信息，从而清除配对记录。
 *          删除完成后，Peer Manager将触发 PM_EVT_PEERS_DELETE_SUCCEEDED 事件，用于通知广播模块重新开始广播。
 */
void ble_device_delete_bonds(void)
{
    ret_code_t err_code; /**< 用于存储函数返回的错误代码。 */

    NRF_LOG_INFO("Erase bonds!"); /**< 日志信息：开始删除绑定信息。 */

    err_code = pm_peers_delete(); /**< 调用Peer Manager接口删除所有绑定的中心设备信息。 */
    APP_ERROR_CHECK(err_code); /**< 检查删除操作是否成功，出错则触发错误处理。 */
}

 /***************************************
 *         		BLE 服务模块函数        *	
 **************************************/ 
/**@brief 初始化Queued Write模块的函数。
 *
 * @details 该函数配置并初始化Queued Write模块，使其能够处理长写操作。
 *          Queued Write模块为服务提供缓冲区管理，支持在BLE连接中安全传输较长数据。
 */
void ble_device_queue_write_init(void)
{
    ret_code_t         err_code;          /**< 用于存储函数返回的错误代码。 */
    nrf_ble_qwr_init_t qwr_init_obj = {0}; /**< Queued Write模块初始化结构体，初始化为零。 */

    qwr_init_obj.error_handler = ble_device_queue_write_error_handler; /**< 设置错误处理函数。 */

    err_code = nrf_ble_qwr_init(&module_queue_write, &qwr_init_obj); /**< 初始化Queued Write模块。 */
    APP_ERROR_CHECK(err_code);                          /**< 检查Queued Write模块初始化是否成功，出错则触发错误处理。 */
}

/**@brief 处理Queued Write模块错误的函数。
 *
 * @details 该函数的指针将传递给需要报告错误的服务，使得服务在发生错误时可以通知应用程序。
 *
 * @param[in]   nrf_error   包含错误信息的错误代码。
 */
void ble_device_queue_write_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error); /**< 调用错误处理程序并传入错误代码。 */
}



/**@brief 初始化设备信息服务（Device Information Service）的函数。
 *
 * @details 该函数配置并初始化设备信息服务，用于提供设备的基本信息（如制造商、产品ID等）。
 *          设备信息服务使中心设备能够读取关于设备的基本描述信息。
 */
void ble_device_info_system_init(void)
{
    ret_code_t       err_code;            /**< 用于存储函数返回的错误代码。 */
    ble_dis_init_t   dis_init_obj;        /**< 设备信息服务初始化结构体，用于配置设备信息服务。 */
    ble_dis_pnp_id_t pnp_id;              /**< PnP ID结构体，用于描述设备的厂商ID、产品ID和版本信息。 */

    pnp_id.vendor_id_source = PNP_ID_VENDOR_ID_SOURCE; /**< 设置厂商ID来源（通常为蓝牙SIG指定）。 */
    pnp_id.vendor_id        = PNP_ID_VENDOR_ID;        /**< 设置厂商ID。 */
    pnp_id.product_id       = PNP_ID_PRODUCT_ID;       /**< 设置产品ID。 */
    pnp_id.product_version  = PNP_ID_PRODUCT_VERSION;  /**< 设置产品版本。 */

    memset(&dis_init_obj, 0, sizeof(dis_init_obj));    /**< 清零设备信息服务初始化结构体。 */

    ble_srv_ascii_to_utf8(&dis_init_obj.manufact_name_str, MANUFACTURER_NAME); /**< 设置制造商名称。 */
    dis_init_obj.p_pnp_id = &pnp_id; /**< 将PnP ID信息添加到设备信息服务中。 */

    dis_init_obj.dis_char_rd_sec = SEC_JUST_WORKS; /**< 设置设备信息特征读取的安全级别。 */

    err_code = ble_dis_init(&dis_init_obj); /**< 初始化设备信息服务模块。 */
    APP_ERROR_CHECK(err_code);              /**< 检查设备信息服务初始化是否成功，出错则触发错误处理。 */
}


 /***************************************
 *         		BLE HID模块函数        *	
 **************************************/ 
/**@brief 初始化HID（人机接口设备）服务的函数。
 *
 * @details 该函数初始化HID服务，定义设备的HID报告映射，包括鼠标按钮、移动、滚轮、媒体控制等功能。
 *          HID服务可用于BLE鼠标或键盘等设备，提供标准的HID协议支持。
 */
void ble_device_hid_service_init(void)
{
    ret_code_t                err_code;         /**< 用于存储函数返回的错误代码。 */
    ble_hids_init_t           hids_init_obj;    /**< HID服务初始化结构体，用于配置HID服务。 */
    ble_hids_inp_rep_init_t * p_input_report;   /**< 输入报告指针，用于设置各输入报告属性。 */
    uint8_t                   hid_info_flags;   /**< HID信息标志位，定义设备的唤醒和连接属性。 */

    static ble_hids_inp_rep_init_t inp_rep_array[INPUT_REPORT_COUNT]; /**< 输入报告数组。 */
    static uint8_t rep_map_data[] =  /**< HID报告映射，定义鼠标按键、移动、滚轮和媒体控制。 */
    {
        0x05, 0x01, // Usage Page (Generic Desktop)
        0x09, 0x02, // Usage (Mouse)

        // Report ID 1：使用
				0xA1, 0x01, // Collection (Application)
        0x85, 0x01,       // Report Id 1
        0x09, 0x01,       // Usage (Pointer)
        0xA1, 0x00,       // Collection (Physical)
				0x05, 0x09,       		// Usage Page (Buttons)
        0x95, 0x08,       					// Report Count (8)
        0x75, 0x01,       					// Report Size (1)
        0x19, 0x01,       					// Usage Minimum (01)
        0x29, 0x08,       					// Usage Maximum (08)
        0x15, 0x00,       					// Logical Minimum (0)
        0x25, 0x01,       					// Logical Maximum (1)
        0x81, 0x02,       					// Input (Data, Variable, Absolute)
				0x05, 0x01,       		// Usage Page (Generic Desktop)
				0x75, 0x10,       					// Report Size (16)
        0x95, 0x02,       					// Report Count (2)
        0x09, 0x30,       					// Usage (X)
        0x09, 0x31,       					// Usage (Y)
        0x16, 0x00, 0x80, 					// LogicalMinimum(-32,768)									
				0x26, 0xFF, 0x7F, 					// LogicalMaximum(32,767)									
        0x81, 0x06,       					// Input (Data, Variable, Relative)
				0x05, 0x01,       		// Usage Page (Generic Desktop)
        0x09, 0x38,       					// Usage (Wheel)
        0x75, 0x08,       					// Report Size (8)
        0x95, 0x01,       					// Report Count (1)
        0x09, 0x38,       					// Usage (Wheel)
        0x15, 0x81,       					// Logical Minimum (-127)
        0x25, 0x7F,       					// Logical Maximum (127)
        0x81, 0x06,       					// Input (Data, Variable, Relative)
        0xC0,             // End Collection (Physical)
				0xC0,             // End Collection (Application)

        // Report ID 2: 保留
				0xA1, 0x01, // Collection (Application)
        0x85, 0x02,       // Report Id 2
        0x09, 0x01,       // Usage (Pointer)
        0xA1, 0x00,       // Collection (Physical)
        0x75, 0x10,       // Report Size (16)
        0x95, 0x02,       // Report Count (2)
        0x05, 0x01,       // Usage Page (Generic Desktop)
        0x09, 0x30,       // Usage (X)
        0x09, 0x31,       // Usage (Y)
        0x16, 0x00, 0x80,    				/*   LogicalMinimum(-32,768)										*/\
				0x26, 0xFF, 0x7F,    				/*   LogicalMaximum(32,767)										*/\
        0x81, 0x06,       // Input (Data, Variable, Relative)
        0xC0,             // End Collection (Physical)
				0xC0,             // End Collection (Application)

        // Report ID 3: 保留
        0x05, 0x0C,       // Usage Page (Consumer)
        0x09, 0x01,       // Usage (Consumer Control)
        0xA1, 0x01,       // Collection (Application)
        0x85, 0x03,       // Report Id (3)
        0x15, 0x00,       // Logical minimum (0)
        0x25, 0x01,       // Logical maximum (1)
        0x75, 0x01,       // Report Size (1)
        0x95, 0x01,       // Report Count (1)

        0x09, 0xCD,       // Usage (Play/Pause)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x0A, 0x83, 0x01, // Usage (AL Consumer Control Configuration)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x09, 0xB5,       // Usage (Scan Next Track)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x09, 0xB6,       // Usage (Scan Previous Track)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)

        0x09, 0xEA,       // Usage (Volume Down)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x09, 0xE9,       // Usage (Volume Up)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x0A, 0x25, 0x02, // Usage (AC Forward)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x0A, 0x24, 0x02, // Usage (AC Back)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0xC0              // End Collection
    };

    memset(inp_rep_array, 0, sizeof(inp_rep_array)); /**< 清零输入报告数组。 */

    // 配置鼠标按键输入报告
    p_input_report                      = &inp_rep_array[INPUT_REP_BUTTONS_INDEX];
    p_input_report->max_len             = INPUT_REP_BUTTONS_LEN;
    p_input_report->rep_ref.report_id   = INPUT_REP_REF_BUTTONS_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;
    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr      = SEC_JUST_WORKS;
    p_input_report->sec.rd      = SEC_JUST_WORKS;

    // 配置鼠标移动输入报告
    p_input_report                      = &inp_rep_array[INPUT_REP_MOVEMENT_INDEX];
    p_input_report->max_len             = INPUT_REP_MOVEMENT_LEN;
    p_input_report->rep_ref.report_id   = INPUT_REP_REF_MOVEMENT_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;
    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr      = SEC_JUST_WORKS;
    p_input_report->sec.rd      = SEC_JUST_WORKS;

    // 配置媒体控制输入报告
    p_input_report                      = &inp_rep_array[INPUT_REP_MPLAYER_INDEX];
    p_input_report->max_len             = INPUT_REP_MEDIA_PLAYER_LEN;
    p_input_report->rep_ref.report_id   = INPUT_REP_REF_MPLAYER_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;
    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr      = SEC_JUST_WORKS;
    p_input_report->sec.rd      = SEC_JUST_WORKS;

    hid_info_flags = HID_INFO_FLAG_REMOTE_WAKE_MSK | HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK; /**< 设置HID信息标志。 */

    memset(&hids_init_obj, 0, sizeof(hids_init_obj)); /**< 清零HID服务初始化对象。 */

    // 设置HID服务的各项参数
    hids_init_obj.evt_handler                    = ble_device_hid_service_event_handler;
    hids_init_obj.error_handler                  = ble_device_hid_service_error_handler;
    hids_init_obj.is_kb                          = false;
    hids_init_obj.is_mouse                       = true;
    hids_init_obj.inp_rep_count                  = INPUT_REPORT_COUNT;
    hids_init_obj.p_inp_rep_array                = inp_rep_array;
    hids_init_obj.outp_rep_count                 = 0;
    hids_init_obj.p_outp_rep_array               = NULL;
    hids_init_obj.feature_rep_count              = 0;
    hids_init_obj.p_feature_rep_array            = NULL;
    hids_init_obj.rep_map.data_len               = sizeof(rep_map_data);
    hids_init_obj.rep_map.p_data                 = rep_map_data;
    hids_init_obj.hid_information.bcd_hid        = BASE_USB_HID_SPEC_VERSION;
    hids_init_obj.hid_information.b_country_code = 0;
    hids_init_obj.hid_information.flags          = hid_info_flags;
    hids_init_obj.included_services_count        = 0;
    hids_init_obj.p_included_services_array      = NULL;

    // 配置安全等级
    hids_init_obj.rep_map.rd_sec         = SEC_JUST_WORKS;
    hids_init_obj.hid_information.rd_sec = SEC_JUST_WORKS;
    hids_init_obj.boot_mouse_inp_rep_sec.cccd_wr = SEC_JUST_WORKS;
    hids_init_obj.boot_mouse_inp_rep_sec.wr      = SEC_JUST_WORKS;
    hids_init_obj.boot_mouse_inp_rep_sec.rd      = SEC_JUST_WORKS;
    hids_init_obj.protocol_mode_rd_sec = SEC_JUST_WORKS;
    hids_init_obj.protocol_mode_wr_sec = SEC_JUST_WORKS;
    hids_init_obj.ctrl_point_wr_sec    = SEC_JUST_WORKS;

    err_code = ble_hids_init(&module_hid_service, &hids_init_obj); /**< 初始化HID服务。 */
    APP_ERROR_CHECK(err_code); /**< 检查HID服务初始化是否成功，出错则触发错误处理。 */
}

/**@brief 处理HID事件的函数。
 *
 * @details 该函数会在所有HID事件触发时被调用，根据事件类型执行相应的操作，
 *          包括进入Boot模式、Report模式及启用通知等。
 *
 * @param[in]   p_hids  HID服务结构体指针。
 * @param[in]   p_evt   从HID服务接收到的事件。
 */
void ble_device_hid_service_event_handler(ble_hids_t * p_hids, ble_hids_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_HIDS_EVT_BOOT_MODE_ENTERED:
            mark_in_boot_mode = true; /**< 标记设备进入Boot模式，Boot模式适用于基本数据传输。 */
            break;

        case BLE_HIDS_EVT_REPORT_MODE_ENTERED:
            mark_in_boot_mode = false; /**< 标记设备进入Report模式，Report模式支持更多的自定义数据传输。 */
            break;

        case BLE_HIDS_EVT_NOTIF_ENABLED:
            // 启用通知的事件。可以在此添加处理逻辑（如准备发送数据）。
            break;

        default:
            // 不需要其他事件的处理。
            break;
    }
}

/**@brief 处理服务错误的函数。
 *
 * @details 该函数的指针将传递给需要报告错误的服务，使得服务在发生错误时可以通知应用程序。
 *
 * @param[in]   nrf_error   包含错误信息的错误代码。
 */
void ble_device_hid_service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error); /**< 调用通用错误处理程序并传入错误代码。 */
}

 /***************************************
 *         BLE 事件调度器模块函数        *	
 **************************************/ 
/**@brief 初始化事件调度器的函数。
 *
 * @details 该函数初始化事件调度器，用于管理和调度系统中的异步事件。
 *          调度器允许事件在主循环中顺序处理，避免在中断中直接执行较长的代码。
 */
void ble_device_scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE); /**< 初始化调度器，配置事件数据的最大大小和队列大小。 */
}

/****************************************
 *             FreeRTOS函数  			   		*
 ****************************************/
