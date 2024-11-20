/***************************************
 *              库文件             		 *
 ***************************************/
#include "mouse.h"   // 鼠标头文件

/***************************************
 *             鼠标变量            		 *
 ***************************************/
// DPI预设
static const uint16_t dpi_presets[NUM_MOUSE_DPI_PRESET][2] = 
{
    {800, 800},   // DPI预设1
    {1200, 1200}, // DPI预设2
    {1600, 1600}, // DPI预设3
    {2400, 2400}, // DPI预设4
    {3200, 3200}  // DPI预设5
};

// 当前DPI索引
static uint8_t current_dpi_index = 1; // 默认使用第二组DPI

// 鼠标数据
static uint8_t button_data = 0x00;	// 存储按键数据
static int8_t wheel_data = 0x00;	// 存储滚轮数据
static uint8_t databuffer[6];  // 用于存储发送的鼠标数据
static nrf_esb_payload_t tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // 定义并初始化一个ESB发射（TX）负载，包含初始数据（8字节）

// 传感器运动位
static bool motion_state = 0;

// 按键去抖计数
static uint8_t left_button_count = 0;
static uint8_t right_button_count = 0;
static uint8_t middle_button_count = 0;
static uint8_t side_button_1_count = 0;
static uint8_t side_button_2_count = 0;
static uint8_t dpi_increase_button_count = 0;
static uint8_t dpi_decrease_button_count = 0;

// 按键当前位
static bool left_button_state = 0;
static bool right_button_state = 0;
static bool middle_button_state = 0;
static bool side_button_1_state = 0;
static bool side_button_2_state = 0;
static bool dpi_increase_button_state = 0;
static bool dpi_decrease_button_state = 0;

// 按键指示位
static uint8_t left_button_bit = 0;
static uint8_t right_button_bit = 0;
static uint8_t middle_button_bit = 0;
static uint8_t side_button_1_bit = 0;
static uint8_t side_button_2_bit = 0;
static uint8_t dpi_increase_button_bit = 0;
static uint8_t dpi_decrease_button_bit = 0;

// 按键历史
static uint8_t last_button_data = 0x00;

// 滚轮的A相和B相状态
static uint8_t last_wheel_data = 0x00;
static uint8_t current_wheel_data = 0x00;

// 传感器偏移
static int16_t delta_x = 0x00;
static int16_t delta_y = 0x00;

// 挂起计数器
uint32_t suspend_counter = 0;
uint32_t suspend_time_ticks = SUSPEND_TIME * configTICK_RATE_HZ * 20; // ** 不准，待改进
bool mark_device_suspended = false;

/***************************************
 *           鼠标模式变量            	 *
 ***************************************/
 static bool mark_use_mode_usb = false;
 static bool mark_use_mode_ble = false;
 static bool mark_use_mode_esb = false;
 static bool wireless_mode_choose_ble = true;

/***************************************
 *           鼠标辅助函数               *
 ***************************************/
 /**
 * @brief 通用按键去抖动处理函数
 *
 * @param[in] button_state 当前按键状态（1 表示松开，0 表示按下）
 * @param[in,out] button_count 按键按下计数器
 * @param[in,out] button_bit 按键位，表示按键是否已被确认按下
 */
void mouse_button_debounce(bool button_state, uint8_t *button_count, uint8_t *button_bit)
{
    if (!button_state) // 检测到按键按下
    {
        if (*button_count < DEBOUNCE_COUNT)
        {
            (*button_count)++;
        }
        if (*button_count >= DEBOUNCE_COUNT)
        {
            *button_bit = 1; // 确认按下状态
        }
    }
    else
    {
        *button_bit = 0;     // 更新为松开状态
        *button_count = 0;   // 重置计数器
    }
}

/**
 * @brief DPI更新处理
 *
 * @param[in] increase 是否增加DPI（true表示增加，false表示减少）
 */
