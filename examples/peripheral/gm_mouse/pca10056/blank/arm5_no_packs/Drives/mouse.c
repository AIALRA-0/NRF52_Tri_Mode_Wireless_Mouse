/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include "mouse.h"   // ���ͷ�ļ�

/***************************************
 *             ������            		 *
 ***************************************/
// DPIԤ��
static const uint16_t dpi_presets[NUM_MOUSE_DPI_PRESET][2] = 
{
    {800, 800},   // DPIԤ��1
    {1200, 1200}, // DPIԤ��2
    {1600, 1600}, // DPIԤ��3
    {2400, 2400}, // DPIԤ��4
    {3200, 3200}  // DPIԤ��5
};

// ��ǰDPI����
static uint8_t current_dpi_index = 1; // Ĭ��ʹ�õڶ���DPI

// �������
static uint8_t button_data = 0x00;	// �洢��������
static int8_t wheel_data = 0x00;	// �洢��������
static uint8_t databuffer[6];  // ���ڴ洢���͵��������
static nrf_esb_payload_t tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // ���岢��ʼ��һ��ESB���䣨TX�����أ�������ʼ���ݣ�8�ֽڣ�

// �������˶�λ
static bool motion_state = 0;

// ����ȥ������
static uint8_t left_button_count = 0;
static uint8_t right_button_count = 0;
static uint8_t middle_button_count = 0;
static uint8_t side_button_1_count = 0;
static uint8_t side_button_2_count = 0;
static uint8_t dpi_increase_button_count = 0;
static uint8_t dpi_decrease_button_count = 0;

// ������ǰλ
static bool left_button_state = 0;
static bool right_button_state = 0;
static bool middle_button_state = 0;
static bool side_button_1_state = 0;
static bool side_button_2_state = 0;
static bool dpi_increase_button_state = 0;
static bool dpi_decrease_button_state = 0;

// ����ָʾλ
static uint8_t left_button_bit = 0;
static uint8_t right_button_bit = 0;
static uint8_t middle_button_bit = 0;
static uint8_t side_button_1_bit = 0;
static uint8_t side_button_2_bit = 0;
static uint8_t dpi_increase_button_bit = 0;
static uint8_t dpi_decrease_button_bit = 0;

// ������ʷ
static uint8_t last_button_data = 0x00;

// ���ֵ�A���B��״̬
static uint8_t last_wheel_data = 0x00;
static uint8_t current_wheel_data = 0x00;

// ������ƫ��
static int16_t delta_x = 0x00;
static int16_t delta_y = 0x00;

// ���������
uint32_t suspend_counter = 0;
uint32_t suspend_time_ticks = SUSPEND_TIME * configTICK_RATE_HZ * 20; // ** ��׼�����Ľ�
bool mark_device_suspended = false;

// SAADC
APP_TIMER_DEF(module_battery_timer);								/**< @brief ���岢����һ����ʱ��ʵ��*/
static nrf_saadc_value_t sample_buffer; /**< �������������ڴ洢���β���ֵ�� */
static int8_t sample_filter_buffer[FILTER_WINDOW_SIZE] = {0}; // �������ڻ�����
static uint8_t sample_filter_index = 0; // ��ǰ����������
static int16_t sample_filter_sum = 0;   // ���������ڲ���ֵ���ܺ�
static int8_t filtered_value = 0; // �˲���Ľ��

// ��ز���
static float battery_v_ref = 0.6f;              // �ο���ѹ����λ��V��
static int adc_resolution = 1024;       // ADC�ֱ��ʣ�10λ��2^10 = 1024��
static float	adc_gain = 1.0f / 6.0f;        // ���棨1/6�����뷶Χ0 ~ 3.6V��
static float battery_v_min = 3.0f;      // �����͹�����ѹ����λ��V��
static float battery_v_max = 3.6f;      // ��������ѹ����λ��V��
static battery_state_t last_battery_state = BATTERY_HIGH;  // ��ʼ��Ϊ�͵���״̬


/***************************************
 *           ���ģʽ����            	 *
 ***************************************/
 static bool mark_use_mode_usb = false;
 static bool mark_use_mode_ble = false;
 static bool mark_use_mode_esb = false;
 static bool wireless_mode_choose_ble = true;

/***************************************
 *           ��긨������               *
 ***************************************/
 /**
 * @brief ͨ�ð���ȥ����������
 *
 * @param[in] button_state ��ǰ����״̬��1 ��ʾ�ɿ���0 ��ʾ���£�
 * @param[in,out] button_count �������¼�����
 * @param[in,out] button_bit ����λ����ʾ�����Ƿ��ѱ�ȷ�ϰ���
 */
