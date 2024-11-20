/***************************************
 *              库文件             		 *
 ***************************************/
#include "paw3399.h"

 /****************************************
 *             PAW3399变量               *
 ****************************************/
static uint8_t motion_burst_data[12] = {0};	// 存储Motion Burst数据
static uint8_t raw_data[1295] = {0};	// 存储Raw Data数据
static uint8_t var_a = 0;	// 抬升切断校准变量A
static uint8_t var_b = 0;	// 抬升切断校准变量B
static int16_t delta_x = 0;  // 用于存储读取到的Delta_X值
static int16_t delta_y = 0;  // 用于存储读取到的Delta_Y值
static uint16_t x_resolution = 5000;  // 默认5000 CPI
static uint16_t y_resolution = 5000;  // 默认5000 CPI
static uint16_t run_downshift_mult = 256;  // 默认值为256，对应寄存器值0x07
static uint16_t rest1_downshift_mult = 64;	// 默认值为64，对应寄存器值0x05
static uint16_t rest2_downshift_mult = 64;	// 默认值为64，对应寄存器值0x05

/****************************************
 *             SPIM变量                  *
 ****************************************/
static const nrfx_spim_t spim = NRFX_SPIM_INSTANCE(SPIM_INSTANCE);  /**< 定义 SPIM 实例 */
static volatile bool spim_xfer_done;  /**< 标志位，用于指示 SPIM 传输是否完成 */
static uint8_t m_tx_buf[2];  /**< 发送缓冲区，存储1字节地址+1字节数据 */
static uint8_t m_rx_buf[13];  /**< 接收缓冲区，存储接收到的12字节数据 */

/***************************************
 *         传感器初始化函数             *
 ***************************************/
/**
 * @brief 初始化传感器
 * 
 */
void paw3399_init(void)
{
    paw3399_spim_init();  // 调用SPI初始化函数
		paw3399_gpio_init();  // 调用GPIO初始化函数
		paw3399_print_pin_info(); // 打印引脚信息
		paw3399_power_up(); // 初始化PAW3399-T4QU
		paw3399_high_performance_mode();	// 设置为Gaming Mode
		paw3399_set_res_mod_different_src(false); // 设置XY分辨率同源
		paw3399_set_xy_resolution(5000, 5000); // 设置横纵分辨率
		
		// paw3399_test();			// 测试3399功能
}

/****************************************
 *            变量获取函数               *
 ****************************************/
/**
 * @brief  获取存储在 motion_burst_data 中的 Motion Burst 数据。
 *
 * 该函数返回指向静态存储的 motion_burst_data 数组的指针。
 * motion_burst_data 是存储从 PAW3399 传感器获取的 Motion Burst 数据的缓冲区。
 *
 * @return uint8_t* 返回一个指向 motion_burst_data 数组的指针。
 *                   该数组包含 12 字节的 Motion Burst 数据。
 *
 * @note   调用者可以直接通过返回的指针访问 motion_burst_data 的内容，
 *         但需要注意的是，修改此数据将直接影响内部的存储。
 *         如果需要保持数据一致性，建议在外部进行数据拷贝。
 */
uint8_t* paw3399_get_var_motion_burst_data(void)
{
    return motion_burst_data;
}

/**
 * @brief 获取存储在 raw_data 数组中的 Raw Data 数据。
 *
 * 该函数返回指向存储的 raw_data 数组的指针。
 * raw_data 是存储传感器或设备原始数据的缓冲区。
 *
 * @return uint8_t* 返回一个指向 raw_data 数组的指针。
 *                  该数组包含 1295 字节的原始数据。
 */
uint8_t* paw3399_get_var_raw_data(void)
{
    return raw_data;
}

/**
 * @brief 获取抬升切断校准变量A的值。
 *
 * 该函数返回存储在 var_a 中的抬升切断校准变量A的值。
 *
 * @return uint8_t 返回 var_a 的值。
 */
uint8_t paw3399_get_var_var_a(void)
{
    return var_a;
}

/**
 * @brief 获取抬升切断校准变量B的值。
 *
 * 该函数返回存储在 var_b 中的抬升切断校准变量B的值。
 *
 * @return uint8_t 返回 var_b 的值。
 */
uint8_t paw3399_get_var_var_b(void)
{
    return var_b;
}

/**
 * @brief 获取当前读取的 Delta_X 值。
 *
 * 该函数返回存储在 delta_x 中的 X 轴变化值。
 *
 * @return int16_t 返回 delta_x 的值。
 */
int16_t paw3399_get_var_delta_x(void)
{
    return delta_x;
}

/**
 * @brief 获取当前读取的 Delta_Y 值。
 *
 * 该函数返回存储在 delta_y 中的 Y 轴变化值。
 *
 * @return int16_t 返回 delta_y 的值。
 */
int16_t paw3399_get_var_delta_y(void)
{
    return delta_y;
}

/**
 * @brief 获取 X 轴分辨率 (CPI)。
 *
 * 该函数返回存储在 x_resolution 中的 X 轴分辨率，默认值为 5000 CPI。
 *
 * @return uint16_t 返回 x_resolution 的值。
 */
uint16_t paw3399_get_var_x_resolution(void)
{
    return x_resolution;
}

/**
 * @brief 获取 Y 轴分辨率 (CPI)。
 *
 * 该函数返回存储在 y_resolution 中的 Y 轴分辨率，默认值为 5000 CPI。
 *
 * @return uint16_t 返回 y_resolution 的值。
 */
uint16_t paw3399_get_var_y_resolution(void)
{
    return y_resolution;
}

/**
 * @brief 获取运行状态下的降档系数 (Downshift Multiplier)。
 *
 * 该函数返回存储在 run_downshift_mult 中的降档系数，默认值为 256。
 *
 * @return uint16_t 返回 run_downshift_mult 的值。
 */
uint16_t paw3399_get_var_run_downshift_mult(void)
{
    return run_downshift_mult;
}

/**
 * @brief 获取休眠 1 状态下的降档系数 (Downshift Multiplier)。
 *
 * 该函数返回存储在 rest1_downshift_mult 中的降档系数，默认值为 64。
 *
 * @return uint16_t 返回 rest1_downshift_mult 的值。
 */
uint16_t paw3399_get_var_rest1_downshift_mult(void)
{
    return rest1_downshift_mult;
}

/**
 * @brief 获取休眠 2 状态下的降档系数 (Downshift Multiplier)。
 *
 * 该函数返回存储在 rest2_downshift_mult 中的降档系数，默认值为 64。
 *
 * @return uint16_t 返回 rest2_downshift_mult 的值。
 */
uint16_t paw3399_get_var_rest2_downshift_mult(void)
{
    return rest2_downshift_mult;
}

/****************************************
 *           外设初始化函数              *
 ****************************************/
/**
 * @brief SPIM 用户事件处理函数。
 *
 * 当 SPIM 传输完成时，该函数被调用，处理接收到的数据并输出日志。
 *
 * @param[in] p_event  SPIM 事件结构指针。
 * @param[in] p_context 用户上下文（未使用）。
 */
void spim_event_handler(nrfx_spim_evt_t const * p_event, void * p_context)
{
    if (p_event->type == NRFX_SPIM_EVENT_DONE)
    {
        spim_xfer_done = true;
    }
}

/**
 * @brief 初始化GPIO引脚
 * 
 * 该函数用于初始化GPIO引脚
 */
void paw3399_gpio_init(void)
{
		// 初始化 GPIO 引脚
    nrf_gpio_cfg_output(NRESET_PIN);  // NRESET 引脚配置为输出
		nrf_gpio_cfg_input(MOTION_PIN, NRF_GPIO_PIN_PULLUP); // 配置Motion Pin
		nrf_gpio_pin_clear(NRESET_PIN);  // 默认将 NRESET 引脚拉高
}

/**
 * @brief 初始化 SPIM
 *
 * 该函数独立完成 SPIM 的初始化工作，包括引脚配置和事件处理函数绑定
 */
void paw3399_spim_init(void)
{
    nrfx_spim_config_t spim_config = NRFX_SPIM_DEFAULT_CONFIG;
    spim_config.frequency   = NRF_SPIM_FREQ_8M;    // 16 MHz 频率
    spim_config.ss_pin   = SPI_SS_PIN;    // 片选引脚
    spim_config.miso_pin = SPI_MISO_PIN;  // MISO 引脚
    spim_config.mosi_pin = SPI_MOSI_PIN;  // MOSI 引脚
    spim_config.sck_pin  = SPI_SCK_PIN;   // 时钟引脚

    APP_ERROR_CHECK(nrfx_spim_init(&spim, &spim_config, spim_event_handler, NULL));  // 初始化 SPIM
}

/****************************************
 *           传感器状态函数              *
 ****************************************/
/**
 * @brief PAW3399复位过程
 * 
 * 该函数通过控制NRESET引脚对PAW3399进行复位操作。将NRESET引脚拉低触发复位，
 * 然后拉高结束复位。整个过程会有1毫秒的延时以确保稳定性。
 */
void paw3399_reset(void)
{
    // 将NRESET引脚拉低，触发设备复位
    nrf_gpio_pin_clear(NRESET_PIN);
    nrf_delay_ms(1);  // 保持NRESET为低电平1毫秒

    // 将NRESET引脚拉高，结束复位
    nrf_gpio_pin_set(NRESET_PIN);
	
		nrf_delay_ms(50); // 等待50ms
}

