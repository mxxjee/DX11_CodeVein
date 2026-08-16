#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class GameObject;

class OcTree final : public Base
{	
public:
	/* 8개 자식 노드 (3비트: X, Y, Z 부호로 구분) */
	enum OCTANT
	{
		/* 하단 (Y-) */
		OCT_LBN = 0,  // 왼쪽 앞	(-X, -Y, -Z) Left_Bottom_Near
		OCT_RBN,      // 오른쪽 앞  (+X, -Y, -Z)
		OCT_LBF,      // 왼쪽 뒤	(-X, -Y, +Z) Left_Bottom_Far
		OCT_RBF,      // 오른쪽 뒤  (+X, -Y, +Z)
		/* 상단 (Y+) */
		OCT_LTN,      // 왼쪽 앞	(-X, +Y, -Z)
		OCT_RTN,      // 오른쪽 앞  (+X, +Y, -Z)
		OCT_LTF,      // 왼쪽 뒤	(-X, +Y, +Z)
		OCT_RTF,      // 오른쪽 뒤	(+X, +Y, +Z)
		OCT_END
	};

private:
	explicit OcTree();
	virtual ~OcTree();

public:
	HRESULT Initialize(const _float3& _center, _float _halfSize, _uint _maxDepth, _uint _currentDepth = 0);

	/* 오브젝트 삽입/제거 */
	_bool Insert(const STATIC_OBJ_INFO* _info);
	/* 옥트리에서 제거 */
	_bool Remove(const STATIC_OBJ_INFO* _pInfo);

	/* 절두체 컬링: 절두체 안에 있는 오브젝트들을 결과 벡터에 추가 */
	void Query_Frustum(vector<const STATIC_OBJ_INFO*>& _outInfos) const;

	/* 범위 쿼리: 특정 AABB와 겹치는 오브젝트들 반환 */
	void Query_AABB(const BoundingBox& _aabb, vector<const STATIC_OBJ_INFO*>& _outInfos) const;

	/* 전체 오브젝트 수 반환 */
	_uint Get_TotalObjectCount() const;

	/* 트리 초기화 (오브젝트 제거, 구조 유지) */
	void Clear();

	/* Frustum 평면 직접 캐시하여 간접 호출 제거 + Fully Inside 판정 추가 */
	/* OUTSIDE/INTERSECT/FULLY_INSIDE 3-state 판정 */
	static FRUSTUM_RESULT TestAABB_Frustum(const BoundingBox& _aabb, const FrustumPlanes& _frustum);
	

	// OcTree가 절두체에 들어왔다고 판단되면 가지고 있는 모든 Object에게 각각 Render 전파하는게 아니라 한 번에 전파
	void Query_Frustum_Optimized(vector<const STATIC_OBJ_INFO*>& _outInfos, const FrustumPlanes& _frustum) const;


private:
	/* 자식 노드 생성 */
	void Subdivide();

	/* AABB가 어느 Octant에 속하는지 계산 (-1이면 여러 개에 걸침) */
	_int Get_OctantIndex(const BoundingBox& _aabb) const;

	/* 현재 노드의 AABB와 겹치는지 확인 */
	_bool Intersects(const BoundingBox& _aabb) const;

private:
	class GameInstance* m_pGameInstance = { nullptr };

	/* 노드 영역 정보 */
	_float3 m_vCenter = {};       // 노드 중심점
	_float m_fHalfSize = {};      // 노드 반크기 (정육면체 가정)
	BoundingBox m_BoundingBox = {};  // 노드의 AABB

	/* 트리 구조 */
	OcTree* m_pChildren[OCT_END] = { nullptr };
	_uint m_iMaxDepth = {};
	_uint m_iCurrentDepth = {};

	/* 이 노드에 저장된 오브젝트들 */
	vector<const STATIC_OBJ_INFO*> m_vecObjectInfos;

	/* 분할 임계값: 이 개수 초과 시 분할 */
	static constexpr _uint MAX_OBJECTS_PER_NODE = 8;

public:
	static OcTree* Create(const _float3& _center, _float _halfSize, _uint _maxDepth);
	virtual void Free() override;
};

NS_END