void mouse_button_debounce(bool button_state, uint8_t *button_count, uint8_t *button_bit)
{
    if (!button_state) // ��⵽��������
    {
        if (*button_count < DEBOUNCE_COUNT)
        {
            (*button_count)++;
        }
        if (*button_count >= DEBOUNCE_COUNT)
        {
            *button_bit = 1; // ȷ�ϰ���״̬
        }
    }
    else
    {
        *button_bit = 0;     // ����Ϊ�ɿ�״̬
        *button_count = 0;   // ���ü�����
    }
}

/**
 * @brief DPI���´���
 *
 * @param[in] increase �Ƿ�����DPI��true��ʾ���ӣ�false��ʾ���٣�
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
        while (!nrf_gpio_pin_read(MOUSE_BTN_DPI_INCREASE)); // �ȴ������ɿ�
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
        while (!nrf_gpio_pin_read(MOUSE_BTN_DPI_DECREASE)); // �ȴ������ɿ�
    }

    // ����X��Y��DPI�ֱ���
    paw3399_set_xy_resolution(dpi_presets[current_dpi_index][0], dpi_presets[current_dpi_index][1]);
    NRF_LOG_INFO("DPI Updated, X: %d, Y: %d", paw3399_get_var_x_resolution(), paw3399_get_var_y_resolution());
    NRF_LOG_FLUSH();
}

/**
 * @brief �����������״̬������λ
 * 
 */
void mouse_connection_mode_update(void)
{
		// �������״̬�ı��򴥷���λ
		if(mark_use_mode_usb && nrf_drv_power_usbstatus_get() != NRF_DRV_POWER_USB_STATE_READY) // usbģʽ��usb�Ͽ�
		{
			WS2812B_clearLight();
			NVIC_SystemReset(); // ������λ
		}
		else if((mark_use_mode_ble || mark_use_mode_esb) && nrf_drv_power_usbstatus_get() == NRF_DRV_POWER_USB_STATE_READY) // esb/bleģʽ��usb����
		{
			NVIC_SystemReset(); // ������λ
		}
		else if (mark_use_mode_ble && !wireless_mode_choose_ble) // bleģʽ��esb����
		{
			NVIC_SystemReset(); // ������λ
		}
		else if(mark_use_mode_esb && wireless_mode_choose_ble) // esbģʽ��ble����
		{
			NVIC_SystemReset(); // ������λ
		}
		
		// ���ݱ�����ù���ģʽ
		if(nrf_drv_power_usbstatus_get() == NRF_DRV_POWER_USB_STATE_READY) // usb����
		{
				if(!mark_use_mode_usb) // ��usbģʽ������usb
				{
					NRF_LOG_INFO("USB Mode");
					NRF_LOG_FLUSH();
					
					// ���ñ��
					mark_use_mode_usb = true;
					mark_use_mode_ble = false;
					mark_use_mode_esb = false;
				
					// ��ʼ���������
					mark_device_suspended = false;
					suspend_counter = 0;
					
					// �������ģʽ
					paw3399_high_performance_mode();
					
					// �������ӵ�
					WS2812B_setConnectedLight();
					
					// �򿪵�ظ��¼�ʱ��
					nrf_gpio_pin_clear(MOUSE_ADC_CONTROL_PIN); // ������ز��� PMOS
					mouse_battery_update_timer_start();
					
				}
		}
		else if(!mark_use_mode_ble && wireless_mode_choose_ble)
		{	
				NRF_LOG_INFO("BLE Mode");
				NRF_LOG_FLUSH();
				
				// ����ģ���ʼ��
				ble_device_init();
			
				// ���ñ��
				mark_use_mode_usb = false;
				mark_use_mode_ble = true;
				mark_use_mode_esb = false;
			
				// ��ʼ���������
				mark_device_suspended = false;
				suspend_counter = 0;
			
				// �������ģʽ
				// paw3399_high_performance_mode();
				paw3399_office_mode();
			
				// ��ʼ�㲥
				ble_device_advertising_start(false,false);
				mouse_wait_for_ble_connection();
				
				// �򿪵�ظ��¼�ʱ��
				nrf_gpio_pin_clear(MOUSE_ADC_CONTROL_PIN); // ������ز��� PMOS
				mouse_battery_update_timer_start();
		}
		else if(!mark_use_mode_esb && !wireless_mode_choose_ble)
		{
				NRF_LOG_INFO("ESB Mode");
				NRF_LOG_FLUSH();
				
				// ESBģ���ʼ��
				esb_device_init();
				
				// ��������LED״̬
				WS2812B_setConnectedLight();
				
				// ���ñ��
				mark_use_mode_usb = false;
				mark_use_mode_ble = false;
				mark_use_mode_esb = true;
			
				// ��ʼ���������
				mark_device_suspended = false;
				suspend_counter = 0;
			
				// �������ģʽ
				paw3399_high_performance_mode();
			
				// �򿪵�ظ��¼�ʱ��
				nrf_gpio_pin_clear(MOUSE_ADC_CONTROL_PIN); // ������ز��� PMOS
				mouse_battery_update_timer_start();
		}
}

