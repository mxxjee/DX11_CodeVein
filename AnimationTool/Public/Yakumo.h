#pragma once
#include "AnimationTool_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class Model;
class StateMachine;
class Shader;
NS_END


NS_BEGIN(AnimationTool)
class Yakumo : public ContainerObject
{
public:
    typedef struct tagYakumoDesc : public GameObject::GAMEOBJECT_DESC
    {
        PHYSX_CONTROLLER_DESC tControllerDesc;

    }YAKUMO_DESC;

private:
    explicit Yakumo();
    explicit Yakumo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Yakumo(const Yakumo& original);
    virtual ~Yakumo();

public:
    class Yakumo_Weapon* Get_ActivePlayerWeapon() { return m_pActiveWeapon; }

public:
    virtual HRESULT				Initialize_Prototype(LEVEL _level);
    virtual HRESULT				Initialize(void* arg) override;
    virtual _int				Update_Priority(const _float fTimeDelta) override;
    virtual _int				Update_Parallel(const _float fTimeDelta) override;
    virtual _int				Update(const _float fTimeDelta) override;
    virtual _int				Update_Late(const _float fTimeDelta) override;
    virtual HRESULT				Render(const _float fTimeDelta) override;

private:
    void                        Move_Smooth(const _float fTimeDelta);

    class Yakumo_Weapon*        m_pActiveWeapon = { nullptr };

public:
    _float3                     m_vRootMotionDelta = {};
    Vector3                     m_vMoveDir = { 0.f, 0.f, 0.f };
    _float                      m_fSpeed = { 5.f };


private:
    HRESULT						Ready_Components();
    HRESULT                     Bind_ShaderResources();
    HRESULT						Ready_PartObjects();
    HRESULT						Ready_States();
    HRESULT						Ready_Event();
    HRESULT						Ready_PhysXEvent();

public:
    static Yakumo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override final;
};

NS_END