#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Rect final : public VIBuffer
{
private:
	explicit VIBuffer_Rect();
	explicit VIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit VIBuffer_Rect(const VIBuffer_Rect& original);
	virtual ~VIBuffer_Rect();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	static VIBuffer_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg) override;

public:
	void Free() override final;

};

NS_END
