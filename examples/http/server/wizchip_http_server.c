/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>

#include "port_common.h"

#include "wizchip_conf.h"
#include "wizchip_spi.h"

#include "httpServer.h"
#include "web_page.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
#define PLL_SYS_KHZ (48 * 1000)

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

/* Socket */
#define HTTP_SOCKET_MAX_NUM 4

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 1, 115},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 1, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
#if _WIZCHIP_ > W5500
        .lla = {0xfe, 0x80, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x02, 0x08, 0xdc, 0xff,
                0xfe, 0x57, 0x57, 0x25},             // Link Local Address
        .gua = {0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00},             // Global Unicast Address
        .sn6 = {0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00},             // IPv6 Prefix
        .gw6 = {0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00},             // Gateway IPv6 Address
        .dns6 = {0x20, 0x01, 0x48, 0x60,
                0x48, 0x60, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x88, 0x88},             // DNS6 server
        .ipmode = NETINFO_STATIC_ALL
#else
        .dhcp = NETINFO_STATIC        
#endif
};

/* HTTP */
static uint8_t g_http_send_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_http_recv_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_http_socket_num_list[HTTP_SOCKET_MAX_NUM] = {0, 1, 2, 3};

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void);

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{
    /* Initialize */
    uint8_t i = 0;

    set_clock_khz();

    stdio_init_all();
    sleep_ms(3000);    
    wizchip_spi_initialize();
    printf("initialized wizchip_spi\n");

    wizchip_cris_initialize();
    printf("initialized wizchip_cris\n");

    wizchip_reset();
    printf("reset wizchip\n");
    wizchip_initialize();
    printf("initialized wizchip\n");
    wizchip_check();
    printf("checked wizchip\n");

    network_initialize(g_net_info);

    httpServer_init(g_http_send_buf, g_http_recv_buf, HTTP_SOCKET_MAX_NUM, g_http_socket_num_list);

    /* Get network information */
    print_network_information(g_net_info);

    /* Register web page */
    reg_httpServer_webContent("index.html", index_page);

    /* Infinite loop */
    while (1)
    {
        /* Run HTTP server */
        for (i = 0; i < HTTP_SOCKET_MAX_NUM; i++)
        {
            httpServer_run(i);
        }
    }
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}
