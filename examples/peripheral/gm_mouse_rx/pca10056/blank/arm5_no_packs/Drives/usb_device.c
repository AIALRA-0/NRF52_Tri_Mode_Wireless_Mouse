/***************************************
 *              库文件             		 *
 ***************************************/
#include "usb_device.h"

/***************************************
 *         USBD静态通信常量数组         *
 ***************************************/
/**
 * @brief 设备描述符数据
 * @details 存储设备的基本信息描述符，用于主机识别设备类型
 */
static const uint8_t get_descriptor_device[] = {
    USBD_DEVICE_DESCRIPTOR
};

/**
 * @brief 配置描述符数据
 * @details 包含配置、接口和端点的描述符，用于描述设备的不同接口和配置选项
 */
static const uint8_t get_descriptor_configuration[] = {
    USBD_CONFIG_DESCRIPTOR,        /**< 配置描述符 */
    USBD_INTERFACE0_DESCRIPTOR,    /**< 接口描述符 */
    USBD_HID0_DESCRIPTOR,          /**< HID描述符 */
    USBD_ENDPOINT1_DESCRIPTOR      /**< 端点描述符 */
};

/**
 * @brief 语言字符串描述符数据
 * @details 包含支持的语言描述符，提供设备支持的语言列表
 */
static const uint8_t get_descriptor_string_language[] = {
    USBD_STRING_LANG
};

/**
 * @brief 厂商字符串描述符数据
 * @details 提供设备厂商名称的字符串描述符
 */
static const uint8_t get_descriptor_string_manufactuer[] = {
    USBD_STRING_MANUFACTURER
};

/**
 * @brief 产品字符串描述符数据
 * @details 提供设备产品名称的字符串描述符
 */
static const uint8_t get_descriptor_string_product[] = {
    USBD_STRING_PRODUCT
};

/**
 * @brief 报告描述符数据
 * @details 包含HID设备的报告描述符，用于描述设备报告的格式和结构
 */
static const uint8_t get_descriptor_report_interface_0[] = {
    USBD_MOUSE_REPORT_DESCRIPTOR
};

/**
 * @brief 配置状态响应：已配置
 * @details 表示设备已配置
 */
static const uint8_t get_config_response_configured[] = {1};

/**
 * @brief 配置状态响应：未配置
 * @details 表示设备未配置
 */
static const uint8_t get_config_response_unconfigured[] = {0};

/**
 * @brief 设备状态响应：自供电且无远程唤醒
 * @details 设备处于自供电状态且不支持远程唤醒
 * @note LSB优先：自供电，无远程唤醒
 */
static const uint8_t get_status_device_response_no_remote_wakeup[] = {
    ((DEVICE_SELF_POWERED) ? 1 : 0), /**< LSB优先：自供电，无远程唤醒 */
    0
};

/**
 * @brief 设备状态响应：自供电且支持远程唤醒
 * @details 设备处于自供电状态并且支持远程唤醒
 * @note LSB优先：自供电，支持远程唤醒
 */
static const uint8_t get_status_device_response_remote_wakeup[] = {
    ((DEVICE_SELF_POWERED) ? 1 : 0) | 2, /**< LSB优先：自供电，支持远程唤醒 */
    0
};

/**
 * @brief 接口状态响应
 * @details 接口的状态响应，表示接口正常状态
 */
static const uint8_t get_status_interface_response[] = {0, 0};

/**
 * @brief 端点状态响应：已停止
 * @details 端点的状态已停止（halted）
 */
static const uint8_t get_status_endpoint_halted_response[] = {1, 0};

/**
 * @brief 端点状态响应：已激活
 * @details 端点的状态为已激活（active）
 */
static const uint8_t get_status_endpoint_active_response[] = {0, 0};

/***************************************
 *          USBD描述符大小            	 *
 ***************************************/
#define GET_CONFIG_DESCRIPTOR_SIZE    sizeof(get_descriptor_configuration) // 获取配置描述符大小
#define GET_INTERFACE_DESCRIPTOR_SIZE 9  // 接口描述符大小
#define GET_HID_DESCRIPTOR_SIZE       9  // HID描述符大小
#define GET_ENDPOINT_DESC_SIZE  7  // 端点描述符大小

/***************************************
 *              USBD地址           	 *	
 **************************************/
/**
 * @brief 获取接口描述符的起始地址
 * @details 返回配置描述符中接口描述符的起始地址
 */
#define get_descriptor_interface_0 &get_descriptor_configuration[9]

/**
 * @brief 获取HID描述符的起始地址
 * @details 返回配置描述符中 HID 描述符的起始地址
 */
#define get_descriptor_hid_0 &get_descriptor_configuration[9 + GET_INTERFACE_DESCRIPTOR_SIZE]

/**
 * @brief 获取端点描述符的起始地址
 * @details 返回配置描述符中端点描述符的起始地址
 */
#define get_descriptor_endpoint_1 &get_descriptor_configuration[9 + GET_INTERFACE_DESCRIPTOR_SIZE + GET_HID_DESCRIPTOR_SIZE]


/***************************************
 *              USB标志           	  *	
 **************************************/ 
/**
 * @brief USB配置标志
 *
 * 用于标记USB已配置且准备好传输数据的标志
 */
static volatile bool mark_usbd_configured = false;

/**
 * @brief USB挂起标志
 *
 * 用于标记USB处于挂起状态，当有新数据可用时需要唤醒
 *
 * @note 此变量在主循环中更改。
 */
static bool mark_usbd_suspended = false;

/**
 * @brief 远程唤醒功能启用标志
 *
 * 用于标记主机是否在此设备中启用了远程唤醒功能的内部标志
 */
static
#if REMOTE_WU
    volatile // 仅在远程唤醒启用时，禁止编译器优化
#endif
bool mark_usbd_remote_wakeup_enabled = true;

/**
 * @brief 设备数据发送待定标志
 *
 * 设定此标志意味着USB端点正在发送上一次的设备数据
 */
static volatile bool mark_send_device_data = false;

/**
 * @brief 请求的挂起状态
 *
 * 基于从USBD库接收到的事件，请求当前的挂起状态。
 * 如果此值与@ref mark_usbd_suspended 不同，表示设备的挂起状态需要更新，状态更改将在主循环中处理。
 */
static volatile bool mark_usbd_suspend_state_request = false;
		
