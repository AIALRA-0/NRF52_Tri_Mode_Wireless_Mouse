/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include "ble_device.h"

/***************************************
 *              �궨��             		 *
 ***************************************/
// �豸��Ϣ����
#define DEVICE_NAME                     "AIALRA_GAMING_MOUSE"                                /**< @brief �豸���ƣ��������ڹ㲥�����С� */
#define MANUFACTURER_NAME               "AIALRA"                       /**< @brief ���������ƣ������ݸ��豸��Ϣ���� */

// ��Ӧ�̺Ͳ�Ʒ��Ϣ����
#define PNP_ID_VENDOR_ID_SOURCE         0x02                                        /**< @brief ��Ӧ��ID��Դ�� */
#define PNP_ID_VENDOR_ID                0x1915                                      /**< @brief ��Ӧ��ID�� */
#define PNP_ID_PRODUCT_ID               0xEEEE                                      /**< @brief ��ƷID�� */
#define PNP_ID_PRODUCT_VERSION          0x0001                                      /**< @brief ��Ʒ�汾�� */

// BLE���ò�������
#define APP_BLE_OBSERVER_PRIO           3                                           /**< @brief Ӧ�õ�BLE�۲������ȼ���ͨ�������޸ġ� */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< @brief SoftDevice BLE���ñ�ʶ�� */

// ��ط�����
#define BATTERY_LEVEL_MEAS_INTERVAL     APP_TIMER_TICKS(2000)                       /**< @brief ��ص������������ticks���� */
#define MIN_BATTERY_LEVEL               81                                          /**< @brief ģ���ص���С������ */
#define MAX_BATTERY_LEVEL               100                                         /**< @brief ģ���ص��������� */
#define BATTERY_LEVEL_INCREMENT         1                                           /**< @brief ÿ��ģ���ص��������������� */

// ���Ӳ�������
/*lint -emacro(524, MIN_CONN_INTERVAL) // Loss of precision */
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)            /**< @brief ��С���Ӽ����7.5���룩�� */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(15, UNIT_1_25_MS)             /**< @brief ������Ӽ����15���룩�� */
#define SLAVE_LATENCY                   20                                          /**< @brief ���豸�ӳ١� */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(3000, UNIT_10_MS)             /**< @brief ���Ӽල��ʱ��3000���룩�� */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< @brief �Ӵ����¼������ӻ�֪ͨ��ʼ�����״ε������Ӳ������µ�ʱ�䣨5�룩�� */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< @brief �״����Ӳ������º�ÿ�ε��õ�ʱ������30�룩�� */
#define MAX_CONN_PARAM_UPDATE_COUNT     3                                           /**< @brief �ڷ������Ӳ���Э��֮ǰ������Դ����� */


// Swift Pair���ܶ���
#define SWIFT_PAIR_SUPPORTED            1                                           /**< @brief ֧��Swift Pair���ܡ� */
#if SWIFT_PAIR_SUPPORTED == 1
#define MICROSOFT_VENDOR_ID             0x0006                                      /**< @brief Microsoft��Ӧ��ID�� */
#define MICROSOFT_BEACON_ID             0x03                                        /**< @brief Microsoft Beacon ID����ʾ֧��Swift Pair���ܡ� */
#define MICROSOFT_BEACON_SUB_SCENARIO   0x00                                        /**< @brief Microsoft Beacon�ӳ���������ָʾ���ͨ��Swift Pair��ԡ� */
#define RESERVED_RSSI_BYTE              0x80                                        /**< @brief ������RSSI�ֽڣ����ڱ�����ǰ���������ԡ� */
#endif

// �㲥��������
#define APP_ADV_FAST_INTERVAL           0x0028                                      /**< @brief ���ٹ㲥�������λΪ0.625���룬��Ӧ��25���룩�� */
#define APP_ADV_SLOW_INTERVAL           0x00A0                                      /**< @brief ���ٹ㲥�������λΪ0.625���룬��Ӧ��100���룩�� */
#define APP_ADV_FAST_DURATION           3000                                        /**< @brief ���ٹ㲥�ĳ���ʱ�䣬��λΪ10���롣 */
#define APP_ADV_SLOW_DURATION           18000                                       /**< @brief ���ٹ㲥�ĳ���ʱ�䣬��λΪ10���롣 */

// ��ȫ��������
#define SEC_PARAM_BOND                  1                                           /**< @brief �Ƿ������ԡ� */
#define SEC_PARAM_MITM                  0                                           /**< @brief �Ƿ���Ҫ�м��˱����� */
#define SEC_PARAM_LESC                  0                                           /**< @brief �Ƿ�����LE��ȫ���ӡ� */
#define SEC_PARAM_KEYPRESS              0                                           /**< @brief �Ƿ����ð���֪ͨ�� */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< @brief ��I/O������ */
#define SEC_PARAM_OOB                   0                                           /**< @brief �޴������ݡ� */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< @brief ��С������Կ��С�� */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< @brief ��������Կ��С�� */

// HID������ö���
#define BASE_USB_HID_SPEC_VERSION       0x0101                                      /**< @brief Ӧ��ʵ�ֵĻ���USB HID�淶�汾�š� */
#define INPUT_REPORT_COUNT              3                                           /**< @brief ��Ӧ���е����뱨�������� */
#define INPUT_REP_BUTTONS_LEN           6                                           /**< @brief ������ť���ݵ�������뱨��ĳ��ȡ� */
#define INPUT_REP_MOVEMENT_LEN          4                                           /**< @brief �����˶����ݵ�������뱨��ĳ��ȡ� */
#define INPUT_REP_MEDIA_PLAYER_LEN      1                                           /**< @brief ����ý�岥�������ݵ�������뱨��ĳ��ȡ� */
#define INPUT_REP_BUTTONS_INDEX         0                                           /**< @brief ������ť���ݵ�������뱨��������� */
#define INPUT_REP_MOVEMENT_INDEX        1                                           /**< @brief �����˶����ݵ�������뱨��������� */
#define INPUT_REP_MPLAYER_INDEX         2                                           /**< @brief ����ý�岥�������ݵ�������뱨��������� */
#define INPUT_REP_REF_BUTTONS_ID        1                                           /**< @brief ������ť���ݵ�������뱨�������ID�� */
#define INPUT_REP_REF_MOVEMENT_ID       2                                           /**< @brief �����˶����ݵ�������뱨�������ID�� */
#define INPUT_REP_REF_MPLAYER_ID        3                                           /**< @brief ����ý�岥�������ݵ�������뱨�������ID�� */

// ���������ö���
#define SCHED_MAX_EVENT_DATA_SIZE       APP_TIMER_SCHED_EVENT_DATA_SIZE             /**< @brief �������¼�������С�� */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                20                                          /**< @brief �����������е�����¼������������л�Ӧ����Ҫ���ࡣ */
#else
#define SCHED_QUEUE_SIZE                10                                          /**< @brief �����������е�����¼����� */
#endif

/***************************************
 *              BLE ��־           	  *	
 **************************************/ 
// HIDģ��
static bool              mark_in_boot_mode = false;                                    /**< @brief ��ǰЭ��ģʽ��ָʾ�Ƿ�������ģʽ�� */

/***************************************
 *           BLE ��־��������           *	
 **************************************/ 
 /**@brief ���� mark_in_boot_mode ��־�ĺ�����
 *
 * @param[in] boot_mode  ����ֵ��true ��ʾ���� Boot ģʽ��false ��ʾ�˳� Boot ģʽ��
 */
void ble_device_set_mark_in_boot_mode(bool boot_mode)
{
    mark_in_boot_mode = boot_mode;
}

/**@brief ��ȡ mark_in_boot_mode ��־�ĺ�����
 *
 * @return ��ǰ mark_in_boot_mode ��־�Ĳ���ֵ��true ��ʾ���� Boot ģʽ��false ��ʾ���� Report ģʽ��
 */
bool ble_device_get_mark_in_boot_mode(void)
{
    return mark_in_boot_mode;
}

/***************************************
 *              BLE ģ��           	  *	
 **************************************/ 
 
// BLE��ص���ģ��
APP_TIMER_DEF(module_battery_timer);								/**< @brief ���岢����һ��������־���ܵ�Ӧ�ö�ʱ��ʵ����ʹ�ö�ʱ���Ĵ�������־�����ø��Ӽ���һ�� */
static sensorsim_cfg_t   module_battery_simulation_cfg;         /**< @brief ��ص���������ģ���������á� */
static sensorsim_state_t module_battery_simulation_state;       /**< @brief ��ص���������ģ������״̬�� */
BLE_BAS_DEF(module_ble_battery_service);                   /**< @brief ��ط���ʵ����������BLE�豸���������Ϣ�� */

// BLE����ģ��
static uint16_t          module_connection_handle  = BLE_CONN_HANDLE_INVALID;                  /**< @brief ��ǰ���ӵľ����BLE_CONN_HANDLE_INVALID��ʾ�����ӡ� */
NRF_BLE_GATT_DEF(module_gatt);                                                           /**< @brief GATTģ��ʵ�������ڴ���ͨ�����������ļ������ӡ� */

