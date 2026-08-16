#pragma once
#include "ShaderTool_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class VIBuffer_Terrain;
class Shader;
class Texture;
NS_END

NS_BEGIN(ShaderTool)

class Terrain final : public GameObject
{
private:
	Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	Terrain(const Terrain& _Prototype);
	virtual ~Terrain() = default;

public:
	//얘는 Initialze가 2가지다(원본, 복사본)
	virtual HRESULT Initialize_ProtoType(LEVEL _level);
	virtual HRESULT Initialize(void* pArg);
	//update를 3가지로 만든다
	virtual _int Update_Priority(_float _fTimeDelta);
	virtual _int Update(_float _fTimeDelta);
	virtual _int Update_Late(_float _fTimeDelta);
	//렌더 만들어줘야된다
	virtual HRESULT Render(const _float fTimeDelta);

private:
	//컴포넌트 복사생성위해서
	HRESULT Ready_Component();
	HRESULT Bind_Shader();

	//그리기위해서 VIBuffer, Shader, Texture를 가진다
private:
	VIBuffer_Terrain* m_pVIBuffer = {};
	Shader* m_pShaderCom = {};
	Texture* m_pTexture = {};

public:
	static Terrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext, LEVEL _level);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END