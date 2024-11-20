/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include "usb_device.h"

/***************************************
 *         USBD��̬ͨ�ų�������         *
 ***************************************/
/**
 * @brief �豸����������
 * @details �洢�豸�Ļ�����Ϣ����������������ʶ���豸����
 */
static const uint8_t get_descriptor_device[] = {
    USBD_DEVICE_DESCRIPTOR
};

/**
 * @brief ��������������
 * @details �������á��ӿںͶ˵�������������������豸�Ĳ�ͬ�ӿں�����ѡ��
 */
static const uint8_t get_descriptor_configuration[] = {
    USBD_CONFIG_DESCRIPTOR,        /**< ���������� */
    USBD_INTERFACE0_DESCRIPTOR,    /**< �ӿ������� */
    USBD_HID0_DESCRIPTOR,          /**< HID������ */
    USBD_ENDPOINT1_DESCRIPTOR      /**< �˵������� */
};

/**
 * @brief �����ַ�������������
 * @details ����֧�ֵ��������������ṩ�豸֧�ֵ������б�
 */
static const uint8_t get_descriptor_string_language[] = {
    USBD_STRING_LANG
};

/**
 * @brief �����ַ�������������
 * @details �ṩ�豸�������Ƶ��ַ���������
 */
static const uint8_t get_descriptor_string_manufactuer[] = {
    USBD_STRING_MANUFACTURER
};

/**
 * @brief ��Ʒ�ַ�������������
 * @details �ṩ�豸��Ʒ���Ƶ��ַ���������
 */
static const uint8_t get_descriptor_string_product[] = {
    USBD_STRING_PRODUCT
};

/**
 * @brief ��������������
 * @details ����HID�豸�ı��������������������豸����ĸ�ʽ�ͽṹ
 */
static const uint8_t get_descriptor_report_interface_0[] = {
    USBD_MOUSE_REPORT_DESCRIPTOR
};

/**
 * @brief ����״̬��Ӧ��������
 * @details ��ʾ�豸������
 */
static const uint8_t get_config_response_configured[] = {1};

/**
 * @brief ����״̬��Ӧ��δ����
 * @details ��ʾ�豸δ����
 */
static const uint8_t get_config_response_unconfigured[] = {0};

/**
 * @brief �豸״̬��Ӧ���Թ�������Զ�̻���
 * @details �豸�����Թ���״̬�Ҳ�֧��Զ�̻���
 * @note LSB���ȣ��Թ��磬��Զ�̻���
 */
static const uint8_t get_status_device_response_no_remote_wakeup[] = {
    ((DEVICE_SELF_POWERED) ? 1 : 0), /**< LSB���ȣ��Թ��磬��Զ�̻��� */
    0
};

/**
 * @brief �豸״̬��Ӧ���Թ�����֧��Զ�̻���
 * @details �豸�����Թ���״̬����֧��Զ�̻���
 * @note LSB���ȣ��Թ��磬֧��Զ�̻���
 */
static const uint8_t get_status_device_response_remote_wakeup[] = {
    ((DEVICE_SELF_POWERED) ? 1 : 0) | 2, /**< LSB���ȣ��Թ��磬֧��Զ�̻��� */
    0
};

/**
 * @brief �ӿ�״̬��Ӧ
 * @details �ӿڵ�״̬��Ӧ����ʾ�ӿ�����״̬
 */
static const uint8_t get_status_interface_response[] = {0, 0};

/**
 * @brief �˵�״̬��Ӧ����ֹͣ
 * @details �˵��״̬��ֹͣ��halted��
 */
static const uint8_t get_status_endpoint_halted_response[] = {1, 0};

/**
 * @brief �˵�״̬��Ӧ���Ѽ���
 * @details �˵��״̬Ϊ�Ѽ��active��
 */
static const uint8_t get_status_endpoint_active_response[] = {0, 0};

/***************************************
 *          USBD��������С            	 *
 ***************************************/
#define GET_CONFIG_DESCRIPTOR_SIZE    sizeof(get_descriptor_configuration) // ��ȡ������������С
#define GET_INTERFACE_DESCRIPTOR_SIZE 9  // �ӿ���������С
#define GET_HID_DESCRIPTOR_SIZE       9  // HID��������С
#define GET_ENDPOINT_DESC_SIZE  7  // �˵���������С

/***************************************
 *              USBD��ַ           	 *	
 **************************************/
/**
 * @brief ��ȡ�ӿ�����������ʼ��ַ
 * @details ���������������нӿ�����������ʼ��ַ
 */
#define get_descriptor_interface_0 &get_descriptor_configuration[9]

/**
 * @brief ��ȡHID����������ʼ��ַ
 * @details ���������������� HID ����������ʼ��ַ
 */
#define get_descriptor_hid_0 &get_descriptor_configuration[9 + GET_INTERFACE_DESCRIPTOR_SIZE]

/**
 * @brief ��ȡ�˵�����������ʼ��ַ
 * @details ���������������ж˵�����������ʼ��ַ
 */
#define get_descriptor_endpoint_1 &get_descriptor_configuration[9 + GET_INTERFACE_DESCRIPTOR_SIZE + GET_HID_DESCRIPTOR_SIZE]


/***************************************
 *              USB��־           	  *	
 **************************************/ 
/**
 * @brief USB���ñ�־
 *
 * ���ڱ��USB��������׼���ô������ݵı�־
 */
static volatile bool mark_usbd_configured = false;

/**
 * @brief USB�����־
 *
 * ���ڱ��USB���ڹ���״̬�����������ݿ���ʱ��Ҫ����
 *
 * @note �˱�������ѭ���и��ġ�
 */
static bool mark_usbd_suspended = false;

/**
 * @brief Զ�̻��ѹ������ñ�־
 *
 * ���ڱ�������Ƿ��ڴ��豸��������Զ�̻��ѹ��ܵ��ڲ���־
 */
static
#if REMOTE_WU
    volatile // ����Զ�̻�������ʱ����ֹ�������Ż�
#endif
bool mark_usbd_remote_wakeup_enabled = true;

/**
 * @brief �豸���ݷ��ʹ�����־
 *
 * �趨�˱�־��ζ��USB�˵����ڷ�����һ�ε��豸����
 */
static volatile bool mark_send_device_data = false;

/**
 * @brief ����Ĺ���״̬
 *
 * ���ڴ�USBD����յ����¼�������ǰ�Ĺ���״̬��
 * �����ֵ��@ref mark_usbd_suspended ��ͬ����ʾ�豸�Ĺ���״̬��Ҫ���£�״̬���Ľ�����ѭ���д���
 */
static volatile bool mark_usbd_suspend_state_request = false;
		
/**
 * @brief ϵͳ�Ƿ��ж���
 *
 */
