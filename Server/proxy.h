#pragma once

class CSectorList;
extern CSectorList* sectorList;

class CNetwork;
extern CNetwork* network;

class CStub;
extern CStub* packetMake;

class CProtocolBuffer;

extern void sectorLogic(stUser* user);

class CProxy {
public:

	bool CS_MoveStartProxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_MoveStopProxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_Attack1(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_Attack2(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_Attack3(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_Tp(stUser* user, CProtocolBuffer* protocolBuffer);

private:
	void attackProc(void (*attackStub)(CProtocolBuffer*, int, char, short, short), const int attackRangeX, const int attackRangeY, const int damage, stUser* user);
	void syncProc(stUser*, short x, short y);
};