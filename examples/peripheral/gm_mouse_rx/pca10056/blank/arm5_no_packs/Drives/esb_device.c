/***************************************
 *              库文件             		 *
 ***************************************/
#include "esb_device.h"
#include "usb_device.h"

/***************************************
 *            ESB负载定义              *
 ***************************************/
/// @brief 定义一个接收（RX）负载结构体
static nrf_esb_payload_t rx_payload;
static uint8_t databuffer[6];  // 用于存储发送的鼠标数据


/***************************************
 *              ESB函数             	 *
 ***************************************/
/**
 * @brief ESB事件处理函数
 * @param p_event 指向ESB事件的指针
 * 
 * 根据事件类型处理相应的ESB事件：成功发送、发送失败、接收到数据包等。
 */
void esb_device_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            NRF_LOG_DEBUG("TX SUCCESS EVENT");  // 记录传输成功事件日志
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            NRF_LOG_DEBUG("TX FAILED EVENT");   // 记录传输失败事件日志
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            NRF_LOG_DEBUG("RX RECEIVED EVENT"); // 记录接收数据事件日志
            if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
            {
                set_mark_usbd_suspend_state_request(false);
								// 如果 USB 处于挂起状态并且启用了远程唤醒
								if (get_mark_usbd_suspended() && get_mark_usbd_remote_wakeup_enabled())
								{
										UNUSED_RETURN_VALUE(nrf_drv_usbd_wakeup_req()); // 请求 USB 唤醒
								}
								else  // 如果 USB 没有挂起
								{	
										// 复制 rx_payload.data 的前 6 个字节到 databuffer 中
										memcpy(databuffer, rx_payload.data, 6);
									
										if (get_mark_usbd_configured() && !get_mark_send_device_data()) // 如果当前不在发送鼠标数据
										{
												// 构建用于 USB 传输的数据结构
												static const nrf_drv_usbd_transfer_t transfer =
												{
														.p_data = {.tx = &databuffer}, // 设置传输数据源为数据缓冲区
														.size = sizeof(databuffer) // 传输数据大小
												};
								
												set_mark_send_device_data(true); // 设置 正在发送鼠标位置标记
								
												// 使用端点1，开始 USB 传输
												UNUSED_RETURN_VALUE(nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN1, &transfer));

										}
								}  
            }
            break;
    }
}

/**
 * @brief 初始化增强型ShockBurst（ESB）协议
 * @return 返回初始化的错误代码
 * 
 * 配置并初始化ESB通信协议，包括基地址和前缀地址设置。
 */
uint32_t esb_module_init(void)
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7}; // 基地址0，用于接收/发送地址的匹配
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2}; // 基地址1，用于接收/发送地址的匹配，一种辅助基地址，在某些特殊场景下使用，例如多设备通信
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8}; // 管道前缀地址数组，用于多通道通信
		
		// PTX：使用base_addr_0作为主要目标地址，将数据发送到接收端的一个或多个通道
		// PRX：基于base_addr_0与前缀地址生成多个接收地址，监听多个逻辑通道

    // 默认ESB配置结构体
    nrf_esb_config_t nrf_esb_config = NRF_ESB_DEFAULT_CONFIG; // 加载默认的ESB配置参数

    // 设置协议类型
    nrf_esb_config.protocol = NRF_ESB_PROTOCOL_ESB_DPL; // 使用Enhanced ShockBurst DPL协议（ESB DPL），支持动态负载长度 Dynamic Payload Length

    // 设置有效负载大小
    nrf_esb_config.payload_length           = 8;                      ///< 设置有效负载长度为8字节

    // 设置通信速率
    nrf_esb_config.bitrate = NRF_ESB_BITRATE_2MBPS; // 设置为2Mbps的数据传输速率，提供较快的通信速度

    // 设置ESB事件处理函数
    nrf_esb_config.event_handler = esb_device_event_handler; // 指定用户定义的ESB事件处理函数，用于处理发送成功、失败及接收数据事件

    // 设置工作模式
    nrf_esb_config.mode = NRF_ESB_MODE_PRX; // 设置设备为接收模式（PRX）

    // 设置自动应答
    nrf_esb_config.selective_auto_ack = false; // 禁用选择性自动应答，即所有接收方都自动返回ACK，简化通信过程

    // 初始化ESB模块
    err_code = nrf_esb_init(&nrf_esb_config); // 使用配置结构体进行初始化
    VERIFY_SUCCESS(err_code); // 验证初始化成功

    // 设置基地址0
    err_code = nrf_esb_set_base_address_0(base_addr_0); // 将base_addr_0设置为基地址0
    VERIFY_SUCCESS(err_code); // 验证设置成功

    // 设置基地址1
    err_code = nrf_esb_set_base_address_1(base_addr_1); // 将base_addr_1设置为基地址1
    VERIFY_SUCCESS(err_code); // 验证设置成功

    // 设置前缀地址
    err_code = nrf_esb_set_prefixes(addr_prefix, NRF_ESB_PIPE_COUNT); // 配置8个前缀地址，用于多达8个独立的数据通道
    VERIFY_SUCCESS(err_code); // 验证设置成功

    return err_code; // 返回最后的错误代码
}


/**
 * @brief esb初始化程序
 * 
 * 初始化日志、GPIO、时钟、ESB协议，并不断发送数据包。
 */
void esb_device_init(void)
{
    ret_code_t err_code;

    esb_device_clocks_start();                     // 启动时钟

    err_code = esb_module_init();              // 初始化ESB协议
    APP_ERROR_CHECK(err_code);          // 检查错误

    NRF_LOG_INFO("Enhanced ShockBurst Transmitter Example started.");
	
		err_code = nrf_esb_start_rx(); // 启动ESB接收模式
    APP_ERROR_CHECK(err_code);
}


/***************************************
 *              ESB时钟函数             	*
 ***************************************/
/**
 * @brief 启动时钟
 * 
 * 启动高频时钟以确保系统稳定运行。
 */
void esb_device_clocks_start(void)
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0; // 清除HFCLK启动事件
    NRF_CLOCK->TASKS_HFCLKSTART = 1;    // 启动HFCLK

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0); // 等待HFCLK启动
}
