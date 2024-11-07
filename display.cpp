/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"


// 출력할 내용들의 좌상단(topleft) 좌표s
const POSITION RESOURCE_POS = { 0, 0 };
const POSITION MAP_POS = { 0, 1 };
const size_t STATUS_WIDTH = 53;
const size_t STATUS_HEIGHT = 18;;
const POSITION STATUS_POS = { 62, 1 };
const size_t SYSMSG_WIDTH = 60;
const size_t SYSMSG_HEIGHT = 10;
const POSITION SYSMSG_POS = { 0, 20 };
const size_t COMMAND_WIDTH = 53;
const size_t COMMAND_HEIGHT = 10;
const POSITION COMMAND_POS = { 62, 20 };

#define CONSOLE_WIDTH 120
#define CONSOLE_HEIGHT 40


PIXEL back_buffer[CONSOLE_HEIGHT][CONSOLE_WIDTH] = { 0 };
PIXEL front_buffer[CONSOLE_HEIGHT][CONSOLE_WIDTH] = { 0 };


void console_goto(int x, int y) {
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

void console_setcolor(unsigned short color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
};

void draw_border(POSITION pos, int width, int height) {
	for (int i = 0; i < width; i++) {
		back_buffer[pos.y][pos.x + i] = { '-', Color::White };
	}
	for (int j = 1; j < height - 1; j++) {
		back_buffer[pos.y + j][pos.x] = { '|', Color::White };
		for (int i = 1; i < width - 1; i++) {
			back_buffer[pos.y + j][pos.x + i] = { ' ', Color::White };
		}
		back_buffer[pos.y + j][pos.x + width - 1] = { '|', Color::White };
	}
	for (int i = 0; i < width; i++) {
		back_buffer[pos.y + height - 1][pos.x + i] = { '-', Color::White };
	}
};
void refresh() {
	for (int i = 0; i < CONSOLE_HEIGHT; i++) {
		for (int j = 0; j < CONSOLE_WIDTH; j++) {
			if (back_buffer[i][j].Chr >> 7) {
				if ((front_buffer[i][j].Chr != back_buffer[i][j].Chr) || (front_buffer[i][j].Color != back_buffer[i][j].Color) || (front_buffer[i][j + 1].Chr != back_buffer[i][j + 1].Chr)) {
					console_goto(j, i);
					console_setcolor(back_buffer[i][j].Color);
					printf("%c%c", back_buffer[i][j].Chr, back_buffer[i][j + 1].Chr);
					front_buffer[i][j].Chr = back_buffer[i][j].Chr;
					front_buffer[i][j].Color = back_buffer[i][j].Color;
					front_buffer[i][j + 1].Chr = back_buffer[i][j + 1].Chr;
					front_buffer[i][j + 1].Color = back_buffer[i][j + 1].Color;

				}
				j++;
			}
			else {
				if ((front_buffer[i][j].Chr != back_buffer[i][j].Chr) || (front_buffer[i][j].Color != back_buffer[i][j].Color)) {
					console_goto(j, i);
					console_setcolor(back_buffer[i][j].Color);
					printf("%c", back_buffer[i][j].Chr);
					front_buffer[i][j].Chr = back_buffer[i][j].Chr;
					front_buffer[i][j].Color = back_buffer[i][j].Color;
				}
			}
		}
	}
};

void initialize_display() {
	system("color 0f");
	system("cls");
	for (int i = 0; i < CONSOLE_HEIGHT; i++) {
		for (int j = 0; j < CONSOLE_WIDTH; j++) {
			if (j == CONSOLE_WIDTH - 1) {
				back_buffer[i][j].Chr = '\n';
				front_buffer[i][j].Chr = '\n';
			}
			else {
				back_buffer[i][j].Chr = ' ';
				front_buffer[i][j].Chr = ' ';
			}
			back_buffer[i][j].Color = Color::White;
			front_buffer[i][j].Color = Color::White;
		}
	}
}
bool initialized = false;


void draw_resource(RESOURCE resource) {
	char buf[CONSOLE_WIDTH] = { 0 };
	snprintf(buf, CONSOLE_WIDTH, "spice = %d/%d, population=%d/%d",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
	for (int i = 0; i < CONSOLE_WIDTH; i++) {
		back_buffer[RESOURCE_POS.y][RESOURCE_POS.x + i] = { buf[i], Color::White };
	}
}
void display(
	RESOURCE resource,
	vector<BUILDING>& layer0,
	vector<UNIT>& layer1,
	CURSOR cursor)
{
	draw_resource(resource);
	draw_border(MAP_POS, MAP_WIDTH, MAP_HEIGHT);
	draw_border(STATUS_POS, STATUS_WIDTH, STATUS_HEIGHT);
	draw_border(SYSMSG_POS, SYSMSG_WIDTH, SYSMSG_HEIGHT);
	draw_border(COMMAND_POS, COMMAND_WIDTH, COMMAND_HEIGHT);
	draw_map(layer0, layer1);
	draw_system_message();
	draw_status_message();
	draw_console_message();
	refresh();
	draw_cursor(cursor);
	//display_system_message()
	//display_object_info()
	// display_commands()
	// ...
}
vector<string> system_messages;
void draw_system_message() {
	int system_messages_len = system_messages.size();
	for (int i = 0; i < system_messages.size(); i++) {
		for (int j = 0; j < system_messages[system_messages_len - i - 1].length() && j < SYSMSG_WIDTH - 2; j++) {
			unsigned short color;
			if (i == 0) {
				color = Color::White;
			}
			else {
				color = Color::DarkGray;
			}
			back_buffer[SYSMSG_POS.y + SYSMSG_HEIGHT - i - 2][SYSMSG_POS.x + j + 1] = { system_messages[system_messages_len - i - 1][j], color };
		}
	}
}

void log_system_message(string message) {
	system_messages.push_back(message);
	if (system_messages.size() > SYSMSG_HEIGHT - 2) {
		system_messages.erase(system_messages.begin());
	}
}
void draw_map(vector<BUILDING>& layer0, vector<UNIT>& layer1) {
	for (int i = 0; i < layer0.size(); i++) {
		BUILDING building = layer0[i];
		for (int x = 0; x < building.size; x++)
			for (int y = 0; y < building.size; y++) {
				back_buffer[MAP_POS.y + building.pos.y + y][MAP_POS.x + building.pos.x + x] = { building.repr, building.color };
			}
	}
	for (int i = 0; i < layer1.size(); i++) {
		UNIT unit = layer1[i];
		back_buffer[MAP_POS.y + unit.pos.y][MAP_POS.x + unit.pos.x] = { unit.repr, unit.color };
	}
}

void draw_cursor(CURSOR cursor) {
	int prev_x = MAP_POS.x + cursor.previous.x;
	int prev_y = MAP_POS.y + cursor.previous.y;
	int curr_x = MAP_POS.x + cursor.current.x;
	int curr_y = MAP_POS.y + cursor.current.y;
	console_goto(prev_x, prev_y);
	console_setcolor(front_buffer[prev_y][prev_x].Color);
	printf("%c", front_buffer[prev_y][prev_x].Chr);
	console_goto(curr_x, curr_y);
	console_setcolor(Color::Gray << 4);
	printf("%c", front_buffer[curr_y][curr_x].Chr);
	console_goto(curr_x, curr_y);
}
string status_messaage;
void draw_status_message() {                //상태창 메시지 출력
	for (int i = 0; i < STATUS_HEIGHT - 2; i++) {
		for (int j = 0; j < STATUS_WIDTH - 2; j++) {
			back_buffer[STATUS_POS.y + i + 1][STATUS_POS.x + j + 1].Chr = ' ';
		}
	}
	int l = 1, k = 1;
	for (int j = 0; j < status_messaage.length(); j++ & k++) {
		if (status_messaage[j] >> 7) {
			if (k > STATUS_WIDTH - 3) {
				k = 1;
				l++;
			}
			back_buffer[STATUS_POS.y + l][STATUS_POS.x + k].Chr = status_messaage[j];
			back_buffer[STATUS_POS.y + l][STATUS_POS.x + k + 1].Chr = status_messaage[j + 1];
			k++;
			j++;
		}
		else {
			if (status_messaage[j] == '\n') {
				k = 0;
				l++;
				continue;
			}
			if (k > STATUS_WIDTH - 2) {
				k = 1;
				l++;
			}
			back_buffer[STATUS_POS.y + l][STATUS_POS.x + k].Chr = status_messaage[j];
		}
	}
}
void log_status_message(string message) {
	status_messaage = message;
}
string console_messaage;
void draw_console_message() {                //상태창 메시지 출력
	for (int i = 0; i < COMMAND_HEIGHT - 2; i++) {
		for (int j = 0; j < COMMAND_WIDTH - 2; j++) {
			back_buffer[COMMAND_POS.y + i + 1][COMMAND_POS.x + j + 1].Chr = ' ';
		}
	}
	int l = 1, k = 1;
	for (int j = 0; j < console_messaage.length(); j++ & k++) {
		if (console_messaage[j] >> 7) {
			if (k > COMMAND_WIDTH - 3) {
				k = 1;
				l++;
			}
			back_buffer[COMMAND_POS.y + l][COMMAND_POS.x + k].Chr = console_messaage[j];
			back_buffer[COMMAND_POS.y + l][COMMAND_POS.x + k + 1].Chr = console_messaage[j + 1];
			k++;
			j++;
		}
		else {
			if (console_messaage[j] == '\n') {
				k = 0;
				l++;
				continue;
			}
			if (k > COMMAND_WIDTH - 2) {
				k = 1;
				l++;
			}
			back_buffer[COMMAND_POS.y + l][COMMAND_POS.x + k].Chr = console_messaage[j];
		}
	}
}
void log_console_message(string message) {
	console_messaage = message;
}