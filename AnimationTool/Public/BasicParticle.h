#pragma once
#include "AnimationTool_Define.h"
#include "ParticleEffect.h"

NS_BEGIN(Engine)
class VIBuffer_Particle_Rect;
class Shader;
class ComputeShader;
class NewTexture;
NS_END

NS_BEGIN(AnimationTool)

class BasicParticle final : public ParticleEffect
{
private:
	explicit BasicParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit BasicParticle(const BasicParticle& Prototype);
	virtual ~BasicParticle() = default;

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
	HRESULT Change_Texture(const _wstring strFilePath);					// 텍스쳐 교체 함수
	HRESULT Change_MaskTexture(const _wstring strFilePath);				// 마스크 텍스처 교체 함수
	HRESULT Change_AlphaMaskTexture(const _wstring strFilePath);		// 알파 마스크 텍스처 교체 함수
	HRESULT Change_NoiseTexture(const _wstring strFilePath);			// 노이즈 텍스쳐 교체 함수
	HRESULT Change_Shader(const _wstring strShaderTag);					// 쉐이더 교체 함수
	HRESULT Change_Compute_Shader(const _wstring strShaderTag);			// 컴퓨트 쉐이더 교체 함수

public:
	static BasicParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END