// ����д��ģ��
NRF_BLE_QWR_DEF(module_queue_write);                                                             /**< @brief ����д��ģ��������ģ����ڹ���д���� */

// BLE�㲥ģ��
BLE_ADVERTISING_DEF(module_ble_advertising);                                                 /**< @brief �㲥ģ��ʵ�������ڹ���BLE�㲥�� */
static ble_uuid_t        module_advertising_uuids[] =                                            /**< @brief �㲥��ʹ�õķ���UUID��ͨ��Ψһ��ʶ������ */
{
    {BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE, BLE_UUID_TYPE_BLE}                    /**< @brief �˻��ӿ��豸�����UUID�� */
};
BLE_ADVERTISING_DEF(module_advertising);                                                 /**< @brief �㲥ģ��ʵ�������ڹ���BLE�㲥�� */

// BLE���ģ��
static pm_peer_id_t      module_peer_device_id;                                                 /**< @brief ��ǰ�Ѱ󶨵������豸���豸���þ���� */
 
// SWIFT PAIR ���ģ�飬����΢���һ��������Թ��ܣ������豸�������
#if SWIFT_PAIR_SUPPORTED == 1
static uint8_t module_swift_pair_payload[] =                                                     /**< @brief ΢��Swift Pair���ܵĹ㲥���ݽṹ���ء� */
{
    MICROSOFT_BEACON_ID,                                                            /**< @brief ΢���ű� ID�����ڱ�ʶ����һ��֧�� Swift Pair ���ܵ��豸������ֵ��΢���塣 */
    MICROSOFT_BEACON_SUB_SCENARIO,                                                  /**< @brief ΢���ű���ӳ�������һ���������ű�����û����͡���ֵ�����豸���ݳ���������ꡢ���̣�������Ӧ�������ʾ�� */
    RESERVED_RSSI_BYTE                                                              /**< @brief ������ RSSI �ֽڣ����ڱ�ʾ�����ź�ǿ��ָʾ��RSSI������ͨ����һ���̶�ֵ���Թ� Windows ʹ�á� */
};
static ble_advdata_manuf_data_t module_swift_pair_manufacturer_advertising_data =                                /**< @brief ΢��Swift Pair���ܵĹ㲥���ݽṹ�� */
{
    .company_identifier = MICROSOFT_VENDOR_ID,                                      /**< @brief ΢��˾��ʶ���� */
    .data               =
    {
        .size   = sizeof(module_swift_pair_payload),                                             /**< @brief ���ݵĴ�С���ֽ����� */
        .p_data = &module_swift_pair_payload[0]                                                  /**< @brief ָ��㲥���ݸ��ص�ָ�롣 */
    }
};
static ble_advdata_t module_swift_pair_advertising_data;                                                  /**< @brief Swift Pair��BLE�㲥���ݡ� */
#endif

// BLE HIDģ��
BLE_HIDS_DEF(module_hid_service,                                     /**< HID����ʵ�������ڴ��������豸�����ݴ��䡣 */
             NRF_SDH_BLE_TOTAL_LINK_COUNT,                                          /**< ֧�ֵ�BLE���������� */
             INPUT_REP_BUTTONS_LEN,                                                 /**< ���뱨���а�ť���ݵĳ��ȡ� */
             INPUT_REP_MOVEMENT_LEN,                                                /**< ���뱨�����ƶ����ݵĳ��ȡ� */
             INPUT_REP_MEDIA_PLAYER_LEN);                                           /**< ���뱨����ý�岥�������ݵĳ��ȡ� */

/***************************************
 *           BLE ģ���������           *	
 **************************************/ 
/**@brief ��ȡ module_hid_service ʵ���ĺ�����
 *
 * @return ���� BLE HID ����ʵ����ָ�롣
 */
ble_hids_t* ble_device_get_module_hid_service(void)
{
    return &module_hid_service;
}


/**@brief ��ȡ��ǰ���Ӿ���ĺ�����
 *
 * @return ���ص�ǰ���Ӿ����ֵ����������ӣ��򷵻� BLE_CONN_HANDLE_INVALID��
 */
uint16_t ble_device_get_module_connection_handle(void)
{
    return module_connection_handle;
}

 /****************************************
 *             BLED ��ʼ������  			     *
 ****************************************/
 /**
 * @brief �ú���ִ��ϵͳ��ʼ����������ջ���úͱ�Ҫ�ķ�����������������ѭ�����������״̬��
 * 
 * @return int ���򷵻�״̬��ͨ�����᷵��ֵ��
 */
void ble_device_init(void)
{
    bool erase_bonds;  /**< ָʾ�Ƿ���Ҫ�����Ѱ󶨵��豸��Ϣ�� */

    // ��ؼ��ģ���ʼ��
		ble_device_battery_timer_init();              /**< ��ʼ����ʱ��ģ�飬Ϊϵͳ��ʱ�¼��ṩ֧�֡� */
		ble_device_battery_sensor_simulator_init();    									/**< ��ʼ��������ģ����������ģ�⴫�������ݣ����ص������� */
	
		// ���ʹ���ģ���ʼ��
		ble_device_power_management_init();    /**< ��ʼ����Դ����ģ�飬���ڿ����豸�ĵ͹���״̬�� */
	
		// BLEģʽ��ť��LED��ʼ��
		ble_device_buttons_leds_init(&erase_bonds);				/**< ��ʼ����ť��LEDģ�飬�����û������ָʾ�ƿ��ƹ��ܣ�����Ƿ���Ҫ�����󶨡� */
	
		// BLE��ջ��Э��ģ���ʼ��
		ble_device_ble_stack_init();           /**< ��ʼ��BLE��ջ�������������ܡ� */
		ble_device_gap_init();					 /**< ����GAP��ͨ�÷��������ļ��������������豸���ơ��ɼ��Ժ����Ӽ���� */
		ble_device_gatt_init();                /**< ��ʼ��GATT��ͨ�����������ļ���ģ�飬�������Ժͷ���Ĳ���, �涨������ͨ�ŵĿ�ܡ� */
	  ble_device_connection_parameters_init(); /**< ��ʼ�����Ӳ���ģ�飬ȷ��BLE���ӵ��ȶ��ԡ� */
	
		// �㲥�����ģ���ʼ��
		ble_device_advertising_init();         /**< ��ʼ���㲥ģ�飬�����豸�Ĺ㲥��Ϣ�� */
	  ble_device_peer_manager_init();				/**< ��ʼ��Peer Managerģ�飬�����豸��ԡ��󶨺ͼ��ܹ��ܡ� */
	
		// ����ģ���ʼ��
		ble_device_queue_write_init();   /**< ��ʼ��Queued Writeģ�飬֧�ֳ�д������ */
    ble_device_info_system_init();   /**< ��ʼ���豸��Ϣ����DIS�����ṩ�豸������Ϣ�� */
		ble_device_battery_service_init();  /**< ��ʼ����ط���BAS�������ڼ��ͱ����ص����� */
		
		// HIDģ���ʼ��
		ble_device_hid_service_init();
		
		// �¼�����ģ���ʼ��
		ble_device_scheduler_init();
		
		// ����ɨ�贰��
		NRF_LOG_INFO("BLE device initialized. Starting advertising..."); /**< ��־�����HID���ʾ���������� */
    ble_device_battery_timer_start();         /**< ������ر��涨ʱ���¼��� */
    ble_device_advertising_start(erase_bonds); /**< ��ʼ�㲥���Ƿ������ȡ����erase_bonds��־�� */
		NRF_LOG_INFO("Waiting for pairing and connection...");
}

/**@brief �������״̬����ѭ����������
 *
 * @details �ú������ڴ������״̬�µ�������ȡ����û�д��������־�����������͹���ģʽ��
 *          ʹ�豸����˯��״̬��ֱ����һ���¼�������
 */
void ble_device_idle_state_handle(void)
{
    app_sched_execute(); /**< ִ�е������еĴ��������� */

}

 /***************************************
 *            BLE ��غ���          *	
 **************************************/
/**@brief ��ʱ����ʼ��������
 *
 * @details �ú��������ʼ����ʱ��ģ�飬������һ�����ڵ�ص��������Ķ�ʱ����
 */
void ble_device_battery_timer_init(void)
{
    ret_code_t err_code;  /**< ���ڴ洢�������صĴ�����롣 */

    err_code = app_timer_init();  /**< ��ʼ��Ӧ�ó���ʱ��ģ�顣 */
    APP_ERROR_CHECK(err_code);     /**< ��鶨ʱ����ʼ���Ƿ�ɹ��������򴥷������� */

    // ������ض�ʱ����
    err_code = app_timer_create(&module_battery_timer,               /**< ��ʱ��������������õ�ض�ʱ���� */
                                APP_TIMER_MODE_REPEATED,           /**< ��ʱ��ģʽ������Ϊ�ظ�ģʽ�� */
                                ble_device_battery_timer_timeout_handler); /**< ��ʱ����ʱ������������ʱ����ʱʱ���øú����� */
    APP_ERROR_CHECK(err_code);     /**< ��鶨ʱ�������Ƿ�ɹ��������򴥷������� */
}

