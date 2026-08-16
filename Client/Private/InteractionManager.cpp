#include "Client_Define.h"
#include "InteractionManager.h"
#include "IInteractable.h"
#include "GameObject.h"



IMPLEMENT_SINGLETON(InteractionManager);

Client::InteractionManager::InteractionManager()
{
}

Client::InteractionManager::~InteractionManager()
{
}

HRESULT Client::InteractionManager::Initialize()
{
	m_pGameInstance = GameInstance::GetInstance();
	Load_SavePointData("../../DataFiles/Level_All/MapData_SP.json");

	return S_OK;
}
void Client::InteractionManager::Register_InteractableObject(IInteractable* pObj)
{
	INTERACTION_TYPE eType = pObj->Get_InteractionType();
	auto vecIter = m_InteractionMap.find(eType);
	if (vecIter != m_InteractionMap.end())
	{
		//원래 있던 벡터에넣을경우..
		vecIter->second.push_back(pObj);
	}


	else
	{
		INTERACTION_TYPE eType = pObj->Get_InteractionType();


		//새로운벡터에 넣을경우
		vector<IInteractable*>	NewVector;
		NewVector.push_back(pObj);

		m_InteractionMap.emplace(eType, NewVector);
	}

}

void Client::InteractionManager::UnRegisterInteractable(IInteractable* pObj)
{
	INTERACTION_TYPE eType = pObj->Get_InteractionType();
	auto iter = m_InteractionMap.find(eType);
	if(iter!= m_InteractionMap.end())
	{
		for (auto vecIter = iter->second.begin(); vecIter != iter->second.end(); ++vecIter)
		{
			if (*vecIter == pObj)
			{
				iter->second.erase(vecIter);
				return;
			}
		}
	}

}

void Client::InteractionManager::Update(const _float& fTimeDelta)
{
	CHECK_JUST_NULL(m_pMainPlayer);
	CHECK_TRUE(m_InteractionMap.empty());
	CHECK_FALSE(m_bEnable);


	//최적의 InteratableOBject를 찾아서 저장
	IInteractable* pBest = nullptr;
	_vector vPlayerPos = m_pMainPlayer->Get_Position();
	fMinDistSq = FLT_MAX;




	//전체 리스트를 돌면서 상호작용가능한 조건을 가지는 애들을 간추리기
	for (auto& pair : m_InteractionMap)
	{
		for (auto& pObj : pair.second)
		{
			//상호작용 가능 여부체크
			bool bInteractable = pObj->IsInteractable();

			if (!bInteractable)
			{
				// 현재는 상호작용이 불가능한데,이전에 범위 안에 있었다면
				// 강제로 Exit 시키고 상태를 초기화
				if (pObj->m_bPrevRange)
				{
					pObj->Exit_InteractionRange(m_pMainPlayer);
					pObj->m_bPrevRange = false;
				}
				continue; //다음 오브젝트로 넘기기
			}

			// 상호작용 가능한 객체만 거리 계산 수행
			XMVECTOR vObjPos = pObj->Get_Position();
			
			//수평 거리만따지기 일단.
			XMVECTOR vDist = XMVector3LengthSq(XMVectorSetY(vPlayerPos,0.f) - XMVectorSetY(vObjPos,0.f));
			float fDistSq = XMVectorGetX(vDist);

			float fRange = pObj->Get_InteractionRange();

			bool bInRange = (fDistSq <= (fRange * fRange));

			if (!pObj->m_bPrevRange && bInRange)
				pObj->Enter_InteractionRange(m_pMainPlayer);
			else if (pObj->m_bPrevRange && bInRange)
				pObj->Stay_InteractionRange(m_pMainPlayer,fTimeDelta);
			else if (pObj->m_bPrevRange && !bInRange)
			{
				pObj->Exit_InteractionRange(m_pMainPlayer);
				iBestPriority = (int)INTERACTION_TYPE::END;
			}
			pObj->m_bPrevRange = bInRange;

			//타겟 선정
			if (bInRange) 
			{
				int iCurrentPriority = (int)pObj->Get_InteractionType(); 
		
				if (iCurrentPriority < iBestPriority ||
					(iCurrentPriority == iBestPriority && fDistSq < fMinDistSq))
				{
					pBest = pObj;
					iBestPriority = iCurrentPriority;
					fMinDistSq = fDistSq;
				}
			}
		}
	}

	m_pCurrent = pBest;
	if (m_pCurrent && m_pCurrent->m_bPreInteraction)
		m_pCurrent->Stay_Interaction(m_pMainPlayer,fTimeDelta);


	//검출한 상호작용물체에 대한 처리
	CHECK_JUST_NULL(m_pCurrent);

	//Exit안된애들잉쓰면 알아서 처리하도록.
	for (auto& pair : m_InteractionMap)
	{
		for (auto& pObj : pair.second)
		{

			//다른객체의 interaction exit가호출되지 않았다면, 자동으로 호출해주는함수.
			if (pObj->m_bPreInteraction && pObj != m_pCurrent)
			{
				iBestPriority = (int)INTERACTION_TYPE::END;
				pObj->Exit_Interaction(m_pMainPlayer);
				pObj->m_bPreInteraction = false;

			}
		}
	}
}

