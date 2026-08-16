#include "MT_Defines.h"
#include "SplineMgr.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Monster.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(SplineMgr)

SplineMgr::SplineMgr()
{
	srand(static_cast<unsigned int>(time(NULL)));
	m_pCurrentPath = nullptr;
}

HRESULT SplineMgr::Ready_SplineMgr()
{
	return S_OK;
}

void SplineMgr::HandleInput(const _float fTimeDelta)
{
	GameInstance* pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_fInputCooldown > 0.f)
		m_fInputCooldown -= fTimeDelta;

	// 점 등록
	if (pGameInstance->MouseDown(MOUSEKEYSTATE::LB) && m_fInputCooldown <= 0.f)
	{
		_float4 vPickPos;
		if (pGameInstance->PickingObject(&vPickPos))
		{
			if (m_pCurrentPath == nullptr)
			{
				m_pCurrentPath = new SplinePath();
				m_PathList.push_back(m_pCurrentPath);

				m_pCurrentPath->SetIsLoop(m_bIsLoop);
			}

			Vector3 hitPos(vPickPos.x, vPickPos.y, vPickPos.z);
			SplineNode tNode;
			tNode.vPos = hitPos;
			tNode.eType = POINT_TYPE::PATH;
			tNode.fRadius = 0.f;

			m_pCurrentPath->AddPoint(tNode);
			m_fInputCooldown = 0.2f;
		}
	}

	// 점 취소
	if (pGameInstance->MouseDown(MOUSEKEYSTATE::RB) && m_fInputCooldown <= 0.f)
	{
		if(m_pCurrentPath)
			RemoveLastPoint();
		m_fInputCooldown = 0.2f;
	}

	// 마지막 점 공터 설정
	if (pGameInstance->Get_DIKeyState(DIK_Z) & 0x80 && m_fInputCooldown <= 0.f)
	{
		int lastIdx = Get_PointCount() - 1;
		if (lastIdx >= 0)
		{
			SetPointType(lastIdx, POINT_TYPE::ZONE);
			SetPointRadius(lastIdx, 5.0f);
		}
		m_fInputCooldown = 0.2f;
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80 && m_fInputCooldown <= 0.f)
	{
		m_pCurrentPath = nullptr;
		m_fInputCooldown = 0.5f;
	}

	Safe_Release(pGameInstance);
}

void SplineMgr::Update(const _float fTimeDelta)
{
	if (m_PathList.empty()) return;

	GameInstance* pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Layer* pLayer = pGameInstance->Get_Layer(TEXT("Layer_Monster"));
	if (pLayer)
	{
		auto& gameObjects = pLayer->Get_GameObjects();

		for (auto& pair : gameObjects)
		{
			CMonster* pMonster = dynamic_cast<CMonster*>(pair.second);
			if (!pMonster || pMonster->Is_Dead())
			{
				if (pMonster) m_mapMonsterData.erase(pMonster);
				continue;
			}

			// 몬스터 데이터가 없거나, 소속된 Path가 유효하지 않을 때 초기화
			bool bNeedInit = (m_mapMonsterData.find(pMonster) == m_mapMonsterData.end());
			if (!bNeedInit && m_mapMonsterData[pMonster].pOwnerPath == nullptr) bNeedInit = true;

			if (bNeedInit)
			{
				Transform* pTr = static_cast<Transform*>(pMonster->Get_Component_FromName(Com_Transform));
				if (pTr)
				{
					Vector3 vPos = pTr->Get_State(DIRECTION::POSITION);

					// **가장 가까운 트랙과 T값 찾기**
					SplinePath* pBestPath = nullptr;
					_float fBestT = 0.f;
					_float fMinDistSq = FLT_MAX;

					for (auto& path : m_PathList)
					{
						if (path->GetPointCount() < 2) continue;

						_float t = 0.f;
			
						_float distSq = Get_Distance_To_Path(path, vPos, &t);

						if (distSq < fMinDistSq)
						{
							fMinDistSq = distSq;
							pBestPath = path;
							fBestT = t;
						}
					}

					if (pBestPath)
					{
						MONSTER_PATH_DATA newData;
						newData.pOwnerPath = pBestPath; // 찾은 트랙 등록
						newData.fCurrentT = fBestT;
						newData.bIsWandering = false;
						newData.vWanderTarget = Vector3::Zero;
						m_mapMonsterData[pMonster] = newData;
					}
				}
			}
			
			if (m_mapMonsterData.find(pMonster) != m_mapMonsterData.end() &&
				m_mapMonsterData[pMonster].pOwnerPath != nullptr)
			{
				Process_Monster(fTimeDelta, pMonster);
			}
		}
	}
	Safe_Release(pGameInstance);
}

