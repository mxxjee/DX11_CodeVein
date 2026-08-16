#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Instancing final : public VIBuffer
{
private:
	explicit VIBuffer_Instancing();
	explicit VIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit VIBuffer_Instancing(const VIBuffer_Instancing& original);
	virtual ~VIBuffer_Instancing();

public:
	HRESULT Initialize_Prototype(_uint iMaxInstance);
	HRESULT Update_Instancing(const vector<_float4x4>& vecWorldMatrices);
	ID3D11Buffer* Get_InstanceBuffer() { return m_pVB; }

private:
	_uint m_iMaxInstance = 0;

public:
	static VIBuffer_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iMaxInstance);
	virtual Component* Clone(void* arg) override;

public:
	void Free() override final;

};

NS_END