static volatile bool mark_device_has_motion = false;

/***************************************
 *           USB��־��������           	*	
 **************************************/ 
 /**
 * @brief ��ȡUSB���ñ�־
 *
 * ���ڼ��USB�Ƿ���������׼���ô������ݡ�
 *
 * @return bool ��ǰUSB����״̬��`true`��ʾ�����ã�`false`��ʾδ���á�
 */
bool get_mark_usbd_configured(void) 
{
    return mark_usbd_configured;
}

/**
 * @brief ����USB���ñ�־
 *
 * ���ڱ��USB����״̬��
 *
 * @param value `true`��ʾUSB�����ã�`false`��ʾUSBδ���á�
 */
void set_mark_usbd_configured(bool value) 
{
    mark_usbd_configured = value;
}

/**
 * @brief ��ȡUSB�����־
 *
 * ���USB�Ƿ��ڹ���״̬��
 *
 * @return bool ��ǰUSB����״̬��`true`��ʾ����`false`��ʾδ����
 */
bool get_mark_usbd_suspended(void) {
    return mark_usbd_suspended;
}

/**
 * @brief ����USB�����־
 *
 * ��������USB����״̬��
 *
 * @param value `true`��ʾUSB����`false`��ʾδ����
 */
void set_mark_usbd_suspended(bool value) 
{
    mark_usbd_suspended = value;
}

/**
 * @brief ��ȡԶ�̻��ѹ������ñ�־
 *
 * ��������Ƿ�������Զ�̻��ѹ��ܡ�
 *
 * @return bool ��ǰԶ�̻��ѹ�������״̬��`true`��ʾ�����ã�`false`��ʾδ���á�
 */
bool get_mark_usbd_remote_wakeup_enabled(void) 
{
    return mark_usbd_remote_wakeup_enabled;
}

/**
 * @brief ����Զ�̻��ѹ������ñ�־
 *
 * ��������Զ�̻��ѹ��ܵ�����״̬��
 *
 * @param value `true`��ʾ����Զ�̻��ѹ��ܣ�`false`��ʾ����Զ�̻��ѹ��ܡ�
 */
void set_mark_usbd_remote_wakeup_enabled(bool value) 
{
    mark_usbd_remote_wakeup_enabled = value;
}

/**
 * @brief ��ȡ�豸���ݷ��ʹ�����־
 *
 * ����豸�����Ƿ����ڷ��͡�
 *
 * @return bool ��ǰ�豸���ݷ���״̬��`true`��ʾ���ڷ��ͣ�`false`��ʾδ���͡�
 */
bool get_mark_send_device_data(void) 
{
    return mark_send_device_data;
}

/**
 * @brief �����豸���ݷ��ʹ�����־
 *
 * ���ڱ���豸���ݵķ���״̬��
 *
 * @param value `true`��ʾ���ڷ����豸���ݣ�`false`��ʾδ���͡�
 */
void set_mark_send_device_data(bool value) 
{
    mark_send_device_data = value;
}

/**
 * @brief ��ȡUSB����״̬�����־
 *
 * ��鵱ǰ�Ƿ��й���״̬����
 *
 * @return bool ��ǰ����״̬����`true`��ʾ�й�������`false`��ʾ�޹�������
 */
bool get_mark_usbd_suspend_state_request(void) 
	{
    return mark_usbd_suspend_state_request;
}

/**
 * @brief ����USB����״̬�����־
 *
 * ��������USB����״̬����
 *
 * @param value `true`��ʾ�й�������`false`��ʾ�޹�������
 */
void set_mark_usbd_suspend_state_request(bool value) 
{
    mark_usbd_suspend_state_request = value;
}

/**
 * @brief ��ȡ�豸������־
 *
 */
bool get_mark_device_has_motion(void) 
	{
    return mark_device_has_motion;
}

/**
 * @brief �����豸������־
 *
 */
void set_mark_device_has_motion(bool value) 
{
    mark_device_has_motion = value;
}

/****************************************
 *           USBD ��ʼ������  			     	 *
 ****************************************/
/**
 * @brief ��ʼ��ϵͳ���
 * 
 * ������Դ��ʱ�ӡ���־����ť��LED�ȵĳ�ʼ����
 */
void usb_device_init(void)
{
    ret_code_t ret;
	
    // ��ʼ����Դ��ʱ��
    usb_device_init_power_and_clock(); 
		
    // ��ʼ��GPIO��ť��LED
		usb_device_gpio_init();
    usb_device_gpiote_init();
	
		// ����ϵͳLED�Ͱ�ť
    nrf_gpio_pin_clear(LED_SYSTEM_RUNNING);
    nrf_gpio_pin_clear(LED_SYSTEM_ON);

    // ��־��¼������Ϣ
    NRF_LOG_INFO("USBD Started.");
		NRF_LOG_FLUSH();

    // ��¼��λԭ��
    usb_device_log_reset_reason();
    nrf_power_resetreas_clear(nrf_power_resetreas_get());
		
		// USB���������￪ʼ
    ret = nrf_drv_usbd_init(usb_device_event_handler);
    APP_ERROR_CHECK(ret);

    // ����EP0��������ݰ���С
    nrf_drv_usbd_ep_max_packet_size_set(NRF_DRV_USBD_EPOUT0, EP0_MAXPACKETSIZE);
    nrf_drv_usbd_ep_max_packet_size_set(NRF_DRV_USBD_EPIN0, EP0_MAXPACKETSIZE);
		
		if (USBD_POWER_DETECTION)  // �ж��Ƿ�����USB��Դ���
    {
				// ���� USB ��Դ�¼�
        static const nrf_drv_power_usbevt_config_t config =
        {
            .handler = usb_device_power_event_handler	// USB �¼��������
        };
        nrf_drv_power_usbevt_init(&config); // ��ʼ�� USB ��Դ�¼���ע���¼�������
    }
    else
    {
        NRF_LOG_INFO("No USB power detection enabled\r\nStarting USB now");
				NRF_LOG_FLUSH();
        nrf_delay_us(STARTUP_DELAY); // �ӳ�һ��ʱ��������� USB
        
        if (!nrf_drv_usbd_is_enabled())
        {
            nrf_drv_usbd_enable(); // ���� USB ����
            ret = configure_endpoint(0, NRF_DRV_USBD_EPIN1); // ���ö˵� 1
            APP_ERROR_CHECK(ret);
        }

        // �ȴ��������ϵ�
        while (NRF_DRV_POWER_USB_STATE_CONNECTED == nrf_drv_power_usbstatus_get())
        {
            // �ȴ� USB ״̬Ϊ����״̬
        }
				
        if (NRF_DRV_POWER_USB_STATE_READY == nrf_drv_power_usbstatus_get())
        {
						// ����USB�ϵ�״̬
						NRF_LOG_INFO("USB Power Ready Without Power Detection");
						NRF_LOG_FLUSH();
						nrf_gpio_pin_clear(LED_USB_POWER_CONNECTED);
						
            if (!nrf_drv_usbd_is_started())
            {
                nrf_drv_usbd_start(true);  // ��� USB û������������ USB ����
            }
        }
        else
        {
            nrf_drv_usbd_disable(); // ��� USB ״̬���� READY������ USB
        }
    }
}

