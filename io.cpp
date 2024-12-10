/*
* raw(?) I/O
*/
#include "io.h"



KEY get_key(void) {
	if (!_kbhit()) {  // �Էµ� Ű�� �ִ��� Ȯ��
		return k_none;
	}

	int byte = _getch();    // �Էµ� Ű�� ���� �ޱ�
	switch (byte) {
	case 'q': return k_quit;// 'q'�� ������ ����
	case 'h': return k_h;
	case 32: return k_space;
	case 27: return k_esc;
	case 'b': return k_b;
	case 'd': return k_d;
	case 'g': return k_g;
	case 's': return k_s;
	case 'p': return k_p;
	case 'm': return k_m;
	case 224:
		byte = _getch();  // MSB 224�� �Է� �Ǹ� 1����Ʈ �� ���� �ޱ�
		switch (byte) {

		case 72: return k_up;
		case 75: return k_left;
		case 77: return k_right;
		case 80: return k_down;
		default: return k_undef;
		}
	default: return k_undef;
	}
}