void mouse_dpi_update(bool increase)
{
    if (increase)
    {
        if (current_dpi_index < 4) 
        {
            current_dpi_index++;
        }
        else
        {
            current_dpi_index = 0; 
        }
        while (!nrf_gpio_pin_read(MOUSE_BTN_DPI_INCREASE)); // 等待按键松开
    }
    else
    {
        if (current_dpi_index > 0) 
        {
            current_dpi_index--;
        }
        else
        {
            current_dpi_index = 4;
        }
        while (!nrf_gpio_pin_read(MOUSE_BTN_DPI_DECREASE)); // 等待按键松开
    }

    // 更新X和Y的DPI分辨率
    paw3399_set_xy_resolution(dpi_presets[current_dpi_index][0], dpi_presets[current_dpi_index][1]);
    NRF_LOG_INFO("DPI Updated, X: %d, Y: %d", paw3399_get_var_x_resolution(), paw3399_get_var_y_resolution());
    NRF_LOG_FLUSH();
}

/**
 * @brief 根据鼠标外设状态触发软复位
 * 
 */
void mouse_connection_mode_update(void)
{
		// 如果连接状态改变则触发软复位
		if(mark_use_mode_usb && nrf_drv_power_usbstatus_get() != NRF_DRV_POWER_USB_STATE_READY) // usb模式，usb断开
		{
			NVIC_SystemReset(); // 触发软复位
		}
		else if((mark_use_mode_ble || mark_use_mode_esb) && nrf_drv_power_usbstatus_get() == NRF_DRV_POWER_USB_STATE_READY) // esb/ble模式，usb连接
		{
			NVIC_SystemReset(); // 触发软复位
		}
		else if (mark_use_mode_ble && !wireless_mode_choose_ble) // ble模式，esb连接
		{
			NVIC_SystemReset(); // 触发软复位
		}
		else if(mark_use_mode_esb && wireless_mode_choose_ble) // esb模式，ble连接
		{
			NVIC_SystemReset(); // 触发软复位
		}
		
		// 根据标记设置工作模式
		if(nrf_drv_power_usbstatus_get() == NRF_DRV_POWER_USB_STATE_READY) // usb连接
		{
				if(!mark_use_mode_usb) // 非usb模式，启用usb
				{
					NRF_LOG_INFO("USB Mode");
					NRF_LOG_FLUSH();
					
					// 设置标记
					mark_use_mode_usb = true;
					mark_use_mode_ble = false;
					mark_use_mode_esb = false;
					
					// 初始化挂起变量
					mark_device_suspended = false;
					suspend_counter = 0;
				}
		}
		else if(!mark_use_mode_ble && wireless_mode_choose_ble)
		{	
				NRF_LOG_INFO("BLE Mode");
				NRF_LOG_FLUSH();
				
				// 蓝牙模块初始化
				ble_device_init();
			
				// 设置标记
				mark_use_mode_usb = false;
				mark_use_mode_ble = true;
				mark_use_mode_esb = false;
			
				// 初始化挂起变量
				mark_device_suspended = false;
				suspend_counter = 0;
			
				// 开始广播
				ble_device_advertising_start(false,false);
				mouse_wait_for_ble_connection();
		}
		else if(!mark_use_mode_esb && !wireless_mode_choose_ble)
		{
				NRF_LOG_INFO("ESB Mode");
				NRF_LOG_FLUSH();
				
				// ESB模块初始化
				esb_device_init();
				
				// 配置连接LED状态
				nrf_gpio_pin_clear(LED_MOUSE_CONNECTED); // 打开 鼠标连接 LED
				nrf_gpio_pin_set(LED_MOUSE_SUSPENDED); // 关闭 鼠标挂起 LED
				
				// 设置标记
				mark_use_mode_usb = false;
				mark_use_mode_ble = false;
				mark_use_mode_esb = true;
			
				// 初始化挂起变量
				mark_device_suspended = false;
				suspend_counter = 0;
		}
}

/**
 * @brief 鼠标等待蓝牙连接的处理函数
 * 
 */