bool Client::InteractionManager::OnPressedInteractionKey()
{
	CHECK_NULL_RESULT(m_pMainPlayer, false);
	CHECK_NULL_RESULT(m_pCurrent,false);

	if (m_pGameInstance->KeyDown(DIK_E))
	{
		//아이템 타입도 알아야하긴해
		//아이템인지 / 박스인지 / 사다리인지 등등에 따라서 플레이어 상태를 바꿔주고

		if (!m_pCurrent->m_bPreInteraction)
		{
			m_pCurrent->m_bPreInteraction = true;
			m_pCurrent->Enter_Interaction(m_pMainPlayer);

			return true;
		}

	
			
	}
	 
	return false;
}

void Client::InteractionManager::Clear_InteractionManager()
{
	for (auto& pair : m_InteractionMap)
	{
		pair.second.clear();

	}
	m_pCurrent = nullptr;
	m_pMainPlayer = nullptr;
}

void Client::InteractionManager::Set_Enable(bool b)
{
	CHECK_TRUE(m_bLock);

	m_bEnable = b;
	if (!b)
	{
		if (m_pCurrent)
		{
			iBestPriority= (int)INTERACTION_TYPE::END;
			m_pCurrent->Exit_Interaction(m_pMainPlayer);
			
		}
			

	}
}


void Client::InteractionManager::Set_LastSavePointInfo(LEVEL eLevel, int iIdx)
{
	auto iter = m_SavePoints.find(eLevel);
	if (iter != m_SavePoints.end())
	{
		auto seconditer = iter->second.find(iIdx);
		if (seconditer != iter->second.end())
			m_LastSaveInfo = seconditer->second;

		
	}

}

SAVE_POINT_INFO* Client::InteractionManager::Get_SavePointInfo(LEVEL eLevel, int iIdx)
{
	auto iter = m_SavePoints.find(eLevel);
	if (iter != m_SavePoints.end())
	{

		auto seconditer = iter->second.find(iIdx);
		if(seconditer!=iter->second.end())
			return seconditer->second;

		
	}

	return nullptr;

}

void Client::InteractionManager::Finishi_Interaction()
{
	CHECK_TRUE(m_pCurrent == nullptr);
	CHECK_TRUE(m_pMainPlayer == nullptr);

	//초기화..

	CHECK_FALSE(m_pCurrent->Get_InteractionType() != INTERACTION_TYPE::SAVEPOINT);


	m_pCurrent->Exit_Interaction(m_pMainPlayer); 
	m_pCurrent->m_bPreInteraction = false;
	m_pCurrent->m_bPrevRange = false;

	iBestPriority = (int)INTERACTION_TYPE::END;
	m_pCurrent = nullptr;

}

LEVEL Client::InteractionManager::Get_Level_By_MapType(MAP_TYPE eType)
{
	switch (eType)
	{
	case MAP_TYPE::NONE:
		return LEVEL::END;
		break;
	case MAP_TYPE::ST00_BASE:
		return LEVEL::BASE;
		break;

	case MAP_TYPE::ST01_UNDER:
		return LEVEL::MAIN;
		break;

	case MAP_TYPE::ST02_CHURCH:
		return LEVEL::CHURCH;
		break;
	default:
		break;
	}
	return LEVEL();
}

HRESULT Client::InteractionManager::Load_SavePointData(string path)
{
	ifstream file(path);
	if (!file.is_open())
		return E_FAIL;

	ordered_json root = ordered_json::parse(file);
	for (auto& SavePointData : root["data"])
	{
		SAVE_POINT_INFO* Info=new SAVE_POINT_INFO();


		Info->mapType = (MAP_TYPE)SavePointData["ExtraData"]["MyMapType"];//자신이 속한 savepoint맵
		Info->targetLevel = Get_Level_By_MapType(Info->mapType);

		auto& matrixArray = SavePointData["WorldMatrix"];
		_float3 vPosition = _float3(matrixArray[12], matrixArray[13], matrixArray[14]);

		//자신 위치에서 x(+-0.08, z +-0.03)
		Info->spawnPosition = _float4(vPosition.x + m_pGameInstance->RandomValue(-3.f, 3.f),
			vPosition.y,
			vPosition.z,1.f);


		Info->iIndex = SavePointData["ExtraData"]["MyIndex"];
		Info->spawnRotation = _float4(0.f, 0.f, 0.f, 0.f);

		auto iter = m_SavePoints.find(Info->targetLevel);
		if (iter != m_SavePoints.end())
		{
			map<int, SAVE_POINT_INFO*>& mapInfo = iter->second;
			mapInfo[Info->iIndex] = Info;


		}

		else
		{
			map<int, SAVE_POINT_INFO*> InnerMap;
			InnerMap[Info->iIndex] = Info;

			m_SavePoints.emplace(Info->targetLevel, InnerMap);
		}


	}


	Set_LastSavePointInfo(LEVEL::MAIN, 1);
	return S_OK;


}

void Client::InteractionManager::Free()
{
	Clear_InteractionManager();

	for (auto& pair : m_SavePoints)
	{
		for (auto& Info : pair.second)
		{
			if (Info.second)
				Safe_Delete(Info.second);
		}

	}
	m_SavePoints.clear();

	__super::Free();
}
