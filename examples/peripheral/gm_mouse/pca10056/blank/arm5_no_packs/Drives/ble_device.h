#ifndef BLE_DEVICE_H
#define BLE_DEVICE_H

/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include <stdint.h>   // ��׼�������Ͷ���
#include <stdbool.h>  // �������Ͷ���
#include <stddef.h>   // ��׼���壨NULL�ȣ�

// Nordic��
#include "nordic_common.h"  // ����NordicоƬ��ͨ�ö���͹��ܣ��繤�ߺ�ȡ�
#include "nrf.h"            // �ṩNordicоƬ�ļĴ������ڴ�ӳ�䶨�塣
#include "nrf_sdm.h"        // �ṩSoftDevice��Nordic BLEЭ��ջ���Ĺ����ܡ�
#include "app_error.h"      // �ṩ������ʹ������ܣ����ڵ��Ժʹ�����־��¼��
#include "nrf_delay.h"			// nrf�ӳٿ�

// BLE������
#include "ble.h"            // ����BLEЭ��ջ�Ļ���API���ṩBLE�¼��ͽṹ���塣
#include "ble_err.h"        // ����BLE�����룬���ڴ�����ʹ���
#include "ble_hci.h"        // ����HCI���������ƽӿڣ�Э��Ĵ����롣
#include "ble_srv_common.h" // BLE�����ͨ�ýӿڣ����ڴ����͹���BLE����
#include "ble_advdata.h"    // �ṩBLE�㲥���ݵ����ù��ܡ�

// BLE�����
#include "ble_hids.h"       // BLE HID��Human Interface Device������Ľӿڣ�����ʵ�ּ��̻�����HID�豸��
#include "ble_bas.h"        // BLE��ط���ӿڣ��ṩ�豸��ص�����Ϣ��
#include "ble_dis.h"        // BLE�豸��Ϣ����ӿڣ��ṩ�豸�Ļ�����Ϣ���������̡��ͺŵȣ���

// BLE�����
#include "ble_conn_params.h"// ����BLE���Ӳ������������Ӽ������ʱ�ȡ�
#include "bsp_btn_ble.h"    // ������BLE��صİ����¼����������ӡ��Ͽ��Ȳ�����
#include "app_scheduler.h"  // Ӧ�ó�������������ڽ����������в���ִ�С�
#include "nrf_sdh.h"        // SoftDevice handler�����ṩSoftDevice��ʼ�����¼�����
#include "nrf_sdh_soc.h"    // �ṩSoC��ϵͳ��оƬ����ص��¼������ܡ�
#include "nrf_sdh_ble.h"    // SoftDevice��BLE����ӿڣ���ʼ��BLE������BLE�¼��ȡ�
#include "ble_advertising.h"// BLE�㲥���ܹ������ù㲥���ݲ����ƹ㲥״̬��
#include "ble_conn_state.h" // ����״̬�����ṩ����״̬��ѯ�ȹ��ܡ�
#include "peer_manager.h"   // ����BLE�豸����Ժͼ������ӡ�

// ϵͳ�����
#include "app_timer.h"      // Ӧ�ó���ʱ���ӿڣ��ṩ��ʱ������֧�֡�
#include "fds.h"            // Flash���ݴ洢�ӿڣ������������д洢������Ϣ�����ݡ�

// GATT���� 
#include "nrf_ble_gatt.h"   // GATTģ������ṩGATT���ӵ����ú����ݴ��䡣
#include "nrf_ble_qwr.h"    // ����д��ģ�飬��GATT��д����������д������

// ��Դ����
#include "nrf_pwr_mgmt.h"   // ��Դ����ģ�飬�ṩ�͹��ĺ͵�Դ�����ܡ�

// ��Թ���
#include "peer_manager_handler.h"  // ��Թ�������򣬴�����ԺͰ�ȫ������¼���

// ��־����
#include "nrf_log.h"             // ��־ģ�飬���ڼ�¼���Ժ�������Ϣ��
#include "nrf_log_ctrl.h"        // ��־����ģ�飬���ڿ�����־��¼��������ֹͣ��
#include "nrf_log_default_backends.h" // ��־��˹���������־����ľ���λ�ã��紮�ڻ�洢��

// ���Թ���
#include "sensorsim.h"      // ģ�⴫�����������ɣ����ڲ��Դ��������Ӧ�á�

// LEDָʾ��
#include "ws2812b.h" // WS2812B����

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
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)            /**< @brief ��С���Ӽ�� */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)             /**< @brief ������Ӽ�� */
#define SLAVE_LATENCY                   0                                          	/**< @brief ���豸�ӳ١� */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(3000, UNIT_10_MS)             /**< @brief ���Ӽල��ʱ��3000���룩�� */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< @brief �Ӵ����¼������ӻ�֪ͨ��ʼ�����״ε������Ӳ������µ�ʱ�䣨5�룩�� */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< @brief �״����Ӳ������º�ÿ�ε��õ�ʱ������30�룩�� */
#define MAX_CONN_PARAM_UPDATE_COUNT     5                                           /**< @brief �ڷ������Ӳ���Э��֮ǰ������Դ����� */

// Swift Pair���ܶ���
#define SWIFT_PAIR_SUPPORTED            1                                           /**< @brief ֧��Swift Pair���ܡ� */
#if SWIFT_PAIR_SUPPORTED == 1
#define MICROSOFT_VENDOR_ID             0x0006                                      /**< @brief Microsoft��Ӧ��ID�� */
#define MICROSOFT_BEACON_ID             0x03                                        /**< @brief Microsoft Beacon ID����ʾ֧��Swift Pair���ܡ� */
#define MICROSOFT_BEACON_SUB_SCENARIO   0x00                                        /**< @brief Microsoft Beacon�ӳ���������ָʾ���ͨ��Swift Pair��ԡ� */
#define RESERVED_RSSI_BYTE              0x80                                        /**< @brief ������RSSI�ֽڣ����ڱ�����ǰ���������ԡ� */
#endif

