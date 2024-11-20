/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include "esb_device.h"
#include "usb_device.h"

/***************************************
 *            ESB���ض���              *
 ***************************************/
/// @brief ����һ�����գ�RX�����ؽṹ��
static nrf_esb_payload_t rx_payload;
static uint8_t databuffer[6];  // ���ڴ洢���͵��������


/***************************************
 *              ESB����             	 *
 ***************************************/
/**
 * @brief ESB�¼�������
 * @param p_event ָ��ESB�¼���ָ��
 * 
 * �����¼����ʹ�����Ӧ��ESB�¼����ɹ����͡�����ʧ�ܡ����յ����ݰ��ȡ�
 */
void esb_device_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            NRF_LOG_DEBUG("TX SUCCESS EVENT");  // ��¼����ɹ��¼���־
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            NRF_LOG_DEBUG("TX FAILED EVENT");   // ��¼����ʧ���¼���־
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            NRF_LOG_DEBUG("RX RECEIVED EVENT"); // ��¼���������¼���־
            if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
            {
                set_mark_usbd_suspend_state_request(false);
								// ��� USB ���ڹ���״̬����������Զ�̻���
								if (get_mark_usbd_suspended() && get_mark_usbd_remote_wakeup_enabled())
								{
										UNUSED_RETURN_VALUE(nrf_drv_usbd_wakeup_req()); // ���� USB ����
								}
								else  // ��� USB û�й���
								{	
										// ���� rx_payload.data ��ǰ 6 ���ֽڵ� databuffer ��
										memcpy(databuffer, rx_payload.data, 6);
									
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
            break;
    }
}

/**
 * @brief ��ʼ����ǿ��ShockBurst��ESB��Э��
 * @return ���س�ʼ���Ĵ������
 * 
 * ���ò���ʼ��ESBͨ��Э�飬��������ַ��ǰ׺��ַ���á�
 */
uint32_t esb_module_init(void)
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7}; // ����ַ0�����ڽ���/���͵�ַ��ƥ��
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2}; // ����ַ1�����ڽ���/���͵�ַ��ƥ�䣬һ�ָ�������ַ����ĳЩ���ⳡ����ʹ�ã�������豸ͨ��
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8}; // �ܵ�ǰ׺��ַ���飬���ڶ�ͨ��ͨ��
		
		// PTX��ʹ��base_addr_0��Ϊ��ҪĿ���ַ�������ݷ��͵����ն˵�һ������ͨ��
		// PRX������base_addr_0��ǰ׺��ַ���ɶ�����յ�ַ����������߼�ͨ��

    // Ĭ��ESB���ýṹ��
    nrf_esb_config_t nrf_esb_config = NRF_ESB_DEFAULT_CONFIG; // ����Ĭ�ϵ�ESB���ò���

    // ����Э������
    nrf_esb_config.protocol = NRF_ESB_PROTOCOL_ESB_DPL; // ʹ��Enhanced ShockBurst DPLЭ�飨ESB DPL����֧�ֶ�̬���س��� Dynamic Payload Length

    // ������Ч���ش�С
    nrf_esb_config.payload_length           = 8;                      ///< ������Ч���س���Ϊ8�ֽ�

    // ����ͨ������
    nrf_esb_config.bitrate = NRF_ESB_BITRATE_2MBPS; // ����Ϊ2Mbps�����ݴ������ʣ��ṩ�Ͽ��ͨ���ٶ�

    // ����ESB�¼�������
    nrf_esb_config.event_handler = esb_device_event_handler; // ָ���û������ESB�¼������������ڴ����ͳɹ���ʧ�ܼ����������¼�

    // ���ù���ģʽ
    nrf_esb_config.mode = NRF_ESB_MODE_PRX; // �����豸Ϊ����ģʽ��PRX��

    // �����Զ�Ӧ��
    nrf_esb_config.selective_auto_ack = false; // ����ѡ�����Զ�Ӧ�𣬼����н��շ����Զ�����ACK����ͨ�Ź���

    // ��ʼ��ESBģ��
    err_code = nrf_esb_init(&nrf_esb_config); // ʹ�����ýṹ����г�ʼ��
    VERIFY_SUCCESS(err_code); // ��֤��ʼ���ɹ�

    // ���û���ַ0
    err_code = nrf_esb_set_base_address_0(base_addr_0); // ��base_addr_0����Ϊ����ַ0
    VERIFY_SUCCESS(err_code); // ��֤���óɹ�

    // ���û���ַ1
    err_code = nrf_esb_set_base_address_1(base_addr_1); // ��base_addr_1����Ϊ����ַ1
    VERIFY_SUCCESS(err_code); // ��֤���óɹ�

    // ����ǰ׺��ַ
    err_code = nrf_esb_set_prefixes(addr_prefix, NRF_ESB_PIPE_COUNT); // ����8��ǰ׺��ַ�����ڶ��8������������ͨ��
    VERIFY_SUCCESS(err_code); // ��֤���óɹ�

    return err_code; // �������Ĵ������
}


/**
 * @brief esb��ʼ������
 * 
 * ��ʼ����־��GPIO��ʱ�ӡ�ESBЭ�飬�����Ϸ������ݰ���
 */
void esb_device_init(void)
{
    ret_code_t err_code;

    esb_device_clocks_start();                     // ����ʱ��

    err_code = esb_module_init();              // ��ʼ��ESBЭ��
    APP_ERROR_CHECK(err_code);          // ������

    NRF_LOG_INFO("Enhanced ShockBurst Transmitter Example started.");
	
		err_code = nrf_esb_start_rx(); // ����ESB����ģʽ
    APP_ERROR_CHECK(err_code);
}


/***************************************
 *              ESBʱ�Ӻ���             	*
 ***************************************/
/**
 * @brief ����ʱ��
 * 
 * ������Ƶʱ����ȷ��ϵͳ�ȶ����С�
 */
void esb_device_clocks_start(void)
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0; // ���HFCLK�����¼�
    NRF_CLOCK->TASKS_HFCLKSTART = 1;    // ����HFCLK

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0); // �ȴ�HFCLK����
}
