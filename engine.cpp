#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"
#include<algorithm>

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

int find_closest_building(int x, int y, bool perfect_match) {
	if (perfect_match) {
		for (int i = 0; i < buildings.size(); i++) {
			POSITION pos = buildings[i].pos;
			int size = buildings[i].size;
			if ((pos.x <= x) && (x < pos.x + size) && (pos.y <= y) && (y < pos.y + size)) {
				return i;
			}
		}
		return -1;
	}
	else {
		vector<double> distance_square(buildings.size());
		for (int i = 0; i < buildings.size(); i++) {
			POSITION pos = buildings[i].pos;
			int size = buildings[i].size;
			distance_square[i] = pow(pos.x + ((size - 1) / 2.0) - x, 2) + pow(pos.y + ((size - 1) / 2.0) - y, 2);
		}
		double minimum_distance = 1e9;
		int minimum_index = -1;
		for (int i = 0; i < buildings.size(); i++) {
			if (distance_square[i] < minimum_distance) {
				minimum_distance = distance_square[i];
				minimum_index = i;
			}
		}
		return minimum_index;
	}
}
int find_closest_unit(int x, int y, bool perfect_match) {
	if (perfect_match) {
		for (int i = 0; i < units.size(); i++) {
			POSITION pos = units[i].pos;
			if ((pos.x == x) && (pos.y == y)) {
				return i;
			}
		}
		return -1;
	}
	else {
		vector<int> distance_square(units.size());
		for (int i = 0; i < units.size(); i++) {
			POSITION pos = units[i].pos;
			distance_square[i] = (pos.x - x) * (pos.x - x) + (pos.y - y) * (pos.y - y);
		}
		int minimum_distance = INT_MAX;
		int minimum_index = -1;
		for (int i = 0; i < units.size(); i++) {
			if (distance_square[i] < minimum_distance) {
				minimum_distance = distance_square[i];
				minimum_index = i;
			}
		}
		return minimum_index;
	}
}