/***************************************
 *           USBD ��������          	   *	
 **************************************/ 
/**
 * @brief ��ӡ��ǰϵͳ״̬��־
 *
 * �ú�����ӡ����USB��ϵͳ��ر�־�ĵ�ǰ״̬����ʾ��Ϊtrue��false��
 */
void print_usb_device_system_status(void)
{
		NRF_LOG_INFO("/*****************************************/");
	
    NRF_LOG_INFO("System Status:");
    
    // ��ӡUSB���ñ�־
    NRF_LOG_INFO("USB Configured: %s", mark_usbd_configured ? "true" : "false");
    
    // ��ӡUSB�����־
    NRF_LOG_INFO("USB Suspended: %s", mark_usbd_suspended ? "true" : "false");
		
		// ��ӡUSB����״̬�����־
    NRF_LOG_INFO("USB Suspend State Request: %s", mark_usbd_suspend_state_request ? "true" : "false");
    
    // ��ӡԶ�̻��ѹ������ñ�־
    NRF_LOG_INFO("Remote Wakeup Enabled: %s", mark_usbd_remote_wakeup_enabled ? "true" : "false");
    
    // ��ӡ�豸���ݷ��ʹ�����־
    NRF_LOG_INFO("Send Device Data: %s", mark_send_device_data ? "true" : "false");
		
		NRF_LOG_INFO("/*****************************************/");
		
		NRF_LOG_FLUSH();
}
 /****************************************
 *              USBD ���ú���  			     *
 ****************************************/
 /**
 * @brief ����ָ���˵�Ϊѡ������
 *
 * �ú���Ϊ�ض����ú��ض��˵��������ж˵㡣
 *
 * @note
 * ������������Ϊ0��ζ�ż����Ͻ�����HID�ӿڣ��������Ϊ��Ӧ�������á�
 * ���豸�������⵽USB����ʱ��Ӧ���ô����á�
 *
 * @param index ��������
 * @param ep    Ҫ���õĶ˵�ţ�IN��OUT�˵㣩
 *
 * @retval NRF_ERROR_INVALID_PARAM ��Ч������
 * @retval NRF_SUCCESS             ���óɹ�����
 */
ret_code_t configure_endpoint(uint8_t index, nrf_drv_usbd_ep_t ep)
{
    if (index == 1)	// ����1
    {
        nrf_drv_usbd_ep_dtoggle_clear(ep);  // ���ָ���˵������л�λ
        nrf_drv_usbd_ep_stall_clear(ep);    // ���ָ���˵�ͣ��״̬
        nrf_drv_usbd_ep_enable(ep);         // ����ָ���˵�
        mark_usbd_configured = true;           // ���Ϊ������
        nrf_drv_usbd_setup_clear();         // ����������������ý׶Σ��������
    }
    else if (index == 0) // ����0
    {
        nrf_drv_usbd_ep_disable(ep);        // ����ָ���˵�
        mark_usbd_configured = false;          // ���Ϊδ����
        nrf_drv_usbd_setup_clear();         // ����������������ý׶Σ��������
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;     // ������Ч���ô���
    }
    return NRF_SUCCESS;                     // �������óɹ�
}

 /****************************************
 *           USBD ������Ӧ����  			     *
 ****************************************/
/**
 * @brief ͨ���˵�0��Ӧ���������� Setup ���󣬷����豸����Ϣ�Խ�������
 *
 *
 * @param[in] p_setup ָ��ǰ�����������ݵ�ָ�룬
 *                    �����������ϸ��Ϣ����������С��
 * @param[in] p_data  ָ����Ҫ���͵�ʵ�����ݵ�ָ�롣
 * @param[in] size    Ҫ���͵������ֽ�����
 *
 * @note �ڽ��յ�USBD_EVT_BUFREADY�¼�����ʾ���ݻ�������׼���ã�USB ���������Կ�ʼʵ�ʴ�������֮ǰ��p_dataָ������ݱ��뱣����Ч��
 */
void respond_setup_request(nrf_drv_usbd_setup_t const * const p_setup, void const * p_data, size_t size)
{
		// �����Ҫ���͵����ݴ�С�Ƿ񳬹����������������󳤶� 
    if (size > p_setup->wLength)
    {
        size = p_setup->wLength; // ���������������Ϊ������ָ������󳤶�
    }

    ret_code_t ret;	// ���󱨸�
    
    // �������ڴ����nrf_drv_usbd_transfer_t�ṹ��
    nrf_drv_usbd_transfer_t transfer =
    {
        .p_data = {.tx = p_data}, // ���ô�������ָ�룬ָ��Ҫ���͵�����
        .size = size              // ���ô�������ݴ�С
    };
    
    // ͨ��USB�˵�0��������
    ret = nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN0, &transfer);
    
    // �����������ʧ�ܣ���¼������־
    if (ret != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Transfer starting failed: %d", (uint32_t)ret); // ��¼ʧ��ԭ��
    }

    // ȷ�����������ɹ�������ʱ���ڲ������
    ASSERT(ret == NRF_SUCCESS); // ������ʧ�ܣ�����ʱ�ᴥ������

    // ����δʹ�õı������棬�����������ʾ
    UNUSED_VARIABLE(ret);
}


/**
 * @brief ��Ӧ���������� GetStatus ����ͨ��respond_setup_request������Ϣ
 *
 * �ú������ݲ�ͬ���������ͣ��豸���ӿڡ��˵㣩����GetStatus����
 * ������������ͺ��豸�ĵ�ǰ״̬��������Ӧ��״̬���ݣ������ͣ��״̬��
 *
 * @param[in] p_setup ָ���������͵������������ָ�룬�����������ϸ��Ϣ���������ͺ�������
 */
