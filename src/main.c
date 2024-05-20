#include "esp_common.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "gpio.h"
#include "spi_init.h"
#include "hw_timer.h"
#include "uart.h"
#include "wifi.h"

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
 *******************************************************************************/

uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map)
    {
    case FLASH_SIZE_4M_MAP_256_256:
        rf_cal_sec = 128 - 5;
        break;

    case FLASH_SIZE_8M_MAP_512_512:
        rf_cal_sec = 256 - 5;
        break;

    case FLASH_SIZE_16M_MAP_512_512:
    case FLASH_SIZE_16M_MAP_1024_1024:
        rf_cal_sec = 512 - 5;
        break;

    case FLASH_SIZE_32M_MAP_512_512:
    case FLASH_SIZE_32M_MAP_1024_1024:
        rf_cal_sec = 1024 - 5;
        break;

    default:
        rf_cal_sec = 0;
        break;
    }

    return rf_cal_sec;
}

static xTimerHandle myTimerHandle;

bool ledOn = false;
bool triggered = true;
int val = 0;

void timerTrig(xTimerHandle xTimer)
{
}

void uart_init(void)
{

    uart_div_modify(0, UART_CLK_FREQ / 115200);
}

void hwTimerInterrupt(void)
{

    gpio16_output_set(ledOn);
    ledOn = !ledOn;
    triggered = !triggered;
    check_wifi_status();
}

void init_UART0(void)
{
    UART_ConfigTypeDef uart_config;
    uart_config.baud_rate = BIT_RATE_115200;
    uart_config.data_bits = UART_WordLength_8b;
    uart_config.parity = USART_Parity_None;
    uart_config.stop_bits = USART_StopBits_1;
    uart_config.flow_ctrl = USART_HardwareFlowControl_None;
    uart_config.UART_RxFlowThresh = 120;
    uart_config.UART_InverseMask = UART_None_Inverse;
    UART_ParamConfig(UART0, &uart_config);

    // Set UART0 as the standard output for printf
    UART_SetPrintPort(UART0);
}

void setup(void)
{
    // system_update_cpu_freq(SYS_CPU_80MHZ);
    initSPI();
    gpio16_output_conf();

    myTimerHandle = xTimerCreate(
        "MyTimer",
        100,
        pdTRUE,
        NULL,
        timerTrig);

    hw_timer_init();
    hw_timer_set_func(hwTimerInterrupt);
    hw_timer_arm(400000, true);
    init_UART0();

    wifi_connect();
}

void task_blink(void *ignore)
{
    setup();
    // xTimerStart(myTimerHandle,0);
    while (true)
    {
    }

    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void user_init(void)
{
    xTaskCreate(&task_blink, "startup", 2048, NULL, 1, NULL);
}