/**
 * @brief ���ȴ��������ӵĴ�����
 * 
 */
void mouse_wait_for_ble_connection(void)
{		
		NRF_LOG_INFO("Advertising, Wait for pairing");
		NRF_LOG_FLUSH();
    while (ble_device_get_module_connection_handle() == BLE_CONN_HANDLE_INVALID) // ����Ƿ�δ����
    {
        // �Ƿ����������״̬����
        if (!nrf_gpio_pin_read(MOUSE_MODE) || nrf_drv_power_usbstatus_get() == NRF_DRV_POWER_USB_STATE_READY)
        {
            NRF_LOG_WARNING("Mouse mode or USB ready detected. Triggering reset.");
            NVIC_SystemReset(); // ������λ
        }

        // ������԰���
        mouse_ble_pairing_handler();

        // ִ�е������еĴ���������
        ble_device_idle_state_handle();
    }
		
		ble_device_led_timer_stop();
    WS2812B_setConnectedLight();  // ��������� LED
}



/**
 * @brief ���BLE��԰����Ĵ�����
 * 
 */
void mouse_ble_pairing_handler(void)
{
    // ����Ƿ��������԰�ť����
    if (!nrf_gpio_pin_read(BTN_BLE_PAIRING))
    {
        // ���Э��ջ�Ƿ�������
        if (nrf_sdh_is_enabled())
        {
            // ��ʼ��������
            uint32_t counter = 0;
            uint32_t max_count = 3000; // �ȴ�ʱ�䣬��λΪ����
            
            while (counter < max_count)
            {
                // ÿ��ѭ���ȴ� 1 ms
                nrf_delay_ms(1);
								
								// ִ�е������еĴ���������
								ble_device_idle_state_handle();

                // ����������
                counter++;
            }
						
						WS2812B_togglePairingLight();  // ��������� LED
						
						// ȥ��
						if (!nrf_gpio_pin_read(BTN_BLE_PAIRING))
						{					
						
								// ������
								ble_device_set_mark_device_disconnected(false);
										
								// ������Ȼ�����£��������ģʽ
								NRF_LOG_INFO("Entering pairing mode");
								NRF_LOG_FLUSH();
		
								// ȥ���󶨲����¿�ʼ�㲥
								ble_device_advertising_start(true, true);
								mouse_wait_for_ble_connection();
						}
        }
    }
}


/**
 * @brief ���BLE��԰����Ĵ�����
 * 
 */
void mouse_ble_host_disconnect_handler(void)
{
		// �Ƿ�Ϊ����ģʽ
		if(mark_use_mode_ble)
		{
			// ���豸�Ͽ�����
			if(ble_device_get_mark_device_disconnected() == true)
			{						
				// ֹͣ�㲥
				ble_device_stop_advertising();
				
				NRF_LOG_INFO("Host Disconnected");
				NRF_LOG_FLUSH();
				
				ble_device_set_mark_need_host_delete(false);
				
				if(ble_device_get_mark_need_host_delete())
				{
					WS2812B_setDisconnectedLight();
				}
				else
				{
					WS2812B_setHostDeleteLight();
				}
				
				// �ȴ�������Ի��߸ı�״̬
				while(nrf_gpio_pin_read(BTN_BLE_PAIRING) && nrf_gpio_pin_read(MOUSE_MODE) && (nrf_drv_power_usbstatus_get() != NRF_DRV_POWER_USB_STATE_READY))
				{
						// ִ�е������еĴ���������
						ble_device_idle_state_handle();
				}
				
				// �ָ����
				ble_device_set_mark_device_disconnected(false);

			}
		}
}

/**
 * @brief ���ϵͳ״̬���µĺ���
 * 
 */