void mouse_wait_for_ble_connection(void)
{
    uint32_t led_toggle_interval_count = 100000;  // 计数器翻转间隔，假设每次循环计数 +1 表示 1ms
    uint32_t counter = 0;                      // 简单的增值计数器

    while (ble_device_get_module_connection_handle() == BLE_CONN_HANDLE_INVALID) // 检查是否未连接
    {
        // 是否有鼠标连接状态更改
        if (!nrf_gpio_pin_read(MOUSE_MODE) || nrf_drv_power_usbstatus_get() == NRF_DRV_POWER_USB_STATE_READY)
        {
            NRF_LOG_WARNING("Mouse mode or USB ready detected. Triggering reset.");
            NVIC_SystemReset(); // 触发软复位
        }

        // 处理配对按键
        mouse_ble_pairing_handler();

        // 执行调度器中的待处理任务
        ble_device_idle_state_handle();

        // 计数器递增
        counter++;

        // 检查是否达到 LED 翻转的计数值
        if (counter >= led_toggle_interval_count)
        {
            nrf_gpio_pin_toggle(LED_MOUSE_CONNECTED); // 翻转 LED
            counter = 0; // 重置计数器
        }
    }
    nrf_gpio_pin_clear(LED_MOUSE_CONNECTED);  // 打开鼠标连接 LED
}



/**
 * @brief 鼠标BLE配对按键的处理函数
 * 
 */
void mouse_ble_pairing_handler(void)
{
		// 是否有鼠标配对按钮按下
		if(!nrf_gpio_pin_read(BTN_BLE_PAIRING))
		{
			// 检查sdh协议栈有效性
			if(nrf_sdh_is_enabled())
			{
				// 等待按下3s
				nrf_delay_ms(3000);
				
				// 去抖
				if(!nrf_gpio_pin_read(BTN_BLE_PAIRING))
				{
					// 去除绑定，重新开始广播
					NRF_LOG_INFO("Entering pairing mode");
					NRF_LOG_FLUSH();
					
					ble_device_advertising_start(true,false);
					mouse_wait_for_ble_connection();
				}
			}
		}
}

/**
 * @brief 鼠标BLE配对按键的处理函数
 * 
 */
void mouse_ble_host_disconnect_handler(void)
{
		// 是否为蓝牙模式
		if(mark_use_mode_ble)
		{
			// 主设备断开连接
			if(ble_device_get_module_connection_handle() == BLE_CONN_HANDLE_INVALID)
			{
				NRF_LOG_INFO("Host Disconnected");
				NRF_LOG_FLUSH();
				
				// 重新配对
				ble_device_advertising_start(false ,false);
				mouse_wait_for_ble_connection();
			}
		}
}

/**
 * @brief 鼠标系统状态更新的函数
 * 
 */
void mouse_system_status_update(void)
{
		// USB系统状态更新
		if(mark_use_mode_usb)
		{
				// 检查是否需要改变USB挂起状态
				if (usb_device_get_mark_usbd_suspended() != usb_device_get_mark_usbd_suspend_state_request())
				{
						// 如果请求USB进入挂起状态
						if (usb_device_get_mark_usbd_suspend_state_request()) 
						{
								usb_device_force_suspend();
						}
						// 如果请求退出USB挂起状态
						else 
						{
								usb_device_force_leaving_suspend();
						}
				}
		}
		
		// BLE系统状态更新
		if(mark_use_mode_ble)
		{
				ble_device_idle_state_handle();
		}
		
		// ESB系统状态更新
		if(mark_use_mode_esb)
		{
				
		}
}

/**
 * @brief 鼠标数据状态更新的函数
 * 
 */
