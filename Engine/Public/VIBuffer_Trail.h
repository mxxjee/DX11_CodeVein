#pragma once
#include "Engine_Define.h"
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Trail final : public VIBuffer
{
public:
	typedef struct tagTrailDesc				// ImGui 에서 조절할 값들
	{
		_uint iMaxPoints = { 64 };			// 최대 제어점 개수
		_float fLifeTime = { 0.5f };		// 각 점의 수명
		_float fMinDistance = { 0.01f };	// 새 점을 추가할 때 최소 거리
		_float fTexRotation = {};			// 텍스쳐 회전 수치(라디안)
	}TRAIL_DESC;

	typedef struct tagTrailPoint			// 매 프레임 기록하는 검의 양 끝 위치
	{
		_float3 vRoot;						// 무기의 뿌리
		_float3 vTip;						// 무기의 끝
		_float fTimeStamp;					// 기록 시각
	}TRAIL_POINT;

private:
	explicit VIBuffer_Trail();
	explicit VIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit VIBuffer_Trail(const VIBuffer_Trail& original);
	virtual ~VIBuffer_Trail() = default;

public:
	TRAIL_DESC					Get_TrailDesc() const { return m_tTrailDesc; }
	const deque<TRAIL_POINT>&	Get_TrailPoints() const { return m_deqPoints; }
	_bool						Get_IsDeqEmpty() const { return m_deqPoints.empty(); }
	
	void						Set_TrailDesc(const TRAIL_DESC& desc);

public:
	HRESULT Initialize_Prototype(const TRAIL_DESC* pDesc);
	HRESULT Initialize(void* arg) override;
	HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Bind_Resource() override;

public:
	// 매 프레임 새 위치 쌍 추가
	void Add_Point(const _float3& vRoot, const _float3 vTip);

	// 끝난 점 제거 후 버퍼 재설정
	void Update_Trail(const _float fTimeDelta);

	// 초기화
	void Reset();

private:
	void Make_TriangleStrip();

private:
	TRAIL_DESC				m_tTrailDesc{};
	deque<TRAIL_POINT>		m_deqPoints;
	_uint					m_iMaxVertices = {};
	_uint					m_iActiveVertices = {};
	_float					m_fAccumulatedTime = {};

public:
	static VIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_DESC* pDesc);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END