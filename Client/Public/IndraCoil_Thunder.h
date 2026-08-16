#pragma once

#include "Client_Define.h"
#include "Projectile.h"

NS_BEGIN(Client)
class IndraCoil_Thunder final : public Projectile
{
public:
    typedef struct tagIndraCoil_Thunder : public Projectile::PROJECTILE_DESC
    {

    }INDRACOILTHUNDER_DESC;

private:
    explicit IndraCoil_Thunder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit IndraCoil_Thunder(const IndraCoil_Thunder& original);
    virtual ~IndraCoil_Thunder() = default;

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level);
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_Priority(const _float fTimeDelta) override;
    virtual _int    Update_Parallel(const _float fTimeDelta) override;
    virtual _int    Update(const _float fTimeDelta) override;
    virtual _int    Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

private:
    void            OnSpawn(void* _arg) override;
    void            OnDespawn() override;

public:
    static IndraCoil_Thunder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;
};
NS_END
