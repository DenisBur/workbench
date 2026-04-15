//! Радиус зоны флага (м).
class TFA_Constants
{
	static const float ZONE_RADIUS = 300.0;
	static const float TELEPORT_EXTRA = 300.0;
	static const float ADD_MEMBER_MAX_DIST = 4.0;

	//! RPC: ответ игроку (сервер -> клиент) и RPC на объект флага (клиент -> сервер)
	static const int RPC_TFA_UI_PAYLOAD = 984002;
	static const int RPC_TFA_ADD_MEMBER = 984003;
	static const int RPC_TFA_REMOVE_MEMBER = 984004;
	static const int RPC_TFA_SET_ROLE = 984005;
	static const int RPC_TFA_TRANSFER_OWNER = 984006;
	static const int RPC_TFA_TRANSFER_NEAREST = 984007;
}

//! Роли (кроме владельца — отдельное поле UID)
enum ETFA_Role
{
	Resident = 0,
	Builder = 1
}
