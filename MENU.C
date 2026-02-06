#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <alloc.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "MENU.H"

TEXT* head = NULL;
TEXT* current = NULL;
int cursor_x = 0;

/* ================================================= */
/* 빨리끝내자                                        */
/* ================================================= */

void init_editor(void)
{
    clear_monitor();
    init_monitor();
    make_txtbox();
    make_helpbar();
    
    move_cursor(0, TEXT_START_COL, TEXT_ROW);
    cursor_on();
}

void run_editor(void)
{
    int key;
    int in_menu = 0; /* 0: 커서 -> 텍스트창 | 1: 커서 -> 메뉴창 */
    int menu_idx = 0; /* 0 1 2 3 4 */
	int save_input;

    while (1) 
    {
        key = getch();

		/* 커서 -> 메뉴창 */
        if (key == 0) 
		{
            key = getch(); 
            
            if (in_menu) 
			{
                switch (key) 
				{
                    case 75: /* <- */
                        menu_idx--;
                        if (menu_idx < 0) 
						{
							menu_idx = 4;
						}
                        draw_menu_bar(menu_idx);
                        break;
						
                    case 77: /* -> */
                        menu_idx++;
                        if (menu_idx > 4)
						{
							menu_idx = 0;
						} 
                        draw_menu_bar(menu_idx);
                        break;
                }
            }
            /* 커서 -> 텍스트창 */
            else 
			{
                switch (key) 
				{
                    case 75: /* <- */
                        if (current != NULL) 
						{
                            current = current->prev;
                            cursor_x--;
                            if (cursor_x < 0) 
							{ 
                                 cursor_x = -1; 
                                 current = NULL; 
                            }
                        }
                        update_cursor(TEXT_START_COL, TEXT_ROW);
                        break;
						
                    case 77: /* -> */
                        if (current == NULL) 
						{
                            if (head != NULL) 
							{
                                current = head;
                                cursor_x = 0;
                            }
                        }
						else if (current->next != NULL) 
						{
                            current = current->next;
                            cursor_x++;
                        }
                        update_cursor(TEXT_START_COL, TEXT_ROW);
                        break;
						
                    case 68: /* F10 */
                        in_menu = 1;
                        menu_idx = 0; /* New */
                        draw_menu_bar(menu_idx); /* 메뉴 하이라이트 */
                        cursor_off(); /* 커서 숨김 */
                        break;
                    case 45: /* Alt + X */
                        return;
                }
            }
        } 
        else 
		{
            /* 커서 -> 메뉴창 */
            if (in_menu) {
				/* ESC */
                if (key == 27) 
				{
                    in_menu = 0;
                    draw_menu_bar(-1); /* 하이라이트 제거 */
                    update_cursor(TEXT_START_COL, TEXT_ROW);
                    cursor_on();
                }
				/* Enter */
                else if (key == 13) 
				{
                    in_menu = 0;
                    draw_menu_bar(-1); /* 하이라이트 제거 */
                    cursor_on();
                    
                    switch (menu_idx) 
					{
                        case MENU_NEW:
                            new_file(TEXT_START_COL, TEXT_ROW);
                            break;
                        case MENU_LOAD:
                            load_file("TEST.TXT", TEXT_START_COL, TEXT_ROW);
                            break;
                        case MENU_SAVE:
                            save_file("TEST.TXT");
                            break;
                        case MENU_SAVEAS:
							make_save_box();
							update_cursor(SAVE_TXT_COL + 3, SAVE_TXT_ROW + 1);
							cursor_on();
							save_input = getch();
							while(save_input != 13)
							{
								if (save_input == 13)
								{
									break;
								}
							};
                            save_file("TEST.TXT");
                            break;
                        case MENU_EXIT:
                            return;
                    }
                    update_cursor(TEXT_START_COL, TEXT_ROW);
                }
            }
            /* 커서 -> 텍스트창 */
            else 
			{
				/* Backspace */
                if (key == 8)
				{
                    delete_char(TEXT_START_COL, TEXT_ROW);
                }
				/* ESC */
                else if (key == 27) 
				{

                }
				/* 텍스트 입력 */
                else if (key >= 32 && key <= 126) 
				{
                    insert_char((char)key, TEXT_START_COL, TEXT_ROW);
                }
            }
        }
    }
}

/* ================================================= */
/* UI              						 			 */
/* ================================================= */