void mouse_data_update(void)
{
		// 获取传感器的 delta_x 和 delta_y 值
		motion_state = nrf_gpio_pin_read(MOTION_PIN);
		if(!motion_state)
		{
				paw3399_get_delta_xy();
				delta_x = paw3399_get_var_delta_x();
				delta_y = paw3399_get_var_delta_y();
		}
		
		// 将A相和B相的上次状态和当前状态组合成一个状态变量
		current_wheel_data = (nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_A) << 1) | nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_B);  // 当前状态
		
		// 编码器状态的状态机转换表：
		// 顺时针 (CW) 旋转时，状态的顺序应该是：00 -> 01 -> 11 -> 10 -> 00
		// 逆时针 (CCW) 旋转时，状态的顺序应该是：00 -> 10 -> 11 -> 01 -> 00
		
		// 顺时针判断
		if ((last_wheel_data == 0 && current_wheel_data == 1) ||
				(last_wheel_data == 1 && current_wheel_data == 3) ||
				(last_wheel_data == 3 && current_wheel_data == 2) ||
				(last_wheel_data == 2 && current_wheel_data == 0))
		{
				wheel_data = -1;  // 顺时针旋转
		}
		// 逆时针判断
		else if ((last_wheel_data == 0 && current_wheel_data == 2) ||
						(last_wheel_data == 2 && current_wheel_data == 3) ||
						(last_wheel_data == 3 && current_wheel_data == 1) ||
						(last_wheel_data == 1 && current_wheel_data == 0))
		{
				wheel_data = 1;  // 逆时针旋转
		}
		else
		{
				wheel_data = 0;  // 没有变化
		}
		
		// 更新上次A相和B相的状态
		last_wheel_data = current_wheel_data;
		
		// 获取当前引脚状态
		left_button_state = nrf_gpio_pin_read(MOUSE_BTN_LEFT);
		right_button_state = nrf_gpio_pin_read(MOUSE_BTN_RIGHT);
    middle_button_state = nrf_gpio_pin_read(MOUSE_BTN_MIDDLE);
		side_button_1_state = nrf_gpio_pin_read(MOUSE_BTN_SIDE_1);
    side_button_2_state = nrf_gpio_pin_read(MOUSE_BTN_SIDE_2);
		dpi_increase_button_state = nrf_gpio_pin_read(MOUSE_BTN_DPI_INCREASE);
    dpi_decrease_button_state = nrf_gpio_pin_read(MOUSE_BTN_DPI_DECREASE);
		
    // 左键去抖动处理
    mouse_button_debounce(left_button_state, &left_button_count, &left_button_bit);
		
    // 右键去抖动处理
    mouse_button_debounce(right_button_state, &right_button_count, &right_button_bit);
		
    // 中键去抖动处理
    mouse_button_debounce(middle_button_state, &middle_button_count, &middle_button_bit);
		
		// 侧键1去抖动处理
		mouse_button_debounce(side_button_1_state, &side_button_1_count, &side_button_1_bit);
		
		// 侧键2去抖动处理
		mouse_button_debounce(side_button_2_state, &side_button_2_count, &side_button_2_bit);
		
		// DPI+键去抖动处理
		mouse_button_debounce(dpi_increase_button_state, &dpi_increase_button_count, &dpi_increase_button_bit);
		
		// DPI-键去抖动处理
		mouse_button_debounce(dpi_decrease_button_state, &dpi_decrease_button_count, &dpi_decrease_button_bit);
		
		// DPI 更新
		if (dpi_increase_button_bit)
		{
				mouse_dpi_update(true);
		}
		if (dpi_decrease_button_bit)
		{
				mouse_dpi_update(false);
		}

		
		// 更新 button_data
		button_data = 0x00;  // 重置按键数据
		button_data |= (left_button_bit << 0);   // 左键按下
		button_data |= (right_button_bit << 1);  // 右键按下
		button_data |= (middle_button_bit << 2); // 中键按下
		button_data |= (side_button_1_bit << 3);  // 侧键1按下
		button_data |= (side_button_2_bit << 4); // 侧键2按下
}

/**
 * @brief 通过usb发送鼠标数据
 * 
 */