/**@brief ��ز�����ʱ����ʱ��������
 *
 * @details ÿ����ص���������ʱ������ʱ�������ô˺����������ڲ�ִ�е�ص������²�����
 *
 * @param[in]   p_context   ָ�������ĵ�ָ�룬���ڴ�app_start_timer()���ô���������Ϣ��
 *                          �ڱ�������δʹ�ô˲�����
 */
void ble_device_battery_timer_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);     /**< ���p_contextΪδʹ�ò�����������������档 */
    ble_device_battery_level_update();          /**< ���õ�ص������º���������ǰ������Ϣ�����������豸�� */
}

/**@brief ִ�е�ز��������µ�ط����еĵ�ص�������ֵ��
 *
 * @details �ú���ͨ��������ģ������ȡ��ǰ��ص���ֵ����������µ���ط���Battery Service���ĵ�ص�������ֵ�С�
 *          ������¹����г��ִ��󣨷�Ԥ�ڵĴ��󣩣������ô��������
 */
void ble_device_battery_level_update(void)
{
    ret_code_t err_code;             /**< ���ڴ洢�������صĴ�����롣 */
    uint8_t  battery_level;          /**< ��ǰ��ص���ֵ����λΪ�ٷֱȣ�0-100���� */

    battery_level = (uint8_t)sensorsim_measure(&module_battery_simulation_state, &module_battery_simulation_cfg); /**< ͨ��������ģ����������ص����� */

    err_code = ble_bas_battery_level_update(&module_ble_battery_service, battery_level, BLE_CONN_HANDLE_ALL);  /**< �������ĵ�ص������µ���ط�������ֵ�С� */
    if ((err_code != NRF_SUCCESS) &&                                                     /**< �����²����Ƿ�ɹ����ں���״̬�� */
        (err_code != NRF_ERROR_BUSY) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != NRF_ERROR_FORBIDDEN) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       )
    {
        APP_ERROR_HANDLER(err_code);  /**< ��������Ԥ�ڴ��󣬵��ô�������� */
    }
}

/**@brief ��ʼ����ط���Battery Service���ĺ�����
 *
 * @details �ú������ò���ʼ����ط��������������豸���浱ǰ��ص�������֧�ֵ�ص���֪ͨ���ܡ�
 */
void ble_device_battery_service_init(void)
{
    ret_code_t     err_code;       /**< ���ڴ洢�������صĴ�����롣 */
    ble_bas_init_t bas_init_obj;   /**< ��ط����ʼ���ṹ�壬�������õ�ط��� */

    memset(&bas_init_obj, 0, sizeof(bas_init_obj)); /**< �����ط����ʼ���ṹ�塣 */

    bas_init_obj.evt_handler          = NULL;           /**< ��ʹ���¼�������� */
    bas_init_obj.support_notification = true;           /**< ���õ�ص���֪ͨ���ܡ� */
    bas_init_obj.p_report_ref         = NULL;           /**< ��ʹ�ñ������á� */
    bas_init_obj.initial_batt_level   = 100;            /**< ���ó�ʼ��ص���Ϊ100%�� */

    // ���õ�ط���İ�ȫ�ȼ�
    bas_init_obj.bl_rd_sec        = SEC_JUST_WORKS;     /**< ���õ�ص�����ȡ�İ�ȫ���� */
    bas_init_obj.bl_cccd_wr_sec   = SEC_JUST_WORKS;     /**< ����CCCD���ͻ�������������������д��İ�ȫ���� */
    bas_init_obj.bl_report_rd_sec = SEC_JUST_WORKS;     /**< ���õ�ر����ȡ�İ�ȫ���� */

    err_code = ble_bas_init(&module_ble_battery_service, &bas_init_obj);     /**< ��ʼ����ط���ģ�顣 */
    APP_ERROR_CHECK(err_code);                          /**< ����ط����ʼ���Ƿ�ɹ��������򴥷������� */
}

/**@brief ��ʼ����ش�����ģ�����ĺ�����
 *
 * @details �ú������õ�ص���ģ�����Ĳ�����������С�������������͵��������ȣ�
 *          ����ģ���ص����ı仯��
 */
void ble_device_battery_sensor_simulator_init(void)
{
    module_battery_simulation_cfg.min          = MIN_BATTERY_LEVEL;      /**< ����ģ���ص�������Сֵ�� */
    module_battery_simulation_cfg.max          = MAX_BATTERY_LEVEL;      /**< ����ģ���ص��������ֵ�� */
    module_battery_simulation_cfg.incr         = BATTERY_LEVEL_INCREMENT; /**< ���õ�ص�����������ÿ�θ���ʱ�����仯��ֵ�� */
    module_battery_simulation_cfg.start_at_max = true;                   /**< ָ��ģ��������������ʼ�� */

    sensorsim_init(&module_battery_simulation_state, &module_battery_simulation_cfg); /**< ��ʼ�����ģ����״̬��Ӧ�����õĲ����� */
}

/**@brief ������ʱ���ĺ�����
 *
 * @details �ú����������ڵ�ص��������Ķ�ʱ����ʹ�䰴�趨�ļ��������ص��������¼���
 */
void ble_device_battery_timer_start(void)
{
    ret_code_t err_code; /**< ���ڴ洢�������صĴ�����롣 */

    err_code = app_timer_start(module_battery_timer, BATTERY_LEVEL_MEAS_INTERVAL, NULL); /**< ������ز�����ʱ��������ָ���Ĳ������������ */
    APP_ERROR_CHECK(err_code); /**< ��鶨ʱ�������Ƿ�ɹ��������򴥷������� */
}


 /***************************************
 *            BLE ���ʹ�����          *	
 **************************************/ 
/**@brief ��ʼ����Դ����ģ��ĺ�����
 *
 * @details �ú��������ʼ����Դ����ģ�飨Power Management����
 *          �Ա��豸�ڿ���ʱ���Խ���͹���ģʽ���Ӷ���ʡ������
 *          �����ʼ��ʧ�ܣ������ô��������
 */
void ble_device_power_management_init(void)
{
    ret_code_t err_code;                /**< ���ڴ洢�������صĴ�����롣 */
    err_code = nrf_pwr_mgmt_init();     /**< ��ʼ����Դ����ģ�飬���õ͹��Ĺ��ܡ� */
    APP_ERROR_CHECK(err_code);          /**< ����Դ����ģ���Ƿ��ʼ���ɹ��������򴥷������� */
}

 /***************************************
 *        BLE ģʽ��ť��LED����(TODO)         *	
 **************************************/ 

/**@brief ��ť��LED�ĳ�ʼ��������
 *
 * @details �ú�����ʼ�����صİ�ť��LED���Ա���Ӧ����ʹ�á�ͬʱ�����Ƿ�����������������ݵİ�ť��
 *          ����ڻ���ʱ����������󶨰�ť����ͨ������`p_erase_bonds`���ظ���Ϣ��
 *
 * @param[out] p_erase_bonds  �����������������ݵİ�ť���ò���������true����ʾ��Ҫ����豸�İ���Ϣ��
 */
void ble_device_buttons_leds_init(bool * p_erase_bonds)
{
    ret_code_t err_code;               /**< ���ڴ洢�������صĴ�����롣 */
    bsp_event_t startup_event;         /**< ���ڴ洢��ť�������¼�����������������¼��� */

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, ble_device_buttons_event_handler);  /**< ��ʼ��LED�Ͱ�ť�������¼��������� */
    APP_ERROR_CHECK(err_code);                                                /**< ��鰴ť��LED��ʼ���Ƿ�ɹ��������򴥷������� */

    err_code = bsp_btn_ble_init(NULL, &startup_event);                         /**< ��ʼ��BLE��ť���ܣ�����Ƿ��������¼���������󶨣��� */
    APP_ERROR_CHECK(err_code);                                                /**< ���BLE��ť��ʼ���Ƿ�ɹ��������򴥷������� */

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);         /**< �����⵽������¼���������p_erase_bondsΪtrue�� */
}

/**@brief �������԰�ťģ����¼��ĺ�����
 *
 * @details �ú������ݲ�ͬ�İ�ť�¼�ִ����Ӧ�Ĳ�������������˯��ģʽ���Ͽ����ӡ������㲥�Լ���������ƶ��źŵȡ�
 *
 * @param[in]   event   ��ť���²������¼����͡�
 */
