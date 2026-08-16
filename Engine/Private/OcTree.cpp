#include "Engine_Define.h"
#include "OcTree.h"
#include "GameInstance.h"
#include "GameObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::OcTree::OcTree()
	: m_pGameInstance{ GameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

Engine::OcTree::~OcTree()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 초기화 함수 ////////////////////////////////////////////////////////
HRESULT Engine::OcTree::Initialize(const _float3& _center, _float _halfSize, _uint _maxDepth, _uint _currentDepth)
{
	m_vCenter = _center;
	m_fHalfSize = _halfSize;
	m_iMaxDepth = _maxDepth;
	m_iCurrentDepth = _currentDepth;

	/* 노드의 AABB 설정 */
	m_BoundingBox.Center = m_vCenter;
	m_BoundingBox.Extents = _float3(_halfSize, _halfSize, _halfSize);

	return S_OK;
}
/******************************************************* 초기화 함수 *******************************************************/



//////////////////////////////////////////////////////// 오브젝트 삽입 함수 ////////////////////////////////////////////////////////
_bool Engine::OcTree::Insert(const STATIC_OBJ_INFO* _info)
{
	/* _Info의 Object AABB가 이 노드 영역과 겹치지 않으면 실패 */
	if (!Intersects(_info->tAABB))
		return false;

	/* 리프 노드이거나 최대 깊이 도달 */
	if (m_pChildren[0] == nullptr)
	{
		/* 아직 분할 전이고, 오브젝트 수가 임계값 이하면 여기에 저장 */
		if (m_vecObjectInfos.size() < MAX_OBJECTS_PER_NODE || m_iCurrentDepth >= m_iMaxDepth)
		{
			m_vecObjectInfos.push_back(_info);
			// Safe_AddRef는 Renderer가 이미 들고 있으니 여기서 굳이 안 해도 됨 (선택사항)
			return true;
		}

		/* 임계값 초과 → 분할 */
		Subdivide();

		/* 기존 오브젝트들을 자식으로 재분배 및 재배치 */
		vector<const STATIC_OBJ_INFO*> tempInfos = std::move(m_vecObjectInfos);
		m_vecObjectInfos.clear();

		for (auto& pInfo : tempInfos)
		{
			_int index = Get_OctantIndex(pInfo->tAABB);
			if (index != -1) m_pChildren[index]->Insert(pInfo);
			else             m_vecObjectInfos.push_back(pInfo);
		}
	}

	/* 자식이 있으면 적절한 자식에 삽입 시도 */
	_int index = Get_OctantIndex(_info->tAABB);
	if (index != -1) return m_pChildren[index]->Insert(_info);

	/* 여러 자식에 걸치면 현재 노드에 저장 */
	m_vecObjectInfos.push_back(_info);
	return true;
}
/******************************************************* 오브젝트 삽입 함수 *******************************************************/





//////////////////////////////////////////////////////// 오브젝트 제거 함수 ////////////////////////////////////////////////////////
_bool Engine::OcTree::Remove(const STATIC_OBJ_INFO* _pInfo)
{
	/* 현재 노드에서 찾기 (주소값 비교) */
	for (auto iter = m_vecObjectInfos.begin(); iter != m_vecObjectInfos.end(); ++iter)
	{
		if (*iter == _pInfo)
		{
			m_vecObjectInfos.erase(iter);
			return true;
		}
	}

	/* 자식 노드에서 찾기 */
	// 만약 자식이 없다면(리프 노드라면) 더 이상 찾을 필요 없음
	if (m_pChildren[0] == nullptr)
		return false;

	_int index = Get_OctantIndex(_pInfo->tAABB);

	if (index != -1)
	{
		// 특정 구역에 완전히 포함된다면 그 자식만 뒤지면 됨
		return m_pChildren[index]->Remove(_pInfo);
	}
	else
	{
		// 걸쳐있는 경우라면 모든 자식을 다 뒤져봐야 함 (혹시 모를 예외 처리)
		// 보통 걸쳐있으면 부모(현재 노드)에 있어야 정상이지만, 
		// AABB가 변했거나 하는 경우를 대비해 전체 검색
		for (_uint i = 0; i < OCT_END; ++i)
		{
			if (m_pChildren[i]->Remove(_pInfo))
				return true;
		}
	}

	return false;
}
/******************************************************* 오브젝트 제거 함수 *******************************************************/



//////////////////////////////////////////////////////// 절두체 쿼리 함수 ////////////////////////////////////////////////////////
void Engine::OcTree::Query_Frustum(vector<const STATIC_OBJ_INFO*>& _outInfos) const
{
	/* 이 노드가 절두체와 겹치는지 확인 */
	if (!m_pGameInstance->IsIn_Frustum_AABB(m_BoundingBox))
		return;

	/* 현재 노드의 오브젝트들 검사 */
	for (const auto* pInfo : m_vecObjectInfos)
	{
		// 이미 가지고 있는 AABB로 검사
		if (m_pGameInstance->IsIn_Frustum_AABB(pInfo->tAABB))
		{
			_outInfos.push_back(pInfo); // Lookup 없이 정보 통째로 담기
		}
	}

	/* 자식 노드들 재귀 검사 */
	for (_uint i = 0; i < OCT_END; ++i)
	{
		if (m_pChildren[i]) m_pChildren[i]->Query_Frustum(_outInfos);
	}
}
/******************************************************* 절두체 쿼리 함수 *******************************************************/



//////////////////////////////////////////////////////// AABB 쿼리 함수 ////////////////////////////////////////////////////////
void Engine::OcTree::Query_AABB(const BoundingBox& _aabb, vector<const STATIC_OBJ_INFO*>& _outInfos) const
{
	/* 이 노드가 쿼리 AABB와 겹치는지 확인 */
	if (!m_BoundingBox.Intersects(_aabb))
		return;

	/* 현재 노드의 오브젝트들 검사 */
	for (const auto* pInfo : m_vecObjectInfos)
	{
		if (pInfo->tAABB.Intersects(_aabb))
		{
			_outInfos.push_back(pInfo);
		}
	}

	/* 자식 노드들 재귀 검사 */
	if (m_pChildren[0] != nullptr)
	{
		for (_uint i = 0; i < OCT_END; ++i)
		{
			m_pChildren[i]->Query_AABB(_aabb, _outInfos);
		}
	}
}

FRUSTUM_RESULT Engine::OcTree::TestAABB_Frustum(const BoundingBox& _aabb, const FrustumPlanes& _frustum)
{
	_float3 vMin;
	vMin.x = _aabb.Center.x - _aabb.Extents.x;
	vMin.y = _aabb.Center.y - _aabb.Extents.y;
	vMin.z = _aabb.Center.z - _aabb.Extents.z;

	_float3 vMax;
	vMax.x = _aabb.Center.x + _aabb.Extents.x;
	vMax.y = _aabb.Center.y + _aabb.Extents.y;
	vMax.z = _aabb.Center.z + _aabb.Extents.z;

	_bool fullyInside = true;

	for (_uint i = 0; i < 6; ++i)
	{
		_float nx = _frustum.planes[i].x;
		_float ny = _frustum.planes[i].y;
		_float nz = _frustum.planes[i].z;
		_float nd = _frustum.planes[i].w;

		/* P-Vertex: 법선 방향으로 가장 먼 꼭짓점 (dot 최대) */
		_float pDist =
			nx * ((nx >= 0.f) ? vMax.x : vMin.x) +
			ny * ((ny >= 0.f) ? vMax.y : vMin.y) +
			nz * ((nz >= 0.f) ? vMax.z : vMin.z) + nd;

		/* N-Vertex: 법선 반대 방향으로 가장 먼 꼭짓점 (dot 최소) */
		_float nDist =
			nx * ((nx >= 0.f) ? vMin.x : vMax.x) +
			ny * ((ny >= 0.f) ? vMin.y : vMax.y) +
			nz * ((nz >= 0.f) ? vMin.z : vMax.z) + nd;

		/* N-Vertex가 밖(양수)이면 AABB 전체가 밖 */
		if (nDist > 0.f)
			return FRUSTUM_RESULT::OUTSIDE;

		/* P-Vertex가 밖(양수)이면 이 평면에 걸쳐있음 */
		if (pDist >= 0.f)
			fullyInside = false;
	}

	return fullyInside ? FRUSTUM_RESULT::FULLY_INSIDE : FRUSTUM_RESULT::INTERSECT;
}
/******************************************************* AABB 쿼리 함수 *******************************************************/



//////////////////////////////////////////////////////// 유틸리티 함수 ////////////////////////////////////////////////////////
_uint Engine::OcTree::Get_TotalObjectCount() const
{
	_uint count = (_uint)m_vecObjectInfos.size();

	if (m_pChildren[0] != nullptr)
	{
		for (_uint i = 0; i < OCT_END; ++i)
		{
			count += m_pChildren[i]->Get_TotalObjectCount();
		}
	}

	return count;
}

void Engine::OcTree::Clear()
{
	/* 현재 노드의 오브젝트 목록 비우기 */
	// Gameobject의 객체는 Renderer가 담당하기 때문에 Safe_Release 안 해줌
	m_vecObjectInfos.clear();

	/* 자식 노드들 재귀 정리 및 해제 */
	if (m_pChildren[0] != nullptr)
	{
		for (_uint i = 0; i < OCT_END; ++i)
		{
			if (m_pChildren[i])
			{
				m_pChildren[i]->Clear(); // 자식의 자식도 클리어
				Safe_Release(m_pChildren[i]); // 자식 노드 자체를 메모리 해제
				m_pChildren[i] = nullptr;
			}
		}
	}
}
/******************************************************* 유틸리티 함수 *******************************************************/



//////////////////////////////////////////////////////// 내부 함수 ////////////////////////////////////////////////////////
void Engine::OcTree::Subdivide()
{
	_float childHalfSize = m_fHalfSize * 0.5f;
	_float offset = childHalfSize; // 중심에서 자식 중심까지의 거리

	/* 8개 자식 노드의 중심점 계산 */
	// Y가 위쪽인 좌표계 가정 (DirectX)
	_float3 childCenters[OCT_END] =
	{
		// 하단 (Y - offset)
		_float3(m_vCenter.x - offset, m_vCenter.y - offset, m_vCenter.z - offset),  // 0: LBN (Left-Bottom-Near)
		_float3(m_vCenter.x + offset, m_vCenter.y - offset, m_vCenter.z - offset),  // 1: RBN
		_float3(m_vCenter.x - offset, m_vCenter.y - offset, m_vCenter.z + offset),  // 2: LBF
		_float3(m_vCenter.x + offset, m_vCenter.y - offset, m_vCenter.z + offset),  // 3: RBF

		// 상단 (Y + offset)
		_float3(m_vCenter.x - offset, m_vCenter.y + offset, m_vCenter.z - offset),  // 4: LTN
		_float3(m_vCenter.x + offset, m_vCenter.y + offset, m_vCenter.z - offset),  // 5: RTN
		_float3(m_vCenter.x - offset, m_vCenter.y + offset, m_vCenter.z + offset),  // 6: LTF
		_float3(m_vCenter.x + offset, m_vCenter.y + offset, m_vCenter.z + offset),  // 7: RTF
	};

	for (_uint i = 0; i < OCT_END; ++i)
	{
		m_pChildren[i] = Engine::OcTree::Create(childCenters[i], childHalfSize, m_iMaxDepth);
		if (m_pChildren[i])
		{
			m_pChildren[i]->m_iCurrentDepth = m_iCurrentDepth + 1;
		}
	}
}

_int Engine::OcTree::Get_OctantIndex(const BoundingBox& _aabb) const
{
	_int index = 0;

	// AABB의 중심점이 아니라 전체 영역이 자식 노드 안에 완전히 포함되는지 확인해야 함
	_float3 aabbMin = _float3(_aabb.Center.x - _aabb.Extents.x, _aabb.Center.y - _aabb.Extents.y, _aabb.Center.z - _aabb.Extents.z);
	_float3 aabbMax = _float3(_aabb.Center.x + _aabb.Extents.x, _aabb.Center.y + _aabb.Extents.y, _aabb.Center.z + _aabb.Extents.z);

	// X축 판별
	if (aabbMin.x >= m_vCenter.x)      index |= 1; // 오른쪽 (+X)
	else if (aabbMax.x <= m_vCenter.x) index |= 0; // 왼쪽 (-X)
	else return -1; // X축 중간에 걸침

	// Y축 판별 (Z축이 깊이인 경우, Y축이 높이)
	if (aabbMin.y >= m_vCenter.y)      index |= 4; // 위쪽 (+Y)
	else if (aabbMax.y <= m_vCenter.y) index |= 0; // 아래쪽 (-Y)
	else return -1; // Y축 중간에 걸침

	// Z축 판별
	if (aabbMin.z >= m_vCenter.z)      index |= 2; // 뒤쪽 (+Z)
	else if (aabbMax.z <= m_vCenter.z) index |= 0; // 앞쪽 (-Z)
	else return -1; // Z축 중간에 걸침

	return index;
}

_bool Engine::OcTree::Intersects(const BoundingBox& _aabb) const
{
	return m_BoundingBox.Intersects(_aabb);
}
/******************************************************* 내부 함수 *******************************************************/





/* 스택 기반 Fully Inside 전파 직접 평면 테스트 */
void Engine::OcTree::Query_Frustum_Optimized(vector<const STATIC_OBJ_INFO*>& _outInfos, const FrustumPlanes& _frustum) const
{
    struct StackEntry
    {
        const OcTree* pNode;
        _bool bParentFullyInside; // 부모가 완전히 안에 있으면 true
		// 부모 옥트리가 전부 화면에 들어와있는데 굳이 자식 옥트리도 검사할 필요 없기때문
    };

    /* thread_local로 매 프레임 할당 방지 */
    static thread_local vector<StackEntry> stack;
    stack.clear();
    stack.push_back({ this, false });

    while (!stack.empty())
    {
        StackEntry entry = stack.back();
        stack.pop_back();

        const OcTree* pNode = entry.pNode;
        _bool skipFrustumTest = entry.bParentFullyInside;

        /* 부모가 fully inside가 아니면 이 노드 테스트 */
        FRUSTUM_RESULT nodeResult = FRUSTUM_RESULT::FULLY_INSIDE;
        if (!skipFrustumTest)
        {
            nodeResult = TestAABB_Frustum(pNode->m_BoundingBox, _frustum);
            if (nodeResult == FRUSTUM_RESULT::OUTSIDE)
                continue; // 서브트리 전체 스킵
        }

        _bool childrenFullyInside = (nodeResult == FRUSTUM_RESULT::FULLY_INSIDE);

        /* 현재 노드의 오브젝트 처리 */
        if (childrenFullyInside)
        {
            /* 완전히 안이면 개별 AABB 테스트 불필요 전부 추가 */
            for (const auto* pInfo : pNode->m_vecObjectInfos)
                _outInfos.push_back(pInfo);
        }
        else
        {
            /* 걸쳐있으면 개별 테스트 */
            for (const auto* pInfo : pNode->m_vecObjectInfos)
            {
                if (TestAABB_Frustum(pInfo->tAABB, _frustum) != FRUSTUM_RESULT::OUTSIDE)
                    _outInfos.push_back(pInfo);
            }
        }

        /* 자식 노드 스택에 추가 */
        if (pNode->m_pChildren[0] != nullptr)
        {
            for (_uint i = 0; i < OCT_END; ++i)
            {
                if (pNode->m_pChildren[i])
                    stack.push_back({ pNode->m_pChildren[i], childrenFullyInside });
            }
        }
    }
}



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
OcTree* Engine::OcTree::Create(const _float3& _center, _float _halfSize, _uint _maxDepth)
{
	OcTree* pInstance = new OcTree();

	if (FAILED(pInstance->Initialize(_center, _halfSize, _maxDepth)))
	{
		MSG_BOX("Failed to Created : OcTree");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::OcTree::Free()
{
	__super::Free();

	m_vecObjectInfos.clear();

	/* 자식 노드 해제 */
	for (_uint i = 0; i < OCT_END; ++i)
	{
		Safe_Release(m_pChildren[i]);
	}

	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/