/**
 * @brief 系统是否有动作
 *
 */
static volatile bool mark_device_has_motion = false;

/***************************************
 *           USB标志操作函数           	*	
 **************************************/ 
 /**
 * @brief 获取USB配置标志
 *
 * 用于检查USB是否已配置且准备好传输数据。
 *
 * @return bool 当前USB配置状态，`true`表示已配置，`false`表示未配置。
 */
bool get_mark_usbd_configured(void) 
{
    return mark_usbd_configured;
}

/**
 * @brief 设置USB配置标志
 *
 * 用于标记USB配置状态。
 *
 * @param value `true`表示USB已配置，`false`表示USB未配置。
 */
void set_mark_usbd_configured(bool value) 
{
    mark_usbd_configured = value;
}

/**
 * @brief 获取USB挂起标志
 *
 * 检查USB是否处于挂起状态。
 *
 * @return bool 当前USB挂起状态，`true`表示挂起，`false`表示未挂起。
 */
bool get_mark_usbd_suspended(void) {
    return mark_usbd_suspended;
}

/**
 * @brief 设置USB挂起标志
 *
 * 用于设置USB挂起状态。
 *
 * @param value `true`表示USB挂起，`false`表示未挂起。
 */
void set_mark_usbd_suspended(bool value) 
{
    mark_usbd_suspended = value;
}

/**
 * @brief 获取远程唤醒功能启用标志
 *
 * 检查主机是否启用了远程唤醒功能。
 *
 * @return bool 当前远程唤醒功能启用状态，`true`表示已启用，`false`表示未启用。
 */
bool get_mark_usbd_remote_wakeup_enabled(void) 
{
    return mark_usbd_remote_wakeup_enabled;
}

/**
 * @brief 设置远程唤醒功能启用标志
 *
 * 用于设置远程唤醒功能的启用状态。
 *
 * @param value `true`表示启用远程唤醒功能，`false`表示禁用远程唤醒功能。
 */
void set_mark_usbd_remote_wakeup_enabled(bool value) 
{
    mark_usbd_remote_wakeup_enabled = value;
}

/**
 * @brief 获取设备数据发送待定标志
 *
 * 检查设备数据是否正在发送。
 *
 * @return bool 当前设备数据发送状态，`true`表示正在发送，`false`表示未发送。
 */
bool get_mark_send_device_data(void) 
{
    return mark_send_device_data;
}

/**
 * @brief 设置设备数据发送待定标志
 *
 * 用于标记设备数据的发送状态。
 *
 * @param value `true`表示正在发送设备数据，`false`表示未发送。
 */
void set_mark_send_device_data(bool value) 
{
    mark_send_device_data = value;
}

/**
 * @brief 获取USB挂起状态请求标志
 *
 * 检查当前是否有挂起状态请求。
 *
 * @return bool 当前挂起状态请求，`true`表示有挂起请求，`false`表示无挂起请求。
 */
bool get_mark_usbd_suspend_state_request(void) 
	{
    return mark_usbd_suspend_state_request;
}

/**
 * @brief 设置USB挂起状态请求标志
 *
 * 用于设置USB挂起状态请求。
 *
 * @param value `true`表示有挂起请求，`false`表示无挂起请求。
 */
void set_mark_usbd_suspend_state_request(bool value) 
{
    mark_usbd_suspend_state_request = value;
}

/**
 * @brief 获取设备动作标志
 *
 */
bool get_mark_device_has_motion(void) 
	{
    return mark_device_has_motion;
}

/**
 * @brief 设置设备动作标志
 *
 */
void set_mark_device_has_motion(bool value) 
{
    mark_device_has_motion = value;
}

/****************************************
 *           USBD 初始化函数  			     	 *
 ****************************************/
/**
 * @brief 初始化系统组件
 * 
 * 包括电源、时钟、日志、按钮和LED等的初始化。
 */
void usb_device_init(void)
{
    ret_code_t ret;
	
    // 初始化电源和时钟
    usb_device_init_power_and_clock(); 
		
    // 初始化GPIO按钮和LED
		usb_device_gpio_init();
    usb_device_gpiote_init();
	
		// 点亮系统LED和按钮
    nrf_gpio_pin_clear(LED_SYSTEM_RUNNING);
    nrf_gpio_pin_clear(LED_SYSTEM_ON);

    // 日志记录启动信息
    NRF_LOG_INFO("USBD Started.");
		NRF_LOG_FLUSH();

    // 记录复位原因
    usb_device_log_reset_reason();
    nrf_power_resetreas_clear(nrf_power_resetreas_get());
		
		// USB工作从这里开始
    ret = nrf_drv_usbd_init(usb_device_event_handler);
    APP_ERROR_CHECK(ret);

    // 配置EP0的最大数据包大小
    nrf_drv_usbd_ep_max_packet_size_set(NRF_DRV_USBD_EPOUT0, EP0_MAXPACKETSIZE);
    nrf_drv_usbd_ep_max_packet_size_set(NRF_DRV_USBD_EPIN0, EP0_MAXPACKETSIZE);
		
		if (USBD_POWER_DETECTION)  // 判断是否启用USB电源检测
    {
				// 配置 USB 电源事件
        static const nrf_drv_power_usbevt_config_t config =
        {
            .handler = usb_device_power_event_handler	// USB 事件处理程序
        };
        nrf_drv_power_usbevt_init(&config); // 初始化 USB 电源事件，注册事件处理函数
    }
    else
    {
        NRF_LOG_INFO("No USB power detection enabled\r\nStarting USB now");
				NRF_LOG_FLUSH();
        nrf_delay_us(STARTUP_DELAY); // 延迟一段时间后再启动 USB
        
        if (!nrf_drv_usbd_is_enabled())
        {
            nrf_drv_usbd_enable(); // 启用 USB 驱动
            ret = configure_endpoint(0, NRF_DRV_USBD_EPIN1); // 配置端点 1
            APP_ERROR_CHECK(ret);
        }

        // 等待调节器上电
        while (NRF_DRV_POWER_USB_STATE_CONNECTED == nrf_drv_power_usbstatus_get())
        {
            // 等待 USB 状态为连接状态
        }
				
        if (NRF_DRV_POWER_USB_STATE_READY == nrf_drv_power_usbstatus_get())
        {
						// 点亮USB上电状态
						NRF_LOG_INFO("USB Power Ready Without Power Detection");
						NRF_LOG_FLUSH();
						nrf_gpio_pin_clear(LED_USB_POWER_CONNECTED);
						
            if (!nrf_drv_usbd_is_started())
            {
                nrf_drv_usbd_start(true);  // 如果 USB 没有启动，启动 USB 传输
            }
        }
        else
        {
            nrf_drv_usbd_disable(); // 如果 USB 状态不是 READY，禁用 USB
        }
    }
}

