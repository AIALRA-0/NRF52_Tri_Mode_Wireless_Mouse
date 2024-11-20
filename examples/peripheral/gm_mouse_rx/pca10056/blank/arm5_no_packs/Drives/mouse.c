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
void mouse_update_dpi(bool increase)
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



/***************************************
 *          �������ѯ����             *
 ***************************************/
/**
 * @brief ������лʱ�������ȡֵ��������
 * 
 */
void mouse_main_task(void)
{			
		// ���¹���״̬ 
		last_wheel_data = (nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_A) << 1) | nrf_gpio_pin_read(MOUSE_WHEEL_PHASE_B);
		
		// ����ѯѭ��
		while(true)
    {		
				// ϵͳ״̬����
				if(USE_USB)
				{
						// ����Ƿ���Ҫ�ı�USB����״̬
						if (get_mark_usbd_suspended() != get_mark_usbd_suspend_state_request())
						{
								if (get_mark_usbd_suspend_state_request()) // �������USB�������״̬
								{
										usb_device_force_suspend();
								}
								else // ��������˳�USB����״̬
								{
										usb_device_force_leaving_suspend();
								}
						}
				}
				if(USE_BLE)
				{
						ble_device_idle_state_handle();
				}
			
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
						mouse_update_dpi(true);
				}
				if (dpi_decrease_button_bit)
				{
						mouse_update_dpi(false);
				}

				
				// ���� button_data
				button_data = 0x00;  // ���ð�������
				button_data |= (left_button_bit << 0);   // �������
				button_data |= (right_button_bit << 1);  // �Ҽ�����
				button_data |= (middle_button_bit << 2); // �м�����
				button_data |= (side_button_1_bit << 3);  // ���1����
				button_data |= (side_button_2_bit << 4); // ���2����
				
				if(wheel_data != 0 || button_data != last_button_data || delta_x != 0 || delta_y != 0)
				{					
						// ͬ������
						last_button_data = button_data;
						
						// ��չ��������
						suspend_counter = 0;

						
						/* USB �����߼� */
						if(USE_USB)
						{
								set_mark_usbd_suspend_state_request(false);
								// ��� USB ���ڹ���״̬����������Զ�̻���
								if (get_mark_usbd_suspended() && get_mark_usbd_remote_wakeup_enabled())
								{
										UNUSED_RETURN_VALUE(nrf_drv_usbd_wakeup_req()); // ���� USB ����
								}
								else  // ��� USB û�й���
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
									
										if (get_mark_usbd_configured() && !get_mark_send_device_data()) // �����ǰ���ڷ����������
										{
												// �������� USB ��������ݽṹ
												static const nrf_drv_usbd_transfer_t transfer =
												{
														.p_data = {.tx = &databuffer}, // ���ô�������ԴΪ���ݻ�����
														.size = sizeof(databuffer) // �������ݴ�С
												};
								
												set_mark_send_device_data(true); // ���� ���ڷ������λ�ñ��
								
												// ʹ�ö˵�1����ʼ USB ����
												UNUSED_RETURN_VALUE(nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN1, &transfer));

										}
								}  
						}
						/* ���� �����߼� */
						else if(USE_BLE)
						{
								if (ble_device_get_module_connection_handle() != BLE_CONN_HANDLE_INVALID)    /**< ȷ���豸������ */
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
									
										ret_code_t err_code; /**< ���ڴ洢�������صĴ�����롣 */
									
										err_code = ble_hids_inp_rep_send(ble_device_get_module_hid_service(),            /**< ���ͱ�׼HID���뱨�档 */
																										0, /**< ���뱨�������� */
																										6,   /**< ���뱨�泤�ȡ� */
																										databuffer,                   /**< HID�������ݡ� */
																										ble_device_get_module_connection_handle());           /**< ��ǰ���Ӿ���� */
								
										// ������
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
						
						/* 2.4G �����߼� */
						else if(USE_ESB)
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
							
								NRF_LOG_INFO("Transmitting packet");
								for (int i = 0; i < tx_payload.length; i++) {
										NRF_LOG_INFO("Byte %d: 0x%02x", i, tx_payload.data[i]);
								}
								NRF_LOG_FLUSH();

								tx_payload.noack = false; // ����Ϊfalse�������Զ�Ӧ��ACK����
				
								// ���Է����غ�����
								if (nrf_esb_write_payload(&tx_payload) != NRF_SUCCESS) // �������ʧ�ܣ���¼��־������Ϣ
								{
										NRF_LOG_INFO("Sending packet failed"); 
								}
								
								NRF_LOG_FLUSH();
						}
						
						// �ָ�Ĭ��ֵ
						suspend_counter = 0;
						delta_x = 0x00;
						delta_y = 0x00;
						button_data = 0x00;
						wheel_data = 0;
				}
				else
				{
						// ͬ������
						last_button_data = button_data;
						
						// �������������
						suspend_counter++;
				}
				
				// �����߼�����
				if (suspend_counter >= suspend_time_ticks)
				{
						/* USB �����߼� */
						if(USE_USB)
						{
								if (!get_mark_usbd_suspended())
								{
										set_mark_usbd_suspend_state_request(true);
										usb_device_force_suspend();
								}
						}
						
						/* ���� �����߼� */
						else if(USE_BLE)
						{
								NRF_LOG_INFO("BLE_SUSPENDED");
								while (NRF_LOG_PROCESS() != false);    /**< ����Ƿ���δ�������־��������� */
								nrf_pwr_mgmt_run();            /**< ����͹���ģʽ���ȴ���һ�¼������豸�� */
						}
						
						
						
						/* 2.4G �����߼� */
						else if(USE_ESB)
						{
					
						}
						
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
}

/**
 * @brief ��ʼ������
 * 
 * ����ϵͳ�ر�����USB�����뻽�ѡ�������ݷ��͵��߼���
 */
void mouse_init(void)
{	
		// ��ʼ��GPIO
		mouse_gpio_init();

} 

