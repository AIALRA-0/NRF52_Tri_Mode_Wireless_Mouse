/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include "paw3399.h"

 /****************************************
 *             PAW3399����               *
 ****************************************/
static uint8_t motion_burst_data[12] = {0};	// �洢Motion Burst����
static uint8_t raw_data[1295] = {0};	// �洢Raw Data����
static uint8_t var_a = 0;	// ̧���ж�У׼����A
static uint8_t var_b = 0;	// ̧���ж�У׼����B
static int16_t delta_x = 0;  // ���ڴ洢��ȡ����Delta_Xֵ
static int16_t delta_y = 0;  // ���ڴ洢��ȡ����Delta_Yֵ
static uint16_t x_resolution = 5000;  // Ĭ��5000 CPI
static uint16_t y_resolution = 5000;  // Ĭ��5000 CPI
static uint16_t run_downshift_mult = 256;  // Ĭ��ֵΪ256����Ӧ�Ĵ���ֵ0x07
static uint16_t rest1_downshift_mult = 64;	// Ĭ��ֵΪ64����Ӧ�Ĵ���ֵ0x05
static uint16_t rest2_downshift_mult = 64;	// Ĭ��ֵΪ64����Ӧ�Ĵ���ֵ0x05

/****************************************
 *             SPIM����                  *
 ****************************************/
static const nrfx_spim_t spim = NRFX_SPIM_INSTANCE(SPIM_INSTANCE);  /**< ���� SPIM ʵ�� */
static volatile bool spim_xfer_done;  /**< ��־λ������ָʾ SPIM �����Ƿ���� */
static uint8_t m_tx_buf[2];  /**< ���ͻ��������洢1�ֽڵ�ַ+1�ֽ����� */
static uint8_t m_rx_buf[13];  /**< ���ջ��������洢���յ���12�ֽ����� */

/***************************************
 *         ��������ʼ������             *
 ***************************************/
/**
 * @brief ��ʼ��������
 * 
 */
void paw3399_init(void)
{
    paw3399_spim_init();  // ����SPI��ʼ������
		paw3399_gpio_init();  // ����GPIO��ʼ������
		paw3399_print_pin_info(); // ��ӡ������Ϣ
		paw3399_power_up(); // ��ʼ��PAW3399-T4QU
		paw3399_high_performance_mode();	// ����ΪGaming Mode
		paw3399_set_res_mod_different_src(false); // ����XY�ֱ���ͬԴ
		paw3399_set_xy_resolution(5000, 5000); // ���ú��ݷֱ���
		
		// paw3399_test();			// ����3399����
}

/****************************************
 *            ������ȡ����               *
 ****************************************/
/**
 * @brief  ��ȡ�洢�� motion_burst_data �е� Motion Burst ���ݡ�
 *
 * �ú�������ָ��̬�洢�� motion_burst_data �����ָ�롣
 * motion_burst_data �Ǵ洢�� PAW3399 ��������ȡ�� Motion Burst ���ݵĻ�������
 *
 * @return uint8_t* ����һ��ָ�� motion_burst_data �����ָ�롣
 *                   ��������� 12 �ֽڵ� Motion Burst ���ݡ�
 *
 * @note   �����߿���ֱ��ͨ�����ص�ָ����� motion_burst_data �����ݣ�
 *         ����Ҫע����ǣ��޸Ĵ����ݽ�ֱ��Ӱ���ڲ��Ĵ洢��
 *         �����Ҫ��������һ���ԣ��������ⲿ�������ݿ�����
 */
uint8_t* paw3399_get_var_motion_burst_data(void)
{
    return motion_burst_data;
}

/**
 * @brief ��ȡ�洢�� raw_data �����е� Raw Data ���ݡ�
 *
 * �ú�������ָ��洢�� raw_data �����ָ�롣
 * raw_data �Ǵ洢���������豸ԭʼ���ݵĻ�������
 *
 * @return uint8_t* ����һ��ָ�� raw_data �����ָ�롣
 *                  ��������� 1295 �ֽڵ�ԭʼ���ݡ�
 */
uint8_t* paw3399_get_var_raw_data(void)
{
    return raw_data;
}

/**
 * @brief ��ȡ̧���ж�У׼����A��ֵ��
 *
 * �ú������ش洢�� var_a �е�̧���ж�У׼����A��ֵ��
 *
 * @return uint8_t ���� var_a ��ֵ��
 */
uint8_t paw3399_get_var_var_a(void)
{
    return var_a;
}

/**
 * @brief ��ȡ̧���ж�У׼����B��ֵ��
 *
 * �ú������ش洢�� var_b �е�̧���ж�У׼����B��ֵ��
 *
 * @return uint8_t ���� var_b ��ֵ��
 */
uint8_t paw3399_get_var_var_b(void)
{
    return var_b;
}

/**
 * @brief ��ȡ��ǰ��ȡ�� Delta_X ֵ��
 *
 * �ú������ش洢�� delta_x �е� X ��仯ֵ��
 *
 * @return int16_t ���� delta_x ��ֵ��
 */
int16_t paw3399_get_var_delta_x(void)
{
    return delta_x;
}

/**
 * @brief ��ȡ��ǰ��ȡ�� Delta_Y ֵ��
 *
 * �ú������ش洢�� delta_y �е� Y ��仯ֵ��
 *
 * @return int16_t ���� delta_y ��ֵ��
 */
int16_t paw3399_get_var_delta_y(void)
{
    return delta_y;
}

/**
 * @brief ��ȡ X ��ֱ��� (CPI)��
 *
 * �ú������ش洢�� x_resolution �е� X ��ֱ��ʣ�Ĭ��ֵΪ 5000 CPI��
 *
 * @return uint16_t ���� x_resolution ��ֵ��
 */
uint16_t paw3399_get_var_x_resolution(void)
{
    return x_resolution;
}

/**
 * @brief ��ȡ Y ��ֱ��� (CPI)��
 *
 * �ú������ش洢�� y_resolution �е� Y ��ֱ��ʣ�Ĭ��ֵΪ 5000 CPI��
 *
 * @return uint16_t ���� y_resolution ��ֵ��
 */
uint16_t paw3399_get_var_y_resolution(void)
{
    return y_resolution;
}

/**
 * @brief ��ȡ����״̬�µĽ���ϵ�� (Downshift Multiplier)��
 *
 * �ú������ش洢�� run_downshift_mult �еĽ���ϵ����Ĭ��ֵΪ 256��
 *
 * @return uint16_t ���� run_downshift_mult ��ֵ��
 */
uint16_t paw3399_get_var_run_downshift_mult(void)
{
    return run_downshift_mult;
}

/**
 * @brief ��ȡ���� 1 ״̬�µĽ���ϵ�� (Downshift Multiplier)��
 *
 * �ú������ش洢�� rest1_downshift_mult �еĽ���ϵ����Ĭ��ֵΪ 64��
 *
 * @return uint16_t ���� rest1_downshift_mult ��ֵ��
 */
uint16_t paw3399_get_var_rest1_downshift_mult(void)
{
    return rest1_downshift_mult;
}

/**
 * @brief ��ȡ���� 2 ״̬�µĽ���ϵ�� (Downshift Multiplier)��
 *
 * �ú������ش洢�� rest2_downshift_mult �еĽ���ϵ����Ĭ��ֵΪ 64��
 *
 * @return uint16_t ���� rest2_downshift_mult ��ֵ��
 */
uint16_t paw3399_get_var_rest2_downshift_mult(void)
{
    return rest2_downshift_mult;
}

/****************************************
 *           �����ʼ������              *
 ****************************************/
/**
 * @brief SPIM �û��¼���������
 *
 * �� SPIM �������ʱ���ú��������ã�������յ������ݲ������־��
 *
 * @param[in] p_event  SPIM �¼��ṹָ�롣
 * @param[in] p_context �û������ģ�δʹ�ã���
 */
void spim_event_handler(nrfx_spim_evt_t const * p_event, void * p_context)
{
    if (p_event->type == NRFX_SPIM_EVENT_DONE)
    {
        spim_xfer_done = true;
    }
}

/**
 * @brief ��ʼ��GPIO����
 * 
 * �ú������ڳ�ʼ��GPIO����
 */
void paw3399_gpio_init(void)
{
		// ��ʼ�� GPIO ����
    nrf_gpio_cfg_output(NRESET_PIN);  // NRESET ��������Ϊ���
		nrf_gpio_cfg_input(MOTION_PIN, NRF_GPIO_PIN_PULLUP); // ����Motion Pin
		nrf_gpio_pin_clear(NRESET_PIN);  // Ĭ�Ͻ� NRESET ��������
}

/**
 * @brief ��ʼ�� SPIM
 *
 * �ú���������� SPIM �ĳ�ʼ�������������������ú��¼���������
 */
void paw3399_spim_init(void)
{
    nrfx_spim_config_t spim_config = NRFX_SPIM_DEFAULT_CONFIG;
    spim_config.frequency   = NRF_SPIM_FREQ_8M;    // 16 MHz Ƶ��
    spim_config.ss_pin   = SPI_SS_PIN;    // Ƭѡ����
    spim_config.miso_pin = SPI_MISO_PIN;  // MISO ����
    spim_config.mosi_pin = SPI_MOSI_PIN;  // MOSI ����
    spim_config.sck_pin  = SPI_SCK_PIN;   // ʱ������

    APP_ERROR_CHECK(nrfx_spim_init(&spim, &spim_config, spim_event_handler, NULL));  // ��ʼ�� SPIM
}

/****************************************
 *           ������״̬����              *
 ****************************************/
/**
 * @brief PAW3399��λ����
 * 
 * �ú���ͨ������NRESET���Ŷ�PAW3399���и�λ��������NRESET�������ʹ�����λ��
 * Ȼ�����߽�����λ���������̻���1�������ʱ��ȷ���ȶ��ԡ�
 */
void paw3399_reset(void)
{
    // ��NRESET�������ͣ������豸��λ
    nrf_gpio_pin_clear(NRESET_PIN);
    nrf_delay_ms(1);  // ����NRESETΪ�͵�ƽ1����

    // ��NRESET�������ߣ�������λ
    nrf_gpio_pin_set(NRESET_PIN);
	
		nrf_delay_ms(50); // �ȴ�50ms
}

/**
 * @brief PAW3399��Դ��������
 * 
 * �ú���ͨ��SPIЭ�����ö���Ĵ��������PAW3399�ĵ�Դ�����ͳ�ʼ�����̡�
 * ��������SPI���ߵĸ�λ�������������Ĵ���д���ض��ĳ�ʼ��ֵ��
 * �����ָ���ĳ��Դ�����δ�ܳɹ���ȡ���Ĵ��� 0x6C ������ֵ��0x80����
 * ��ִ�лָ��������������üĴ�����
 */