/***************************************
 *           USBD 辅助函数          	   *	
 **************************************/ 
/**
 * @brief 打印当前系统状态标志
 *
 * 该函数打印所有USB和系统相关标志的当前状态，显示其为true或false。
 */
void print_usb_device_system_status(void)
{
		NRF_LOG_INFO("/*****************************************/");
	
    NRF_LOG_INFO("System Status:");
    
    // 打印USB配置标志
    NRF_LOG_INFO("USB Configured: %s", mark_usbd_configured ? "true" : "false");
    
    // 打印USB挂起标志
    NRF_LOG_INFO("USB Suspended: %s", mark_usbd_suspended ? "true" : "false");
		
		// 打印USB挂起状态请求标志
    NRF_LOG_INFO("USB Suspend State Request: %s", mark_usbd_suspend_state_request ? "true" : "false");
    
    // 打印远程唤醒功能启用标志
    NRF_LOG_INFO("Remote Wakeup Enabled: %s", mark_usbd_remote_wakeup_enabled ? "true" : "false");
    
    // 打印设备数据发送待定标志
    NRF_LOG_INFO("Send Device Data: %s", mark_send_device_data ? "true" : "false");
		
		NRF_LOG_INFO("/*****************************************/");
		
		NRF_LOG_FLUSH();
}
 /****************************************
 *              USBD 配置函数  			     *
 ****************************************/
 /**
 * @brief 配置指定端点为选定配置
 *
 * 该函数为特定配置和特定端点设置所有端点。
 *
 * @note
 * 设置配置索引为0意味着技术上禁用了HID接口，否则调整为响应索引配置。
 * 当设备启动或检测到USB重置时，应设置此配置。
 *
 * @param index 配置索引
 * @param ep    要配置的端点号（IN或OUT端点）
 *
 * @retval NRF_ERROR_INVALID_PARAM 无效的配置
 * @retval NRF_SUCCESS             配置成功设置
 */
ret_code_t configure_endpoint(uint8_t index, nrf_drv_usbd_ep_t ep)
{
    if (index == 1)	// 配置1
    {
        nrf_drv_usbd_ep_dtoggle_clear(ep);  // 清除指定端点数据切换位
        nrf_drv_usbd_ep_stall_clear(ep);    // 清除指定端点停滞状态
        nrf_drv_usbd_ep_enable(ep);         // 启用指定端点
        mark_usbd_configured = true;           // 标记为已配置
        nrf_drv_usbd_setup_clear();         // 接受了请求，清除设置阶段，处理完成
    }
    else if (index == 0) // 配置0
    {
        nrf_drv_usbd_ep_disable(ep);        // 禁用指定端点
        mark_usbd_configured = false;          // 标记为未配置
        nrf_drv_usbd_setup_clear();         // 接受了请求，清除设置阶段，处理完成
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;     // 返回无效配置错误
    }
    return NRF_SUCCESS;                     // 返回配置成功
}

 /****************************************
 *           USBD 请求响应函数  			     *
 ****************************************/
/**
 * @brief 通过端点0响应主机发出的 Setup 请求，返回设备的信息以建立链接
 *
 *
 * @param[in] p_setup 指向当前设置请求数据的指针，
 *                    包含请求的详细信息，如最大传输大小。
 * @param[in] p_data  指向需要发送的实际数据的指针。
 * @param[in] size    要发送的数据字节数。
 *
 * @note 在接收到USBD_EVT_BUFREADY事件，表示数据缓冲区已准备好，USB 控制器可以开始实际传输数据之前，p_data指向的数据必须保持有效。
 */
void respond_setup_request(nrf_drv_usbd_setup_t const * const p_setup, void const * p_data, size_t size)
{
		// 检查需要发送的数据大小是否超过设置请求允许的最大长度 
    if (size > p_setup->wLength)
    {
        size = p_setup->wLength; // 如果超过，则限制为请求中指定的最大长度
    }

    ret_code_t ret;	// 错误报告
    
    // 构建用于传输的nrf_drv_usbd_transfer_t结构体
    nrf_drv_usbd_transfer_t transfer =
    {
        .p_data = {.tx = p_data}, // 设置传输数据指针，指向要发送的数据
        .size = size              // 设置传输的数据大小
    };
    
    // 通过USB端点0启动传输
    ret = nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN0, &transfer);
    
    // 如果传输启动失败，记录错误日志
    if (ret != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Transfer starting failed: %d", (uint32_t)ret); // 记录失败原因
    }

    // 确保传输启动成功，调试时用于捕获错误
    ASSERT(ret == NRF_SUCCESS); // 若传输失败，调试时会触发断言

    // 忽略未使用的变量警告，避免编译器提示
    UNUSED_VARIABLE(ret);
}


/**
 * @brief 响应主机发出的 GetStatus 请求，通过respond_setup_request返回信息
 *
 * 该函数根据不同的请求类型（设备、接口、端点）处理GetStatus请求。
 * 根据请求的类型和设备的当前状态，返回相应的状态数据，或进入停滞状态。
 *
 * @param[in] p_setup 指向主机发送的请求请求包的指针，包含请求的详细信息如请求类型和索引。
 */