void ble_device_buttons_event_handler(bsp_event_t event)
{
    ret_code_t err_code;  /**< ���ڴ洢�������صĴ�����롣 */

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            ble_device_enter_sleep_mode(); /**< ����˯��ģʽ�Խ�ʡ���ġ� */
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(module_connection_handle, /**< �Ͽ���ǰBLE���ӡ� */
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)        /**< ����Ƿ�����Ч״̬�µ����˶Ͽ������� */
            {
                APP_ERROR_CHECK(err_code); /**< ���Ͽ����Ӳ����Ƿ�ɹ��������򴥷������� */
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (module_connection_handle == BLE_CONN_HANDLE_INVALID)   /**< ���豸��ǰδ���ӣ������������������Ĺ㲥�� */
            {
                err_code = ble_advertising_restart_without_whitelist(&module_ble_advertising); /**< �����㲥����ʹ�ð������� */
                if (err_code != NRF_ERROR_INVALID_STATE)                             /**< ���㲥�����Ƿ�����Ч״̬�¡� */
                {
                    APP_ERROR_CHECK(err_code); /**< ���㲥���������Ƿ�ɹ��������򴥷������� */
                }
            }
            break;

        default:
            break;
    }
}

/**@brief ʹоƬ����˯��ģʽ�ĺ�����
 *
 * @details �ú�����оƬ����ϵͳ�رգ�system-off��ģʽ����ʵ����͹��ġ�
 *          ����ʱ�ᵼ��оƬ��λ���Ӷ���������Ӧ�ó���
 *
 * @note �ú������᷵�أ�һ������system-offģʽ���豸ֻ��ͨ���ⲿ�¼����簴ť���ѣ��ָ���
 */
void ble_device_enter_sleep_mode(void)
{
    ret_code_t err_code; /**< ���ڴ洢�������صĴ�����롣 */

    err_code = bsp_indication_set(BSP_INDICATE_IDLE); /**< �����豸�Ŀ���ָʾ��LED��ָʾ�ƣ��� */
    APP_ERROR_CHECK(err_code);

    // ׼�����Ѱ�ť��
    err_code = bsp_btn_ble_sleep_mode_prepare(); /**< ���ð�ť��������˯��ģʽ�»����豸�� */
    APP_ERROR_CHECK(err_code);

    // ����ϵͳ�ر�ģʽ���˺������᷵�أ����ѽ������豸��λ����
    err_code = sd_power_system_off(); /**< ����SoftDevice��������system-offģʽ�� */
    APP_ERROR_CHECK(err_code);
}


 /***************************************
 *         BLE ��ջ��Э��ģ�麯��        *	
 **************************************/ 

/**@brief ��ʼ��BLE��ջ�ĺ�����
 *
 * @details �ú�����������SoftDevice��Nordic��BLEЭ��ջ��������BLE��ջ��Ĭ�����ã���ע��BLE�¼��жϴ������
 */
void ble_device_ble_stack_init(void)
{
    ret_code_t err_code;  /**< ���ڴ洢�������صĴ�����롣 */

    err_code = nrf_sdh_enable_request();  /**< ����SoftDevice�Լ���BLEЭ��ջ�� */
    APP_ERROR_CHECK(err_code);            /**< ���SoftDevice�Ƿ�ɹ����ã������򴥷������� */

    // ����BLE��ջ��ʹ��Ĭ�����á���ȡӦ�ó���RAM����ʼ��ַ��
    uint32_t ram_start = 0;               /**< Ӧ�ó���RAM����ʼ��ַ����BLE��ջ����ʹ�á� */
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);  /**< ����Ĭ��BLE�������ã�������RAM��ʼ��ַ�� */
    APP_ERROR_CHECK(err_code);            /**< ���BLE��ջ�����Ƿ�ɹ��������򴥷������� */

    // ����BLE��ջ��
    err_code = nrf_sdh_ble_enable(&ram_start);  /**< ���ݵ������RAM��ʼ��ַ����BLE��ջ�� */
    APP_ERROR_CHECK(err_code);                  /**< ���BLE��ջ�Ƿ�ɹ����ã������򴥷������� */

    // ע��BLE�¼��������
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_device_observer_event_handler, NULL);  /**< ע��BLE�¼��۲��ߣ������¼����ȼ��ʹ������� */
}

/**@brief ����BLE�¼��ĺ�����
 *
 * @details �ú������ݲ�ͬ��BLE�¼�����ִ����Ӧ�Ĳ������������ӡ��Ͽ����ӡ�PHY���������GATT��ʱ����ȡ�
 *
 * @param[in]   p_ble_evt   ָ��BLE��ջ�¼���ָ�룬�����¼����ͼ������Ϣ��
 * @param[in]   p_context   δʹ�õĲ�����
 */
void ble_device_observer_event_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;  /**< ���ڴ洢�������صĴ�����롣 */

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");  /**< ��־��Ϣ�������ӡ� */
						NRF_LOG_FLUSH();
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED); /**< ��������״̬��LEDָʾ�� */
            APP_ERROR_CHECK(err_code);  /**< ���LEDָʾ�Ƿ�ɹ��������򴥷������� */

            module_connection_handle = p_ble_evt->evt.gap_evt.conn_handle; /**< ���浱ǰ���Ӿ���� */

            err_code = nrf_ble_qwr_conn_handle_assign(&module_queue_write, module_connection_handle); /**< �����Ӿ�������Queued Writeģ�顣 ��Ҫ���͵����ݳ�������ʱ����Ҫʹ�ó�д���ֶδ���*/
            APP_ERROR_CHECK(err_code); /**< ����������Ƿ�ɹ��������򴥷������� */
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");  /**< ��־��Ϣ���ѶϿ����ӡ� */
            // LEDָʾ���ڹ㲥����ʱ���ġ�

            module_connection_handle = BLE_CONN_HANDLE_INVALID; /**< �����Ӿ������Ϊ��Ч����ʾ��ǰ�����ӡ� */
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request."); /**< ��־��Ϣ��PHY�������� */
            ble_gap_phys_t const phys =          /**< ����PHY����Ϊ�Զ�ѡ��ģʽ�� */
            {
                .rx_phys = BLE_GAP_PHY_AUTO,     /**< ����PHY����Ϊ�Զ�ѡ�� */
                .tx_phys = BLE_GAP_PHY_AUTO,     /**< ����PHY����Ϊ�Զ�ѡ�� */
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys); /**< ����PHY�������� */
            APP_ERROR_CHECK(err_code); /**< ���PHY���������Ƿ�ɹ��������򴥷������� */
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            NRF_LOG_DEBUG("GATT Client Timeout."); /**< ��־��Ϣ��GATT�ͻ��˳�ʱ�� */
            // GATT�ͻ��˳�ʱ�¼��������Ͽ����ӡ�
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code); /**< ���Ͽ����Ӳ����Ƿ�ɹ��������򴥷������� */
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            NRF_LOG_DEBUG("GATT Server Timeout."); /**< ��־��Ϣ��GATT��������ʱ�� */
            // GATT��������ʱ�¼��������Ͽ����ӡ�
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code); /**< ���Ͽ����Ӳ����Ƿ�ɹ��������򴥷������� */
            break;

        default:
            // ����ʵ�������¼���
            break;
    }
}

/**@brief ��ʼ��GAP��ͨ�÷��������ļ��������ĺ�����
 *
 * @details �ú��������豸��GAP�����������豸���ơ�����Լ���ѡ���Ӳ�����
 */
void ble_device_gap_init(void)
{
    ret_code_t              err_code;          /**< ���ڴ洢�������صĴ�����롣 */
    ble_gap_conn_params_t   gap_conn_params;   /**< GAP���Ӳ����ṹ�塣 */
    ble_gap_conn_sec_mode_t sec_mode;          /**< GAP���Ӱ�ȫģʽ�� */

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode); /**< �������Ӱ�ȫģʽΪ���ţ�������Լ������ӡ� */

    err_code = sd_ble_gap_device_name_set(&sec_mode,               /**< ����softdevice�豸���Ƶİ�ȫģʽ�� */
                                          (const uint8_t *)DEVICE_NAME, /**< ����softdevice�豸���ơ� */
                                          strlen(DEVICE_NAME));         /**< ����softdevice�豸���Ƶĳ��ȡ� */
    APP_ERROR_CHECK(err_code);         /**< ����豸���������Ƿ�ɹ��������򴥷������� */

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HID_MOUSE); /**< �����豸���ΪHID��ꡣ */
    APP_ERROR_CHECK(err_code);         /**< �����������Ƿ�ɹ��������򴥷������� */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params)); /**< �����Ӳ����ṹ�������Գ�ʼ���� */

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL; /**< ������С���Ӽ���� */
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL; /**< ����������Ӽ���� */
    gap_conn_params.slave_latency     = SLAVE_LATENCY;     /**< ���ô��豸�ӳ٣��Լ���ͨ��Ƶ�ʡ� */
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;  /**< �������ӳ�ʱ���������ӶϿ�ʱ�䡣 */

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);      /**< ������ѡ���Ӳ�����PPCP�����������豸�ο���BLE ���豸����ͨ�� PPCP �������Ӳ����������豸��һ���ϸ���ѭ��Щ���� */
    APP_ERROR_CHECK(err_code);         /**< ������Ӳ��������Ƿ�ɹ��������򴥷������� */
}