// �㲥��������
#define APP_ADV_DIRECT_INTERVAL         0x0020                                   	/**< @brief ֱ�ӹ㲥�������λΪ0.625���룬��Ӧ��20���룩�� */
#define APP_ADV_FAST_INTERVAL           0x0028                                      /**< @brief ���ٹ㲥�������λΪ0.625���룬��Ӧ��25���룩�� */
#define APP_ADV_SLOW_INTERVAL           0x00A0                                      /**< @brief ���ٹ㲥�������λΪ0.625���룬��Ӧ��100���룩�� */
#define APP_ADV_DIRECT_DURATION         1000                                        /**< @brief ֱ�ӹ㲥�ĳ���ʱ�䣬��λΪ10���롣 */
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
#define INPUT_REP_BUTTONS_LEN           3                                           /**< @brief ������ť���ݵ�������뱨��ĳ��ȡ� */
#define INPUT_REP_MOVEMENT_LEN          4                                           /**< @brief �����˶����ݵ�������뱨��ĳ��ȡ� */
#define INPUT_REP_MEDIA_PLAYER_LEN      1                                           /**< @brief ����ý�岥�������ݵ�������뱨��ĳ��ȡ� */
#define INPUT_REP_BUTTONS_INDEX         0                                           /**< @brief ������ť���ݵ�������뱨��������� */
#define INPUT_REP_MOVEMENT_INDEX        1                                           /**< @brief �����˶����ݵ�������뱨��������� */
#define INPUT_REP_MPLAYER_INDEX         2                                           /**< @brief ����ý�岥�������ݵ�������뱨��������� */
#define INPUT_REP_REF_BUTTONS_ID        1                                           /**< @brief ������ť���ݵ�������뱨�������ID�� */
#define INPUT_REP_REF_MOVEMENT_ID       2                                           /**< @brief �����˶����ݵ�������뱨�������ID�� */
#define INPUT_REP_REF_MPLAYER_ID        3                                           /**< @brief ����ý�岥�������ݵ�������뱨�������ID�� */


 /****************************************
 *                 ����  			           *
 ****************************************/
// BLE ��־�������� 
void ble_device_set_mark_in_boot_mode(bool boot_mode);
bool ble_device_get_mark_in_boot_mode(void);
bool ble_device_get_mark_device_disconnected(void);
void ble_device_set_mark_device_disconnected(bool is_disconncted);
bool ble_device_get_mark_need_host_delete(void);
void ble_device_set_mark_need_host_delete(bool need_delete);

// BLE ģ��������� 
ble_hids_t* ble_device_get_module_hid_service(void);
uint16_t ble_device_get_module_connection_handle(void);
const app_timer_id_t* ble_device_get_module_led_timer_handle(void);

// BLE ��غ���
void ble_device_battery_timer_timeout_handler(void * p_context);
void ble_device_battery_level_update(uint8_t  battery_level);
void ble_device_battery_service_init(void);
void ble_device_battery_sensor_simulator_init(void);  

// BLE ���ʹ�����
void ble_device_power_management_init(void);

// BLE ��ջ��Э��ģ�麯��
void ble_device_ble_stack_init(void);
void ble_device_observer_event_handler(ble_evt_t const * p_ble_evt, void * p_context);
void ble_device_gap_init(void);
void ble_device_gatt_init(void);
void ble_device_connection_parameters_init(void);
void ble_device_connection_parameters_error_handler(uint32_t nrf_error);
	
// BLE ģʽ��ť��LED���� 
void ble_device_disconnect(void);
void ble_device_enter_sleep_mode(void);
void ble_device_led_timer_handler(void *p_context);
void ble_device_led_timer_stop(void);

// BLE �㲥����
void ble_device_advertising_init(void);
void ble_device_advertising_event_handler(ble_adv_evt_t ble_adv_evt);
void ble_device_advertising_error_handler(uint32_t nrf_error);
void ble_device_advertising_start(bool erase_bonds, bool use_whitelist);
bool has_valid_peer(void);
bool has_device_identities(void);

// BLE ����������
void ble_device_set_filtered_device_identity_list(pm_peer_id_list_skip_t skip);
void ble_device_set_filtered_white_list(pm_peer_id_list_skip_t skip);

// BLE ���ģ�麯��
void ble_device_peer_manager_init(void);
void ble_device_peer_manager_event_handler(pm_evt_t const * p_evt);
void ble_device_delete_bonds(void);
void ble_device_stop_advertising(void);
void ble_device_generate_random_private_address(void);
uint32_t ble_device_get_peer_count(void);

// BLE ����ģ�麯�� 
void ble_device_queue_write_init(void);
void ble_device_queue_write_error_handler(uint32_t nrf_error);
void ble_device_info_system_init(void);

// BLE HIDģ�麯��
void ble_device_hid_service_init(void);
void ble_device_hid_service_event_handler(ble_hids_t * p_hids, ble_hids_evt_t * p_evt);
void ble_device_hid_service_error_handler(uint32_t nrf_error);

// BLE ��ʼ������
void ble_device_timer_init(void);
void ble_device_idle_state_handle(void);
void ble_device_init(void);
	
#endif // BLE_DEVICE_H
