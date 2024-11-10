#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>
#include <vector>
#include<string>

/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)

#define N_LAYER 2
#define MAP_WIDTH	60
#define MAP_HEIGHT	18

using namespace std;

/* ================= ��ġ�� ���� =================== */
// �ʿ��� ��ġ�� ��Ÿ���� ����ü
typedef struct {
	int x, y;
} POSITION;

// Ŀ�� ��ġ
typedef struct {
	POSITION previous;  // ���� ��ġ
	POSITION current;   // ���� ��ġ
} CURSOR;

// �Է� ������ Ű ����.
// �������� enum�� �����ߴµ�, ũ�� ����� ������ ���� �˻�
typedef enum {
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef, // ���ǵ��� ���� Ű �Է�
	k_space, //�����̽� �� �Է�
	k_esc, // esc �Է�
	k_h,
} KEY;


/* ================= ��ġ�� ����(2) =================== */
// ���Ǽ��� ���� �Լ���. KEY, POSITION, DIRECTION ����ü���� ���������� ��ȯ

// ���Ǽ� �Լ�
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.x + p2.x, p1.y + p2.y };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.x - p2.x, p1.y - p2.y };
	return p;
}

// ����Ű���� Ȯ���ϴ� �Լ�
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// ȭ��ǥ 'Ű'(KEY)�� '����'(DIRECTION)���� ��ȯ. ���� ���� �Ȱ����� Ÿ�Ը� �ٲ��ָ� ��
#define ktod(k)		(DIRECTION)(k)

// p�� d �������� �̵���Ų POSITION
#define pmove(p, d)		(padd((p), dtop(d)))



/* ================= game data =================== */
typedef struct {
	int spice;		// ���� ������ �����̽�
	int spice_max;  // �����̽� �ִ� ���差
	int population; // ���� �α� ��
	int population_max;  // ���� ������ �α� ��
} RESOURCE;
typedef struct {
	double x, y;
} POSITION_d;

// �밭 ����� ����. ��� �߰��ϸ鼭 ���� ������ ��
typedef struct {
	POSITION pos;        // ���� ��ġ(position)
	int size;
	char repr;            // ȭ�鿡 ǥ���� ����(representation)
	unsigned short color;
	int cost;
	int health;
	char enemy; // 0 : �Ʊ�, 1 : ����, 2 : �߸�
	string status_msg;
	string console_msg;
	string system_msg;
} BUILDING;

typedef struct {
	POSITION pos;        // ���� ��ġ(position)
	POSITION dest;        // ������(destination)
	char repr;            // ȭ�鿡 ǥ���� ����(representation)
	unsigned short color;
	int speed;    // '�� ms���� �� ĭ �����̴���'�� ����
	int cost;
	int population;
	char enemy;
	int damage;
	int attack_speed;
	int health;
	int fov;
	int last_moved_time;
	bool destroyed;
	string status_msg;
	string console_msg;
} UNIT;

typedef enum Color {
	Black = 0,  	//0
	DarkBlue = 1,	//1
	DarkGreen = 2,	//2
	DarkSkyBlue = 3,    //3
	DarkRed = 4,  	//4
	DarkPurple = 5,	//5
	DarkYellow = 6,	//6
	Gray = 7,		//7
	DarkGray = 8,	//8
	Blue = 9,		//9
	Green = 10,		//10
	SkyBlue = 11,	//11
	Red = 12,		//12
	Purple = 13,		//13
	Yellow = 14,		//14
	White = 15		//15
} COLOR;

#endif