void draw_menu_bar(int selected_idx)
{
    write_str(1, 5, "New");
    write_str(1, 15, "Load");
    write_str(1, 25, "Save");
    write_str(1, 35, "Save as");
    write_str(1, 45, "Exit");

    /* VGA_inverse_bar(row, col, length) */
    /* cal_location(x, y) -> x=Row, y=Col */
    
    if (selected_idx == MENU_NEW)
	{
		VGA_inverse_bar(1, 5, 3);
	}
    if (selected_idx == MENU_LOAD)
	{
		VGA_inverse_bar(1, 15, 4);
	}
	
    if (selected_idx == MENU_SAVE)
	{
		VGA_inverse_bar(1, 25, 4);
	}
    if (selected_idx == MENU_SAVEAS)
	{
		VGA_inverse_bar(1, 35, 7);
	}
    if (selected_idx == MENU_EXIT)
	{
		VGA_inverse_bar(1, 45, 4);
	}
}

void make_save_box(void)
{
	char far* sb_loc;
	char far* txt_loc;
    int i;
	int j;
    int k;
    int top_row = 8;
    int bot_row = 13;
    int left_col = 5;
    int right_col = 35;
	
	/* 그리는 부분 */
	for (i = left_col; i <= right_col; i++) 
	{
        sb_loc = cal_location(top_row, i);
        if (i == left_col)
		{
			*sb_loc = 218;
		}
		else if (i == right_col)
		{
			*sb_loc = 191;
		}
		else
		{
			*sb_loc = 196;
		} 
        *(sb_loc+1) = 0x07;
    }
	
    for (i = left_col; i <= right_col; i++) 
	{
        sb_loc = cal_location(bot_row, i);
        if (i == left_col)
		{
			*sb_loc = 192;
		}
		else if (i == right_col)
		{
			*sb_loc = 217;
		}
		else
		{
			*sb_loc = 196;
		} 
        *(sb_loc+1) = 0x07;
    }
	
	for (j = 9; j < 13; j++) 
	{
        sb_loc = cal_location(j, 5);
        *sb_loc = 179;
		*(sb_loc+1) = 0x07;
		
        sb_loc = cal_location(j, 35);
        *sb_loc = 179;
		*(sb_loc+1) = 0x07;
    }

    sb_loc = cal_location(top_row + 1, left_col); 
    *sb_loc = 179;
	*(sb_loc+1) = 0x07;
    
    sb_loc = cal_location(top_row + 1, right_col); 
    *sb_loc = 179;
	*(sb_loc+1) = 0x07;
	
	write_str(top_row + 1, 15, "File Name");
	
	/* 입력창 부분 */
	
    /* 위쪽 */
    for (k = 8; k <= 30; k++) 
	{
        txt_loc = cal_location(10, k);
        if (k == 8)
		{
			*txt_loc = 218;
		}
		else if (k == 30)
		{
			*txt_loc = 191;
		}
		else
		{
			*txt_loc = 196;
		} 
        *(txt_loc+1) = 0x07;
    }
    /* 아래쪽 */
    for (k = 8; k <= 30; k++) 
	{
        txt_loc = cal_location(12, k);
        if (k == 8)
		{
			*txt_loc = 192;
		}
		else if (k == 30)
		{
			*txt_loc = 217;
		}
		else
		{
			*txt_loc = 196;
		} 
        *(txt_loc+1) = 0x07;
    }
    /* 기둥 */
    txt_loc = cal_location(11, 8);
	*txt_loc = 179;
	*(txt_loc+1) = 0x07;
	
    txt_loc = cal_location(11, 30);
	*txt_loc = 179;
	*(txt_loc+1) = 0x07;
	
}

void make_helpbar(void)
{
    char far* loc;
    int i;
    int top_row = 21;
    int bot_row = 23;
    int left_col = 5;
    int right_col = 65;

    /* 위쪽 선 */
    for (i = left_col; i <= right_col; i++) 
	{
        loc = cal_location(top_row, i);
        if (i == left_col)
		{
			*loc = 218;
		}
		else if (i == right_col)
		{
			*loc = 191;
		}
		else
		{
			*loc = 196;
		} 
        *(loc+1) = 0x07;
    }
    /* 아래쪽 선 */
    for (i = left_col; i <= right_col; i++) 
	{
        loc = cal_location(bot_row, i);
        if (i == left_col)
		{
			*loc = 192;
		}
		else if (i == right_col)
		{
			*loc = 217;
		}
		else
		{
			*loc = 196;
		} 
        *(loc+1) = 0x07;
    }
    /* 기둥 */
    loc = cal_location(top_row + 1, left_col); 
    *loc = 179;
	*(loc+1) = 0x07;
    
    loc = cal_location(top_row + 1, right_col); 
    *loc = 179;
	*(loc+1) = 0x07;

    /* PRINT TEXT */
    write_str(top_row + 1, 10, "F10 - Menu       Alt + x - Exit       F1 - Help");
}

