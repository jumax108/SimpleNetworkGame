#pragma once

constexpr int MAP_WIDTH = 6400;
constexpr int MAP_HEIGHT = 6400;

constexpr int SECTOR_WIDTH = 128;
constexpr int SECTOR_HEIGHT = 128;

constexpr int MOVE_X_SPEED = 3;
constexpr int MOVE_Y_SPEED = 2;

#pragma pack(1)
struct stHeader {
	char code;
	char payloadSize;
	char payloadType;
};
struct stSC_CreateMyCharacter {
	int id;
	char direction;
	short x;
	short y;
	char hp;
};
struct stSC_CreateOtherCharacter {
	int id;
	char direction;
	short x;
	short y;
	char hp;
};
struct stSC_DeleteCharacter {
	int id;
};
struct stCS_MoveStart {
	char direction;
	short x;
	short y;
};
struct stSC_MoveStart {
	int id;
	char direction;
	short x;
	short y;
};
struct stCS_MoveStop {
	char direction;
	short x;
	short y;
};
struct stSC_MoveStop {
	int id;
	char direction;
	short x;
	short y;
};
struct stCS_Attack1 {
	char direction;
	short x;
	short y;
};
struct stSC_Attack1 {
	int id;
	char direction;
	short x;
	short y;
};
struct stCS_Attack2 {
	char direction;
	short x;
	short y;
};
struct stSC_Attack2 {
	int id;
	char direction;
	short x;
	short y;
};
struct stCS_Attack3 {
	char direction;
	short x;
	short y;
};
struct stSC_Attack3 {
	int id;
	char direction;
	short x;
	short y;
};
struct stSC_Damage {
	int attackId;
	int damageId;
	char damageHp;
};
struct stSC_Sync {
	int id;
	short x;
	short y;
};

struct stCS_Tp {
	int id;
	short x;
	short y;
};
struct stCS_KillUser {
	int id;
};
struct stCS_UserList {
	int id;
	char direction;
	short left;
	short top;
	short right;
	short bottom;
};
struct stSC_UserList {
	int num;
	int* id;
	char* direction;
	short* x;
	short* y;
};
#pragma pack()

constexpr unsigned int SC_CreateMyCharacter = 0;
constexpr unsigned int SC_CreateOtherCharacter = 1;
constexpr unsigned int SC_DeleteCharacter = 2;
constexpr unsigned int CS_MoveStart = 10;
constexpr unsigned int SC_MoveStart = 11;
constexpr unsigned int CS_MoveStop = 12;
constexpr unsigned int SC_MoveStop = 13;
constexpr unsigned int CS_Attack1 = 20;
constexpr unsigned int SC_Attack1 = 21;
constexpr unsigned int CS_Attack2 = 22;
constexpr unsigned int SC_Attack2 = 23;
constexpr unsigned int CS_Attack3 = 24;
constexpr unsigned int SC_Attack3 = 25;
constexpr unsigned int SC_Damage = 30;
constexpr unsigned int CS_Tp = 100;
constexpr unsigned int CS_KillUser = 101;
constexpr unsigned int CS_UserList = 102;
constexpr unsigned int SC_UserList = 103;
constexpr unsigned int SC_Sync = 251;

enum class MOVE_DIR {
	NONE = -1,
	MOVE_DIR_LL = 0,
	MOVE_DIR_LU = 1,
	MOVE_DIR_UU = 2,
	MOVE_DIR_RU = 3,
	MOVE_DIR_RR = 4,
	MOVE_DIR_RD = 5,
	MOVE_DIR_DD = 6,
	MOVE_DIR_LD = 7
};

constexpr int ATTACK1_RANGE_X = 80;
constexpr int ATTACK2_RANGE_X = 90;
constexpr int ATTACK3_RANGE_X = 100;

constexpr int ATTACK1_RANGE_Y = 10;
constexpr int ATTACK2_RANGE_Y = 10;
constexpr int ATTACK3_RANGE_Y = 20;

constexpr int ATTACK1_DAMAGE = 5;
constexpr int ATTACK2_DAMAGE = 7;
constexpr int ATTACK3_DAMAGE = 9;

constexpr int COORDINATE_ERROR_RANGE = 50;