void SplineMgr::Process_Monster(const _float fDT, CMonster* pMonster)
{
	MONSTER_PATH_DATA& data = m_mapMonsterData[pMonster];

	Move_On_Spline(fDT, pMonster, data, 1.0f, 1.0f);
}

void SplineMgr::Update_Zone_Wander(const _float fDT, CMonster* pMonster, MONSTER_PATH_DATA& data, const SplineNode* pNode)
{
}

void SplineMgr::Move_On_Spline(const _float fDT, CMonster* pMonster, MONSTER_PATH_DATA& data, _float fDirection, _float fSpeedMult)
{
	SplinePath* pTargetTrack = data.pOwnerPath;
	if (!pTargetTrack || pTargetTrack->GetPointCount() < 2) return;

	_float fTotalLen = pTargetTrack->GetTotalLength();
	if (fTotalLen <= 0.001f) return;

	_float tIncrement = (m_fSpeed * fSpeedMult / fTotalLen) * (_float)pTargetTrack->GetPointCount() * fDT * fDirection;
	data.fCurrentT += tIncrement;

	_float fMaxT = (_float)(pTargetTrack->GetPointCount() - 1);

	if (m_bIsLoop)
	{
		if (data.fCurrentT > fMaxT) data.fCurrentT -= fMaxT;
		else if (data.fCurrentT < 0.f) data.fCurrentT += fMaxT;
	}
	else
	{
		if (data.fCurrentT >= fMaxT)
		{
			data.fCurrentT = 0.0f;
		}
		else if (data.fCurrentT < 0.f)
		{
			data.fCurrentT = 0.f;
		}
	}

	Vector3 vTargetPos = pTargetTrack->GetPositionOnSpline(data.fCurrentT);

	Transform* pTr = static_cast<Transform*>(pMonster->Get_Component_FromName(Com_Transform));
	Vector3 vMonsterPos = pTr->Get_State(DIRECTION::POSITION);

	Vector3 vDir = vTargetPos - vMonsterPos;
	vDir.y = 0.f;

	//if (vDir.LengthSquared() > 0.01f)
	//{
	//	vDir.Normalize();
	//	pMonster->Set_NavDir(vDir);
	//}
	//else
	//{

	//	// pMonster->Set_NavDir(Vector3::Zero); 

	//	Vector3 vFuturePos = pTargetTrack->GetPositionOnSpline(data.fCurrentT + 0.1f);
	//	Vector3 vFlowDir = vFuturePos - vTargetPos;
	//	vFlowDir.y = 0.f;
	//	vFlowDir.Normalize();
	//	pMonster->Set_NavDir(vFlowDir);
	//}
}

_float SplineMgr::Get_Closest_T_On_Spline(Vector3 vTargetPos)
{
	return _float();
}

//_float SplineMgr::Get_Closest_T_On_Spline(Vector3 vTargetPos)
//{
//	if (!m_pPath) return 0.f;
//
//	_float fBestT = 0.f;
//	_float fMinDistSq = FLT_MAX;
//
//	_int iSamples = m_pPath->GetPointCount() * 10;
//	if (iSamples == 0) return 0.f;
//
//	for (int i = 0; i <= iSamples; ++i)
//	{
//		_float t = (_float)i / (_float)iSamples * (_float)(m_pPath->GetPointCount() - 1);
//		Vector3 vPoint = m_pPath->GetPositionOnSpline(t);
//
//		_float fDistSq = Vector3::DistanceSquared(vPoint, vTargetPos);
//		if (fDistSq < fMinDistSq)
//		{
//			fMinDistSq = fDistSq;
//			fBestT = t;
//		}
//	}
//	return fBestT;
//}

const std::vector<SplineNode>& SplineMgr::Get_Nodes(_int iIndex) const
{
	if (iIndex >= 0 && iIndex < m_PathList.size())
	{
		if (m_PathList[iIndex])
		{
			return m_PathList[iIndex]->Get_Nodes();
		}
	}
	static vector<SplineNode> empty;
	return empty;
}

