#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"


void init(void);
void intro(void);
void outro(void);

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { {1, 1}, {1, 1} };


/* ================= game data =================== */
RESOURCE resource = {
	0,
	0,
	0,
	0
};

vector<BUILDING> buildings;
vector<UNIT> units;
typedef struct system_message {
	
};
/* ================= main() =================== */
int main(void) {
	RECT r;
	HWND console = GetConsoleWindow();
	GetWindowRect(console, &r); //현재 창의 크기 가져오기
	MoveWindow(console, r.left, r.top, 1000, 600, TRUE); //창의 크기를 가로 1000 세로 600으로

	srand((unsigned int)time(NULL));
	int click_time_up, click_time_down, click_time_right, click_time_left, double_click = 0;
	init(); //맵 초기화
	intro();
	initialize_display();
	display(resource, buildings, units, cursor);
	while (1) {
		KEY key = get_key();
		cursor.previous.x = cursor.current.x;
		cursor.previous.y = cursor.current.y;
		switch (key) {
		case k_up:
			cursor.current.y -= 1;
			break;
		case k_down:
			cursor.current.y += 1;
			break;
		case k_right:
			cursor.current.x += 1;
			break;
		case k_left:
			cursor.current.x -= 1;
			break;
		case k_quit:
			outro();
		case k_none:
		case k_undef:
		default:
			break;
		}
		if (!(1 <= cursor.current.y && cursor.current.y <= MAP_HEIGHT - 2 && 1 <= cursor.current.x && cursor.current.x <= MAP_WIDTH - 2)) {
			cursor.current.x = cursor.previous.x;
			cursor.current.y = cursor.previous.y;
		}

		// 화면 출력
		display(resource, buildings, units, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

void init() {
	//좌표 범위 좌상단 (1, 1) ~ 우하단 (MAP_WIDTH - 1, MAP_HEIGHT - 1) = (59, 17)
	buildings.push_back({
		{MAP_WIDTH - 3, 1},2,'B',(Color::Red << 4) + Color::White, 0,50,1
		});
	buildings.push_back({
		{1, MAP_HEIGHT - 3}, 2, 'B', (Color::Blue << 4) + Color::White, 0, 50, 0
		});
	buildings.push_back({
		{MAP_WIDTH - 5, 1}, 2, 'P', (Color::DarkGray << 4) + Color::White, 1, -1, 1
		});
	buildings.push_back({
		{3, MAP_HEIGHT - 3}, 2, 'P', (Color::DarkGray << 4) + Color::White, 1, -1, 0
		});
	buildings.push_back({
		{7, 10}, 1, 'R', (Color::Gray << 4) + Color::Black, -1, -1, 2
		});
	buildings.push_back({
		{19, 4}, 2, 'R', (Color::Gray << 4) + Color::Black, -1, -1, 2
		});
	buildings.push_back({
		{30, 12}, 2, 'R', (Color::Gray << 4) + Color::Black, -1, -1, 2
		});
	buildings.push_back({
		{38, 5}, 1, 'R', (Color::Gray << 4) + Color::Black, -1, -1, 2
		});
	buildings.push_back({
		{45, 13}, 1, 'R', (Color::Gray << 4) + Color::Black, -1, -1, 2
		});
	buildings.push_back({
		{1,MAP_HEIGHT - 6},1,'5',(Color::DarkYellow << 4) + Color::White, 0 , 50 ,1
		});
	buildings.push_back({
		{MAP_WIDTH-2,5},1,'5',(Color::DarkYellow << 4) + Color::White, 0 , 50 ,1
		});
	units.push_back({
		{5, 3}, {5, 3}, 'W', (Color::Yellow << 4) + Color::Black, 0, 0, 0, 0, 0, 0, 0, 0
		});
	units.push_back({
		{47, 13}, {47, 13}, 'W', (Color::Yellow << 4) + Color::Black, 0, 0, 0, 0, 0, 0, 0, 0
		});
	units.push_back({
		{MAP_WIDTH - 2,3},{MAP_WIDTH-2,3}, 'H' , (Color::Red << 4) + Color::White, 0,50,1
		});
	units.push_back({
		{1,MAP_HEIGHT-4},{1,MAP_HEIGHT-4},'H',(Color::Blue << 4) + Color::White, 0,50,1
		});
}

/* ================= subfunctions =================== */
void intro(void) {
	system("color 0f");
	printf("D");
	Sleep(200);
	printf("U");
	Sleep(200);
	printf("N");
	Sleep(200);
	printf("E");
	Sleep(200);
	printf(" 1");
	Sleep(200);
	printf(".");
	Sleep(200);
	printf("5");
	Sleep(1000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}
