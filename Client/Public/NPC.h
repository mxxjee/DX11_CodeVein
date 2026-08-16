#pragma once
#include "Client_Define.h"
#include "Character.h"
#include "IInteractable.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

NS_BEGIN(Client)

class NPC abstract : public Character, public IInteractable
{
public:
    typedef struct tagNpcDesc : public GameObject::GAMEOBJECT_DESC
    {
        PHYSX_CONTROLLER_DESC tControllerDesc;

    }NPC_DESC;

protected:
    explicit NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit NPC(const NPC& original);
    virtual ~NPC() = default;

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level);
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update_Priority(const _float fTimeDelta) override;
    virtual _int Update_Parallel(const _float fTimeDelta) override;
    virtual _int Update(const _float fTimeDelta) override;
    virtual _int Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

protected:
    virtual HRESULT Ready_Components();
    virtual HRESULT Ready_States();

    void Apply_Gravity(const _float fTimeDelta);
    _float Calculate_DistanceToPlayer();    // 플레이어와의 거리 계산
    _bool Is_PlayerInRange(_float fRange);    // 플레이어가 상호작용 범위 내에 있는지

protected:
    _float m_fVelocityY = 0.f;      // 현재 수직 속도
    _bool m_bIsStartInterAction = false;

#pragma region IInteractable
public:
    virtual bool		IsInteractable();	//상호작용가능한지 물어보는 함수
    virtual _float		Get_InteractionRange() { return 0.f; }         //
    virtual void		Enter_InteractionRange(GameObject* pPlayer);		//상호작용 가능 범위에들어왔을때 호출되는함수
    virtual void		Stay_InteractionRange(GameObject* pPlayer, const _float& fTimeDelta);
    virtual void		Exit_InteractionRange(GameObject* pPlayer);		//범위에 들어왔다가 나갔을때 호출되는 함수

    virtual void		Enter_Interaction(GameObject* pPlayer);		//키를 눌러서 실제로 인터렉션실행(대부분 여기서 진짜 상호작용 행동을 정의)
    virtual void		Stay_Interaction(GameObject* pPlayer, const _float& fTimeDelta);
    virtual void		Exit_Interaction(GameObject* pPlayer);			//객체가 판단하여 알아서 exit_Interaction()을 수행해야한다.

    virtual _vector		Get_Position() { return GameObject::Get_Position(); }

    _bool               Get_Is_StartInterAction() { return m_bIsStartInterAction; }
    void                Set_StartInteraction(bool b) { m_bIsStartInterAction = b; }

    virtual void        Start_NPCCamera() {}
#pragma endregion


public:
    virtual HRESULT     Ready_ActionHashs() { return S_OK; }
    virtual bool        DoAction(string ActionName) { return false; }//선택지 선택할 시 호출되는함수. 여기안에서 맞는동작 정의

public:
	void Free() override;

protected:
    NPCData*                m_pNPCData = nullptr;
    unordered_map<size_t ,function<void()>>          m_ActionHashs;  //액션정의할 이름의 해시들저장


};

NS_END