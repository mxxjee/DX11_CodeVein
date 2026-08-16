#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Texture final : public Component
{
private:
	explicit Texture();
	explicit Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Texture(const Texture& original);
	virtual ~Texture();

public:
	HRESULT Initialize_Prototype(const _wstring& textureFilePath, const _uint numTextures);
	HRESULT Initialize(void* arg);

	/* 쉐이더 컴포넌트, 값을 넘길 변수 이름, 텍스쳐 번호 */
	HRESULT Bind_ShaderResource(class Shader* shader, const _string& constantname, _uint textureindex = 0);
	/* 쉐이더 컴포넌트, 값을 넘길 변수 이름, 텍스쳐 번호 */
	HRESULT Bind_ShaderResources(Shader* shader, const _string& constantname);
    /* 특정 범위의 텍스처만 바인딩 */
    HRESULT Bind_ShaderResources_Range(Shader* shader, _uint slot, _uint startIndex, _uint count, _uint stageMask = stage_PS);
    /* 슬롯 직접 지정하여 단일 텍스처 바인딩 */
    HRESULT Bind_ShaderResource_FullSlot(Shader* shader, _uint slot, _uint textureIndex, _uint stageMask = stage_PS);
    /* 슬롯 직접 지정하여 전체 텍스처 바인딩 */
    HRESULT Bind_ShaderResources_FullSlot(Shader* shader, _uint slot, _uint stageMask = stage_PS);
    /* 슬롯 직접 지정하여 범위 텍스처 바인딩 */
    HRESULT Bind_ShaderResources_Range_FullSlot(Shader* shader, _uint slot, _uint startIndex, _uint count, _uint stageMask = stage_PS);
    /* 특정 인덱스의 SRV 반환 */
    ID3D11ShaderResourceView* Get_SRV(_uint index) const {
        if (index >= m_iNumTextures)
            return nullptr;

        return m_vecTextures[index];
    }

private:
	_uint								m_iNumTextures = {};
	vector<ID3D11ShaderResourceView*>	m_vecTextures = {};

public:
	static Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& textureFilePath, const _uint numTextures = 1);
	virtual Component* Clone(void* arg);

public:
	void Free() override final;

};

NS_END
