#pragma once

class CNetwork;
extern CNetwork* network;

class CLogger;
extern CLogger* logger;

class CStub {

public:

	void SC_CreateMyCharacterStub(CProtocolBuffer* buffer, int id, char direction, short x, short y, char hp);
	void SC_CreateOtherCharacterStub(CProtocolBuffer* buffer,  int id, char direction, short x, short y, char hp);
	void SC_DeleteCharacterStub(CProtocolBuffer* buffer,  int id);
	void SC_MoveStartStub(CProtocolBuffer* buffer, int id, char direction, short x, short y);
	void SC_MoveStopStub(CProtocolBuffer* buffer, int id, char direction, short x, short y);
	static void SC_Attack1Stub(CProtocolBuffer* buffer, int id, char direction, short x, short y);
	static void SC_Attack2Stub(CProtocolBuffer* buffer, int id, char direction, short x, short y);
	static void SC_Attack3Stub(CProtocolBuffer* buffer, int id, char direction, short x, short y);
	void SC_DamageStub(CProtocolBuffer* buffer, int attackId, int damageId, char damageHp);
	void SC_SyncStub(CProtocolBuffer* protocolBuffer, int id, short x, short y);

private:



};