/**
 * @brief PAW3399电源启动过程
 * 
 * 该函数通过SPI协议配置多个寄存器以完成PAW3399的电源启动和初始化流程。
 * 它包括对SPI总线的复位操作，并向多个寄存器写入特定的初始化值。
 * 如果在指定的尝试次数内未能成功读取到寄存器 0x6C 的期望值（0x80），
 * 则执行恢复操作来重新配置寄存器。
 */
void paw3399_power_up(void)
{
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("Start Paw3399 Power Up");
		nrf_gpio_pin_set(NRESET_PIN);

		// 等待至少50毫秒
		nrf_delay_ms(50);  // 延时50毫秒
	
		// 重置SPI端口
		nrf_gpio_pin_set(SPI_SS_PIN);   
    nrf_delay_ms(1);                // 延时1ms（防止电平变化过快）
    nrf_gpio_pin_clear(SPI_SS_PIN); 
		nrf_delay_ms(1);                // 延时1ms（防止电平变化过快）
		
		// 将0x5A写入Power_Up_Reset寄存器
		paw3399_write(0x3A, 0x5A);
		nrf_delay_ms(5);  // 延时5毫秒
	
		// 加载上电初始化寄存器设置
		
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
		
    // 使用 for 循环，最多尝试 60 次，每次间隔 1 毫秒
		uint8_t reg_value = 0;
    for (uint8_t attempt = 0; attempt < 60; attempt++)
    {
        // 读取寄存器 0x6C
        reg_value = paw3399_read(0x6C);

        // 检查是否读取到 0x80
        if (reg_value == 0x80)
        {
            NRF_LOG_INFO("Register 0x6C value 0x80 obtained after %d attempts.", attempt + 1);
            break;  // 成功读取到，退出循环
        }

        // 延时 1 毫秒，确保每次读取的间隔
        nrf_delay_ms(1);
    }

    // 如果 60 次后没有读取到 0x80，执行写操作进行恢复
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
		
		// 读取寄存器 0x02, 0x03, 0x04, 0x05, 0x06 并打印读取到的值
    uint8_t reg_value_02 = paw3399_read(0x02);
    uint8_t reg_value_03 = paw3399_read(0x03);
    uint8_t reg_value_04 = paw3399_read(0x04);
    uint8_t reg_value_05 = paw3399_read(0x05);
    uint8_t reg_value_06 = paw3399_read(0x06);

    // 打印读取到的值
    NRF_LOG_INFO("Register Motion    0x02: 0x%02x", reg_value_02);
    NRF_LOG_INFO("Register Delta_X_L 0x03: 0x%02x", reg_value_03);
    NRF_LOG_INFO("Register Delta_X_H 0x04: 0x%02x", reg_value_04);
    NRF_LOG_INFO("Register Delta_Y_L 0x05: 0x%02x", reg_value_05);
    NRF_LOG_INFO("Register Delta_Y_H 0x06: 0x%02x", reg_value_06);
		
		// 读取并打印传感器信息
    uint8_t product_id = paw3399_get_product_id();  // 读取Product ID
		uint8_t inv_product_id = paw3399_get_inv_product_id();  // 读取INV Product ID
    uint8_t revision_id = paw3399_get_revision_id(); // 读取Revision ID

    NRF_LOG_INFO("PAW3399 Product ID: 0x%02x", product_id);
		NRF_LOG_INFO("PAW3399 Inv Product ID: 0x%02x", inv_product_id);
    NRF_LOG_INFO("PAW3399 Revision ID: 0x%02x", revision_id);
		
		// 检查运行状态
		paw3399_print_status();
		
		NRF_LOG_INFO("PAW3399 Power Up Successfully.");
		NRF_LOG_INFO("/*******************************/");
}

/**
 * @brief 进入 PAW3399 关机模式。
 * 
 * 该函数将芯片置于关机模式。在关机模式下，SPI 通信被忽略，直到发送上电命令。
 * 
 * @note 唤醒时间较长，关机不应用于鼠标正常操作的电源管理。
 * 
 * @param none 无参数
 */
void paw3399_shutdown(void)
{
    // 写入关机寄存器 0x3B，值为 0xB6 以进入关机模式
    paw3399_write(0x3B, 0xB6);
		nrf_delay_ms(500); 
}

/**
 * @brief 退出 PAW3399 关机模式并重新上电。
 * 
 * 该函数将重新上电 PAW3399，解除关机模式。必须在断言 NCS 低电平后
 * 向寄存器 0x3A 写入 0x5A 才能恢复芯片的功能。
 * 
 * @note 重新上电时 NCS 必须保持低电平，MISO 必须上拉以满足低功耗要求。
 * 
 * @param none 无参数
 */
void paw3399_power_up_from_shutdown(void)
{
    // NCS 拉低
    nrf_gpio_pin_clear(SPI_SS_PIN);
		nrf_delay_ms(1);

    // 写入上电命令到寄存器 0x3A，值为 0x5A
    paw3399_write(0x3A, 0x5A);
    
    // 等待一段时间确保重新上电
    nrf_delay_ms(50); // 等待50ms以确保电源上电稳定
    
    // NCS 拉高
    nrf_gpio_pin_set(SPI_SS_PIN);
	
		// 检查运行状态
		paw3399_print_status();
}

/**
 * @brief 检查CHIP_OBSERVATION寄存器值是否正常
 *
 * 该函数通过写入0x00清除CHIP_OBSERVATION寄存器，等待适当的延迟后读取寄存器的值，
 * 并检查其是否为0xB7或0xBF。
 * 
 * @return bool 返回true表示寄存器值正常，否则返回false。
 */