void make_txtbox(void)
{
    char far* txt_loc;
    int i;
    
    /* 위쪽 (Row 3, Col 5~65) */
    for (i = 5; i <= 65; i++) 
	{
        txt_loc = cal_location(3, i);
        if (i == 5)
		{
			*txt_loc = 218;
		}
		else if (i == 65)
		{
			*txt_loc = 191;
		}
		else
		{
			*txt_loc = 196;
		} 
        *(txt_loc+1) = 0x07;
    }
    /* 아래쪽 (Row 5, Col 5~65) */
    for (i = 5; i <= 65; i++) 
	{
        txt_loc = cal_location(5, i);
        if (i == 5)
		{
			*txt_loc = 192;
		}
		else if (i == 65)
		{
			*txt_loc = 217;
		}
		else
		{
			*txt_loc = 196;
		} 
        *(txt_loc+1) = 0x07;
    }
    /* 기둥 (Row 4) */
    txt_loc = cal_location(4, 5);
	*txt_loc = 179;
	*(txt_loc+1) = 0x07;
	
    txt_loc = cal_location(4, 65);
	*txt_loc = 179;
	*(txt_loc+1) = 0x07;
}

void clear_monitor(void)
{
    char far* location = (char far*)LOCATION;
    int i;
    for (i = 0; i < 80 * 25; i++) 
	{
        *location = ' ';
		location++;
        *location = 0x07;
		location++;
    }
}

char far* cal_location(int x, int y)
{
    char far* location = (char far*)LOCATION;
    location = location + (x * 160) + (y * 2);
    return location;
}

void write_str(int x, int y, char* str)
{
    char far* target_loc = cal_location(x, y);
    while (*str != '\0') 
	{
        *target_loc = *str;     
		target_loc++;
        *target_loc = 0x07;
		target_loc++;
        str++;
    }
}

void init_monitor(void)
{
    char far* location;
    char attr = 7;
    int i, j, k;

    /* 상단 */
    location = cal_location(0, 0);
    for (i = 0; i < 80; i++)
	{
        if(i == 0)
		{
			*location = 218;
		}
		else if (i == 79)
		{
			*location = 191;
		}
		else
		{
			*location = 196;
		} 
        *(location+1) = attr;
        location += 2;
    }
    /* 좌우 기둥 */
    for (j = 1; j < 24; j++) 
	{
        location = cal_location(j, 0);
        *location = 179;
		*(location+1) = attr;
		
        location = cal_location(j, 79);
        *location = 179;
		*(location+1) = attr;
    }
    /* 하단 */
    location = cal_location(24, 0);
    for (k = 0; k < 80; k++) 
	{
        if (k == 0)
		{
			*location = 192;
		}
		else if (k == 79)
		{
			*location = 217;
		}
		else
		{
			*location = 196;
		}
        *(location+1) = attr;
        location += 2;
    }
    
    /* MENU TEXT INIT */
    draw_menu_bar(-1); 
}

void VGA_inverse_attrib(unsigned char far* attrib)
{
    unsigned char origin_attrib;
    origin_attrib = *attrib;
    *attrib >>= 4;
    *attrib = *attrib & 0x0f;
    origin_attrib <<= 4;
    *attrib = *attrib | origin_attrib;
}

void VGA_inverse_bar(int x, int y, int length)
{
    int i = 0;
    unsigned char far *attr_memory = (unsigned char far *) 0xb8000001L;
    attr_memory = attr_memory + (x * 160) + (y * 2);
    for (i = 0; i < length; i++) 
	{
        VGA_inverse_attrib(attr_memory);
        attr_memory += 2;
    }
}

void move_cursor (int page, int x, int y)
{ 
    union REGS regs;
    regs.h.ah = 2;
    regs.h.dh = y;
    regs.h.dl = x;
    regs.h.bh = page;
    int86(0x10, &regs, &regs);
} 

void cursor_off(void)
{
    union REGS regs;
    regs.h.ah = 1;
	regs.h.ch = 0x20;
	regs.h.cl = 0;
    int86(0x10, &regs, &regs);
} 

void cursor_on(void)
{
    union REGS regs;
    regs.h.ah = 1; 
	regs.h.ch = 0x0B; 
	regs.h.cl = 0x0C;
    int86(0x10, &regs, &regs);
}

/* ================================================= */
/* I/O                                 				 */
/* ================================================= */