void respond_get_status_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    switch (p_setup->bmRequestType) // ���������, �豸���ӿڻ�˵�
    {
    case 0x80: // �豸���� 
        if (((p_setup->wIndex) & 0xff) == 0) // ȷ������Ϊ0��������������豸��������ĳ������Ľӿڻ�˵�
        {
            // ��Ӧ��Ӧ���豸״̬
            respond_setup_request(
                p_setup,	// �����
                mark_usbd_remote_wakeup_enabled ? get_status_device_response_remote_wakeup : get_status_device_response_no_remote_wakeup,	// �豸״̬��Ӧ��̬����
                sizeof(get_status_device_response_no_remote_wakeup));  // ������ֽ���
            return; // ������ɣ�ֱ�ӷ���
        }
        break; // δƥ������������ʱ��������������

    case 0x81: // �ӿ����� 
        if (mark_usbd_configured) // ����USB�豸������ʱ��Ӧ�ӿ�����
        {
            if (((p_setup->wIndex) & 0xff) == 0) // �����ѯĳ���ض��ӿڵ�״̬����֧�ֽӿ�0������
            {
								// ��Ӧ��Ӧ�Ľӿ�״̬
                respond_setup_request(
                    p_setup, // �����
                    get_status_interface_response, // �ӿ�����״̬
                    sizeof(get_status_interface_response)); // ������ֽ���
                return; // ������ɣ�ֱ�ӷ���
            }
        }
        break; // δƥ������������ʱ��������������

    case 0x82: // �˵����� 
        if (((p_setup->wIndex) & 0xff) == 0) // �����ѯĳ���ض��˵��״̬����֧�ֶ˵�0������
        {
            // �˵�0ͨ���ǿ��ƶ˵㣬������״̬
            respond_setup_request(
                p_setup, // �����
                get_status_endpoint_active_response,	// �˵�����״̬���Ѽ���
                sizeof(get_status_endpoint_active_response)); // ������ֽ���
            return; // ������ɣ�ֱ�ӷ���
        }
				
				// ����USB�豸������ʱ��Ӧ�����˵�����
        if (mark_usbd_configured) 
        {
            if (((p_setup->wIndex) & 0xff) == NRF_DRV_USBD_EPIN1) // ����Ƿ�Ϊ�˵�1������
            {
                if (nrf_drv_usbd_ep_stall_check(NRF_DRV_USBD_EPIN1)) // ���˵�1�Ƿ���ͣ��״̬
                {
                    // ����˵�1����ͣ��״̬������ͣ��״̬��Ӧ
                    respond_setup_request(
                        p_setup,	// �����
                        get_status_endpoint_halted_response, // �˵����״̬��δ����
                        sizeof(get_status_endpoint_halted_response)); // ������ֽ���
                    return; // ������ɣ�ֱ�ӷ���
                }
                else
                {
                    // ����˵�1δͣ�ͣ����ػ״̬��Ӧ
                    respond_setup_request(
                        p_setup,
                        get_status_endpoint_active_response, // �˵�����״̬���Ѽ���
                        sizeof(get_status_endpoint_active_response)); // ������ֽ���
                    return; // ������ɣ�ֱ�ӷ���
                }
            }
        }
        break; // ���δ���û�˵㲻ƥ�䣬����

    default:
        break; // ����������Ͳ�ƥ�䣬����Ĭ�ϴ�������
    }

    // ����޷���������󣬼�¼������־
    NRF_LOG_ERROR("Unknown status: 0x%2x", p_setup->bmRequestType); // ��¼δ֪��״̬����
    nrf_drv_usbd_setup_stall(); // ���˵����ͣ��״̬����ʾ�޷����������
}


/**
 * @brief ��Ӧ���������� ClearFeature��������˵�/�豸��״̬/����
 *
 * �ú������������������USB�豸��ĳЩ���ԣ���˵�ͣ��״̬��Զ�̻��ѹ��ܡ�
 * ���豸���յ�ClearFeature����ʱ���ú������������Ľ����ߣ��˵���豸��
 * ������Ӧ�Ĵ������������״̬������޷����������������ͣ��״̬��
 *
 * @param[in] p_setup ָ��ǰUSB�����������ݵ�ָ�룬
 *                    �����������ϸ��Ϣ���������͡����Ա�ŵȡ�
 */
void respond_clear_feature_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    // ������������Ƿ�Ϊ��׼�����ҽ�����Ϊ�˵� (bmRequestType = 0x02)
    if ((p_setup->bmRequestType) == 0x02) // ��׼���󣬽�����Ϊ�˵�
    {
        // �����������ͱ�� (wValue == 0) ����ʾ����������ı�˵�ͣ��״̬
        if ((p_setup->wValue) == 0)
        {
            // �������Ķ˵����� (wIndex) �Ƿ�Ϊ�˵�1
            if ((p_setup->wIndex) == NRF_DRV_USBD_EPIN1)
            {
                // ����˵�1��ͣ��״̬
                nrf_drv_usbd_ep_stall_clear(NRF_DRV_USBD_EPIN1); // ����˵�ͣ��״̬
                nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ�������ɣ���ʾ�����ɹ�
                return; // ������ɣ�����
            }
        }
    }
    // ������������Ƿ�Ϊ��׼�����ҽ�����Ϊ�豸 (bmRequestType = 0x00)
    else if ((p_setup->bmRequestType) == 0x0) // ��׼���󣬽�����Ϊ�豸
    {
        // ����Ƿ�������Զ�̻��ѹ��� (REMOTE_WU)
        if (REMOTE_WU)
        {
            // �����������Ա�� (wValue == 1)����ʾ����������ı�Զ�̻��ѹ���
            if ((p_setup->wValue) == 1) // ���Ա��ΪԶ�̻���
            {
                // �����豸��Զ�̻��ѹ���
                mark_usbd_remote_wakeup_enabled = false; // ����Զ�̻��ѹ���
                nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ�������ɣ���ʾ�����ɹ�
                return; // ������ɣ�����
            }
        }
    }

    // ������������޷��������Ч����¼������־
    NRF_LOG_ERROR("Unknown feature to clear"); // δ֪����������󣬼�¼������־
    nrf_drv_usbd_setup_stall(); // ����ͣ��״̬����ʾ�޷����������
}



/**
 * @brief ��Ӧ���������� SetFeature�������ö˵�/�豸��״̬/����
 *
 * �ú������������������󣬸�������Ľ����ߣ��˵���豸����
 * ����ĳЩ���ԣ���˵�ͣ��״̬��Զ�̻��ѹ��ܣ���
 *
 * @param[in] p_setup ָ��ǰUSB�����������ݵ�ָ�룬
 *                    �����������ϸ��Ϣ���������͡����Ա�ŵȡ�
 */
