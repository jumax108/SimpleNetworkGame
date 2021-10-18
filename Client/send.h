#pragma once

class CNetwork;

extern CNetwork* network;

class CStubFunc{
public:
	virtual void SC_CreateMyCharacterStub(CRingBuffer* sendBuffer, int id, char direction, short x, short y, char hp)
	{

		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_CreateMyCharacter);
		packet << (char)SC_CreateMyCharacter;

		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		packet << hp;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void SC_OtherMyCharacterStub(CRingBuffer* sendBuffer, int id, char direction, short x, short y, char hp)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_OtherMyCharacter);
		packet << (char)SC_OtherMyCharacter;

		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		packet << hp;

		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void SC_DeleteCharacterStub(CRingBuffer* sendBuffer, int id)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_DeleteCharacter);
		packet << (char)SC_DeleteCharacter;

		packet << id;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());


		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void CS_MoveStartStub(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stCS_MoveStart);
		packet << (char)CS_MoveStart;

		packet << direction;
		packet << x;
		packet << y;

		printf("dir: %d, x: %d, y: %d\n",direction, x, y);

		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());


		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void SC_MoveStartStub(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_MoveStart);
		packet << (char)SC_MoveStart;

		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());


		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void CS_MoveStopStub(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

 		packet << (char)0x89;
		packet << (char)sizeof(stCS_MoveStop);
		packet << (char)CS_MoveStop;

		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());


		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void SC_MoveStopStub(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_MoveStop);
		packet << (char)SC_MoveStop;

		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());


		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void CS_Attack1Stub(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stCS_Attack1);
		packet << (char)CS_Attack1;

		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());


		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void SC_Attack1Stub(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_Attack1);
		packet << (char)SC_Attack1;

		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void CS_Attack2Stub(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stCS_Attack2);
		packet << (char)CS_Attack2;

		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void SC_Attack2Stub(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_Attack2);
		packet << (char)SC_Attack2;

		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void CS_Attack3Stub(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stCS_Attack3);
		packet << (char)CS_Attack3;

		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void SC_Attack3Stub(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_Attack3);
		packet << (char)SC_Attack3;

		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
	virtual void SC_DamageStub(CRingBuffer* sendBuffer, int attackId, int damageId, char damageHp)
	{
		CProtocolBuffer packet(50);

		packet << (char)0x89;
		packet << (char)sizeof(stSC_Damage);
		packet << (char)SC_Damage;

		packet << attackId;
		packet << damageId;
		packet << damageHp;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

		if (network->ableSendPacket == true) {
			network->sendPacket();
		}
	}
};