/**@brief ��ʼ��GATT��ͨ�����������ļ���ģ��ĺ�����
 *
 * @details �ú��������ʼ��GATTģ�飬�Ա��豸����ʹ��BLE��GATT���ܽ������ݴ��䡣
 *          ��ʼ��������δָ���Զ����¼������������GATTģ�齫ʹ��Ĭ�����á�
 */
void ble_device_gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&module_gatt, NULL); /**< ��ʼ��GATTģ�飬�ڶ�������Ϊ�¼�������򣬴˴�ΪNULL��ʾʹ��Ĭ�����á� */
    APP_ERROR_CHECK(err_code);                             /**< ���GATT��ʼ���Ƿ�ɹ��������򴥷������� */
}

/**@brief ��ʼ�����Ӳ���ģ��ĺ�����
 *
 * @details �ú������úͳ�ʼ�����Ӳ���ģ�飬�Ա��������ڼ䶯̬�������Ӳ�����
 *          ������Ӳ����ĸ�������ʧ�ܣ��豸����Ͽ����ӣ����Ǽ���ʹ�����еĲ�����
*						���ӽ������ٸ��²��������Ը����豸�ĵ�ǰ״̬���紫�����ݡ��͹��ĵȣ�����������ʵ����Ӳ������Ա��ڴ������ʺ͹���֮���ҵ����ƽ��
 */
void ble_device_connection_parameters_init(void)
{
    ret_code_t             err_code;    /**< ���ڴ洢�������صĴ�����롣 */
    ble_conn_params_init_t cp_init;     /**< ���Ӳ�����ʼ���ṹ�壬�����������Ӳ���ģ�顣 */

    memset(&cp_init, 0, sizeof(cp_init)); /**< �����Ӳ�����ʼ���ṹ�����㡣 */

    cp_init.p_conn_params                  = NULL;                     /**< ָ���Զ������Ӳ�����ָ�롣�������Ϊ NULL����ʹ�� BLE Э��ջ�е�Ĭ�����Ӳ����� */
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY; /**< ���θ������Ӳ������ӳ�ʱ��,�豸�����Ӻ��ʱ�״γ��Ը������Ӳ����� */
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;  /**< �����������Ӳ������ӳ�ʱ��, �����һ�θ���ʧ�ܣ�ģ���������ӳ�ʱ����ٴγ��Ը������Ӳ����� */
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAM_UPDATE_COUNT;   /**< ������Ӳ������³��Դ���, ����������������Ȼʧ�ܣ��豸�����ٳ��Ը��²Ρ� */
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;       /**< ����Ϊ��Ч�������ʾ������֪ͨ�����������Ӳ������½����������õ�ʱ�������� */
    cp_init.disconnect_on_fail             = false;                         /**< ������ʧ���򲻶Ͽ����ӡ� */
    cp_init.evt_handler                    = NULL;                          /**< ��ʹ���Զ����¼��������� */
    cp_init.error_handler                  = ble_device_connection_parameters_error_handler;     /**< ָ������������ */

    err_code = ble_conn_params_init(&cp_init); /**< ��ʼ�����Ӳ���ģ�顣 */
    APP_ERROR_CHECK(err_code);                 /**< ������Ӳ�����ʼ���Ƿ�ɹ��������򴥷������� */
}

/**@brief �������Ӳ���ģ�����ĺ�����
 *
 * @param[in]   nrf_error   ����������Ϣ�Ĵ�����롣
 *
 * @details ������Ӳ���ģ���ʼ�������й����г��ִ��󣬴˺���������ô���
 */
void ble_device_connection_parameters_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error); /**< ���ô�������򲢴��������롣 */
}

 /***************************************
 *         BLE �㲥ģ�麯��        		*	
 **************************************/ 
/**@brief ��ʼ��BLE�㲥���ܵĺ�����
 *
 * @details �ú������ù㲥���ݡ��㲥ģʽ�Ͷ�ʱ���Ȳ������������͹���BLE�豸�Ĺ㲥��
 */
void ble_device_advertising_init(void)
{
    ret_code_t             err_code;     /**< ���ڴ洢�������صĴ�����롣 */
    uint8_t                adv_flags;    /**< �㲥��־������ָʾ�㲥ģʽ���豸�ɼ��ԡ� */
    ble_advertising_init_t init;         /**< �㲥��ʼ���ṹ�壬�����㲥���ú����ݡ� */

    memset(&init, 0, sizeof(init));      /**< ���㲥��ʼ���ṹ�����㡣 */

    adv_flags                            = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE; /**< ���ù㲥��־�����޵͹���BLEģʽ�� */
    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;        /**< �ڹ㲥�����а����豸���������ơ� */
    init.advdata.include_appearance      = true;                         /**< �ڹ㲥�����а����豸��ۡ� */
    init.advdata.flags                   = adv_flags;                    /**< ���ù㲥��־�� */
    init.advdata.uuids_complete.uuid_cnt = sizeof(module_advertising_uuids) / sizeof(module_advertising_uuids[0]); /**< �㲥�а�����UUID������ */
    init.advdata.uuids_complete.p_uuids  = module_advertising_uuids;                  /**< �㲥�а�����UUID�б� */

#if SWIFT_PAIR_SUPPORTED == 1
    init.advdata.p_manuf_specific_data = &module_swift_pair_manufacturer_advertising_data;            /**< ���֧��Swift Pair���������Զ���������ӵ��㲥�С� */
    memcpy(&module_swift_pair_advertising_data, &init.advdata, sizeof(module_swift_pair_advertising_data));          /**< ���ƹ㲥���ݽṹ�塣 */
#endif

    // ���ù㲥ģʽ��
    init.config.ble_adv_whitelist_enabled          = true;               /**< ���ð������㲥ģʽ��ֻ���ڰ������е��豸����Ԥ����Ի�������豸������ɨ�赽�ù㲥 */
    init.config.ble_adv_directed_high_duty_enabled = true;               /**< ���ø�ռ�ձȶ���㲥�����ڿ�����������ģʽ�Ĺ㲥Ƶ�ʺܸߣ��ܹ����ٱ���һ�������豸ʶ�𲢽������� */
    init.config.ble_adv_directed_enabled           = false;              /**< ������ͨ����㲥����ͨ����㲥��һ�ֽϵ�Ƶ�ʵĹ㲥ģʽ���㲥Ŀ������һ�����ӵ��豸�� */
    init.config.ble_adv_directed_interval          = 0;                  /**< ����㲥�ļ���������ã��� */
    init.config.ble_adv_directed_timeout           = 0;                  /**< ����㲥�ĳ�ʱʱ�䣨�����ã��� */
    init.config.ble_adv_fast_enabled               = true;               /**< ���ÿ��ٹ㲥ģʽ�����ٹ㲥ģʽ�Ĺ㲥����϶̣�ʹ���豸�����ױ�ɨ�赽�����ӡ�ͨ�����豸����������Ҫ���ٽ�������ʱʹ�á� */
    init.config.ble_adv_fast_interval              = APP_ADV_FAST_INTERVAL; /**< ���ÿ��ٹ㲥�ļ���� */
    init.config.ble_adv_fast_timeout               = APP_ADV_FAST_DURATION; /**< ���ÿ��ٹ㲥�ĳ���ʱ�䡣 */
    init.config.ble_adv_slow_enabled               = true;               /**< �������ٹ㲥ģʽ���������ڿ��ٹ㲥��ʱ����ϣ���豸���������ֵ���������ٹ㲥���������͹��ġ� */
    init.config.ble_adv_slow_interval              = APP_ADV_SLOW_INTERVAL; /**< �������ٹ㲥�ļ���� */
    init.config.ble_adv_slow_timeout               = APP_ADV_SLOW_DURATION; /**< �������ٹ㲥�ĳ���ʱ�䡣 */

    init.evt_handler   = ble_device_advertising_event_handler;                /**< ���ù㲥�¼�������� */
    init.error_handler = ble_device_advertising_error_handler; /**< ���ù㲥��������� */

    err_code = ble_advertising_init(&module_advertising, &init); /**< ��ʼ���㲥ģ�鲢���ù㲥���ݡ� */
    APP_ERROR_CHECK(err_code);                              /**< ���㲥��ʼ���Ƿ�ɹ��������򴥷������� */

    ble_advertising_conn_cfg_tag_set(&module_advertising, APP_BLE_CONN_CFG_TAG); /**< �����������ñ�ǩ����ƥ���������á� */
}

/**@brief ����㲥�¼��ĺ�����
 *
 * @details �ú������ڸ��ֹ㲥�¼�����ʱ�����ã�ִ����Ӧ�Ĺ㲥ָʾ�Ͳ�����������ͬģʽ�Ĺ㲥�Լ����д���
 *
 * @param[in] ble_adv_evt  �㲥�¼������͡�
 */
