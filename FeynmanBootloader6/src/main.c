#include <asf.h>
#include "conf_usb.h"

#include <stdint.h>
#include "conf_board.h"
#include "conf_clock.h"
#include "conf_bootloader.h"
#include "sam_ba_monitor.h"

static volatile bool main_b_cdc_enable = false;

#ifdef __SAMG55J19__
#define BLINK_PIN PIO_PA6 //Xplained Pro LED yellow
#define BLINK_PORT PIOA
#endif


/*
 * If BOOT_DOUBLE_TAP_ADDRESS is defined the bootloader is started by
 * quickly tapping two times on the reset button.
 * BOOT_DOUBLE_TAP_ADDRESS must point to a free SRAM cell that must not
 * be touched from the loaded application.
 */
 #define BOOT_DOUBLE_TAP_ADDRESS           (IRAM_ADDR + IRAM_SIZE - 4)
 #define BOOT_DOUBLE_TAP_DATA              (*((volatile uint32_t *) BOOT_DOUBLE_TAP_ADDRESS))

/**
 * \brief Check the application startup condition
 *
 */
static void check_start_application(void)
{
	uint32_t app_start_address;

	/* Load the Reset Handler address of the application */
	app_start_address = *(uint32_t *)(APP_START_ADDRESS + 4);

	/**
	 * Test reset vector of application @APP_START_ADDRESS+4
	 * Stay in SAM-BA if *(APP_START+0x4) == 0xFFFFFFFF
	 * Application erased condition
	 */
	if (app_start_address == 0xFFFFFFFF) {
		/* Stay in bootloader */
		return;
	}
	#if 0
	volatile PortGroup *boot_port = (volatile PortGroup *)(&(PORT->Group[BOOT_LOAD_PIN / 32]));
	volatile bool boot_en;

	/* Enable the input mode in Boot GPIO Pin */
	boot_port->DIRCLR.reg = GPIO_BOOT_PIN_MASK;
	boot_port->PINCFG[BOOT_LOAD_PIN & 0x1F].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	boot_port->OUTSET.reg = GPIO_BOOT_PIN_MASK;
	/* Read the BOOT_LOAD_PIN status */
	boot_en = (boot_port->IN.reg) & GPIO_BOOT_PIN_MASK;

	/* Check the bootloader enable condition */
	if (!boot_en) {
		/* Stay in bootloader */
		return;
	}
	#endif

	#if defined(BOOT_DOUBLE_TAP_ADDRESS)
	#define DOUBLE_TAP_MAGIC 0x07738135
	if(RSTC->RSTC_SR & RSTC_SR_RSTTYP_GENERAL_RST)
	{
    /* On power-on initialize double-tap */
    BOOT_DOUBLE_TAP_DATA = 0;
    }
	else
	{
		if (BOOT_DOUBLE_TAP_DATA == DOUBLE_TAP_MAGIC)
		{
			/* Second tap, stay in bootloader */
			BOOT_DOUBLE_TAP_DATA = 0;
			return;
		}

		/* First tap */
		BOOT_DOUBLE_TAP_DATA = DOUBLE_TAP_MAGIC;

		/* Wait 0.5sec to see if the user tap reset again.
			* The loop value is based on SAMD21 default 1MHz clock @ reset.
			*/
		// SAM4E MAINCK is 4 Mhz after after reset.
		for (uint32_t i=0; i<(125000*4); i++) /* 500ms */
			/* force compiler to not optimize this... */
			__asm__ __volatile__("");

		/* Timeout happened, continue boot... */
		BOOT_DOUBLE_TAP_DATA = 0;
	}
	#endif //DOUBLE_TAP

	/* Rebase the Stack Pointer */
	__set_MSP(*(uint32_t *) APP_START_ADDRESS);

	/* Rebase the vector table base address */
	SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

	/* Jump to application Reset Handler in the application */
	asm("bx %0"::"r"(app_start_address));
}


volatile uint32_t g_ul_ms_ticks = 0;
void SysTick_Handler(void)
{
	g_ul_ms_ticks++;
}

static void mdelay(uint32_t ul_dly_ticks)
{
	uint32_t ul_cur_ticks;

	ul_cur_ticks = g_ul_ms_ticks;
	while ((g_ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);
}

int main (void)
{
	//board_init();

	WDT->WDT_MR = WDT_MR_WDDIS;

	check_start_application();

	udc_start();

	//system_init_flash(120000000);
	flash_init(FLASH_ACCESS_MODE_128, 5);  //Max FWS (flash wait state) value is 5 for SAMG55 - allows 120Mhz read timings using 6 cycles

	if (SysTick_Config(SystemCoreClock / 1000)) { while (1); } // Set Systick to 1ms interval, common to all SAM3 variants

	BLINK_PORT->PIO_OER = BLINK_PIN;

	//flash_unlock((0x00404000), 0x004FC000, 0, 0);

	/* Insert application code here, after the board has been initialized. */

	/* This skeleton code simply sets the LED to the state of the button. */
	while (1) {
		#ifdef CONF_USBCDC_INTERFACE_SUPPORT
		/* Check if a USB enumeration has succeeded and com port was opened */
		if(main_b_cdc_enable) {
			sam_ba_monitor_init(SAM_BA_INTERFACE_USBCDC);
			/* SAM-BA on USB loop */
			while(1) {
				sam_ba_monitor_run();
			}
		}
		#endif
		/* Is button pressed? */
		//udi_cdc_putc('R');
		BLINK_PORT->PIO_CODR = BLINK_PIN;
		mdelay(500);
		BLINK_PORT->PIO_SODR = BLINK_PIN;
		mdelay(500);
		/*
		if (ioport_get_pin_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			// Yes, so turn LED on. 
			ioport_set_pin_level(LED_0_PIN, LED_0_ACTIVE);
		} else {
			// No, so turn LED off. 
			ioport_set_pin_level(LED_0_PIN, !LED_0_ACTIVE);
		} */
	}
}

void main_suspend_action(void)
{

}

void main_resume_action(void)
{

}

void main_sof_action(void)
{

}


bool main_cdc_enable(uint8_t port)
{
	main_b_cdc_enable = true;
	return true;
}

void main_cdc_disable(uint8_t port)
{
	main_b_cdc_enable = false;
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{

}
