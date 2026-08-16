#include "Client_Define.h"
#include "Camera_Customize.h"

#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Camera_Customize::Camera_Customize()
{
}

Client::Camera_Customize::Camera_Customize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Camera(pDevice, pContext)
{
}

Client::Camera_Customize::Camera_Customize(const Camera_Customize& original)
	: Camera(original)
{
}

Client::Camera_Customize::~Camera_Customize()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Camera_Customize::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Camera_Customize::Initialize(void* arg)
{
	static _uint namenum = 0;

	CAMFREE_DESC desc{};

	if (arg == nullptr)
	{
		desc.vEye = _float3(0.f, 1.1f, 2.f);
		desc.vAt = _float3(0.f, 1.f, 0.f);
		desc.fFov = XMConvertToRadians(45.f);
		desc.fNear = 0.1f;
		desc.fFar = 100.f;
		desc.fSensor = m_fSensor = 0.5f;

		desc.fSpeed = 13.f;
		desc.fRotationSpeed = XMConvertToRadians(180.f);
		desc.fAspect = _float(WINCX) / WINCY;
		desc.fSmoothness = 19.f;
		arg = &desc;
	}
	else
	{
		CAMFREE_DESC* camdesc = CAST(CAMFREE_DESC*)(arg);

		m_fSensor = camdesc->fSensor;
	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Camer_Customize_" + namenum++;

	CHECK_FAILED(Camera::Initialize(arg), E_FAIL);

	// Yaw 계산 (Y축 기준 회전, XZ 평면에서의 각도)
	m_fYaw = 0.f;

	/* 카메라가 바라볼 원점 */
	m_vPivot = _float4(m_vAt.x, m_vAt.y, m_vAt.z, 1.f);
	m_vOrigin = m_vPivot;

	/* 카메라 이동량 제어 */
	m_fMaxSpeed = 1.f;
	m_fAccel = 2.5f;
	m_fDecel = 0.99f;                                                                                                                                                         
	m_fZoomSpeed = 0.5f;
	m_fMinDistance = 0.3f;
	m_fMaxDistance = 3.f;
	
	m_fOrbitDistance = 1.f;
	m_fTargetDistance = m_fOrbitDistance;


	m_fMaxOffsetX = 1.2f;
	m_fMaxOffsetY = 0.8f;

	/* 카메라 움직임 잠금 */
	m_bLock = false;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Customize::Update_Priority(const _float fTimeDelta)
{
	if (m_bIsActive)
		return -1;

	//if (m_pGameInstance->KeyPress(DIK_NUMPADPLUS))
	//{
	//    m_fFov += XMConvertToRadians(1.f);
	//    COUT(XMConvertToDegrees(m_fFov));
	//}
	//if (m_pGameInstance->KeyPress(DIK_NUMPADMINUS))
	//{
	//    if(XMConvertToDegrees(m_fFov) > 1.1f)
	//        m_fFov -= XMConvertToRadians(1.f);
	//    COUT(XMConvertToDegrees(m_fFov));
	//}


	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Customize::Update(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;

	//조명 따라다니기 + 각도
	LIGHT_DESC* temp = m_pGameInstance->Get_LightDesc(0);

	//그냥 look을 normalize
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::RIGHT));

	_matrix rotMat = XMMatrixRotationAxis(vRight, XMConvertToRadians(15.f)); //Right를 15도 회전(회전행렬만들고)
	_vector dir = XMVector3Normalize(XMVector3TransformNormal(vLook, rotMat)); //벡터 * 행렬(15도 회전행렬을 곱해준다)
	XMStoreFloat4(&temp->vDirection, dir);
	m_pGameInstance->Set_LightDesc(0, *temp);

	return 0;
}

void Client::Camera_Customize::Update_Camera(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::WHEEL))
	{
		m_bLock = !m_bLock;
	}

	// 카메라 이동
	if (m_bLock == false)
	{
		Move_CustomizeCam_X(fTimeDelta);	// A, D X방향 이동
		Move_CustomizeCam_Y(fTimeDelta);	// W, S Y방향 이동
		Process_OrbitInput(fTimeDelta);		// 카메라 회전
	}
	Compute_Orbit();					// 카메라 최종 위치 결정


	Bind_PipeLine();

	m_pGameInstance->Add_RenderObject(RENDER_GROUP::CAMERA, this);

}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Customize::Update_Late(const _float fTimeDelta)
{
	if (m_bIsActive)
		return -1;

#ifdef _DEBUG
	// m_pColliderCom->Add_Debug_Render();
#endif // _DEBUG

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Camera_Customize::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 이동 제어 함수 ////////////////////////////////////////////////////////
void Client::Camera_Customize::Move_CustomizeCam_X(const _float fTimeDelta)
{
	_bool keyPressed = false;
	_float reverseSpeed = 1.f;

	// W키 눌렀으면 속도 업
	if (m_pGameInstance->KeyPress(DIK_D))
	{
		if (m_fCurrentXSpeed < 0.f)
			reverseSpeed = 3.f;
		keyPressed = true;
		m_fCurrentXSpeed += m_fAccel * fTimeDelta * reverseSpeed;
	}
	if (m_pGameInstance->KeyPress(DIK_A))
	{
		if (m_fCurrentXSpeed > 0.f)
			reverseSpeed = 3.f;
		keyPressed = true;
		m_fCurrentXSpeed -= m_fAccel * fTimeDelta * reverseSpeed;
	}

	// 최대 속도 제한
	//m_fCurrentXSpeed = clamp<_float>(m_fCurrentXSpeed, 0.f, m_fMaxSpeed);
	if (fabs(m_fCurrentXSpeed) > fabs(m_fMaxSpeed))
	{
		// 속도가 음수면 음수 최대값
		if (m_fCurrentXSpeed < 0.f)
		{
			m_fCurrentXSpeed = -m_fMaxSpeed;
		}
		// 양수면 양수 최대값
		else
		{
			m_fCurrentXSpeed = m_fMaxSpeed;
		}
	}

	// 키가 눌려있지 않고, 속도가 있다면
	if (keyPressed == false && fabs(m_fCurrentXSpeed) > 0.f)
	{
		// 속도가 음수면 속도 0까지 증가
		if (m_fCurrentXSpeed < 0.f)
		{
			m_fCurrentXSpeed += m_fAccel * fTimeDelta * m_fDecel;
			m_fCurrentXSpeed = clamp<_float>(m_fCurrentXSpeed, -m_fMaxSpeed, 0.f);
		}
		// 양수면 감소
		else
		{
			m_fCurrentXSpeed -= m_fAccel * fTimeDelta * m_fDecel;
			m_fCurrentXSpeed = clamp<_float>(m_fCurrentXSpeed, 0.f, m_fMaxSpeed);
		}
	}
	else if (fabs(m_fCurrentXSpeed) <= 0.001f)
	{
		m_fCurrentXSpeed = 0.f;
		return;
	}

	m_fOffsetX -= m_fCurrentXSpeed * fTimeDelta;
	m_fOffsetX = clamp(m_fOffsetX, -m_fMaxOffsetX, m_fMaxOffsetX);
}

void Client::Camera_Customize::Move_CustomizeCam_Y(const _float fTimeDelta)
{
	_bool keyPressed = false;
	_float reverseSpeed = 1.f;	// 반대방향으로 갈 때 가속도

	// W키 눌렀으면 속도 업
	if (m_pGameInstance->KeyPress(DIK_W))
	{
		if (m_fCurrentYSpeed < 0.f)
			reverseSpeed = 3.f;
		keyPressed = true;
		m_fCurrentYSpeed += m_fAccel * fTimeDelta * reverseSpeed;
	}
	if (m_pGameInstance->KeyPress(DIK_S))
	{
		if (m_fCurrentYSpeed > 0.f)
			reverseSpeed = 3.f;
		keyPressed = true;
		m_fCurrentYSpeed -= m_fAccel * fTimeDelta * reverseSpeed;
	}

	// 최대 속도 제한
	//m_fCurrentYSpeed = clamp<_float>(m_fCurrentYSpeed, 0.f, m_fMaxSpeed);
	if (fabs(m_fCurrentYSpeed) > fabs(m_fMaxSpeed))
	{
		// 속도가 음수면 음수 최대값
		if (m_fCurrentYSpeed < 0.f)
		{
			m_fCurrentYSpeed = -m_fMaxSpeed;
		}
		// 양수면 양수 최대값
		else
		{
			m_fCurrentYSpeed = m_fMaxSpeed;
		}
	}

	// 키가 눌려있지 않고, 속도가 있다면
	if (keyPressed == false && fabs(m_fCurrentYSpeed) > 0.f)
	{
		// 속도가 음수면 속도 0까지 증가
		if (m_fCurrentYSpeed < 0.f)
		{
			m_fCurrentYSpeed += m_fAccel * fTimeDelta * m_fDecel;
			m_fCurrentYSpeed = clamp<_float>(m_fCurrentYSpeed, -m_fMaxSpeed, 0.f);
		}
		// 양수면 감소
		else
		{
			m_fCurrentYSpeed -= m_fAccel * fTimeDelta * m_fDecel;
			m_fCurrentYSpeed = clamp<_float>(m_fCurrentYSpeed, 0.f, m_fMaxSpeed);
		}
	}
	else if (fabs(m_fCurrentYSpeed) <= 0.001f)
	{
		m_fCurrentYSpeed = 0.f;
		return;
	}

	m_fOffsetY += m_fCurrentYSpeed * fTimeDelta;
	m_fOffsetY = clamp(m_fOffsetY, -m_fMaxOffsetY, m_fMaxOffsetY);
}
/******************************************************* 이동 제어 함수 *******************************************************/



//////////////////////////////////////////////////////// 줌인 / 줌아웃 함수 ////////////////////////////////////////////////////////
void Client::Camera_Customize::Process_OrbitInput(const _float fTimeDelta)
{
	_long mouseMove = {};
	// 좌클릭 드래그 중일 때만
	if (m_pGameInstance->MousePress(MOUSEKEYSTATE::LB))
	{

		// X 드래그 -> yaw 변경 (pivot 주위 공전)
		if ((mouseMove = m_pGameInstance->MouseMove(MOUSEMOVESTATE::X)))
			m_fYaw += mouseMove * m_fSensor * fTimeDelta;
	}

	// 마우스 휠 -> distance 변경 (가까워지고 멀어지고)
	if ((mouseMove = m_pGameInstance->MouseMove(MOUSEMOVESTATE::WHEEL)))
	{
		m_fTargetDistance -= mouseMove * m_fZoomSpeed * fTimeDelta * 0.5f;
		m_fTargetDistance = clamp(m_fTargetDistance, m_fMinDistance, m_fMaxDistance);
	}
}
/******************************************************* 줌인 / 줌아웃 함수 *******************************************************/



//////////////////////////////////////////////////////// 카메라 최종 위치 계산 ////////////////////////////////////////////////////////
void Client::Camera_Customize::Compute_Orbit()
{
	m_fOrbitDistance = fLerp(m_fOrbitDistance, m_fTargetDistance, 0.1f);//Lerp갑승로 부드럽게


	_vector origin = XMLoadFloat4(&m_vOrigin);	// 플레이어 원점

	
	// 원점(origin) 기준 카메라가 어디쯤 Y축 회전 위치에 있을지
	_matrix rotation = XMMatrixRotationY(m_fYaw);
	_vector direction = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), rotation);

	// 회전한곳에서 얼마나 떨어져있을지 정한 위치값
	_vector orbitPos = XMVectorMultiplyAdd(direction, XMVectorReplicate(m_fOrbitDistance), origin);

	// 현재 yaw 기준 Right / Up 계산
	_vector right = XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f), rotation);
	const _vector up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	// X, Y Offset을 더 한 최종 카메라 위치 설정
	_vector finalPosition = orbitPos;
	finalPosition = XMVectorMultiplyAdd(right, XMVectorReplicate(m_fOffsetX), finalPosition);
	finalPosition = XMVectorMultiplyAdd(up, XMVectorReplicate(m_fOffsetY), finalPosition);

	// position 세팅
	m_pTransformCom->Set_State(DIRECTION::POSITION, finalPosition);

	// 카메라 위치에서 바라보는 방향으로 나아간 점
	_vector lookAtTarget = finalPosition - direction;
	m_pTransformCom->LookAt(lookAtTarget);
}
/******************************************************* 카메라 최종 위치 계산 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Camera_Customize* Client::Camera_Customize::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Camera_Customize* pInstance = new Camera_Customize(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Camera_Customize 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Camera_Customize::Clone(void* pArg)
{
	Camera_Customize* pInstance = new Camera_Customize(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Camera_Customize 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Camera_Customize::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
