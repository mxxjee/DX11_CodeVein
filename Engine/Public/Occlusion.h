#pragma once

#include "Base.h"
#include "MaskedOcclusionCulling/MaskedOcclusionCulling.h"

#include <profileapi.h>

NS_BEGIN(Engine)

class Occlusion final : public Base
{
private:
	explicit Occlusion();
	explicit Occlusion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Occlusion();

public:
	HRESULT Initialize(_uint _width, _uint _height);

	// 매 프레임 depth buffer 초기화
	void BeginFrame();

	// Occluder AABB를 내부 depth buffer에 래스터라이즈
	void RenderOccluder_AABB(const BoundingBox& _worldAABB, const _fmatrix _viewProjection);

	// Occludee AABB가 가려졌는지 테스트 (반환값이 true = 보임)
	_bool IsBoxVisible(const BoundingBox& _worldAABB, const _matrix& _viewProjection);

	// AABB 면적 기반 Occluder 자동 판별
	static _bool IsGoodOccluder(const BoundingBox& _aabb, _float _areaThreshold = 5.f);

	// 디버그용 depth buffer 시각화
	void ComputeDebugDepthBuffer(_float* _outBuffer);

	_uint Get_Width() const { return m_iWidth; }
	_uint Get_Height() const { return m_iHeight; }

private:
	MaskedOcclusionCulling* m_MOC = { nullptr };
	_uint m_iWidth = {};
	_uint m_iHeight = {};

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static Occlusion* Create();
	static Occlusion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _width, _uint _height);

public:
	void Free() override final;

};

NS_END
