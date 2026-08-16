#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Model_gltf final : public Component
{
private:
    explicit Model_gltf();
    explicit Model_gltf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Model_gltf(const Model_gltf& original);
    virtual ~Model_gltf();

public:
    HRESULT Initialize_Prototype(MODEL _modeltype, const _wstring& _filepath, const _fmatrix& _preTransformMatrix);
    HRESULT Initialize(void* arg);
    HRESULT Render(const _float fTimeDelta, const _uint _meshIndex);

private:
    HRESULT Ready_Meshes();
    HRESULT Ready_Materials(const _wstring& _filepath);
    HRESULT Ready_Bones(const aiNode* _aiNode, _int iParentIndex);
    HRESULT Ready_Animations();

public:
    HRESULT Bind_Material(class Shader* _shader, const _string& _constantname, _uint _meshindex, aiTextureType _texturetype, _uint _textureindex = 0);
    HRESULT Bind_Bones(Shader* _shader, const _string& _constantname, _uint _meshIndex);

    HRESULT Play_Animation(const _float fTimeDelta);

    inline _uint Get_NumMeshes() const { return m_iNumMeshes; }
    _int Get_BoneIndex(const _char* _name);
    _bool Is_AnimFinished() const { return m_bIsAnimationFinished; }

    void Set_Animation(_uint _animationIndex, _bool _isLoop);

private:
    Assimp::Importer	m_Importer = {};
    const aiScene*		m_pAIScene = { nullptr };

private:
    MODEL m_eModelType = { MODEL::END };
    _float4x4 m_PreTransformMatrix = {};

    _uint m_iNumMeshes = {};
    vector<class Mesh*> m_vecMeshes = {};

    _uint m_iNumMaterials = {};
    vector<class Material*> m_vecMaterials = {};

    _uint m_iNumBones = {};
    vector<class Bone*> m_vecBones;

    _uint m_iNumAnimations = {};
    _uint m_iCurrentAnimationIndex = {};
    vector<class Animation*> m_vecAnimatinos;
    _bool m_bIsLoopAnimation = {};
    _bool m_bIsAnimationFinished = {};

public:
    static Model_gltf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL _modeltype, const _wstring& _filepath, const _fmatrix& _preTransformMatrix);
    virtual Component* Clone(void* arg);

public:
    void Free() override final;

};

NS_END
