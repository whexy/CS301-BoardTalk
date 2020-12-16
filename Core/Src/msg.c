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

conn_type_typedef conn_create(const uint8_t *rx_addr, const uint8_t *tx_addr) {
    int i;
    pkg_tx_typedef tx_status;
    pkg_type_typedef rx_status;
    char buf[RX_PLOAD_WIDTH + 1];
    if (rx_addr != NULL) {
        for (i = 0; i < RX_ADR_WIDTH; ++i) {
            RX_ADDRESS[i] = rx_addr[i];
        }
    }
    if (tx_addr != NULL) {
        for (i = 0; i < TX_ADR_WIDTH; ++i) {
            TX_ADDRESS[i] = tx_addr[i];
        }
    }
    tx_status = SEND_SYN();
    if (tx_status == PKG_TX_OK) {
        screen_write_lalign("SYN OK", GREEN);
        screen_update();
        for (i = 10; i > 0; --i) {
            do {
                rx_status = pkg_recv(buf);
                HAL_Delay(100);
            } while (rx_status == EMPTY);
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
    sprintf(buf, "#%1d:%s", type, msg);
    NRF24L01_TX_Mode();
    if (NRF24L01_TxPacket((uint8_t *) buf) != TX_OK) {
        status = PKG_TX_FAIL;
    }
    NRF24L01_RX_Mode();
    return status;
}

pkg_type_typedef pkg_recv(char *buf) {
    char raw_buf[RX_PLOAD_WIDTH + 1];
    NRF24L01_RX_Mode();
    memset(buf, 0, RX_PLOAD_WIDTH * sizeof(char));
    if (NRF24L01_RxPacket((uint8_t *) raw_buf)) {
        screen_write_lalign("<EMPTY>", GREEN);
        screen_update();
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
    pkg_type_typedef rx_status;
    char buf[RX_PLOAD_WIDTH + 1];
    if (SEND_FIN() == FIN_TYPE) {
        screen_write_lalign("FIN OK", GREEN);
        screen_update();
        for (i = 10; i > 0; --i) {
            do {
                rx_status = pkg_recv(buf);
            } while (rx_status == EMPTY);
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