/* ================= main() =================== */
int main(void) {
	RECT r;
	HWND console = GetConsoleWindow();
	GetWindowRect(console, &r); //���� â�� ũ�� ��������
	MoveWindow(console, r.left, r.top, 1000, 600, TRUE); //â�� ũ�⸦ ���� 1000 ���� 600����

	srand((unsigned int)time(NULL));
	unsigned long click_time_up = 0, click_time_down = 0, click_time_left = 0, click_time_right = 0;
	init(); //�� �ʱ�ȭ
	intro();
	initialize_display();
	display(resource, buildings, units, cursor);

	while (1) {
		KEY key = get_key();
		cursor.previous.x = cursor.current.x;
		cursor.previous.y = cursor.current.y;
		int building_index, unit_index;
		switch (key) {
		case k_up:
			cursor.current.y -= 1;
			log_system_message(to_string(clock() - click_time_up));
			if ((click_time_up > 0) && (clock() - click_time_up < 200)) { //k_up�� 0.2�ʾȿ� �ѹ� �� ������
				cursor.current.y -= 4;
				click_time_up = 0;
			}
			else {
				click_time_up = clock();
			}
			break;
		case k_down:
			cursor.current.y += 1;
			log_system_message(to_string(clock() - click_time_down));
			if ((click_time_down > 0) && (clock() - click_time_down < 200)) {
				cursor.current.y += 4;
				click_time_down = 0;
			}
			else {
				click_time_down = clock();
			}
			break;
		case k_right:
			cursor.current.x += 1;
			log_system_message(to_string(clock() - click_time_right));
			if ((click_time_right > 0) && (clock() - click_time_right < 200)) {
				cursor.current.x += 4;
				click_time_right = 0;
			}
			else {
				click_time_right = clock();
			}
			break;
		case k_left:
			cursor.current.x -= 1;
			log_system_message(to_string(clock() - click_time_left));
			if ((click_time_left > 0) && (clock() - click_time_left < 200)) {
				cursor.current.x -= 4;
				click_time_left = 0;
			}
			else {
				click_time_left = clock();
			}
			break;
		case k_space:
			building_index = find_closest_building(cursor.current.x, cursor.current.y, true);
			unit_index = find_closest_unit(cursor.current.x, cursor.current.y, true);
			if (building_index >= 0) {
				BUILDING current_building = buildings[building_index];
				log_status_message(current_building.status_msg);
			}
			else if (unit_index >= 0) {
				UNIT current_unit = units[unit_index];
				log_status_message(current_unit.status_msg);
			}
			else {
				log_status_message("�����Ը��� �ɵ��� �ִ� �縷�̴�.");
			}
			break;
		case k_esc:
			log_status_message("");
			break;
		case k_quit:
			outro();
		case k_none:
		case k_undef:

		default:
			break;
		}
		if (!(1 <= cursor.current.y && cursor.current.y <= MAP_HEIGHT - 2 && 1 <= cursor.current.x && cursor.current.x <= MAP_WIDTH - 2)) {
			cursor.current.x = max(min(cursor.current.x, MAP_WIDTH - 2), 1);
			cursor.current.y = max(min(cursor.current.y, MAP_HEIGHT - 2), 1);
		}

		// ȭ�� ���
		display(resource, buildings, units, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}
BUILDING make_Base(int x, int y, bool is_enemy) {
	BUILDING Building;
	Building.pos = { x, y };
	Building.size = 2;
	if (is_enemy) {
		Building.color = (Color::Red << 4) + Color::White;
		Building.enemy = 1;
		Building.status_msg = "���ڳ��� ���̽��̴�.";
	}
	else {
		Building.color = (Color::Blue << 4) + Color::White;
		Building.enemy = 0;
		Building.status_msg = "��Ʈ���̵��� ���̽��̴�.";
	}
	Building.repr = 'B';
	Building.cost = 0;
	Building.health = 50;
	return Building;
}

BUILDING make_Plate(int x, int y, int size) {
	BUILDING Building;
	Building.pos = { x, y };
	Building.size = size;
	Building.color = (Color::DarkGray << 4) + Color::White;
	Building.enemy = 2;
	Building.repr = 'P';
	Building.cost = 1;
	Building.health = -1;
	Building.status_msg = "�ǹ��� ���� �� �ִ� �÷���Ʈ�̴�.";
	return Building;
}

BUILDING make_Rock(int x, int y, int size) {
	BUILDING Building;
	Building.pos = { x, y };
	Building.size = size;
	Building.color = (Color::Gray << 4) + Color::Black;
	Building.enemy = 2;
	Building.repr = 'R';
	Building.cost = -1;
	Building.health = -1;
	Building.status_msg = "�׳� ���ε� �ϴ�.";
	return Building;
}
BUILDING make_Spice(int x, int y) {
	BUILDING Building;
	Building.pos = { x , y };
	Building.size = 1;
	Building.color = (Color::Yellow << 4) + Color::Black;
	Building.enemy = 2;
	Building.repr = '5';
	Building.status_msg = "���ֿ��� ���� ���� ������ �����̽��̴�.\n��Ȳ���� ���簰�� ���¿� �ణ Ǫ������ ������ ������.";
	return Building;
}
UNIT make_Sandworm(int x, int y) {
	UNIT Unit;
	Unit.pos = { x, y };
	Unit.dest = { x, y };
	Unit.color = (Color::DarkYellow << 4) + Color::Black;
	Unit.cost = -1;
	Unit.repr = 'W';
	Unit.population = -1;
	Unit.health = -1;
	Unit.fov = -1;
	Unit.speed = 2500;
	Unit.attack_speed = 10000;
	Unit.damage = INT_MAX;
	Unit.status_msg = "�𷡼ӿ� ��� ������̴�.\n������ ���� ����� �Լ����� ������\n��� ���� ���ƹ��� �� �ִ�.";
	return Unit;
}
UNIT make_Haverster(int x, int y, bool is_enemy) {
	UNIT Unit;
	Unit.pos = { x, y };
	Unit.dest = { x,y };
	if (is_enemy) {
		Unit.color = (Color::Red << 4) + Color::White;
		Unit.enemy = 1;
		Unit.status_msg = "���ڳ��� �Ϻ������̴�.";
	}
	else {
		Unit.color = (Color::Blue << 4) + Color::White;
		Unit.enemy = 0;
		Unit.status_msg = "��Ʈ���̵��� �Ϻ������̴�.";
	}
	Unit.repr = 'H';
	Unit.cost = 5;
	Unit.health = 70;
	Unit.speed = 2000;
	Unit.fov = 0;
	return Unit;
}
void init() {
	buildings.push_back(make_Base(MAP_WIDTH - 3, 1, true));
	buildings.push_back(make_Base(1, MAP_HEIGHT - 3, false));
	buildings.push_back(make_Plate(MAP_WIDTH - 5, 1, 2));
	buildings.push_back(make_Plate(3, MAP_HEIGHT - 3, 2));
	buildings.push_back(make_Rock(7, 10, 1));
	buildings.push_back(make_Rock(19, 4, 2));
	buildings.push_back(make_Rock(30, 12, 2));
	buildings.push_back(make_Rock(38, 5, 1));
	buildings.push_back(make_Rock(45, 13, 1));
	buildings.push_back(make_Spice(MAP_WIDTH - 2, 5));
	buildings.push_back(make_Spice(1, MAP_HEIGHT - 6));
	units.push_back(make_Sandworm(5, 3));
	units.push_back(make_Sandworm(47, 13));
	units.push_back(make_Haverster(1, MAP_HEIGHT - 4, false));
	units.push_back(make_Haverster(MAP_WIDTH - 2, 3, true));
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
