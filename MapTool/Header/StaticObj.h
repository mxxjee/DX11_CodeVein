#pragma once
#include "MapObject.h"
#include "MT_Defines.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

class CStaticObj final : public MapObject
{
private:
    explicit CStaticObj(ID3D11Device* pD, ID3D11DeviceContext* pC);
    explicit CStaticObj(const CStaticObj& original);
    virtual ~CStaticObj() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual _int    Update(const _float fDT) override;
    virtual _int    Update_Late(const _float fDT) override;
    virtual HRESULT Render(const _float fDT) override;
    virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

    void Set_Hovered(_bool bHovered) { m_bIsHovered = bHovered; }
    const _wstring& Get_ModelTag() const { return m_pProtoname; }

private:
    _wstring m_pProtoname = L"";
    _bool m_bIsHovered = false;
    physx::PxRigidStatic* m_pPhysXActor = nullptr;
    _float m_fAccTime = 0.f;

    bool m_bWasSelected = false;
    bool m_bOriginalVisible = true;

private:
    HRESULT Ready_Components();

public:
    static CStaticObj* Create(ID3D11Device* pD, ID3D11DeviceContext* pC);
    virtual GameObject* Clone(void* arg) override;
    virtual void Free() override;
};