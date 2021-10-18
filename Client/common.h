#pragma pack(1)
struct stHeader{
	char code;
	char payloadSize;
	char payloadType;
};
struct stSC_CreateMyCharacter{
	int id;
	char direction;
	short x;
	short y;
	char hp;
};
struct stSC_OtherMyCharacter{
	int id;
	char direction;
	short x;
	short y;
	char hp;
};
struct stSC_DeleteCharacter{
	int id;
};
struct stCS_MoveStart{
	char direction;
	short x;
	short y;
};
struct stSC_MoveStart{
	int id;
	char direction;
	short x;
	short y;
};
struct stCS_MoveStop{
	char direction;
	short x;
	short y;
};
struct stSC_MoveStop{
	int id;
	char direction;
	short x;
	short y;
};
struct stCS_Attack1{
	char direction;
	short x;
	short y;
};
struct stSC_Attack1{
	int id;
	char direction;
	short x;
	short y;
};
struct stCS_Attack2{
	char direction;
	short x;
	short y;
};
struct stSC_Attack2{
	int id;
	char direction;
	short x;
	short y;
};
struct stCS_Attack3{
	char direction;
	short x;
	short y;
};
struct stSC_Attack3{
	int id;
	char direction;
	short x;
	short y;
};
struct stSC_Damage{
	int attackId;
	int damageId;
	char damageHp;
};
#pragma pack()
constexpr unsigned int SC_CreateMyCharacter = 0;
constexpr unsigned int SC_OtherMyCharacter = 1;
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
