#pragma once
#include "GameObject.h"

NS_BEGIN(UITool)

class Dummy :
    public GameObject
{
protected:
	explicit Dummy();
	explicit Dummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Dummy(const Dummy& original);
	virtual ~Dummy();

	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg);


public:
	virtual _int	Update_Late(const _float fTimeDelta);

private:
	void		Take_Damage(_float fDamage);

public:
	static Dummy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	virtual GameObject* Clone(void* arg);

public:
	void Free() override;

private:
};
NS_END