void mouse_system_status_update(void)
{
		// USBϵͳ״̬����
		if(mark_use_mode_usb)
		{
				// ����Ƿ���Ҫ�ı�USB����״̬
				if (usb_device_get_mark_usbd_suspended() != usb_device_get_mark_usbd_suspend_state_request())
				{
						// �������USB�������״̬
						if (usb_device_get_mark_usbd_suspend_state_request()) 
						{
								usb_device_force_suspend();
						}
						// ��������˳�USB����״̬
						else 
						{
								usb_device_force_leaving_suspend();
						}
				}
		}
		
		// BLEϵͳ״̬����
		if(mark_use_mode_ble)
		{
				ble_device_idle_state_handle();
		}
		
		// ESBϵͳ״̬����
		if(mark_use_mode_esb)
		{
				
		}
}

/**
 * @brief �������״̬���µĺ���
 * 
 */
void mouse_data_update(void)
{
		// ��ȡ�������� delta_x �� delta_y ֵ
		motion_state = nrf_gpio_pin_read(MOTION_PIN);
		if(!motion_state)
		{
				paw3399_get_delta_xy();
				delta_x = paw3399_get_var_delta_x();
				delta_y = paw3399_get_var_delta_y();
		}
		
		// ��A���B����ϴ�״̬�͵�ǰ״̬��ϳ�һ��״̬����
		current_wheel_data = (nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_A) << 1) | nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_B);  // ��ǰ״̬
		
		// ������״̬��״̬��ת����
		// ˳ʱ�� (CW) ��תʱ��״̬��˳��Ӧ���ǣ�00 -> 01 -> 11 -> 10 -> 00
		// ��ʱ�� (CCW) ��תʱ��״̬��˳��Ӧ���ǣ�00 -> 10 -> 11 -> 01 -> 00
		
		// ˳ʱ���ж�
		if ((last_wheel_data == 0 && current_wheel_data == 1) ||
				(last_wheel_data == 1 && current_wheel_data == 3) ||
				(last_wheel_data == 3 && current_wheel_data == 2) ||
				(last_wheel_data == 2 && current_wheel_data == 0))
		{
				wheel_data = -1;  // ˳ʱ����ת
		}
		// ��ʱ���ж�
		else if ((last_wheel_data == 0 && current_wheel_data == 2) ||
						(last_wheel_data == 2 && current_wheel_data == 3) ||
						(last_wheel_data == 3 && current_wheel_data == 1) ||
						(last_wheel_data == 1 && current_wheel_data == 0))
		{
				wheel_data = 1;  // ��ʱ����ת
		}
		else
		{
				wheel_data = 0;  // û�б仯
		}
		
		// �����ϴ�A���B���״̬
		last_wheel_data = current_wheel_data;
		
		// ��ȡ��ǰ����״̬
		left_button_state = nrf_gpio_pin_read(MOUSE_BTN_LEFT);
		right_button_state = nrf_gpio_pin_read(MOUSE_BTN_RIGHT);
    middle_button_state = nrf_gpio_pin_read(MOUSE_BTN_MIDDLE);
		side_button_1_state = nrf_gpio_pin_read(MOUSE_BTN_SIDE_1);
    side_button_2_state = nrf_gpio_pin_read(MOUSE_BTN_SIDE_2);
		dpi_increase_button_state = nrf_gpio_pin_read(MOUSE_BTN_DPI_INCREASE);
    dpi_decrease_button_state = nrf_gpio_pin_read(MOUSE_BTN_DPI_DECREASE);
		
    // ���ȥ��������
    mouse_button_debounce(left_button_state, &left_button_count, &left_button_bit);
		
    // �Ҽ�ȥ��������
    mouse_button_debounce(right_button_state, &right_button_count, &right_button_bit);
		
    // �м�ȥ��������
    mouse_button_debounce(middle_button_state, &middle_button_count, &middle_button_bit);
		
		// ���1ȥ��������
		mouse_button_debounce(side_button_1_state, &side_button_1_count, &side_button_1_bit);
		
		// ���2ȥ��������
		mouse_button_debounce(side_button_2_state, &side_button_2_count, &side_button_2_bit);
		
		// DPI+��ȥ��������
		mouse_button_debounce(dpi_increase_button_state, &dpi_increase_button_count, &dpi_increase_button_bit);
		
		// DPI-��ȥ��������
		mouse_button_debounce(dpi_decrease_button_state, &dpi_decrease_button_count, &dpi_decrease_button_bit);
		
		// DPI ����
		if (dpi_increase_button_bit)
		{
				mouse_dpi_update(true);
		}
		if (dpi_decrease_button_bit)
		{
				mouse_dpi_update(false);
		}

		
		// ���� button_data
		button_data = 0x00;  // ���ð�������
		button_data |= (left_button_bit << 0);   // �������
		button_data |= (right_button_bit << 1);  // �Ҽ�����
		button_data |= (middle_button_bit << 2); // �м�����
		button_data |= (side_button_1_bit << 3);  // ���1����
		button_data |= (side_button_2_bit << 4); // ���2����
}