void print_list(void)
{
    char far* vmem;
    TEXT* p = head;
    int i;
    
    /* CLEAR */
    for (i=0; i<59; i++) 
	{ 
        vmem = cal_location(TEXT_ROW, TEXT_START_COL + i);
        *vmem = ' ';
		*(vmem+1) = 0x07;
    }

    /* PRINT LIST */
    i = 0;
    while (p != NULL && i < 59)
	{
        vmem = cal_location(TEXT_ROW, TEXT_START_COL + i);
        *vmem = p->character;
        *(vmem+1) = 0x07;
        p = p->next;
        i++;
    }
}

void update_cursor(int col, int row)
{
    int x_pos = col + cursor_x;
    if (current == NULL)
	{
		x_pos = col - 1; 
	}
    move_cursor(0, x_pos + 1, row); 
}

/*
void get_filename(char* buf)
{
    int i = 0;
    int key;
    
    int input_row = 11; 
    int input_col = 9;

    move_cursor(0, input_col, input_row);
    cursor_on();

    while (1)
    {
        key = getch();

        if (key == 13)
        {
            buf[i] = '\0';
            break;
        }
        else if (key == 27)
        {
            buf[0] = '\0';
            break;
        }
        else if (key == 8)
        {
            if (i > 0)
            {
                i--;
                char space_str[] = " ";
                write_str(input_col + i, input_row, space_str);
                move_cursor(0, input_col + i, input_row);
            }
        }
        else if (key >= 32 && key <= 126)
        {
            if (i < 20)
            {
                buf[i] = (char)key;
                char str[2];
                str[0] = (char)key;
                str[1] = '\0';
                write_str(input_col + i, input_row, str);
                
                i++;
                move_cursor(0, input_col + i, input_row);
            }
        }
    }
    cursor_off();
}
*/

void insert_char(char ch, int col, int row)
{
	int count = 0;
	TEXT* temp = head;
    TEXT* new_node;
    new_node = (TEXT*)malloc(sizeof(TEXT));
    
	if (new_node == NULL)
	{
		return;
	}
	
	while (temp != NULL) 
	{
        count++;
        temp = temp->next;
    }
	if (count >= 58) 
	{
        return; 
    }
    
    new_node->character = ch;
    new_node->next = NULL;
    new_node->prev = NULL;

    if (head == NULL) 
	{ 
        head = new_node;
        current = new_node;
        cursor_x = 0;
    }
	else if (current == NULL) 
	{
        new_node->next = head;
        head->prev = new_node;
        head = new_node;
        current = new_node;
        cursor_x = 0;
    }
	else 
	{
        new_node->next = current->next;
        new_node->prev = current;
        if (current->next != NULL)
		{
			current->next->prev = new_node;
		}
        current->next = new_node;
        current = new_node; 
        cursor_x++;
    }
    print_list();
    update_cursor(col, row);
}

void delete_char(int col, int row)
{
    TEXT* del_node;
    if (head == NULL || current == NULL)
	{
		return;
	}

    del_node = current;
    if (del_node == head) 
	{
        head = head->next;
        if (head != NULL)
		{
			head->prev = NULL;
		}
        current = NULL;
        cursor_x = -1;
    }
	else 
	{
        del_node->prev->next = del_node->next;
        if (del_node->next != NULL)
		{
			del_node->next->prev = del_node->prev;
		}
        current = del_node->prev;
        cursor_x--;
    }
    free(del_node);
    print_list();
    update_cursor(col, row);
}

void clear_list(int col, int row)
{
    TEXT* temp;
    while(head != NULL) 
	{
        temp = head;
        head = head->next;
        free(temp);
    }
    head = NULL;
    current = NULL;
    cursor_x = -1;
    print_list();
    update_cursor(col, row);
}

void new_file(int col, int row)
{
    clear_list(col, row);
}

void save_file(char* fileName)
{
    int fd;
    TEXT* p = head;
    char buf[1];
    fd = open(fileName, O_CREAT | O_WRONLY | O_TEXT | O_TRUNC, S_IWRITE | S_IREAD);
    if (fd == -1)
	{
		return;
	}
    while(p != NULL) 
	{
        buf[0] = p->character;
        write(fd, buf, 1);
        p = p->next;
    }
    close(fd);
}

void load_file(char* fileName, int col, int row)
{
    int fd;
    char buf[1];
    new_file(col, row);
    fd = open(fileName, O_RDONLY | O_TEXT);
    if (fd == -1)
	{
		return;
	}
    while(read(fd, buf, 1) > 0) 
	{
        insert_char(buf[0], col, row);
    }
    close(fd);
}
