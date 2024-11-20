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