void respond_set_feature_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    // ������������Ƿ�Ϊ��׼���󣬽�����Ϊ�˵� (bmRequestType = 0x02)
    if ((p_setup->bmRequestType) == 0x02) // ��׼���󣬽�����Ϊ�˵�
    {
        if ((p_setup->wValue) == 0) //  �����������ͱ�� (wValue == 0) ����ʾ����������ı�˵�ͣ��״̬
        {
            if ((p_setup->wIndex) == NRF_DRV_USBD_EPIN1) // ����Ƿ���Զ˵�1
            {
                nrf_drv_usbd_ep_stall(NRF_DRV_USBD_EPIN1); // ʹ�˵�1����ͣ��״̬
                nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ��������
                return; // �����ɹ�������
            }
        }
    }
    // ������������Ƿ�Ϊ��׼���󣬽�����Ϊ�豸 (bmRequestType = 0x00)
    else if ((p_setup->bmRequestType) == 0x00) // ��׼���󣬽�����Ϊ�豸
    {
        if (REMOTE_WU) // ����豸֧��Զ�̻���
        {
            if ((p_setup->wValue) == 1) // �����������Ա�� (wValue == 1)����ʾ����������ı�Զ�̻��ѹ���
            {
                mark_usbd_remote_wakeup_enabled = true; // �����豸��Զ�̻��ѹ���
                nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ��������
                return; // �����ɹ�������
            }
        }
    }

    // ��������޷�������¼������־
    NRF_LOG_ERROR("Unknown feature to set"); // δ֪������������
    nrf_drv_usbd_setup_stall(); // ���豸����ͣ��״̬����ʾ�޷����������
}

/**
 * @brief ��Ӧ���������� GetDescriptor ���󣬷�������������
 *
 * �ú��������������ͷ�����Ӧ�������������豸�����á��ӿڡ��˵�ȣ���
 * ��������������Ͱ����豸���������������������ַ�����������
 * �ӿ����������˵���������HID��������HID������������
 *
 * @param[in] p_setup ָ��ǰUSB�����������ݵ�ָ�룬
 *                    �����������ϸ��Ϣ����������������ͺ�ֵ��
 */
void respond_get_descriptor_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    // ����wValue�ĸ�8λ��ȷ�����������������
    switch ((p_setup->wValue) >> 8)
    {
    case 1: // �豸������
        if ((p_setup->bmRequestType) == 0x80) // ��������Ϊ ��׼ �豸������ �豸 ����
        {
            respond_setup_request(
                p_setup,
                get_descriptor_device, // �����豸������
                sizeof(get_descriptor_device)); // ������ֽڴ�С
            return;
        }
        break;
    case 2: // ����������
        if ((p_setup->bmRequestType) == 0x80) // ��������Ϊ��׼IN����
        {
            respond_setup_request(
                p_setup,
                get_descriptor_configuration, // ��������������
                GET_CONFIG_DESCRIPTOR_SIZE); // ������ֽڴ�С
            return;
        }
        break;
    case 3: // �ַ���������
        if ((p_setup->bmRequestType) == 0x80) // ��������Ϊ ��׼ �豸������ �豸 ����
        {
            // ����wValue�ĵ�8λѡ��������ַ���������
            switch ((p_setup->wValue) & 0xFF)
            {
            case USBD_STRING_LANG_IX: // �����ַ��������� 0x00
                respond_setup_request(
                    p_setup,
                    get_descriptor_string_language, // ���������ַ���������
                    sizeof(get_descriptor_string_language));
                return;
            case USBD_STRING_MANUFACTURER_IX: // �����ַ��������� 0x01
                respond_setup_request(
                    p_setup,
                    get_descriptor_string_manufactuer, // ���س����ַ���������
                    sizeof(get_descriptor_string_manufactuer));
                return;
            case USBD_STRING_PRODUCT_IX: // ��Ʒ�ַ��������� 0x02
                respond_setup_request(
                    p_setup,
                    get_descriptor_string_product, // ���ز�Ʒ�ַ���������
                    sizeof(get_descriptor_string_product));
                return;
            default:
                break;
            }
        }
        break;
    case 4: // �ӿ�������
        if ((p_setup->bmRequestType) == 0x80) // ��������Ϊ ��׼ �豸������ �豸 ����
        {
            if (((p_setup->wValue) & 0xFF) == 0) // ��֧�ֽӿ�0
            {
                respond_setup_request(
                    p_setup,
                    get_descriptor_interface_0, // ���ؽӿ�0������
                    GET_INTERFACE_DESCRIPTOR_SIZE); // ���ͽӿ�������
                return;
            }
        }
        break;
    case 5: // �˵�������
        if ((p_setup->bmRequestType) == 0x80) // ��������Ϊ ��׼ �豸������ �豸 ����
        {
            if (((p_setup->wValue) & 0xFF) == 1) // ��֧�ֶ˵�1
            {
                respond_setup_request(
                    p_setup,
                    get_descriptor_endpoint_1, // ���ض˵�1������
                    GET_ENDPOINT_DESC_SIZE); // ���Ͷ˵�������
                return;
            }
        }
        break;
		case 6: // �豸�޶�������
        if ((p_setup->bmRequestType) == 0x80) // ��������Ϊ ��׼ �豸������ �豸 ����
        {
						nrf_drv_usbd_setup_stall(); // ��֧�ָ����󣬷���STALL�ź�
            return;
        }
        break;
    case 0x21: // HID����������
        if ((p_setup->bmRequestType) == 0x81) // ��������Ϊ ��׼ �豸������ �ӿ� ����
        {
            // �������Ľӿ�
            if (((p_setup->wValue) & 0xFF) == 0) // ��֧�ֽӿ�0
            {
                respond_setup_request( // ����HID�ӿ�������
                    p_setup,
                    get_descriptor_hid_0,
                    GET_HID_DESCRIPTOR_SIZE);
                return; // ������ϣ�����
            }
        }
        break;

    case 0x22: // HID��������������
        if ((p_setup->bmRequestType) == 0x81) // ��������Ϊ ��׼ �豸������ �ӿ� ����
        {
            // �������Ľӿ�
            if (((p_setup->wValue) & 0xFF) == 0) // ��֧�ֽӿ�0
            {
                respond_setup_request( // ����HID����������
                    p_setup,
                    get_descriptor_report_interface_0,
                    sizeof(get_descriptor_report_interface_0));
                return; // ������ϣ�����
            }
        }
        break;

    default:
        break; // ��֧�ֵ��������ͣ�����ͣ��״̬
    }

    // ��־��¼δ֪�������Ϣ
    NRF_LOG_ERROR("Unknown descriptor requested: 0x%2x, type: 0x%2x or value: 0x%2x",
        p_setup->wValue >> 8, // ����������������
        p_setup->bmRequestType, // ��������
        p_setup->wValue & 0xFF); // ������ض�ֵ
    nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
}


/**
 * @brief ����GetConfig����
 *
 * �����豸������״̬��������Ӧ��������Ӧ��
 * @param[in] p_setup ָ�������������ݵ�ָ�롣
 */
