#pragma once

class CStub {

public:

	void CS_TpStub(CProtocolBuffer* buffer, int id, short x, short y);
	void CS_KillStub(CProtocolBuffer* buffer, int id);
	void CS_UserListStub(CProtocolBuffer* buffer, int id, char direction, short left, short top, short right, short bottom);

private:



};