#pragma once

#include "Base.h"
/* 뼈 : aiBone, aiNode, aiNodeAnim */


NS_BEGIN(Engine)

class Bone final : public Base
{
private:
	explicit Bone();
	explicit Bone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Bone(const Bone& original);
	virtual ~Bone();

public:
	HRESULT Initialize(const aiNode* _aiNode, _int _parentBoneIndex, myNode* _mynode, vector<LERPCONTAIN>& _vecLerp);
    HRESULT Initialize_Binary(const myNode* _mynode, vector<LERPCONTAIN>& _vecLerp);
	void Update_CombinedTransformationMatrix(const vector<Bone*>& _bones, const _fmatrix& _preTransformMatrix);

public:
	inline _matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_matCombinedTransformMatrix);
	}

    inline const _float4x4* Get_CombinedTransformationMatrixPtr() const {
        return &m_matCombinedTransformMatrix;
    }

    inline const _float4x4& Get_TransformationMatrix() {
        return m_matTransformMatrix;
    }

	inline _bool Compare_Name(const _string& _name) {
		return (m_strName == _name);
	}

    inline Bone* Compare_Name_Return_Bone(const _string& _name) {
        if (m_strName == _name) return this;
        else return nullptr;
    }

	inline void Set_TransformationMatrix(const _float4x4& _transformationMatrix) {
		m_matTransformMatrix = _transformationMatrix;
	}

	inline _string& Get_BoneName() { return m_strName; }

	inline _int Get_ParentBoneIndex() const { return m_iParentBoneIndex; }

	_vector Get_Scale() const { return m_vScale; }
	_vector Get_Rotation() const { return m_vRotation; }
	_vector Get_Translation() const { return m_vTrasnlation; }
#ifdef _DEBUG
	void Set_DebugDraw(_bool _debug) { m_bIsDebugDraw = _debug; }
	void Add_DebugBone();
#endif // _DEBUG


private:
	_string		m_strName = {};
	_float4x4	m_matTransformMatrix = {}; /* 이 뼈만의 위치를 저장하는 행렬 */
	_float4x4	m_matCombinedTransformMatrix = {}; /* 부모를 기준으로 변환한 위치를 저장하는 행렬 */
	_int		m_iParentBoneIndex = { -1 };
	_vector		m_vScale = {};
	_vector		m_vRotation = {};
	_vector		m_vTrasnlation = {};

#ifdef _DEBUG
	_bool m_bIsDebugDraw = { true };
#endif // _DEBUG


private:
	//ID3D11Device* m_pDevice = { nullptr };
	//ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
    /* aiNode를 넣으면 FBX, 아니면 바이너리 */
	static Bone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiNode* _aiNode, _int _parentBoneIndex, myNode* _mynode, vector<LERPCONTAIN>& _vecLerp);
    static Bone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, myNode* _myNode, vector<LERPCONTAIN>& _vecLerp);
    Bone* Clone();

public:
	void Free() override final;

};

NS_END
