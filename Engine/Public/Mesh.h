#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL Mesh final : public VIBuffer
{
public:
    typedef struct tagMorphDeltaVertex {
        _float3 deltaPos;
        _float3 deltaNor;
        _float3 padding;
    }VTXMORPH;

    struct MorphInfos {
        _uint numActiveMorphs = {};
        _uint numVertices = {};
        _uint padding[2] = {};
    } ;

private:
    explicit Mesh();
    explicit Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Mesh(const Mesh& original);
    virtual ~Mesh();

public:
    HRESULT Initialize_FBX(MODEL _modelType, const aiMesh* _aimesh, class Model* _model, const _fmatrix& _prematrix, myMesh* _myMesh);
    HRESULT Initialize_Binary(MODEL _modelType, myMesh* _myMesh, Model* _model, const _fmatrix& _prematrix);

    HRESULT Ready_For_NonAnimation(const aiMesh* _aiMesh, const _fmatrix& _preTransformMatrix, myMesh* _myMesh);
    HRESULT Ready_For_Animation(const aiMesh* _aiMesh, class Model* _model, myMesh* _myMesh);
    HRESULT Ready_For_AnimationPlayer(const aiMesh* _aiMesh, class Model* _model, myMesh* _myMesh);

    HRESULT Ready_For_NonAnimation_Binary(myMesh* _myMesh, const _fmatrix& _preTransformMatrix);
    HRESULT Ready_For_PlayerAnimation_Binary(myMesh* _myMesh, class Model* _model);
    HRESULT Ready_For_Animation_Binary(myMesh* _myMesh, class Model* _model);

    void Update_Skinned_VertexPositions(const vector<class Bone*>& _bones);

    void Calculate_LocalAABB();


    HRESULT Create_RemapBuffer(class Model* pPartModel, class Model* pMasterModel, const vector<_uint>& vecRemapBone);
    HRESULT Create_StandAloneRemapBuffer(class Model* pMyModel);
private: //애니메이션 관련
    _uint FallBackMasterBoneIndex(_uint iPartBoneIndex, const vector<_uint>& _RemapBone, const vector<class Bone*>& PartBone); //매핑안되는 파츠들의 뼈들을 부모 통해서 찾기위함
  


public:
    HRESULT Bind_Bones(class Shader* _shader, const _string& _constantname, const vector<class Bone*>& _bones);
    HRESULT Bind_BonesMasterRig(class Shader* _shader, const _string& _constantname, const vector<class Bone*>& _MasterRigBones,const vector<_uint>& _RemapBone,
        const vector<class Bone*>& Bones);
    HRESULT Bind_Morphs(Shader* _shader, const vector<_string>& _morphNames, const vector<_float>& _weights);
    HRESULT Bind_RemapSRV(class Shader* pShader);
    HRESULT Register_StructuredBuffer(class Shader* _shader);

#pragma region Get함수
    inline _uint Get_MaterialIndex() const { return m_iMaterialIndex; }
    inline _uint Get_NumAnimMesh() const { return m_iNumAnimMesh; }
    inline _uint GetNumVertices() const { return m_iNumVertices; }
    _bool Has_Morph() const { return m_bHasMorph; }
    const vector<MORPH>& Get_Morphs() const { return m_vecMorphs; }
    _string Get_Name() { return m_strName; }
    const BoundingBox& Get_BoundingBox() { return m_tLocalAABB; }
    const vector< _float4x4>& Get_OffsetMatrices() { return m_vecOffsetMatrices; }
    const vector<_uint>& Get_BoneIndices() { return m_vecBoneIndicies; }
    _uint Get_MeshNumBones() const { return m_iNumBones; }

#pragma endregion

    //Instancing
    HRESULT Ready_InstanceBuffer(_uint iNumMaxInstance = 1000);
    HRESULT Render_Instancing(const vector<_float4x4>& InstanceData);

    set<_uint> Get_ActiveBoneIndices() const;
    set<_uint> Get_ActiveBoneIndices_Player() const;


private:
    _string m_strName = {};
    _uint m_iMaterialIndex = {};
    _uint m_iNumBones = {}; /* 이 메시를 구성하는 정점들에게 영향을 주는 Bone의 개수 */

    /* 이 메시에게 영향을 주는 Bone들을 모아놓는다 */
    /* Bone들의 인덱스 : 모델 객체가 들고 있는 전체 Bone컨테이너의 특정 인덱스 */
    vector<_uint> m_vecBoneIndicies;
    _float4x4 m_matBoneMatrices[g_iMaxNumBones] = {};

    /* 특정 뼈를 정점에게 적용할 때, 추가 보정을 거쳐서 정점에게 상태를 전달할 수 있도록한다.  */
    vector<_float4x4>	m_vecOffsetMatrices;
    
    VTXANIMMESH* m_pVtxInfos = { nullptr };
    VTXPLAYERANIMMESH* m_pVtxPlyayerInfo = { nullptr };


    /* 얼굴 모프 타겟용 */
    vector<MORPH> m_vecMorphs;
    _bool m_bHasMorph = false;
    _uint m_iNumAnimMesh = {};
    _float3* m_pFlatPosDeltas = { nullptr };
    _float3* m_pFlatNorDeltas = { nullptr };
    unordered_map<_string, _uint> m_umapMorphNameToIndex;

    ID3D11Buffer* m_pMorphPosBuffer = { nullptr };
    ID3D11Buffer* m_pMorphNorBuffer = { nullptr };
    ID3D11ShaderResourceView* m_pMorphPosSRV = { nullptr };
    ID3D11ShaderResourceView* m_pMorphNorSRV = { nullptr };

    //Instancing
    ID3D11Buffer* m_pInstanceBuffer = nullptr; //인스턴스버퍼
    _uint         m_iNumInstance = 0; //개수
    _uint         m_iInstanceBuffer = 0;// 필요한버퍼수담아올곳

    
    // 일단 OcTree용 BoundingBox
    BoundingBox m_tLocalAABB;

    //애니메이션
    ID3D11Buffer* m_pRemapBuffer = { nullptr };
    ID3D11ShaderResourceView* m_pReampSRV = { nullptr };


public:
	static Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL _modelType, const aiMesh* _aimesh, class Model* _model, const _fmatrix& _prematrix
    , myMesh* _myMesh);
    static Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL _modelType, Model* _model, myMesh* _myMesh, const _fmatrix& _prematrix);

public:
	void Free() override final;

};

NS_END
