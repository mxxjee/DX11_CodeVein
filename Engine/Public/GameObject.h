#pragma once

#include "Transform.h"

NS_BEGIN(Engine)

class GameInstance;
class Shader;
class Model;
class LightComponent;
class StructuredBuffer;
class Collider;
class Layer;

class ENGINE_DLL GameObject abstract : public Base
{
public:
	typedef struct tagGameObjectDesc : public Transform::TRANSFORMDESC
	{
		_bool bIsActive = { true };
		_bool bIsVisible = { true };
		_bool bIsDead = { false };
		_bool bIsPartOBJ = { false };
		_wstring wstrName = L"";
		_wstring wstrShaderName = L"";
		_wstring wstrModelName = L"";
		vector<_int> vecPasses;

		void* voidValue0 = { nullptr };

	}GAMEOBJECT_DESC;

	typedef struct tagSBDesc {
		StructuredBuffer* pBuffer = { nullptr };
		SBUSAGE eUsage = {};
		_uint iNumElements = {};
		_uint iStride = {};
		const void* initialData = { nullptr };
	}SB_DESC;

protected:
	explicit GameObject();
	explicit GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GameObject(const GameObject& original);
	virtual ~GameObject();

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* arg);

public:
	virtual _int	Update_Priority(const _float fTimeDelta);
	virtual _int    Update_Parallel(const _float fTimeDelta) { return 0; }
	virtual _int	Update(const _float fTimeDelta);
	virtual _int	Update_Late(const _float fTimeDelta);
	virtual HRESULT Render(const _float fTimeDelta);
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) { return S_OK; }
	virtual HRESULT Render_BlobShadow(const _float fTimeDelta) { return S_OK; }
	void Add_RenderGroup(RENDER_GROUP _rendergroup);

	HRESULT Add_Component(_uint prototypelevelID, const _wstring& prototypename, const _wstring& componentname, Component** outcomponent, void* arg = nullptr);
	HRESULT Add_StructuredBuffer(StructuredBuffer*& _buffer, SBUSAGE _usage, _uint _numElements, _uint _stride, const void* _initialData);
	HRESULT Add_StructuredBuffer(SB_DESC& _desc);
	HRESULT Add_Collider(COLLISION_GROUP _groupTag, COLLIDER _type, Collider** _outCollider, void* _initialData);

	virtual HRESULT Bind_ShaderResources() { return S_OK; }
	virtual HRESULT Bind_ShaderPicking() { return S_OK; }
	HRESULT Bind_ProjectionMatrices(Shader* _shader);

	_float RandomDamage(_float _damage, _uint _range = 10);	// 입력값과 범위로 랜덤값 출력할 함수


#pragma region Get Function
	_wstring Get_Name() const { return m_wstrName; }
	_uint Get_ObjectID() const { return m_iObjectID; }
	_matrix Get_WorldMatrix() const { return m_pTransformCom->Get_WorldMatrix(); }
	_float4x4 Get_WorldFloat4x4() const { return m_pTransformCom->Get_WorldFloat4x4(); }
	_vector Get_Position() { return m_pTransformCom->Get_State(DIRECTION::POSITION); }
	_float3 Get_Position_Float3() { return m_pTransformCom->Get_Position_Float3(); }
	_float4 Get_Position_Float4() { return m_pTransformCom->Get_Position_Float4(); }
	_vector Get_Look() { return m_pTransformCom->Get_State(DIRECTION::LOOK); }
	_float3 Get_Look_Float3() { return m_pTransformCom->Get_Look_Float3(); }
	_float3 Get_Scale();
	_float Get_Speed();
	_wstring Get_PrototypeName() const { return m_wstrPrototypeName; }
	_bool Is_Dead() const { return m_bIsDead; }
	_bool Is_Active() const { return m_bIsActive; }
	_bool Is_Visible() const { return m_bIsVisible; }
	_bool Is_PartObj() const { return m_bIsPartObj; }

	Layer* Get_Layer() { return m_pLayer; }

	class Component* Get_Component_FromName(const _wstring& _componentName);
	class Transform* Get_Transform() { return m_pTransformCom; }
	virtual Shader* Get_Shader() { return nullptr; }
	virtual Shader* Get_PickingShader() { return nullptr; }
	virtual Model* Get_Model() { return nullptr; }
	virtual LightComponent*& Get_LightPtr() { static LightComponent* dummy1 = nullptr; return dummy1; };
	virtual Collider*& Get_ColliderPtr() { static Collider* dummy2 = nullptr; return dummy2; };
	virtual class vector<class Animation*>* Get_Animation() { return nullptr; }
	unordered_map<_wstring, Component*>& Get_umapComponents() { return m_umapComponents; }
	const _wstring& Get_ModelName() const { return m_wstrModelName; }
	virtual class Stat* Get_StatComponent() const { return nullptr; }

	GameObject* Get_Target() { return m_pTarget; }
	virtual physx::PxController* Get_Controller() { return nullptr; }
