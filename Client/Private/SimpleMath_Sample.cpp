#include "Client_Define.h"
#include "SimpleMath_Sample.h"

//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::SimpleMath_Sample::SimpleMath_Sample()
{
}

Client::SimpleMath_Sample::SimpleMath_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

Client::SimpleMath_Sample::SimpleMath_Sample(const SimpleMath_Sample& original)
	: GameObject(original)
{
}

Client::SimpleMath_Sample::~SimpleMath_Sample()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::SimpleMath_Sample::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::SimpleMath_Sample::Initialize(void* arg)
{
	static _uint namenum = 0;

	GAMEOBJECT_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"SimpleMath_Sample_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	/* ============ 파티클 초기화 (예제용) ============ */
	m_vecParticles.resize(m_iParticleCount);
	for (_uint i = 0; i < m_iParticleCount; ++i)
	{
		m_vecParticles[i].vPosition = { (_float)(rand() % 100), (_float)(rand() % 100), (_float)(rand() % 100) };
		m_vecParticles[i].vVelocity = { 0.f, 10.f, 0.f };
		m_vecParticles[i].vAcceleration = { 0.f, -9.8f, 0.f };  // 중력
		m_vecParticles[i].fLifeTime = 5.f;
		m_vecParticles[i].bIsAlive = true;
	}

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::SimpleMath_Sample::Ready_Components()
{
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::SimpleMath_Sample::Update_Priority(const _float fTimeDelta)
{
	/* SimpleMath 쓸 때 주의사항 */
	// SimpleMath는 내부적으로 XMStore, XMLoad를 "여러번" 사용하기 때문에
	// 되도록 호출이 적은 함수에서 사용하는것이 좋다
	// store, load가 2번 사용되고 말거 4번씩 사용되는경우도 왕왕 생기기 때문에 되도록 호출이 적은 기존 방식을 쓰는게 좋다
	// 
	//	상황					| 권장 방식		| 이유
	//	게임오브젝트 Transform	| SimpleMath	| 오브젝트당 1회(가독성)
	//	카메라 계산				| SimpleMath	| 프레임당 1~2회(가독성)
	//	파티클 시스템(1000 + )	| DirectXMath	| 대량 반복(성능)
	//	스키닝 / 본 시스템		| DirectXMath	| 본 개수 x 버텍스만큼(성능)
	//	버텍스 변환(CPU)		| DirectXMath	| 대량 데이터(성능)
	//	물리 충돌 검사			| 상황 따라		| 충돌 쌍이 많으면 성능, 아니면 가독성
	//	UI 위치 계산			| SimpleMath	| 소량(가독성)
	//	셰이더 상수버퍼 세팅	| 어느 쪽이든	| 어차피 Store해서 GPU전송



	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::SimpleMath_Sample::Update(const _float fTimeDelta)
{
	/* 기본 사칙연산 예시 */
	Example_VectorBasic();

	/* 대량반복 나쁜예시 */
	Example_MassUpdate_Bad(fTimeDelta);
	/* 대량반복 좋은 예시 */
	Example_MassUpdate_Good(fTimeDelta);

	/* 내적 외적 사용 예시 */
	Example_DotCross();
	/* 쿼터니언 회전 사용 예시 */
	Example_Quaternion(fTimeDelta);

	m_bIsActive = false;

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::SimpleMath_Sample::Update_Late(const _float fTimeDelta)
{
	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::SimpleMath_Sample::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 샘플 함수 ////////////////////////////////////////////////////////
void Client::SimpleMath_Sample::Example_VectorBasic()
{
	if (!m_bIsActive)
		return;

	/* ========= 생성 방법들 ========= */
	_smvec3 v1 = { 1.f, 2.f, 3.f };           // 직접 초기화
	_smvec3 v2(4.f, 5.f, 6.f);                // 생성자
	_smvec3 v3 = Vector3::Zero;               // (0, 0, 0)
	_smvec3 v4 = Vector3::One;                // (1, 1, 1)
	_smvec3 v5 = Vector3::Right;                 // (1, 0, 0)
	_smvec3 v6 = Vector3::Up;                 // (0, 1, 0)
	_smvec3 v7 = Vector3::Forward;            // (0, 0, -1) 주의: 오른손 좌표계 기준

	/* ========= 생성 방법대체 ========= */
	_smvec3 vA = v3Zero;            // (0, 0, 0)
	_smvec3 vB = v3One;             // (1, 1, 1)
	_smvec3 vC = v3Right;           // (1, 0, 0)
	_smvec3 vD = v3Up;				// (0, 1, 0)
	_smvec3 vE = v3Look;            // (0, 0, 1) 이건 왼손좌표계임
	_smvec3 vF = v3Forward;         // (0, 0, 1) 이건 왼손좌표계임2
	


	/* ========= 사칙연산 ========= */
	_smvector3 smvec3 = { 1.f, 1.f, 2.f };
	_float3 float3 = { 3.f, 3.f, 4.f };
	_vector vector = { 5.f, 5.f, 5.f, 6.f };
	_smvector3 smvec3_2 = { 7.f, 7.f, 8.f };

	_smvector3 result = {};
	
	result = smvec3 + float3;
	COUT("result  = " << result.x << ".f, " << result.y << ".f, " << result.z << ".f;");// result = 4.f, 4.f, 6.f;
	result = smvec3 + _smvector3(vector); // 묵시적 형변환이 필요, result = 6.f, 6.f, 8.f;
	COUT("result  = " << result.x << ".f, " << result.y << ".f, " << result.z << ".f;");// result = 6.f, 6.f, 8.f;
	result = smvec3 + smvec3_2; // result = 8.f, 8.f, 10.f;
	COUT("result  = " << result.x << ".f, " << result.y << ".f, " << result.z << ".f;");// result = 8.f, 8.f, 10.f;

	// 가장 좋은건 simplemath끼리 쓰는것


}

/*
  대량 반복 - 나쁜 예시 (SimpleMath 연산자 남용)
  매 연산마다 내부적으로 XMLoad/XMStore가 반복되어 오버헤드 발생
*/
void Client::SimpleMath_Sample::Example_MassUpdate_Bad(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	for (auto& tParticle : m_vecParticles)
	{
		if (!tParticle.bIsAlive)
			continue;

		/* SimpleMath 연산자 사용 - 각 줄마다 Load/Store 발생 */
		// += 내부: Load 2번 → Add → Store 1번
		// * 내부: Load 1번 → Multiply → Store 1번
		// 총: Load 6번, Store 4번 (오버헤드 큼)
		tParticle.vVelocity += tParticle.vAcceleration * fTimeDelta;
		tParticle.vPosition += tParticle.vVelocity * fTimeDelta;

		tParticle.fLifeTime -= fTimeDelta;
		if (tParticle.fLifeTime <= 0.f)
			tParticle.bIsAlive = false;
	}
}

/*
  대량 반복 - 좋은 예시 (XMVECTOR로 변환 후 연산)
  Load/Store를 최소화하여 SIMD 레지스터 안에서 연산
*/
void Client::SimpleMath_Sample::Example_MassUpdate_Good(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	/* TimeDelta를 XMVECTOR로 미리 변환 (루프 밖에서 1번만) */
	_vector vTimeDelta = XMVectorReplicate(fTimeDelta);

	for (auto& tParticle : m_vecParticles)
	{
		if (!tParticle.bIsAlive)
			continue;

		/* SimpleMath → XMVECTOR 로드 (한 번만) */
		// SimpleMath는 XMVECTOR로 암시적 변환 가능
		_vector vPos = tParticle.vPosition;
		_vector vVel = tParticle.vVelocity;
		_vector vAcc = tParticle.vAcceleration;

		/* SIMD 연산 (레지스터 안에서 처리, Load/Store 없음) */
		// vVel = vVel + (vAcc * fTimeDelta)
		vVel = XMVectorAdd(vVel, XMVectorMultiply(vAcc, vTimeDelta));
		// vPos = vPos + (vVel * fTimeDelta)
		vPos = XMVectorAdd(vPos, XMVectorMultiply(vVel, vTimeDelta));

		/* XMVECTOR → SimpleMath 저장 (한 번만) */
		// XMVECTOR는 SimpleMath로 암시적 변환 가능
		tParticle.vPosition = vPos;
		tParticle.vVelocity = vVel;

		/* 스칼라 연산은 그냥 처리 */
		tParticle.fLifeTime -= fTimeDelta;
		if (tParticle.fLifeTime <= 0.f)
			tParticle.bIsAlive = false;
	}
}

/*
   내적(Dot Product) / 외적(Cross Product) 예제
   
   내적: 두 벡터의 방향 관계 파악 (같은 방향=양수, 수직=0, 반대=음수)
   외적: 두 벡터에 수직인 벡터 생성 (법선 벡터 계산 등)
 */
void Client::SimpleMath_Sample::Example_DotCross()
{
	if (!m_bIsActive)
		return;

	COUT("========== 내적/외적 예제 ==========");

	/* ========= 내적 (Dot Product) ========= */
	// 용도: 각도 계산, 시야 판정, 투영 등

	_smvec3 vForward = v3Forward;                // 내 앞 방향 (0, 0, 1)
	_smvec3 vToTarget = { 1.f, 0.f, 1.f };       // 타겟 방향
	vToTarget.Normalize();                        // 정규화 무적권 해야됨

	// 내적 = cos(세타), """정규화된""" 벡터끼리 내적하면 -1 ~ 1 범위
	_float fDot = vForward.Dot(vToTarget);		// SimpleMath의 Dot사용
	COUT("내적 결과: " << fDot);                  // 약 0.707 (45도)

	// 내적으로 각도 구하기
	_float fAngleRad = acosf(fDot);               // 라디안
	_float fAngleDeg = XMConvertToDegrees(fAngleRad);
	COUT("두 벡터 사이 각도: " << fAngleDeg << "도");

	// 시야 판정 예시: 시야각 90도 (cos(45도) = 0.707)
	_float fFovHalf = cosf(XMConvertToRadians(45.f));
	if (fDot > fFovHalf)
	{
		COUT("타겟이 시야 안에 있음!");
	}
	else
	{
		COUT("타겟이 시야 밖에 있음!");
	}

	// 앞/뒤 판정
	if (fDot > 0.f)
	{
		COUT("타겟이 내 앞에 있음");
	}
	else if (fDot < 0.f)
	{
		COUT("타겟이 내 뒤에 있음");
	}
	else
		COUT("타겟이 내 옆에 있음 (수직)");


	/* ========= 외적 (Cross Product) ========= */
	// 용도: 법선 벡터 계산, 좌/우 판정, 회전축 계산 등
	// 주의: 순서에 따라 방향이 반대! (A x B ≠ B x A)

	_smvec3 vRight = v3Right;    // (1, 0, 0)
	_smvec3 vUp = v3Up;          // (0, 1, 0)

	// 왼손 좌표계: Right X Up = Forward (0, 0, 1)
	_smvec3 vCross1 = vRight.Cross(vUp);
	COUT("Right x Up = (" << vCross1.x << ", " << vCross1.y << ", " << vCross1.z << ")");

	// 순서 바꾸면 반대 방향
	_smvec3 vCross2 = vUp.Cross(vRight);
	COUT("Up x Right = (" << vCross2.x << ", " << vCross2.y << ", " << vCross2.z << ")");

	// 삼각형 법선 벡터 계산 예시
	_smvec3 vP0 = { 0.f, 0.f, 0.f };
	_smvec3 vP1 = { 1.f, 0.f, 0.f };
	_smvec3 vP2 = { 0.f, 0.f, 1.f };

	_smvec3 vEdge1 = vP1 - vP0;
	_smvec3 vEdge2 = vP2 - vP0;
	_smvec3 vNormal = vEdge1.Cross(vEdge2);
	vNormal.Normalize();
	COUT("삼각형 법선: (" << vNormal.x << ", " << vNormal.y << ", " << vNormal.z << ")");

	// 좌/우 판정 예시 (2D 기준, Y값으로 판단)
	_smvec3 vMyForward = { 0.f, 0.f, 1.f };
	_smvec3 vToEnemy = { 1.f, 0.f, 0.5f };
	vToEnemy.Normalize();

	_smvec3 vCrossResult = vMyForward.Cross(vToEnemy);
	if (vCrossResult.y > 0.f)
	{
		COUT("적이 내 오른쪽에 있음");
	}
	else if (vCrossResult.y < 0.f)
	{
		COUT("적이 내 왼쪽에 있음");
	}
	else
		COUT("적이 정면 또는 후방에 있음");
}

/*
   쿼터니언 예제
  
   쿼터니언: 회전을 표현하는 4차원 복소수 (x, y, z, w)
   장점: 짐벌락 없음, 보간이 자연스러움, 메모리 효율적
 */
void Client::SimpleMath_Sample::Example_Quaternion(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	COUT("========== 쿼터니언 예제 ==========");

	/* ========= 쿼터니언 생성 ========= */

	// 단위 쿼터니언 (회전 없음)
	_smquat qIdentity = _smquat::Identity;  // (0, 0, 0, 1)

	// 축-각도로 생성: Y축 기준 90도 회전
	_smvec3 vAxis = v3Up;
	_float fAngle = XMConvertToRadians(90.f);
	_smquat qFromAxis = _smquat::CreateFromAxisAngle(vAxis, fAngle);

	// 오일러 각도로 생성 (Yaw, Pitch, Roll 순서)
	_float fYaw = XMConvertToRadians(45.f);    // Y축 회전
	_float fPitch = XMConvertToRadians(0.f);   // X축 회전
	_float fRoll = XMConvertToRadians(0.f);    // Z축 회전
	_smquat qFromEuler = _smquat::CreateFromYawPitchRoll(fYaw, fPitch, fRoll);


	/* ========= 쿼터니언으로 벡터 회전 ========= */

	_smvec3 vOriginal = v3Forward;  // (0, 0, 1)

	// Y축 90도 회전 적용
	_smvec3 vRotated = _smvec3::Transform(vOriginal, qFromAxis);
	COUT("원본: (0, 0, 1) → Y축 90도 회전 후: ("
		<< vRotated.x << ", " << vRotated.y << ", " << vRotated.z << ")");
	// 결과: 약 (1, 0, 0) - Forward가 Right가 됨


	/* ========= 쿼터니언 조합 (곱셈) ========= */
	// 주의: 곱셈 순서 = 적용 순서의 역순

	_smquat qRotY90 = _smquat::CreateFromAxisAngle(v3Up, XMConvertToRadians(90.f));
	_smquat qRotX45 = _smquat::CreateFromAxisAngle(v3Right, XMConvertToRadians(45.f));

	// Y 90도 회전 후 X 45도 회전
	_smquat qCombined = qRotY90 * qRotX45;


	/* ========= 쿼터니언 보간 (Slerp) ========= */
	// 두 회전 사이를 부드럽게 보간 - 캐릭터 회전에 필수!

	_smquat qStart = _smquat::Identity;                                    // 시작 회전
	_smquat qEnd = _smquat::CreateFromAxisAngle(v3Up, XMConvertToRadians(180.f));  // 목표 회전

	// 0.0 = qStart, 1.0 = qEnd, 0.5 = 중간
	_float fProgress = 0.5f;
	_smquat qInterpolated = _smquat::Slerp(qStart, qEnd, fProgress);
	COUT("Slerp 50%: Y축 약 90도 회전된 상태");


	/* ========= 실전 예시: 부드러운 회전 ========= */
	// 매 프레임 목표 회전을 향해 부드럽게 회전

	_smquat qCurrent = _smquat::CreateFromYawPitchRoll(
		XMConvertToRadians(m_vRotationEuler.y),
		XMConvertToRadians(m_vRotationEuler.x),
		XMConvertToRadians(m_vRotationEuler.z)
	);

	_smquat qTarget = _smquat::CreateFromAxisAngle(v3Up, XMConvertToRadians(180.f));

	// 보간 속도 (0.1 = 느림, 0.9 = 빠름)
	_float fLerpSpeed = 5.f * fTimeDelta;  // TimeDelta 곱해서 프레임 독립적으로
	fLerpSpeed = min(fLerpSpeed, 1.f);     // 1.0 초과 방지

	_smquat qSmooth = _smquat::Slerp(qCurrent, qTarget, fLerpSpeed);


	/* ========= 쿼터니언 → 행렬 변환 ========= */
	// 셰이더에 넘기거나 월드 행렬 계산 시 사용

	_smmat matRotation = _smmat::CreateFromQuaternion(qSmooth);

	// 월드 행렬 조합: Scale * Rotation * Translation
	_smvec3 vScale = v3One;
	_smvec3 vPosition = { 0.f, 0.f, 0.f };

	_smmat matWorld = _smmat::CreateScale(vScale) *
		matRotation *
		_smmat::CreateTranslation(vPosition);

	COUT("월드 행렬 생성 완료");


	/* ========= 쿼터니언 → 오일러 각도 추출 ========= */
	// 디버그용, UI 표시용 등
	// 주의: 짐벌락 구간에서 부정확할 수 있음

	_smvec3 vEulerExtracted;

	// 간단한 추출 방법 (Y축 회전만 필요할 때)
	_smvec3 vTestDir = _smvec3::Transform(v3Forward, qSmooth);
	_float fExtractedYaw = atan2f(vTestDir.x, vTestDir.z);
	COUT("추출된 Yaw: " << XMConvertToDegrees(fExtractedYaw) << "도");
}
/******************************************************* 샘플 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::SimpleMath_Sample::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
SimpleMath_Sample* Client::SimpleMath_Sample::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	SimpleMath_Sample* pInstance = new SimpleMath_Sample(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"SimpleMath_Sample 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::SimpleMath_Sample::Clone(void* pArg)
{
	SimpleMath_Sample* pInstance = new SimpleMath_Sample(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"SimpleMath_Sample 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::SimpleMath_Sample::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