void mouse_usb_data_send(void)
{
		// 禁用usb挂起标记
		usb_device_set_mark_usbd_suspend_state_request(false);
		
		// 如果 USB 处于挂起状态并且启用了远程唤醒
		if (usb_device_get_mark_usbd_suspended() && usb_device_get_mark_usbd_remote_wakeup_enabled())
		{
				// 请求 USB 唤醒
				UNUSED_RETURN_VALUE(nrf_drv_usbd_wakeup_req()); 
		}
		
		// 如果 USB 没有挂起
		else  
		{	
				// 按键状态（1字节）
				databuffer[0] = button_data;
		
				// X 轴变化值（2字节，16位）
				databuffer[1] = (uint8_t)delta_x;       // 低字节
				databuffer[2] = (uint8_t)(delta_x >> 8); // 高字节
				
				// Y 轴变化值（2字节，16位）
				databuffer[3] = (uint8_t)delta_y;       // 低字节
				databuffer[4] = (uint8_t)(delta_y >> 8); // 高字节
				
				// 滚轮变化值（1字节，8位）
				databuffer[5] = (uint8_t)wheel_data;
				
				// 如果当前不在发送鼠标数据
				if (usb_device_get_mark_usbd_configured() && !usb_device_get_mark_send_device_data())
				{
						// 构建用于 USB 传输的数据结构
						static const nrf_drv_usbd_transfer_t transfer =
						{
								// 设置传输数据源为数据缓冲区
								.p_data = {.tx = &databuffer},
								
								// 传输数据大小
								.size = sizeof(databuffer) 
						};
		
						usb_device_set_mark_send_device_data(true); // 设置正在发送鼠标位置标记
		
						// 使用端点1，开始USB传输
						if(nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN1, &transfer) != NRF_SUCCESS)
						{
								// 发送失败时，尝试一次重传
								NRF_LOG_INFO("USB Sending packet failed"); 
								NRF_LOG_FLUSH();
								nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN1, &transfer); 
						}

				}
		}  
}

/**
 * @brief 通过ble发送鼠标数据
 * 
 */
void mouse_ble_data_send(void)
{
		// 确保设备已连接
		if (ble_device_get_module_connection_handle() != BLE_CONN_HANDLE_INVALID)    
		{
				delta_x = MIN(delta_x, 0x0fff);
        delta_y = MIN(delta_y, 0x0fff);
			
				// 按键状态（1字节）
				databuffer[0] = button_data;
		
				// X 轴变化值（2字节，16位）
				databuffer[1] = (uint8_t)delta_x;       // 低字节
				databuffer[2] = (uint8_t)(delta_x >> 8); // 高字节
				
				// Y 轴变化值（2字节，16位）
				databuffer[3] = (uint8_t)delta_y;       // 低字节
				databuffer[4] = (uint8_t)(delta_y >> 8); // 高字节
				
				// 滚轮变化值（1字节，8位）
				databuffer[5] = (uint8_t)wheel_data;
			
				// 用于存储函数返回的错误代码
				ret_code_t err_code; 
			
				// 尝试通过BLE HID发送数据
				err_code = ble_hids_inp_rep_send(ble_device_get_module_hid_service(),            /**< 发送标准HID输入报告。 */
																				0, /**< 输入报告索引。 */
																				6,   /**< 输入报告长度。 */
																				databuffer,                   /**< HID报告数据。 */
																				ble_device_get_module_connection_handle());           /**< 当前连接句柄。 */
				
				// 处理发送错误
				if ((err_code != NRF_SUCCESS) &&                 
						(err_code != NRF_ERROR_INVALID_STATE) &&
						(err_code != NRF_ERROR_RESOURCES) &&
						(err_code != NRF_ERROR_BUSY) &&
						(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING))
				{
						// 发送失败时，尝试一次重传
						NRF_LOG_INFO("BLE Sending packet failed"); 
						NRF_LOG_FLUSH();
						ble_hids_inp_rep_send(ble_device_get_module_hid_service(),            /**< 发送标准HID输入报告。 */
																				0, /**< 输入报告索引。 */
																				6,   /**< 输入报告长度。 */
																				databuffer,                   /**< HID报告数据。 */
																				ble_device_get_module_connection_handle());           /**< 当前连接句柄。 */
						APP_ERROR_HANDLER(err_code); /**< 处理非预期错误。 */
				}
			
		}
}