void respond_get_status_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    switch (p_setup->bmRequestType) // 请求的类型, 设备、接口或端点
    {
    case 0x80: // 设备请求 
        if (((p_setup->wIndex) & 0xff) == 0) // 确保索引为0，请求针对整个设备，而不是某个具体的接口或端点
        {
            // 响应相应的设备状态
            respond_setup_request(
                p_setup,	// 请求包
                mark_usbd_remote_wakeup_enabled ? get_status_device_response_remote_wakeup : get_status_device_response_no_remote_wakeup,	// 设备状态响应静态数组
                sizeof(get_status_device_response_no_remote_wakeup));  // 传输的字节数
            return; // 处理完成，直接返回
        }
        break; // 未匹配索引或条件时，跳出处理流程

    case 0x81: // 接口请求 
        if (mark_usbd_configured) // 仅当USB设备已配置时响应接口请求
        {
            if (((p_setup->wIndex) & 0xff) == 0) // 请求查询某个特定接口的状态，仅支持接口0的请求
            {
								// 响应相应的接口状态
                respond_setup_request(
                    p_setup, // 请求包
                    get_status_interface_response, // 接口正常状态
                    sizeof(get_status_interface_response)); // 传输的字节数
                return; // 处理完成，直接返回
            }
        }
        break; // 未匹配索引或条件时，跳出处理流程

    case 0x82: // 端点请求 
        if (((p_setup->wIndex) & 0xff) == 0) // 请求查询某个特定端点的状态，仅支持端点0的请求
        {
            // 端点0通常是控制端点，返回其活动状态
            respond_setup_request(
                p_setup, // 请求包
                get_status_endpoint_active_response,	// 端点正常状态，已激活
                sizeof(get_status_endpoint_active_response)); // 传输的字节数
            return; // 处理完成，直接返回
        }
				
				// 仅当USB设备已配置时响应其他端点请求
        if (mark_usbd_configured) 
        {
            if (((p_setup->wIndex) & 0xff) == NRF_DRV_USBD_EPIN1) // 检查是否为端点1的请求
            {
                if (nrf_drv_usbd_ep_stall_check(NRF_DRV_USBD_EPIN1)) // 检查端点1是否处于停滞状态
                {
                    // 如果端点1处于停滞状态，返回停滞状态响应
                    respond_setup_request(
                        p_setup,	// 请求包
                        get_status_endpoint_halted_response, // 端点挂起状态，未激活
                        sizeof(get_status_endpoint_halted_response)); // 传输的字节数
                    return; // 处理完成，直接返回
                }
                else
                {
                    // 如果端点1未停滞，返回活动状态响应
                    respond_setup_request(
                        p_setup,
                        get_status_endpoint_active_response, // 端点正常状态，已激活
                        sizeof(get_status_endpoint_active_response)); // 传输的字节数
                    return; // 处理完成，直接返回
                }
            }
        }
        break; // 如果未配置或端点不匹配，跳出

    default:
        break; // 如果请求类型不匹配，进入默认处理流程
    }

    // 如果无法处理该请求，记录错误日志
    NRF_LOG_ERROR("Unknown status: 0x%2x", p_setup->bmRequestType); // 记录未知的状态请求
    nrf_drv_usbd_setup_stall(); // 将端点进入停滞状态，表示无法处理该请求
}


/**
 * @brief 响应主机发出的 ClearFeature请求，清除端点/设备的状态/功能
 *
 * 该函数根据设置请求清除USB设备的某些特性，如端点停滞状态或远程唤醒功能。
 * 当设备接收到ClearFeature请求时，该函数会根据请求的接收者（端点或设备）
 * 进行相应的处理，并清除特性状态。如果无法处理该请求，则会进入停滞状态。
 *
 * @param[in] p_setup 指向当前USB设置请求数据的指针，
 *                    包含请求的详细信息如请求类型、特性编号等。
 */
void respond_clear_feature_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    // 检查请求类型是否为标准请求，且接收者为端点 (bmRequestType = 0x02)
    if ((p_setup->bmRequestType) == 0x02) // 标准请求，接收者为端点
    {
        // 检查请求的类型编号 (wValue == 0) ，表示此请求请求改变端点停滞状态
        if ((p_setup->wValue) == 0)
        {
            // 检查请求的端点索引 (wIndex) 是否为端点1
            if ((p_setup->wIndex) == NRF_DRV_USBD_EPIN1)
            {
                // 清除端点1的停滞状态
                nrf_drv_usbd_ep_stall_clear(NRF_DRV_USBD_EPIN1); // 清除端点停滞状态
                nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成，表示操作成功
                return; // 操作完成，返回
            }
        }
    }
    // 检查请求类型是否为标准请求，且接收者为设备 (bmRequestType = 0x00)
    else if ((p_setup->bmRequestType) == 0x0) // 标准请求，接收者为设备
    {
        // 检查是否启用了远程唤醒功能 (REMOTE_WU)
        if (REMOTE_WU)
        {
            // 检查请求的特性编号 (wValue == 1)，表示此请求请求改变远程唤醒功能
            if ((p_setup->wValue) == 1) // 特性编号为远程唤醒
            {
                // 禁用设备的远程唤醒功能
                mark_usbd_remote_wakeup_enabled = false; // 禁用远程唤醒功能
                nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成，表示操作成功
                return; // 操作完成，返回
            }
        }
    }

    // 如果请求类型无法处理或无效，记录错误日志
    NRF_LOG_ERROR("Unknown feature to clear"); // 未知特性清除请求，记录错误日志
    nrf_drv_usbd_setup_stall(); // 进入停滞状态，表示无法处理该请求
}



/**
 * @brief 响应主机发出的 SetFeature请求，启用端点/设备的状态/功能
 *
 * 该函数处理设置特性请求，根据请求的接收者（端点或设备），
 * 启用某些特性（如端点停滞状态或远程唤醒功能）。
 *
 * @param[in] p_setup 指向当前USB设置请求数据的指针，
 *                    包含请求的详细信息如请求类型、特性编号等。
 */
void respond_set_feature_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    // 检查请求类型是否为标准请求，接收者为端点 (bmRequestType = 0x02)
    if ((p_setup->bmRequestType) == 0x02) // 标准请求，接收者为端点
    {
        if ((p_setup->wValue) == 0) //  检查请求的类型编号 (wValue == 0) ，表示此请求请求改变端点停滞状态
        {
            if ((p_setup->wIndex) == NRF_DRV_USBD_EPIN1) // 检查是否针对端点1
            {
                nrf_drv_usbd_ep_stall(NRF_DRV_USBD_EPIN1); // 使端点1进入停滞状态
                nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成
                return; // 操作成功，返回
            }
        }
    }
    // 检查请求类型是否为标准请求，接收者为设备 (bmRequestType = 0x00)
    else if ((p_setup->bmRequestType) == 0x00) // 标准请求，接收者为设备
    {
        if (REMOTE_WU) // 如果设备支持远程唤醒
        {
            if ((p_setup->wValue) == 1) // 检查请求的特性编号 (wValue == 1)，表示此请求请求改变远程唤醒功能
            {
                mark_usbd_remote_wakeup_enabled = true; // 启用设备的远程唤醒功能
                nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成
                return; // 操作成功，返回
            }
        }
    }

    // 如果请求无法处理，记录错误日志
    NRF_LOG_ERROR("Unknown feature to set"); // 未知特性设置请求
    nrf_drv_usbd_setup_stall(); // 将设备进入停滞状态，表示无法处理该请求
}