/**
 * @brief ͨ��usb�����������
 * 
 */
void mouse_usb_data_send(void)
{
		// ����usb������
		usb_device_set_mark_usbd_suspend_state_request(false);
		
		// ��� USB ���ڹ���״̬����������Զ�̻���
		if (usb_device_get_mark_usbd_suspended() && usb_device_get_mark_usbd_remote_wakeup_enabled())
		{
				// ���� USB ����
				UNUSED_RETURN_VALUE(nrf_drv_usbd_wakeup_req()); 
		}
		
		// ��� USB û�й���
		else  
		{	
				// ����״̬��1�ֽڣ�
				databuffer[0] = button_data;
		
				// X ��仯ֵ��2�ֽڣ�16λ��
				databuffer[1] = (uint8_t)delta_x;       // ���ֽ�
				databuffer[2] = (uint8_t)(delta_x >> 8); // ���ֽ�
				
				// Y ��仯ֵ��2�ֽڣ�16λ��
				databuffer[3] = (uint8_t)delta_y;       // ���ֽ�
				databuffer[4] = (uint8_t)(delta_y >> 8); // ���ֽ�
				
				// ���ֱ仯ֵ��1�ֽڣ�8λ��
				databuffer[5] = (uint8_t)wheel_data;
				
				// �����ǰ���ڷ����������
				if (usb_device_get_mark_usbd_configured() && !usb_device_get_mark_send_device_data())
				{
						// �������� USB ��������ݽṹ
						static const nrf_drv_usbd_transfer_t transfer =
						{
								// ���ô�������ԴΪ���ݻ�����
								.p_data = {.tx = &databuffer},
								
								// �������ݴ�С
								.size = sizeof(databuffer) 
						};
		
						usb_device_set_mark_send_device_data(true); // �������ڷ������λ�ñ��
		
						// ʹ�ö˵�1����ʼUSB����
						if(nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN1, &transfer) != NRF_SUCCESS)
						{
								// ����ʧ��ʱ������һ���ش�
								NRF_LOG_INFO("USB Sending packet failed"); 
								NRF_LOG_FLUSH();
								nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN1, &transfer); 
						}

				}
		}  
}

/**
 * @brief ͨ��ble�����������
 * 
 */
void mouse_ble_data_send(void)
{
		// ȷ���豸������
		if (ble_device_get_module_connection_handle() != BLE_CONN_HANDLE_INVALID)    
		{						
				// ���ڴ洢�������صĴ������
				ret_code_t err_code; 
				
				NRF_LOG_INFO("why??????");
				NRF_LOG_FLUSH();
			
				// X ��仯ֵ��2�ֽڣ�16λ��
				databuffer[0] = (uint8_t)delta_x;       // ���ֽ�
				databuffer[1] = (uint8_t)(delta_x >> 8); // ���ֽ�
				
				// Y ��仯ֵ��2�ֽڣ�16λ��
				databuffer[2] = (uint8_t)delta_y;       // ���ֽ�
				databuffer[3] = (uint8_t)(delta_y >> 8); // ���ֽ�
				
				err_code = ble_hids_inp_rep_send(ble_device_get_module_hid_service(),            /**< ���ͱ�׼HID���뱨�档 */
																				1, /**< ���뱨�������� */
																				4,   /**< ���뱨�泤�ȡ� */
																				databuffer,                   /**< HID�������ݡ� */
																				ble_device_get_module_connection_handle());           /**< ��ǰ���Ӿ���� */
				
				// �����ʹ���
				if ((err_code != NRF_SUCCESS) &&                 
						(err_code != NRF_ERROR_INVALID_STATE) &&
						(err_code != NRF_ERROR_RESOURCES) &&
						(err_code != NRF_ERROR_BUSY) &&
						(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING))
				{
						APP_ERROR_HANDLER(err_code); /**< �����Ԥ�ڴ��� */
				}
				
				// ����&�����仯ֵ��2�ֽڣ�16λ��
				databuffer[0] = button_data;  //��������
				databuffer[1] = (uint8_t)wheel_data; // ��������
				databuffer[2] = 0x00; 
				
				// ����ͨ��BLE HID��������
				err_code = ble_hids_inp_rep_send(ble_device_get_module_hid_service(),            /**< ���ͱ�׼HID���뱨�档 */
																				0, /**< ���뱨�������� */
																				3,   /**< ���뱨�泤�ȡ� */
																				databuffer,                   /**< HID�������ݡ� */
																				ble_device_get_module_connection_handle());           /**< ��ǰ���Ӿ���� */
				
				// �����ʹ���
				if ((err_code != NRF_SUCCESS) &&                 
						(err_code != NRF_ERROR_INVALID_STATE) &&
						(err_code != NRF_ERROR_RESOURCES) &&
						(err_code != NRF_ERROR_BUSY) &&
						(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING))
				{
						APP_ERROR_HANDLER(err_code); /**< �����Ԥ�ڴ��� */
				}
				
			
		}
}

