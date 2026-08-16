#pragma once
#include "Client_Define.h"
#include "NPC.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

NS_BEGIN(Client)

class NPC_Murasame : public NPC
{
private:
    explicit NPC_Murasame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit NPC_Murasame(const NPC_Murasame& original);
    virtual ~NPC_Murasame() = default;

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level) override;
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update_Priority(const _float fTimeDelta) override;
    virtual _int Update_Parallel(const _float fTimeDelta) override;
    virtual _int Update(const _float fTimeDelta) override;
    virtual _int Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

private:
    virtual HRESULT Ready_Components() override;
    virtual HRESULT Ready_States() override;

#pragma region IInteractable
public:
    virtual bool		IsInteractable();	//상호작용가능한지 물어보는 함수
    virtual _float		Get_InteractionRange() { return 3.f; }         //
    virtual void		Enter_InteractionRange(GameObject* pPlayer);		//상호작용 가능 범위에들어왔을때 호출되는함수
    virtual void		Stay_InteractionRange(GameObject* pPlayer, const _float& fTimeDelta);
    virtual void		Exit_InteractionRange(GameObject* pPlayer);		//범위에 들어왔다가 나갔을때 호출되는 함수

    virtual void		Enter_Interaction(GameObject* pPlayer);		//키를 눌러서 실제로 인터렉션실행(대부분 여기서 진짜 상호작용 행동을 정의)
    virtual void		Stay_Interaction(GameObject* pPlayer, const _float& fTimeDelta);
    virtual void		Exit_Interaction(GameObject* pPlayer);			//객체가 판단하여 알아서 exit_Interaction()을 수행해야한다.


    virtual _vector		Get_Position() { return GameObject::Get_Position(); }

    virtual void        Start_NPCCamera();
#pragma endregion

private:
    void Update_LookAt(const _float fTimeDelta);

private:
    _uint m_iCurrentAnimIndex = 0;
public:
    virtual HRESULT     Ready_ActionHashs();
    virtual bool        DoAction(string ActionName);//선택지 선택할 시 호출되는함수. 여기안에서 맞는동작 정의


public:
    static NPC_Murasame* Create(ID3D11Device* pD, ID3D11DeviceContext* pC, LEVEL _level);
    virtual GameObject* Clone(void* arg) override;
    virtual void Free() override;
    

};

NS_END