/**
 * @brief 响应主机发出的 GetDescriptor 请求，返回描述符配置
 *
 * 该函数根据请求类型返回相应的描述符（如设备、配置、接口、端点等）。
 * 处理的描述符类型包括设备描述符、配置描述符、字符串描述符、
 * 接口描述符、端点描述符、HID描述符和HID报告描述符。
 *
 * @param[in] p_setup 指向当前USB设置请求数据的指针，
 *                    包含请求的详细信息如请求的描述符类型和值。
 */
void respond_get_descriptor_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    // 解析wValue的高8位，确定请求的描述符类型
    switch ((p_setup->wValue) >> 8)
    {
    case 1: // 设备描述符
        if ((p_setup->bmRequestType) == 0x80) // 请求类型为 标准 设备到主机 设备 请求
        {
            respond_setup_request(
                p_setup,
                get_descriptor_device, // 返回设备描述符
                sizeof(get_descriptor_device)); // 传输的字节大小
            return;
        }
        break;
    case 2: // 配置描述符
        if ((p_setup->bmRequestType) == 0x80) // 请求类型为标准IN请求
        {
            respond_setup_request(
                p_setup,
                get_descriptor_configuration, // 返回配置描述符
                GET_CONFIG_DESCRIPTOR_SIZE); // 传输的字节大小
            return;
        }
        break;
    case 3: // 字符串描述符
        if ((p_setup->bmRequestType) == 0x80) // 请求类型为 标准 设备到主机 设备 请求
        {
            // 根据wValue的低8位选择请求的字符串描述符
            switch ((p_setup->wValue) & 0xFF)
            {
            case USBD_STRING_LANG_IX: // 语言字符串描述符 0x00
                respond_setup_request(
                    p_setup,
                    get_descriptor_string_language, // 返回语言字符串描述符
                    sizeof(get_descriptor_string_language));
                return;
            case USBD_STRING_MANUFACTURER_IX: // 厂商字符串描述符 0x01
                respond_setup_request(
                    p_setup,
                    get_descriptor_string_manufactuer, // 返回厂商字符串描述符
                    sizeof(get_descriptor_string_manufactuer));
                return;
            case USBD_STRING_PRODUCT_IX: // 产品字符串描述符 0x02
                respond_setup_request(
                    p_setup,
                    get_descriptor_string_product, // 返回产品字符串描述符
                    sizeof(get_descriptor_string_product));
                return;
            default:
                break;
            }
        }
        break;
    case 4: // 接口描述符
        if ((p_setup->bmRequestType) == 0x80) // 请求类型为 标准 设备到主机 设备 请求
        {
            if (((p_setup->wValue) & 0xFF) == 0) // 仅支持接口0
            {
                respond_setup_request(
                    p_setup,
                    get_descriptor_interface_0, // 返回接口0描述符
                    GET_INTERFACE_DESCRIPTOR_SIZE); // 发送接口描述符
                return;
            }
        }
        break;
    case 5: // 端点描述符
        if ((p_setup->bmRequestType) == 0x80) // 请求类型为 标准 设备到主机 设备 请求
        {
            if (((p_setup->wValue) & 0xFF) == 1) // 仅支持端点1
            {
                respond_setup_request(
                    p_setup,
                    get_descriptor_endpoint_1, // 返回端点1描述符
                    GET_ENDPOINT_DESC_SIZE); // 发送端点描述符
                return;
            }
        }
        break;
		case 6: // 设备限定描述符
        if ((p_setup->bmRequestType) == 0x80) // 请求类型为 标准 设备到主机 设备 请求
        {
						nrf_drv_usbd_setup_stall(); // 不支持该请求，发送STALL信号
            return;
        }
        break;
    case 0x21: // HID描述符请求
        if ((p_setup->bmRequestType) == 0x81) // 请求类型为 标准 设备到主机 接口 请求
        {
            // 检查请求的接口
            if (((p_setup->wValue) & 0xFF) == 0) // 仅支持接口0
            {
                respond_setup_request( // 发送HID接口描述符
                    p_setup,
                    get_descriptor_hid_0,
                    GET_HID_DESCRIPTOR_SIZE);
                return; // 处理完毕，返回
            }
        }
        break;

    case 0x22: // HID报告描述符请求
        if ((p_setup->bmRequestType) == 0x81) // 请求类型为 标准 设备到主机 接口 请求
        {
            // 检查请求的接口
            if (((p_setup->wValue) & 0xFF) == 0) // 仅支持接口0
            {
                respond_setup_request( // 发送HID报告描述符
                    p_setup,
                    get_descriptor_report_interface_0,
                    sizeof(get_descriptor_report_interface_0));
                return; // 处理完毕，返回
            }
        }
        break;

    default:
        break; // 不支持的请求类型，进入停滞状态
    }

    // 日志记录未知请求的信息
    NRF_LOG_ERROR("Unknown descriptor requested: 0x%2x, type: 0x%2x or value: 0x%2x",
        p_setup->wValue >> 8, // 请求描述符的类型
        p_setup->bmRequestType, // 请求类型
        p_setup->wValue & 0xFF); // 请求的特定值
    nrf_drv_usbd_setup_stall(); // 进入停滞状态
}


/**
 * @brief 处理GetConfig请求
 *
 * 根据设备的配置状态，发送相应的配置响应。
 * @param[in] p_setup 指向设置请求数据的指针。
 */
void respond_get_config_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    if (mark_usbd_configured) // 检查设备是否已配置
    {
        respond_setup_request( // 发送已配置的响应
            p_setup,
            get_config_response_configured,	// 已配置
            sizeof(get_config_response_configured));
    }
    else
    {
        respond_setup_request( // 发送未配置的响应
            p_setup,
            get_config_response_unconfigured, // 未配置
            sizeof(get_config_response_unconfigured));
    }
}

/**
 * @brief 处理SetConfig请求
 *
 * 根据设置请求中的参数更新设备配置状态。
 * @param[in] p_setup 指向设置请求数据的指针。
 */
