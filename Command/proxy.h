#pragma once

class stUser;
class CProtocolBuffer;

class CProxy {
public:
	bool CS_Tp(stUser* user, CProtocolBuffer* protocolBuffer);
	bool CS_KillUser(stUser* user, CProtocolBuffer* protocolBuffer);

private:
};