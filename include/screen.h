#ifndef SCREEN_H_
#define SCREEN_H_

#include<stdarg.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

enum screen_color {
    SCREEN_BLACK,
    SCREEN_BLUE,
    SCREEN_GREEN,
    SCREEN_CYAN,
    SCREEN_RED,
    SCREEN_MAGENTA,
    SCREEN_BROWN,
    SCREEN_LIGHT_GREY,
    SCREEN_DARK_GREY,
    SCREEN_LIGHT_BLUE,
    SCREEN_LIGHT_GREEN,
    SCREEN_LIGHT_CYAN,
    SCREEN_LIGHT_RED,
    SCREEN_LIGHT_MAGENTA,
    SCREEN_LIGHT_BROWN,
    SCREEN_WHITE
};

//Screen Ports Commands
#define SCREEN_HIGH_BYTE_CMD 14
#define SCREEN_LOW_BYTE_CMD 15

// Screen device I / O ports
# define REG_SCREEN_CTRL 0x3D4
# define REG_SCREEN_DATA 0x3D5

#define PACK_COLOR_ATTR(bg,fg) ((bg << 4) | fg)
#define DEFAULT_COLOR_ATTR PACK_COLOR_ATTR(SCREEN_BLACK, SCREEN_WHITE)

extern char attribute_byte;

extern void kputchar(char c);
extern void print_char(char character, int col, int row, char attribute_byte);
extern void print_at(char* message, int col, int row, char attribute_byte);
extern void print(char* message, char attribute_byte);
extern int get_screen_offset(int col,int row);
extern void clear_screen(char attribute_byte);
extern void fill_screen(char character,char attribute_byte);
extern int get_cursor(void);
extern int set_cursor(int offset);
extern int handle_scrolling(int offset);
extern void kitoa(char* buffer, int base, int input);
extern void kprintf(char* format, ...);
extern void ksprintf(char* buff, char* format, ...);
extern int kstrlen(const char* buffer);
#endif
