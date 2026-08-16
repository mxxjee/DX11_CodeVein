#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class StructuredBuffer;
class Shader;
class ComputeShader;
NS_END

NS_BEGIN(Client)

class Sample_StructuredBuffer final : public GameObject
{
private:
	explicit Sample_StructuredBuffer();
	explicit Sample_StructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Sample_StructuredBuffer(const Sample_StructuredBuffer& original);
	virtual ~Sample_StructuredBuffer();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	StructuredBuffer* m_pSB = { nullptr };
	StructuredBuffer* m_pSB2 = { nullptr };
	StructuredBuffer* m_pSB3 = { nullptr };
	Shader* m_pShaderCom = { nullptr };
	ComputeShader* m_pCSCom = { nullptr };


public:
	static Sample_StructuredBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