void paw3399_power_up(void)
{
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("Start Paw3399 Power Up");
		nrf_gpio_pin_set(NRESET_PIN);

		// �ȴ�����50����
		nrf_delay_ms(50);  // ��ʱ50����
	
		// ����SPI�˿�
		nrf_gpio_pin_set(SPI_SS_PIN);   
    nrf_delay_ms(1);                // ��ʱ1ms����ֹ��ƽ�仯���죩
    nrf_gpio_pin_clear(SPI_SS_PIN); 
		nrf_delay_ms(1);                // ��ʱ1ms����ֹ��ƽ�仯���죩
		
		// ��0x5Aд��Power_Up_Reset�Ĵ���
		paw3399_write(0x3A, 0x5A);
		nrf_delay_ms(5);  // ��ʱ5����
	
		// �����ϵ��ʼ���Ĵ�������
		
		paw3399_write(0x40, 0x80);
		paw3399_write(0x7F, 0x0E);
		paw3399_write(0x55, 0x0D);
		paw3399_write(0x56, 0x1B);
		paw3399_write(0x57, 0xE8);
		paw3399_write(0x58, 0xD5);
		paw3399_write(0x7F, 0x14);
		paw3399_write(0x42, 0xBC);
		paw3399_write(0x43, 0x74);
		paw3399_write(0x4B, 0x20);
		paw3399_write(0x4D, 0x00);
		paw3399_write(0x53, 0x0D);
		paw3399_write(0x7F, 0x05);
		paw3399_write(0x51, 0x40);
		paw3399_write(0x53, 0x40);
		paw3399_write(0x55, 0xCA);
		paw3399_write(0x61, 0x31);
		paw3399_write(0x62, 0x64);
		paw3399_write(0x6D, 0xB8);
		paw3399_write(0x6E, 0x0F);
		paw3399_write(0x70, 0x02);
		paw3399_write(0x4A, 0x2A);
		paw3399_write(0x60, 0x26);
		paw3399_write(0x7F, 0x06);
		paw3399_write(0x6D, 0x70);
		paw3399_write(0x6E, 0x60);
		paw3399_write(0x6F, 0x04);
		paw3399_write(0x53, 0x02);
		paw3399_write(0x55, 0x11);
		paw3399_write(0x7D, 0x51);
		paw3399_write(0x7F, 0x08);
		paw3399_write(0x71, 0x4F);
		paw3399_write(0x7F, 0x09);
		paw3399_write(0x62, 0x1F);
		paw3399_write(0x63, 0x1F);
		paw3399_write(0x65, 0x03);
		paw3399_write(0x66, 0x03);
		paw3399_write(0x67, 0x1F);
		paw3399_write(0x68, 0x1F);
		paw3399_write(0x69, 0x03);
		paw3399_write(0x6A, 0x03);
		paw3399_write(0x6C, 0x1F);
		paw3399_write(0x6D, 0x1F);
		paw3399_write(0x51, 0x04);
		paw3399_write(0x53, 0x20);
		paw3399_write(0x54, 0x20);
		paw3399_write(0x71, 0x0F);
		paw3399_write(0x72, 0x0A);
		paw3399_write(0x7F, 0x0A);
		paw3399_write(0x4A, 0x14);
		paw3399_write(0x4C, 0x14);
		paw3399_write(0x55, 0x19);
		paw3399_write(0x7F, 0x14);
		paw3399_write(0x63, 0x16);
		paw3399_write(0x7F, 0x0C);
		paw3399_write(0x41, 0x30);
		paw3399_write(0x55, 0x14);
		paw3399_write(0x49, 0x0A);
		paw3399_write(0x42, 0x00);
		paw3399_write(0x44, 0x0D);
		paw3399_write(0x4A, 0x12);
		paw3399_write(0x4B, 0x09);
		paw3399_write(0x4C, 0x30);
		paw3399_write(0x5A, 0x0D);
		paw3399_write(0x5F, 0x1E);
		paw3399_write(0x5B, 0x05);
		paw3399_write(0x5E, 0x0F);
		paw3399_write(0x7F, 0x0D);
		paw3399_write(0x48, 0xDD);
		paw3399_write(0x4F, 0x03);
		paw3399_write(0x5A, 0x29);
		paw3399_write(0x5B, 0x47);
		paw3399_write(0x5C, 0x81);
		paw3399_write(0x5D, 0x40);
		paw3399_write(0x71, 0xDC);
		paw3399_write(0x70, 0x07);
		paw3399_write(0x73, 0x00);
		paw3399_write(0x72, 0x08);
		paw3399_write(0x75, 0xDC);
		paw3399_write(0x74, 0x07);
		paw3399_write(0x77, 0x00);
		paw3399_write(0x76, 0x08);
		paw3399_write(0x7F, 0x10);
		paw3399_write(0x4C, 0xD0);
		paw3399_write(0x7F, 0x00);
		paw3399_write(0x4F, 0x63);
		paw3399_write(0x4E, 0x00);
		paw3399_write(0x52, 0x63);
		paw3399_write(0x51, 0x00);
		paw3399_write(0x5A, 0x10);
		paw3399_write(0x77, 0x4F);
		paw3399_write(0x47, 0x01);
		paw3399_write(0x5B, 0x40);
		paw3399_write(0x66, 0x13);
		paw3399_write(0x67, 0x0F);
		paw3399_write(0x78, 0x01);
		paw3399_write(0x79, 0x9C);
		paw3399_write(0x55, 0x02);
		paw3399_write(0x23, 0x70);
		paw3399_write(0x22, 0x01);
		nrf_delay_ms(1); 
		
    // ʹ�� for ѭ������ೢ�� 60 �Σ�ÿ�μ�� 1 ����
		uint8_t reg_value = 0;
    for (uint8_t attempt = 0; attempt < 60; attempt++)
    {
        // ��ȡ�Ĵ��� 0x6C
        reg_value = paw3399_read(0x6C);

        // ����Ƿ��ȡ�� 0x80
        if (reg_value == 0x80)
        {
            NRF_LOG_INFO("Register 0x6C value 0x80 obtained after %d attempts.", attempt + 1);
            break;  // �ɹ���ȡ�����˳�ѭ��
        }

        // ��ʱ 1 ���룬ȷ��ÿ�ζ�ȡ�ļ��
        nrf_delay_ms(1);
    }

    // ��� 60 �κ�û�ж�ȡ�� 0x80��ִ��д�������лָ�
    if (reg_value != 0x80)
    {
        NRF_LOG_INFO("Value 0x80 not obtained, executing alternate sequence.");
        paw3399_write(0x7F, 0x14);
        paw3399_write(0x6C, 0x00);
        paw3399_write(0x7F, 0x00);
    }
		
		paw3399_write(0x22, 0x00);
    paw3399_write(0x55, 0x00);
    paw3399_write(0x7F, 0x00);
    paw3399_write(0x40, 0x00);
		
		// ��ȡ�Ĵ��� 0x02, 0x03, 0x04, 0x05, 0x06 ����ӡ��ȡ����ֵ
    uint8_t reg_value_02 = paw3399_read(0x02);
    uint8_t reg_value_03 = paw3399_read(0x03);
    uint8_t reg_value_04 = paw3399_read(0x04);
    uint8_t reg_value_05 = paw3399_read(0x05);
    uint8_t reg_value_06 = paw3399_read(0x06);

    // ��ӡ��ȡ����ֵ
    NRF_LOG_INFO("Register Motion    0x02: 0x%02x", reg_value_02);
    NRF_LOG_INFO("Register Delta_X_L 0x03: 0x%02x", reg_value_03);
    NRF_LOG_INFO("Register Delta_X_H 0x04: 0x%02x", reg_value_04);
    NRF_LOG_INFO("Register Delta_Y_L 0x05: 0x%02x", reg_value_05);
    NRF_LOG_INFO("Register Delta_Y_H 0x06: 0x%02x", reg_value_06);
		
		// ��ȡ����ӡ��������Ϣ
    uint8_t product_id = paw3399_get_product_id();  // ��ȡProduct ID
		uint8_t inv_product_id = paw3399_get_inv_product_id();  // ��ȡINV Product ID
    uint8_t revision_id = paw3399_get_revision_id(); // ��ȡRevision ID

    NRF_LOG_INFO("PAW3399 Product ID: 0x%02x", product_id);
		NRF_LOG_INFO("PAW3399 Inv Product ID: 0x%02x", inv_product_id);
    NRF_LOG_INFO("PAW3399 Revision ID: 0x%02x", revision_id);
		
		// �������״̬
		paw3399_print_status();
		
		NRF_LOG_INFO("PAW3399 Power Up Successfully.");
		NRF_LOG_INFO("/*******************************/");
}

/**
 * @brief ���� PAW3399 �ػ�ģʽ��
 * 
 * �ú�����оƬ���ڹػ�ģʽ���ڹػ�ģʽ�£�SPI ͨ�ű����ԣ�ֱ�������ϵ����
 * 
 * @note ����ʱ��ϳ����ػ���Ӧ����������������ĵ�Դ����
 * 
 * @param none �޲���
 */
void paw3399_shutdown(void)
{
    // д��ػ��Ĵ��� 0x3B��ֵΪ 0xB6 �Խ���ػ�ģʽ
    paw3399_write(0x3B, 0xB6);
		nrf_delay_ms(500); 
}

/**
 * @brief �˳� PAW3399 �ػ�ģʽ�������ϵ硣
 * 
 * �ú����������ϵ� PAW3399������ػ�ģʽ�������ڶ��� NCS �͵�ƽ��
 * ��Ĵ��� 0x3A д�� 0x5A ���ָܻ�оƬ�Ĺ��ܡ�
 * 
 * @note �����ϵ�ʱ NCS ���뱣�ֵ͵�ƽ��MISO ��������������͹���Ҫ��
 * 
 * @param none �޲���
 */
void paw3399_power_up_from_shutdown(void)
{
    // NCS ����
    nrf_gpio_pin_clear(SPI_SS_PIN);
		nrf_delay_ms(1);

    // д���ϵ�����Ĵ��� 0x3A��ֵΪ 0x5A
    paw3399_write(0x3A, 0x5A);
    
    // �ȴ�һ��ʱ��ȷ�������ϵ�
    nrf_delay_ms(50); // �ȴ�50ms��ȷ����Դ�ϵ��ȶ�
    
    // NCS ����
    nrf_gpio_pin_set(SPI_SS_PIN);
	
		// �������״̬
		paw3399_print_status();
}

/**
 * @brief ���CHIP_OBSERVATION�Ĵ���ֵ�Ƿ�����
 *
 * �ú���ͨ��д��0x00���CHIP_OBSERVATION�Ĵ������ȴ��ʵ����ӳٺ��ȡ�Ĵ�����ֵ��
 * ��������Ƿ�Ϊ0xB7��0xBF��
 * 
 * @return bool ����true��ʾ�Ĵ���ֵ���������򷵻�false��
 */
bool paw3399_chip_observation(void)
{
    // ����Ĵ�����д��0x00
    paw3399_write(0x15, 0x00);
	
		uint16_t t_dly_obs = paw3399_get_rest3_period() + 50;

    // �ȴ�T_dly_obs����ʾ�֡���� + 10% �ı仯���֡���ڷ�����оƬ���� Rest3 ģʽʱ����Ҫ����ʱ��Ƶ�ʵ��ݲ�ֵ
    nrf_delay_ms(t_dly_obs);  // ������������������ӳ�Ϊ504ms + 50ms = 554ms��Ĭ�ϣ�

    // ��ȡ�Ĵ���ֵ
    uint8_t observation_value = paw3399_read(0x15);

    // ���ֵ�Ƿ�Ϊ0xB7��0xBF
    if (observation_value == 0xB7 || observation_value == 0xBF)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief ��ȡоƬ��ǰ����ģʽ
 * 
 * ���MOTION�Ĵ����е�OP_Mode1:0λ�����ص�ǰ����ģʽ��
 * 
 * @return uint8_t ����ֵ��ʾ����ģʽ��
 *         00 - Run Mode
 *         01 - Rest 1
 *         10 - Rest 2
 *         11 - Rest 3
 */
uint8_t paw3399_get_op_mode(void)
{
    uint8_t motion = paw3399_read(0x02);  // ��ȡMOTION�Ĵ���

    // ����OP_Mode1:0 (bit 1:0)��ֵ
    uint8_t op_mode = motion & 0x03;

    return op_mode;
}

/****************************************
 *           ������ģʽ����              *
 ****************************************/
/**
 * @brief �л���������ģʽ (High Performance Mode)��
 *
 * �ú������� PAW3399 ���������ģʽ��������ģʽ��Ĭ������ʱ��Ϊ����״̬��
 * �ʺ���Ҫ�ϸ���Ӧ�����ܵ�ʹ�ó�����
 */
void paw3399_high_performance_mode(void)
{
		// д�Ĵ���											
    paw3399_write(0x7F, 0x05);
    paw3399_write(0x51, 0x40);
    paw3399_write(0x53, 0x40);
    paw3399_write(0x61, 0x31);
    paw3399_write(0x6E, 0x0F);
    paw3399_write(0x7F, 0x07);
    paw3399_write(0x42, 0x32);
		paw3399_write(0x43, 0x00);
    paw3399_write(0x7F, 0x0D);
    paw3399_write(0x51, 0x00);
    paw3399_write(0x52, 0x49);
    paw3399_write(0x53, 0x00);
    paw3399_write(0x54, 0x5B);
    paw3399_write(0x55, 0x00);
    paw3399_write(0x56, 0x64);
    paw3399_write(0x57, 0x02);
    paw3399_write(0x58, 0xA5);
    paw3399_write(0x7F, 0x14);
    paw3399_write(0x63, 0x16);
    paw3399_write(0x7F, 0x00);
    paw3399_write(0x54, 0x54);
    paw3399_write(0x78, 0x01);
    paw3399_write(0x79, 0x9C);
    paw3399_write_with_mask(0x40, 0x03, 0x00); // �޸�bit[1:0]
}

/**
 * @brief �л����͹���ģʽ (Low Power Mode)��
 *
 * �ú������� PAW3399 ����͹���ģʽ���ڵ͹���ģʽ�£�оƬ���ܺĽ��ͣ�
 * �ʺϲ���Ҫ�����ܵĳ���������ܺʹ���ģʽ��
 *
 */
void paw3399_low_power_mode(void)
{	
		// д�Ĵ���
    paw3399_write(0x7F, 0x05);
    paw3399_write(0x51, 0x40);
    paw3399_write(0x53, 0x40);
    paw3399_write(0x61, 0x3B);
    paw3399_write(0x6E, 0x1F);
    paw3399_write(0x7F, 0x07);
    paw3399_write(0x42, 0x32);
		paw3399_write(0x43, 0x00);
    paw3399_write(0x7F, 0x0D);
    paw3399_write(0x51, 0x00);
    paw3399_write(0x52, 0x49);
    paw3399_write(0x53, 0x00);
    paw3399_write(0x54, 0x5B);
    paw3399_write(0x55, 0x00);
    paw3399_write(0x56, 0x64);
    paw3399_write(0x57, 0x02);
    paw3399_write(0x58, 0xA5);
    paw3399_write(0x7F, 0x14);
    paw3399_write(0x63, 0x16);
    paw3399_write(0x7F, 0x00);
    paw3399_write(0x54, 0x54);
    paw3399_write(0x78, 0x01);
    paw3399_write(0x79, 0x9C);
    paw3399_write_with_mask(0x40, 0x03, 0x01); // �޸�bit[1:0]
}

/**
 * @brief �л����칫ģʽ (Office Mode)��
 *
 * �ú������� PAW3399 ����칫ģʽ���칫ģʽ�£�оƬ�����ƽ��Ĺ��ĺ����ܿ��ƣ�
 * �ʺ���ͨʹ�ó��������ĵ��������Ȳ�����
 *
 */
void paw3399_office_mode(void)
{
		// д�Ĵ���
    paw3399_write(0x7F, 0x05);
    paw3399_write(0x51, 0x28);
    paw3399_write(0x53, 0x30);
    paw3399_write(0x61, 0x3B);
    paw3399_write(0x6E, 0x1F);
    paw3399_write(0x7F, 0x07);
    paw3399_write(0x42, 0x32);
		paw3399_write(0x43, 0x00);
    paw3399_write(0x7F, 0x0D);
    paw3399_write(0x51, 0x00);
    paw3399_write(0x52, 0x49);
    paw3399_write(0x53, 0x00);
    paw3399_write(0x54, 0x5B);
    paw3399_write(0x55, 0x00);
    paw3399_write(0x56, 0x64);
    paw3399_write(0x57, 0x02);
    paw3399_write(0x58, 0xA5);
    paw3399_write(0x7F, 0x14);
    paw3399_write(0x63, 0x16);
    paw3399_write(0x7F, 0x00);
    paw3399_write(0x54, 0x52);
    paw3399_write(0x78, 0x0A);
    paw3399_write(0x79, 0x0F);
    paw3399_write_with_mask(0x40, 0x03, 0x02); // �޸�bit[1:0]
}

/**
 * @brief �л���������Ϸģʽ (Corded Gaming Mode)��
 *
 * �ú������� PAW3399 ����������Ϸģʽ����ģʽ������Ҫ�������Ӧ�͸����ܵ���Ϸ������
 * 
 */
void paw3399_corded_gaming_mode(void)
{
    paw3399_write(0x7F, 0x05);
    paw3399_write(0x51, 0x40);
    paw3399_write(0x53, 0x40);
    paw3399_write(0x61, 0x31);
    paw3399_write(0x6E, 0x0F);
    paw3399_write(0x7F, 0x07);
    paw3399_write(0x42, 0x2F);
    paw3399_write(0x43, 0x00);
    paw3399_write(0x7F, 0x0D);
    paw3399_write(0x51, 0x12);
    paw3399_write(0x52, 0xDB);
    paw3399_write(0x53, 0x12);
    paw3399_write(0x54, 0xDC);
    paw3399_write(0x55, 0x12);
    paw3399_write(0x56, 0xE4);
    paw3399_write(0x57, 0x15);
    paw3399_write(0x58, 0x2D);
    paw3399_write(0x7F, 0x14);
    paw3399_write(0x63, 0x1E);
    paw3399_write(0x7F, 0x00);
    paw3399_write(0x54, 0x55);
    paw3399_write(0x40, 0x83); 
}

/**
 * @brief ��ȡ������PAW3399�ĵ�ǰ����ģʽ��
 *
 * �ú���ͨ����ȡ�Ĵ���0x40��bit[1:0]���ж�PAW3399�ĵ�ǰ����ģʽ��
 * 
 * @return uint8_t ���ص�ǰ����ģʽ�ļĴ���ֵ��
 */
uint8_t paw3399_get_mode(void)
{
    return paw3399_read(0x40) & 0x03;
}

/****************************************
 *           ��������������              *
 ****************************************/
/**
 * @brief ִ��һ�� SPIM ������
 *
 * �ú�������7λ�Ĵ�����ַ���Ӹõ�ַ��ȡ8λ���ݡ�
 *
 * @param[in] address  7λ�Ĵ�����ַ
 * @return uint8_t     �ӼĴ�����ַ��ȡ����8λ����
 */
uint8_t paw3399_read(uint8_t address)
{
    uint8_t read_address = address & 0x7F;  // ��Ϊ������
    m_tx_buf[0] = read_address;

    spim_xfer_done = false;
    nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TRX(m_tx_buf, 2, m_rx_buf, 2);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spim, &xfer_desc, 0));

    while (!spim_xfer_done)
    {
        __WFE();
    }

    uint8_t read_value = m_rx_buf[1];

    return read_value;
}

