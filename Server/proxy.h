#pragma once

class CSectorList;
extern CSectorList* sectorList;

class CNetwork;
extern CNetwork* network;

class CStub;
extern CStub* packetMake;

class CProtocolBuffer;

extern stUser* userViewer;

extern void sectorLogic(stUser* user);

class CProxy {
public:

	bool CS_MoveStartProxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_MoveStopProxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_Attack1Proxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_Attack2Proxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_Attack3Proxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_TpProxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_KillUserProxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_UserListProxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_RegistUserViewerProxy(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_EchoProxy(stUser* user, CProtocolBuffer* protocolBuffer);

private:
	void attackProc(void (*attackStub)(CProtocolBuffer*, int, char, short, short), const int attackRangeX, const int attackRangeY, const int damage, stUser* user);
	void syncProc(stUser*, short x, short y);
};