/**
 * @brief 通过esb发送鼠标数据
 * 
 */
void mouse_esb_data_send(void)
{
		// 按键状态（1字节）
		tx_payload.data[0] = button_data;
		
		// X 轴变化值（2字节，16位）
		tx_payload.data[1] = (uint8_t)delta_x;       // 低字节
		tx_payload.data[2] = (uint8_t)(delta_x >> 8); // 高字节
		
		// Y 轴变化值（2字节，16位）
		tx_payload.data[3] = (uint8_t)delta_y;       // 低字节
		tx_payload.data[4] = (uint8_t)(delta_y >> 8); // 高字节
		
		// 滚轮变化值（1字节，8位）
		tx_payload.data[5] = (uint8_t)wheel_data;			

		 // 设置为false以启用自动应答ACK功能
		tx_payload.noack = false;
		
		// 尝试发送载荷数据
		if (nrf_esb_write_payload(&tx_payload) != NRF_SUCCESS) 
		{
				// 如果发送失败，尝试一次重传
				NRF_LOG_INFO("ESB Sending packet failed"); 
				NRF_LOG_FLUSH();
				nrf_esb_write_payload(&tx_payload); 
		}
}

/**
 * @brief 更新鼠标挂起状态
 * 
 */
void mouse_suspend_status_update(void)
{
		// USB 挂起逻辑 
		if(mark_use_mode_usb)
		{
				// 检查是否已经挂起
				if (!usb_device_get_mark_usbd_suspended())
				{
						// 检查是否有未处理的日志输出操作
						while (NRF_LOG_PROCESS() != false);    
					
						// 强制USB进入挂起
						usb_device_set_mark_usbd_suspend_state_request(true);
						usb_device_force_suspend();
				}
		}
		
		// BLE 挂起逻辑
		else if(mark_use_mode_ble)
		{
				// 检查是否已经挂起
				if(!mark_device_suspended)
				{
					//更新挂起变量状态
					NRF_LOG_INFO("BLE_SUSPEND");
					mark_device_suspended = true;
					nrf_gpio_pin_clear(LED_MOUSE_SUSPENDED);
				};
				// 检查是否有未处理的日志输出操作
				if (NRF_LOG_PROCESS() != false)    
				{
					// 进入低功耗模式，等待下一事件唤醒设备
					nrf_pwr_mgmt_run();           
				}
		}
		
		// ESB 挂起逻辑
		else if(mark_use_mode_esb)
		{
				// 检查是否已经挂起
				if(!mark_device_suspended)
				{
					//更新挂起变量状态
					NRF_LOG_INFO("ESB_SUSPEND");
					mark_device_suspended = true;
					nrf_gpio_pin_clear(LED_MOUSE_SUSPENDED);
				}
				// 检查是否有未处理的日志输出操作
				if (NRF_LOG_PROCESS() != false)    
				{
					// 挂起ESB设备
					nrf_esb_suspend();
					
					// 进入低功耗模式，等待下一事件唤醒设备
					nrf_pwr_mgmt_run();           
				}
		}
}

 /****************************************
 *           鼠标初始化函数  			    	*
 ****************************************/
/**
 * @brief 鼠标GPIO初始化函数
 * 
 */
