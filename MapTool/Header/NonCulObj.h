#pragma once
#include "MapObject.h"
#include "MT_Defines.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

class CNonCulObj final : public MapObject
{
private:
    explicit CNonCulObj(ID3D11Device* pD, ID3D11DeviceContext* pC);
    explicit CNonCulObj(const CNonCulObj& original);
    virtual ~CNonCulObj() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual _int    Update(const _float fDT) override;
    virtual _int    Update_Late(const _float fDT) override;
    virtual HRESULT Render(const _float fDT) override;

    void Set_Hovered(_bool bHovered) { m_bIsHovered = bHovered; }
    const _wstring& Get_ModelTag() const { return m_pProtoname; }

private:
    _wstring m_pProtoname = L"";
    _bool m_bIsHovered = false;

private:
    HRESULT Ready_Components();

public:
    static CNonCulObj* Create(ID3D11Device* pD, ID3D11DeviceContext* pC);
    virtual GameObject* Clone(void* arg) override;
    virtual void Free() override;
};