/**
 * @brief ִ��һ�� SPIM д����
 *
 * �ú�������7λ�Ĵ�����ַ��8λ���ݣ�������д�뵽ָ����ַ��
 *
 * @param[in] address  7λ�Ĵ�����ַ
 * @param[in] value    8λ����
 */
void paw3399_write(uint8_t address, uint8_t value)
{
    uint8_t write_address = address | 0x80;  // ��Ϊд����
    m_tx_buf[0] = write_address;
    m_tx_buf[1] = value;

    spim_xfer_done = false;
    nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TX(m_tx_buf, 2);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spim, &xfer_desc, 0));

    while (!spim_xfer_done)
    {
        __WFE();
    }
}

/**
 * @brief ִ��һ�� Motion Burst ��ȡ������
 *
 * �ú����� PAW3399 ���������� Motion Burst �����ַ (0x16)�����Ӵ������ж�ȡ12�ֽڵ��˶����ݡ�
 * ������ɺ󣬶�ȡ�����ݱ��洢�ڽ��ջ���������ӡ������
 *
 * @param[out] buffer ���ڴ洢��ȡ�� 12 �ֽ����ݵĻ�����ָ�롣
 *
 * @details
 * ��ȡ�������ֶΰ�����
 * - BYTE[00]: Motion
 * - BYTE[01]: Observation
 * - BYTE[02]: Delta_X_L
 * - BYTE[03]: Delta_X_H
 * - BYTE[04]: Delta_Y_L
 * - BYTE[05]: Delta_Y_H
 * - BYTE[06]: SQUAL
 * - BYTE[07]: RawData_Sum
 * - BYTE[08]: Maximum_RawData
 * - BYTE[09]: Minimum_Rawdata
 * - BYTE[10]: Shutter_Upper
 * - BYTE[11]: Shutter_Lower
 *
 * @note �˺������� SPI �ӿ��Ѿ���ʼ�������Ҵ������Ѵ��ڹ���״̬��
 *
 * @return void
 */
void paw3399_motion_burst(void)
{
		// ����Motion_Burst��ַ(0x16)
    m_tx_buf[0] = 0x16;
    spim_xfer_done = false;
    nrfx_spim_xfer_desc_t xfer_desc_tx = NRFX_SPIM_XFER_TRX(m_tx_buf, 1, m_rx_buf, 13);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spim, &xfer_desc_tx, 0));
	
		// �ȴ��������
    while (!spim_xfer_done)
    {
        __WFE();
    }
		
		for (int i = 0; i < 12; i++) 
		{
				motion_burst_data[i] = m_rx_buf[i+1];
		}
		
		// �ȴ� tBEXIT ʱ��
    nrf_delay_us(1);

}

/**
 * @brief ��ȡ�Ĵ�����д���޸ĺ��ֵ
 *
 * �ú����ȶ�ȡָ���Ĵ����ĵ�ǰֵ�����ݸ�����λ�����޸��ض���λ��
 * �����޸ĺ��ֵд�ؼĴ�����
 *
 * @param[in] reg_address �Ĵ�����ַ
 * @param[in] mask λ���룬����ָ��Ҫ�޸ĵ�λ
 * @param[in] value ��ֵ�����������������޸��ض�λ
 */
void paw3399_write_with_mask(uint8_t address, uint8_t mask, uint8_t value)
{
    // ��ȡ�Ĵ����ĵ�ǰֵ
    uint8_t current_value = paw3399_read(address);

    // ʹ�����������Ҫ�޸ĵ�λ����������λ����
    current_value &= ~mask;

    // ������Ҫ�޸ĵ�λΪ�µ�ֵ
    current_value |= (value & mask);

    // ���޸ĺ��ֵд�ؼĴ���
    paw3399_write(address, current_value);
}

/****************************************
 *           ��������Ϣ����              *
 ****************************************/
/**
 * @brief ��ӡ���ź���Ϣ
 * 
 * �ú�����ӡMOTION_PIN��NRESET_PIN�Լ�SPI������ţ�MISO��MOSI��SCK��SS�������ź���Ϣ��
 */
void paw3399_print_pin_info(void)
{
    // ��ӡMOTION_PIN����Ϣ
		NRF_LOG_INFO("/*******************************/");
    NRF_LOG_INFO("MOTION_PIN: %d", MOTION_PIN);

    // ��ӡSPI���ŵ���Ϣ
    NRF_LOG_INFO("SPI_SS_PIN: %d", SPI_SS_PIN);
    NRF_LOG_INFO("SPI_MISO_PIN: %d", SPI_MISO_PIN);
    NRF_LOG_INFO("SPI_MOSI_PIN: %d", SPI_MOSI_PIN);
    NRF_LOG_INFO("SPI_SCK_PIN: %d", SPI_SCK_PIN);
	
		// ��ӡNRESET_PIN����Ϣ
		NRF_LOG_INFO("NRESET_PIN: %d", NRESET_PIN);
		NRF_LOG_INFO("/*******************************/");

    NRF_LOG_FLUSH();  // ˢ����־���
}

/**
 * @brief ��ȡPAW3399�Ĳ�ƷID��Product ID����
 *
 * �ú���ͨ����ȡ�Ĵ���0x00����ȡPAW3399�Ĳ�ƷID������ֵΪ��ID��PID7-0��
 *
 * @return uint8_t ���ز�ƷID��ֵ��
 */
uint8_t paw3399_get_product_id(void)
{
    return paw3399_read(0x00); // ���ز�ƷID
}

 /* @brief ��ȡPAW3399��INV��Ʒ ID��Product ID����
 *
 * �ú���ͨ����ȡ�Ĵ���0x5F����ȡPAW3399�Ĳ�ƷID������ֵΪ��ID��PID0-7��
 *
 * @return uint8_t ���ز�ƷID��ֵ��
 */
uint8_t paw3399_get_inv_product_id(void)
{
    return paw3399_read(0x5F); // ���ز�ƷINV ID
}

/**
 * @brief ��ȡPAW3399���޶��汾ID��Revision ID����
 *
 * �ú���ͨ����ȡ�Ĵ���0x01����ȡPAW3399���޶��汾ID������ֵΪ��ID��PID7-0��
 *
 * @return uint8_t �����޶��汾ID��ֵ��
 */
uint8_t paw3399_get_revision_id(void)
{
    return paw3399_read(0x01); // �����޶��汾ID
}

/****************************************
 *           ������Ϣ����                *
 ****************************************/
/**
 * @brief ����PAW3399����ָ��ִ��RawData������С�
 *
 * �˺���ͨ�����ض��Ĵ���д��ֵ������RawDataץȡ���̣����Ӵ������ж�ȡԭʼ���ݡ�
 * �ú�����ѭ�����ֲ��������Ĳ��裬��ȷ����ȷ��ȡ���ݣ�ͬʱ�������Ĵ���ֵ��ȷ����ȡ������Ч���ݡ�
 *
 * @note ��RawData��������У����뽫������ھ�ֹλ�ã��˺������� SPI �ӿ��Ѿ���ʼ�������Ҵ������Ѵ��ڹ���״̬��
 *
 * @return void
 */
void paw3399_get_rawdata(void)
{
		// д�Ĵ���
    paw3399_write(0x7F, 0x00);
    paw3399_write(0x55, 0x04);
    paw3399_write(0x50, 0x01);
    paw3399_write(0x40, 0x80);

    // ������ȡ�Ĵ��� 0x02 (Motion) ֱ�� OP_Mode1 �� OP_Mode0 ��Ϊ 0
    uint8_t motion;
    do {
        motion = paw3399_read(0x02);

    } while ((motion & 0x03) != 0);
		
		
		// д�Ĵ���
    paw3399_write(0x58, 0xFF);

    // ������ȡ�Ĵ��� 0x59 ֱ�� PG_FIRST �� PG_VALID ��Ϊ 1
    uint8_t status;
    do {
        status = paw3399_read(0x59);
    } while ((status & 0xC0) != 0xC0);

    // ��ȡ�Ĵ��� 0x58 �ĵ�һ�� rawdata
    uint8_t rawdata = paw3399_read(0x58);
    raw_data[0] = rawdata;

    // ������ȡ�Ĵ��� 0x59 ֱ�� PG_VALID Ϊ 1����ȡ 1295 �Σ��γ�������ͼƬԪ������
		for (int i = 1; i < 1296; i++) {
				// ������ȡ�Ĵ��� 0x59��ֱ�� PG_VALID Ϊ 1
				do {
						status = paw3399_read(0x59);
				} while ((status & 0x80) == 0);  // PG_VALID λΪ 1 ��ʾ��Ч����
		
				// ��ȡʵ������
				rawdata = paw3399_read(0x58);
				raw_data[i] = rawdata;
		}

    // д�Ĵ���
    paw3399_write(0x40, 0x00);
    paw3399_write(0x50, 0x00);
    paw3399_write(0x55, 0x00);
}

/**
 * @brief ��ȡRAW������ͼĴ�����ֵ
 * 
 * �ú�����ȡRAW������ͼĴ�����0x08��������оƬ��ƽ������ֵ����ȡ�üĴ���ǰ����Ҫȷ��оƬ��������ģʽ������ģʽӦ���á�
 *
 * @return uint16_t ����ƽ������ֵ�� 
 */
uint16_t paw3399_get_rawdata_sum(void)
{
    return ((paw3399_read(0x08) * 1024) / 1296);
}

/**
 * @brief ��ȡ��ǰ֡�����RawDataֵ
 * 
 * �ú�����ȡMAXIMUM_RAWDATA�Ĵ�������ȡ��ǰ֡�����ԭʼ����ֵ��
 * 
 * @return uint8_t �������RawDataֵ����ΧΪ0-127����
 */
uint8_t paw3399_get_max_rawdata(void)
{
    return paw3399_read(0x09);  // ��ȡMAXIMUM_RAWDATA�Ĵ���
}

/**
 * @brief ��ȡ��ǰ֡����СRawDataֵ
 * 
 * �ú�����ȡMINIMUM_RAWDATA�Ĵ�������ȡ��ǰ֡����Сԭʼ����ֵ��
 * 
 * @return uint8_t ������СRawDataֵ����ΧΪ0-127����
 */
uint8_t paw3399_get_min_rawdata(void)
{
    return paw3399_read(0x0A);  // ��ȡMINIMUM_RAWDATA�Ĵ���
}

/**
 * @brief ��ȡ�����㵱ǰ�� SQUAL��������������
 *
 * �ú�����ȡ SQUAL �Ĵ�����ֵ�������ݼĴ���ֵ�������Ч�ı�������������
 * ��ʽΪ����Ч�������� = SQUAL �Ĵ���ֵ * 4��
 *
 * @return uint16_t ���ر�������������
 */
uint16_t paw3399_get_squal(void)
{
    // ��ȡ SQUAL �Ĵ�����ֵ
    uint8_t squal_value = paw3399_read(0x07);  // SQUAL �Ĵ�����ַΪ 0x07

    // ������Ч��������
    uint16_t feature_count = squal_value * 4;

    return feature_count;
}

/****************************************
 *           XYƫ�ƺ���                 *
 ****************************************/
/**
 * @brief ��ȡ���洢Delta_X��Delta_Y��ֵ
 * 
 * ���MOTλΪ�ߣ����ȡDelta_X��Delta_Yֵ���洢��ȫ�ֱ�����
 * ���MOTλΪ�ͣ���ȫ�ֱ���delta_x��delta_y����Ϊ0��
 */
void paw3399_get_delta_xy(void)
{
    uint8_t motion = paw3399_read(0x02);  // ��ȡMOTION�Ĵ���

    if (motion & 0x80)  // ���MOTλ�Ƿ�Ϊ��
    {
        // ��ȡDelta_X��Delta_Y�Ĵ���
        uint8_t delta_x_l = paw3399_read(0x03);
        uint8_t delta_x_h = paw3399_read(0x04);
        uint8_t delta_y_l = paw3399_read(0x05);
        uint8_t delta_y_h = paw3399_read(0x06);
        
        // �ϲ��ߵ��ֽڣ����洢��ȫ�ֱ���
        delta_x = (int16_t)((delta_x_h << 8) | delta_x_l);
        delta_y = (int16_t)((delta_y_h << 8) | delta_y_l);

        // �ж�delta_x�Ƿ��쳣
        if (delta_x > 200)
        {
            delta_x = 0;
        }
        else if (delta_x < -200)
        {
            delta_x = 0;
        }

        // �ж�delta_y�Ƿ��쳣
        if (delta_y > 200)
        {
            delta_y = 0;
        }
        else if (delta_y < -200)
        {
            delta_y = 0;
        }
    }
    else
    {
        // ���MOTλΪ�ͣ���Delta_X��Delta_Y����Ϊ0
        delta_x = 0;
        delta_y = 0;
    }
}


