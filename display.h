/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
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

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
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

