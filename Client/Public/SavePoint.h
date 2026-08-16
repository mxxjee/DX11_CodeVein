#pragma once
#include "MapObject.h"
#include "Client_Define.h"
#include "IInteractable.h"

namespace Client
{
	class MinimapRenderComponent;
}
NS_BEGIN(Engine)
class Player_Stat;
NS_END

NS_BEGIN(Client)
class InteractionManager;
class InventoryManager;



class SavePoint final : public MapObject, IInteractable
{
private:
	explicit SavePoint(ID3D11Device* pD, ID3D11DeviceContext* pC);
	explicit SavePoint(const SavePoint& original);
	virtual ~SavePoint() = default;

public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg) override;
	_int Update_Priority(const _float fTimeDelta) override;
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta) override;
	_int Update_Late(const _float fTimeDelta) override;
	HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

	HRESULT Player_Teleport(const _float fDT);

	virtual ordered_json Get_ExtraData() override;
	const SAVE_POINT_INFO* Get_SavePointInfo() const { return &m_SavePointInfo; }

	void Set_Value();

private:
	SAVE_POINT_INFO m_SavePointInfo = {};
	MAP_TYPE m_eSPMapType = MAP_TYPE::ST00_BASE;
	_uint m_iSPIndex = 0;
	class Player_Stat* m_pPlayerStat = { nullptr };
	class ParticleSystem* m_pEffect = nullptr;

	//사운드용
	string  m_strSoundGroupName = "";
	_float  m_fMaxSoundDistance = 8.0f; // 소리가 들리기 시작하는 최대 반경
	_float  m_fMaxVolume = 0.4f;         // 가까울 때의 최대 소리 크기

	bool m_bSoundStarted = false;
private:
	HRESULT Ready_Components();
#pragma region IInteractable
public:
	virtual bool		IsInteractable();	//상호작용가능한지 물어보는 함수
	virtual _float		Get_InteractionRange() { return 3.f; }         //
	virtual void		Enter_InteractionRange(GameObject* pPlayer);		//상호작용 가능 범위에들어왔을때 호출되는함수
	virtual void		Stay_InteractionRange(GameObject* pPlayer,const _float& fTimeDelta);
	virtual void		Exit_InteractionRange(GameObject* pPlayer);		//범위에 들어왔다가 나갔을때 호출되는 함수

	virtual void		Enter_Interaction(GameObject* pPlayer);		//키를 눌러서 실제로 인터렉션실행(대부분 여기서 진짜 상호작용 행동을 정의)
	virtual void		Stay_Interaction(GameObject* pPlayer,const _float& fTimeDelta);
	virtual void		Exit_Interaction(GameObject* pPlayer);			//객체가 판단하여 알아서 exit_Interaction()을 수행해야한다.


	virtual _vector		Get_Position() { return GameObject::Get_Position(); }
#pragma endregion
public:
	static SavePoint* Create(ID3D11Device* pD, ID3D11DeviceContext* pC, LEVEL _level);
	virtual GameObject* Clone(void* arg) override;
	virtual void Free() override;

private:
	MinimapRenderComponent* m_pMinimapRenderCom = { nullptr };
	InteractionManager* m_pInteractionManager = nullptr;
	InventoryManager* m_pInventoryManager = nullptr;

};

NS_END
