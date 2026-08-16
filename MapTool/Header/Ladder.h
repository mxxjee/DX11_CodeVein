#pragma once
#include "MapObject.h"
#include "MT_Defines.h"

class CLadder final : public MapObject
{
public:

private:
    explicit CLadder(ID3D11Device* pD, ID3D11DeviceContext* pC);
    explicit CLadder(const CLadder& original);
    virtual ~CLadder() = default;

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(void* _arg) override;
    _int Update_Priority(const _float fTimeDelta) override;
    _int Update_Parallel(const _float fTimeDelta) override;
    _int Update(const _float fTimeDelta) override;
    _int Update_Late(const _float fTimeDelta) override;
    HRESULT Render(const _float fTimeDelta) override;

    virtual ordered_json Get_ExtraData() override;

private:
    HRESULT Ready_Components();

private:
    Model* m_pModelTopCom = { nullptr };
    Model* m_pModelMiddleCom = { nullptr };
    Model* m_pModelBottomCom = { nullptr };

    _uint m_iLength = 0;
    _uint m_iTotalLength = 0;

    _float3 m_vTopPosition;
    _float3 m_vBottomPosition;

public:
    static CLadder* Create(ID3D11Device* pD, ID3D11DeviceContext* pC);
    virtual GameObject* Clone(void* arg) override;
    virtual void Free() override;
};