/**
 * @brief ͨ��esb�����������
 * 
 */
void mouse_esb_data_send(void)
{
		// ����״̬��1�ֽڣ�
		tx_payload.data[0] = button_data;
		
		// X ��仯ֵ��2�ֽڣ�16λ��
		tx_payload.data[1] = (uint8_t)delta_x;       // ���ֽ�
		tx_payload.data[2] = (uint8_t)(delta_x >> 8); // ���ֽ�
		
		// Y ��仯ֵ��2�ֽڣ�16λ��
		tx_payload.data[3] = (uint8_t)delta_y;       // ���ֽ�
		tx_payload.data[4] = (uint8_t)(delta_y >> 8); // ���ֽ�
		
		// ���ֱ仯ֵ��1�ֽڣ�8λ��
		tx_payload.data[5] = (uint8_t)wheel_data;			

		 // ����Ϊfalse�������Զ�Ӧ��ACK����
		tx_payload.noack = false;
		
		// ���Է����غ�����
		if (nrf_esb_write_payload(&tx_payload) != NRF_SUCCESS) 
		{
				// �������ʧ�ܣ�����һ���ش�
				NRF_LOG_INFO("ESB Sending packet failed"); 
				NRF_LOG_FLUSH();
				nrf_esb_write_payload(&tx_payload); 
		}
}

/**
 * @brief ����������״̬
 * 
 */
void mouse_suspend_status_update(void)
{
		// USB �����߼� 
		if(mark_use_mode_usb)
		{
				// ����Ƿ��Ѿ�����
				if (!usb_device_get_mark_usbd_suspended())
				{
						// ����Ƿ���δ�������־�������
						while (NRF_LOG_PROCESS() != false);    
					
						// ǿ��USB�������
						usb_device_set_mark_usbd_suspend_state_request(true);
						usb_device_force_suspend();
				}
		}
		
		// BLE �����߼�
		else if(mark_use_mode_ble)
		{
				// ����Ƿ��Ѿ�����
				if(!mark_device_suspended)
				{
					//���¹������״̬
					NRF_LOG_INFO("BLE_SUSPEND");
					mark_device_suspended = true;
					WS2812B_setSuspendLight();
				};
				// ����Ƿ���δ�������־�������
				if (NRF_LOG_PROCESS() != false)    
				{
					// ����͹���ģʽ���ȴ���һ�¼������豸
					nrf_pwr_mgmt_run();           
				}
		}
		
		// ESB �����߼�
		else if(mark_use_mode_esb)
		{
				// ����Ƿ��Ѿ�����
				if(!mark_device_suspended)
				{
					//���¹������״̬
					NRF_LOG_INFO("ESB_SUSPEND");
					mark_device_suspended = true;
					WS2812B_setSuspendLight();
				}
				// ����Ƿ���δ�������־�������
				if (NRF_LOG_PROCESS() != false)    
				{
					// ����ESB�豸
					nrf_esb_suspend();
					
					// ����͹���ģʽ���ȴ���һ�¼������豸
					nrf_pwr_mgmt_run();           
				}
		}
}

 /****************************************
 *           ����ʼ������  			    	*
 ****************************************/
/**
 * @brief ���GPIO��ʼ������
 * 
 */