void mouse_gpio_init(void)
{
		nrf_gpio_cfg_input(MOUSE_BTN_LEFT, NRF_GPIO_PIN_PULLUP); // 配置左键
		nrf_gpio_cfg_input(MOUSE_BTN_MIDDLE, NRF_GPIO_PIN_PULLUP); // 配置中键
		nrf_gpio_cfg_input(MOUSE_BTN_RIGHT, NRF_GPIO_PIN_PULLUP); // 配置左键
		nrf_gpio_cfg_input(MOUSE_BTN_SIDE_1, NRF_GPIO_PIN_PULLUP); // 配置侧键1
		nrf_gpio_cfg_input(MOUSE_BTN_SIDE_2, NRF_GPIO_PIN_PULLUP); // 配置侧键2
		nrf_gpio_cfg_input(MOUSE_BTN_DPI_INCREASE, NRF_GPIO_PIN_PULLUP); // 配置DPI+键
		nrf_gpio_cfg_input(MOUSE_BTN_DPI_DECREASE, NRF_GPIO_PIN_PULLUP); // 配置DPI-键
		nrf_gpio_cfg_input(MOUSE_WHEEL_PHASE_A, NRF_GPIO_PIN_PULLUP); // 配置滚轮A相
		nrf_gpio_cfg_input(MOUSE_WHEEL_PHASE_B, NRF_GPIO_PIN_PULLUP); // 配置滚轮B相
		
		nrf_gpio_cfg_input(MOUSE_MODE, NRF_GPIO_PIN_PULLUP); // 配置模式选择GPIO为输入模式，使用上拉电阻
		nrf_gpio_cfg_input(BTN_BLE_PAIRING, NRF_GPIO_PIN_PULLUP); // 配置模式选择GPIO为输入模式，使用上拉电阻
}

/**
 * @brief 鼠标滚轮数据初始化函数
 * 
 * 初始化鼠标滚轮数据
 */
void mouse_wheel_data_init(void)
{	
		// 初始化滚轮状态 
		last_wheel_data = (nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_A) << 1) | nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_B);
}

/**
 * @brief 鼠标初始化函数
 * 
 * 初始化鼠标模块。
 */
void mouse_init(void)
{	
		// 初始化GPIO
		mouse_gpio_init();
	
		// 初始化滚轮状态 
		mouse_wheel_data_init();
} 

/***************************************
 *          鼠标主轮询任务             *
 ***************************************/
/**
 * @brief 当鼠标有活动时，它会获取值，并发送
 * 
 */
void mouse_main_task(void)
{			
		NRF_LOG_INFO("Start Mouse");
		NRF_LOG_FLUSH();
	
		// 主轮询循环
		while(true)
    {		
				// 更新系统状态
				mouse_system_status_update();
			
				// 更新无线模式选择按钮状态
				wireless_mode_choose_ble = nrf_gpio_pin_read(MOUSE_MODE);
				
				// 更新鼠标连接模式
				mouse_connection_mode_update();
				
				// 处理无模式状态
				if(!mark_use_mode_usb && !mark_use_mode_esb && !mark_use_mode_ble)
				{
						NRF_LOG_INFO("No Operation Mode");
						NRF_LOG_FLUSH();
						continue;
				}
				
				// 处理配对按键
				mouse_ble_pairing_handler();
				
				// 处理主机主动断开
				mouse_ble_host_disconnect_handler();
				
				// 更新鼠标数据
				mouse_data_update();
				
				// 发生数据改变，发送报文
				if(wheel_data != 0 || button_data != last_button_data || delta_x != 0 || delta_y != 0) 
				{					
						// 刷新按键历史					
						last_button_data = button_data;
						
						/* USB 处理逻辑 */
						if(mark_use_mode_usb)
						{
								mouse_usb_data_send();
						}
						/* BLE 处理逻辑 */
						else if(mark_use_mode_ble)
						{
								mouse_ble_data_send();
						}
						
						/* ESB 处理逻辑 */
						else if(mark_use_mode_esb)
						{
								mouse_esb_data_send();		
						}
						
						// 刷新挂起变量
						mark_device_suspended = false;
						nrf_gpio_pin_set(LED_MOUSE_SUSPENDED);
						suspend_counter = 0;
						
						// 刷新数据变量
						delta_x = 0x00;
						delta_y = 0x00;
						button_data = 0x00;
						wheel_data = 0;
				}
				
				// 数据未发生改变
				else
				{
						// 同步按键，保持上一次历史
						last_button_data = button_data;
						
						// 递增挂起计数器
						suspend_counter++;
				}
				
				// 挂起逻辑处理
				if (suspend_counter >= suspend_time_ticks)
				{
						// 处理挂起状态更新
						mouse_suspend_status_update();	
				}
		}
}