/****************************************
 *           CPI/DPI����                *
 ****************************************/
/**
 * @brief ��ȡXY���CPI�ֱ���
 * 
 * ��ȡRESOLUTION_X��RESOLUTION_Y�Ĵ���������X��Y���CPI�ֱ��ʡ�
 * 
 * 
 * @return void
 */
void paw3399_get_xy_resolution(void)
{
    // ��ȡX��ֱ���
    uint8_t res_x_low = paw3399_read(0x48);
    uint8_t res_x_high = paw3399_read(0x49);
    x_resolution = ((res_x_high << 8) | res_x_low) * 50;

    // ��ȡY��ֱ���
    uint8_t res_y_low = paw3399_read(0x4A);
    uint8_t res_y_high = paw3399_read(0x4B);
    y_resolution = ((res_y_high << 8) | res_y_low) * 50;
}

/**
 * @brief ����XY���CPI�ֱ���
 * 
 * ����X��Y���CPI�ֱ��ʡ���鷶Χ�Ƿ���Ч���Ƿ���50�ı����������ó���9000 CPI��������Ripple Control��
 * 
 * @param[in] new_x_resolution �µ�X��ֱ��ʣ���λΪCPI��
 * @param[in] new_y_resolution �µ�Y��ֱ��ʣ���λΪCPI��
 * 
 * @return ���óɹ�����true������false
 */
bool paw3399_set_xy_resolution(uint16_t new_x_resolution, uint16_t new_y_resolution)
{
    // ��������Ƿ�Ϊ50�ı���
    if (new_x_resolution % 50 != 0 || new_y_resolution % 50 != 0)
    {
        return false;
    }

    // ��鷶Χ�Ƿ���50��20000 CPI֮��
    if (new_x_resolution < 50 || new_x_resolution > 20000 || new_y_resolution < 50 || new_y_resolution > 20000)
    {
        return false;
    }
		
		// ��ȡRES_Mod���ã��ж�XY�Ƿ�ͬԴ
    bool different_src = paw3399_get_res_mod_different_src();
		
		// ���XYͬԴ��RES_Mod = 0������ǿ�ƽ�X�ֱ��ʸ�ֵ��Y�ֱ���
    if (!different_src)
    {
        new_y_resolution = new_x_resolution;
    }

    // ����Ĵ���ֵ
    uint8_t res_x_low = (new_x_resolution / 50) & 0xFF;
    uint8_t res_x_high = (new_x_resolution / 50) >> 8;
    uint8_t res_y_low = (new_y_resolution / 50) & 0xFF;
    uint8_t res_y_high = (new_y_resolution / 50) >> 8;

    // д��X���Y��ֱ���
    paw3399_write(0x48, res_x_low);  // д��X��λ
    paw3399_write(0x49, res_x_high); // д��X��λ
    paw3399_write(0x4A, res_y_low);  // д��Y��λ
    paw3399_write(0x4B, res_y_high); // д��Y��λ

    // �����·ֱ�������
    paw3399_write(0x47, 0x01);  // д��SET_RES�Ĵ���

    // ����ȫ�ֱ�����ͨ�����ò����޸�
    x_resolution = new_x_resolution;
    y_resolution = new_y_resolution;

    // ����Ripple Control����ֱ��ʴ���9000 CPI
    if (new_x_resolution >= 9000 || new_y_resolution >= 9000)
    {
        paw3399_set_ripple_control(true);  // ����Ripple Control
    }
    else
    {
        paw3399_set_ripple_control(false);   // ����Ripple Control
    }

    return true;
}

/**
 * @brief ��ȡ RES_Mod ��״̬
 * 
 * �ú�����ȡ MOTION_CTRL �Ĵ��������� RES_Mod �ĵ�ǰ���á�
 * 
 * @return bool ���� true ��ʾ X �� X �� Y �� Y ���壬false ��ʾ X �� Y ���� X ���塣
 */
bool paw3399_get_res_mod_different_src(void)
{
    uint8_t motion_ctrl = paw3399_read(0x5C);  // ��ȡ MOTION_CTRL �Ĵ���
    return (motion_ctrl & (1 << 1)) ? true : false;  // �ж� RES_Mod λ�Ƿ�Ϊ 1
}

/**
 * @brief ���� RES_Mod ��״̬
 * 
 * �ú������� MOTION_CTRL �Ĵ����� RES_Mod λ��
 * 
 * @param[in] res_mod ����ֵ��true ��ʾ X �� X �� Y �� Y ���壬false ��ʾ X �� Y ���� X ���塣
 */
void paw3399_set_res_mod_different_src(bool different_src)
{
    uint8_t motion_ctrl = paw3399_read(0x5C);  // ��ȡ��ǰ�Ĵ���ֵ

    if (different_src)
    {
        motion_ctrl |= (1 << 1);  // ���� RES_Mod Ϊ 1
    }
    else
    {
        motion_ctrl &= ~(1 << 1);  // ��� RES_Mod Ϊ 0
    }

    paw3399_write(0x5C, motion_ctrl);  // д�ؼĴ���
}


/****************************************
 *           ̧���жϺ���               *
 ****************************************/
/**
 * @brief ִ�� PAW3399 ���ֶ� Lift Off У׼��
 *
 * �ú����������û�������Ͻ����ֶ� Lift Off У׼��У׼������Ҫ���û������
 * ���ڱ����ϲ����ֲ�̧��֮���ƶ����һ�ξ������У׼��У׼�ɹ����д��
 * ��ؼĴ���ֵ��У׼ʧ����ָ�Ĭ��ֵ��
 *
 *
 * @note ��У׼�����У��û���Ҫ�ƶ�������� 20 Ӣ�������Ǵ󲿷ֵ�������
 */
void paw3399_manual_lift_cut_off_calibration(void)
{
    NRF_LOG_INFO("/*******************************/");
    NRF_LOG_INFO("Manual Lift Off Calibration is about to begin...");
    NRF_LOG_INFO("Please place the mouse on the surface and do not lift it.");
    NRF_LOG_INFO("Calibration will start in 5 seconds...");
    NRF_LOG_FLUSH();

    // ����ʱ5��
    for (int i = 5; i > 0; i--)
    {
        NRF_LOG_INFO("%d...", i);
        NRF_LOG_FLUSH();
        nrf_delay_ms(1000);  // ��ʱ1��
    }

    NRF_LOG_INFO("Starting calibration process...");
    NRF_LOG_FLUSH();

    // ִ��У׼����
    uint8_t var_mode = paw3399_read(0x40);  // ��ȡ 0x40
    paw3399_write(0x7F, 0x00);
    paw3399_write(0x40, 0x80);
    paw3399_write(0x7F, 0x05);
    paw3399_write(0x43, 0xE7);
    paw3399_write(0x7F, 0x04);
    paw3399_write(0x40, 0xC0);
    paw3399_write(0x41, 0x10);
    paw3399_write(0x44, 0x0F);
    paw3399_write(0x45, 0x0F);
    paw3399_write(0x46, 0x0F);
    paw3399_write(0x47, 0x0F);
    paw3399_write(0x48, 0x0F);
    paw3399_write(0x49, 0x0F);
    paw3399_write(0x4A, 0x0F);
    paw3399_write(0x4B, 0x0F);
    paw3399_write(0x40, 0xC1);

    // ��ʾ�û��ƶ�������У׼����ӡ��ʾ��Ϣ����ʾ�û��ƶ���곬��20Ӣ��
    NRF_LOG_INFO("Please move the mouse over a distance of more than 20 inches to cover most of the surface area of the mat.");
    NRF_LOG_INFO("You have 10 seconds to complete this movement.");
    NRF_LOG_FLUSH();

    // ����ʱ10��
    for (int i = 10; i > 0; i--)
    {
        NRF_LOG_INFO("Time remaining: %d seconds", i);
        NRF_LOG_FLUSH();
        nrf_delay_ms(1000);  // �ӳ�1��
    }

    // ֹͣУ׼
    paw3399_write(0x40, 0x40);

    // ���У׼״̬
    uint8_t status = paw3399_read(0x4C);
    if ((status & 0x0F) == 0x05) {
        NRF_LOG_INFO("Calibration succeeded!");
				NRF_LOG_INFO("/*******************************/");
        // ����У׼���д�� VarA �� VarB
        var_a = paw3399_read(0x4D);
        paw3399_write(0x7F, 0x0C);

        // �ж� VarA �Ƿ���ڵ��� 0x32
        if (var_a >= 0x32) {
            var_b = 0x05;
        } else {
            var_b = 0x03;
        }

        paw3399_write(0x4E, 0x08);
        paw3399_write(0x7F, 0x05);
        paw3399_write(0x43, 0xE4);
        paw3399_write(0x7F, 0x00);
        paw3399_write(0x40, var_mode);  // д�� Var_Mode ��ֵ
    } else {
        NRF_LOG_INFO("Calibration failed, resetting to default 1mm.");
				NRF_LOG_INFO("/*******************************/");
        // �ָ��Ĵ���
        paw3399_write(0x4E, 0x08);
        paw3399_write(0x7F, 0x05);
        paw3399_write(0x43, 0xE4);
        paw3399_write(0x7F, 0x00);
        paw3399_write(0x40, var_mode);
    }
}


/**
 * @brief ���� PAW3399 �� Lift Off У׼���á�
 *
 * ���ݸ����� VarA �� VarB ֵ������Ӧ�ļĴ�������Ϊ���� Lift Off У׼���á�
 *
 * @param[in] var_a У׼�����е� VarA ֵ��
 * @param[in] var_b У׼�����е� VarB ֵ��
 */
void paw3399_enable_lift_cut_off_calibration_setting(void)
{
    // ִ��ָ���ļĴ���д��˳��
    paw3399_write(0x7F, 0x0C);  
    paw3399_write(0x41, var_a); 
    paw3399_write(0x43, 0x30);  
    paw3399_write(0x44, var_b); 
    paw3399_write(0x4E, 0x08);  
    paw3399_write(0x5A, 0x0D);  
    paw3399_write(0x5B, 0x05);  
    paw3399_write(0x7F, 0x05);  
    paw3399_write(0x6E, 0x0F);  
    paw3399_write(0x7F, 0x09);  
    paw3399_write(0x71, 0x0F);  
    paw3399_write(0x7F, 0x00);  

    NRF_LOG_INFO("Lift Off Calibration Setting Enabled.");
    NRF_LOG_FLUSH();
}

/**
 * @brief ���� PAW3399 �� Lift Off У׼���ã����ָ���Ĭ�ϵ� 1mm ���á�
 *
 * ����Ӧ�ļĴ�������Ϊ���� Lift Off У׼���á�
 *
 */
void paw3399_disable_lift_cut_off_calibration_setting(void)
{
		// ִ��ָ���ļĴ���д��˳��
    paw3399_write(0x7F, 0x0C);  
    paw3399_write(0x41, 0x30);  
    paw3399_write(0x43, 0x20);  
    paw3399_write(0x44, 0x0D);  
    paw3399_write(0x4A, 0x12);  
    paw3399_write(0x4B, 0x09);  
    paw3399_write(0x4C, 0x30);  
    paw3399_write(0x4E, 0x08);  
    paw3399_write(0x53, 0x16);  
    paw3399_write(0x55, 0x14);  
    paw3399_write(0x5A, 0x0D);  
    paw3399_write(0x5B, 0x05);  
    paw3399_write(0x5F, 0x1E);  
    paw3399_write(0x66, 0x30);  
    paw3399_write(0x7F, 0x05);  
    paw3399_write(0x6E, 0x0F);  
    paw3399_write(0x7F, 0x09);  
    paw3399_write(0x71, 0x0F);  
    paw3399_write(0x72, 0x0A);  
    paw3399_write(0x7F, 0x00);  

    NRF_LOG_INFO("Lift Off Calibration Setting Disabled.");
    NRF_LOG_FLUSH();
}

/**
 * @brief ����PAW3399��Lift����
 *
 * �ú�������PAW3399��Lift���üĴ�����0xC4E���е�LIFTλ������ѡ��1mm��2mm��3mm��̧���жϾ��롣
 *
 * @param[in] lift_setting ѡ���Lift���� (0x0: 1mm, 0x1: 2mm, 0x2: 3mm)
 */
void paw3399_set_lift_config(uint8_t lift_setting)
{
    // ȷ��lift_setting�ںϷ���Χ��
    if (lift_setting > 0x02)
    {
        NRF_LOG_ERROR("Invalid lift setting.");
        return;
    }

    // д��Ĵ���0x7F�����ø��ֽڵ�ַ(MSB)
    paw3399_write(0x7F, 0x0C);  // 0xC4E���ֽ�Ϊ0x0C

    // д��Ĵ���0x4E������LIFTλ
    uint8_t current_value = paw3399_read(0x4E);  // ��ȡ�Ĵ�����ǰֵ
    current_value &= ~0x03;  // ���LIFTλ
    current_value |= (lift_setting & 0x03);  // ����LIFTλΪ�µ�lift_settingֵ
    paw3399_write(0x4E, current_value);  // д�ؼĴ���

    // д��Ĵ���0x7F���ָ�Ϊ0x00
    paw3399_write(0x7F, 0x00);
}

/**
 * @brief ��ȡPAW3399��Lift����
 *
 * �ú�����ȡPAW3399��Lift���üĴ�����0xC4E�������ص�ǰ��̧���жϾ������á�
 *
 * @return uint8_t ���ص�ǰ��Lift���� (0x0: 1mm, 0x1: 2mm, 0x2: 3mm)
 */
