#pragma once
#include "VFXTool_Define.h"
#include "Decal.h"

NS_BEGIN(VFXTool)

class Decal_Blood final : public Decal
{
private:
	explicit Decal_Blood();
	explicit Decal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Decal_Blood(const Decal_Blood& original);
	virtual ~Decal_Blood() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static Decal_Blood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END