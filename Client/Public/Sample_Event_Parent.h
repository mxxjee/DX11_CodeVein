#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class Sample_Event_Parent abstract : public GameObject
{
protected:
	explicit Sample_Event_Parent();
	explicit Sample_Event_Parent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Sample_Event_Parent(const Sample_Event_Parent& original);
	virtual ~Sample_Event_Parent();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);

protected:
	enum targetID { TARGET_PLAYER, TARGET_MONSTER, TARGET_PROP, TARGET_END };
	_uint m_iTargetID = TARGET_END;

public:
	static Sample_Event_Parent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

public:
	void Free() override;

};

NS_END
