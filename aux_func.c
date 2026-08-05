/* 
 *
 * Copyright (C) 2026 Microchip Technology Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; If not, see <https://www.gnu.org/licenses/>.
 *
 */
/*
 * Note: all the codes here are taken from https://github.com/MicrochipTech/mepa-app-malibu10-rpi/
 * Used a different source file so the mepa app looks clean.
 */

#include <stdint.h>
#include <microchip/ethernet/phy/api.h>
#include <vtss_phy_10g_api.h>
#include <vtss_phy_api.h>
#include <stdio.h>

/*
 * Below codes are taken from https://github.com/MicrochipTech/mepa-app-malibu10-rpi
 */
// Reference: mesa/meba/src/sparx5/meba.c
typedef struct {
    vtss_gpio_10g_no_t gpio_tx_dis;      /* Tx Disable GPIO number */
    vtss_gpio_10g_no_t gpio_aggr_int;    /* Aggregated Interrupt-0 GPIO number */
    vtss_gpio_10g_no_t gpio_i2c_clk;     /* GPIO Pin selection as I2C_CLK for I2C
                                            communication with SFP  */
    vtss_gpio_10g_no_t gpio_i2c_dat;     /* GPIO Pin selection as I2C_DATA for I2C
                                            communication with SFP */
    vtss_gpio_10g_no_t gpio_virtual;     /* Per port Virtual GPIO number,for internal GPIO usage */
    vtss_gpio_10g_no_t gpio_sfp_mod_det; /* GPIO Pin selection as SFP module detect */
    vtss_gpio_10g_no_t gpio_tx_fault;    /* GPIO Pin for TX_FAULT */
    vtss_gpio_10g_no_t gpio_rx_los;      /* GPIO Pin for RX_LOS */
    uint32_t           aggr_intrpt;      /* Channel interrupt bitmask */
} appl_malibu_gpio_port_map_t;

// Reference: mesa/meba/src/sparx5/meba.c
static const appl_malibu_gpio_port_map_t malibu_gpio_map[] = {
    // PHY CH0
    {
        .gpio_tx_dis        = 4,
        .gpio_aggr_int      = 34,
        .gpio_i2c_clk       = 2,
        .gpio_i2c_dat       = 3,
        .gpio_virtual       = 0,
        .gpio_sfp_mod_det   = 1,
        .gpio_tx_fault      = 5,
        .gpio_rx_los        = 6,
        .aggr_intrpt        = ((1 << VTSS_10G_GPIO_AGGR_INTRPT_CH0_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_0_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_1_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_GPIO_INTR_EN)),
    },
    // PHY CH1
    {
        .gpio_tx_dis        = 12, 
        .gpio_aggr_int      = 34,
        .gpio_i2c_clk       = 10,
        .gpio_i2c_dat       = 11,
        .gpio_virtual       = 0,
        .gpio_sfp_mod_det   = 9,
        .gpio_tx_fault      = 13,
        .gpio_rx_los        = 14,
        .aggr_intrpt        = ((1 << VTSS_10G_GPIO_AGGR_INTRPT_CH1_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_0_INTR1_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_1_INTR1_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_GPIO_INTR_EN)),
     },    
     // PHY CH2
    {
        .gpio_tx_dis        = 20,
        .gpio_aggr_int      = 34,
        .gpio_i2c_clk       = 18,
        .gpio_i2c_dat       = 19,
        .gpio_virtual       = 0,
        .gpio_sfp_mod_det   = 17,
        .gpio_tx_fault      = 21,
        .gpio_rx_los        = 22,
        .aggr_intrpt        = ((1 << VTSS_10G_GPIO_AGGR_INTRPT_CH2_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_0_INTR2_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_1_INTR2_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_GPIO_INTR_EN)),
     },
     // PHY CH3
    {
        .gpio_tx_dis        = 28,
        .gpio_aggr_int      = 34,
        .gpio_i2c_clk       = 26,
        .gpio_i2c_dat       = 27,
        .gpio_virtual       = 0,
        .gpio_sfp_mod_det   = 25,
        .gpio_tx_fault      = 29,
        .gpio_rx_los        = 30,
        .aggr_intrpt        = ((1 << VTSS_10G_GPIO_AGGR_INTRPT_CH3_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_0_INTR3_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_1_INTR3_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_GPIO_INTR_EN)),
     },
};

// Configuring GPIOs are required as SFP side band signals 
// like TX_DIS needs to be cleared so link partner would be able to detect link.

