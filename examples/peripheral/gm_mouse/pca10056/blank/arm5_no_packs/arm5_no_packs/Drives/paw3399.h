#ifndef PAW3399_H
#define PAW3399_H

/***************************************
 *              ���ļ�             		 *
 ***************************************/
#include <stdint.h>
#include <stdbool.h>  				// ����������ʹ�� bool, true, false
#include "nrfx_spim.h"        // SPIM ������
#include "nrf_gpio.h"           // GPIO ���ƿ�
#include "nrf_delay.h"          // ��ʱ��
#include "app_error.h"          // �������
#include <string.h>             // �ַ��������
#include "nrf_log.h"            // ��־��
#include "nrf_log_ctrl.h"       // ��־���ƿ�
#include "nrf_log_default_backends.h"  // ��־��˿�

/****************************************
 *             SPIM����                  *
 ****************************************/
#define SPIM_INSTANCE  3  /**< SPIM ʵ������ */

 /****************************************
 *                 ����  			           *
 ****************************************/
// ��������ʼ������
void paw3399_init(void);
	
// ������ȡ����
uint8_t* paw3399_get_var_motion_burst_data(void);
uint8_t* paw3399_get_var_raw_data(void);
uint8_t paw3399_get_var_var_a(void);
uint8_t paw3399_get_var_var_b(void);
int16_t paw3399_get_var_delta_x(void);
int16_t paw3399_get_var_delta_y(void);
uint16_t paw3399_get_var_x_resolution(void);
uint16_t paw3399_get_var_y_resolution(void);
uint16_t paw3399_get_var_run_downshift_mult(void);
uint16_t paw3399_get_var_rest1_downshift_mult(void);
uint16_t paw3399_get_var_rest2_downshift_mult(void);

// �����ʼ������
void paw3399_gpio_init(void);
void paw3399_spim_init(void);

// ������״̬����
void paw3399_reset(void);
void paw3399_power_up(void);
void paw3399_shutdown(void);
void paw3399_power_up_from_shutdown(void);
bool paw3399_chip_observation(void);
uint8_t paw3399_get_op_mode(void);

// ������ģʽ����
void paw3399_high_performance_mode(void);
void paw3399_low_power_mode(void);
void paw3399_office_mode(void);
void paw3399_corded_gaming_mode(void);
uint8_t paw3399_get_mode(void);

// ��������������
uint8_t paw3399_read(uint8_t address);
void paw3399_write(uint8_t address, uint8_t value);
void paw3399_motion_burst(void);
void paw3399_write_with_mask(uint8_t reg_address, uint8_t mask, uint8_t value);

// ��������Ϣ����
void paw3399_print_pin_info(void);
uint8_t paw3399_get_product_id(void);
uint8_t paw3399_get_inv_product_id(void);
uint8_t paw3399_get_revision_id(void);

// ������Ϣ����
void paw3399_get_rawdata(void);
uint16_t paw3399_get_rawdata_sum(void);
uint8_t paw3399_get_max_rawdata(void);
uint8_t paw3399_get_min_rawdata(void);
uint16_t paw3399_get_squal(void);

// XYƫ�ƺ���
void paw3399_get_delta_xy(void);

// CPI/DPI����
void paw3399_get_xy_resolution(void);
bool paw3399_set_xy_resolution(uint16_t new_x_resolution, uint16_t new_y_resolution);
bool paw3399_get_res_mod_different_src(void);
void paw3399_set_res_mod_different_src(bool different_src);

// ̧���жϺ���
void paw3399_manual_lift_cut_off_calibration(void);
void paw3399_enable_lift_cut_off_calibration_setting(void);
void paw3399_disable_lift_cut_off_calibration_setting(void);
void paw3399_set_lift_config(uint8_t lift_setting);
uint8_t paw3399_get_lift_config(void);
bool paw3399_is_chip_lifted(void);

// Rest Mode����
void paw3399_set_rest_mode(bool enable);
bool paw3399_get_rest_mode(void);
uint16_t paw3399_get_run_downshift_time(void);
void paw3399_set_run_downshift_time(uint16_t downshift_time_ms);
uint8_t paw3399_get_rest1_period(void);
void paw3399_set_rest1_period(uint8_t period_ms);
uint32_t paw3399_get_rest1_downshift_time(void);
void paw3399_set_rest1_downshift_time(uint32_t period_ms);
uint16_t paw3399_get_rest2_period(void);
void paw3399_set_rest2_period(uint16_t period_ms);
uint32_t paw3399_get_rest2_downshift_time(void);
void paw3399_set_rest2_downshift_time(uint32_t downshift_time_ms);
uint16_t paw3399_get_rest3_period(void);
void paw3399_set_rest3_period(uint16_t period_ms);
uint16_t paw3399_get_run_downshift_mult(void);
void paw3399_set_run_downshift_mult(uint16_t new_value);
void paw3399_get_rest_downshift_mult(void);
void paw3399_set_rest_downshift_mult(uint16_t rest1_mult_value, uint16_t rest2_mult_value);

// Angle Snap����
void paw3399_set_angle_snap(bool enable);
bool paw3399_get_angle_snap(void);

// ��任����
void paw3399_set_swap_xy(bool enable);
bool paw3399_get_swap_xy(void);
void paw3399_set_inv_x(bool enable);
bool paw3399_get_inv_x(void);
void paw3399_set_inv_y(bool enable);
bool paw3399_get_inv_y(void);

// �Ʋ����ƺ���
void paw3399_set_ripple_control(bool enable);
bool paw3399_get_ripple_control(void);

// ���ź���
uint16_t paw3399_get_shutter_value(void);

// Motion ����
bool paw3399_get_mot_active_low(void);
void paw3399_set_mot_active_low(bool active_low);

// Angle Tune ����
int8_t paw3399_get_angle_tune_angle(void);
bool paw3399_set_angle_tune_angle(int8_t angle);
void paw3399_set_angle_tune(bool enable);
bool paw3399_is_angle_tune_enabled(void);

// ���Ը�������
void paw3399_print_status(void);
void paw3399_print_op_mode(uint8_t op_mode);
void paw3399_print_mode(uint8_t mode);
void paw3399_dealy_countdown(int seconds);
void paw3399_print_rawdata_map(void);
void paw3399_print_motion_burst_data(void);

// ���Ժ���
void paw3399_test(void);

#endif // PAW3399_H