void mouse_gpio_init(void)
{
		nrf_gpio_cfg_input(MOUSE_BTN_LEFT, NRF_GPIO_PIN_PULLUP); // �������
		nrf_gpio_cfg_input(MOUSE_BTN_MIDDLE, NRF_GPIO_PIN_PULLUP); // �����м�
		nrf_gpio_cfg_input(MOUSE_BTN_RIGHT, NRF_GPIO_PIN_PULLUP); // �������
		nrf_gpio_cfg_input(MOUSE_BTN_SIDE_1, NRF_GPIO_PIN_PULLUP); // ���ò��1
		nrf_gpio_cfg_input(MOUSE_BTN_SIDE_2, NRF_GPIO_PIN_PULLUP); // ���ò��2
		nrf_gpio_cfg_input(MOUSE_BTN_DPI_INCREASE, NRF_GPIO_PIN_PULLUP); // ����DPI+��
		nrf_gpio_cfg_input(MOUSE_BTN_DPI_DECREASE, NRF_GPIO_PIN_PULLUP); // ����DPI-��
		nrf_gpio_cfg_input(MOUSE_WHEEL_PHASE_A, NRF_GPIO_PIN_PULLUP); // ���ù���A��
		nrf_gpio_cfg_input(MOUSE_WHEEL_PHASE_B, NRF_GPIO_PIN_PULLUP); // ���ù���B��
		
		nrf_gpio_cfg_input(MOUSE_MODE, NRF_GPIO_PIN_PULLUP); // ����ģʽѡ��GPIOΪ����ģʽ��ʹ����������
		nrf_gpio_cfg_input(BTN_BLE_PAIRING, NRF_GPIO_PIN_PULLUP); // ����ģʽѡ��GPIOΪ����ģʽ��ʹ����������
	
		nrf_gpio_cfg_output(MOUSE_ADC_CONTROL_PIN); // ����ģʽѡ��GPIOΪ���
	
		nrf_gpio_pin_set(MOUSE_ADC_CONTROL_PIN); // Ĭ������
}

/**
 * @brief ���������ݳ�ʼ������
 * 
 * ��ʼ������������
 */
void mouse_wheel_data_init(void)
{	
		// ��ʼ������״̬ 
		last_wheel_data = (nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_A) << 1) | nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_B);
}

/**
 * @brief ����ʼ������
 * 
 * ��ʼ�����ģ�顣
 */
void mouse_init(void)
{	
		// ��ʼ��GPIO
		mouse_gpio_init();
		
		// ��ʼ��������
		mouse_scheduler_init();
	
		// ��ʼ�� SAADC
		mouse_saadc_init();
		
		// ��ʼ������״̬ 
		mouse_wheel_data_init();
} 

/**@brief ��ʼ���¼��������ĺ�����
 *
 * @details �ú�����ʼ���¼������������ڹ���͵���ϵͳ�е��첽�¼���
 *          �����������¼�����ѭ����˳�����������ж���ֱ��ִ�нϳ��Ĵ��롣
 */
void mouse_scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE); /**< ��ʼ���������������¼����ݵ�����С�Ͷ��д�С�� */
}


/***************************************
 *          ��ص�������             *
 ***************************************/
/**
 * @brief ��ʼ��SAADCģ�顣
 */
void mouse_saadc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(MOUSE_ADC_PIN);  // ���õ���SAADCͨ��

    err_code = nrf_drv_saadc_init(NULL, mouse_saadc_callback);  // ��ʼ��SAADC
    APP_ERROR_CHECK(err_code);  // ������

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);  // ��ʼ��ͨ��0
    APP_ERROR_CHECK(err_code);  // ������

    // ���õ������������ڴ洢һ������ֵ
    err_code = nrf_drv_saadc_buffer_convert(&sample_buffer, 1);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief SAADC�ص��������������˲���
 *
 * @param[in] p_event SAADC�¼��ṹָ�롣
 */
void mouse_saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)  // ����¼�����
    {
        ret_code_t err_code;

        // ת����������׼����һ�β���
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, 1);
        APP_ERROR_CHECK(err_code);

        // ��ȡ�²���ֵ
        uint8_t new_sample = saadc_value_to_battery_percent(p_event->data.done.p_buffer[0]);

        // �����˲�����
        sample_filter_sum -= sample_filter_buffer[sample_filter_index];       // ���ܺ����Ƴ����ֵ
        sample_filter_buffer[sample_filter_index] = new_sample;        // ����ֵ���뻺����
        sample_filter_sum += new_sample;                        // ����ֵ�����ܺ�

        // ��������
        sample_filter_index = (sample_filter_index + 1) % FILTER_WINDOW_SIZE;

        // ���㻬��ƽ��ֵ
        filtered_value = sample_filter_sum / FILTER_WINDOW_SIZE;
			
				// �жϵ�ǰ���״̬
        battery_state_t current_battery_state;
        if (filtered_value >= BATTERY_HIGH_LIM)
        {
            current_battery_state = BATTERY_HIGH;
        }
        else if (filtered_value >= BATTERY_LOW_LIM)
        {
            current_battery_state = BATTERY_MIDDLE;
        }
        else
        {
            current_battery_state = BATTERY_LOW;
        }	
				
				// ������״̬�����仯������µƹ�
        if (current_battery_state != last_battery_state)
        {
            switch (current_battery_state)
            {
                case BATTERY_HIGH:
                    WS2812B_setBatteryHighLight();
                    break;
                case BATTERY_MIDDLE:
                    WS2812B_setBatteryMiddleLight();
                    break;
                case BATTERY_LOW:
                    WS2812B_setBatteryLowLight();
                    break;
            }
        }
				
				last_battery_state = current_battery_state;  // ������һ��״̬

        // ���ʹ�� BLE ģʽ�����µ�ص����ٷֱ�
        if (mark_use_mode_ble)
        {
            ble_device_battery_level_update(filtered_value);
        }
    }
}