mepa_rc aux_malibu_gpio_conf(mepa_port_no_t port_no)
{
    vtss_rc rc = VTSS_RC_OK;
    vtss_gpio_10g_gpio_mode_t gpio_conf = {};
    const appl_malibu_gpio_port_map_t *gmap = &malibu_gpio_map[port_no];

    // Initialize Malibu PHY GPIOs for VSC8258EV
    // See the VSC8258EV schematics in the Product page: https://www.microchip.com/en-us/product/vsc8258
    //      Scroll down to find "VSC8256/VSC8257/VSC8258 Evaluation Board Design Files"
    // NOTE: main reference for this function is meba/src/sparx5/meba.c > malibu_gpio_conf()
    // -------------
    // Also note that as of SW-MEPA 2025.12, no implementation of mepa_gpio_mode_set() exists for Malibu PHYs
    // yet, so we call vtss APIs directly.

    /* ********************************************************** */
    // GPIO Input functionality
    /* ********************************************************** */
    // SFP_MOD_DET
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_sfp_mod_det, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        gpio_conf.mode = VTSS_10G_PHY_GPIO_IN;
        gpio_conf.input = VTSS_10G_GPIO_INPUT_NONE;
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_sfp_mod_det, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        printf("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: INPUT (SFP_MOD_DET)\n", port_no, gmap->gpio_sfp_mod_det);
        return rc;
    }
    else
    {
        printf("Malibu GPIO Input: SFP_MOD_DET configuration for port %d, gpio %d \n", port_no, gmap->gpio_sfp_mod_det);
    }

    // RX_LOS
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_rx_los, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        gpio_conf.mode = VTSS_10G_PHY_GPIO_IN;
        gpio_conf.input = VTSS_10G_GPIO_INPUT_LINE_LOPC;
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_rx_los, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        printf("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: INPUT (RX_LOS)\n", port_no, gmap->gpio_rx_los);
        return rc;
    }
    else
    {
        printf("Malibu GPIO Input: RX_LOS configuration for port %d, gpio %d \n", port_no, gmap->gpio_rx_los);
    }

    // TX_FAULT
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_tx_fault, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        gpio_conf.mode = VTSS_10G_PHY_GPIO_IN;
        gpio_conf.input = VTSS_10G_GPIO_INPUT_NONE;
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_tx_fault, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        printf("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: INPUT (TX_FAULT)\n", port_no, gmap->gpio_tx_fault);
        return rc;
    }
    else
    {
        printf("Malibu GPIO Input: TX_FAULT configuration for port %d, gpio %d \n", port_no, gmap->gpio_tx_fault);
    }

    /* ********************************************************** */
    // GPIO Output functionality
    /* ********************************************************** */
    // TXDIS
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_tx_dis, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        gpio_conf.mode = VTSS_10G_PHY_GPIO_DRIVE_LOW;
        gpio_conf.in_sig = VTSS_10G_GPIO_INTR_SGNL_NONE;
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_tx_dis, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        printf("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: DRIVE_LOW (TX_DISABLE)\n", port_no, gmap->gpio_tx_dis);
        return rc;
    }
    else
    {
        printf("Malibu GPIO Output: Driving LOW configuration for port %d, gpio %d (TX_DISABLE)\n", port_no, gmap->gpio_tx_dis);
    }

    /* ********************************************************** */
    // GPIO I2C Pins
    /* ********************************************************** */
    // SDA
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_i2c_dat, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        printf("Malibu port %d I2C DAT pin %d\n", port_no, gmap->gpio_i2c_dat);
        gpio_conf.mode = VTSS_10G_PHY_GPIO_OUT;
        gpio_conf.in_sig = VTSS_10G_GPIO_INTR_SGNL_I2C_MSTR_DATA_OUT;
        gpio_conf.p_gpio = 0;   // Route the internal signal "SDA Output" to GPIO0_OUT
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_i2c_dat, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        printf("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: OUTPUT (I2C SDA)\n", port_no, gmap->gpio_i2c_dat);
        return rc;
    }
    else
    {
        printf("Malibu GPIO Output: I2C Master DATA configuration for port %d, gpio %d\n", port_no, gmap->gpio_i2c_dat);
    }

    // SCL
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_i2c_clk, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        printf("Malibu port %d I2C CLK pin %d\n", port_no, gmap->gpio_i2c_clk);
        gpio_conf.mode = VTSS_10G_PHY_GPIO_OUT;
        gpio_conf.in_sig = VTSS_10G_GPIO_INTR_SGNL_I2C_MSTR_CLK_OUT;
        gpio_conf.p_gpio = 1;   // Route the internal signal "SCL Output" to GPIO1_OUT
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_i2c_clk, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        printf("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: OUTPUT (I2C SCL)\n", port_no, gmap->gpio_i2c_clk);
        return rc;
    }
    else
    {
        printf("Malibu GPIO Output: I2C Master CLK configuration for port %d, gpio %d\n", port_no, gmap->gpio_i2c_clk);
    }
    /* ********************************************************** */

    return rc;
}

mepa_rc aux_malibu_ckout_conf(mepa_port_no_t port_no)
{
    vtss_phy_10g_sckout_conf_t    sckout;
    vtss_phy_10g_lane_sync_conf_t ls;

    /* 1) Configure the SCKOUT DF2F macro (freq + output buffer) */
    memset(&sckout, 0, sizeof(sckout));
    sckout.enable      = TRUE;
    sckout.mode        = VTSS_PHY_10G_SYNC_DISABLE;   /* selected via lane_sync */
    sckout.freq        = VTSS_PHY_10G_SCKOUT_156_25;  /* or _125_00 */
    sckout.src         = VTSS_CKOUT_NO_SQUELCH;
    sckout.squelch_inv = FALSE;
    if (vtss_phy_10g_sckout_conf_set(NULL, port_no, &sckout) != VTSS_RC_OK) {
        printf("sckout_conf_set failed on port %u\n", port_no);
        return VTSS_RC_ERROR;
    }

    /* 2) Bind Line-2 recovered clock to SCKOUT and enable synth (mandatory) */
    memset(&ls, 0, sizeof(ls));
    ls.enable   = TRUE;
    ls.rx_macro = VTSS_PHY_10G_RX_MACRO_LINE;
    ls.rx_ch    = 2;                              /* Line 2 */
    ls.tx_macro = VTSS_PHY_10G_TX_MACRO_SCKOUT;
    ls.tx_ch    = 0;
    if (vtss_phy_10g_lane_sync_set(NULL, port_no, &ls) != VTSS_RC_OK) {
        printf("lane_sync_set failed on port %u\n", port_no);
        return VTSS_RC_ERROR;
    }

    return VTSS_RC_OK;
}