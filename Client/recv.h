
class CProxyFuncBase{
public:
	virtual bool SC_CreateMyCharacterProxy(int id, char direction, short x, short y, char hp){ return false; }
	virtual bool SC_OtherMyCharacterProxy(int id, char direction, short x, short y, char hp){ return false; }
	virtual bool SC_DeleteCharacterProxy(int id){ return false; }
	virtual bool CS_MoveStartProxy(char direction, short x, short y){ return false; }
	virtual bool SC_MoveStartProxy(int id, char direction, short x, short y){ return false; }
	virtual bool CS_MoveStopProxy(char direction, short x, short y){ return false; }
	virtual bool SC_MoveStopProxy(int id, char direction, short x, short y){ return false; }
	virtual bool CS_Attack1Proxy(char direction, short x, short y){ return false; }
	virtual bool SC_Attack1Proxy(int id, char direction, short x, short y){ return false; }
	virtual bool CS_Attack2Proxy(char direction, short x, short y){ return false; }
	virtual bool SC_Attack2Proxy(int id, char direction, short x, short y){ return false; }
	virtual bool CS_Attack3Proxy(char direction, short x, short y){ return false; }
	virtual bool SC_Attack3Proxy(int id, char direction, short x, short y){ return false; }
	virtual bool SC_DamageProxy(int attackId, int damageId, char damageHp){ return false; }
	virtual bool SC_SyncProxy(unsigned int id, unsigned short x, unsigned short y) { return false; }
	 
	void packetProc(stHeader* header, CProtocolBuffer* payload, CProxyFuncBase* proxy) {
		switch (header->payloadType) {
		case SC_CreateMyCharacter:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			char hp;
			*payload >> hp;
			proxy->SC_CreateMyCharacterProxy(id, direction, x, y, hp);
		}
		break;
		case SC_OtherMyCharacter:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			char hp;
			*payload >> hp;
			proxy->SC_OtherMyCharacterProxy(id, direction, x, y, hp);
		}
		break;
		case SC_DeleteCharacter:
		{
			int id;
			*payload >> id;
			proxy->SC_DeleteCharacterProxy(id);
		}
		break;
		case CS_MoveStart:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->CS_MoveStartProxy(direction, x, y);
		}
		break;
		case SC_MoveStart:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->SC_MoveStartProxy(id, direction, x, y);
		}
		break;
		case CS_MoveStop:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->CS_MoveStopProxy(direction, x, y);
		}
		break;
		case SC_MoveStop:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->SC_MoveStopProxy(id, direction, x, y);
		}
		break;
		case CS_Attack1:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->CS_Attack1Proxy(direction, x, y);
		}
		break;
		case SC_Attack1:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->SC_Attack1Proxy(id, direction, x, y);
		}
		break;
		case CS_Attack2:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->CS_Attack2Proxy(direction, x, y);
		}
		break;
		case SC_Attack2:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->SC_Attack2Proxy(id, direction, x, y);
		}
		break;
		case CS_Attack3:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->CS_Attack3Proxy(direction, x, y);
		}
		break;
		case SC_Attack3:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxy->SC_Attack3Proxy(id, direction, x, y);
		}
		break;
		case SC_Damage:
		{
			int attackId;
			*payload >> attackId;
			int damageId;
			*payload >> damageId;
			char damageHp;
			*payload >> damageHp;
			proxy->SC_DamageProxy(attackId, damageId, damageHp);
		}
		break;
		}
	}

};

class CProxyFunc : public CProxyFuncBase {

	virtual bool SC_CreateMyCharacterProxy(int id, char direction, short x, short y, char hp);
	virtual bool SC_OtherMyCharacterProxy(int id, char direction, short x, short y, char hp);
	virtual bool SC_DeleteCharacterProxy(int id);
	virtual bool SC_MoveStartProxy(int id, char direction, short x, short y);
	virtual bool SC_MoveStopProxy(int id, char direction, short x, short y);
	virtual bool SC_Attack1Proxy(int id, char direction, short x, short y);
	virtual bool SC_Attack2Proxy(int id, char direction, short x, short y);
	virtual bool SC_Attack3Proxy(int id, char direction, short x, short y);
	virtual bool SC_DamageProxy(int attackId, int damageId, char damageHp);
	virtual bool SC_SyncProxy(unsigned int id, unsigned short x, unsigned short y);

}; 

