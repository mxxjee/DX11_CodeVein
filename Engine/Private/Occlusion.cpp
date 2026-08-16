#include "Engine_Define.h"
#include "Occlusion.h"

//#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Occlusion::Occlusion()
{
}

Engine::Occlusion::Occlusion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)//, m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	//Safe_AddRef(m_pGameInstance);
}

Engine::Occlusion::~Occlusion()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Occlusion::Initialize(_uint _width, _uint _height)
{
    m_MOC = MaskedOcclusionCulling::Create();
    MSG_NULL(m_MOC, L"MOC 생성 실패", L"Caution!!!", E_FAIL);

    // 해상도는 폭 8배수, 높이 4배수로 맞춤 (MOC 내부 타일 요구사항임)
    m_iWidth = (_width + 7) & ~7u;
    m_iHeight = (_height + 3) & ~3u;
    m_MOC->SetResolution(m_iWidth, m_iHeight);

    // DX11 near clip과 맞추기
    m_MOC->SetNearClipPlane(0.1f);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 오클루전 시작 함수 ////////////////////////////////////////////////////////
void Engine::Occlusion::BeginFrame()
{
	m_MOC->ClearBuffer();
}
/******************************************************* 오클루전 시작 함수 *******************************************************/



//////////////////////////////////////////////////////// Occluder 래스터라이즈 ////////////////////////////////////////////////////////
void Engine::Occlusion::RenderOccluder_AABB(const BoundingBox& _worldAABB, const _fmatrix _viewProjection)
{
    _float3 c = _worldAABB.Center;
    _float3 e = _worldAABB.Extents;

    // 실제 메시보다 AABB가 크기 때문에 줄여야 정확함
    static constexpr _float OCCLUDER_SHRINK = 0.6f;
    e.x *= OCCLUDER_SHRINK;
    e.y *= OCCLUDER_SHRINK;
    e.z *= OCCLUDER_SHRINK;

    _float verts[8][4] =
    {
        { c.x - e.x, c.y - e.y, c.z - e.z, 1.f },
        { c.x + e.x, c.y - e.y, c.z - e.z, 1.f },
        { c.x - e.x, c.y + e.y, c.z - e.z, 1.f },
        { c.x + e.x, c.y + e.y, c.z - e.z, 1.f },
        { c.x - e.x, c.y - e.y, c.z + e.z, 1.f },
        { c.x + e.x, c.y - e.y, c.z + e.z, 1.f },
        { c.x - e.x, c.y + e.y, c.z + e.z, 1.f },
        { c.x + e.x, c.y + e.y, c.z + e.z, 1.f },
    };

    static const _uint indices[36] =
    {
        0,2,1, 1,2,3,
        4,5,6, 5,7,6,
        0,1,4, 1,5,4,
        2,6,3, 3,6,7,
        0,4,2, 2,4,6,
        1,3,5, 3,7,5,
    };

    _float4x4 vpMatrix;
    XMStoreFloat4x4(&vpMatrix, XMMatrixTranspose(_viewProjection));

    m_MOC->RenderTriangles(
        (_float*)verts,
        indices,
        12,
        (_float*)&vpMatrix,
        MaskedOcclusionCulling::BACKFACE_CW);
}
/******************************************************* Occluder 래스터라이즈 *******************************************************/



//////////////////////////////////////////////////////// Occludee 테스트 ////////////////////////////////////////////////////////
_bool Engine::Occlusion::IsBoxVisible(const BoundingBox& _worldAABB, const _matrix& _viewProjection)
{
    // AABB 8개 꼭짓점을 clip space로 변환 -> NDC min/max 계산
    XMFLOAT3 corners[8];
    _worldAABB.GetCorners(corners);

    _float ndcMinX = FLT_MAX, ndcMinY = FLT_MAX;
    _float ndcMaxX = -FLT_MAX, ndcMaxY = -FLT_MAX;
    _float wMin = FLT_MAX;

    for (_uint i = 0; i < 8; ++i)
    {
        XMVECTOR corner = XMLoadFloat3(&corners[i]);
        XMVECTOR clipPos = XMVector3Transform(corner, _viewProjection);

        _float x = XMVectorGetX(clipPos);
        _float y = XMVectorGetY(clipPos);
        _float w = XMVectorGetW(clipPos);

        // w가 0 이하면 카메라 뒤쪽이므로 보수적으로 visible 처리
        if (w <= 0.f)
            return true;

        _float ndcX = x / w;
        _float ndcY = y / w;

        ndcMinX = min(ndcMinX, ndcX);
        ndcMinY = min(ndcMinY, ndcY);
        ndcMaxX = max(ndcMaxX, ndcX);
        ndcMaxY = max(ndcMaxY, ndcY);
        wMin = min(wMin, w);
    }

    // 화면 밖이면 frustum culling에서 이미 걸렸으므로 visible 처리
    if (ndcMaxX < -1.f || ndcMinX > 1.f || ndcMaxY < -1.f || ndcMinY > 1.f)
        return true;

    // NDC 범위를 화면 안으로 클램프
    ndcMinX = max(ndcMinX, -1.f);
    ndcMinY = max(ndcMinY, -1.f);
    ndcMaxX = min(ndcMaxX, 1.f);
    ndcMaxY = min(ndcMaxY, 1.f);

    auto result = m_MOC->TestRect(ndcMinX, ndcMinY, ndcMaxX, ndcMaxY, wMin);
    return result != MaskedOcclusionCulling::OCCLUDED;
}
/******************************************************* Occludee 테스트 *******************************************************/



//////////////////////////////////////////////////////// 디버그, 유틸리티 함수 ////////////////////////////////////////////////////////
void Engine::Occlusion::ComputeDebugDepthBuffer(_float* _outBuffer)
{
    m_MOC->ComputePixelDepthBuffer(_outBuffer, false);
}

_bool Engine::Occlusion::IsGoodOccluder(const BoundingBox& _aabb, _float _areaThreshold)
{
    _float sizes[3] = { _aabb.Extents.x, _aabb.Extents.y, _aabb.Extents.z };

    if (sizes[0] > sizes[1]) swap(sizes[0], sizes[1]);
    if (sizes[1] > sizes[2]) swap(sizes[1], sizes[2]);
    if (sizes[0] > sizes[1]) swap(sizes[0], sizes[1]);

    _float largestFaceArea = sizes[1] * sizes[2];

    if (largestFaceArea < _areaThreshold)
        return false;

    // 수정됨 : 0.3 → 0.5 (벽+바닥+큰 구조물 포함, 정육면체급 바위만 제외)
    _float flatness = sizes[0] / sizes[2];
    if (flatness > 0.5f)
        return false;

    return true;
}
/******************************************************* 디버그, 유틸리티 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Occlusion* Engine::Occlusion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _width, _uint _height)
{
	Occlusion* pInstance = new Occlusion(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_width, _height), L"Occlusion 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Occlusion::Free()
{
	__super::Free();

    if (m_MOC)
    {
        MaskedOcclusionCulling::Destroy(m_MOC);
        m_MOC = { nullptr };
    }

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	//Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