void respond_set_config_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    if ((p_setup->bmRequestType) == 0x00) // 确保请求类型为 标准 主机到设备 设备 请求
    {
        // 仅接受配置索引0和1，并且请求长度为0
        if (((p_setup->wIndex) == 0) && ((p_setup->wLength) == 0) &&
            ((p_setup->wValue) <= UINT8_MAX))
        {
            // 调用configure_endpoint函数设置端点1配置
            if (NRF_SUCCESS == configure_endpoint((uint8_t)(p_setup->wValue),NRF_DRV_USBD_EPIN1))
            {
                nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成
                return; // 处理成功，返回
            }
        }
    }
    // 日志记录错误的配置请求
    NRF_LOG_ERROR("Wrong configuration: Index: 0x%2x, Value: 0x%2x.",
        p_setup->wIndex, // 请求的索引
        p_setup->wValue); // 请求的值
    nrf_drv_usbd_setup_stall(); // 进入停滞状态
}

/**
 * @brief 处理SetIdle请求 TODO：空闲状态管理
 *
 * 接收空闲请求并接受了请求，清除设置阶段，处理完成。
 * @param[in] p_setup 指向设置请求数据的指针。
 */
void respond_set_idle_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    if (p_setup->bmRequestType == 0x21) // 确保请求类型为 class 主机到设备 接口 请求
    {
        // 接受任何值
        nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成
        return; // 处理成功，返回
    }
    // 日志记录错误的请求类型
    NRF_LOG_ERROR("Set Idle wrong type: 0x%2x.", p_setup->bmRequestType);
    nrf_drv_usbd_setup_stall(); // 进入停滞状态
}

/**
 * @brief 处理SetInterface请求
 *
 * 不支持替代设置，始终进入停滞状态。
 * @param[in] p_setup 指向设置请求数据的指针。
 */
void respond_set_interface_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    // 不支持任何替代接口，始终进入停滞状态
    NRF_LOG_ERROR("No alternate interfaces supported.");
    nrf_drv_usbd_setup_stall(); // 进入停滞状态
}

/**
 * @brief 处理SetProtocol请求 TODO: 引导协议
 *
 * 接收设置协议请求并接受了请求，清除设置阶段，处理完成。
 * @param[in] p_setup 指向设置请求数据的指针。
 */
static void respond_set_protocol_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    if (p_setup->bmRequestType == 0x21) // 确保请求类型为 class 主机到设备 接口 请求
    {
        // 接受任何值
        nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成
        return; // 处理成功，返回
    }
    // 日志记录错误的请求类型
    NRF_LOG_ERROR("Set Protocol wrong type: 0x%2x.", p_setup->bmRequestType);
    nrf_drv_usbd_setup_stall(); // 进入停滞状态
}

 /****************************************
 *            USBD 事件处理函数  			   *
 ****************************************/
/**
 * @brief USB 事件处理程序
 *
 * 根据接收到的 USB 事件类型，执行相应的操作，包括暂停、恢复、重置等。
 * @param[in] p_event 指向事件数据的指针。
 */
void usb_device_event_handler(nrf_drv_usbd_evt_t const * const p_event)
{
    switch (p_event->type) // 根据事件类型进行处理
    {
    case NRF_DRV_USBD_EVT_SUSPEND:	// 请求暂停
        NRF_LOG_INFO("Suspend Requested"); 
        mark_usbd_suspend_state_request = true; // 设置暂停请求状态
        break;

    case NRF_DRV_USBD_EVT_RESUME: // 请求从暂停恢复
        NRF_LOG_INFO("Suspend Resume Requested");
        mark_usbd_suspend_state_request = false; // 清除暂停请求状态
        break;

    case NRF_DRV_USBD_EVT_WUREQ:	// 请求远程唤醒
        NRF_LOG_INFO("Remote Wake Up Requested"); 
        mark_usbd_suspend_state_request = false; // 清除暂停请求状态
        break;

    case NRF_DRV_USBD_EVT_RESET: // 请求重置
        {
						NRF_LOG_INFO("Reset Requested"); 
            ret_code_t ret = configure_endpoint(0, NRF_DRV_USBD_EPIN1); // 重置端点1配置
            ASSERT(ret == NRF_SUCCESS); // 确保重置成功
            UNUSED_VARIABLE(ret); // 防止未使用变量警告
            mark_usbd_suspend_state_request = false; // 清除暂停请求状态
            break;
        }

    case NRF_DRV_USBD_EVT_SOF: // 起始帧 Start Of Frame， 提供视觉反馈，指示设备的 USB 状态
        {
						if (mark_usbd_configured)
						{
								nrf_gpio_pin_clear(LED_USB_CONFIGURED); // 打开表示USB状态的LED
						}
						else
						{
								static uint32_t cycle = 0; // 计数周期
								++cycle; // 增加周期计数
								if (cycle % 1000 == 0) // 已配置则常亮，否则1000个周期闪烁一次，周期取决于端点描述符轮询配置
								{
									nrf_gpio_pin_toggle(LED_USB_CONFIGURED);
								}
						}
            break;
        }

    case NRF_DRV_USBD_EVT_EPTRANSFER: // 端点传输事件处理，端点完成传输时触发，输入端点向主机发送
        if (NRF_DRV_USBD_EPIN1 == p_event->data.eptransfer.ep) // 检查是否由IN端点1完成发送
        {
            mark_send_device_data = false; // 表示此次设备数据报告已发送完成，不需要再次发送，直到需要新的位置报告时
        }
        else if (NRF_DRV_USBD_EPIN0 == p_event->data.eptransfer.ep) // 检查是否由IN端点0完成发送
        {
            if (NRF_USBD_EP_OK == p_event->data.eptransfer.status) // 传输成功
            {
                nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成
            }
            else if (NRF_USBD_EP_ABORTED == p_event->data.eptransfer.status) // 传输中止
            {
                NRF_LOG_INFO("Transfer aborted event on EPIN0"); // 记录中止事件
            }
            else // 传输失败
            {
                NRF_LOG_ERROR("Transfer failed on EPIN0: %d", p_event->data.eptransfer.status); // 记录错误
                nrf_drv_usbd_setup_stall(); // 进入停滞状态
            }
        }
        else if (NRF_DRV_USBD_EPOUT0 == p_event->data.eptransfer.ep) // 检查是否由OUT端点0完成接收
        {
            if (NRF_USBD_EP_OK == p_event->data.eptransfer.status) // 传输成功
            {
                nrf_drv_usbd_setup_clear(); // 接受了请求，清除设置阶段，处理完成
            }
            else if (NRF_USBD_EP_ABORTED == p_event->data.eptransfer.status) // 传输中止
            {
                NRF_LOG_INFO("Transfer aborted event on EPOUT0"); // 记录中止事件
            }
            else // 传输失败
            {
                NRF_LOG_ERROR("Transfer failed on EPOUT0: %d", p_event->data.eptransfer.status); // 记录错误
                nrf_drv_usbd_setup_stall(); // 进入停滞状态
            }
        }
        else
        {
            // 无需处理的情况
        }
        break;

    case NRF_DRV_USBD_EVT_SETUP: // 处理设置请求事件，主机请求设置从机/获取从机信息
        {
            nrf_drv_usbd_setup_t setup; // 设置请求包体
            nrf_drv_usbd_setup_get(&setup); // 获取设置请求数据 -> 请求包体
            switch (setup.bRequest) // 根据请求类型进行处理
            {
            case 0x00: // GetStatus
                respond_get_status_request(&setup); // 获取状态请求处理
                break;
            case 0x01: // ClearFeature
                respond_clear_feature_request(&setup); // 清除特性请求处理
                break;
            case 0x03: // SetFeature
                respond_set_feature_request(&setup); // 设置特性请求处理
                break;
            case 0x05: // SetAddress
                // 不做任何操作，由硬件处理，但不进入 STALL
                break;
            case 0x06: // GetDescriptor
                respond_get_descriptor_request(&setup); // 获取描述符请求处理
                break;
            case 0x08: // GetConfig
                respond_get_config_request(&setup); // 获取配置请求处理
                break;
            case 0x09: // SetConfig
                respond_set_config_request(&setup); // 设置配置请求处理
                break;
            // HID 类
            case 0x0A: // SetIdle
                respond_set_idle_request(&setup); // 设置空闲请求处理
                break;
            case 0x0B: // SetProtocol 或 SetInterface
                if (setup.bmRequestType == 0x01) // 标准请求，接收者为接口
                {
                    respond_set_interface_request(&setup); // 设置接口请求处理
                }
                else if (setup.bmRequestType == 0x21) // 类请求，接收者为接口
                {
                    respond_set_protocol_request(&setup); // 设置协议请求处理
                }
                else
                {
                    NRF_LOG_ERROR("Command 0xB. Unknown request: 0x%2x", setup.bmRequestType); // 记录未知请求错误
                    nrf_drv_usbd_setup_stall(); // 进入停滞状态
                }
                break;
            default:
                NRF_LOG_ERROR("Unknown request: 0x%2x", setup.bRequest); // 记录未知请求错误
                nrf_drv_usbd_setup_stall(); // 进入停滞状态
                return; // 返回
            }
            break;
        }

    default:
        break; // 处理其他事件
    }
}

 /****************************************
 *            USBD 电源函数  			     	*
 ****************************************/
