//
// Created by 黄炜杰 on 2020/12/15.
//

#include <stdio.h>
#include <string.h>
#include "msg.h"
#include "24l01.h"
#include "screen.h"

void conn_init(void) {
    NRF24L01_Init();
    while (NRF24L01_Check()) {
        HAL_Delay(400);
    }
    NRF24L01_RX_Mode();
}

conn_type_typedef conn_create() {
    int i;
    uint32_t start_tick;
    pkg_type_typedef rx_status;
    char buf[RX_PLOAD_WIDTH + 1];
    if (SEND_SYN() == PKG_TX_OK) {
        screen_write_lalign("SYN OK", GREEN);
        screen_update();
        for (i = 10; i > 0; --i) {
            start_tick = HAL_GetTick();
            do {
                screen_write_lalign("RECVING", RED);
                screen_update();
                rx_status = pkg_recv(buf);
                screen_write_lalign("GOT A PKG", RED);
                screen_update();
            } while (rx_status == EMPTY && HAL_GetTick() - start_tick <= 1000);
            screen_write_lalign("OUT LOOP", RED);
            screen_update();
            if (rx_status == SYN_TYPE) {
                break;
            }
        }
        if (i == 0) {
            screen_write_lalign("SYN BACK ERR", RED);
            screen_update();
            return CONN_ERR;
        } else {
            screen_write_lalign("SYN BACK OK", GREEN);
            screen_update();
            return CONN_ON;
        }
    } else {
        screen_write_lalign("SYN ERR", RED);
        screen_update();
        return CONN_ERR;
    }
}

pkg_tx_typedef pkg_transmit(const char *msg, pkg_type_typedef type) {
    char buf[TX_PLOAD_WIDTH + 1];
    pkg_tx_typedef status = PKG_TX_OK;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    sprintf(buf, "#%1d:%s", type, msg);
    NRF24L01_TX_Mode();
    if (NRF24L01_TxPacket((uint8_t *) buf) != TX_OK) {
        status = PKG_TX_FAIL;
    }
    NRF24L01_RX_Mode();
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    return status;
}

pkg_type_typedef pkg_recv(char *buf) {
    char raw_buf[RX_PLOAD_WIDTH + 1];
    NRF24L01_RX_Mode();
    memset(buf, 0, RX_PLOAD_WIDTH * sizeof(char));
    if (NRF24L01_RxPacket((uint8_t *) raw_buf)) {
//        screen_write_lalign("<EMPTY>", GREEN);
//        screen_update();
        return EMPTY;
    }
    screen_write_lalign(raw_buf, GREEN);
    screen_update();
    if (raw_buf[0] != '#' || raw_buf[2] != ':') {
        return ERR_TYPE;
    }
    strcpy(buf, raw_buf + 3);
    switch (raw_buf[1] - '0') {
        case SYN_TYPE:
            if (strcmp(buf, SYN_BODY) == 0) {
                return SYN_TYPE;
            }
            return ERR_TYPE;
        case BEAT_TYPE:
            if (strcmp(buf, BEAT_BODY) == 0) {
                return BEAT_TYPE;
            }
            return ERR_TYPE;
        case MSG_TYPE:
            return MSG_TYPE;
        case FIN_TYPE:
            if (strcmp(buf, FIN_BODY) == 0) {
                return FIN_TYPE;
            }
            return ERR_TYPE;
        default:
            return ERR_TYPE;
    }
}

conn_type_typedef conn_close(void) {
    int i;
    uint32_t start_tick;
    pkg_type_typedef rx_status;
    char buf[RX_PLOAD_WIDTH + 1];
    if (SEND_FIN() == PKG_TX_OK) {
        screen_write_lalign("FIN OK", GREEN);
        screen_update();
        for (i = 10; i > 0; --i) {
            start_tick = HAL_GetTick();
            do {
                screen_write_lalign("RECVING", RED);
                screen_update();
                rx_status = pkg_recv(buf);
                screen_write_lalign("GOT A PKG", RED);
                screen_update();
            } while (rx_status == EMPTY && HAL_GetTick() - start_tick <= 1000);
            if (rx_status == FIN_TYPE) {
                break;
            }
        }
        if (i == 0) {
            screen_write_lalign("FIN BACK ERR", RED);
            screen_update();
            return CONN_ERR;
        } else {
            screen_write_lalign("FIN BACK OK", RED);
            screen_update();
            return CONN_OFF;
        }
    } else {
        screen_write_lalign("FIN ERR", RED);
        screen_update();
        return CONN_ERR;
    }
}