/**
 * @brief ��SAADC����ֵת��Ϊ��ص����ٷֱȡ�
 *
 * @param[in] adc_value SAADC����ֵ��
 * @return    ��ص����ٷֱȣ�0-100����
 */
uint8_t saadc_value_to_battery_percent(int16_t adc_value)
{
    // ������ֵת��Ϊ�����ѹ
    float v_input = ((float)adc_value * battery_v_ref) / ((adc_resolution - 1) * adc_gain);

    // �������ѹӳ��Ϊ��ص����ٷֱ�
    if (v_input <= battery_v_min)
    {
        return 0;  // �����ľ�
    }
    else if (v_input >= battery_v_max)
    {
        return 100;  // ����
    }
    else
    {
        return (uint8_t)((v_input - battery_v_min) / (battery_v_max - battery_v_min) * 100.0f);
    }
}


/**
 * @brief ��ʱ���ص����������ڴ���SAADC������
 *
 * @param[in] p_context �����Ĳ�����������δʹ�ã���
 */
void mouse_battery_update_timer_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);

    // �ֶ�����SAADC����
    nrf_drv_saadc_sample();
}

/**
 * @brief �������ֵ���¶�ʱ����ÿ2000 ticks����һ�ε��ֵ��
 */
void mouse_battery_update_timer_start(void)
{
    ret_code_t err_code;

    // ������ʱ�����ظ�ģʽ��
    err_code = app_timer_create(&module_battery_timer, APP_TIMER_MODE_REPEATED, mouse_battery_update_timer_handler);
    APP_ERROR_CHECK(err_code);

    // ������ʱ����ÿ 2000 ticks ����һ��
    err_code = app_timer_start(module_battery_timer, APP_TIMER_TICKS(2000), NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Battery update timer started, interval: 2000 ticks");
    NRF_LOG_FLUSH();
}

/***************************************
 *          �������ѯ����             *
 ***************************************/
/**
 * @brief ������лʱ�������ȡֵ��������
 * 
 */
void mouse_main_task(void)
{			
		NRF_LOG_INFO("Start Mouse");
		NRF_LOG_FLUSH();
	
		// ����ѯѭ��
		while(true)
    {		
				// ���������
				app_sched_execute();
			
				// ����ϵͳ״̬
				mouse_system_status_update();
			
				// ��������ģʽѡ��ť״̬
				wireless_mode_choose_ble = nrf_gpio_pin_read(MOUSE_MODE);
				
				// �����������ģʽ
				mouse_connection_mode_update();
				
				// ������ģʽ״̬
				if(!mark_use_mode_usb && !mark_use_mode_esb && !mark_use_mode_ble)
				{
						NRF_LOG_INFO("No Operation Mode");
						NRF_LOG_FLUSH();
						continue;
				}
				
				if(mark_use_mode_ble)
				{
						// ������԰���
						mouse_ble_pairing_handler();
						
						// �������������Ͽ�
						mouse_ble_host_disconnect_handler();
				}
				
				// �����������
				mouse_data_update();
				
				// �������ݸı䣬���ͱ���
				if(wheel_data != 0 || button_data != last_button_data || delta_x != 0 || delta_y != 0) 
				{											
						/* USB �����߼� */
						if(mark_use_mode_usb)
						{
								mouse_usb_data_send();
						}
						/* BLE �����߼� */
						else if(mark_use_mode_ble)
						{
								mouse_ble_data_send();
						}
						
						/* ESB �����߼� */
						else if(mark_use_mode_esb)
						{
								mouse_esb_data_send();		
						}
						
						// ˢ�¹������
						mark_device_suspended = false;
						WS2812B_setConnectedLight();
						suspend_counter = 0;
						
						// ˢ�����ݱ���
						last_button_data = button_data;
						delta_x = 0x00;
						delta_y = 0x00;
						button_data = 0x00;
						wheel_data = 0;
				}
				
				// ����δ�����ı�
				else
				{
						// ͬ��������������һ����ʷ
						last_button_data = button_data;
						
						// �������������
						suspend_counter++;
				}
				
				// �����߼�����
				if (suspend_counter >= suspend_time_ticks)
				{
						// �������״̬����
						mouse_suspend_status_update();	
				}
		}
}