/**
 * @brief USB 电源事件处理程序
 *
 * 根据 USB 电源事件类型，执行相应的操作，例如检测到 USB 电源、移除电源或准备就绪。
 * @param event USB 电源事件类型
 */
void usb_device_power_event_handler(nrf_drv_power_usb_evt_t event)
{
    switch (event) // 根据事件类型进行处理
    {
    case NRF_DRV_POWER_USB_EVT_DETECTED: // 检测到 USB 电源
        NRF_LOG_INFO("USB Power Detected"); // 记录检测到电源
        if (!nrf_drv_usbd_is_enabled()) // 如果 USB 尚未启用
        {
            nrf_drv_usbd_enable(); // 启用 USB
        }
        break;

    case NRF_DRV_POWER_USB_EVT_REMOVED: // USB 电源被移除
        NRF_LOG_INFO("USB Power Removed"); // 记录电源移除
        mark_usbd_configured = false; // 设置 USB 配置为未配置
        mark_send_device_data = false; // 清除发送设备数据标志
        if (nrf_drv_usbd_is_started()) // 如果 USB 已启动
        {
            nrf_drv_usbd_stop(); // 停止 USB
        }
        if (nrf_drv_usbd_is_enabled()) // 如果 USB 已启用
        {
            nrf_drv_usbd_disable(); // 禁用 USB
        }
        /* 关闭 LED */
        nrf_gpio_pin_set(LED_USB_CONFIGURED); // 关闭 USB 状态 LED
        nrf_gpio_pin_set(LED_USB_POWER_CONNECTED ); // 关闭 USB 电源 LED
        break;

    case NRF_DRV_POWER_USB_EVT_READY: // USB 准备就绪
        NRF_LOG_INFO("USB Power Ready With Power Detection"); // 记录 USB 电源准备就绪
        nrf_gpio_pin_clear(LED_USB_POWER_CONNECTED ); // 打开表示USB电源的指示灯
        if (!nrf_drv_usbd_is_started()) // 如果 USB 尚未启动
        {
            nrf_drv_usbd_start(true); // 启动 USB
        }
        break;

    default:
        ASSERT(false); // 如果遇到未知事件，断言失败
    }
}

/****************************************
 *              GPIO 函数               *
 ****************************************/
/**
 * @brief 处理GPIO事件
 * @param pin 引脚号
 * @param action GPIO引脚动作（上升沿或下降沿）
 * 
 * 根据引脚状态执行相应操作，包括系统关闭请求的设置。
 */
void gpiote_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (pin == BTN_SYSTEM_OFF) // 检查是否是系统关闭按钮的引脚
    {
        if (action == NRF_GPIOTE_POLARITY_HITOLO) // 按钮释放触发
        {		
						nrf_gpio_pin_set(LED_USB_POWER_CONNECTED ); // 关闭表示系统运行状态的LED
						nrf_gpio_pin_set(LED_USB_CONFIGURED); // 关闭表示系统活跃状态的LED
            nrf_gpio_pin_set(LED_SYSTEM_RUNNING); // 关闭表示系统运行状态的LED
						nrf_gpio_pin_set(LED_SYSTEM_ON); // 关闭表示系统活跃状态的LED
						if (nrf_drv_usbd_is_started()) // 如果 USB 已启动
						{
								nrf_drv_usbd_stop(); // 停止 USB
						}
						if (nrf_drv_usbd_is_enabled()) // 如果 USB 已启用
						{
								nrf_drv_usbd_disable(); // 禁用 USB
						}
						NRF_LOG_INFO("System OFF"); // 打印系统即将关机的日志信息
						NRF_LOG_FLUSH();
            nrf_power_system_off(); // 将系统进入关机模式
        }
    }
}

