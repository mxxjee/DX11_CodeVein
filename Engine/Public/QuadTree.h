#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class QuadTree final : public Base
{
public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };
private:
	QuadTree();
	virtual ~QuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(const _float3* pVertexPositions, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv);
	void Make_Neighbors();
private:
	_uint m_iCorners[CORNER_END] = {};
	_uint m_iCenter = {};
	QuadTree* m_pChildren[CORNER_END] = { nullptr };
	QuadTree* m_pNeighbors[NEIGHBOR_END] = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

private:
	_bool isDraw(const _float3* pVertexPositions, _fmatrix WorldMatrixInv);

public:
	static QuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

NS_END