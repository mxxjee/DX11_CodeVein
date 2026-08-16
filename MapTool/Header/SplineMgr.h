#pragma once
#include "Base.h"
#include "SplinePath.h"

using namespace DirectX::SimpleMath;

struct MONSTER_PATH_DATA
{
	class SplinePath* pOwnerPath = nullptr;
	_float  fCurrentT = 0.f;        // 진행도
	_bool   bIsWandering = false;   // 여부
	Vector3 vWanderTarget = Vector3::Zero; // 목표
	_float  fWanderTime = 0.f;      // 타이머
};

class SplineMgr final : public Base
{
	DECLARE_SINGLETON(SplineMgr)

private:
	SplineMgr();
	virtual ~SplineMgr() = default;

public:
	HRESULT Ready_SplineMgr();
	void    Update(const _float fTimeDelta);
	void Process_Monster(const _float fDT, class CMonster* pMonster);
	void Update_Zone_Wander(const _float fDT, class CMonster* pMonster, MONSTER_PATH_DATA& data, const SplineNode* pNode);
	void Move_On_Spline(const _float fDT, class CMonster* pMonster, MONSTER_PATH_DATA& data, _float fDirection, _float fSpeedMult);
	_float Get_Closest_T_On_Spline(Vector3 vTargetPos);
	void    Render(PrimitiveBatch<VertexPositionColor>* batch);
	void    HandleInput(const _float fTimeDelta);

	void ResetPath();
	void ToggleLoop();

	void RemoveLastPoint();
	void DeletePoint(int index);

	void SetPointType(int index, POINT_TYPE type);
	void SetPointRadius(int index, float radius);

	void Save_Spline(const string& strFileName);
	void Load_Spline(const string& strFileName);

	void    Set_Speed(_float speed) { m_fSpeed = speed; }
	_float  Get_Speed() const { return m_fSpeed; }

	_int    Get_PointCount() const;
	_bool   Is_Loop() const { return m_bIsLoop; }

	_int Get_PathCount() const { return (_int)m_PathList.size(); }
	void Select_Path(_int iIndex)
	{
		if (iIndex >= 0 && iIndex < m_PathList.size())
		{
			m_pCurrentPath = m_PathList[iIndex];
		}
	}

	const vector<SplineNode>& Get_Nodes(_int iIndex) const;

	_float Get_Distance_To_Path(SplinePath* pPath, Vector3 vPos, _float* outT);

	const SplineNode* Get_NodeInfo(_uint index);

	virtual void Free() override;

private:

private:
	vector<SplinePath*> m_PathList;
	SplinePath* m_pCurrentPath;

	const _string m_strDataPath = "../../DataFiles/Map_Data/LineData/";
	_bool           m_bIsLoop = false;
	_float          m_fSpeed = 5.0f;        // 이동 속도
	_float          m_fInputCooldown = 0.f; // 입력 쿨타임

	map<class CMonster*, MONSTER_PATH_DATA> m_mapMonsterData;
};