void respond_get_config_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    if (mark_usbd_configured) // ����豸�Ƿ�������
    {
        respond_setup_request( // ���������õ���Ӧ
            p_setup,
            get_config_response_configured,	// ������
            sizeof(get_config_response_configured));
    }
    else
    {
        respond_setup_request( // ����δ���õ���Ӧ
            p_setup,
            get_config_response_unconfigured, // δ����
            sizeof(get_config_response_unconfigured));
    }
}

/**
 * @brief ����SetConfig����
 *
 * �������������еĲ��������豸����״̬��
 * @param[in] p_setup ָ�������������ݵ�ָ�롣
 */
void respond_set_config_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    if ((p_setup->bmRequestType) == 0x00) // ȷ����������Ϊ ��׼ �������豸 �豸 ����
    {
        // ��������������0��1���������󳤶�Ϊ0
        if (((p_setup->wIndex) == 0) && ((p_setup->wLength) == 0) &&
            ((p_setup->wValue) <= UINT8_MAX))
        {
            // ����configure_endpoint�������ö˵�1����
            if (NRF_SUCCESS == configure_endpoint((uint8_t)(p_setup->wValue),NRF_DRV_USBD_EPIN1))
            {
                nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ��������
                return; // ����ɹ�������
            }
        }
    }
    // ��־��¼�������������
    NRF_LOG_ERROR("Wrong configuration: Index: 0x%2x, Value: 0x%2x.",
        p_setup->wIndex, // ���������
        p_setup->wValue); // �����ֵ
    nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
}

/**
 * @brief ����SetIdle���� TODO������״̬����
 *
 * ���տ������󲢽���������������ý׶Σ�������ɡ�
 * @param[in] p_setup ָ�������������ݵ�ָ�롣
 */
void respond_set_idle_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    if (p_setup->bmRequestType == 0x21) // ȷ����������Ϊ class �������豸 �ӿ� ����
    {
        // �����κ�ֵ
        nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ��������
        return; // ����ɹ�������
    }
    // ��־��¼�������������
    NRF_LOG_ERROR("Set Idle wrong type: 0x%2x.", p_setup->bmRequestType);
    nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
}

/**
 * @brief ����SetInterface����
 *
 * ��֧��������ã�ʼ�ս���ͣ��״̬��
 * @param[in] p_setup ָ�������������ݵ�ָ�롣
 */
void respond_set_interface_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    // ��֧���κ�����ӿڣ�ʼ�ս���ͣ��״̬
    NRF_LOG_ERROR("No alternate interfaces supported.");
    nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
}

/**
 * @brief ����SetProtocol���� TODO: ����Э��
 *
 * ��������Э�����󲢽���������������ý׶Σ�������ɡ�
 * @param[in] p_setup ָ�������������ݵ�ָ�롣
 */
static void respond_set_protocol_request(nrf_drv_usbd_setup_t const * const p_setup)
{
    if (p_setup->bmRequestType == 0x21) // ȷ����������Ϊ class �������豸 �ӿ� ����
    {
        // �����κ�ֵ
        nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ��������
        return; // ����ɹ�������
    }
    // ��־��¼�������������
    NRF_LOG_ERROR("Set Protocol wrong type: 0x%2x.", p_setup->bmRequestType);
    nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
}

 /****************************************
 *            USBD �¼�������  			   *
 ****************************************/
/**
 * @brief USB �¼��������
 *
 * ���ݽ��յ��� USB �¼����ͣ�ִ����Ӧ�Ĳ�����������ͣ���ָ������õȡ�
 * @param[in] p_event ָ���¼����ݵ�ָ�롣
 */
void usb_device_event_handler(nrf_drv_usbd_evt_t const * const p_event)
{
    switch (p_event->type) // �����¼����ͽ��д���
    {
    case NRF_DRV_USBD_EVT_SUSPEND:	// ������ͣ
        NRF_LOG_INFO("Suspend Requested"); 
        mark_usbd_suspend_state_request = true; // ������ͣ����״̬
        break;

    case NRF_DRV_USBD_EVT_RESUME: // �������ͣ�ָ�
        NRF_LOG_INFO("Suspend Resume Requested");
        mark_usbd_suspend_state_request = false; // �����ͣ����״̬
        break;

    case NRF_DRV_USBD_EVT_WUREQ:	// ����Զ�̻���
        NRF_LOG_INFO("Remote Wake Up Requested"); 
        mark_usbd_suspend_state_request = false; // �����ͣ����״̬
        break;

    case NRF_DRV_USBD_EVT_RESET: // ��������
        {
						NRF_LOG_INFO("Reset Requested"); 
            ret_code_t ret = configure_endpoint(0, NRF_DRV_USBD_EPIN1); // ���ö˵�1����
            ASSERT(ret == NRF_SUCCESS); // ȷ�����óɹ�
            UNUSED_VARIABLE(ret); // ��ֹδʹ�ñ�������
            mark_usbd_suspend_state_request = false; // �����ͣ����״̬
            break;
        }

    case NRF_DRV_USBD_EVT_SOF: // ��ʼ֡ Start Of Frame�� �ṩ�Ӿ�������ָʾ�豸�� USB ״̬
        {
						if (mark_usbd_configured)
						{
								nrf_gpio_pin_clear(LED_USB_CONFIGURED); // �򿪱�ʾUSB״̬��LED
						}
						else
						{
								static uint32_t cycle = 0; // ��������
								++cycle; // �������ڼ���
								if (cycle % 1000 == 0) // ����������������1000��������˸һ�Σ�����ȡ���ڶ˵���������ѯ����
								{
									nrf_gpio_pin_toggle(LED_USB_CONFIGURED);
								}
						}
            break;
        }

    case NRF_DRV_USBD_EVT_EPTRANSFER: // �˵㴫���¼������˵���ɴ���ʱ����������˵�����������
        if (NRF_DRV_USBD_EPIN1 == p_event->data.eptransfer.ep) // ����Ƿ���IN�˵�1��ɷ���
        {
            mark_send_device_data = false; // ��ʾ�˴��豸���ݱ����ѷ�����ɣ�����Ҫ�ٴη��ͣ�ֱ����Ҫ�µ�λ�ñ���ʱ
        }
        else if (NRF_DRV_USBD_EPIN0 == p_event->data.eptransfer.ep) // ����Ƿ���IN�˵�0��ɷ���
        {
            if (NRF_USBD_EP_OK == p_event->data.eptransfer.status) // ����ɹ�
            {
                nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ��������
            }
            else if (NRF_USBD_EP_ABORTED == p_event->data.eptransfer.status) // ������ֹ
            {
                NRF_LOG_INFO("Transfer aborted event on EPIN0"); // ��¼��ֹ�¼�
            }
            else // ����ʧ��
            {
                NRF_LOG_ERROR("Transfer failed on EPIN0: %d", p_event->data.eptransfer.status); // ��¼����
                nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
            }
        }
        else if (NRF_DRV_USBD_EPOUT0 == p_event->data.eptransfer.ep) // ����Ƿ���OUT�˵�0��ɽ���
        {
            if (NRF_USBD_EP_OK == p_event->data.eptransfer.status) // ����ɹ�
            {
                nrf_drv_usbd_setup_clear(); // ����������������ý׶Σ��������
            }
            else if (NRF_USBD_EP_ABORTED == p_event->data.eptransfer.status) // ������ֹ
            {
                NRF_LOG_INFO("Transfer aborted event on EPOUT0"); // ��¼��ֹ�¼�
            }
            else // ����ʧ��
            {
                NRF_LOG_ERROR("Transfer failed on EPOUT0: %d", p_event->data.eptransfer.status); // ��¼����
                nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
            }
        }
        else
        {
            // ���账������
        }
        break;

    case NRF_DRV_USBD_EVT_SETUP: // �������������¼��������������ôӻ�/��ȡ�ӻ���Ϣ
        {
            nrf_drv_usbd_setup_t setup; // �����������
            nrf_drv_usbd_setup_get(&setup); // ��ȡ������������ -> �������
            switch (setup.bRequest) // �����������ͽ��д���
            {
            case 0x00: // GetStatus
                respond_get_status_request(&setup); // ��ȡ״̬������
                break;
            case 0x01: // ClearFeature
                respond_clear_feature_request(&setup); // �������������
                break;
            case 0x03: // SetFeature
                respond_set_feature_request(&setup); // ��������������
                break;
            case 0x05: // SetAddress
                // �����κβ�������Ӳ�������������� STALL
                break;
            case 0x06: // GetDescriptor
                respond_get_descriptor_request(&setup); // ��ȡ������������
                break;
            case 0x08: // GetConfig
                respond_get_config_request(&setup); // ��ȡ����������
                break;
            case 0x09: // SetConfig
                respond_set_config_request(&setup); // ��������������
                break;
            // HID ��
            case 0x0A: // SetIdle
                respond_set_idle_request(&setup); // ���ÿ���������
                break;
            case 0x0B: // SetProtocol �� SetInterface
                if (setup.bmRequestType == 0x01) // ��׼���󣬽�����Ϊ�ӿ�
                {
                    respond_set_interface_request(&setup); // ���ýӿ�������
                }
                else if (setup.bmRequestType == 0x21) // �����󣬽�����Ϊ�ӿ�
                {
                    respond_set_protocol_request(&setup); // ����Э��������
                }
                else
                {
                    NRF_LOG_ERROR("Command 0xB. Unknown request: 0x%2x", setup.bmRequestType); // ��¼δ֪�������
                    nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
                }
                break;
            default:
                NRF_LOG_ERROR("Unknown request: 0x%2x", setup.bRequest); // ��¼δ֪�������
                nrf_drv_usbd_setup_stall(); // ����ͣ��״̬
                return; // ����
            }
            break;
        }

    default:
        break; // ���������¼�
    }
}

 /****************************************
 *            USBD ��Դ����  			     	*
 ****************************************/