bool paw3399_chip_observation(void)
{
    // 清除寄存器，写入0x00
    paw3399_write(0x15, 0x00);
	
		uint16_t t_dly_obs = paw3399_get_rest3_period() + 50;

    // 等待T_dly_obs，表示最长帧周期 + 10% 的变化，最长帧周期发生在芯片处于 Rest3 模式时，需要考虑时钟频率的容差值
    nrf_delay_ms(t_dly_obs);  // 根据描述，假设最大延迟为504ms + 50ms = 554ms（默认）

    // 读取寄存器值
    uint8_t observation_value = paw3399_read(0x15);

    // 检查值是否为0xB7或0xBF
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
 * @brief 获取芯片当前运行模式
 * 
 * 检查MOTION寄存器中的OP_Mode1:0位，返回当前运行模式。
 * 
 * @return uint8_t 返回值表示运行模式：
 *         00 - Run Mode
 *         01 - Rest 1
 *         10 - Rest 2
 *         11 - Rest 3
 */
uint8_t paw3399_get_op_mode(void)
{
    uint8_t motion = paw3399_read(0x02);  // 读取MOTION寄存器

    // 返回OP_Mode1:0 (bit 1:0)的值
    uint8_t op_mode = motion & 0x03;

    return op_mode;
}

/****************************************
 *           传感器模式函数              *
 ****************************************/
/**
 * @brief 切换到高性能模式 (High Performance Mode)。
 *
 * 该函数配置 PAW3399 进入高性能模式。高性能模式在默认启动时即为激活状态，
 * 适合需要较高响应和性能的使用场景。
 */
void paw3399_high_performance_mode(void)
{
		// 写寄存器											
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
    paw3399_write_with_mask(0x40, 0x03, 0x00); // 修改bit[1:0]
}

/**
 * @brief 切换到低功耗模式 (Low Power Mode)。
 *
 * 该函数配置 PAW3399 进入低功耗模式。在低功耗模式下，芯片的能耗降低，
 * 适合不需要高性能的场景，如节能和待机模式。
 *
 */
void paw3399_low_power_mode(void)
{	
		// 写寄存器
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
    paw3399_write_with_mask(0x40, 0x03, 0x01); // 修改bit[1:0]
}

/**
 * @brief 切换到办公模式 (Office Mode)。
 *
 * 该函数配置 PAW3399 进入办公模式。办公模式下，芯片会进行平衡的功耗和性能控制，
 * 适合普通使用场景，如文档处理或轻度操作。
 *
 */
void paw3399_office_mode(void)
{
		// 写寄存器
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
    paw3399_write_with_mask(0x40, 0x03, 0x02); // 修改bit[1:0]
}

/**
 * @brief 切换到有线游戏模式 (Corded Gaming Mode)。
 *
 * 该函数配置 PAW3399 进入有线游戏模式。此模式适用于要求快速响应和高性能的游戏环境，
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
 * @brief 获取并返回PAW3399的当前工作模式。
 *
 * 该函数通过读取寄存器0x40的bit[1:0]来判断PAW3399的当前工作模式。
 * 
 * @return uint8_t 返回当前工作模式的寄存器值。
 */
uint8_t paw3399_get_mode(void)
{
    return paw3399_read(0x40) & 0x03;
}

/****************************************
 *           传感器操作函数              *
 ****************************************/
/**
 * @brief 执行一次 SPIM 读操作
 *
 * 该函数接受7位寄存器地址，从该地址读取8位数据。
 *
 * @param[in] address  7位寄存器地址
 * @return uint8_t     从寄存器地址读取到的8位数据
 */
uint8_t paw3399_read(uint8_t address)
{
    uint8_t read_address = address & 0x7F;  // 设为读操作
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
 * @brief 执行一次 SPIM 写操作
 *
 * 该函数接受7位寄存器地址和8位数据，将数据写入到指定地址。
 *
 * @param[in] address  7位寄存器地址
 * @param[in] value    8位数据
 */
void paw3399_write(uint8_t address, uint8_t value)
{
    uint8_t write_address = address | 0x80;  // 设为写操作
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
 * @brief 执行一次 Motion Burst 读取操作。
 *
 * 该函数向 PAW3399 传感器发送 Motion Burst 命令地址 (0x16)，并从传感器中读取12字节的运动数据。
 * 传输完成后，读取的数据被存储在接收缓冲区并打印出来。
 *
 * @param[out] buffer 用于存储读取的 12 字节数据的缓冲区指针。
 *
 * @details
 * 读取的数据字段包括：
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
 * @note 此函数假设 SPI 接口已经初始化，并且传感器已处于工作状态。
 *
 * @return void
 */
void paw3399_motion_burst(void)
{
		// 发送Motion_Burst地址(0x16)
    m_tx_buf[0] = 0x16;
    spim_xfer_done = false;
    nrfx_spim_xfer_desc_t xfer_desc_tx = NRFX_SPIM_XFER_TRX(m_tx_buf, 1, m_rx_buf, 13);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spim, &xfer_desc_tx, 0));
	
		// 等待传输完成
    while (!spim_xfer_done)
    {
        __WFE();
    }
		
		for (int i = 0; i < 12; i++) 
		{
				motion_burst_data[i] = m_rx_buf[i+1];
		}
		
		// 等待 tBEXIT 时间
    nrf_delay_us(1);

}

/**
 * @brief 读取寄存器并写入修改后的值
 *
 * 该函数先读取指定寄存器的当前值，根据给定的位掩码修改特定的位，
 * 并将修改后的值写回寄存器。
 *
 * @param[in] reg_address 寄存器地址
 * @param[in] mask 位掩码，用于指定要修改的位
 * @param[in] value 新值，将与掩码结合用于修改特定位
 */
void paw3399_write_with_mask(uint8_t address, uint8_t mask, uint8_t value)
{
    // 读取寄存器的当前值
    uint8_t current_value = paw3399_read(address);

    // 使用掩码清除需要修改的位，保留其他位不变
    current_value &= ~mask;

    // 设置需要修改的位为新的值
    current_value |= (value & mask);

    // 将修改后的值写回寄存器
    paw3399_write(address, current_value);
}

/****************************************
 *           传感器信息函数              *
 ****************************************/
/**
 * @brief 打印引脚号信息
 * 
 * 该函数打印MOTION_PIN、NRESET_PIN以及SPI相关引脚（MISO、MOSI、SCK、SS）的引脚号信息。
 */
void paw3399_print_pin_info(void)
{
    // 打印MOTION_PIN的信息
		NRF_LOG_INFO("/*******************************/");
    NRF_LOG_INFO("MOTION_PIN: %d", MOTION_PIN);

    // 打印SPI引脚的信息
    NRF_LOG_INFO("SPI_SS_PIN: %d", SPI_SS_PIN);
    NRF_LOG_INFO("SPI_MISO_PIN: %d", SPI_MISO_PIN);
    NRF_LOG_INFO("SPI_MOSI_PIN: %d", SPI_MOSI_PIN);
    NRF_LOG_INFO("SPI_SCK_PIN: %d", SPI_SCK_PIN);
	
		// 打印NRESET_PIN的信息
		NRF_LOG_INFO("NRESET_PIN: %d", NRESET_PIN);
		NRF_LOG_INFO("/*******************************/");

    NRF_LOG_FLUSH();  // 刷新日志输出
}

/**
 * @brief 获取PAW3399的产品ID（Product ID）。
 *
 * 该函数通过读取寄存器0x00来获取PAW3399的产品ID，返回值为该ID的PID7-0。
 *
 * @return uint8_t 返回产品ID的值。
 */
uint8_t paw3399_get_product_id(void)
{
    return paw3399_read(0x00); // 返回产品ID
}

 /* @brief 获取PAW3399的INV产品 ID（Product ID）。
 *
 * 该函数通过读取寄存器0x5F来获取PAW3399的产品ID，返回值为该ID的PID0-7。
 *
 * @return uint8_t 返回产品ID的值。
 */
uint8_t paw3399_get_inv_product_id(void)
{
    return paw3399_read(0x5F); // 返回产品INV ID
}

/**
 * @brief 获取PAW3399的修订版本ID（Revision ID）。
 *
 * 该函数通过读取寄存器0x01来获取PAW3399的修订版本ID，返回值为该ID的PID7-0。
 *
 * @return uint8_t 返回修订版本ID的值。
 */
uint8_t paw3399_get_revision_id(void)
{
    return paw3399_read(0x01); // 返回修订版本ID
}

/****************************************
 *           表面信息函数                *
 ****************************************/
/**
 * @brief 基于PAW3399操作指南执行RawData输出序列。
 *
 * 此函数通过向特定寄存器写入值以启动RawData抓取过程，并从传感器中读取原始数据。
 * 该函数遵循数据手册中描述的步骤，以确保正确获取数据，同时持续检查寄存器值以确保读取到的有效数据。
 *
 * @note 在RawData输出过程中，必须将鼠标置于静止位置，此函数假设 SPI 接口已经初始化，并且传感器已处于工作状态。
 *
 * @return void
 */
void paw3399_get_rawdata(void)
{
		// 写寄存器
    paw3399_write(0x7F, 0x00);
    paw3399_write(0x55, 0x04);
    paw3399_write(0x50, 0x01);
    paw3399_write(0x40, 0x80);

    // 连续读取寄存器 0x02 (Motion) 直到 OP_Mode1 和 OP_Mode0 都为 0
    uint8_t motion;
    do {
        motion = paw3399_read(0x02);

    } while ((motion & 0x03) != 0);
		
		
		// 写寄存器
    paw3399_write(0x58, 0xFF);

    // 连续读取寄存器 0x59 直到 PG_FIRST 和 PG_VALID 都为 1
    uint8_t status;
    do {
        status = paw3399_read(0x59);
    } while ((status & 0xC0) != 0xC0);

    // 读取寄存器 0x58 的第一个 rawdata
    uint8_t rawdata = paw3399_read(0x58);
    raw_data[0] = rawdata;

    // 连续读取寄存器 0x59 直到 PG_VALID 为 1，读取 1295 次，形成完整的图片元素阵列
		for (int i = 1; i < 1296; i++) {
				// 连续读取寄存器 0x59，直到 PG_VALID 为 1
				do {
						status = paw3399_read(0x59);
				} while ((status & 0x80) == 0);  // PG_VALID 位为 1 表示有效数据
		
				// 读取实际数据
				rawdata = paw3399_read(0x58);
				raw_data[i] = rawdata;
		}

    // 写寄存器
    paw3399_write(0x40, 0x00);
    paw3399_write(0x50, 0x00);
    paw3399_write(0x55, 0x00);
}

/**
 * @brief 读取RAW数据求和寄存器的值
 * 
 * 该函数读取RAW数据求和寄存器（0x08），计算芯片的平均像素值。读取该寄存器前，需要确保芯片处于运行模式，休眠模式应禁用。
 *
 * @return uint16_t 返回平均像素值。 
 */
uint16_t paw3399_get_rawdata_sum(void)
{
    return ((paw3399_read(0x08) * 1024) / 1296);
}

/**
 * @brief 读取当前帧的最大RawData值
 * 
 * 该函数读取MAXIMUM_RAWDATA寄存器，获取当前帧的最大原始数据值。
 * 
 * @return uint8_t 返回最大RawData值（范围为0-127）。
 */
uint8_t paw3399_get_max_rawdata(void)
{
    return paw3399_read(0x09);  // 读取MAXIMUM_RAWDATA寄存器
}

/**
 * @brief 读取当前帧的最小RawData值
 * 
 * 该函数读取MINIMUM_RAWDATA寄存器，获取当前帧的最小原始数据值。
 * 
 * @return uint8_t 返回最小RawData值（范围为0-127）。
 */
uint8_t paw3399_get_min_rawdata(void)
{
    return paw3399_read(0x0A);  // 读取MINIMUM_RAWDATA寄存器
}

/**
 * @brief 读取并计算当前的 SQUAL（表面质量）。
 *
 * 该函数读取 SQUAL 寄存器的值，并根据寄存器值计算出有效的表面特征数量。
 * 公式为：有效特征数量 = SQUAL 寄存器值 * 4。
 *
 * @return uint16_t 返回表面特征数量。
 */
uint16_t paw3399_get_squal(void)
{
    // 读取 SQUAL 寄存器的值
    uint8_t squal_value = paw3399_read(0x07);  // SQUAL 寄存器地址为 0x07

    // 计算有效特征数量
    uint16_t feature_count = squal_value * 4;

    return feature_count;
}

/****************************************
 *           XY偏移函数                 *
 ****************************************/
/**
 * @brief 读取并存储Delta_X和Delta_Y的值
 * 
 * 如果MOT位为高，则读取Delta_X和Delta_Y值并存储到全局变量。
 * 如果MOT位为低，则将全局变量delta_x和delta_y设置为0。
 */
void paw3399_get_delta_xy(void)
{
    uint8_t motion = paw3399_read(0x02);  // 读取MOTION寄存器

    if (motion & 0x80)  // 检查MOT位是否为高
    {
        // 读取Delta_X和Delta_Y寄存器
        uint8_t delta_x_l = paw3399_read(0x03);
        uint8_t delta_x_h = paw3399_read(0x04);
        uint8_t delta_y_l = paw3399_read(0x05);
        uint8_t delta_y_h = paw3399_read(0x06);
        
        // 合并高低字节，并存储到全局变量
        delta_x = (int16_t)((delta_x_h << 8) | delta_x_l);
        delta_y = (int16_t)((delta_y_h << 8) | delta_y_l);

        // 判断delta_x是否异常
        if (delta_x > 200)
        {
            delta_x = 0;
        }
        else if (delta_x < -200)
        {
            delta_x = 0;
        }

        // 判断delta_y是否异常
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
        // 如果MOT位为低，则将Delta_X和Delta_Y设置为0
        delta_x = 0;
        delta_y = 0;
    }
}


/****************************************
 *           CPI/DPI函数                *
 ****************************************/
/**
 * @brief 获取XY轴的CPI分辨率
 * 
 * 读取RESOLUTION_X和RESOLUTION_Y寄存器，返回X和Y轴的CPI分辨率。
 * 
 * 
 * @return void
 */
void paw3399_get_xy_resolution(void)
{
    // 读取X轴分辨率
    uint8_t res_x_low = paw3399_read(0x48);
    uint8_t res_x_high = paw3399_read(0x49);
    x_resolution = ((res_x_high << 8) | res_x_low) * 50;

    // 读取Y轴分辨率
    uint8_t res_y_low = paw3399_read(0x4A);
    uint8_t res_y_high = paw3399_read(0x4B);
    y_resolution = ((res_y_high << 8) | res_y_low) * 50;
}

/**
 * @brief 设置XY轴的CPI分辨率
 * 
 * 设置X和Y轴的CPI分辨率。检查范围是否有效，是否是50的倍数，若设置超过9000 CPI，则启用Ripple Control。
 * 
 * @param[in] new_x_resolution 新的X轴分辨率，单位为CPI。
 * @param[in] new_y_resolution 新的Y轴分辨率，单位为CPI。
 * 
 * @return 设置成功返回true，否则false
 */
bool paw3399_set_xy_resolution(uint16_t new_x_resolution, uint16_t new_y_resolution)
{
    // 检查输入是否为50的倍数
    if (new_x_resolution % 50 != 0 || new_y_resolution % 50 != 0)
    {
        return false;
    }

    // 检查范围是否在50到20000 CPI之间
    if (new_x_resolution < 50 || new_x_resolution > 20000 || new_y_resolution < 50 || new_y_resolution > 20000)
    {
        return false;
    }
		
		// 获取RES_Mod设置，判断XY是否同源
    bool different_src = paw3399_get_res_mod_different_src();
		
		// 如果XY同源（RES_Mod = 0），则强制将X分辨率赋值给Y分辨率
    if (!different_src)
    {
        new_y_resolution = new_x_resolution;
    }

    // 计算寄存器值
    uint8_t res_x_low = (new_x_resolution / 50) & 0xFF;
    uint8_t res_x_high = (new_x_resolution / 50) >> 8;
    uint8_t res_y_low = (new_y_resolution / 50) & 0xFF;
    uint8_t res_y_high = (new_y_resolution / 50) >> 8;

    // 写入X轴和Y轴分辨率
    paw3399_write(0x48, res_x_low);  // 写入X低位
    paw3399_write(0x49, res_x_high); // 写入X高位
    paw3399_write(0x4A, res_y_low);  // 写入Y低位
    paw3399_write(0x4B, res_y_high); // 写入Y高位

    // 启用新分辨率设置
    paw3399_write(0x47, 0x01);  // 写入SET_RES寄存器

    // 更新全局变量，通过引用参数修改
    x_resolution = new_x_resolution;
    y_resolution = new_y_resolution;

    // 启用Ripple Control如果分辨率大于9000 CPI
    if (new_x_resolution >= 9000 || new_y_resolution >= 9000)
    {
        paw3399_set_ripple_control(true);  // 启用Ripple Control
    }
    else
    {
        paw3399_set_ripple_control(false);   // 禁用Ripple Control
    }

    return true;
}

/**
 * @brief 获取 RES_Mod 的状态
 * 
 * 该函数读取 MOTION_CTRL 寄存器并返回 RES_Mod 的当前设置。
 * 
 * @return bool 返回 true 表示 X 由 X 和 Y 由 Y 定义，false 表示 X 和 Y 都由 X 定义。
 */
bool paw3399_get_res_mod_different_src(void)
{
    uint8_t motion_ctrl = paw3399_read(0x5C);  // 读取 MOTION_CTRL 寄存器
    return (motion_ctrl & (1 << 1)) ? true : false;  // 判断 RES_Mod 位是否为 1
}

/**
 * @brief 设置 RES_Mod 的状态
 * 
 * 该函数设置 MOTION_CTRL 寄存器的 RES_Mod 位。
 * 
 * @param[in] res_mod 布尔值，true 表示 X 由 X 和 Y 由 Y 定义，false 表示 X 和 Y 都由 X 定义。
 */
void paw3399_set_res_mod_different_src(bool different_src)
{
    uint8_t motion_ctrl = paw3399_read(0x5C);  // 读取当前寄存器值

    if (different_src)
    {
        motion_ctrl |= (1 << 1);  // 设置 RES_Mod 为 1
    }
    else
    {
        motion_ctrl &= ~(1 << 1);  // 清除 RES_Mod 为 0
    }

    paw3399_write(0x5C, motion_ctrl);  // 写回寄存器
}


/****************************************
 *           抬升切断函数               *
 ****************************************/
/**
 * @brief 执行 PAW3399 的手动 Lift Off 校准。
 *
 * 该函数会引导用户在鼠标上进行手动 Lift Off 校准，校准过程中要求用户将鼠标
 * 放在表面上并保持不抬起，之后移动鼠标一段距离完成校准。校准成功后会写入
 * 相关寄存器值，校准失败则恢复默认值。
 *
 *
 * @note 在校准过程中，用户需要移动鼠标至少 20 英寸来覆盖大部分垫子区域。
 */
void paw3399_manual_lift_cut_off_calibration(void)
{
    NRF_LOG_INFO("/*******************************/");
    NRF_LOG_INFO("Manual Lift Off Calibration is about to begin...");
    NRF_LOG_INFO("Please place the mouse on the surface and do not lift it.");
    NRF_LOG_INFO("Calibration will start in 5 seconds...");
    NRF_LOG_FLUSH();

    // 倒计时5秒
    for (int i = 5; i > 0; i--)
    {
        NRF_LOG_INFO("%d...", i);
        NRF_LOG_FLUSH();
        nrf_delay_ms(1000);  // 延时1秒
    }

    NRF_LOG_INFO("Starting calibration process...");
    NRF_LOG_FLUSH();

    // 执行校准过程
    uint8_t var_mode = paw3399_read(0x40);  // 读取 0x40
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

    // 提示用户移动鼠标进行校准，打印提示信息，提示用户移动鼠标超过20英寸
    NRF_LOG_INFO("Please move the mouse over a distance of more than 20 inches to cover most of the surface area of the mat.");
    NRF_LOG_INFO("You have 10 seconds to complete this movement.");
    NRF_LOG_FLUSH();

    // 倒计时10秒
    for (int i = 10; i > 0; i--)
    {
        NRF_LOG_INFO("Time remaining: %d seconds", i);
        NRF_LOG_FLUSH();
        nrf_delay_ms(1000);  // 延迟1秒
    }

    // 停止校准
    paw3399_write(0x40, 0x40);

    // 检查校准状态
    uint8_t status = paw3399_read(0x4C);
    if ((status & 0x0F) == 0x05) {
        NRF_LOG_INFO("Calibration succeeded!");
				NRF_LOG_INFO("/*******************************/");
        // 根据校准结果写入 VarA 和 VarB
        var_a = paw3399_read(0x4D);
        paw3399_write(0x7F, 0x0C);

        // 判断 VarA 是否大于等于 0x32
        if (var_a >= 0x32) {
            var_b = 0x05;
        } else {
            var_b = 0x03;
        }

        paw3399_write(0x4E, 0x08);
        paw3399_write(0x7F, 0x05);
        paw3399_write(0x43, 0xE4);
        paw3399_write(0x7F, 0x00);
        paw3399_write(0x40, var_mode);  // 写入 Var_Mode 的值
    } else {
        NRF_LOG_INFO("Calibration failed, resetting to default 1mm.");
				NRF_LOG_INFO("/*******************************/");
        // 恢复寄存器
        paw3399_write(0x4E, 0x08);
        paw3399_write(0x7F, 0x05);
        paw3399_write(0x43, 0xE4);
        paw3399_write(0x7F, 0x00);
        paw3399_write(0x40, var_mode);
    }
}


/**
 * @brief 启用 PAW3399 的 Lift Off 校准设置。
 *
 * 根据给定的 VarA 和 VarB 值，将相应的寄存器配置为启用 Lift Off 校准设置。
 *
 * @param[in] var_a 校准过程中的 VarA 值。
 * @param[in] var_b 校准过程中的 VarB 值。
 */
void paw3399_enable_lift_cut_off_calibration_setting(void)
{
    // 执行指定的寄存器写入顺序
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
 * @brief 禁用 PAW3399 的 Lift Off 校准设置，并恢复到默认的 1mm 设置。
 *
 * 将相应的寄存器配置为禁用 Lift Off 校准设置。
 *
 */
void paw3399_disable_lift_cut_off_calibration_setting(void)
{
		// 执行指定的寄存器写入顺序
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
 * @brief 设置PAW3399的Lift配置
 *
 * 该函数设置PAW3399的Lift配置寄存器（0xC4E）中的LIFT位，允许选择1mm、2mm或3mm的抬升切断距离。
 *
 * @param[in] lift_setting 选择的Lift配置 (0x0: 1mm, 0x1: 2mm, 0x2: 3mm)
 */
void paw3399_set_lift_config(uint8_t lift_setting)
{
    // 确保lift_setting在合法范围内
    if (lift_setting > 0x02)
    {
        NRF_LOG_ERROR("Invalid lift setting.");
        return;
    }

    // 写入寄存器0x7F，设置高字节地址(MSB)
    paw3399_write(0x7F, 0x0C);  // 0xC4E高字节为0x0C

    // 写入寄存器0x4E，设置LIFT位
    uint8_t current_value = paw3399_read(0x4E);  // 读取寄存器当前值
    current_value &= ~0x03;  // 清除LIFT位
    current_value |= (lift_setting & 0x03);  // 设置LIFT位为新的lift_setting值
    paw3399_write(0x4E, current_value);  // 写回寄存器

    // 写入寄存器0x7F，恢复为0x00
    paw3399_write(0x7F, 0x00);
}

/**
 * @brief 读取PAW3399的Lift配置
 *
 * 该函数读取PAW3399的Lift配置寄存器（0xC4E），返回当前的抬升切断距离设置。
 *
 * @return uint8_t 返回当前的Lift设置 (0x0: 1mm, 0x1: 2mm, 0x2: 3mm)
 */
uint8_t paw3399_get_lift_config(void)
{
    // 写入寄存器0x7F，设置高字节地址(MSB)
    paw3399_write(0x7F, 0x0C);  // 0xC4E高字节为0x0C

    // 读取寄存器0x4E的LIFT位
    uint8_t current_value = paw3399_read(0x4E);
    uint8_t lift_setting = current_value & 0x03;  // 只保留LIFT位

    // 写入寄存器0x7F，恢复为0x00
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

    NRF_LOG_FLUSH();  // 刷新日志输出

    return lift_setting;
}

/**
 * @brief 检查芯片是否抬起
 * 
 * 检查MOTION寄存器中的Lift_Stat位，返回芯片是否处于抬起状态。
 * 
 * @return bool 返回true表示芯片抬起，false表示芯片在表面上。
 */
bool paw3399_is_chip_lifted(void)
{
		uint8_t motion = paw3399_read(0x02);  // 读取MOTION寄存器
	
    // 检查Lift_Stat位 (bit 3)，为1表示芯片抬起
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
 *           Rest Mode函数              *
 ****************************************/
/**
 * @brief 设置芯片的休眠模式（Rest Mode）
 * 
 * 该函数根据传递的布尔值设置芯片的休眠模式。传递 `true` 启用休眠模式，传递 `false` 禁用休眠模式。
 * 
 * @param[in] enable 传递 `true` 启用休眠模式，传递 `false` 禁用休眠模式。
 */
void paw3399_set_rest_mode(bool enable)
{
    uint8_t performance_reg = paw3399_read(0x40);  // 读取PERFORMANCE寄存器

    if (enable)
    {
        performance_reg &= ~(1 << 7);  // 将AWAKE位清零，启用休眠模式
    }
    else
    {
        performance_reg |= (1 << 7);  // 将AWAKE位置1，禁用休眠模式
    }

    paw3399_write(0x40, performance_reg);  // 写回PERFORMANCE寄存器
}


/**
 * @brief 获取当前芯片的休眠模式状态
 * 
 * 该函数读取PERFORMANCE寄存器的AWAKE位，判断芯片当前是否处于休眠模式。
 * 
 * @return bool 返回true表示休眠模式已禁用，false表示休眠模式已启用。
 */
bool paw3399_get_rest_mode(void)
{
    uint8_t performance_reg = paw3399_read(0x40);  // 读取PERFORMANCE寄存器
    bool is_rest_mode_enabled = (performance_reg & (1 << 7)) == 0;  // 检查AWAKE位是否为0，0表示启用休眠模式
    return is_rest_mode_enabled;
}

/**
 * @brief 获取当前的运行下滑时间，Downshift 是一种从高功耗模式切换到低功耗模式的过程。
 * 
 * 该函数读取RUN_DOWNSHIFT寄存器，计算并返回从运行模式切换到Rest1模式的时间（毫秒）。
 * 
 * @return uint16_t 返回下滑时间，单位为毫秒。
 */
uint16_t paw3399_get_run_downshift_time(void)
{
    uint8_t reg_value = paw3399_read(0x77);  // 读取RUN_DOWNSHIFT寄存器值
    uint16_t downshift_time_ms = reg_value * run_downshift_mult * 50 / 1000;  // 计算下滑时间 (ms)
	
    return downshift_time_ms;
}

/**
 * @brief 设置运行下滑时间。
 * 
 * 该函数根据给定的时间（毫秒）计算出相应的寄存器值并写入RUN_DOWNSHIFT寄存器。
 * 
 * @param[in] downshift_time_ms 要设置的运行下滑时间，单位为毫秒。
 */
void paw3399_set_run_downshift_time(uint16_t downshift_time_ms)
{
    // 定义最大允许下滑时间 3276ms
    const uint16_t max_downshift_time_ms = 3276;
    
    // 如果超过最大下滑时间，则将时间限制为最大值
    if (downshift_time_ms > max_downshift_time_ms)
    {
        downshift_time_ms = max_downshift_time_ms;
        NRF_LOG_WARNING("Downshift time exceeds max value. Setting to maximum: %d ms", max_downshift_time_ms);
    }

    // 计算寄存器值
    uint8_t reg_value = (downshift_time_ms * 1000) / (run_downshift_mult * 50);
    
    // 确保寄存器值不为 0
    if (reg_value == 0)
    {
        reg_value = 1;  // 确保最小值为1，避免被剪裁为0
    }

    // 写入RUN_DOWNSHIFT寄存器
    paw3399_write(0x77, reg_value);
}


/**
 * @brief 读取REST1周期时间，周期时间在进入低功耗模式（Rest Mode）后，定期唤醒自己以检查是否有运动的时间间隔
 * 
 * 读取REST1_PERIOD寄存器的值，并返回对应的时间（以毫秒为单位）。
 * 
 * @return uint8_t 返回REST1周期时间，单位为ms。
 */
uint8_t paw3399_get_rest1_period(void)
{
    uint8_t rest1_period_reg = paw3399_read(0x78);  // 读取REST1_PERIOD寄存器
    uint8_t period_ms = rest1_period_reg * 1;  // 1ms倍数
    return period_ms;
}

/**
 * @brief 设置REST1周期时间
 * 
 * 设置REST1_PERIOD寄存器的值，修改Rest1的周期时间。
 * 
 * @param[in] period_ms REST1周期时间，单位为ms。
 */
void paw3399_set_rest1_period(uint8_t period_ms)
{
    uint8_t rest1_period_reg = period_ms / 1;  // 将时间转换为寄存器值（1ms的倍数）
    if (rest1_period_reg == 0)
    {
        rest1_period_reg = 1;  // 确保最小值为1，避免被剪裁为0
    }
    paw3399_write(0x78, rest1_period_reg);  // 写入REST1_PERIOD寄存器
}

/**
 * @brief 获取Rest1降档时间
 *
 * 该函数读取Rest1_DOWNSHIFT寄存器，并计算从Rest1到Rest2的降档时间（毫秒）。
 *
 * @return uint32_t 返回降档时间，单位为毫秒。
 */
uint32_t paw3399_get_rest1_downshift_time(void)
{
    uint8_t rest1_downshift_reg = paw3399_read(0x79);  // 读取寄存器值
    uint32_t downshift_time_ms = rest1_downshift_reg * rest1_downshift_mult * paw3399_get_rest1_period(); 
    return downshift_time_ms;
}

/**
 * @brief 设置Rest1降档时间
 *
 * 该函数设置Rest1到Rest2的降档时间，时间参数会转换为寄存器值写入Rest1_DOWNSHIFT寄存器。
 *
 * @param[in] period_ms 降档时间，单位为毫秒。
 */
void paw3399_set_rest1_downshift_time(uint32_t period_ms)
{
    uint8_t rest1_downshift_reg = period_ms / (rest1_downshift_mult * paw3399_get_rest1_period());  // 将时间转换为寄存器值
    if (rest1_downshift_reg == 0)
    {
        rest1_downshift_reg = 1;  // 确保最小值为1，避免被剪裁为0
    }
    paw3399_write(0x79, rest1_downshift_reg);  // 写入寄存器
}

/**
 * @brief 读取REST2周期时间
 * 
 * 读取REST2_PERIOD寄存器的值，并返回对应的时间（以毫秒为单位）。
 * 
 * @return uint16_t 返回REST2周期时间，单位为ms。
 */
uint16_t paw3399_get_rest2_period(void)
{
    uint8_t rest2_period_reg = paw3399_read(0x7A);  // 读取REST2_PERIOD寄存器
    uint16_t period_ms = rest2_period_reg * SLOW_CLOCK * 4;  // 1ms * 4 的倍数
    return period_ms;
}

/**
 * @brief 设置REST2周期时间
 * 
 * 设置REST2_PERIOD寄存器的值，修改Rest2的周期时间。
 * 
 * @param[in] period_ms REST2周期时间，单位为ms。
 */
void paw3399_set_rest2_period(uint16_t period_ms)
{
    uint8_t rest2_period_reg = period_ms / (4 * SLOW_CLOCK);  // 将时间转换为寄存器值（4ms的倍数）
    if (rest2_period_reg == 0)
    {
        rest2_period_reg = 1;  // 确保最小值为1，避免被剪裁为0
    }
    paw3399_write(0x7A, rest2_period_reg);  // 写入REST2_PERIOD寄存器
}

/**
 * @brief 获取Rest2到Rest3的降档时间
 *
 * 该函数读取REST2_DOWNSHIFT寄存器，并计算从Rest2降档到Rest3的时间（毫秒）。
 *
 * @return uint32_t 返回Rest2到Rest3的降档时间，单位为毫秒。
 */
uint32_t paw3399_get_rest2_downshift_time(void)
{
    uint8_t downshift_reg = paw3399_read(0x7B);  // 读取寄存器值
    uint32_t downshift_time_ms = downshift_reg * rest2_downshift_mult * paw3399_get_rest2_period();  
    return downshift_time_ms;
}

/**
 * @brief 设置Rest2到Rest3的降档时间
 *
 * 该函数通过向REST2_DOWNSHIFT寄存器写入值，来设置从Rest2降档到Rest3的时间。
 *
 * @param downshift_time_ms 新的降档时间，单位为毫秒。
 */
void paw3399_set_rest2_downshift_time(uint32_t downshift_time_ms)
{
    uint8_t rest3_downshift_reg = downshift_time_ms / (rest2_downshift_mult * paw3399_get_rest2_period());  // 将时间转换为寄存器值
    if (rest3_downshift_reg == 0)
    {
        rest3_downshift_reg = 1;  // 确保最小值为1，避免被剪裁为0
    }
    paw3399_write(0x7B, rest3_downshift_reg);  // 写入寄存器值
}

/**
 * @brief 获取Rest3模式周期时间
 *
 * 该函数读取PAW3399的REST3_PERIOD寄存器的值，计算并返回Rest3模式的周期时间（单位：ms）。
 * 同时打印当前的周期时间。
 *
 * @return uint16_t 返回Rest3模式的周期时间，单位为毫秒。
 */
uint16_t paw3399_get_rest3_period(void)
{
    uint8_t rest3_period_reg = paw3399_read(0x7C);  // 读取寄存器0x7C，获取Rest3周期的寄存器值
    uint16_t rest3_period_ms = rest3_period_reg * SLOW_CLOCK * 8;  // 根据寄存器值计算周期时间
    return rest3_period_ms;
}

/**
 * @brief 设置Rest3模式周期时间
 *
 * 该函数将指定的周期时间（ms）写入REST3_PERIOD寄存器，设置新的Rest3模式周期。
 * 注意：最小有效值为0x01，最大值为0xFF。设置成功后打印新设置的周期时间。
 *
 * @param[in] period_ms Rest3模式的新周期时间，单位为毫秒。
 */
void paw3399_set_rest3_period(uint16_t period_ms)
{
    uint8_t rest3_period_reg = period_ms / (SLOW_CLOCK * 8);  // 将时间反推为寄存器值
    if (rest3_period_reg == 0)
    {
        rest3_period_reg = 1;  // 确保最小值为1，避免被剪裁为0
    }
    paw3399_write(0x7C, rest3_period_reg);  // 写入新的周期时间到寄存器0x7C
}

/**
 * @brief 读取 Run Downshift Multiplier 的实际值
 *
 * 该函数从寄存器中读取当前的 Run Downshift Multiplier 的寄存器值，并转换为实际的乘数值。
 *
 * @return uint16_t 返回当前的 Run Downshift Multiplier 的实际值。
 */
uint16_t paw3399_get_run_downshift_mult(void)
{
    // 读取寄存器0x7D的值，获取RUN_DOWN_M[3:0]的值
    uint8_t reg_value = paw3399_read(0x7D) & 0x0F;

    // 根据寄存器值计算实际乘数值
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
        default: run_downshift_mult = 256; break;  // 默认值为256
    }

    return run_downshift_mult;
}

/**
 * @brief 设置 Run Downshift Multiplier 的实际值
 *
 * 该函数根据输入的乘数值计算对应的寄存器值，并写入寄存器，同时更新全局变量。
 *
 * @param new_value 要设置的Run Downshift Multiplier的实际值。
 */
void paw3399_set_run_downshift_mult(uint16_t new_value)
{
    uint8_t reg_value;

    // 根据输入的实际乘数值计算对应的寄存器值
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

    // 将计算出的寄存器值写入寄存器0x7D
    paw3399_write(0x7D, reg_value);

    // 更新全局变量
    run_downshift_mult = new_value;
}

/**
 * @brief 读取REST1和REST2 Downshift Multiplier
 *
 * 该函数从REST_DOWNSHIFT_MULT寄存器读取REST1和REST2的下滑乘数，更新全局变量rest1_downshift_mult和rest2_downshift_mult。
 */
void paw3399_get_rest_downshift_mult(void)
{
    // 读取寄存器0x7E的值
    uint8_t reg_value = paw3399_read(0x7E);

    // 提取REST_M0到REST_M2的位，计算REST1的乘数值
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

    // 提取REST_M4到REST_M6的位，计算REST2的乘数值
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

    // 打印当前的REST1和REST2 Downshift Multiplier
    NRF_LOG_INFO("Current REST1 Downshift Multiplier: %d", rest1_downshift_mult);
    NRF_LOG_INFO("Current REST2 Downshift Multiplier: %d", rest2_downshift_mult);
    NRF_LOG_FLUSH();
}

/**
 * @brief 设置REST1和REST2 Downshift Multiplier
 *
 * 该函数将新的乘数写入REST_DOWNSHIFT_MULT寄存器，并更新全局变量rest1_downshift_mult和rest2_downshift_mult。
 *
 * @param[in] rest1_mult_value 要设置的REST1乘数值 (有效范围为2到256)。
 * @param[in] rest2_mult_value 要设置的REST2乘数值 (有效范围为2到256)。
 */
void paw3399_set_rest_downshift_mult(uint16_t rest1_mult_value, uint16_t rest2_mult_value)
{
    // 限制rest1_mult_value和rest2_mult_value在2到256之间
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

    // 根据乘数值计算寄存器值
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

    // 合并REST1和REST2的寄存器值
    uint8_t reg_value = (rest2_reg_value << 4) | rest1_reg_value;

    // 写入寄存器0x7E的值
    paw3399_write(0x7E, reg_value);

    // 打印设置的REST1和REST2 Downshift Multiplier
    NRF_LOG_INFO("Set REST1 Downshift Multiplier: %d", rest1_mult_value);
    NRF_LOG_INFO("Set REST2 Downshift Multiplier: %d", rest2_mult_value);
    NRF_LOG_FLUSH();
}

/****************************************
 *           Angle Snap函数             *
 ****************************************/
/**
 * @brief 设置角度锁定功能
 *
 * 该函数根据传入的布尔值设置角度锁定功能是否启用。
 * 
 * @param[in] enable 传入true启用角度锁定功能，false则禁用。
 */
void paw3399_set_angle_snap(bool enable)
{
    uint8_t reg_value = paw3399_read(0x56);  // 读取当前寄存器值

    if (enable)
    {
        reg_value |= 0x80;  // 设置EN位为1，启用角度锁定功能
    }
    else
    {
        reg_value &= ~0x80;  // 清除EN位，禁用角度锁定功能
    }

    paw3399_write(0x56, reg_value);  // 写入寄存器
}

/**
 * @brief 检查角度锁定功能是否启用
 *
 * 该函数读取ANGLE_SNAP寄存器，返回角度锁定功能是否被启用。
 * 
 * @return bool 返回true表示角度锁定已启用，false表示未启用。
 */
bool paw3399_get_angle_snap(void)
{
    uint8_t reg_value = paw3399_read(0x56);  // 读取ANGLE_SNAP寄存器值
    bool enabled = reg_value & 0x80;  // 检查EN位
    return enabled;
}

/****************************************
 *           轴变换函数                 *
 ****************************************/
/**
 * @brief 设置 XY 轴交换功能
 * 
 * 该函数根据传入的布尔值启用或禁用 XY 轴交换功能。
 * 
 * @param[in] enable 传入 true 启用 XY 轴交换功能，false 禁用 XY 轴交换功能。
 */
void paw3399_set_swap_xy(bool enable)
{
    uint8_t reg_value = paw3399_read(0x5B);  // 读取 AXIS_CONTROL 寄存器

    if (enable)
    {
        reg_value |= 0x80;  // 设置 Swap_XY 位 (bit 7)
    }
    else
    {
        reg_value &= ~0x80;  // 清除 Swap_XY 位 (bit 7)
    }

    paw3399_write(0x5B, reg_value);  // 写回 AXIS_CONTROL 寄存器
}

/**
 * @brief 获取当前是否启用了 XY 轴交换功能
 * 
 * 该函数读取 AXIS_CONTROL 寄存器，返回是否启用了 XY 轴交换功能。
 * 
 * @return bool 返回 true 表示启用了 XY 轴交换功能，false 表示未启用。
 */
bool paw3399_get_swap_xy(void)
{
    uint8_t reg_value = paw3399_read(0x5B);  // 读取 AXIS_CONTROL 寄存器
    bool enabled = reg_value & 0x80;  // 检查 Swap_XY 位 (bit 7)
    return enabled;
}


/**
 * @brief 设置 X 轴反转功能
 * 
 * 该函数根据传入的布尔值启用或禁用 X 轴反转功能。
 * 
 * @param[in] enable 传入 true 启用 X 轴反转功能，false 禁用 X 轴反转功能。
 */
void paw3399_set_inv_x(bool enable)
{
    uint8_t reg_value = paw3399_read(0x5B);  // 读取 AXIS_CONTROL 寄存器

    if (enable)
    {
        reg_value |= 0x20;  // 设置 INV_X 位 (bit 5)
    }
    else
    {
        reg_value &= ~0x20;  // 清除 INV_X 位 (bit 5)
    }

    paw3399_write(0x5B, reg_value);  // 写回 AXIS_CONTROL 寄存器
}

/**
 * @brief 获取当前是否启用了 X 轴反转功能
 * 
 * 该函数读取 AXIS_CONTROL 寄存器，返回是否启用了 X 轴反转功能。
 * 
 * @return bool 返回 true 表示启用了 X 轴反转功能，false 表示未启用。
 */
bool paw3399_get_inv_x(void)
{
    uint8_t reg_value = paw3399_read(0x5B);  // 读取 AXIS_CONTROL 寄存器
    bool enabled = reg_value & 0x20;  // 检查 INV_X 位 (bit 5)
    return enabled;
}

/**
 * @brief 设置 Y 轴反转功能
 * 
 * 该函数根据传入的布尔值启用或禁用 Y 轴反转功能。
 * 
 * @param[in] enable 传入 true 启用 Y 轴反转功能，false 禁用 Y 轴反转功能。
 */
void paw3399_set_inv_y(bool enable)
{
    uint8_t reg_value = paw3399_read(0x5B);  // 读取 AXIS_CONTROL 寄存器

    if (enable)
    {
        reg_value |= 0x40;  // 设置 INV_Y 位 (bit 6)
    }
    else
    {
        reg_value &= ~0x40;  // 清除 INV_Y 位 (bit 6)
    }

    paw3399_write(0x5B, reg_value);  // 写回 AXIS_CONTROL 寄存器
}

/**
 * @brief 获取当前是否启用了 Y 轴反转功能
 * 
 * 该函数读取 AXIS_CONTROL 寄存器，返回是否启用了 Y 轴反转功能。
 * 
 * @return bool 返回 true 表示启用了 Y 轴反转功能，false 表示未启用。
 */
bool paw3399_get_inv_y(void)
{
    uint8_t reg_value = paw3399_read(0x5B);  // 读取 AXIS_CONTROL 寄存器
    bool enabled = reg_value & 0x40;  // 检查 INV_Y 位 (bit 6)
    return enabled;
}

/****************************************
 *           纹波控制函数               *
 ****************************************/
/**
 * @brief 设置Ripple Control功能。
 * 
 * 该函数根据传入的布尔值启用或禁用Ripple Control功能。
 * 
 * @param[in] enable 传入true启用Ripple Control，false禁用Ripple Control。
 */
void paw3399_set_ripple_control(bool enable)
{
    uint8_t reg_value = paw3399_read(0x5A);  // 读取寄存器0x5A

    if (enable)
    {
        reg_value |= 0x80;  // 设置CTRL位为1，启用Ripple Control
    }
    else
    {
        reg_value &= ~0x80;  // 清除CTRL位为0，禁用Ripple Control
    }

    paw3399_write(0x5A, reg_value);  // 写回寄存器0x5A
}

/**
 * @brief 检查Ripple Control是否已启用。
 * 
 * 该函数检查寄存器0x5A的CTRL位，判断Ripple Control是否已启用。
 * 
 * @return true 如果Ripple Control已启用，返回true。否则返回false。
 */
bool paw3399_get_ripple_control(void)
{
    uint8_t reg_value = paw3399_read(0x5A);  // 读取寄存器0x5A
    bool enabled = reg_value & 0x80;
		return enabled;    
}

/****************************************
 *           快门函数                   *
 ****************************************/
/**
 * @brief 获取当前快门值
 * 
 * 该函数读取12位的快门寄存器，先读取SHUTTER_UPPER，再读取SHUTTER_LOWER，组合为完整的12位快门值。
 * 
 * @return uint16_t 返回当前的快门值（12位）。
 */
uint16_t paw3399_get_shutter_value(void)
{
    // 读取快门的高4位
    uint8_t shutter_upper = paw3399_read(0x0C) & 0x0F;

    // 读取快门的低8位
    uint8_t shutter_lower = paw3399_read(0x0B);

    // 组合12位快门值
    uint16_t shutter_value = (shutter_upper << 8) | shutter_lower;

	return shutter_value;
}

/**
 * @brief 获取 MOT_Set 是否为低电平有效（active low）
 * 
 * 该函数读取 MOTION_CTRL 寄存器并判断 MOT_Set 是否为低电平有效（0）。
 * 
 * @return bool 返回 true 表示 MOT_Set 为低电平有效（active low），false 表示高电平有效（active high）。
 */
bool paw3399_get_mot_active_low(void)
{
    uint8_t motion_ctrl = paw3399_read(0x5C);  // 读取 MOTION_CTRL 寄存器
    return (motion_ctrl & (1 << 7)) ? false : true;  // 如果 MOT_Set 为 0，返回 true 表示 active low
}

/**
 * @brief 设置 MOT_Set 的状态
 * 
 * 该函数设置 MOTION_CTRL 寄存器的 MOT_Set 位。
 * 
 * @param[in] mot_set 布尔值，true 表示设置为低电平有效 (motion active low)，false 表示设置为高电平有效 (motion active high)。
 */
void paw3399_set_mot_active_low(bool active_low)
{
    uint8_t motion_ctrl = paw3399_read(0x5C);  // 读取当前寄存器值

    if (active_low)
    {
				motion_ctrl &= ~(1 << 7);  // 清除 MOT_Set 为 0 (低电平)
    }
    else
    {
        motion_ctrl |= (1 << 7);  // 设置 MOT_Set 为 1 (高电平)
    }

    paw3399_write(0x5C, motion_ctrl);  // 写回寄存器
}

/***************************************
 *          Angle Tune函数             *
 ***************************************/
/**
 * @brief 读取当前角度设置
 * 
 * @return int8_t 返回角度设置值，单位为度数（-30, -10, 0, 15, 30）
 */
int8_t paw3399_get_angle_tune_angle(void)
{
    paw3399_write(0x7F, 0x05);  // 写入 MSB (0x05) 到寄存器 0x7F
    uint8_t angle_value = paw3399_read(0x77);  // 读取寄存器 0x0577 的值
    paw3399_write(0x7F, 0x00);  // 恢复寄存器 0x7F 的值

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
            return -1;  // 未知角度
    }
}

/**
 * @brief 设置角度调整值
 * 
 * @param[in] angle 要设置的角度值，合法值为 -30, -10, 0, 15, 30
 * 
 * @return bool 成功返回 true，否则返回 false
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

    paw3399_write(0x7F, 0x05);  // 写入 MSB (0x05) 到寄存器 0x7F
    paw3399_write(0x77, angle_value);  // 写入角度设置
    paw3399_write(0x7F, 0x00);  // 恢复寄存器 0x7F 的值

    return true;
}

/**
 * @brief 启用或禁用角度调整功能
 * 
 * @param[in] enable true 表示启用角度调整，false 表示禁用角度调整
 */
void paw3399_set_angle_tune(bool enable)
{
    paw3399_write(0x7F, 0x05);  // 写入 MSB (0x05) 到寄存器 0x7F
    uint8_t angle_tune2 = paw3399_read(0x78);  // 读取寄存器 0x0578 的值

    if (enable)
    {
        angle_tune2 |= (1 << 7);  // 设置 EN 位为 1
    }
    else
    {
        angle_tune2 &= ~(1 << 7);  // 清除 EN 位
    }

    paw3399_write(0x78, angle_tune2);  // 写入更新后的值
    paw3399_write(0x7F, 0x00);  // 恢复寄存器 0x7F 的值
}

/**
 * @brief 检查角度调整功能是否启用
 * 
 * @return bool 如果启用了角度调整，返回 true，否则返回 false
 */
bool paw3399_is_angle_tune_enabled(void)
{
    paw3399_write(0x7F, 0x05);  // 写入 MSB (0x05) 到寄存器 0x7F
    uint8_t angle_tune2 = paw3399_read(0x78);  // 读取寄存器 0x0578 的值
    paw3399_write(0x7F, 0x00);  // 恢复寄存器 0x7F 的值

    return (angle_tune2 & (1 << 7)) != 0;
}



/****************************************
 *           测试辅助函数                *
 ****************************************/
/**
 * @brief 打印工作模式名称。
 *
 * 该函数接收模式值（uint8_t），根据值输出相应的模式名称。
 * 
 * @param[in] mode PAW3399的工作模式，取值为0x00, 0x01, 0x02, 0x03之一。
 */
void paw3399_print_mode(uint8_t mode)
{
    // 根据模式值打印对应的模式名称
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

    NRF_LOG_FLUSH();  // 刷新日志输出
}

/**
 * @brief 打印芯片当前运行模式
 * 
 * 该函数根据传入的操作模式值，打印芯片的当前运行模式。
 * 
 * @param[in] op_mode 芯片当前的运行模式。
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

    NRF_LOG_FLUSH();  // 刷新日志输出
}
/**
 * @brief 检查PAW3399的运行状态和当前工作模式
 *
 * 该函数检查PAW3399芯片的CHIP_OBSERVATION寄存器和当前运行模式，输出相应的状态日志。
 */
void paw3399_print_status(void)
{    
    // 打印CHIP_OBSERVATION的状态
		NRF_LOG_INFO("Chip_Observation: %s", paw3399_chip_observation() ? "Normal" : "Abnormal");

    // 获取当前运行模式
    uint8_t op_mode = paw3399_get_op_mode();
		uint8_t mode = paw3399_get_mode();
    
    // 打印运行模式
    paw3399_print_op_mode(op_mode);
		paw3399_print_mode(mode);
}

/**
 * @brief 倒计时函数，以秒为单位进行倒计时，并在每秒输出日志。
 *
 * @param[in] seconds 倒计时的秒数。
 */
void paw3399_dealy_countdown(int seconds)
{
    for (int i = seconds; i > 0; i--)
    {
        NRF_LOG_INFO("%d...", i);
        NRF_LOG_FLUSH();
        nrf_delay_ms(1000);  // 延时1秒
    }
}

/**
 * @brief 打印RawData图。
 *
 * 该函数根据PAW3399的RawData输出打印完整的图，每行36个数据，行从下到上打印。
 * 同时在顶部和左侧打印行号和列号，便于查看数据结构。
 * 
 * @param[in] pFrame 存储RawData数据的缓冲区指针。
 */
void paw3399_print_rawdata_map(void)
{
    // 每行36个数据，按图中的格式打印
    NRF_LOG_INFO("/*******************************/");
    NRF_LOG_INFO("RawData Map Output Start");
    NRF_LOG_FLUSH();

    // 打印列号 (顶部)
    char line_buffer[512];
    int offset = 0;
    offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "     "); // 左上角的空格
    for (int col = 0; col < 36; col++) {
        offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "%4d", col); // 列号
    }
    NRF_LOG_INFO("%s", line_buffer);
    NRF_LOG_FLUSH();

    // 打印每行数据
    for (int row = 35; row >= 0; row--) // 行从35开始到0
    {
        offset = 0;
        // 打印行号
        offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "%4d ", row); // 行号

        // 打印对应行的所有列的数据
        for (int col = 0; col < 36; col++) {
            int index = row * 36 + col;
            if (index < 1296) {
                offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "%4d", raw_data[index]);
            } else {
                offset += snprintf(line_buffer + offset, sizeof(line_buffer) - offset, "    "); // 超过范围填充空格
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
 * @brief 打印Motion Burst数据。
 *
 * 该函数将PAW3399传感器通过Motion Burst读取到的12字节数据逐个打印。
 * 
 * @param[in] buffer 存储Motion Burst数据的缓冲区指针。
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
 * @brief 检测给定引脚的按下和释放事件
 * 
 * 该函数检测指定引脚的按下和释放。使用防抖机制避免多次触发。
 * 
 * @param[in] pin 按键对应的引脚号。
 * @param[in] default_high 如果按键的默认状态是高电平，传入true；否则传入false。
 * 
 * @return bool 返回true表示按键按下，false表示按键未按下。
 */
bool paw3399_button_is_pressed(uint32_t pin, bool default_high)
{
    static bool last_button_state = false;  // 上一次的按键状态
    bool current_button_state = nrf_gpio_pin_read(pin);  // 读取引脚电平状态

    // 根据默认状态调整电平判断条件
    if (default_high)
    {
        current_button_state = (current_button_state == 0);  // 默认高电平，按下时为低电平
    }
    else
    {
        current_button_state = (current_button_state != 0);  // 默认低电平，按下时为高电平
    }

    // 如果状态发生了变化，进行防抖延时并再次确认
    if (current_button_state != last_button_state)
    {
        nrf_delay_ms(10);  // 防抖延时
        bool rechecked_state = nrf_gpio_pin_read(pin);  // 再次读取

        if (default_high)
        {
            rechecked_state = (rechecked_state == 0);  // 重新确认电平状态
        }
        else
        {
            rechecked_state = (rechecked_state != 0);
        }

        // 如果状态确实变化，更新状态并返回
        if (rechecked_state != last_button_state)
        {
            last_button_state = rechecked_state;
            return rechecked_state;  // 返回按键状态
        }
    }

    return false;  // 没有变化时返回false
}

/**
 * @brief 等待指定按键被按下
 *
 * 该函数持续等待，直到指定的按键被按下。
 *
 * @param[in] pin 按键引脚号
 */
void paw3399_wait_for_button_press(uint32_t pin)
{
    NRF_LOG_INFO("Waiting for button press (BUTTON_PIN: %d)...", pin);
    NRF_LOG_FLUSH();

    // 循环等待直到指定引脚的按键被按下
    while (!paw3399_button_is_pressed(pin, true));  // 这里假设按钮为active low
}


/***************************************
 *             测试函数             		 *
 ***************************************/
/**
 * @brief PAW3399传感器测试函数。
 *
 * 该函数测试PAW3399传感器的各项功能，包括Motion Burst读取、Raw Data输出、
 * 模式切换、关机和重新上电等操作。所有结果均通过日志输出。
 */
void paw3399_test(void)
{
		// 测试用gpio
		nrf_gpio_cfg_input(TEST_BUTTON, NRF_GPIO_PIN_PULLUP);  // 配置引脚为输入模式，并根据参数配置上下拉电阻
		
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Test Start");
		NRF_LOG_FLUSH();
		paw3399_wait_for_button_press(TEST_BUTTON);
	
		// PAW3399 重置 测试
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Reset Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_reset();
		paw3399_dealy_countdown(5); // 延迟5s以观察功耗
		paw3399_power_up();
		NRF_LOG_INFO("PAW3399 Reset Test Complete");
		NRF_LOG_INFO("/*******************************/");
	
		// PAW3399 关机 测试
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Shut Down Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_shutdown();
		paw3399_dealy_countdown(5); // 延迟5s以观察功耗
		paw3399_power_up_from_shutdown();
		NRF_LOG_INFO("PAW3399 Shut Down Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 模式切换 测试
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
	
		// PAW3399 Motion Burst 测试
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Motion Burst Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_motion_burst();
		paw3399_print_motion_burst_data();
		NRF_LOG_INFO("PAW3399 Motion Burst Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 Raw Data 测试
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
		
		// PAW3399 XY Delta 测试
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 XY Delta Test Start");
		NRF_LOG_INFO("Please Move The Sensor");
		paw3399_wait_for_button_press(TEST_BUTTON);
		// 检查 MOTION_PIN 是否为低电平
		for (uint8_t count = 0; count < 10; count++)
		{
			while(nrf_gpio_pin_read(MOTION_PIN));  // 0 表示引脚被拉低
			paw3399_get_delta_xy();
			NRF_LOG_INFO("Delta_X: %d, Delta_Y: %d", delta_x, delta_y);
			NRF_LOG_FLUSH();
		}
		NRF_LOG_INFO("PAW3399 XY Delta Test Complete");
		NRF_LOG_INFO("/*******************************/");
		
		// PAW3399 CPI/DPI 测试
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
		
		// PAW3399 抬升切断 测试
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
		
		// PAW3399 Rest Mode 测试
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
		
		// PAW3399 Angle Snap 测试
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
		
		// PAW3399 Swap XY 测试
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
		
		// PAW3399 Inv X 测试
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
		
		// PAW3399 Inv Y 测试
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
		
		// PAW3399 Ripple Control 测试
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
		
		// PAW3399 快门 测试
		NRF_LOG_INFO("/*******************************/");
		NRF_LOG_INFO("PAW3399 Shutter Test Start");
		paw3399_wait_for_button_press(TEST_BUTTON);
		NRF_LOG_INFO("Shutter Value: %d", paw3399_get_shutter_value());
		NRF_LOG_INFO("PAW3399 Shutter Test Complete");
		NRF_LOG_INFO("/*******************************/");	
		
		// PAW3399 Motion 测试
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

		// PAW3399 Angle Tune 测试
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
		
		// PAW3399 测试 完成
		paw3399_wait_for_button_press(TEST_BUTTON);
		paw3399_print_status();
		NRF_LOG_INFO("PAW3399 Test Complete");
		NRF_LOG_INFO("/*******************************/");
		paw3399_wait_for_button_press(TEST_BUTTON);
		NRF_LOG_FLUSH();
}