#pragma endregion Get Function


#pragma region Set Function
	void Set_State(DIRECTION _direction, _fvector _value);
	void Set_State(DIRECTION _direction, _float4& _value);
	void Set_Scale(const _float ScaleX = 0, const _float ScaleY = 0, const _float ScaleZ = 0);

	virtual void Set_Dead(_bool _isalive);
	virtual void Set_Active(_bool _isActive) { m_bIsActive = _isActive; }
	virtual void Set_Visible(_bool _isVisible) { m_bIsVisible = _isVisible; }

	//UI를 위한 Set_active함수..ondisable애니메이션 적용여부ㅠㅠ
	virtual void Set_Active(_bool isActive, _bool bUseAnim) {}

	void Set_PartObj(_bool _isPartObj) { m_bIsPartObj = _isPartObj; }
    void Set_PrototypeName(const _wstring& _name) { m_wstrPrototypeName = _name; }
    void Set_Speed(const _float _speed);
    void Rotation(_fvector _axis, const _float _radian);
    void Rotation(_float _fRadianX, _float _fRadianY, _float _fRadianZ);
    void LookDir(const _fvector _direction) { m_pTransformCom->LookDir(_direction); }
    virtual void LookAt(const _fvector _targetposition) { m_pTransformCom->LookAt(_targetposition); }

	virtual void Set_Shader(Shader* _shader) {};
	virtual void Set_Shader(const _wstring& _shaderName) {};
	virtual void Set_Model(Model* _model) {};

	void		Set_Target(GameObject* pTarget) { m_pTarget = pTarget; }

	void Add_Object_To_Octree(RENDER_GROUP _group);
	BoundingBox Calculate_WorldAABB();
#ifdef _DEBUG
	void Add_Debug_Capsule(_fvector _vStart, _fvector _vEnd, _float _radius, _float3 _color);
	void Add_Debug_Capsule(const _float3& _startPosition, const _float3& _endPosition, _float _radius, _float3 _color);
	void Add_Debug_Capsule(const _float4& _startPosition, const _float4& _endPosition, _float _radius, _float3 _color);
	void Add_Debug_Capsule(_fmatrix _startMatrix, _fmatrix _endMatrix, _float _radius, _float3 _color);
	void Add_Debug_Capsule(const _float4x4& _startMatrix, const _float4x4& _endMatrix, _float _radius, _float3 _color);
	void Add_Debug_Sphere(BoundingSphere _sphere, _float3 _color = {1.f, 1.f, 0.f});
	virtual void Add_Debug_LookLine();
	void Add_Debug_Fan(_float3 _center, _float3 _look, _float _radius, _float _halfAngle, _float3 _color = _float3(1.f, 0.5f, 0.f));
#endif // _DEBUG


	void		Set_Name(_wstring Name) { m_wstrName = Name; }

	void		Set_Layer(Layer* pLayer) { m_pLayer = pLayer; }
#pragma endregion Set Function


#pragma region EventManager
	template <typename Event>
	EventHandle Subscribe_Event(function<void(const Event&)> _callback);
	template<typename Event>
	void Publish(const Event& _eventData);
	void Unsubscribe_Event(EventHandle _handlenum);
#pragma endregion EventManager


	/* 오브젝트가 사용하는 쉐이더의 패스 관련 */