/**
 * @brief USB ��Դ�¼��������
 *
 * ���� USB ��Դ�¼����ͣ�ִ����Ӧ�Ĳ����������⵽ USB ��Դ���Ƴ���Դ��׼��������
 * @param event USB ��Դ�¼�����
 */
void usb_device_power_event_handler(nrf_drv_power_usb_evt_t event)
{
    switch (event) // �����¼����ͽ��д���
    {
    case NRF_DRV_POWER_USB_EVT_DETECTED: // ��⵽ USB ��Դ
        NRF_LOG_INFO("USB Power Detected"); // ��¼��⵽��Դ
        if (!nrf_drv_usbd_is_enabled()) // ��� USB ��δ����
        {
            nrf_drv_usbd_enable(); // ���� USB
        }
        break;

    case NRF_DRV_POWER_USB_EVT_REMOVED: // USB ��Դ���Ƴ�
        NRF_LOG_INFO("USB Power Removed"); // ��¼��Դ�Ƴ�
        mark_usbd_configured = false; // ���� USB ����Ϊδ����
        mark_send_device_data = false; // ��������豸���ݱ�־
        if (nrf_drv_usbd_is_started()) // ��� USB ������
        {
            nrf_drv_usbd_stop(); // ֹͣ USB
        }
        if (nrf_drv_usbd_is_enabled()) // ��� USB ������
        {
            nrf_drv_usbd_disable(); // ���� USB
        }
        /* �ر� LED */
        nrf_gpio_pin_set(LED_USB_CONFIGURED); // �ر� USB ״̬ LED
        nrf_gpio_pin_set(LED_USB_POWER_CONNECTED ); // �ر� USB ��Դ LED
        break;

    case NRF_DRV_POWER_USB_EVT_READY: // USB ׼������
        NRF_LOG_INFO("USB Power Ready With Power Detection"); // ��¼ USB ��Դ׼������
        nrf_gpio_pin_clear(LED_USB_POWER_CONNECTED ); // �򿪱�ʾUSB��Դ��ָʾ��
        if (!nrf_drv_usbd_is_started()) // ��� USB ��δ����
        {
            nrf_drv_usbd_start(true); // ���� USB
        }
        break;

    default:
        ASSERT(false); // �������δ֪�¼�������ʧ��
    }
}

/****************************************
 *              GPIO ����               *
 ****************************************/
/**
 * @brief ����GPIO�¼�
 * @param pin ���ź�
 * @param action GPIO���Ŷ����������ػ��½��أ�
 * 
 * ��������״ִ̬����Ӧ����������ϵͳ�ر���������á�
 */
void gpiote_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (pin == BTN_SYSTEM_OFF) // ����Ƿ���ϵͳ�رհ�ť������
    {
        if (action == NRF_GPIOTE_POLARITY_HITOLO) // ��ť�ͷŴ���
        {		
						nrf_gpio_pin_set(LED_USB_POWER_CONNECTED ); // �رձ�ʾϵͳ����״̬��LED
						nrf_gpio_pin_set(LED_USB_CONFIGURED); // �رձ�ʾϵͳ��Ծ״̬��LED
            nrf_gpio_pin_set(LED_SYSTEM_RUNNING); // �رձ�ʾϵͳ����״̬��LED
						nrf_gpio_pin_set(LED_SYSTEM_ON); // �رձ�ʾϵͳ��Ծ״̬��LED
						if (nrf_drv_usbd_is_started()) // ��� USB ������
						{
								nrf_drv_usbd_stop(); // ֹͣ USB
						}
						if (nrf_drv_usbd_is_enabled()) // ��� USB ������
						{
								nrf_drv_usbd_disable(); // ���� USB
						}
						NRF_LOG_INFO("System OFF"); // ��ӡϵͳ�����ػ�����־��Ϣ
						NRF_LOG_FLUSH();
            nrf_power_system_off(); // ��ϵͳ����ػ�ģʽ
        }
    }
}

