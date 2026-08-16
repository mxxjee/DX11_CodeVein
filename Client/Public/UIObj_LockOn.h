#pragma once
#include "UIObject.h"


NS_BEGIN(Client)
class Monster;

class UIObj_LockOn :
    public UIObject
{
public:
    enum LockOnEventType{LOCKON,CLEAR};
public:
    struct LockOnUIEvent
    {
        LockOnEventType eType;
        bool bActive = true;
        Monster* pTarget;


    };


protected:
    explicit UIObj_LockOn();
    explicit UIObj_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_LockOn(const UIObj_LockOn& original);
    virtual ~UIObj_LockOn();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

private:
    void        Set_Scale_By_Distance();

public:
    static UIObj_LockOn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    void Free() override;


private:
    //타겟카메라캐싱(플레이어카메라)
    Camera* m_pTargetCam = nullptr;
    Monster* m_pLockOnTarget = nullptr;


};
NS_END

