#pragma once
#include "MapObject.h"
#include "ShaderTool_Define.h"

struct SAVE_POINT_INFO
{
	_uint iIndex;              // 같은 맵 내 저장점 인덱스
	LEVEL targetLevel;          // 텔레포트할 목표 레벨
	MAP_TYPE mapType;           // 맵 타입
	_float4 spawnPosition;      // 플레이어 스폰 위치
	_float4 spawnRotation;      // 플레이어 스폰 회전
	wstring wsName;             // 저장점 이름
};

class SavePoint final : public MapObject
{
private:
	explicit SavePoint(ID3D11Device* pD, ID3D11DeviceContext* pC);
	explicit SavePoint(const SavePoint& original);
	virtual ~SavePoint() = default;

public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg) override;
	_int Update_Priority(const _float fTimeDelta) override;
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta) override;
	_int Update_Late(const _float fTimeDelta) override;
	HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

	HRESULT Player_Teleport(const _float fDT);

	virtual ordered_json Get_ExtraData() override;
	const SAVE_POINT_INFO* Get_SavePointInfo() const { return &m_SavePointInfo; }

	void Set_Value();

private:
	SAVE_POINT_INFO m_SavePointInfo = {};
	MAP_TYPE m_eSPMapType = MAP_TYPE::ST00_BASE;
	_uint m_iSPIndex = 0;

private:
	HRESULT Ready_Components();

public:
	static SavePoint* Create(ID3D11Device* pD, ID3D11DeviceContext* pC, LEVEL _level);
	virtual GameObject* Clone(void* arg) override;
	virtual void Free() override;
};