uint8_t paw3399_get_lift_config(void)
{
    // д��Ĵ���0x7F�����ø��ֽڵ�ַ(MSB)
    paw3399_write(0x7F, 0x0C);  // 0xC4E���ֽ�Ϊ0x0C

    // ��ȡ�Ĵ���0x4E��LIFTλ
    uint8_t current_value = paw3399_read(0x4E);
    uint8_t lift_setting = current_value & 0x03;  // ֻ����LIFTλ

    // д��Ĵ���0x7F���ָ�Ϊ0x00
    paw3399_write(0x7F, 0x00);
	
		switch (lift_setting)
    {
        case 0x00:
            NRF_LOG_INFO("Current Lift Setting: 1mm (default)");
            break;
        case 0x01:
            NRF_LOG_INFO("Current Lift Setting: 2mm");
            break;
        case 0x02:
            NRF_LOG_INFO("Current Lift Setting: 3mm");
            break;
        default:
            NRF_LOG_INFO("Unknown Lift Setting");
            break;
    }

    NRF_LOG_FLUSH();  // ˢ����־���

    return lift_setting;
}

/**
 * @brief ���оƬ�Ƿ�̧��
 * 
 * ���MOTION�Ĵ����е�Lift_Statλ������оƬ�Ƿ���̧��״̬��
 * 
 * @return bool ����true��ʾоƬ̧��false��ʾоƬ�ڱ����ϡ�
 */
