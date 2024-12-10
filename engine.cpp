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
CURSOR cursor = { {1, 1}, {1, 1}, 1, 1 };
/* ================= game data =================== */
RESOURCE resource = {
	15,
	50,
	1,
	5,
};
BUILDING make_Spice(int x, int y, int reserves);
vector<BUILDING> buildings;
vector<UNIT> units;
int find_closest_unit_except(int x, int y, bool perfect_match, char exception) {
	if (perfect_match) {
		for (int i = 0; i < units.size(); i++) {
			POSITION pos = units[i].pos;
			if ((pos.x == x) && (pos.y == y) && (units[i].repr != exception)) {
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
			if ((distance_square[i] < minimum_distance) && (units[i].repr != exception)) {
				minimum_distance = distance_square[i];
				minimum_index = i;
			}
		}
		return minimum_index;
	}
}
int find_closest_building(int x, int y, bool perfect_match) {
	if (perfect_match) {
		for (int i = buildings.size() - 1; i >= 0; i--) {
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
BUILDING make_Plate(int x, int y, int size);
BUILDING make_Dormitory(int x, int y, int size);
BUILDING make_Garage(int x, int y, int size);
BUILDING make_Barracks(int x, int y, int size);
BUILDING make_Shelter(int x, int y, int size);
UNIT make_Haverster(int x, int y, bool is_enemy);
/* ================= main() =================== */
int main(void) {
	RECT r;
	HWND console = GetConsoleWindow();
	GetWindowRect(console, &r); //���� â�� ũ�� ��������
	MoveWindow(console, r.left, r.top, 1000, 600, TRUE); //â�� ũ�⸦ ���� 1000 ���� 600����

	srand((unsigned int)time(NULL));
	unsigned long click_time = 0;
	init(); //�� �ʱ�ȭ
	intro();
	initialize_display();
	display(resource, buildings, units, cursor);

	BUILDING nothing;
	nothing.repr = 0;
	BUILDING* selected_building = &nothing;
	UNIT nothing2;
	nothing2.repr = 0;
	UNIT* selected_unit = &nothing2;
	KEY last_key = k_none;
	bool build_mode = false;
	UNIT* selected_harvester = nullptr;
	log_console_message("B: Build");
	while (1) {
		KEY key = get_key();
		if (((*selected_building).repr == 'B') && ((*selected_building).enemy == 0)) {	//�Ʊ� B�� ���� ������, ���� Ű�� h���  
			if (key == k_h) {
				if (resource.spice < 5) {
					log_system_message("Not enough spice");
				}
				else {
					bool is_havester_exist[MAP_WIDTH] = { false, };
					for (int i = 0; i < units.size(); i++) {
						if ((units[i].repr == 'H') && (units[i].enemy == 0)) {
							is_havester_exist[units[i].pos.x] = true;
						}
					}
					for (int i = 1; i < MAP_WIDTH - 1; i++) {
						if (is_havester_exist[i] == false) {
							log_system_message("A new harvester ready");
							units.push_back(make_Haverster(i, MAP_HEIGHT - 4, false));
							resource.spice -= 5;
							if (resource.population_max < 6) resource.population += 1;
							else log_system_message("���� ������ �α����� �ʰ� �Ͽ����ϴ�.");
							break;
						}
					}
				}
			}
		}
		if (((*selected_unit).repr == 'H') && ((*selected_unit).enemy == 0)) {
			if (key == k_h) {
				selected_harvester = selected_unit; //�Ϻ����Ͱ� ���õǾ����� ǥ��
				log_system_message("�������� �������ּ���.");
			}
		}
		if ((selected_harvester != nullptr) && ((*selected_building).repr > '0' && (*selected_building).repr <= '9')) { //(���õ� �Ϻ����Ͱ� �ְ�) && (���õ� ������ �����̽� ���������)
			(*selected_harvester).dest = (*selected_building).pos; //�Ϻ������� �������� �����̽� ��������
			selected_harvester = nullptr; //�Ϻ����� ���� ����
		}
		if (last_key == k_b) {
			if (key == k_p) {
				build_mode = true;
				cursor.cur_size = 2;
			}
			else if (key == k_d) {
				build_mode = true;
				cursor.cur_size = 2;
			}
			else if (key == k_g) {
				build_mode = true;
				cursor.cur_size = 2;
			}
			else if (key == k_s) {
				build_mode = true;
				cursor.cur_size = 2;
			}
			else if (key == k_b) {
				build_mode = true;
				cursor.cur_size = 2;
			}
		}
		if ((last_key == k_p) && build_mode) {
			if (key == k_space) {
				bool buildable = true;
				for (int d_x = 0; d_x < cursor.cur_size; d_x++) {
					for (int d_y = 0; d_y < cursor.cur_size; d_y++) {
						int result1 = find_closest_building(cursor.current.x + d_x, cursor.current.y + d_y, true);
						int result2 = find_closest_unit(cursor.current.x + d_x, cursor.current.y + d_y, true);
						if ((result1 != -1) || (result2 != -1)) {
							buildable = false;
						}
					}
				}
				if (buildable) {
					if (resource.spice >= 1) {
						resource.spice -= 1;
						buildings.push_back(make_Plate(cursor.current.x, cursor.current.y, 2));
						log_system_message("[������ ��Ƚ��ϴ�.]");
					}
					else {
						log_system_message("[�����̽��� �����մϴ�.]");
					}
				}
				else {
					log_system_message("[�Ǽ��� �����Ͽ����ϴ�.]");
				}
				build_mode = false;
				cursor.cur_size = 1;
			}
		}
		else if ((last_key == k_d) && build_mode) {
			if (key == k_space) {
				bool buildable = true;
				for (int d_x = 0; d_x < cursor.cur_size; d_x++) {
					for (int d_y = 0; d_y < cursor.cur_size; d_y++) {
						int result1 = find_closest_building(cursor.current.x + d_x, cursor.current.y + d_y, true);
						int result2 = find_closest_unit(cursor.current.x + d_x, cursor.current.y + d_y, true);
						if ((result2 != -1) || !((result1 != -1) && (buildings[result1].repr == 'P'))) {
							buildable = false;
						}
					}
				}
				if (buildable) {
					if (resource.spice >= 2) {
						resource.spice -= 2;
						buildings.push_back(make_Dormitory(cursor.current.x, cursor.current.y, 2));
						log_system_message("[���Ҹ� �������ϴ�.]");
						resource.population_max += 10;
					}
					else {
						log_system_message("[�����̽��� �����մϴ�.]");
					}
				}
				else {
					log_system_message("[�Ǽ��� �����Ͽ����ϴ�.]");
				}
				build_mode = false;
				cursor.cur_size = 1;
			}
		}
		else if ((last_key == k_g) && build_mode) {
			if (key == k_space) {
				bool buildable = true;
				for (int d_x = 0; d_x < cursor.cur_size; d_x++) {
					for (int d_y = 0; d_y < cursor.cur_size; d_y++) {
						int result1 = find_closest_building(cursor.current.x + d_x, cursor.current.y + d_y, true);
						int result2 = find_closest_unit(cursor.current.x + d_x, cursor.current.y + d_y, true);
						if ((result2 != -1) || !((result1 != -1) && (buildings[result1].repr == 'P'))) {
							buildable = false;
						}
					}
				}
				if (buildable) {
					if (resource.spice >= 4) {
						resource.spice -= 4;
						buildings.push_back(make_Garage(cursor.current.x, cursor.current.y, 2));
						log_system_message("[â�� �������ϴ�.]");

					}
					else {
						log_system_message("[�����̽��� �����մϴ�.]");
					}
				}
				else {
					log_system_message("[�Ǽ��� �����Ͽ����ϴ�.]");
				}
				build_mode = false;
				cursor.cur_size = 1;
			}
		}
		else if ((last_key == k_b) && build_mode) {
			if (key == k_space) {
				bool buildable = true;
				for (int d_x = 0; d_x < cursor.cur_size; d_x++) {
					for (int d_y = 0; d_y < cursor.cur_size; d_y++) {
						int result1 = find_closest_building(cursor.current.x + d_x, cursor.current.y + d_y, true);
						int result2 = find_closest_unit(cursor.current.x + d_x, cursor.current.y + d_y, true);
						if ((result2 != -1) || !((result1 != -1) && (buildings[result1].repr == 'P'))) {
							buildable = false;
						}
					}
				}
				if (buildable) {
					if (resource.spice >= 4) {
						resource.spice -= 4;
						buildings.push_back(make_Barracks(cursor.current.x, cursor.current.y, 2));
						log_system_message("[������ �Ǽ��Ͽ����ϴ�.]");

					}
					else {
						log_system_message("[�����̽��� �����մϴ�.]");
					}
				}
				else {
					log_system_message("[�Ǽ��� �����Ͽ����ϴ�.]");
				}
				build_mode = false;
				cursor.cur_size = 1;
			}
		}
		else if ((last_key == k_s) && build_mode) {
			if (key == k_space) {
				bool buildable = true;
				for (int d_x = 0; d_x < cursor.cur_size; d_x++) {
					for (int d_y = 0; d_y < cursor.cur_size; d_y++) {
						int result1 = find_closest_building(cursor.current.x + d_x, cursor.current.y + d_y, true);
						int result2 = find_closest_unit(cursor.current.x + d_x, cursor.current.y + d_y, true);
						if ((result2 != -1) || !((result1 != -1) && (buildings[result1].repr == 'P'))) {
							buildable = false;
						}
					}
				}
				if (buildable) {
					if (resource.spice >= 5) {
						resource.spice -= 5;
						buildings.push_back(make_Shelter(cursor.current.x, cursor.current.y, 2));
						log_system_message("[����ó�� �������ϴ�.]");

					}
					else {
						log_system_message("[�����̽��� �����մϴ�.]");
					}
				}
				else {
					log_system_message("[�Ǽ��� �����Ͽ����ϴ�.]");
				}
				build_mode = false;
				cursor.cur_size = 1;
			}
		}
		cursor.previous.x = cursor.current.x;
		cursor.previous.y = cursor.current.y;
		int building_index, unit_index;
		for (int i = 0; i < units.size(); i++) {
			if (units[i].repr == 'W') {
				int closest_unit_index = find_closest_unit_except(units[i].pos.x, units[i].pos.y, false, 'W');
				if (closest_unit_index > -1) {
					units[i].dest.x = units[closest_unit_index].pos.x;      // ������� ���� ����� ���ֿ��� ����
					units[i].dest.y = units[closest_unit_index].pos.y;
				}
				else { //���� ����� ������ ������
					units[i].dest.x = rand() % (MAP_WIDTH - 1) + 1;      // ������� �������� ���� ������ ��ġ��
					units[i].dest.y = rand() % (MAP_HEIGHT - 1) + 1;
				}
			}
			if (units[i].repr == 'H') {
				//�Ϻ����� dest ����
			}
			if (((sys_clock - units[i].last_moved_time) >= units[i].speed) && ((units[i].dest.x != units[i].pos.x) || (units[i].dest.y != units[i].pos.y))) {
				int next_x = units[i].pos.x;
				int next_y = units[i].pos.y;

				int delta_x = units[i].pos.x - units[i].dest.x;            //�̵�
				int delta_y = units[i].pos.y - units[i].dest.y;

				vector<POSITION> other_deltas(3); //���� �̵� ������ ������ �ٸ� �̵� ������� ����(���Ҵ� �翬�� 3��)

				if (abs(delta_x) >= abs(delta_y)) {
					if (delta_x > 0) {
						next_x -= 1;
						other_deltas[0] = { 0, 1 }; //���� �̵� ����(=(-1, 0))�� ������ �ٸ� ������� ���Ϳ� ����
						other_deltas[1] = { 1, 0 };
						other_deltas[2] = { 0, -1 };
					}
					else if (delta_x < 0) {
						next_x += 1;
						other_deltas[0] = { 0, 1 };//���� �̵� ����(=(1, 0))�� ������ �ٸ� ������� ���Ϳ� ����
						other_deltas[1] = { -1, 0 };
						other_deltas[2] = { 0, -1 };
					}
				}
				else {
					if (delta_y > 0) {
						next_y -= 1;
						other_deltas[0] = { 0, 1 };//���� �̵� ����(=(0, -1))�� ������ �ٸ� ������� ���Ϳ� ����
						other_deltas[1] = { 1, 0 };
						other_deltas[2] = { -1, 0 };
					}
					else if (delta_y < 0) {
						next_y += 1;
						other_deltas[0] = { 0, -1 };//���� �̵� ����(=(0, 1))�� ������ �ٸ� ������� ���Ϳ� ����
						other_deltas[1] = { 1, 0 };
						other_deltas[2] = { -1, 0 };
					}
				}

				int unit_in_next_pos, building_in_next_pos; //���� ������ ��ġ�� �����̳� ������ �ִ��� ã�´�.
				unit_in_next_pos = find_closest_unit(next_x, next_y, true);
				building_in_next_pos = find_closest_building(next_x, next_y, true);
				int index = 0;
				random_shuffle(other_deltas.begin(), other_deltas.end()); //���� ���� ������ ��ġ�� ���� ���� ��츦 ����ؼ�, ���� ���� �ܿ� �ٸ� ������ �����ϰ� ����ϱ� ���ؼ� ���͸� �����ϰ� ����

				while ((index < 3) && ((building_in_next_pos > -1) || (unit_in_next_pos > -1))) { //���� ������ ��ġ�� ���� �ִٸ� �ٸ� ������ ����ϱ� ����
					if ((units[i].repr == 'W') && (unit_in_next_pos > -1) && (units[unit_in_next_pos].repr == 'H')) { //���� ���� ������̰� �������� �� ��ġ�� �Ϻ����Ͱ� ������
						units[unit_in_next_pos].destroyed = true; //�Ϻ����͸� units���� �����
						if (selected_harvester == &units[unit_in_next_pos]) selected_harvester = nullptr;
						log_system_message("������ ��������� ��Ƹ������ϴ�.");
						break; //�ٸ� ������ ����ϱ⸦ �׸��д�.
					}
					if ((units[i].repr == 'H') && (building_in_next_pos > -1) && (buildings[building_in_next_pos].repr > '0') && (buildings[building_in_next_pos].repr <= '9')) {
						int mining_spice = rand() % 3 + 2;
						if (buildings[building_in_next_pos].repr > mining_spice + '0') {
							buildings[building_in_next_pos].repr -= mining_spice;
							resource.spice += mining_spice;
							if (resource.spice > resource.spice_max) resource.spice = resource.spice_max;
						}
						else {
							resource.spice += buildings[building_in_next_pos].repr - '0';
							buildings.erase(buildings.begin() + building_in_next_pos);
						}
						break;
					}
					next_x = units[i].pos.x + other_deltas[index].x; //���� �Ϻ����� ���� �����̳� ������ ������, next_x, next_y�� �ٸ� ������ �������� �ٲٰ�
					next_y = units[i].pos.y + other_deltas[index].y;
					unit_in_next_pos = find_closest_unit(next_x, next_y, true); //�ٲ� next_x, next_y�� �����̳� ������ �ִ��� ã�´�.
					building_in_next_pos = find_closest_building(next_x, next_y, true);
					index++;
				}
				if (units[i].repr == 'W') {
					int s_excrement;
					s_excrement = rand() % 20;
					if (s_excrement == 1) {
						buildings.push_back(make_Spice(units[i].pos.x, units[i].pos.y, rand() % 5 + 1));
					}
				}
				units[i].pos = { next_x, next_y }; //�׸��� ������� (next_x, next_y)�� �����δ�
				units[i].last_moved_time = sys_clock;
			}
		}
		for (int i = units.size() - 1; i >= 0; i--) {
			if (units[i].destroyed) {
				units.erase(units.begin() + i);
			}
		}

		switch (key) {
		case k_up:
			cursor.current.y -= 1;
			if ((click_time > 0) && (clock() - click_time < 200)) { //k_up�� 0.2�ʾȿ� �ѹ� �� ������
				cursor.current.y -= 4;
				click_time = 0;
			}
			else {
				click_time = clock();
			}
			break;
		case k_down:
			cursor.current.y += 1;
			if ((click_time > 0) && (clock() - click_time < 200)) {
				cursor.current.y += 4;
				click_time = 0;
			}
			else {
				click_time = clock();
			}
			break;
		case k_right:
			cursor.current.x += 1;
			if ((click_time > 0) && (clock() - click_time < 200)) {
				cursor.current.x += 4;
				click_time = 0;
			}
			else {
				click_time = clock();
			}
			break;
		case k_left:
			cursor.current.x -= 1;
			if ((click_time > 0) && (clock() - click_time < 200)) {
				cursor.current.x -= 4;
				click_time = 0;
			}
			else {
				click_time = clock();
			}
			break;
		case k_space:
			building_index = find_closest_building(cursor.current.x, cursor.current.y, true);
			unit_index = find_closest_unit(cursor.current.x, cursor.current.y, true);
			if (building_index >= 0) {
				BUILDING current_building = buildings[building_index];
				log_status_message(current_building.status_msg);
				selected_building = &buildings[building_index];
				log_console_message(current_building.console_msg);
			}
			else if (unit_index >= 0) {
				UNIT current_unit = units[unit_index];
				log_status_message(current_unit.status_msg);
				selected_unit = &units[unit_index];
				log_console_message(current_unit.console_msg);
			}
			else {
				log_status_message("�����Ը��� �ɵ��� �ִ� �縷�̴�.");
				selected_building = &nothing;
				selected_unit = &nothing2;
			}
			break;
		case k_b:
			log_console_message("P:Plate       D:Dormitory\n\nG:Garage      B:Barracks\n\nS:shelter");
			break;

		case k_esc:
			log_status_message("");
			log_console_message("B: Build");
			cursor.cur_size = 1;
			break;
		case k_quit:
			outro();
		case k_none:
		case k_undef:

		default:
			break;
		}
		if (key != k_none && key != k_up && key != k_right && key != k_left && key != k_down) {
			last_key = key;
		}
		if (!(1 <= cursor.current.y && cursor.current.y <= MAP_HEIGHT - 2 - (cursor.cur_size - 1) && 1 <= cursor.current.x && cursor.current.x <= MAP_WIDTH - 2 - (cursor.cur_size - 1))) {
			cursor.current.x = max(min(cursor.current.x, MAP_WIDTH - 2 - (cursor.cur_size - 1)), 1);
			cursor.current.y = max(min(cursor.current.y, MAP_HEIGHT - 2 - (cursor.cur_size - 1)), 1);
		}

		// ȭ�� ���
		display(resource, buildings, units, cursor);
		cursor.prev_size = cursor.cur_size;
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
		Building.console_msg = "";
	}
	else {
		Building.color = (Color::Blue << 4) + Color::White;
		Building.enemy = 0;
		Building.status_msg = "��Ʈ���̵��� ���̽��̴�.";
		Building.console_msg = "H : �Ϻ����� ����";
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
	Building.console_msg = "";
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
	Building.console_msg = "";
	return Building;
}
BUILDING make_Barracks(int x, int y, int size) {
	BUILDING Building;
	Building.pos = { x, y };
	Building.size = size;
	Building.color = (Color::DarkGreen << 4) + Color::Black;
	Building.enemy = 2;
	Building.repr = 'B';
	Building.cost = 4;
	Building.health = 20;
	char temp[100];
	snprintf(temp, sizeof(temp), "������ ���� �� �� �ִ� �����̴�.\n[�Ǽ����:%d ������ : %d]", Building.cost, Building.health);
	Building.status_msg = temp;
	Building.console_msg = "[���� ����] S:Soldier";
	return Building;

}
BUILDING make_Dormitory(int x, int y, int size) {
	BUILDING Building;
	Building.pos = { x, y };
	Building.size = size;
	Building.color = (Color::DarkGreen << 4) + Color::Black;
	Building.enemy = 2;
	Building.repr = 'D';
	Building.cost = 2;
	Building.health = 10;
	char temp[100];
	snprintf(temp, sizeof(temp), "�α� �ִ�ġ�� ������Ű�� �����̴�.\n[�Ǽ����:%d ������:%d]", Building.cost, Building.health);
	Building.status_msg = temp;
	return Building;
}
BUILDING make_Garage(int x, int y, int size) {
	BUILDING Building;
	Building.pos = { x, y };
	Building.size = size;
	Building.color = (Color::DarkGreen << 4) + Color::Black;
	Building.enemy = 2;
	Building.repr = 'G';
	Building.cost = 4;
	Building.health = 10;
	char temp[100];
	snprintf(temp, sizeof(temp), "�����̽� ���� �ִ�ġ�� ������Ű�� â���̴�.\n[�Ǽ����:%d ������:%d]", Building.cost, Building.health);
	Building.status_msg = temp;
	return Building;
}
BUILDING make_Shelter(int x, int y, int size) {
	BUILDING Building;
	Building.pos = { x, y };
	Building.size = size;
	Building.color = (Color::DarkGreen << 4) + Color::Black;
	Building.enemy = 2;
	Building.repr = 'S';
	Building.cost = 5;
	Building.health = 30;
	char temp[100];
	snprintf(temp, sizeof(temp), "Ư�������� ���� �� �� �ִ� �����̴�\n[�Ǽ����:%d  ������:%d]", Building.cost, Building.health);
	Building.status_msg = temp;
	Building.console_msg = "[������ ����] F:Fremen";
	return Building;
}
BUILDING make_Spice(int x, int y, int reserves) {
	BUILDING Building;
	Building.pos = { x , y };
	Building.size = 1;
	Building.color = (Color::Yellow << 4) + Color::Black;
	Building.enemy = 2;
	Building.repr = '0' + reserves;
	Building.status_msg = "���ֿ��� ���� ���� ������ �����̽��̴�.\n��Ȳ���� ���簰�� ���¿� �ణ Ǫ������ ������ ������.";
	Building.console_msg = "";
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
	Unit.last_moved_time = 0;
	Unit.destroyed = false;
	Unit.damage = INT_MAX;
	Unit.status_msg = "�𷡼ӿ� ��� ������̴�.\n������ ���� ����� �Լ����� ������\n��� ���� ���ƹ��� �� �ִ�.";
	Unit.console_msg = "";
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
		Unit.console_msg = "";
		Unit.last_moved_time = 0;
		Unit.destroyed = false;
	}
	else {
		Unit.color = (Color::Blue << 4) + Color::White;
		Unit.enemy = 0;
		Unit.console_msg = " H: Harvest(��Ȯ), M: move(�̵�)";
		Unit.last_moved_time = 0;
		Unit.cost = 5;
		Unit.population = 1;
		Unit.speed = 2000;
		Unit.health = 70;
		Unit.fov = 0;
		char temp[100];
		snprintf(temp, sizeof(temp), "��Ʈ���̴����� �Ϻ������̴�.\n[������:%d , �α���:%d , ü��:%d]", Unit.cost, Unit.population, Unit.health);
		Unit.status_msg = temp;
		Unit.destroyed = false;

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
	buildings.push_back(make_Spice(MAP_WIDTH - 2, 5, 5));
	buildings.push_back(make_Spice(1, MAP_HEIGHT - 6, 5));
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