void ble_device_advertising_event_handler(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code; /**< ���ڴ洢�������صĴ�����롣 */

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_DIRECTED_HIGH_DUTY: /**< ��ռ�ձȶ���㲥�¼������ڿ�����������һ�������ӵ��豸 */
            NRF_LOG_INFO("Directed advertising."); /**< ��־��Ϣ������㲥�� */
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED); /**< ���ö���㲥ָʾ�� */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_FAST:		/**< ���ٹ㲥�¼��������豸����ʱ������Χ�豸�������� */
            NRF_LOG_INFO("Fast advertising."); /**< ��־��Ϣ�����ٹ㲥�� */
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING); /**< ���ÿ��ٹ㲥ָʾ�� */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_SLOW:	/**< ���ٹ㲥�¼������͹㲥Ƶ���Խ�ʡ���� */
            NRF_LOG_INFO("Slow advertising."); /**< ��־��Ϣ�����ٹ㲥�� */
#if SWIFT_PAIR_SUPPORTED == 1
            module_swift_pair_advertising_data.p_manuf_specific_data = NULL; /**< ���Swift Pair�ĳ������ݡ� */
            err_code = ble_advertising_advdata_update(&module_advertising, &module_swift_pair_advertising_data, NULL); /**< ���¹㲥���ݡ� */
            APP_ERROR_CHECK(err_code);
#endif
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW); /**< �������ٹ㲥ָʾ�� */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_FAST_WHITELIST: /**< ���ٰ������㲥�¼������㲥���������е��豸 */
            NRF_LOG_INFO("Fast advertising with whitelist."); /**< ��־��Ϣ�����ٰ������㲥�� */
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST); /**< ���ÿ��ٰ������㲥ָʾ�� */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_SLOW_WHITELIST: /**< ���ٰ������㲥�¼������㲥���������е��豸 */
            NRF_LOG_INFO("Slow advertising with whitelist."); /**< ��־��Ϣ�����ٰ������㲥�� */
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST); /**< �������ٰ������㲥ָʾ�� */
            APP_ERROR_CHECK(err_code);
            err_code = ble_advertising_restart_without_whitelist(&module_advertising); /**< ���������������Ĺ㲥�� */
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:	 /**< �㲥ֹͣ�¼� */
            err_code = bsp_indication_set(BSP_INDICATE_IDLE); /**< �����豸����ָʾ�� */
            APP_ERROR_CHECK(err_code);
            ble_device_enter_sleep_mode(); /**< ����˯��ģʽ�Խ�ʡ������ */
            break;

        case BLE_ADV_EVT_WHITELIST_REQUEST: /**< �¼����㲥�����������ַ�� IRK��Identity Resolving Key��,ʹ�ù���������������б��ų�û�� IRK ���豸�������µİ������б�Ӧ���ڹ㲥ģ�� */
				{
						// �� BLE �豸������˽ģʽʱ�����������Եظ����Լ����豸��ַ����Ϊ���˽�е�ַ����ʹ�������豸���Ը��ٵ�������ʵ���
						// IRK �����á�������������Ե��豸ͨ������˽�е�ַ��ʶ��Է�����ݣ��Ӷ����½�������
						ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT]; /**< �����������ַ���飬�洢�������ӵ��豸��ַ�� */
						ble_gap_irk_t  whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];  /**< ��������� IRK ���飬���ڴ洢��ݽ�����Կ�� */
						uint32_t       addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;       /**< ��������¼�������еĵ�ַ��������ʼ��Ϊ��������� */
						uint32_t       irk_cnt  = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;       /**< ��������¼�������е� IRK ��������ʼ��Ϊ��������� */
				
						err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt, whitelist_irks, &irk_cnt); /**< ��ȡ�������еĵ�ַ�� IRK���洢�������С� */
						APP_ERROR_CHECK(err_code); /**< ����ȡ�����������Ƿ�ɹ����������򴥷������� */
				
						NRF_LOG_DEBUG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist", addr_cnt, irk_cnt); /**< ��ӡ������Ϣ������������е�ַ�� IRK �������� */
				
						ble_device_set_filtered_device_identity_list(PM_PEER_ID_LIST_SKIP_NO_IRK); /**< ��������б����˵��� IRK ���豸�� */
				
						err_code = ble_advertising_whitelist_reply(&module_advertising, whitelist_addrs, addr_cnt, whitelist_irks, irk_cnt); /**< ����ȡ���İ���������Ӧ���ڹ㲥ģ�顣 */
						APP_ERROR_CHECK(err_code); /**< ��������Ӧ���Ƿ�ɹ����������򴥷������� */
				}
				break;


        case BLE_ADV_EVT_PEER_ADDR_REQUEST: /**< �¼����ڹ㲥������豸��ַʱ�����ز������Ѱ��豸�Ķ����ַ��ʹ�豸�ܹ�ִ�ж���㲥 */
				{
						pm_peer_data_bonding_t peer_bonding_data; /**< ��������ݽṹ�壬���ڴ洢�Ѱ��豸�������Ϣ�� */
				
						if (module_peer_device_id != PM_PEER_ID_INVALID) /**< ��鵱ǰ���豸 ID �Ƿ���Ч�� */
						{
								err_code = pm_peer_data_bonding_load(module_peer_device_id, &peer_bonding_data); /**< �� Peer Manager �����Ѱ��豸��������ݡ� */
								if (err_code != NRF_ERROR_NOT_FOUND) /**< ����Ƿ�ɹ������Ѱ��豸���ݣ�ȷ�����ݴ��ڡ� */
								{
										APP_ERROR_CHECK(err_code); /**< ���������ݳ����򴥷������� */
				
										ble_device_set_filtered_device_identity_list(PM_PEER_ID_LIST_SKIP_ALL); /**< ���ù����������ų������������ַ�������豸�� */
				
										ble_gap_addr_t * p_peer_addr = &(peer_bonding_data.peer_ble_id.id_addr_info); /**< ��ȡ�Ѱ��豸�ĵ�ַ��Ϣ�� */
										err_code = ble_advertising_peer_addr_reply(&module_advertising, p_peer_addr); /**< �����豸��ַ���ݸ��㲥ģ����ִ�ж���㲥�� */
										APP_ERROR_CHECK(err_code); /**< ���㲥ģ���ַ�����Ƿ�ɹ����������򴥷������� */
								}
						}
						break;
				}


        default:
            break;
    }
}

/**@brief ����㲥ģ�����ĺ�����
 *
 * @details �ú����ڹ㲥ģ�鷢������ʱ�����ã�������󲢴��ݴ�������ͨ�ô��������
 *
 * @param[in] nrf_error  ����������Ϣ�Ĵ�����롣
 */
void ble_device_advertising_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error); /**< ����ͨ�ô�������򲢴��������롣 */
}

/**@brief �����㲥�ĺ�����
 *
 * @details �ú��������Ƿ���Ҫ�������Ϣ�������㲥��
 *          ��� `erase_bonds` Ϊ true����ɾ�����а���Ϣ���㲥���ڰ�ɾ���ɹ����¼���PM_EVT_PEERS_DELETE_SUCCEEDED����������
 *          ����ֱ�����ð���������ʼ���ٹ㲥��
 *
 * @param[in] erase_bonds ���Ϊ true������ɾ������Ϣ�������㲥��
 */
void ble_device_advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        ble_device_delete_bonds(); /**< ɾ�����а���Ϣ�� */
        // �㲥���� PM_EVT_PEERS_DELETE_SUCCEEDED �¼�����ʱ������
    }
    else
    {
        ble_device_set_filtered_white_list(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR); /**< ���ð�����������ָ�����豸���ӡ� */

        ret_code_t ret = ble_advertising_start(&module_advertising, BLE_ADV_MODE_FAST); /**< �������ٹ㲥ģʽ�� */
        APP_ERROR_CHECK(ret); /**< ���㲥�����Ƿ�ɹ��������򴥷������� */
    }
}

 /***************************************
 *         		BLE ����������       			*	
 **************************************/ 
/**@brief ���ù��˺���豸����б�ĺ�����
 *
 * @details �ú�������ָ���Ĺ�����������ȡ�����������豸���ID�б�����������Ϊ��ǰ���豸����б�
 *          ��ʹ���豸����ʹ���ض��İ��������ԣ�������ų��ض��豸�����ӡ�
 *
 * @param[in] skip  ���������������ڹ���Peer Manager�е��豸ID�б�
 */
void ble_device_set_filtered_device_identity_list(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT]; /**< �豸���ID���顣 */
    uint32_t     peer_id_count = BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT; /**< �豸���ID����Ĵ�С�� */

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip); /**< ��ȡ�����������豸ID�б� */
    APP_ERROR_CHECK(err_code); /**< ����豸ID�б��ȡ�Ƿ�ɹ��������򴥷������� */

    err_code = pm_device_identities_list_set(peer_ids, peer_id_count); /**< ���õ�ǰ���豸����б��������ӹ��ˡ� */
    APP_ERROR_CHECK(err_code); /**< ����豸����б������Ƿ�ɹ��������򴥷������� */
}

/**@brief ���ù��˺�İ������ĺ�����
 *
 * @details �ú�������ָ���Ĺ���������ȡ�����������豸ID�б�����������Ϊ��������
 *          �������е��豸������BLE�������Զ���Ժ���������������ӵİ�ȫ�Ժ�Ч�ʡ�
 *
 * @param[in] skip  ���������������ڹ���Peer Manager�е��豸ID�б�
 */
