#pragma once
#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(Client)
class Yakumo_Weapon final : public PartObject
{
public:
	typedef struct tagYakumoWeapon : PartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr }; //부착할 뼈의 행렬
		const _float* pDissolveTime = { nullptr };
	}YAKUMOWEAPON_DESC;

private:
	explicit Yakumo_Weapon();
	explicit Yakumo_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Yakumo_Weapon(const Yakumo_Weapon& original);
	virtual ~Yakumo_Weapon();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
	const _float4x4* m_pSocketMatrix = { nullptr }; //구조체 받아서 저장할 변수
	const _float* m_pDissolveTime = { nullptr };

public:
	void Playing_WeaponClash_Sound();

private:
	HRESULT Ready_Components() ;
	HRESULT Bind_ShaderResources();

public:
	static Yakumo_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END