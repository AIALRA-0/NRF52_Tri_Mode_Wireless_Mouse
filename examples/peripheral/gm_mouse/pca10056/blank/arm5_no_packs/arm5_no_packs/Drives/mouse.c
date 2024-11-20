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
			
				// ��ʼ�㲥
				ble_device_advertising_start(false,false);
				mouse_wait_for_ble_connection();
		}
		else if(!mark_use_mode_esb && !wireless_mode_choose_ble)
		{
				NRF_LOG_INFO("ESB Mode");
				NRF_LOG_FLUSH();
				
				// ESBģ���ʼ��
				esb_device_init();
				
				// ��������LED״̬
				nrf_gpio_pin_clear(LED_MOUSE_CONNECTED); // �� ������� LED
				nrf_gpio_pin_set(LED_MOUSE_SUSPENDED); // �ر� ������ LED
				
				// ���ñ��
				mark_use_mode_usb = false;
				mark_use_mode_ble = false;
				mark_use_mode_esb = true;
			
				// ��ʼ���������
				mark_device_suspended = false;
				suspend_counter = 0;
		}
}

/**
 * @brief ���ȴ��������ӵĴ�����
 * 
 */
void mouse_wait_for_ble_connection(void)
{
    uint32_t led_toggle_interval_count = 100000;  // ��������ת���������ÿ��ѭ������ +1 ��ʾ 1ms
    uint32_t counter = 0;                      // �򵥵���ֵ������

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

        // ����������
        counter++;

        // ����Ƿ�ﵽ LED ��ת�ļ���ֵ
        if (counter >= led_toggle_interval_count)
        {
            nrf_gpio_pin_toggle(LED_MOUSE_CONNECTED); // ��ת LED
            counter = 0; // ���ü�����
        }
    }
    nrf_gpio_pin_clear(LED_MOUSE_CONNECTED);  // ��������� LED
}



/**
 * @brief ���BLE��԰����Ĵ�����
 * 
 */
void mouse_ble_pairing_handler(void)
{
		// �Ƿ��������԰�ť����
		if(!nrf_gpio_pin_read(BTN_BLE_PAIRING))
		{
			// ���sdhЭ��ջ��Ч��
			if(nrf_sdh_is_enabled())
			{
				// �ȴ�����3s
				nrf_delay_ms(3000);
				
				// ȥ��
				if(!nrf_gpio_pin_read(BTN_BLE_PAIRING))
				{
					// ȥ���󶨣����¿�ʼ�㲥
					NRF_LOG_INFO("Entering pairing mode");
					NRF_LOG_FLUSH();
					
					ble_device_advertising_start(true,false);
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
			if(ble_device_get_module_connection_handle() == BLE_CONN_HANDLE_INVALID)
			{
				NRF_LOG_INFO("Host Disconnected");
				NRF_LOG_FLUSH();
				
				// �������
				ble_device_advertising_start(false ,false);
				mouse_wait_for_ble_connection();
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
				delta_x = MIN(delta_x, 0x0fff);
        delta_y = MIN(delta_y, 0x0fff);
			
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
			
				// ���ڴ洢�������صĴ������
				ret_code_t err_code; 
			
				// ����ͨ��BLE HID��������
				err_code = ble_hids_inp_rep_send(ble_device_get_module_hid_service(),            /**< ���ͱ�׼HID���뱨�档 */
																				0, /**< ���뱨�������� */
																				6,   /**< ���뱨�泤�ȡ� */
																				databuffer,                   /**< HID�������ݡ� */
																				ble_device_get_module_connection_handle());           /**< ��ǰ���Ӿ���� */
				
				// �����ʹ���
				if ((err_code != NRF_SUCCESS) &&                 
						(err_code != NRF_ERROR_INVALID_STATE) &&
						(err_code != NRF_ERROR_RESOURCES) &&
						(err_code != NRF_ERROR_BUSY) &&
						(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING))
				{
						// ����ʧ��ʱ������һ���ش�
						NRF_LOG_INFO("BLE Sending packet failed"); 
						NRF_LOG_FLUSH();
						ble_hids_inp_rep_send(ble_device_get_module_hid_service(),            /**< ���ͱ�׼HID���뱨�档 */
																				0, /**< ���뱨�������� */
																				6,   /**< ���뱨�泤�ȡ� */
																				databuffer,                   /**< HID�������ݡ� */
																				ble_device_get_module_connection_handle());           /**< ��ǰ���Ӿ���� */
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
					nrf_gpio_pin_clear(LED_MOUSE_SUSPENDED);
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
					nrf_gpio_pin_clear(LED_MOUSE_SUSPENDED);
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
	
		// ��ʼ������״̬ 
		mouse_wheel_data_init();
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
				
				// ������԰���
				mouse_ble_pairing_handler();
				
				// �������������Ͽ�
				mouse_ble_host_disconnect_handler();
				
				// �����������
				mouse_data_update();
				
				// �������ݸı䣬���ͱ���
				if(wheel_data != 0 || button_data != last_button_data || delta_x != 0 || delta_y != 0) 
				{					
						// ˢ�°�����ʷ					
						last_button_data = button_data;
						
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
						nrf_gpio_pin_set(LED_MOUSE_SUSPENDED);
						suspend_counter = 0;
						
						// ˢ�����ݱ���
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
