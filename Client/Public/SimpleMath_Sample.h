#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class SimpleMath_Sample final : public GameObject
{
private:
	explicit SimpleMath_Sample();
	explicit SimpleMath_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit SimpleMath_Sample(const SimpleMath_Sample& original);
	virtual ~SimpleMath_Sample();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
	/* 벡터 연산 예제 */
	void Example_VectorBasic();

	/* 대량 반복 최적화 예제 */
	void Example_MassUpdate_Bad(const _float fTimeDelta);
	void Example_MassUpdate_Good(const _float fTimeDelta);

	void Example_DotCross();
	void Example_Quaternion(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	/* 파티클 구조체 (예제용) */
	struct PARTICLE_DESC
	{
		_smvec3 vPosition = {};
		_smvec3 vVelocity = {};
		_smvec3 vAcceleration = {};
		_float  fLifeTime = {};
		_bool   bIsAlive = true;
	};

	/* 파티클 컨테이너 (예제용) */
	vector<PARTICLE_DESC> m_vecParticles = {};
	_uint m_iParticleCount = 1000;

	/* 쿼터니언 예제용 */
	_smvec3 m_vRotationEuler = {};      // 현재 회전값 (Degree)

public:
	static SimpleMath_Sample* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