bool paw3399_is_chip_lifted(void)
{
		uint8_t motion = paw3399_read(0x02);  // ��ȡMOTION�Ĵ���
	
    // ���Lift_Statλ (bit 3)��Ϊ1��ʾоƬ̧��
    if (motion & 0x08)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/****************************************
 *           Rest Mode����              *
 ****************************************/
/**
 * @brief ����оƬ������ģʽ��Rest Mode��
 * 
 * �ú������ݴ��ݵĲ���ֵ����оƬ������ģʽ������ `true` ��������ģʽ������ `false` ��������ģʽ��
 * 
 * @param[in] enable ���� `true` ��������ģʽ������ `false` ��������ģʽ��
 */
void paw3399_set_rest_mode(bool enable)
{
    uint8_t performance_reg = paw3399_read(0x40);  // ��ȡPERFORMANCE�Ĵ���

    if (enable)
    {
        performance_reg &= ~(1 << 7);  // ��AWAKEλ���㣬��������ģʽ
    }
    else
    {
        performance_reg |= (1 << 7);  // ��AWAKEλ��1����������ģʽ
    }

    paw3399_write(0x40, performance_reg);  // д��PERFORMANCE�Ĵ���
}


/**
 * @brief ��ȡ��ǰоƬ������ģʽ״̬
 * 
 * �ú�����ȡPERFORMANCE�Ĵ�����AWAKEλ���ж�оƬ��ǰ�Ƿ�������ģʽ��
 * 
 * @return bool ����true��ʾ����ģʽ�ѽ��ã�false��ʾ����ģʽ�����á�
 */
bool paw3399_get_rest_mode(void)
{
    uint8_t performance_reg = paw3399_read(0x40);  // ��ȡPERFORMANCE�Ĵ���
    bool is_rest_mode_enabled = (performance_reg & (1 << 7)) == 0;  // ���AWAKEλ�Ƿ�Ϊ0��0��ʾ��������ģʽ
    return is_rest_mode_enabled;
}

/**
 * @brief ��ȡ��ǰ�������»�ʱ�䣬Downshift ��һ�ִӸ߹���ģʽ�л����͹���ģʽ�Ĺ��̡�
 * 
 * �ú�����ȡRUN_DOWNSHIFT�Ĵ��������㲢���ش�����ģʽ�л���Rest1ģʽ��ʱ�䣨���룩��
 * 
 * @return uint16_t �����»�ʱ�䣬��λΪ���롣
 */
uint16_t paw3399_get_run_downshift_time(void)
{
    uint8_t reg_value = paw3399_read(0x77);  // ��ȡRUN_DOWNSHIFT�Ĵ���ֵ
    uint16_t downshift_time_ms = reg_value * run_downshift_mult * 50 / 1000;  // �����»�ʱ�� (ms)
	
    return downshift_time_ms;
}

/**
 * @brief ���������»�ʱ�䡣
 * 
 * �ú������ݸ�����ʱ�䣨���룩�������Ӧ�ļĴ���ֵ��д��RUN_DOWNSHIFT�Ĵ�����
 * 
 * @param[in] downshift_time_ms Ҫ���õ������»�ʱ�䣬��λΪ���롣
 */
void paw3399_set_run_downshift_time(uint16_t downshift_time_ms)
{
    // ������������»�ʱ�� 3276ms
    const uint16_t max_downshift_time_ms = 3276;
    
    // �����������»�ʱ�䣬��ʱ������Ϊ���ֵ
    if (downshift_time_ms > max_downshift_time_ms)
    {
        downshift_time_ms = max_downshift_time_ms;
        NRF_LOG_WARNING("Downshift time exceeds max value. Setting to maximum: %d ms", max_downshift_time_ms);
    }

    // ����Ĵ���ֵ
    uint8_t reg_value = (downshift_time_ms * 1000) / (run_downshift_mult * 50);
    
    // ȷ���Ĵ���ֵ��Ϊ 0
    if (reg_value == 0)
    {
        reg_value = 1;  // ȷ����СֵΪ1�����ⱻ����Ϊ0
    }

    // д��RUN_DOWNSHIFT�Ĵ���
    paw3399_write(0x77, reg_value);
}


/**
 * @brief ��ȡREST1����ʱ�䣬����ʱ���ڽ���͹���ģʽ��Rest Mode���󣬶��ڻ����Լ��Լ���Ƿ����˶���ʱ����
 * 
 * ��ȡREST1_PERIOD�Ĵ�����ֵ�������ض�Ӧ��ʱ�䣨�Ժ���Ϊ��λ����
 * 
 * @return uint8_t ����REST1����ʱ�䣬��λΪms��
 */
uint8_t paw3399_get_rest1_period(void)
{
    uint8_t rest1_period_reg = paw3399_read(0x78);  // ��ȡREST1_PERIOD�Ĵ���
    uint8_t period_ms = rest1_period_reg * 1;  // 1ms����
    return period_ms;
}

/**
 * @brief ����REST1����ʱ��
 * 
 * ����REST1_PERIOD�Ĵ�����ֵ���޸�Rest1������ʱ�䡣
 * 
 * @param[in] period_ms REST1����ʱ�䣬��λΪms��
 */
void paw3399_set_rest1_period(uint8_t period_ms)
{
    uint8_t rest1_period_reg = period_ms / 1;  // ��ʱ��ת��Ϊ�Ĵ���ֵ��1ms�ı�����
    if (rest1_period_reg == 0)
    {
        rest1_period_reg = 1;  // ȷ����СֵΪ1�����ⱻ����Ϊ0
    }
    paw3399_write(0x78, rest1_period_reg);  // д��REST1_PERIOD�Ĵ���
}

/**
 * @brief ��ȡRest1����ʱ��
 *
 * �ú�����ȡRest1_DOWNSHIFT�Ĵ������������Rest1��Rest2�Ľ���ʱ�䣨���룩��
 *
 * @return uint32_t ���ؽ���ʱ�䣬��λΪ���롣
 */
uint32_t paw3399_get_rest1_downshift_time(void)
{
    uint8_t rest1_downshift_reg = paw3399_read(0x79);  // ��ȡ�Ĵ���ֵ
    uint32_t downshift_time_ms = rest1_downshift_reg * rest1_downshift_mult * paw3399_get_rest1_period(); 
    return downshift_time_ms;
}

/**
 * @brief ����Rest1����ʱ��
 *
 * �ú�������Rest1��Rest2�Ľ���ʱ�䣬ʱ�������ת��Ϊ�Ĵ���ֵд��Rest1_DOWNSHIFT�Ĵ�����
 *
 * @param[in] period_ms ����ʱ�䣬��λΪ���롣
 */
void paw3399_set_rest1_downshift_time(uint32_t period_ms)
{
    uint8_t rest1_downshift_reg = period_ms / (rest1_downshift_mult * paw3399_get_rest1_period());  // ��ʱ��ת��Ϊ�Ĵ���ֵ
    if (rest1_downshift_reg == 0)
    {
        rest1_downshift_reg = 1;  // ȷ����СֵΪ1�����ⱻ����Ϊ0
    }
    paw3399_write(0x79, rest1_downshift_reg);  // д��Ĵ���
}

/**
 * @brief ��ȡREST2����ʱ��
 * 
 * ��ȡREST2_PERIOD�Ĵ�����ֵ�������ض�Ӧ��ʱ�䣨�Ժ���Ϊ��λ����
 * 
 * @return uint16_t ����REST2����ʱ�䣬��λΪms��
 */
uint16_t paw3399_get_rest2_period(void)
{
    uint8_t rest2_period_reg = paw3399_read(0x7A);  // ��ȡREST2_PERIOD�Ĵ���
    uint16_t period_ms = rest2_period_reg * SLOW_CLOCK * 4;  // 1ms * 4 �ı���
    return period_ms;
}

/**
 * @brief ����REST2����ʱ��
 * 
 * ����REST2_PERIOD�Ĵ�����ֵ���޸�Rest2������ʱ�䡣
 * 
 * @param[in] period_ms REST2����ʱ�䣬��λΪms��
 */
void paw3399_set_rest2_period(uint16_t period_ms)
{
    uint8_t rest2_period_reg = period_ms / (4 * SLOW_CLOCK);  // ��ʱ��ת��Ϊ�Ĵ���ֵ��4ms�ı�����
    if (rest2_period_reg == 0)
    {
        rest2_period_reg = 1;  // ȷ����СֵΪ1�����ⱻ����Ϊ0
    }
    paw3399_write(0x7A, rest2_period_reg);  // д��REST2_PERIOD�Ĵ���
}

/**
 * @brief ��ȡRest2��Rest3�Ľ���ʱ��
 *
 * �ú�����ȡREST2_DOWNSHIFT�Ĵ������������Rest2������Rest3��ʱ�䣨���룩��
 *
 * @return uint32_t ����Rest2��Rest3�Ľ���ʱ�䣬��λΪ���롣
 */
uint32_t paw3399_get_rest2_downshift_time(void)
{
    uint8_t downshift_reg = paw3399_read(0x7B);  // ��ȡ�Ĵ���ֵ
    uint32_t downshift_time_ms = downshift_reg * rest2_downshift_mult * paw3399_get_rest2_period();  
    return downshift_time_ms;
}

/**
 * @brief ����Rest2��Rest3�Ľ���ʱ��
 *
 * �ú���ͨ����REST2_DOWNSHIFT�Ĵ���д��ֵ�������ô�Rest2������Rest3��ʱ�䡣
 *
 * @param downshift_time_ms �µĽ���ʱ�䣬��λΪ���롣
 */
void paw3399_set_rest2_downshift_time(uint32_t downshift_time_ms)
{
    uint8_t rest3_downshift_reg = downshift_time_ms / (rest2_downshift_mult * paw3399_get_rest2_period());  // ��ʱ��ת��Ϊ�Ĵ���ֵ
    if (rest3_downshift_reg == 0)
    {
        rest3_downshift_reg = 1;  // ȷ����СֵΪ1�����ⱻ����Ϊ0
    }
    paw3399_write(0x7B, rest3_downshift_reg);  // д��Ĵ���ֵ
}

/**
 * @brief ��ȡRest3ģʽ����ʱ��
 *
 * �ú�����ȡPAW3399��REST3_PERIOD�Ĵ�����ֵ�����㲢����Rest3ģʽ������ʱ�䣨��λ��ms����
 * ͬʱ��ӡ��ǰ������ʱ�䡣
 *
 * @return uint16_t ����Rest3ģʽ������ʱ�䣬��λΪ���롣
 */
uint16_t paw3399_get_rest3_period(void)
{
    uint8_t rest3_period_reg = paw3399_read(0x7C);  // ��ȡ�Ĵ���0x7C����ȡRest3���ڵļĴ���ֵ
    uint16_t rest3_period_ms = rest3_period_reg * SLOW_CLOCK * 8;  // ���ݼĴ���ֵ��������ʱ��
    return rest3_period_ms;
}

/**
 * @brief ����Rest3ģʽ����ʱ��
 *
 * �ú�����ָ��������ʱ�䣨ms��д��REST3_PERIOD�Ĵ����������µ�Rest3ģʽ���ڡ�
 * ע�⣺��С��ЧֵΪ0x01�����ֵΪ0xFF�����óɹ����ӡ�����õ�����ʱ�䡣
 *
 * @param[in] period_ms Rest3ģʽ��������ʱ�䣬��λΪ���롣
 */
void paw3399_set_rest3_period(uint16_t period_ms)
{
    uint8_t rest3_period_reg = period_ms / (SLOW_CLOCK * 8);  // ��ʱ�䷴��Ϊ�Ĵ���ֵ
    if (rest3_period_reg == 0)
    {
        rest3_period_reg = 1;  // ȷ����СֵΪ1�����ⱻ����Ϊ0
    }
    paw3399_write(0x7C, rest3_period_reg);  // д���µ�����ʱ�䵽�Ĵ���0x7C
}

/**
 * @brief ��ȡ Run Downshift Multiplier ��ʵ��ֵ
 *
 * �ú����ӼĴ����ж�ȡ��ǰ�� Run Downshift Multiplier �ļĴ���ֵ����ת��Ϊʵ�ʵĳ���ֵ��
 *
 * @return uint16_t ���ص�ǰ�� Run Downshift Multiplier ��ʵ��ֵ��
 */
uint16_t paw3399_get_run_downshift_mult(void)
{
    // ��ȡ�Ĵ���0x7D��ֵ����ȡRUN_DOWN_M[3:0]��ֵ
    uint8_t reg_value = paw3399_read(0x7D) & 0x0F;

    // ���ݼĴ���ֵ����ʵ�ʳ���ֵ
    switch (reg_value)
    {
        case 0x00: run_downshift_mult = 2; break;
        case 0x01: run_downshift_mult = 4; break;
        case 0x02: run_downshift_mult = 8; break;
        case 0x03: run_downshift_mult = 16; break;
        case 0x04: run_downshift_mult = 32; break;
        case 0x05: run_downshift_mult = 64; break;
        case 0x06: run_downshift_mult = 128; break;
        case 0x07: run_downshift_mult = 256; break;
        case 0x08: run_downshift_mult = 512; break;
        case 0x09: run_downshift_mult = 1024; break;
        case 0x0A: run_downshift_mult = 2048; break;
        default: run_downshift_mult = 256; break;  // Ĭ��ֵΪ256
    }

    return run_downshift_mult;
}

/**
 * @brief ���� Run Downshift Multiplier ��ʵ��ֵ
 *
 * �ú�����������ĳ���ֵ�����Ӧ�ļĴ���ֵ����д��Ĵ�����ͬʱ����ȫ�ֱ�����
 *
 * @param new_value Ҫ���õ�Run Downshift Multiplier��ʵ��ֵ��
 */
void paw3399_set_run_downshift_mult(uint16_t new_value)
{
    uint8_t reg_value;

    // ���������ʵ�ʳ���ֵ�����Ӧ�ļĴ���ֵ
    switch (new_value)
    {
        case 2: reg_value = 0x00; break;
        case 4: reg_value = 0x01; break;
        case 8: reg_value = 0x02; break;
        case 16: reg_value = 0x03; break;
        case 32: reg_value = 0x04; break;
        case 64: reg_value = 0x05; break;
        case 128: reg_value = 0x06; break;
        case 256: reg_value = 0x07; break;
        case 512: reg_value = 0x08; break;
        case 1024: reg_value = 0x09; break;
        case 2048: reg_value = 0x0A; break;
        default:
            NRF_LOG_ERROR("Invalid Run Downshift Multiplier value.");
            return;
    }

    // ��������ļĴ���ֵд��Ĵ���0x7D
    paw3399_write(0x7D, reg_value);

    // ����ȫ�ֱ���
    run_downshift_mult = new_value;
}

/**
 * @brief ��ȡREST1��REST2 Downshift Multiplier
 *
 * �ú�����REST_DOWNSHIFT_MULT�Ĵ�����ȡREST1��REST2���»�����������ȫ�ֱ���rest1_downshift_mult��rest2_downshift_mult��
 */
void paw3399_get_rest_downshift_mult(void)
{
    // ��ȡ�Ĵ���0x7E��ֵ
    uint8_t reg_value = paw3399_read(0x7E);

    // ��ȡREST_M0��REST_M2��λ������REST1�ĳ���ֵ
    uint8_t rest1_value = reg_value & 0x07;
    switch (rest1_value) {
        case 0x00: rest1_downshift_mult = 2; break;
        case 0x01: rest1_downshift_mult = 4; break;
        case 0x02: rest1_downshift_mult = 8; break;
        case 0x03: rest1_downshift_mult = 16; break;
        case 0x04: rest1_downshift_mult = 32; break;
        case 0x05: rest1_downshift_mult = 64; break;
        case 0x06: rest1_downshift_mult = 128; break;
        case 0x07: rest1_downshift_mult = 256; break;
    }

    // ��ȡREST_M4��REST_M6��λ������REST2�ĳ���ֵ
    uint8_t rest2_value = (reg_value >> 4) & 0x07;
    switch (rest2_value) {
        case 0x00: rest2_downshift_mult = 2; break;
        case 0x01: rest2_downshift_mult = 4; break;
        case 0x02: rest2_downshift_mult = 8; break;
        case 0x03: rest2_downshift_mult = 16; break;
        case 0x04: rest2_downshift_mult = 32; break;
        case 0x05: rest2_downshift_mult = 64; break;
        case 0x06: rest2_downshift_mult = 128; break;
        case 0x07: rest2_downshift_mult = 256; break;
    }

    // ��ӡ��ǰ��REST1��REST2 Downshift Multiplier
    NRF_LOG_INFO("Current REST1 Downshift Multiplier: %d", rest1_downshift_mult);
    NRF_LOG_INFO("Current REST2 Downshift Multiplier: %d", rest2_downshift_mult);
    NRF_LOG_FLUSH();
}

/**
 * @brief ����REST1��REST2 Downshift Multiplier
 *
 * �ú������µĳ���д��REST_DOWNSHIFT_MULT�Ĵ�����������ȫ�ֱ���rest1_downshift_mult��rest2_downshift_mult��
 *
 * @param[in] rest1_mult_value Ҫ���õ�REST1����ֵ (��Ч��ΧΪ2��256)��
 * @param[in] rest2_mult_value Ҫ���õ�REST2����ֵ (��Ч��ΧΪ2��256)��
 */
void paw3399_set_rest_downshift_mult(uint16_t rest1_mult_value, uint16_t rest2_mult_value)
{
    // ����rest1_mult_value��rest2_mult_value��2��256֮��
    if (rest1_mult_value < 2) {
        rest1_mult_value = 2;
        NRF_LOG_WARNING("REST1 Multiplier value too low, setting to minimum: 2");
    } else if (rest1_mult_value > 256) {
        rest1_mult_value = 256;
        NRF_LOG_WARNING("REST1 Multiplier value too high, setting to maximum: 256");
    }

    if (rest2_mult_value < 2) {
        rest2_mult_value = 2;
        NRF_LOG_WARNING("REST2 Multiplier value too low, setting to minimum: 2");
    } else if (rest2_mult_value > 256) {
        rest2_mult_value = 256;
        NRF_LOG_WARNING("REST2 Multiplier value too high, setting to maximum: 256");
    }

    // ���ݳ���ֵ����Ĵ���ֵ
    uint8_t rest1_reg_value, rest2_reg_value;
    switch (rest1_mult_value) {
        case 2: rest1_reg_value = 0x00; break;
        case 4: rest1_reg_value = 0x01; break;
        case 8: rest1_reg_value = 0x02; break;
        case 16: rest1_reg_value = 0x03; break;
        case 32: rest1_reg_value = 0x04; break;
        case 64: rest1_reg_value = 0x05; break;
        case 128: rest1_reg_value = 0x06; break;
        case 256: rest1_reg_value = 0x07; break;
    }

    switch (rest2_mult_value) {
        case 2: rest2_reg_value = 0x00; break;
        case 4: rest2_reg_value = 0x01; break;
        case 8: rest2_reg_value = 0x02; break;
        case 16: rest2_reg_value = 0x03; break;
        case 32: rest2_reg_value = 0x04; break;
        case 64: rest2_reg_value = 0x05; break;
        case 128: rest2_reg_value = 0x06; break;
        case 256: rest2_reg_value = 0x07; break;
    }

    // �ϲ�REST1��REST2�ļĴ���ֵ
    uint8_t reg_value = (rest2_reg_value << 4) | rest1_reg_value;

    // д��Ĵ���0x7E��ֵ
    paw3399_write(0x7E, reg_value);

    // ��ӡ���õ�REST1��REST2 Downshift Multiplier
    NRF_LOG_INFO("Set REST1 Downshift Multiplier: %d", rest1_mult_value);
    NRF_LOG_INFO("Set REST2 Downshift Multiplier: %d", rest2_mult_value);
    NRF_LOG_FLUSH();
}

/****************************************
 *           Angle Snap����             *
 ****************************************/
/**
 * @brief ���ýǶ���������
 *
 * �ú������ݴ���Ĳ���ֵ���ýǶ����������Ƿ����á�
 * 
 * @param[in] enable ����true���ýǶ��������ܣ�false����á�
 */
void paw3399_set_angle_snap(bool enable)
{
    uint8_t reg_value = paw3399_read(0x56);  // ��ȡ��ǰ�Ĵ���ֵ

    if (enable)
    {
        reg_value |= 0x80;  // ����ENλΪ1�����ýǶ���������
    }
    else
    {
        reg_value &= ~0x80;  // ���ENλ�����ýǶ���������
    }

    paw3399_write(0x56, reg_value);  // д��Ĵ���
}

/**
 * @brief ���Ƕ����������Ƿ�����
 *
 * �ú�����ȡANGLE_SNAP�Ĵ��������ؽǶ����������Ƿ����á�
 * 
 * @return bool ����true��ʾ�Ƕ����������ã�false��ʾδ���á�
 */
bool paw3399_get_angle_snap(void)
{
    uint8_t reg_value = paw3399_read(0x56);  // ��ȡANGLE_SNAP�Ĵ���ֵ
    bool enabled = reg_value & 0x80;  // ���ENλ
    return enabled;
}

/****************************************
 *           ��任����                 *
 ****************************************/
/**
 * @brief ���� XY �ύ������
 * 
 * �ú������ݴ���Ĳ���ֵ���û���� XY �ύ�����ܡ�
 * 
 * @param[in] enable ���� true ���� XY �ύ�����ܣ�false ���� XY �ύ�����ܡ�
 */
void paw3399_set_swap_xy(bool enable)
{
    uint8_t reg_value = paw3399_read(0x5B);  // ��ȡ AXIS_CONTROL �Ĵ���

    if (enable)
    {
        reg_value |= 0x80;  // ���� Swap_XY λ (bit 7)
    }
    else
    {
        reg_value &= ~0x80;  // ��� Swap_XY λ (bit 7)
    }

    paw3399_write(0x5B, reg_value);  // д�� AXIS_CONTROL �Ĵ���
}

/**
 * @brief ��ȡ��ǰ�Ƿ������� XY �ύ������
 * 
 * �ú�����ȡ AXIS_CONTROL �Ĵ����������Ƿ������� XY �ύ�����ܡ�
 * 
 * @return bool ���� true ��ʾ������ XY �ύ�����ܣ�false ��ʾδ���á�
 */
bool paw3399_get_swap_xy(void)
{
    uint8_t reg_value = paw3399_read(0x5B);  // ��ȡ AXIS_CONTROL �Ĵ���
    bool enabled = reg_value & 0x80;  // ��� Swap_XY λ (bit 7)
    return enabled;
}


/**
 * @brief ���� X �ᷴת����
 * 
 * �ú������ݴ���Ĳ���ֵ���û���� X �ᷴת���ܡ�
 * 
 * @param[in] enable ���� true ���� X �ᷴת���ܣ�false ���� X �ᷴת���ܡ�
 */
void paw3399_set_inv_x(bool enable)
{
    uint8_t reg_value = paw3399_read(0x5B);  // ��ȡ AXIS_CONTROL �Ĵ���

    if (enable)
    {
        reg_value |= 0x20;  // ���� INV_X λ (bit 5)
    }
    else
    {
        reg_value &= ~0x20;  // ��� INV_X λ (bit 5)
    }

    paw3399_write(0x5B, reg_value);  // д�� AXIS_CONTROL �Ĵ���
}

/**
 * @brief ��ȡ��ǰ�Ƿ������� X �ᷴת����
 * 
 * �ú�����ȡ AXIS_CONTROL �Ĵ����������Ƿ������� X �ᷴת���ܡ�
 * 
 * @return bool ���� true ��ʾ������ X �ᷴת���ܣ�false ��ʾδ���á�
 */
bool paw3399_get_inv_x(void)
{
    uint8_t reg_value = paw3399_read(0x5B);  // ��ȡ AXIS_CONTROL �Ĵ���
    bool enabled = reg_value & 0x20;  // ��� INV_X λ (bit 5)
    return enabled;
}

/**
 * @brief ���� Y �ᷴת����
 * 
 * �ú������ݴ���Ĳ���ֵ���û���� Y �ᷴת���ܡ�
 * 
 * @param[in] enable ���� true ���� Y �ᷴת���ܣ�false ���� Y �ᷴת���ܡ�
 */
void paw3399_set_inv_y(bool enable)
{
    uint8_t reg_value = paw3399_read(0x5B);  // ��ȡ AXIS_CONTROL �Ĵ���

    if (enable)
    {
        reg_value |= 0x40;  // ���� INV_Y λ (bit 6)
    }
    else
    {
        reg_value &= ~0x40;  // ��� INV_Y λ (bit 6)
    }

    paw3399_write(0x5B, reg_value);  // д�� AXIS_CONTROL �Ĵ���
}

/**
 * @brief ��ȡ��ǰ�Ƿ������� Y �ᷴת����
 * 
 * �ú�����ȡ AXIS_CONTROL �Ĵ����������Ƿ������� Y �ᷴת���ܡ�
 * 
 * @return bool ���� true ��ʾ������ Y �ᷴת���ܣ�false ��ʾδ���á�
 */
bool paw3399_get_inv_y(void)
{
    uint8_t reg_value = paw3399_read(0x5B);  // ��ȡ AXIS_CONTROL �Ĵ���
    bool enabled = reg_value & 0x40;  // ��� INV_Y λ (bit 6)
    return enabled;
}

/****************************************
 *           �Ʋ����ƺ���               *
 ****************************************/
/**
 * @brief ����Ripple Control���ܡ�
 * 
 * �ú������ݴ���Ĳ���ֵ���û����Ripple Control���ܡ�
 * 
 * @param[in] enable ����true����Ripple Control��false����Ripple Control��
 */
void paw3399_set_ripple_control(bool enable)
{
    uint8_t reg_value = paw3399_read(0x5A);  // ��ȡ�Ĵ���0x5A

    if (enable)
    {
        reg_value |= 0x80;  // ����CTRLλΪ1������Ripple Control
    }
    else
    {
        reg_value &= ~0x80;  // ���CTRLλΪ0������Ripple Control
    }

    paw3399_write(0x5A, reg_value);  // д�ؼĴ���0x5A
}

/**
 * @brief ���Ripple Control�Ƿ������á�
 * 
 * �ú������Ĵ���0x5A��CTRLλ���ж�Ripple Control�Ƿ������á�
 * 
 * @return true ���Ripple Control�����ã�����true�����򷵻�false��
 */
bool paw3399_get_ripple_control(void)
{
    uint8_t reg_value = paw3399_read(0x5A);  // ��ȡ�Ĵ���0x5A
    bool enabled = reg_value & 0x80;
		return enabled;    
}

/****************************************
 *           ���ź���                   *
 ****************************************/
/**
 * @brief ��ȡ��ǰ����ֵ
 * 
 * �ú�����ȡ12λ�Ŀ��żĴ������ȶ�ȡSHUTTER_UPPER���ٶ�ȡSHUTTER_LOWER�����Ϊ������12λ����ֵ��
 * 
 * @return uint16_t ���ص�ǰ�Ŀ���ֵ��12λ����
 */
uint16_t paw3399_get_shutter_value(void)
{
    // ��ȡ���ŵĸ�4λ
    uint8_t shutter_upper = paw3399_read(0x0C) & 0x0F;

    // ��ȡ���ŵĵ�8λ
    uint8_t shutter_lower = paw3399_read(0x0B);

    // ���12λ����ֵ
    uint16_t shutter_value = (shutter_upper << 8) | shutter_lower;

	return shutter_value;
}

/**
 * @brief ��ȡ MOT_Set �Ƿ�Ϊ�͵�ƽ��Ч��active low��
 * 
 * �ú�����ȡ MOTION_CTRL �Ĵ������ж� MOT_Set �Ƿ�Ϊ�͵�ƽ��Ч��0����
 * 
 * @return bool ���� true ��ʾ MOT_Set Ϊ�͵�ƽ��Ч��active low����false ��ʾ�ߵ�ƽ��Ч��active high����
 */
bool paw3399_get_mot_active_low(void)
{
    uint8_t motion_ctrl = paw3399_read(0x5C);  // ��ȡ MOTION_CTRL �Ĵ���
    return (motion_ctrl & (1 << 7)) ? false : true;  // ��� MOT_Set Ϊ 0������ true ��ʾ active low
}

/**
 * @brief ���� MOT_Set ��״̬
 * 
 * �ú������� MOTION_CTRL �Ĵ����� MOT_Set λ��
 * 
 * @param[in] mot_set ����ֵ��true ��ʾ����Ϊ�͵�ƽ��Ч (motion active low)��false ��ʾ����Ϊ�ߵ�ƽ��Ч (motion active high)��
 */
void paw3399_set_mot_active_low(bool active_low)
{
    uint8_t motion_ctrl = paw3399_read(0x5C);  // ��ȡ��ǰ�Ĵ���ֵ

    if (active_low)
    {
				motion_ctrl &= ~(1 << 7);  // ��� MOT_Set Ϊ 0 (�͵�ƽ)
    }
    else
    {
        motion_ctrl |= (1 << 7);  // ���� MOT_Set Ϊ 1 (�ߵ�ƽ)
    }

    paw3399_write(0x5C, motion_ctrl);  // д�ؼĴ���
}

/***************************************
 *          Angle Tune����             *
 ***************************************/
/**
 * @brief ��ȡ��ǰ�Ƕ�����
 * 
 * @return int8_t ���ؽǶ�����ֵ����λΪ������-30, -10, 0, 15, 30��
 */
int8_t paw3399_get_angle_tune_angle(void)
{
    paw3399_write(0x7F, 0x05);  // д�� MSB (0x05) ���Ĵ��� 0x7F
    uint8_t angle_value = paw3399_read(0x77);  // ��ȡ�Ĵ��� 0x0577 ��ֵ
    paw3399_write(0x7F, 0x00);  // �ָ��Ĵ��� 0x7F ��ֵ

    switch (angle_value)
    {
        case 0xE2:
            return -30;
        case 0xF6:
            return -10;
        case 0x00:
            return 0;
        case 0x0F:
            return 15;
        case 0x1E:
            return 30;
        default:
            NRF_LOG_INFO("Unknown angle value: 0x%02X", angle_value);
            return -1;  // δ֪�Ƕ�
    }
}

/**
 * @brief ���ýǶȵ���ֵ
 * 
 * @param[in] angle Ҫ���õĽǶ�ֵ���Ϸ�ֵΪ -30, -10, 0, 15, 30
 * 
 * @return bool �ɹ����� true�����򷵻� false
 */
bool paw3399_set_angle_tune_angle(int8_t angle)
{
    uint8_t angle_value;

    switch (angle)
    {
        case -30:
            angle_value = 0xE2;
            break;
        case -10:
            angle_value = 0xF6;
            break;
        case 0:
            angle_value = 0x00;
            break;
        case 15:
            angle_value = 0x0F;
            break;
        case 30:
            angle_value = 0x1E;
            break;
        default:
            NRF_LOG_INFO("Invalid angle: %d", angle);
            return false;
    }

    paw3399_write(0x7F, 0x05);  // д�� MSB (0x05) ���Ĵ��� 0x7F
    paw3399_write(0x77, angle_value);  // д��Ƕ�����
    paw3399_write(0x7F, 0x00);  // �ָ��Ĵ��� 0x7F ��ֵ

    return true;
}

/**
 * @brief ���û���ýǶȵ�������
 * 
 * @param[in] enable true ��ʾ���ýǶȵ�����false ��ʾ���ýǶȵ���
 */
void paw3399_set_angle_tune(bool enable)
{
    paw3399_write(0x7F, 0x05);  // д�� MSB (0x05) ���Ĵ��� 0x7F
    uint8_t angle_tune2 = paw3399_read(0x78);  // ��ȡ�Ĵ��� 0x0578 ��ֵ

    if (enable)
    {
        angle_tune2 |= (1 << 7);  // ���� EN λΪ 1
    }
    else
    {
        angle_tune2 &= ~(1 << 7);  // ��� EN λ
    }

    paw3399_write(0x78, angle_tune2);  // д����º��ֵ
    paw3399_write(0x7F, 0x00);  // �ָ��Ĵ��� 0x7F ��ֵ
}

/**
 * @brief ���Ƕȵ��������Ƿ�����
 * 
 * @return bool ��������˽Ƕȵ��������� true�����򷵻� false
 */
bool paw3399_is_angle_tune_enabled(void)
{
    paw3399_write(0x7F, 0x05);  // д�� MSB (0x05) ���Ĵ��� 0x7F
    uint8_t angle_tune2 = paw3399_read(0x78);  // ��ȡ�Ĵ��� 0x0578 ��ֵ
    paw3399_write(0x7F, 0x00);  // �ָ��Ĵ��� 0x7F ��ֵ

    return (angle_tune2 & (1 << 7)) != 0;
}



/****************************************
 *           ���Ը�������                *
 ****************************************/
/**
 * @brief ��ӡ����ģʽ���ơ�
 *
 * �ú�������ģʽֵ��uint8_t��������ֵ�����Ӧ��ģʽ���ơ�
 * 
 * @param[in] mode PAW3399�Ĺ���ģʽ��ȡֵΪ0x00, 0x01, 0x02, 0x03֮һ��
 */
void paw3399_print_mode(uint8_t mode)
{
    // ����ģʽֵ��ӡ��Ӧ��ģʽ����
    switch (mode)
    {
        case 0x00:
            NRF_LOG_INFO("Mode: High Performance Mode");
            break;
        case 0x01:
            NRF_LOG_INFO("Mode: Low Power Mode");
            break;
        case 0x02:
            NRF_LOG_INFO("Mode: Office Mode");
            break;
        case 0x03:
            NRF_LOG_INFO("Mode: Corded Gaming Mode");
            break;
        default:
            NRF_LOG_INFO("Unknown Mode");
            break;
    }

    NRF_LOG_FLUSH();  // ˢ����־���
}

/**
 * @brief ��ӡоƬ��ǰ����ģʽ
 * 
 * �ú������ݴ���Ĳ���ģʽֵ����ӡоƬ�ĵ�ǰ����ģʽ��
 * 
 * @param[in] op_mode оƬ��ǰ������ģʽ��
 */
void paw3399_print_op_mode(uint8_t op_mode)
{
    switch (op_mode)
    {
        case 0x00:
						NRF_LOG_INFO("Chip OP Mode: Run Mode.");
            break;
        case 0x01:
            NRF_LOG_INFO("Chip OP Mode: Rest Mode 1.");
            break;
        case 0x02:
            NRF_LOG_INFO("Chip OP Mode: Rest Mode 2.");
            break;
        case 0x03:
            NRF_LOG_INFO("Chip OP Mode: Rest Mode 3.");
            break;
        default:
            NRF_LOG_INFO("Unknown Mode.");
            break;
    }

    NRF_LOG_FLUSH();  // ˢ����־���
}
/**
 * @brief ���PAW3399������״̬�͵�ǰ����ģʽ
 *
 * �ú������PAW3399оƬ��CHIP_OBSERVATION�Ĵ����͵�ǰ����ģʽ�������Ӧ��״̬��־��
 */
void paw3399_print_status(void)
{    
    // ��ӡCHIP_OBSERVATION��״̬
		NRF_LOG_INFO("Chip_Observation: %s", paw3399_chip_observation() ? "Normal" : "Abnormal");

    // ��ȡ��ǰ����ģʽ
    uint8_t op_mode = paw3399_get_op_mode();
		uint8_t mode = paw3399_get_mode();
    
    // ��ӡ����ģʽ
    paw3399_print_op_mode(op_mode);
		paw3399_print_mode(mode);
}

/**
 * @brief ����ʱ����������Ϊ��λ���е���ʱ������ÿ�������־��
 *
 * @param[in] seconds ����ʱ��������
 */
void paw3399_dealy_countdown(int seconds)
{
    for (int i = seconds; i > 0; i--)
    {
        NRF_LOG_INFO("%d...", i);
        NRF_LOG_FLUSH();
        nrf_delay_ms(1000);  // ��ʱ1��
    }
}

/**
 * @brief ��ӡRawDataͼ��
 *
 * �ú�������PAW3399��RawData�����ӡ������ͼ��ÿ��36�����ݣ��д��µ��ϴ�ӡ��
 * ͬʱ�ڶ���������ӡ�кź��кţ����ڲ鿴���ݽṹ��
 * 
 * @param[in] pFrame �洢RawData���ݵĻ�����ָ�롣
 */
void paw3399_print_rawdata_map(void)
{
    // ÿ��36�����ݣ���ͼ�еĸ�ʽ��ӡ
    NRF_LOG_INFO("/*******************************/");
    NRF_LOG_INFO("RawData Map Output Start");
    NRF_LOG_FLUSH();

    // ��ӡ�к� (����)
    char line_buffer[512];
    int offset = 0;
    offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "     "); // ���ϽǵĿո�
    for (int col = 0; col < 36; col++) {
        offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "%4d", col); // �к�
    }
    NRF_LOG_INFO("%s", line_buffer);
    NRF_LOG_FLUSH();

    // ��ӡÿ������
    for (int row = 35; row >= 0; row--) // �д�35��ʼ��0
    {
        offset = 0;
        // ��ӡ�к�
        offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "%4d ", row); // �к�

        // ��ӡ��Ӧ�е������е�����
        for (int col = 0; col < 36; col++) {
            int index = row * 36 + col;
            if (index < 1296) {
                offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "%4d", raw_data[index]);
            } else {
                offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "    "); // ������Χ���ո�
            }
        }
        NRF_LOG_INFO("%s", line_buffer);
        NRF_LOG_FLUSH();
    }

    NRF_LOG_INFO("RawData Map Output Complete");
    NRF_LOG_INFO("/*******************************/");
    NRF_LOG_FLUSH();
}

