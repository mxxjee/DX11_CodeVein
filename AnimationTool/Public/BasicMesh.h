#pragma once
#include "AnimationTool_Define.h"
#include "MeshEffect.h"

NS_BEGIN(AnimationTool)

class BasicMesh final : public MeshEffect
{
private:
	explicit BasicMesh();
	explicit BasicMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit BasicMesh(const BasicMesh& original);
	virtual ~BasicMesh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	HRESULT Change_Model(const _wstring strFilePath);					// 모델 교체 함수
	HRESULT Change_Texture(const _wstring strFilePath);					// 텍스쳐 교체 함수
	HRESULT Change_MaskTexture(const _wstring strFilePath);				// 마스크 텍스처 교체 함수
	HRESULT Change_AlphaMaskTexture(const _wstring strFilePath);		// 알파 마스크 텍스처 교체 함수
	HRESULT Change_NoiseTexture(const _wstring strFilePath);			// 노이즈 텍스쳐 교체 함수
	HRESULT Change_Shader(const _wstring strShaderTag);					// 쉐이더 교체 함수

public:
	static BasicMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END