#pragma region ObjPass
	void Set_VecObjPassSize(_int iSize = 2)
	{
		m_vecObjPass.resize(iSize);
	}
	void Set_Pass_VecObjPass(_int Index, _int PassNum)
	{
		if (Index == -1)
			return;
		m_vecObjPass[Index] = PassNum;
	}
	void Set_AllPass_VecObjPass(_int PassNum = 0)
	{
		for (int i = 0; i < m_vecObjPass.size(); i++)
		{
			m_vecObjPass[i] = PassNum;
		}
	}
	void Set_AllPass_ByModel(class Model* _model, _int _passNum = 0);
	_int Get_VecObjPass(_int Index = 0)
	{
		return m_vecObjPass[Index];
	}
	void Set_Passes(vector<_int> _passes) { m_vecObjPass = _passes; }
#pragma endregion ObjPass


#pragma region Parsing
	// 타입 이름 확인
	static const _string& Get_StaticTypeName() {
		static _string tempTypeName = "";
		return tempTypeName;
	}
	virtual const _string& Get_TypeName() { return Get_StaticTypeName(); }
	virtual void Set_TypeName() {};
	virtual void Set_TypeName(const _string& _typename) {};

	// 쉐이더 프로토타입 이름 확인
	virtual const _wstring& Get_ShaderName() { return m_wstrShaderName; }

	// 오브젝트 타입 확인
	OBJTYPE Get_OBJType() { return m_eObjType; }
	vector<_int> Get_Passes() { return m_vecObjPass; }

#pragma endregion Parsing

	
#pragma region UI가상함수.렌더러에서 다이나믹캐스트안하려고 추가함 ㅋㅋㅋㅋㅋㅋ
	virtual _int	Get_ZOrder() const { return 0; }
	virtual _float	Get_CombinedZ() const { return 0.f; }
	virtual bool	IsBlurUI() { return false; }
	virtual void	OnDestroyInLayer() {}
#pragma endregion UI가상함수.렌더러에서 다이나믹캐스트안하려고 추가함 ㅋㅋㅋㅋㅋㅋ

public:
#pragma region ObjectPooling
	/* 풀링 정보 설정/접근 */
	void    Set_PoolInfo(_uint _poolID, _uint _slotIndex)
	{
		m_iPoolID = _poolID; m_iPoolSlotIndex = _slotIndex;
	}

	_uint   Get_PoolID()        const { return m_iPoolID; }
	_uint   Get_PoolSlotIndex() const { return m_iPoolSlotIndex; }
	_bool   Is_Pooled()         const { return m_iPoolID != _uint(-1); }

	/* 풀링 생명주기 콜백 */
	virtual void OnSpawn(void* _arg) {}
	virtual void OnDespawn() {}
#pragma endregion ObjectPooling



protected:
	_bool m_bIsActive = { true };
	_bool m_bIsVisible = { true };
	_bool m_bIsDead = { false };
	_bool m_bIsClone = { false };
	_bool m_bIsPartObj = { false };
	_wstring m_wstrName = L"";
	_uint m_iLevel = {};

	GameObject* m_pTarget = nullptr;
	Layer*		m_pLayer = nullptr;

	unordered_map<_wstring, Component*> m_umapComponents;

	_uint m_iObjectID = {};
	static _uint m_iTotalID;

    _wstring m_wstrPrototypeName = L"";

	// 구독목록 자동 해제용
	vector<_uint> m_vecSubscribeNumbers;
	// 소유 목록 자동 해제용(구조체 버퍼, 충돌체(해제용))
	vector<Base*> m_vecCleanUpTargets;

	/* 파싱용 */
	_wstring m_wstrShaderName = {}; // 쉐이더 프로토타입 이름
	_wstring m_wstrModelName = L"";
	vector<_int> m_vecObjPass = {};	// Index번호의 메쉬를 n번 패스로 그리겠다고 저장
	OBJTYPE m_eObjType = { OBJTYPE::TYPE_END }; // 오브젝트 타입


	/* 오브젝트 풀링 */
	_uint m_iPoolID = { _uint(-1) };       // POOL_ID 값 (-1이면 풀링 대상 아님)
	_uint m_iPoolSlotIndex = {};           // 풀 내 슬롯 인덱스



protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	GameInstance* m_pGameInstance = { nullptr };
	Transform* m_pTransformCom = { nullptr };



public:
	static GameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* arg);

public:
	void Free() override;

};

NS_END

#include "GameObject.inl"
