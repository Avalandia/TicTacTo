#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_uarte.h>
#include <nrf_gpio.h>
#include <nrfx_systick.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nrfx_rtc.h>
#include <assert.h>
#include <nrfx_glue.h>

#define PIN_TXD (20)
#define PIN_RXD (22)


static nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
static uint8_t buffer[1];  // Buffer for receiving data
static volatile bool program_running = true;


void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    char msg[1000];
    nrfx_uarte_t * p_inst = p_context;
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE)
    {
        memset(msg, 0, sizeof(msg));
        switch (buffer[0])
        {
            case 'q':
                program_running = false;
                sprintf(msg, "Program avslutas \n\r");
                break;
			case 'Q':
                program_running = false;
                sprintf(msg, "Program avslutas \n\r");
                break;
            default:
                sprintf(msg, "%c \n\r", buffer[0]);
                break;
        }
        nrfx_uarte_tx(&instance, (uint8_t*)msg, strlen(msg), 0);
        nrfx_systick_delay_ms(500);

        // Restart reception if program is still running
        if (program_running)
        {
            nrfx_uarte_rx(&instance, buffer, sizeof(buffer));
        }
    }
}


int main(void)
{
    nrfx_systick_init();
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(PIN_TXD, PIN_RXD);
    nrfx_uarte_init(&instance, &config, uarte_handler);

    // Start receiving data
    nrfx_uarte_rx(&instance, buffer, sizeof(buffer));

    uint8_t msg[1000];
    uint16_t i = 0;

    while (program_running)
    {
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "%d \n\r", i);
        nrfx_uarte_tx(&instance, (uint8_t*)msg, sizeof(msg), 0);
        while (nrfx_uarte_tx_in_progress(&instance)) {};
        i++;

        nrfx_systick_delay_ms(500);
    }

	return 0;
}