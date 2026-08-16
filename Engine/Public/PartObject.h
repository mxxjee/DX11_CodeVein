#pragma once

#include "Character.h"

NS_BEGIN(Engine)

class Model;

class ENGINE_DLL PartObject abstract : public Character
{
public:
    typedef struct tagPargObjectDesc : public GameObject::GAMEOBJECT_DESC
    {
        const _float4x4* pParentMatrix = { nullptr };
        GameObject* pMasterRig = { nullptr };
           
    }PARTOBJECT_DESC;

protected:
    explicit PartObject();
    explicit PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit PartObject(const PartObject& original);
    virtual ~PartObject();

public:
    void        Set_PartActive(_bool bActive) { m_bActive = bActive; }
    void        Set_OwnerDead(_bool bOwnerDead) { m_bOwnerDead = bOwnerDead; }

    _bool       Get_IsPartActive() const { return m_bActive; }
    _bool       Get_IsOwnerDead() const { return m_bOwnerDead; }
    _float4x4   Get_CombinedMatrix() const { return m_matCombinedMatrix; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int    Update_Parallel(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

protected:
    void Setup_CombinedWorldMatrix(_fmatrix _parentMatrix);
    HRESULT RemapBones(class Model* pPartModelCom);
    HRESULT RemapBonesVec(class Model* pPartModelCom, _int iNum);
    void Setup_MasterModel();   // 마스터모델 컴포넌트를 참조하고 있는지 확인 후 등록

protected:
    vector<_uint>       m_BoneRemap;
    vector<vector<_uint>> m_BoneRemaps; //벡터에 벡터를 싸서 드셔보세요
    GameObject*         m_pMasterRig = { nullptr };
    Model*              m_pMasterModelCom = { nullptr };

protected:
    _bool               m_bActive = { true }; //활성화 ex) 무기 파츠 등록 + 활성화 애들만 업데이트 및 렌더
    _bool               m_bOwnerDead = {};
    const _float4x4*    m_matParentMatrix = { nullptr };
    _float4x4           m_matCombinedMatrix = {};

public:
    static PartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual GameObject* Clone(void* arg);

public:
    void Free() override;

};

NS_END
