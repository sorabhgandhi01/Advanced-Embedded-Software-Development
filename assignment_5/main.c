/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 *
 * Sorabh Gandhi
 *
 * This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"

#include "inc/hw_memmap.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "timers.h"
#include "semphr.h"

typedef struct led_log
{
    char name[24];
    char timestamp[32];
    uint32_t count;
}led_log;

typedef struct temp_log
{
    char timestamp[32];
    uint32_t temperature;
}temp_log;

// Task declarations
void LED_Task(void *pvParameters);
void Logger_Task(void *pvParameters);
void temp102_task(void *pvParameters);
void alert_task(void *pvParameters);
void vTimerCallback( TimerHandle_t xTimer );
void tTimerCallback( TimerHandle_t xTimer );

uint32_t toggle_flag = 0;
uint32_t temp_flag = 0;
uint32_t output_clock_rate_hz;

QueueHandle_t xQueue1, xQueue2;

TaskHandle_t alert_handler = NULL;

// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 115200, SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    xQueue1 = xQueueCreate(100, sizeof(struct led_log));
    if (xQueue1 == NULL) {

    }

    xQueue2 = xQueueCreate(100, sizeof(struct temp_log));
    if (xQueue2 == NULL) {

    }


    // Create demo tasks
    xTaskCreate(LED_Task, (const portCHAR *)"LED Task",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(Logger_Task, (const portCHAR *)"Logger Task",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(temp102_task, (const portCHAR *)"Temp Sensor",
                    configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(alert_task, (const portCHAR *)"Alert Task",
                        configMINIMAL_STACK_SIZE, NULL, 1, &alert_handler);

    vTaskStartScheduler();
    return 0;
}

void vTimerCallback( TimerHandle_t xTimer )
 {
    toggle_flag = 1;
 }
// Flash the LEDs on the launchpad
void LED_Task(void *pvParameters)
{
    uint32_t status = 0;
    char name[24] = "Sorabh Gandhi";
    led_log led = {0};

    TimerHandle_t xTimers_led = xTimerCreate
                    (
                    "LED_Timer",
                    100,
                    pdTRUE,
                    ( void * ) 0,
                    vTimerCallback
                    );

    xTimerStart(xTimers_led, 0);

    for (;;)
    {
        if (toggle_flag == 1)
        {
            LEDRead(&status);

            if ((status & 0x01) == 0x00)
            {
                GPIOPinWrite(CLP_D1_PORT, CLP_D1_PIN, CLP_D1_PIN);
            } else {
                GPIOPinWrite(CLP_D1_PORT, CLP_D1_PIN, 0);
            }

            if ((status & 0x02) == 0x00)
            {
                GPIOPinWrite(CLP_D2_PORT, CLP_D2_PIN, CLP_D2_PIN);
            } else {
                GPIOPinWrite(CLP_D2_PORT, CLP_D2_PIN, 0);
            }

            sprintf(led.name, "%s", name);
            sprintf(led.timestamp, "%d", xTaskGetTickCount());
            led.count = (led.count + 1);

            xQueueSendToBack(xQueue1, (void *)&led, 10);

            toggle_flag = 0;
        }
    }
}

uint16_t i2c_get_byte()
{
    uint8_t MSB, LSB;
    uint16_t data;

    I2CMasterSlaveAddrSet(I2C2_BASE, 0x48, false);
    I2CMasterDataPut(I2C2_BASE, 0x00);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    while (!I2CMasterBusy(I2C2_BASE));
    while (I2CMasterBusy(I2C2_BASE));

    I2CMasterSlaveAddrSet(I2C2_BASE, 0x48, true);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    while (!I2CMasterBusy(I2C2_BASE));
    while (I2CMasterBusy(I2C2_BASE));

    MSB = I2CMasterDataGet(I2C2_BASE);

    I2CMasterSlaveAddrSet(I2C2_BASE, 0x48, true);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    while (!I2CMasterBusy(I2C2_BASE));
    while (I2CMasterBusy(I2C2_BASE));

    LSB = I2CMasterDataGet(I2C2_BASE);

    data = ((MSB << 8) | (LSB)) >> 4;
    return data;
}

void tTimerCallback( TimerHandle_t xTimer )
 {
    temp_flag = 1;
 }

void temp102_task(void *pvParameters)
{
    uint16_t get_temp;
    uint32_t temp_in_integer;
    float temp_in_float;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    GPIOPinConfigure(GPIO_PN4_I2C2SDA);
    GPIOPinConfigure(GPIO_PN5_I2C2SCL);

    GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);
    GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);

    I2CMasterInitExpClk(I2C2_BASE, output_clock_rate_hz, false);

    temp_log send_temp_log = { 0 };

    TimerHandle_t xTimers_temp = xTimerCreate
                        (
                        "TEMP_Timer",
                        1000,
                        pdTRUE,
                        ( void * ) 0,
                        tTimerCallback
                        );

    xTimerStart(xTimers_temp, 0);

    while (1)
    {
        if (temp_flag == 1)
        {
            get_temp = i2c_get_byte();

            temp_in_float = get_temp * 0.0625;
            temp_in_integer = (uint32_t)(10000 * temp_in_float);

            send_temp_log.temperature = temp_in_integer;

            sprintf(send_temp_log.timestamp, "%d", xTaskGetTickCount());

            xQueueSendToBack(xQueue2, (void *)&send_temp_log, 10);

            if (temp_in_float > 29) {
                xTaskNotifyGive(alert_handler);
            }

            //UARTprintf("\r\n Temperature = %d.%d", (temp_in_integer / 10000), (temp_in_integer % 10000));
            temp_flag = 0;
        }
     }
}


// Write text over the Stellaris debug interface UART port
void Logger_Task(void *pvParameters)
{
    led_log recv_led;
    temp_log recv_temp;

    UARTprintf("\r\nHello, world from FreeRTOS 9.0!");
    for (;;)
    {
        //portMAX_DELAY
        BaseType_t recv_status = xQueueReceive(xQueue1, &recv_led, 10);

        if (recv_status == pdTRUE) {
            UARTprintf("\r\n%s\t%s\t%d", recv_led.name, recv_led.timestamp, recv_led.count);
        }

        //portMAX_DELAY
        recv_status = xQueueReceive(xQueue2, &recv_temp, 10);

        if (recv_status == pdTRUE) {
            UARTprintf("\r\n%s\t%d.%d", recv_temp.timestamp, (recv_temp.temperature / 10000), (recv_temp.temperature % 10000));
        }
    }
}


void alert_task(void *pvParameters)
{
    uint32_t recv_status;

    for (;;)
    {
        recv_status = ulTaskNotifyTake(pdTRUE, (TickType_t) portMAX_DELAY);

        if (recv_status > 0) {
            UARTprintf("[ALERT] Temperature warning triggered");
        }
    }
}


/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