/**
 * @brief 初始化GPIO
 * 
 * 初始化GPIO并配置
 */
void usb_device_gpio_init(void)
{
    nrf_gpio_cfg_output(LED_USB_POWER_CONNECTED ); // USB 电源指示灯
		nrf_gpio_cfg_output(LED_USB_CONFIGURED); // USB 状态指示灯
		nrf_gpio_cfg_output(LED_SYSTEM_RUNNING); // USB 运行指示灯
		nrf_gpio_cfg_output(LED_SYSTEM_ON); // USB 活跃指示灯
}

/**
 * @brief 初始化GPIOTE
 * 
 * 初始化GPIOTE模块并配置按钮引脚和事件处理程序。
 */
void usb_device_gpiote_init(void)
{
    ret_code_t ret;

    // 初始化GPIOTE模块
    if (!nrf_drv_gpiote_is_init()) 
    {
        ret = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(ret);
    }

    // 配置系统关闭按钮引脚
    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true); // 配置为下降沿触发
		config.pull = NRF_GPIO_PIN_PULLUP; // 使用内部上拉电阻
    ret = nrf_drv_gpiote_in_init(BTN_SYSTEM_OFF, &config, gpiote_evt_handler); // 初始化引脚
    APP_ERROR_CHECK(ret);

    // 启用引脚事件
    nrf_drv_gpiote_in_event_enable(BTN_SYSTEM_OFF, true);

    /* 避免在禁用断言时的警告 */
    UNUSED_VARIABLE(ret);
}

 /****************************************
 *            时钟 & 电源函数		     	  *
 ****************************************/
/**
 * @brief 初始化时钟模块
 *
 * 初始化高频和低频时钟，确保设备的时钟源可用。
 */
void usb_device_init_power_and_clock(void)
{
    ret_code_t ret;
		
    // 初始化时钟
    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret); // 检查时钟初始化是否成功
		
		// 初始化电源
		ret = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(ret); // 检查电源初始化是否成功

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

 /****************************************
 *               复位函数  			     	  *
 ****************************************/
/**
 * @brief 记录复位原因
 * 
 * 读取并记录设备复位原因，帮助调试和故障排查。
 */
void usb_device_log_reset_reason(void)
{
    /* 复位原因 */
    uint32_t rr = nrf_power_resetreas_get(); // 获取复位原因
    NRF_LOG_INFO("Reset reasons:");
    if (0 == rr)
    {
        NRF_LOG_INFO("- NONE"); // 无复位原因
    }
    if (0 != (rr & NRF_POWER_RESETREAS_RESETPIN_MASK))
    {
        NRF_LOG_INFO("- RESETPIN"); // 复位引脚复位
    }
    if (0 != (rr & NRF_POWER_RESETREAS_DOG_MASK))
    {
        NRF_LOG_INFO("- DOG"); // 看门狗复位
    }
    if (0 != (rr & NRF_POWER_RESETREAS_SREQ_MASK))
    {
        NRF_LOG_INFO("- SREQ"); // 软件请求复位
    }
    if (0 != (rr & NRF_POWER_RESETREAS_LOCKUP_MASK))
    {
        NRF_LOG_INFO("- LOCKUP"); // 锁死复位
    }
    if (0 != (rr & NRF_POWER_RESETREAS_OFF_MASK))
    {
        NRF_LOG_INFO("- OFF"); // 关闭复位
    }
#if defined(NRF_POWER_RESETREAS_LPCOMP_MASK)
    if (0 != (rr & NRF_POWER_RESETREAS_LPCOMP_MASK))
    {
        NRF_LOG_INFO("- LPCOMP"); // 比较器复位
    }
#endif
    if (0 != (rr & NRF_POWER_RESETREAS_DIF_MASK))
    {
        NRF_LOG_INFO("- DIF"); // 差异复位
    }
#if defined(NRF_POWER_RESETREAS_NFC_MASK)
    if (0 != (rr & NRF_POWER_RESETREAS_NFC_MASK))
    {
        NRF_LOG_INFO("- NFC"); // NFC复位
    }
#endif
    if (0 != (rr & NRF_POWER_RESETREAS_VBUS_MASK))
    {
        NRF_LOG_INFO("- VBUS"); // VBUS复位
    }
}

/****************************************
 *               挂起函数  			   		*
 ****************************************/
/**
 * @brief 处理USB的状态,强制进入挂起模式
 * 
 */
void usb_device_force_suspend(void)
{
		NRF_LOG_INFO("Suspend Requested, Going to Suspend"); // 打印挂起日志信息
		NRF_LOG_FLUSH();
		nrfx_usbd_force_bus_suspend(); // 强制总线挂起 **自定义，危险操作
		mark_usbd_suspended = nrf_drv_usbd_suspend(); // 尝试进入USB挂起状态并更新标志
		if (mark_usbd_suspended) // 如果USB成功进入挂起状态
		{
				nrf_gpio_pin_set(LED_SYSTEM_RUNNING); // 关闭表示系统运行状态的LED
				nrf_gpio_pin_set(LED_USB_POWER_CONNECTED ); // 关闭表示系统运行状态的LED
		}
}

/**
 * @brief 处理USB的状态,强制离开挂起模式
 * 
 */
void usb_device_force_leaving_suspend(void)
{
		NRF_LOG_INFO("Leaving From Suspend");
		NRF_LOG_FLUSH();
		if(mark_usbd_configured)
		{
				nrf_gpio_pin_clear(LED_USB_CONFIGURED);
		}
		if(NRF_DRV_POWER_USB_STATE_READY == nrf_drv_power_usbstatus_get())
		{
				nrf_gpio_pin_clear(LED_USB_POWER_CONNECTED);
		}
		nrf_gpio_pin_clear(LED_SYSTEM_RUNNING); // 关闭表示系统运行状态的LED
		mark_usbd_suspended = false; // 将挂起标志设为false
}

