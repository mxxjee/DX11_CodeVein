#include "Engine_Define.h"
#include "Bone.h"
#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Bone::Bone()
{
}

Engine::Bone::Bone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance(GameInstance::GetInstance())//,m_pDevice(pDevice), m_pContext(pContext)
{
	//Safe_AddRef(m_pDevice);
	//Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

Engine::Bone::Bone(const Bone& original)
    : m_pGameInstance(GameInstance::GetInstance())//,m_pDevice(original.m_pDevice), m_pContext(original.m_pContext)
    , m_strName(original.m_strName), m_matTransformMatrix(original.m_matTransformMatrix), m_matCombinedTransformMatrix(original.m_matCombinedTransformMatrix)
    , m_iParentBoneIndex(original.m_iParentBoneIndex)
{
    //Safe_AddRef(m_pDevice);
    //Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

Engine::Bone::~Bone()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Bone::Initialize(const aiNode* _aiNode, _int _parentBoneIndex, myNode* _mynode, vector<LERPCONTAIN>& _vecLerp)
{
    _mynode->mName = m_strName = _aiNode->mName.data;

    _mynode->mParentIndex = m_iParentBoneIndex = _parentBoneIndex;

	memcpy(&m_matTransformMatrix, &_aiNode->mTransformation, sizeof(_float4x4));
    
	TransposeMatrix(m_matTransformMatrix);

    memcpy(&_mynode->mTransformation, &m_matTransformMatrix, sizeof(_float4x4));

	IdentityMatrix(m_matCombinedTransformMatrix);

	/*memcpy(&m_matTransformMatrix, &_aiNode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_matTransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_matTransformMatrix)));

	XMStoreFloat4x4(&m_matCombinedTransformMatrix, XMMatrixIdentity());*/

    //XMMatrixDecompose();

    // 애니메이션간 보간용 최초 매트릭스 저장
    LERPCONTAIN container{};

    _matrix transformmatrix = XMLoadFloat4x4(&m_matTransformMatrix);
    container.vScale.x = XMVectorGetX(XMVector3Length(transformmatrix.r[0]));
    container.vScale.y = XMVectorGetX(XMVector3Length(transformmatrix.r[1]));
    container.vScale.z = XMVectorGetX(XMVector3Length(transformmatrix.r[2]));
    _vector quaternion = XMQuaternionRotationMatrix(transformmatrix);
    XMStoreFloat4(&container.vRotation, quaternion);
    XMStoreFloat3(&container.vPosition, transformmatrix.r[3]);

    _vecLerp.push_back(container);

	return S_OK;
}

HRESULT Engine::Bone::Initialize_Binary(const myNode* _mynode, vector<LERPCONTAIN>& _vecLerp)
{
    m_strName = _mynode->mName;

    m_iParentBoneIndex = _mynode->mParentIndex;
        
    memcpy(&m_matTransformMatrix, &_mynode->mTransformation, sizeof(_float4x4));

    IdentityMatrix(m_matCombinedTransformMatrix);

    // 애니메이션간 보간용 최초 매트릭스 저장
    LERPCONTAIN container{};

    _matrix transformmatrix = XMLoadFloat4x4(&m_matTransformMatrix);
    container.vScale.x = XMVectorGetX(XMVector3Length(transformmatrix.r[0]));
    container.vScale.y = XMVectorGetX(XMVector3Length(transformmatrix.r[1]));
    container.vScale.z = XMVectorGetX(XMVector3Length(transformmatrix.r[2]));
    _vector quaternion = XMQuaternionRotationMatrix(transformmatrix);
    XMStoreFloat4(&container.vRotation, quaternion);
    XMStoreFloat3(&container.vPosition, transformmatrix.r[3]);

    _vecLerp.push_back(container);

    XMMatrixDecompose(&m_vScale, &m_vRotation, &m_vTrasnlation, XMLoadFloat4x4(&m_matTransformMatrix));


    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
void Engine::Bone::Update_CombinedTransformationMatrix(const vector<Bone*>& _bones, const _fmatrix& _preTransformMatrix)
{
	if (m_iParentBoneIndex == -1)
	{
        XMStoreFloat4x4(&m_matCombinedTransformMatrix, XMLoadFloat4x4(&m_matTransformMatrix) * _preTransformMatrix);
 	}
	else
	{
		XMStoreFloat4x4(&m_matCombinedTransformMatrix, XMLoadFloat4x4(&m_matTransformMatrix) * XMLoadFloat4x4(&_bones[m_iParentBoneIndex]->m_matCombinedTransformMatrix));
	}


}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 디버그 렌더 함수 ////////////////////////////////////////////////////////
#ifdef _DEBUG
void Engine::Bone::Add_DebugBone()
{
    if (!m_bIsDebugDraw)
        return;

    BoundingSphere sphere;
    memcpy(&sphere.Center, m_matCombinedTransformMatrix.m[3], sizeof(_float3));
    sphere.Radius = 0.1f;

    m_pGameInstance->Add_Debug_Sphere(sphere);
}
#endif // _DEBUG
/******************************************************* 디버그 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Bone* Engine::Bone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiNode* _aiNode, _int _parentBoneIndex, myNode* _mynode, vector<LERPCONTAIN>& _vecLerp)
{
	Bone* pInstance = new Bone(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_aiNode, _parentBoneIndex, _mynode, _vecLerp), L"Bone 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Bone* Engine::Bone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, myNode* _myNode, vector<LERPCONTAIN>& _vecLerp)
{
    Bone* pInstance = new Bone(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Binary(_myNode, _vecLerp), L"Bone 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Bone* Engine::Bone::Clone()
{
	return new Bone(*this);
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Bone::Free()
{
	__super::Free();

	//Safe_Release(m_pDevice);
	//Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

