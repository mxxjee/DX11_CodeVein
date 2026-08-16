#pragma once

#include "Base.h"
#include "Transform.h"

NS_BEGIN(Engine)

class StructuredBuffer;

class Empty abstract : public Base
{
public:
	typedef struct EmptyDescription : public Transform::TRANSFORMDESC
	{
		_wstring wstrName = L"";
		_wstring wstrShaderName = L"";
		_wstring wstrModelName = L"";
	}EMPTY_DESC;


	typedef struct tagSBDesc {
		StructuredBuffer* pBuffer = { nullptr };
		SBUSAGE eUsage = {};
		_uint iNumElements = {};
		_uint iStride = {};
		const void* initialData = { nullptr };
	}SB_DESC;

protected:
	explicit Empty();
	explicit Empty(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Empty(const Empty& original);
	virtual ~Empty();

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* arg);
	_int	Update_Priority(const _float fTimeDelta);
	_int	Update(const _float fTimeDelta);
	_int	Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
	HRESULT Add_Component(_uint prototypelevelID, const _wstring& prototypename, const _wstring& componentname, Component** outcomponent, void* arg = nullptr);
	HRESULT Add_StructuredBuffer(StructuredBuffer*& _buffer, SBUSAGE _usage, _uint _numElements, _uint _stride, const void* _initialData);
	HRESULT Add_StructuredBuffer(SB_DESC& _desc);


	_wstring Get_Name() const { return m_wstrName; }
	_float3 Get_Scale();
	_float Get_Speed();
	class Component* Get_Component_FromName(const _wstring& _componentName);
	class Transform* Get_Transform() { return m_pTransformCom; }


protected:
	Transform* m_pTransformCom = { nullptr };

	// 컴포넌트 저장용
	unordered_map<_wstring, Component*> m_umapComponents;
	// 구독목록 자동 해제용
	vector<_uint> m_vecSubscribeNumbers;
	// 소유 목록 자동 해제용
	vector<Base*> m_vecCleanUpTargets;


	_wstring m_wstrShaderName = {}; // 쉐이더 프로토타입 이름
	_wstring m_wstrName = L"";		// 객체의 이름

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static Empty* Create();
	static Empty* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Empty* Clone(void* arg);

public:
	void Free() override;

};

NS_END
