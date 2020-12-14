//
// Created by 黄炜杰 on 2020/12/12.
//

#ifndef NRFTEST_SCREEN_H
#define NRFTEST_SCREEN_H

#include <stdint.h>
#include "lcd.h"

// Screen wrapper macros

#define STR(X) #X
#define AS_STR(X) STR(X)
#define BUFFER_SIZE 4096
#define FONT_SIZE 12
#define LINE_CNT 19
#define LINE_CHARS 35
#define LINE_BUF_SIZE (LINE_CHARS + 5)
#define LINE_HEIGHT (FONT_SIZE)
#define LINE_OFFSET 15
#define BOX_START_X 10
#define BOX_START_Y 10
#define PADDING_X 5
#define PADDING_Y 10
#define LINE_START_X (BOX_START_X + PADDING_X)
#define LINE_START_Y (BOX_START_Y + PADDING_Y)
#define BOX_END_X (BOX_START_X + LINE_CHARS * FONT_SIZE / 2 + 2 * PADDING_X)
#define BOX_END_Y (BOX_START_Y + LINE_CNT * (LINE_OFFSET) + 3 * PADDING_Y - LINE_OFFSET)
#define LINE_LENGTH (BOX_END_X - BOX_END_Y)
#define RALIGN "%" AS_STR(LINE_CHARS) "s"

// Screen wrapper function prototypes

void screen_init(void);

void screen_write_lalign(const char *msg, uint16_t color);

void screen_write_ralign(const char *msg, uint16_t color);

void screen_clear(void);

void screen_show(void);

void screen_update(void);

void screen_flush(void);

#endif //NRFTEST_SCREEN_H
