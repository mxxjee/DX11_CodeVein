#pragma once

#include "Client_Define.h"
#include "PartObject.h"
#include "Monster.h"

NS_BEGIN(Client)
class Monster_Weapon : public PartObject
{
public:
	typedef struct tagWeaponDesc : PartObject::PARTOBJECT_DESC
	{
		_bool				bIdentity = false;
		const _float4x4*	pSocketMatrix = { nullptr }; //부착할 뼈의 행렬
		_int				iSocketIndex = { -1 };
		WEAPON_TYPE			eWeaponType = { WEAPON_TYPE::WP_END };
		_float3				vWeaponScale = { 1.f, 1.f, 1.f };
		const _float*		pDissolveTime = { nullptr };
		const _float*		pDissolveMax = { nullptr };
	}MONSTERWEAPON_DESC;

protected:
	explicit Monster_Weapon();
	explicit Monster_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Monster_Weapon(const Monster_Weapon& original);
	virtual ~Monster_Weapon();

public:
	WEAPON_TYPE Get_WeaponType() const { return m_eWeaponType; }
	void Set_SocketMatrix(const _float4x4* pNewSocketMatrix) { m_pSocketMatrix = pNewSocketMatrix; }
	void Set_SocketIndex(_int _boneIndex) { m_iSocketIndex = _boneIndex; }

	HRESULT Change_Component(_uint iLevel, const wstring& strPrototypeTag, const wstring& strComponentTag, Component** ppOutComponent)
	{
		auto iter = m_umapComponents.find(strComponentTag);
		if (iter != m_umapComponents.end())
		{
			Safe_Release(iter->second);
			m_umapComponents.erase(iter);
		}

		return Add_Component(iLevel, strPrototypeTag, strComponentTag, ppOutComponent);
	}

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

protected:
	_bool					m_bIdentity	= false;
	const _float4x4*		m_pSocketMatrix = { nullptr }; //구조체 받아서 저장할 변수
	_int					m_iSocketIndex = { -1 };
	WEAPON_TYPE				m_eWeaponType = { WEAPON_TYPE::WP_END };
	_float3					m_vWeaponScale = {};
	const _float*			m_pDissolveTime = { nullptr };
	const _float*			m_pDissolveMax = { nullptr };

public:
	void Free() override;

};
NS_END
