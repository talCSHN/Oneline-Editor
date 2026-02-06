#ifndef __MENU_H__
#define __MENU_H__

#define WIDTH 196
#define LENGTH 179
#define LOCATION 0xb8000000
#define TEXT_ROW 4       /* 텍스트입력창 y축 좌표 */
#define TEXT_START_COL 6 /* 텍스트입력창 x축 좌표 */
#define SAVE_TXT_ROW 10
#define SAVE_TXT_COL 6
#define MENU_NEW     0
#define MENU_LOAD    1
#define MENU_SAVE    2
#define MENU_SAVEAS  3
#define MENU_EXIT    4

typedef struct char_attrib
{
    char character;
    struct char_attrib* prev;
    struct char_attrib* next;
}TEXT;

extern TEXT* head; /* 첫 번째 글자 */
extern TEXT* current; /* 현재 커서 가리키는 글자*/
extern int cursor_x; /* 커서 x축 좌표 */

/* PROGRAM START */
void init_editor(void);
void run_editor(void);

/* UI */
void init_monitor(void);
void clear_monitor(void);
char far* cal_location(int, int);
void write_str(int, int, char*);
void make_txtbox(void);
void make_helpbar(void);
void draw_menu_bar(int);
void make_save_box(void);

/* CURSOR CONTROL */
void VGA_inverse_attrib(unsigned char far*);
void VGA_inverse_bar(int, int, int);
void move_cursor(int, int, int);
void cursor_off(void);
void cursor_on(void);
void update_cursor(int, int);

/* TEXT EDIT */
void insert_char(char, int, int);
void delete_char(int, int);
void print_list(void);
void clear_list(int, int);

/* FILE I/O */
void save_file(char*);
void load_file(char*, int, int);
void new_file(int, int);

#endif //__MENU_H__
