//
// Created by 黄炜杰 on 2020/12/12.
//
#include "screen.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>

char screen_msg[LINE_CNT][LINE_BUF_SIZE];
uint16_t line_color[LINE_CNT];
int print_idx, write_idx;
int is_full;

/**
 * Initializes the LCD screen and buffer
 */
void screen_init(void) {
    LCD_Init();
    screen_flush();
}

/**
 * Writes the converted message to buffer
 * @param msg The converted message
 * @param color The color of the message
 */
void screen_write(const char *msg, uint16_t color) {
    const char *p = msg;
    uint len = strlen(msg);
    do {
        strncpy(screen_msg[write_idx], p, LINE_CHARS);
        line_color[write_idx++] = color;
        if (write_idx == LINE_CNT) {
            write_idx = 0;
            is_full = 1;
        }
        p += LINE_CHARS;
    } while (p < msg + len);
    if (is_full) {
        print_idx = write_idx;
        if (print_idx == LINE_CNT) {
            print_idx = 0;
        }
    }
}

/**
 * Writes the message to buffer, left-aligned
 * @param msg The message
 * @param color The color of the message
 */
void screen_write_lalign(const char *msg, uint16_t color) {
    screen_write(msg, color);
}

/**
 * Writes the message to buffer, right-aligned
 * @param msg The message
 * @param color The color of the message
 */
void screen_write_ralign(const char *msg, uint16_t color) {
    static char buf[BUFFER_SIZE];
    static char tmp[LINE_CHARS];
    memset(buf, 0, sizeof(buf));
    char *p = buf;
    uint len = strlen(msg);
    strncpy(buf, msg, len);
    while (p + LINE_CHARS < buf + len) {
        p += LINE_CHARS;
    }
    strcpy(tmp, p);
    sprintf(p, RALIGN, tmp);
    screen_write(buf, color);
    strcpy(p, tmp);
}

/**
 * Clears the screen, not erasing the buffer
 */
void screen_clear(void) {
    LCD_Clear(WHITE);
    BACK_COLOR = WHITE;
    LCD_DrawRectangle(BOX_START_X, BOX_START_Y, BOX_END_X, BOX_END_Y);
}

/**
 * Shows the buffer to the LCD screen
 */
void screen_show(void) {
    int i, idx;
    uint16_t point_save_color;
    point_save_color = POINT_COLOR;
    for (i = 0, idx = print_idx; i < LINE_CNT; ++i) {
        POINT_COLOR = line_color[idx];
        LCD_ShowString(LINE_START_X, LINE_START_Y + i * LINE_OFFSET, LINE_LENGTH, LINE_HEIGHT, FONT_SIZE,
                       (uint8_t *) (screen_msg[idx]));
        if (++idx == LINE_CNT) {
            idx = 0;
        }
    }
    POINT_COLOR = point_save_color;
}

/**
 * Clears the screen and shows the buffered message
 */
void screen_update(void) {
    screen_clear();
    screen_show();
}

/**
 * Clears the screen and buffer
 */
void screen_flush(void) {
    print_idx = 0;
    write_idx = 0;
    is_full = 0;
    memset(screen_msg, 0, sizeof(screen_msg));
    memset(line_color, 0, sizeof(line_color));
    screen_clear();
}