_float SplineMgr::Get_Distance_To_Path(SplinePath* pPath, Vector3 vPos, _float* outT)
{
    if (!pPath) return FLT_MAX;
    
    _float fBestT = 0.f;
    _float fMinDistSq = FLT_MAX;
    _int iSamples = pPath->GetPointCount() * 10; 

    for (int i = 0; i <= iSamples; ++i)
    {
        _float t = (_float)i / (_float)iSamples * (_float)(pPath->GetPointCount() - 1);
        Vector3 vPoint = pPath->GetPositionOnSpline(t);

        _float fDistSq = Vector3::DistanceSquared(vPoint, vPos);
        if (fDistSq < fMinDistSq)
        {
            fMinDistSq = fDistSq;
            fBestT = t;
        }
    }

    if (outT) *outT = fBestT;
    return fMinDistSq;
}

const SplineNode* SplineMgr::Get_NodeInfo(_uint index)
{
	if (!m_pCurrentPath) return nullptr;
	const auto& nodes = m_pCurrentPath->Get_Nodes();
	if (index >= nodes.size()) return nullptr;
	return &nodes[index];
}

void SplineMgr::SetPointType(int index, POINT_TYPE type)
{
	if (!m_pCurrentPath) return;
	m_pCurrentPath->SetPointType(index, type);
}

void SplineMgr::SetPointRadius(int index, float radius)
{
	if (!m_pCurrentPath) return;
	m_pCurrentPath->SetPointRadius(index, radius);
}

void SplineMgr::Render(PrimitiveBatch<VertexPositionColor>* batch)
{
	for (auto& path : m_PathList)
	{
		path->Render(batch);
	}
}

void SplineMgr::RemoveLastPoint()
{
	if (!m_pCurrentPath) return;

	int count = m_pCurrentPath->GetPointCount();
	if (count > 0)
	{
		m_pCurrentPath->RemovePoint(count - 1);
	}
}

void SplineMgr::DeletePoint(int index)
{
	if (!m_pCurrentPath) return;
	m_pCurrentPath->RemovePoint(index);
}

void SplineMgr::ResetPath()
{
	for (auto& path : m_PathList)
	{
		Safe_Delete(path);
	}
	m_PathList.clear();
	m_pCurrentPath = nullptr;

	m_mapMonsterData.clear();
}

//void SplineMgr::Save_Spline(const string& strFileName)
//{
//	for (size_t i = 0; i < m_PathList.size(); ++i)
//	{
//		if (m_PathList[i])
//		{
//			// 확장자 제거 후 인덱스 붙이기 로직이 필요하나, 단순화를 위해 뒤에 붙임
//			string strName = strFileName + to_string(i);
//			m_PathList[i]->SaveToFile(m_strDataPath + strName);
//		}
//	}
//}
//
//void SplineMgr::Load_Spline(const string& strFileName)
//{
//	int i = 0;
//	while (true)
//	{
//		pNewPath->LoadFromFile(m_strDataPath + strFileName);
//		if (pNewPath->GetPointCount() > 0) {
//			m_PathList.push_back(pNewPath);
//		}
//		else {
//			delete pNewPath;
//		}
//		break;
//	}
//	i++;
//
//m_mapMonsterData.clear();
//m_pCurrentPath = nullptr;
//}

void SplineMgr::Save_Spline(const string& strFileName)
{
	std::ofstream outFile(m_strDataPath + strFileName, std::ios::binary);
	if (!outFile.is_open()) return;

	size_t iPathCount = m_PathList.size();
	outFile.write(reinterpret_cast<const char*>(&iPathCount), sizeof(size_t));
	
	for (auto& pPath : m_PathList)
	{
		if (pPath)
		{
			pPath->Save(outFile);
		}
	}

	outFile.close();
}

void SplineMgr::Load_Spline(const string& strFileName)
{
	std::ifstream inFile(m_strDataPath + strFileName, std::ios::binary);
	if (!inFile.is_open()) return;

	ResetPath();

	size_t iPathCount = 0;
	inFile.read(reinterpret_cast<char*>(&iPathCount), sizeof(size_t));

	for (size_t i = 0; i < iPathCount; ++i)
	{
		SplinePath* pNewPath = new SplinePath();

		pNewPath->Load(inFile);

		m_PathList.push_back(pNewPath);
	}

	inFile.close();

	m_pCurrentPath = nullptr;
	m_mapMonsterData.clear();
}

void SplineMgr::ToggleLoop()
{
	m_bIsLoop = !m_bIsLoop;
	if (m_pCurrentPath) m_pCurrentPath->SetIsLoop(m_bIsLoop);
}

_int SplineMgr::Get_PointCount() const
{
	return m_pCurrentPath ? (_int)m_pCurrentPath->GetPointCount() : 0;
}

void SplineMgr::Free()
{
	m_mapMonsterData.clear();

	for (auto& path : m_PathList)
	{
		Safe_Delete(path);
	}
	m_PathList.clear();

	__super::Free();
}