/**
 * @brief ��ӡMotion Burst���ݡ�
 *
 * �ú�����PAW3399������ͨ��Motion Burst��ȡ����12�ֽ����������ӡ��
 * 
 * @param[in] buffer �洢Motion Burst���ݵĻ�����ָ�롣
 */
void paw3399_print_motion_burst_data(void)
{
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("Motion Burst Data: ");
		NRF_LOG_INFO("Motion: 0x%02x", motion_burst_data[0]); // BYTE[00] = Motion
		NRF_LOG_INFO("Observation: 0x%02x", motion_burst_data[1]); // BYTE[01] = Observation
		NRF_LOG_INFO("Delta_X_L: 0x%02x", motion_burst_data[2]); // BYTE[02] = Delta_X_L
		NRF_LOG_INFO("Delta_X_H: 0x%02x", motion_burst_data[3]); // BYTE[03] = Delta_X_H
		NRF_LOG_INFO("Delta_Y_L: 0x%02x", motion_burst_data[4]); // BYTE[04] = Delta_Y_L
		NRF_LOG_INFO("Delta_Y_H: 0x%02x", motion_burst_data[5]); // BYTE[05] = Delta_Y_H
		NRF_LOG_INFO("SQUAL: 0x%02x", motion_burst_data[6]); // BYTE[06] = SQUAL
		NRF_LOG_INFO("RawData_Sum: 0x%02x", motion_burst_data[7]); // BYTE[07] = RawData_Sum
		NRF_LOG_INFO("Maximum_RawData: 0x%02x", motion_burst_data[8]); // BYTE[08] = Maximum_RawData
		NRF_LOG_INFO("Minimum_Rawdata: 0x%02x", motion_burst_data[9]); // BYTE[09] = Minimum_Rawdata
		NRF_LOG_INFO("Shutter_Upper: 0x%02x", motion_burst_data[10]); // BYTE[10] = Shutter_Upper
		NRF_LOG_INFO("Shutter_Lower: 0x%02x", motion_burst_data[11]); // BYTE[11] = Shutter_Lower
		NRF_LOG_INFO("/******************************/");
		NRF_LOG_FLUSH();
}

/**
 * @brief ���������ŵİ��º��ͷ��¼�
 * 
 * �ú������ָ�����ŵİ��º��ͷš�ʹ�÷������Ʊ����δ�����
 * 
 * @param[in] pin ������Ӧ�����źš�
 * @param[in] default_high ���������Ĭ��״̬�Ǹߵ�ƽ������true��������false��
 * 
 * @return bool ����true��ʾ�������£�false��ʾ����δ���¡�
 */
bool paw3399_button_is_pressed(uint32_t pin, bool default_high)
{
    static bool last_button_state = false;  // ��һ�εİ���״̬
    bool current_button_state = nrf_gpio_pin_read(pin);  // ��ȡ���ŵ�ƽ״̬

    // ����Ĭ��״̬������ƽ�ж�����
    if (default_high)
    {
        current_button_state = (current_button_state == 0);  // Ĭ�ϸߵ�ƽ������ʱΪ�͵�ƽ
    }
    else
    {
        current_button_state = (current_button_state != 0);  // Ĭ�ϵ͵�ƽ������ʱΪ�ߵ�ƽ
    }

    // ���״̬�����˱仯�����з�����ʱ���ٴ�ȷ��
    if (current_button_state != last_button_state)
    {
        nrf_delay_ms(10);  // ������ʱ
        bool rechecked_state = nrf_gpio_pin_read(pin);  // �ٴζ�ȡ

        if (default_high)
        {
            rechecked_state = (rechecked_state == 0);  // ����ȷ�ϵ�ƽ״̬
        }
        else
        {
            rechecked_state = (rechecked_state != 0);
        }

        // ���״̬ȷʵ�仯������״̬������
        if (rechecked_state != last_button_state)
        {
            last_button_state = rechecked_state;
            return rechecked_state;  // ���ذ���״̬
        }
    }

    return false;  // û�б仯ʱ����false
}