void ble_device_set_filtered_white_list(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_WHITELIST_ADDR_MAX_COUNT]; /**< �������豸ID���顣 */
    uint32_t     peer_id_count = BLE_GAP_WHITELIST_ADDR_MAX_COUNT; /**< �������豸ID����Ĵ�С�� */

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip); /**< ��ȡ�����������豸ID�б� */
    APP_ERROR_CHECK(err_code); /**< ����豸ID�б��ȡ�Ƿ�ɹ��������򴥷������� */

    NRF_LOG_INFO("\tm_whitelist_peer_cnt %d, MAX_PEERS_WLIST %d", /**< ����������豸������Ϣ�����ڵ��ԡ� */
                 peer_id_count + 1,
                 BLE_GAP_WHITELIST_ADDR_MAX_COUNT);

    err_code = pm_whitelist_set(peer_ids, peer_id_count); /**< ���ð�����������������������豸���ӡ� */
    APP_ERROR_CHECK(err_code); /**< �������������Ƿ�ɹ��������򴥷������� */
}

 /***************************************
 *         		BLE ���ģ�麯��        *	
 **************************************/ 
/**@brief ��ʼ��Peer Managerģ��ĺ�����
 *
 * @details �ú�����ʼ��Peer Managerģ�飬�������豸�İ�ȫ����������Ժͼ���Ҫ�󣩣�
 *          ���ڹ����������豸����ԡ��󶨺Ͱ�ȫ���ӡ�
 */
void ble_device_peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;    /**< ��ȫ�����ṹ�壬������Ժͼ���Ҫ�� */
    ret_code_t           err_code;     /**< ���ڴ洢�������صĴ�����롣 */

    err_code = pm_init();              /**< ��ʼ��Peer Managerģ�顣 */
    APP_ERROR_CHECK(err_code);         /**< ���Peer Manager��ʼ���Ƿ�ɹ��������򴥷������� */

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t)); /**< ����ȫ�����ṹ�����㡣 */

    // �����������а�ȫ���̵İ�ȫ������
    sec_param.bond           = SEC_PARAM_BOND;          /**< �����Ƿ�󶨣��״���Ժ󱣴������Ϣ�������´�����ʱ����ֱ��ʹ��֮ǰ�ļ�����Կ������������ԡ� */
    sec_param.mitm           = SEC_PARAM_MITM;          /**< �Ƿ������м��˹������������� MITM �������Է�ֹ�м��˹�����ͨ����֤���˫����ݣ�ȷ�����ݴ���İ�ȫ�� */
    sec_param.lesc           = SEC_PARAM_LESC;          /**< �Ƿ�����LE��ȫ���ӣ�LE Secure Connections����LE Secure Connections ��һ�ָ��߼��ļ��ܷ�ʽ��ʹ����Բ���߼��ܣ�ECC������ǿ��Եİ�ȫ�� */
    sec_param.keypress       = SEC_PARAM_KEYPRESS;      /**< �Ƿ����ð���֪ͨ������֪ͨ�����豸����Թ����д��ݰ��������״̬��Ϣ��ͨ���������ʱ��Ҫ�û�ȷ�ϵĳ����������� PIN �� */
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES; /**< �����豸��������������������������������ȷ����Եİ�ȫ���𣬾�������Թ������Ƿ���Ҫ�û������������� PIN ��򰴼�ȷ�ϣ� */
    sec_param.oob            = SEC_PARAM_OOB;           /**< �Ƿ�ʹ��OOB��Out-of-Band�����ݽ�����ԣ�OOB ������� BLE ֮���ͨ������ NFC���н���������Ϣ���������ݱ������������԰�ȫ�� */
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;  /**< ���ü�����Կ����С���� ��������Թ��������ɵļ�����Կ�ĳ��ȣ���ȷ�����ܵ�ǿ�ȡ���ԿԽ������ȫ��Խ��*/
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;  /**< ���ü�����Կ����󳤶ȡ� */
    sec_param.kdist_own.enc  = 1;                       /**< �����豸�Լ��ַ�������Կ���ַ�������Կ�������豸���Լ��ļ�����Կ���͸������豸���Ա㽨���������� */
    sec_param.kdist_own.id   = 1;                       /**< �����豸�Լ��ַ������Ϣ���ַ������Ϣ�������豸�������Ϣ���͸������豸���Ա��豸������ʱ���������֤ */
    sec_param.kdist_peer.enc = 1;                       /**< ���������豸�ַ�������Կ��Ҫ�������豸�ַ�������Կ��ȷ���豸�ܹ����ܴ�������� */
    sec_param.kdist_peer.id  = 1;                       /**< ���������豸�ַ������Ϣ���������豸�ַ������Ϣ���Ա��豸����ʶ�����֤�����豸����� */

    err_code = pm_sec_params_set(&sec_param);           /**< ����Peer Manager�İ�ȫ������ */
    APP_ERROR_CHECK(err_code);                          /**< ��鰲ȫ���������Ƿ�ɹ��������򴥷������� */

    err_code = pm_register(ble_device_peer_manager_event_handler);             /**< ע��Peer Manager�¼��������� */
    APP_ERROR_CHECK(err_code);                          /**< ����¼�������ע���Ƿ�ɹ��������򴥷������� */
}

/**@brief ����Peer Manager�¼��ĺ�����
 *
 * @details �ú�������Peer Manager�¼�����ִ����Ӧ�����������������Ӱ�ȫ�ɹ���ɾ�����а������Լ����°����ݵ��¼���
 *
 * @param[in] p_evt  Peer Manager�¼��ṹ��ָ�룬�����¼����ͺ�������ݡ�
 */
void ble_device_peer_manager_event_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);                   /**< ����ͨ��Peer Manager�¼��� */
    pm_handler_disconnect_on_sec_failure(p_evt);   /**< �����ȫ����ʧ����Ͽ����ӡ� */
    pm_handler_flash_clean(p_evt);                 /**< ����Ҫʱ����Flash�洢�ռ䡣 */

    switch (p_evt->evt_id)
    {
        case PM_EVT_CONN_SEC_SUCCEEDED:
            module_peer_device_id = p_evt->peer_id; /**< ���Ӱ�ȫ��֤�ɹ��󣬴洢��ǰ�豸��peer ID�� */
            break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            ble_device_advertising_start(false); /**< ɾ�����а����ݳɹ������������㲥�� */
            break;

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            if (p_evt->params.peer_data_update_succeeded.flash_changed &&
                (p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING))
            {
                NRF_LOG_INFO("New Bond, add the peer to the whitelist if possible"); /**< �½��󶨺󣬽��豸��ӵ���������������Ҫ���� */
                
                 ble_device_set_filtered_white_list(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR); /**< ���ð�����������󶨵��豸���ӡ� */
            }
            break;

        default:
            break;
    }
}

/**@brief �ӳ־û��洢���������Ϣ�ĺ�����
 *
 * @details �ú���ɾ���豸����������������豸�İ���Ϣ���Ӷ������Լ�¼��
 *          ɾ����ɺ�Peer Manager������ PM_EVT_PEERS_DELETE_SUCCEEDED �¼�������֪ͨ�㲥ģ�����¿�ʼ�㲥��
 */
void ble_device_delete_bonds(void)
{
    ret_code_t err_code; /**< ���ڴ洢�������صĴ�����롣 */

    NRF_LOG_INFO("Erase bonds!"); /**< ��־��Ϣ����ʼɾ������Ϣ�� */

    err_code = pm_peers_delete(); /**< ����Peer Manager�ӿ�ɾ�����а󶨵������豸��Ϣ�� */
    APP_ERROR_CHECK(err_code); /**< ���ɾ�������Ƿ�ɹ��������򴥷������� */
}

 /***************************************
 *         		BLE ����ģ�麯��        *	
 **************************************/ 
/**@brief ��ʼ��Queued Writeģ��ĺ�����
 *
 * @details �ú������ò���ʼ��Queued Writeģ�飬ʹ���ܹ�����д������
 *          Queued Writeģ��Ϊ�����ṩ����������֧����BLE�����а�ȫ����ϳ����ݡ�
 */
void ble_device_queue_write_init(void)
{
    ret_code_t         err_code;          /**< ���ڴ洢�������صĴ�����롣 */
    nrf_ble_qwr_init_t qwr_init_obj = {0}; /**< Queued Writeģ���ʼ���ṹ�壬��ʼ��Ϊ�㡣 */

    qwr_init_obj.error_handler = ble_device_queue_write_error_handler; /**< ���ô��������� */

    err_code = nrf_ble_qwr_init(&module_queue_write, &qwr_init_obj); /**< ��ʼ��Queued Writeģ�顣 */
    APP_ERROR_CHECK(err_code);                          /**< ���Queued Writeģ���ʼ���Ƿ�ɹ��������򴥷������� */
}