/**
 * @brief ��ʼ��GPIO
 * 
 * ��ʼ��GPIO������
 */
void usb_device_gpio_init(void)
{
    nrf_gpio_cfg_output(LED_USB_POWER_CONNECTED ); // USB ��Դָʾ��
		nrf_gpio_cfg_output(LED_USB_CONFIGURED); // USB ״ָ̬ʾ��
		nrf_gpio_cfg_output(LED_SYSTEM_RUNNING); // USB ����ָʾ��
		nrf_gpio_cfg_output(LED_SYSTEM_ON); // USB ��Ծָʾ��
}

/**
 * @brief ��ʼ��GPIOTE
 * 
 * ��ʼ��GPIOTEģ�鲢���ð�ť���ź��¼��������
 */
void usb_device_gpiote_init(void)
{
    ret_code_t ret;

    // ��ʼ��GPIOTEģ��
    if (!nrf_drv_gpiote_is_init()) 
    {
        ret = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(ret);
    }

    // ����ϵͳ�رհ�ť����
    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true); // ����Ϊ�½��ش���
		config.pull = NRF_GPIO_PIN_PULLUP; // ʹ���ڲ���������
    ret = nrf_drv_gpiote_in_init(BTN_SYSTEM_OFF, &config, gpiote_evt_handler); // ��ʼ������
    APP_ERROR_CHECK(ret);

    // ���������¼�
    nrf_drv_gpiote_in_event_enable(BTN_SYSTEM_OFF, true);

    /* �����ڽ��ö���ʱ�ľ��� */
    UNUSED_VARIABLE(ret);
}

 /****************************************
 *            ʱ�� & ��Դ����		     	  *
 ****************************************/
/**
 * @brief ��ʼ��ʱ��ģ��
 *
 * ��ʼ����Ƶ�͵�Ƶʱ�ӣ�ȷ���豸��ʱ��Դ���á�
 */
void usb_device_init_power_and_clock(void)
{
    ret_code_t ret;
		
    // ��ʼ��ʱ��
    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret); // ���ʱ�ӳ�ʼ���Ƿ�ɹ�
		
		// ��ʼ����Դ
		ret = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(ret); // ����Դ��ʼ���Ƿ�ɹ�

    // �����Ƶʱ�Ӻ͵�Ƶʱ��
    nrf_drv_clock_hfclk_request(NULL); // �����Ƶʱ��
    nrf_drv_clock_lfclk_request(NULL); // �����Ƶʱ��
		
	
    // �ȴ�ʱ������ 
    while (!(nrf_drv_clock_hfclk_is_running() &&
             nrf_drv_clock_lfclk_is_running()))
    {
        // �ȴ�ʱ������
    }

    // ��ʼ��Ӧ�ö�ʱ��
    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    // �����ڽ��ö���ʱ�ľ���
    UNUSED_VARIABLE(ret);
}

 /****************************************
 *               ��λ����  			     	  *
 ****************************************/
/**
 * @brief ��¼��λԭ��
 * 
 * ��ȡ����¼�豸��λԭ�򣬰������Ժ͹����Ų顣
 */
void usb_device_log_reset_reason(void)
{
    /* ��λԭ�� */
    uint32_t rr = nrf_power_resetreas_get(); // ��ȡ��λԭ��
    NRF_LOG_INFO("Reset reasons:");
    if (0 == rr)
    {
        NRF_LOG_INFO("- NONE"); // �޸�λԭ��
    }
    if (0 != (rr & NRF_POWER_RESETREAS_RESETPIN_MASK))
    {
        NRF_LOG_INFO("- RESETPIN"); // ��λ���Ÿ�λ
    }
    if (0 != (rr & NRF_POWER_RESETREAS_DOG_MASK))
    {
        NRF_LOG_INFO("- DOG"); // ���Ź���λ
    }
    if (0 != (rr & NRF_POWER_RESETREAS_SREQ_MASK))
    {
        NRF_LOG_INFO("- SREQ"); // �������λ
    }
    if (0 != (rr & NRF_POWER_RESETREAS_LOCKUP_MASK))
    {
        NRF_LOG_INFO("- LOCKUP"); // ������λ
    }
    if (0 != (rr & NRF_POWER_RESETREAS_OFF_MASK))
    {
        NRF_LOG_INFO("- OFF"); // �رո�λ
    }
#if defined(NRF_POWER_RESETREAS_LPCOMP_MASK)
    if (0 != (rr & NRF_POWER_RESETREAS_LPCOMP_MASK))
    {
        NRF_LOG_INFO("- LPCOMP"); // �Ƚ�����λ
    }
#endif
    if (0 != (rr & NRF_POWER_RESETREAS_DIF_MASK))
    {
        NRF_LOG_INFO("- DIF"); // ���츴λ
    }
#if defined(NRF_POWER_RESETREAS_NFC_MASK)
    if (0 != (rr & NRF_POWER_RESETREAS_NFC_MASK))
    {
        NRF_LOG_INFO("- NFC"); // NFC��λ
    }
#endif
    if (0 != (rr & NRF_POWER_RESETREAS_VBUS_MASK))
    {
        NRF_LOG_INFO("- VBUS"); // VBUS��λ
    }
}

/****************************************
 *               ������  			   		*
 ****************************************/
/**
 * @brief ����USB��״̬,ǿ�ƽ������ģʽ
 * 
 */
void usb_device_force_suspend(void)
{
		NRF_LOG_INFO("Suspend Requested, Going to Suspend"); // ��ӡ������־��Ϣ
		NRF_LOG_FLUSH();
		nrfx_usbd_force_bus_suspend(); // ǿ�����߹��� **�Զ��壬Σ�ղ���
		mark_usbd_suspended = nrf_drv_usbd_suspend(); // ���Խ���USB����״̬�����±�־
		if (mark_usbd_suspended) // ���USB�ɹ��������״̬
		{
				nrf_gpio_pin_set(LED_SYSTEM_RUNNING); // �رձ�ʾϵͳ����״̬��LED
				nrf_gpio_pin_set(LED_USB_POWER_CONNECTED ); // �رձ�ʾϵͳ����״̬��LED
		}
}

/**
 * @brief ����USB��״̬,ǿ���뿪����ģʽ
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
		nrf_gpio_pin_clear(LED_SYSTEM_RUNNING); // �رձ�ʾϵͳ����״̬��LED
		mark_usbd_suspended = false; // �������־��Ϊfalse
}

