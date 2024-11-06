/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include <vector>
#include <string>
#include "common.h"

typedef struct {
	char Chr;
	unsigned short Color;
} PIXEL;

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112

void initialize_display(void);
void console_goto(int x, int y);
void console_setcolor(unsigned short color);
void draw_border(POSITION pos, int width, int height);
void refresh();
void draw_resource(RESOURCE resource);
void draw_map(vector<BUILDING>& layer0, vector<UNIT>& layer1);
void draw_cursor(CURSOR cursor);
void display(
	RESOURCE resource,
	vector<BUILDING>& layer0,
	vector<UNIT>& layer1,
	CURSOR cursor
);
void draw_system_message();
void log_system_message(string message);
void draw_status_message();
void log_status_message(string message);
#endif

