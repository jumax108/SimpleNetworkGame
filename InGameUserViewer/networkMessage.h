#pragma once

#pragma pack(1)
struct stPacketHeader {
	BYTE code;
	BYTE size;
	BYTE type;
};
#pragma pack()

#define	SC_CREATE_MY_CHARACTER			0
#pragma pack(1)
struct stSC_CREATE_MY_CHARACTER {

	UINT id;
	BYTE direction;
	USHORT x;
	USHORT y;
	BYTE hp;

};
#pragma pack()

#define SC_CREATE_OTHER_CHARACTER		1
#pragma pack(1)
struct stSC_CREATE_OTHER_CHARACTER {

	UINT id;
	BYTE direction;
	USHORT x;
	USHORT y;
	BYTE hp;

};
#pragma pack()

#define SC_DELETE_CHARACTER				2
#pragma pack(1)
struct stSC_DELETE_CHARACTER {

	UINT id;

};
#pragma pack()

#define MOVE_DIR_LL						0
#define MOVE_DIR_LU						1
#define MOVE_DIR_UU						2
#define MOVE_DIR_RU						3
#define MOVE_DIR_RR						4
#define MOVE_DIR_RD						5
#define MOVE_DIR_DD						6
#define MOVE_DIR_LD						7

#define CS_MOVE_START					10
#pragma pack(1)
struct stCS_MOVE_START {

	BYTE direction;
	USHORT x;
	USHORT y;

};
#pragma pack()


#define SC_MOVE_START				11
#pragma pack(1)
struct stSC_MOVE_START {
	UINT id;
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()


#define CS_MOVE_STOP				12
#pragma pack(1)
struct stCS_MOVE_STOP {
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()


#define SC_MOVE_STOP				13
#pragma pack(1)
struct stSC_MOVE_STOP {
	UINT id;
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()


#define CS_ATTACK1					20
#pragma pack(1)
struct stCS_ATTACK1 {
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()


#define SC_ATTACK1					21
#pragma pack(1)
struct stSC_ATTACK1 {
	UINT id;
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()


#define CS_ATTACK2					22			
#pragma pack(1)
struct stCS_ATTACK2 {
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()


#define SC_ATTACK2					23
#pragma pack(1)
struct stSC_ATTACK2{
	UINT id;
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()



#define CS_ATTACK3					24
#pragma pack(1)
struct stCS_ATTACK3{
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()


#define SC_ATTACK3					25
#pragma pack(1)
struct stSC_ATTACK3 {
	UINT id;
	BYTE direction;
	USHORT x;
	USHORT y;
};
#pragma pack()


#define SC_DAMAGE					30
#pragma pack(1)
struct stSC_DAMAGE{
	UINT attackId;
	UINT damageId;
	BYTE damageHp;
};
#pragma pack()


#define SC_SYNC						251
#pragma pack(1)
struct stSC_SYNC {
	unsigned int id;
	unsigned short x;
	unsigned short y;
};
#pragma pack()