//
// Created by 黄炜杰 on 2020/12/15.
//

#ifndef NRFTEST_MSG_H
#define NRFTEST_MSG_H

#include "main.h"
#include "24l01.h"

typedef enum CONN_TYPE {
    CONN_OFF,
    CONN_ON,
    CONN_PENDING,
    CONN_AWAIT,
    CONN_ERR
} conn_type_typedef;

typedef enum PKG_TX_TYPE {
    PKG_TX_OK,
    PKG_TX_FAIL
} pkg_tx_typedef;

typedef enum PKG_HEADER_TYPE {
    SYN_TYPE,
    BEAT_TYPE,
    MSG_TYPE,
    FIN_TYPE,
    ERR_TYPE,
    EMPTY
} pkg_type_typedef;

//#define SYN_TYPE 0
//#define BEAT_TYPE 1
//#define MSG_TYPE 2
//#define FIN_TYPE 3
//#define ERR_TYPE 4
//#define EMPTY 5

#define SYN_BODY "SYN"
#define BEAT_BODY "BEAT"
#define FIN_BODY "FIN"

//#define PKG_TX_OK 0
//#define PKG_TX_FAIL 1

//#define CONN_OFF 0
//#define CONN_ON 1
//#define CONN_PENDING 2
//#define CONN_AWAIT 3
//#define CONN_ERR 4

void conn_init(void);

conn_type_typedef conn_create(const uint8_t *rx_addr, const uint8_t *tx_addr);

pkg_tx_typedef pkg_transmit(const char *msg, pkg_type_typedef type);

pkg_type_typedef pkg_recv(char *buf);

conn_type_typedef conn_close(void);

#define SEND_SYN() (pkg_tx_typedef) pkg_transmit(SYN_BODY, SYN_TYPE)
#define SEND_BEAT() (pkg_tx_typedef) pkg_transmit(BEAT_BODY, BEAT_TYPE)
#define SEND_MSG(msg) (pkg_tx_typedef) pkg_transmit((msg), MSG_TYPE)
#define SEND_FIN() (pkg_tx_typedef) pkg_transmit(FIN_BODY, FIN_TYPE)

#endif //NRFTEST_MSG_H