/**
 * @brief �ȴ�ָ������������
 *
 * �ú��������ȴ���ֱ��ָ���İ��������¡�
 *
 * @param[in] pin �������ź�
 */
void paw3399_wait_for_button_press(uint32_t pin)
{
    NRF_LOG_INFO("Waiting for button press (BUTTON_PIN: %d)...", pin);
    NRF_LOG_FLUSH();

    // ѭ���ȴ�ֱ��ָ�����ŵİ���������
    while (!paw3399_button_is_pressed(pin, true));  // ������谴ťΪactive low
}


/***************************************
 *             ���Ժ���             		 *
 ***************************************/
/**
 * @brief PAW3399���������Ժ�����
 *
 * �ú�������PAW3399�������ĸ���ܣ�����Motion Burst��ȡ��Raw Data�����
 * ģʽ�л����ػ��������ϵ�Ȳ��������н����ͨ����־�����
 */
void paw3399_test(void)
{
		// ������gpio
		nrf_gpio_cfg_input(TEST_BUTTON, NRF_GPIO_PIN_PULLUP);  // ��������Ϊ����ģʽ�������ݲ�����������������
		
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Test Start");
		NRF_LOG_FLUSH();
		paw3399_wait_for_button_press(TEST_BUTTON);
	
		// PAW3399 ���� ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Reset Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_reset();
		paw3399_dealy_countdown(5); // �ӳ�5s�Թ۲칦��
		paw3399_power_up();
		NRF_LOG_INFO("PAW3399 Reset Test Complete");
		NRF_LOG_INFO("/*******************************/");
	
		// PAW3399 �ػ� ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Shut Down Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_shutdown();
		paw3399_dealy_countdown(5); // �ӳ�5s�Թ۲칦��
		paw3399_power_up_from_shutdown();
		NRF_LOG_INFO("PAW3399 Shut Down Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 ģʽ�л� ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Running Mode Test Start");
		paw3399_print_mode(paw3399_get_mode());
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_low_power_mode();
		paw3399_print_mode(paw3399_get_mode());
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_office_mode();
		paw3399_print_mode(paw3399_get_mode());
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_corded_gaming_mode();
		paw3399_print_mode(paw3399_get_mode());
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_high_performance_mode();
		paw3399_print_mode(paw3399_get_mode());
		paw3399_wait_for_button_press(TEST_BUTTON);
		NRF_LOG_INFO("PAW3399 Running Mode Test Complete");
		NRF_LOG_INFO("/*******************************/");
	
		// PAW3399 Motion Burst ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Motion Burst Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_motion_burst();
		paw3399_print_motion_burst_data();
		NRF_LOG_INFO("PAW3399 Motion Burst Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 Raw Data ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Raw Data Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_get_rawdata();
		paw3399_print_rawdata_map();
		NRF_LOG_INFO("Max RawData: %d", paw3399_get_max_rawdata());
    NRF_LOG_INFO("Min RawData: %d", paw3399_get_min_rawdata());
    NRF_LOG_INFO("SQUAL: %d", paw3399_get_squal());
		NRF_LOG_INFO("PAW3399 Raw Data Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 XY Delta ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 XY Delta Test Start");
		NRF_LOG_INFO("Please Move The Sensor");
		paw3399_wait_for_button_press(TEST_BUTTON);
		// ��� MOTION_PIN �Ƿ�Ϊ�͵�ƽ
		for (uint8_t count = 0; count < 10; count++)
		{
			while(nrf_gpio_pin_read(MOTION_PIN));  // 0 ��ʾ���ű�����
			paw3399_get_delta_xy();
			NRF_LOG_INFO("Delta_X: %d, Delta_Y: %d", delta_x, delta_y);
			NRF_LOG_FLUSH();
		}
		NRF_LOG_INFO("PAW3399 XY Delta Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 CPI/DPI ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 CPI/DPI Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
    paw3399_get_xy_resolution();
    NRF_LOG_INFO("Current XY resolution: X = %d, Y = %d", x_resolution, y_resolution);
    NRF_LOG_INFO("Set XY resolution to X:1200 Y:1400: %s", paw3399_set_xy_resolution(1200, 1400) ? "Success" : "Failed");
		paw3399_wait_for_button_press(TEST_BUTTON);
    paw3399_get_xy_resolution();
    NRF_LOG_INFO("Current XY resolution: X = %d, Y = %d", x_resolution, y_resolution);
    NRF_LOG_INFO("Set XY resolution to X:40000 Y:3500: %s", paw3399_set_xy_resolution(40000, 3500) ? "Success" : "Failed");
		paw3399_wait_for_button_press(TEST_BUTTON);
    paw3399_get_xy_resolution();
    NRF_LOG_INFO("Current XY resolution: X = %d, Y = %d", x_resolution, y_resolution);
    NRF_LOG_INFO("Set XY resolution to X:0 Y:0: %s", paw3399_set_xy_resolution(0, 0) ? "Success" : "Failed");
		paw3399_wait_for_button_press(TEST_BUTTON);
    paw3399_get_xy_resolution();
    NRF_LOG_INFO("Current XY resolution: X = %d, Y = %d", x_resolution, y_resolution);
    NRF_LOG_INFO("Set XY resolution to X:4999 Y:4999: %s", paw3399_set_xy_resolution(4999, 4999) ? "Success" : "Failed");
		paw3399_wait_for_button_press(TEST_BUTTON);
    paw3399_get_xy_resolution();
    NRF_LOG_INFO("Current XY resolution: X = %d, Y = %d", x_resolution, y_resolution);
    NRF_LOG_INFO("Set XY resolution to X:20000 Y:20000: %s", paw3399_set_xy_resolution(20000, 20000) ? "Success" : "Failed");
    paw3399_set_res_mod_different_src(false);
		NRF_LOG_INFO("Current XY resolution: X = %d, Y = %d", x_resolution, y_resolution);
    NRF_LOG_INFO("Set XY resolution to X:20000 Y:5000: %s", paw3399_set_xy_resolution(20000, 5000) ? "Success" : "Failed");
		NRF_LOG_INFO("Final XY resolution: X = %d, Y = %d",x_resolution, y_resolution);
		paw3399_set_res_mod_different_src(true);
		NRF_LOG_INFO("PAW3399 CPI/DPI Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 ̧���ж� ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Lift Cut Off Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_manual_lift_cut_off_calibration();
		paw3399_enable_lift_cut_off_calibration_setting();
		paw3399_wait_for_button_press(TEST_BUTTON);
		NRF_LOG_INFO("Chip is %s", paw3399_is_chip_lifted() ? "lifted" : "on the surface");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_get_lift_config();
		paw3399_set_lift_config(0x00);
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_get_lift_config();
		paw3399_set_lift_config(0x01);
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_get_lift_config();
		paw3399_set_lift_config(0x02);
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_get_lift_config();
		paw3399_set_lift_config(0x03);
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_get_lift_config();
		paw3399_set_lift_config(0x04);
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_get_lift_config();
		NRF_LOG_INFO("Chip is %s", paw3399_is_chip_lifted() ? "lifted" : "on the surface");
		paw3399_disable_lift_cut_off_calibration_setting();
		NRF_LOG_INFO("PAW3399 Lift Cut Off Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 Rest Mode ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Rest Mode Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_rest_mode(false);
		NRF_LOG_INFO("Rest Mode: %s", paw3399_get_rest_mode() ? "enabled" : "disabled");
		paw3399_set_rest_mode(true);
		NRF_LOG_INFO("Rest Mode: %s", paw3399_get_rest_mode() ? "enabled" : "disabled");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_run_downshift_time(499);
		NRF_LOG_INFO("Run Downshift Time: %d", paw3399_get_run_downshift_time());
		paw3399_set_rest1_period(10);
		NRF_LOG_INFO("Rest1 Period: %d", paw3399_get_rest1_period());
		paw3399_set_rest1_downshift_time(10000);
		NRF_LOG_INFO("Rest1 Downshift Time: %d", paw3399_get_rest1_downshift_time());
		paw3399_set_rest2_period(512);
		NRF_LOG_INFO("Rest2 Period: %d", paw3399_get_rest2_period());
		paw3399_set_rest2_downshift_time(500000);
		NRF_LOG_INFO("Rest2 Downshift Time: %d", paw3399_get_rest2_downshift_time());
		paw3399_set_rest3_period(303);
		NRF_LOG_INFO("Rest3 Period: %d", paw3399_get_rest3_period());
		paw3399_set_run_downshift_mult(127);
		NRF_LOG_INFO("Run Downshift Mult: %d", paw3399_get_run_downshift_mult());
		paw3399_set_rest_downshift_mult(614, 0);
		
		paw3399_wait_for_button_press(TEST_BUTTON);
		
		paw3399_set_run_downshift_time(256);
		NRF_LOG_INFO("Run Downshift Time: %d", paw3399_get_run_downshift_time());
		paw3399_set_rest1_period(1);
		NRF_LOG_INFO("Rest1 Period: %d", paw3399_get_rest1_period());
		paw3399_set_rest1_downshift_time(9216);
		NRF_LOG_INFO("Rest1 Downshift Time: %d", paw3399_get_rest1_downshift_time());
		paw3399_set_rest2_period(100);
		NRF_LOG_INFO("Rest2 Period: %d", paw3399_get_rest2_period());
		paw3399_set_rest2_downshift_time(601600);
		NRF_LOG_INFO("Rest2 Downshift Time: %d", paw3399_get_rest2_downshift_time());
		paw3399_set_rest3_period(504);
		NRF_LOG_INFO("Rest3 Period: %d", paw3399_get_rest3_period());
		paw3399_set_run_downshift_mult(256);
		NRF_LOG_INFO("Run Downshift Mult: %d", paw3399_get_run_downshift_mult());
		paw3399_set_rest_downshift_mult(64, 64);
		paw3399_get_rest_downshift_mult();
		
		NRF_LOG_INFO("PAW3399 Rest Mode Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 Angle Snap ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Angle Snap Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_angle_snap(true);
		NRF_LOG_INFO("Angle Snap: %s", paw3399_get_angle_snap() ? "enabled" : "disabled");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_angle_snap(false);
		NRF_LOG_INFO("Angle Snap: %s", paw3399_get_angle_snap() ? "enabled" : "disabled");
		NRF_LOG_INFO("PAW3399 Angle Snap Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 Swap XY ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Swap XY Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_swap_xy(true);
		NRF_LOG_INFO("Swap XY: %s", paw3399_get_swap_xy() ? "enabled" : "disabled");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_swap_xy(false);
		NRF_LOG_INFO("Swap XY: %s", paw3399_get_swap_xy() ? "enabled" : "disabled");
		NRF_LOG_INFO("PAW3399 Swap XY Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 Inv X ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Inv X Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_inv_x(true);
		NRF_LOG_INFO("Inv X: %s", paw3399_get_inv_x() ? "enabled" : "disabled");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_inv_x(false);
		NRF_LOG_INFO("Inv X: %s", paw3399_get_inv_x() ? "enabled" : "disabled");
		NRF_LOG_INFO("PAW3399 Inv X Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 Inv Y ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Inv Y Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_inv_y(true);
		NRF_LOG_INFO("Inv Y: %s", paw3399_get_inv_y() ? "enabled" : "disabled");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_inv_y(false);
		NRF_LOG_INFO("Inv Y: %s", paw3399_get_inv_y() ? "enabled" : "disabled");
		NRF_LOG_INFO("PAW3399 Inv Y Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 Ripple Control ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Ripple Control Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_ripple_control(true);
		NRF_LOG_INFO("Inv Y: %s", paw3399_get_ripple_control() ? "enabled" : "disabled");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_ripple_control(false);
		NRF_LOG_INFO("Inv Y: %s", paw3399_get_ripple_control() ? "enabled" : "disabled");
		NRF_LOG_INFO("PAW3399 Ripple Control Test Complete");
		NRF_LOG_INFO("/*******************************/");	
		
		// PAW3399 ���� ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Shutter Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		NRF_LOG_INFO("Shutter Value: %d", paw3399_get_shutter_value());
		NRF_LOG_INFO("PAW3399 Shutter Test Complete");
		NRF_LOG_INFO("/*******************************/");	
		
		// PAW3399 Motion ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Motion Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_mot_active_low(false);
		NRF_LOG_INFO("Motion Active: %s", paw3399_get_mot_active_low() ? "low" : "high");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_mot_active_low(true);
		NRF_LOG_INFO("Motion Active: %s", paw3399_get_mot_active_low() ? "low" : "high");
		NRF_LOG_INFO("PAW3399 Motion Test Complete");
		NRF_LOG_INFO("/*******************************/");

		// PAW3399 Angle Tune ����
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PA399 Angle Tune Test Start");
		NRF_LOG_INFO("Angle Tune: %s", paw3399_is_angle_tune_enabled() ? "enabled" : "disabled");
		NRF_LOG_INFO("Angle Tune Angle: %d", paw3399_get_angle_tune_angle());
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_angle_tune_angle(-30);
		NRF_LOG_INFO("Angle Tune Angle: %d", paw3399_get_angle_tune_angle());
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_angle_tune_angle(20);
		NRF_LOG_INFO("Angle Tune Angle: %d", paw3399_get_angle_tune_angle());
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_angle_tune(true);
		NRF_LOG_INFO("Angle Tune Angle: %d", paw3399_get_angle_tune_angle());
		NRF_LOG_INFO("Angle Tune: %s", paw3399_is_angle_tune_enabled() ? "enabled" : "disabled");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_set_angle_tune(false);
		paw3399_set_angle_tune_angle(0);
		NRF_LOG_INFO("Angle Tune Angle: %d", paw3399_get_angle_tune_angle());
		NRF_LOG_INFO("Angle Tune: %s", paw3399_is_angle_tune_enabled() ? "enabled" : "disabled");
		NRF_LOG_INFO("PAW3399 Angle Tune Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 ���� ���
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_print_status();
		NRF_LOG_INFO("PAW3399 Test Complete");
		NRF_LOG_INFO("/*******************************/");
		paw3399_wait_for_button_press(TEST_BUTTON);
		NRF_LOG_FLUSH();
}

