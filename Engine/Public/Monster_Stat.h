#pragma once
#include "Stat.h"

NS_BEGIN(Engine)
class ENGINE_DLL Monster_Stat : public Stat
{
public:
	typedef struct tagMonsterStatDesc : public Stat::STAT_DESC
	{
	}MONSTERSTAT_DESC;

private:
	explicit Monster_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Monster_Stat(const Monster_Stat& original);
	virtual ~Monster_Stat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;

private:
	MONSTERSTAT_DESC m_MonsterStat = {};

public:
	static Monster_Stat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END