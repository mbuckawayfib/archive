/*
 *  TP-LINK TL-WA850RE v9
 *
 *  Copyright (C) 2016 Kris Braun <kris.braun@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define TL_WA850RE_GPIO_LED_LAN		14
#define TL_WA850RE_GPIO_LED_WLAN	13
#define TL_WA850RE_GPIO_LED_RE		15
#define TL_WA850RE_GPIO_LED_SIGNAL1	0
#define TL_WA850RE_GPIO_LED_SIGNAL2	1
#define TL_WA850RE_GPIO_LED_SIGNAL3	2
#define TL_WA850RE_GPIO_LED_SIGNAL4	3
#define TL_WA850RE_GPIO_LED_SIGNAL5	4

#define TL_WA850RE_GPIO_BTN_RESET	12
#define TL_WA850RE_GPIO_BTN_WPS		16

#define TL_WA850RE_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_WA850RE_KEYS_DEBOUNCE_INTERVAL (3 * TL_WA850RE_KEYS_POLL_INTERVAL)

static const char *tl_wa850re_v2_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_wa850re_v2_flash_data = {
	.part_probes	= tl_wa850re_v2_part_probes,
};

static struct gpio_led tl_wa850re_v2_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:blue:lan",
		.gpio		= TL_WA850RE_GPIO_LED_LAN,
		.active_low	= 1,
	}, {
		.name		= "tp-link:blue:wlan",
		.gpio		= TL_WA850RE_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "tp-link:blue:re",
		.gpio		= TL_WA850RE_GPIO_LED_RE,
		.active_low	= 1,
	}, {
		.name		= "tp-link:blue:signal1",
		.gpio		= TL_WA850RE_GPIO_LED_SIGNAL1,
		.active_low	= 1,
	}, {
		.name		= "tp-link:blue:signal2",
		.gpio		= TL_WA850RE_GPIO_LED_SIGNAL2,
		.active_low	= 1,
	}, {
		.name		= "tp-link:blue:signal3",
		.gpio		= TL_WA850RE_GPIO_LED_SIGNAL3,
		.active_low	= 1,
	}, {
		.name		= "tp-link:blue:signal4",
		.gpio		= TL_WA850RE_GPIO_LED_SIGNAL4,
		.active_low	= 1,
	}, {
		.name		= "tp-link:blue:signal5",
		.gpio		= TL_WA850RE_GPIO_LED_SIGNAL5,
		.active_low	= 1,
	},
};

static struct gpio_keys_button tl_wa850re_v2_gpio_keys[] __initdata = {
	{
		.desc		   = "Reset button",
		.type		   = EV_KEY,
		.code		   = KEY_RESTART,
		.debounce_interval = TL_WA850RE_KEYS_DEBOUNCE_INTERVAL,
		.gpio		   = TL_WA850RE_GPIO_BTN_RESET,
		.active_low	   = 1,
	}, {
		.desc		   = "WPS",
		.type		   = EV_KEY,
		.code		   = KEY_WPS_BUTTON,
		.debounce_interval = TL_WA850RE_KEYS_DEBOUNCE_INTERVAL,
		.gpio		   = TL_WA850RE_GPIO_BTN_WPS,
		.active_low	   = 1,
	},
};


static void __init tl_ap143_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 tmpmac[ETH_ALEN];

	ath79_register_m25p80(&tl_wa850re_v2_flash_data);

	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_mdio(0, 0x0);

	/* LAN */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 0);
	ath79_register_eth(1);

	/* WAN */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);
	ath79_register_eth(0);

	ath79_init_mac(tmpmac, mac, 0);
	ath79_register_wmac(ee, tmpmac);
}

static void __init tl_wa850re_v2_setup(void)
{
	tl_ap143_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(tl_wa850re_v2_leds_gpio),
				 tl_wa850re_v2_leds_gpio);

	ath79_register_gpio_keys_polled(1, TL_WA850RE_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(tl_wa850re_v2_gpio_keys),
					tl_wa850re_v2_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_TL_WA850RE_V2, "TL-WA850RE-v2", "TP-LINK TL-WA850RE v2",
	     tl_wa850re_v2_setup);