/**@brief ����Queued Writeģ�����ĺ�����
 *
 * @details �ú�����ָ�뽫���ݸ���Ҫ�������ķ���ʹ�÷����ڷ�������ʱ����֪ͨӦ�ó���
 *
 * @param[in]   nrf_error   ����������Ϣ�Ĵ�����롣
 */
void ble_device_queue_write_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error); /**< ���ô�������򲢴��������롣 */
}



/**@brief ��ʼ���豸��Ϣ����Device Information Service���ĺ�����
 *
 * @details �ú������ò���ʼ���豸��Ϣ���������ṩ�豸�Ļ�����Ϣ���������̡���ƷID�ȣ���
 *          �豸��Ϣ����ʹ�����豸�ܹ���ȡ�����豸�Ļ���������Ϣ��
 */
void ble_device_info_system_init(void)
{
    ret_code_t       err_code;            /**< ���ڴ洢�������صĴ�����롣 */
    ble_dis_init_t   dis_init_obj;        /**< �豸��Ϣ�����ʼ���ṹ�壬���������豸��Ϣ���� */
    ble_dis_pnp_id_t pnp_id;              /**< PnP ID�ṹ�壬���������豸�ĳ���ID����ƷID�Ͱ汾��Ϣ�� */

    pnp_id.vendor_id_source = PNP_ID_VENDOR_ID_SOURCE; /**< ���ó���ID��Դ��ͨ��Ϊ����SIGָ������ */
    pnp_id.vendor_id        = PNP_ID_VENDOR_ID;        /**< ���ó���ID�� */
    pnp_id.product_id       = PNP_ID_PRODUCT_ID;       /**< ���ò�ƷID�� */
    pnp_id.product_version  = PNP_ID_PRODUCT_VERSION;  /**< ���ò�Ʒ�汾�� */

    memset(&dis_init_obj, 0, sizeof(dis_init_obj));    /**< �����豸��Ϣ�����ʼ���ṹ�塣 */

    ble_srv_ascii_to_utf8(&dis_init_obj.manufact_name_str, MANUFACTURER_NAME); /**< �������������ơ� */
    dis_init_obj.p_pnp_id = &pnp_id; /**< ��PnP ID��Ϣ��ӵ��豸��Ϣ�����С� */

    dis_init_obj.dis_char_rd_sec = SEC_JUST_WORKS; /**< �����豸��Ϣ������ȡ�İ�ȫ���� */

    err_code = ble_dis_init(&dis_init_obj); /**< ��ʼ���豸��Ϣ����ģ�顣 */
    APP_ERROR_CHECK(err_code);              /**< ����豸��Ϣ�����ʼ���Ƿ�ɹ��������򴥷������� */
}


 /***************************************
 *         		BLE HIDģ�麯��        *	
 **************************************/ 
/**@brief ��ʼ��HID���˻��ӿ��豸������ĺ�����
 *
 * @details �ú�����ʼ��HID���񣬶����豸��HID����ӳ�䣬������갴ť���ƶ������֡�ý����Ƶȹ��ܡ�
 *          HID���������BLE������̵��豸���ṩ��׼��HIDЭ��֧�֡�
 */
void ble_device_hid_service_init(void)
{
    ret_code_t                err_code;         /**< ���ڴ洢�������صĴ�����롣 */
    ble_hids_init_t           hids_init_obj;    /**< HID�����ʼ���ṹ�壬��������HID���� */
    ble_hids_inp_rep_init_t * p_input_report;   /**< ���뱨��ָ�룬�������ø����뱨�����ԡ� */
    uint8_t                   hid_info_flags;   /**< HID��Ϣ��־λ�������豸�Ļ��Ѻ��������ԡ� */

    static ble_hids_inp_rep_init_t inp_rep_array[INPUT_REPORT_COUNT]; /**< ���뱨�����顣 */
    static uint8_t rep_map_data[] =  /**< HID����ӳ�䣬������갴�����ƶ������ֺ�ý����ơ� */
    {
        0x05, 0x01, // Usage Page (Generic Desktop)
        0x09, 0x02, // Usage (Mouse)

        // Report ID 1��ʹ��
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

        // Report ID 2: ����
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

        // Report ID 3: ����
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

    memset(inp_rep_array, 0, sizeof(inp_rep_array)); /**< �������뱨�����顣 */

    // ������갴�����뱨��
    p_input_report                      = &inp_rep_array[INPUT_REP_BUTTONS_INDEX];
    p_input_report->max_len             = INPUT_REP_BUTTONS_LEN;
    p_input_report->rep_ref.report_id   = INPUT_REP_REF_BUTTONS_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;
    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr      = SEC_JUST_WORKS;
    p_input_report->sec.rd      = SEC_JUST_WORKS;

    // ��������ƶ����뱨��
    p_input_report                      = &inp_rep_array[INPUT_REP_MOVEMENT_INDEX];
    p_input_report->max_len             = INPUT_REP_MOVEMENT_LEN;
    p_input_report->rep_ref.report_id   = INPUT_REP_REF_MOVEMENT_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;
    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr      = SEC_JUST_WORKS;
    p_input_report->sec.rd      = SEC_JUST_WORKS;

    // ����ý��������뱨��
    p_input_report                      = &inp_rep_array[INPUT_REP_MPLAYER_INDEX];
    p_input_report->max_len             = INPUT_REP_MEDIA_PLAYER_LEN;
    p_input_report->rep_ref.report_id   = INPUT_REP_REF_MPLAYER_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;
    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr      = SEC_JUST_WORKS;
    p_input_report->sec.rd      = SEC_JUST_WORKS;

    hid_info_flags = HID_INFO_FLAG_REMOTE_WAKE_MSK | HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK; /**< ����HID��Ϣ��־�� */

    memset(&hids_init_obj, 0, sizeof(hids_init_obj)); /**< ����HID�����ʼ������ */

    // ����HID����ĸ������
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

    // ���ð�ȫ�ȼ�
    hids_init_obj.rep_map.rd_sec         = SEC_JUST_WORKS;
    hids_init_obj.hid_information.rd_sec = SEC_JUST_WORKS;
    hids_init_obj.boot_mouse_inp_rep_sec.cccd_wr = SEC_JUST_WORKS;
    hids_init_obj.boot_mouse_inp_rep_sec.wr      = SEC_JUST_WORKS;
    hids_init_obj.boot_mouse_inp_rep_sec.rd      = SEC_JUST_WORKS;
    hids_init_obj.protocol_mode_rd_sec = SEC_JUST_WORKS;
    hids_init_obj.protocol_mode_wr_sec = SEC_JUST_WORKS;
    hids_init_obj.ctrl_point_wr_sec    = SEC_JUST_WORKS;

    err_code = ble_hids_init(&module_hid_service, &hids_init_obj); /**< ��ʼ��HID���� */
    APP_ERROR_CHECK(err_code); /**< ���HID�����ʼ���Ƿ�ɹ��������򴥷������� */
}

/**@brief ����HID�¼��ĺ�����
 *
 * @details �ú�����������HID�¼�����ʱ�����ã������¼�����ִ����Ӧ�Ĳ�����
 *          ��������Bootģʽ��Reportģʽ������֪ͨ�ȡ�
 *
 * @param[in]   p_hids  HID����ṹ��ָ�롣
 * @param[in]   p_evt   ��HID������յ����¼���
 */
void ble_device_hid_service_event_handler(ble_hids_t * p_hids, ble_hids_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_HIDS_EVT_BOOT_MODE_ENTERED:
            mark_in_boot_mode = true; /**< ����豸����Bootģʽ��Bootģʽ�����ڻ������ݴ��䡣 */
            break;

        case BLE_HIDS_EVT_REPORT_MODE_ENTERED:
            mark_in_boot_mode = false; /**< ����豸����Reportģʽ��Reportģʽ֧�ָ�����Զ������ݴ��䡣 */
            break;

        case BLE_HIDS_EVT_NOTIF_ENABLED:
            // ����֪ͨ���¼��������ڴ���Ӵ����߼�����׼���������ݣ���
            break;

        default:
            // ����Ҫ�����¼��Ĵ���
            break;
    }
}

/**@brief ����������ĺ�����
 *
 * @details �ú�����ָ�뽫���ݸ���Ҫ�������ķ���ʹ�÷����ڷ�������ʱ����֪ͨӦ�ó���
 *
 * @param[in]   nrf_error   ����������Ϣ�Ĵ�����롣
 */
void ble_device_hid_service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error); /**< ����ͨ�ô�������򲢴��������롣 */
}

 /***************************************
 *         BLE �¼�������ģ�麯��        *	
 **************************************/ 
/**@brief ��ʼ���¼��������ĺ�����
 *
 * @details �ú�����ʼ���¼������������ڹ���͵���ϵͳ�е��첽�¼���
 *          �����������¼�����ѭ����˳�����������ж���ֱ��ִ�нϳ��Ĵ��롣
 */
void ble_device_scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE); /**< ��ʼ���������������¼����ݵ�����С�Ͷ��д�С�� */
}

/****************************************
 *             FreeRTOS����  			   		*
 ****************************************/
