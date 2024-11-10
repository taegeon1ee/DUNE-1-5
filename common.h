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

/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int x, y;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;   // 현재 위치
} CURSOR;

// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef, // 정의되지 않은 키 입력
	k_space, //스페이스 바 입력
	k_esc, // esc 입력
	k_h,
} KEY;


/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.x + p2.x, p1.y + p2.y };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.x - p2.x, p1.y - p2.y };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)		(padd((p), dtop(d)))



/* ================= game data =================== */
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;
typedef struct {
	double x, y;
} POSITION_d;

// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
	POSITION pos;        // 현재 위치(position)
	int size;
	char repr;            // 화면에 표시할 문자(representation)
	unsigned short color;
	int cost;
	int health;
	char enemy; // 0 : 아군, 1 : 적군, 2 : 중립
	string status_msg;
	string console_msg;
	string system_msg;
} BUILDING;

typedef struct {
	POSITION pos;        // 현재 위치(position)
	POSITION dest;        // 목적지(destination)
	char repr;            // 화면에 표시할 문자(representation)
	unsigned short color;
	int speed;    // '몇 ms마다 한 칸 움직이는지'를 뜻함
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
