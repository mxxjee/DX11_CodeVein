#include "Engine_Define.h"
#include "Mesh.h"

#include "GameInstance.h"
#include "Bone.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Mesh::Mesh()
{
}

Engine::Mesh::Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: VIBuffer(pDevice, pContext)
{
}

Engine::Mesh::~Mesh()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Mesh::Initialize_FBX(MODEL _modelType, const aiMesh* _aimesh, Model* _model, const _fmatrix& _prematrix, myMesh* _myMesh)
{
    
	_myMesh->mName = m_strName = _aimesh->mName.data;
	_myMesh->mMaterialIndex = m_iMaterialIndex = _aimesh->mMaterialIndex;
	_myMesh->mNumVertices = m_iNumVertices = _aimesh->mNumVertices;
    m_iIndexStride = 4;
    if(g_toolType == TOOLTYPE::SHADER_TOOL)
    {
        COUT("현재 메쉬 : " << m_strName.c_str());
        COUT("현재 메쉬 머티리얼 번호 : " << m_iMaterialIndex);
        COUT("현재 메쉬 정점 개수 : " << m_iNumVertices);
    }

	_myMesh->mNumIndices = m_iNumIndices = _aimesh->mNumFaces * 3;
    _myMesh->mNumFaces = _aimesh->mNumFaces;
    
    m_iNumVertexBuffers = 1;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr{};
	if (_modelType == MODEL::NONANIM)
		hr = Ready_For_NonAnimation(_aimesh, _prematrix, _myMesh);
    else if (_modelType == MODEL::PLAYERANIM)
    {
        hr = Ready_For_AnimationPlayer(_aimesh, _model, _myMesh);
        m_bIsAnimated = true;
    }
    else
    {
        //r = Ready_For_AnimationTest(_aimesh, _model, _myMesh);
        hr = Ready_For_Animation(_aimesh, _model, _myMesh);
        m_bIsAnimated = true;
    }
    
    MSG_FAIL(hr, L"애니메이션 로드에 실패했습니다!", L"엥", E_FAIL);

#pragma region IndexBuffer
	D3D11_BUFFER_DESC IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _uint[m_iNumIndices]{};

	_uint indicesCount{};
    _myMesh->mFaces.resize(_aimesh->mNumFaces);

	for (size_t i = 0; i < _aimesh->mNumFaces; i++)
	{
        m_pIndices[indicesCount++] = _myMesh->mFaces[i].mIndices[0] = _aimesh->mFaces[i].mIndices[0];
        m_pIndices[indicesCount++] = _myMesh->mFaces[i].mIndices[1] = _aimesh->mFaces[i].mIndices[1];
        m_pIndices[indicesCount++] = _myMesh->mFaces[i].mIndices[2] = _aimesh->mFaces[i].mIndices[2];
	}

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = m_pIndices;

	MSG_FAIL(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB), L"메쉬 인덱스 버퍼 생성에 실패했습니다", L"오류!!!", E_FAIL);
#pragma endregion

    //if(m_bIsAnimated)
    //{
    //    m_pSkinnedVertexPositions = new _float3[m_iNumVertices]{};
    //    *m_pSkinnedVertexPositions = *m_pVertexPositions;
    //}

	return S_OK;
}

HRESULT Engine::Mesh::Initialize_Binary(MODEL _modelType, myMesh* _myMesh, Model* _model, const _fmatrix& _prematrix)
{
     m_strName = _myMesh->mName;

     m_iMaterialIndex = _myMesh->mMaterialIndex;

     m_iNumVertices = _myMesh->mNumVertices;

     m_iIndexStride = 4;

     m_iNumIndices = _myMesh->mNumIndices;

     m_iNumVertexBuffers = 1;
     m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

     HRESULT hr{};

     if (_modelType == MODEL::NONANIM)
     {
         hr = Ready_For_NonAnimation_Binary(_myMesh, _prematrix);
         MSG_FAIL(hr, L"실패", L"왜지", E_FAIL);
     }
     else if (_modelType == MODEL::PLAYERANIM)
     {
         hr = Ready_For_PlayerAnimation_Binary(_myMesh, _model);
         m_bIsAnimated = true;
         MSG_FAIL(hr, L"실패", L"왜지", E_FAIL);
     }
     else
     {
         hr = Ready_For_Animation_Binary(_myMesh, _model);
         m_bIsAnimated = true;
         MSG_FAIL(hr, L"실패", L"왜지", E_FAIL);
     }

#pragma region IndexBuffer
     D3D11_BUFFER_DESC IndexBufferDesc{};
     IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
     IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
     IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
     IndexBufferDesc.CPUAccessFlags = 0;
     IndexBufferDesc.MiscFlags = 0;
     IndexBufferDesc.StructureByteStride = m_iIndexStride;

     m_pIndices = new _uint[m_iNumIndices]{};

     _uint indicesCount{};

     for (size_t i = 0; i < _myMesh->mNumFaces; i++)
     {
         m_pIndices[indicesCount++] = _myMesh->mFaces[i].mIndices[0];
         m_pIndices[indicesCount++] = _myMesh->mFaces[i].mIndices[1];
         m_pIndices[indicesCount++] = _myMesh->mFaces[i].mIndices[2];
     }

     D3D11_SUBRESOURCE_DATA IndexInitialData{};
     IndexInitialData.pSysMem = m_pIndices;

     MSG_FAIL(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB), L"메쉬 인덱스 버퍼 생성에 실패했습니다", L"오류!!!", E_FAIL);
#pragma endregion

     //if(m_bIsAnimated)
     //{
     //    m_pSkinnedVertexPositions = new _float3[m_iNumVertices]{};
     //    *m_pSkinnedVertexPositions = *m_pVertexPositions;
     //}

     if (FAILED(Ready_InstanceBuffer(1000))) return E_FAIL;

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 타입별 생성 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Mesh::Ready_For_NonAnimation(const aiMesh* _aiMesh, const _fmatrix& _preTransformMatrix, myMesh* _myMesh)
{
	m_iVertexStride = sizeof(VTXMESH);

#pragma region VertexBuffer
	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* vertices = new VTXMESH[m_iNumVertices]{};

	m_pVertexPositions = new _float3[m_iNumVertices]{};

    _myMesh->mVertices.resize(m_iNumVertices);
    _myMesh->mNormals.resize(m_iNumVertices);
    _myMesh->mTangents.resize(m_iNumVertices);
    _myMesh->mBiNormals.resize(m_iNumVertices);
    _myMesh->mTexcoords.resize(m_iNumVertices);
    _myMesh->mNumBones = 0;
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&vertices[i].vPosition, &_aiMesh->mVertices[i], sizeof(_float3));
        _myMesh->mVertices[i] = vertices[i].vPosition;
		XMStoreFloat3(&vertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&vertices[i].vPosition), _preTransformMatrix));
		m_pVertexPositions[i] = vertices[i].vPosition;

		memcpy(&vertices[i].vNormal, &_aiMesh->mNormals[i], sizeof(_float3));
        _myMesh->mNormals[i] = vertices[i].vNormal;
        XMStoreFloat3(&vertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&vertices[i].vNormal), _preTransformMatrix)));

		memcpy(&vertices[i].vTangent, &_aiMesh->mTangents[i], sizeof(_float3));
        _myMesh->mTangents[i] = vertices[i].vTangent;
        XMStoreFloat3(&vertices[i].vTangent, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&vertices[i].vTangent), _preTransformMatrix)));

		memcpy(&vertices[i].vBinormal, &_aiMesh->mBitangents[i], sizeof(_float3));
        _myMesh->mBiNormals[i] = vertices[i].vBinormal;
        XMStoreFloat3(&vertices[i].vBinormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&vertices[i].vBinormal), _preTransformMatrix)));

		memcpy(&vertices[i].vTexcoord, &_aiMesh->mTextureCoords[0][i], sizeof(_float2));
        _myMesh->mTexcoords[i] = vertices[i].vTexcoord;
    }

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = vertices;

	MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"메쉬 정점 버퍼 생성에 실패했습니다", L"오류!!!", E_FAIL);

	Safe_Delete_Array(vertices);

#pragma endregion

	return S_OK;
}


HRESULT Engine::Mesh::Ready_For_Animation(const aiMesh* _aiMesh, Model* _model, myMesh* _myMesh)
{
    m_iVertexStride = sizeof(VTXANIMMESH); //VTXPLAYERMESH

#pragma region VertexBuffer
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    m_pVtxInfos = new VTXANIMMESH[m_iNumVertices]{}; //VTXPLAYERMESH
    m_pVertexPositions = new _float3[m_iNumVertices]{};

    _myMesh->mVertices.resize(m_iNumVertices);
    _myMesh->mNormals.resize(m_iNumVertices);
    _myMesh->mTangents.resize(m_iNumVertices);
    _myMesh->mBiNormals.resize(m_iNumVertices);
    _myMesh->mTexcoords.resize(m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&m_pVtxInfos[i].vPosition, &_aiMesh->mVertices[i], sizeof(_float3));
        m_pVertexPositions[i] = m_pVtxInfos[i].vPosition;
        _myMesh->mVertices[i] = m_pVtxInfos[i].vPosition;

        memcpy(&m_pVtxInfos[i].vNormal, &_aiMesh->mNormals[i], sizeof(_float3));
        _myMesh->mNormals[i] = m_pVtxInfos[i].vNormal;

        memcpy(&m_pVtxInfos[i].vTangent, &_aiMesh->mTangents[i], sizeof(_float3));
        _myMesh->mTangents[i] = m_pVtxInfos[i].vTangent;

        memcpy(&m_pVtxInfos[i].vBinormal, &_aiMesh->mBitangents[i], sizeof(_float3));
        _myMesh->mBiNormals[i] = m_pVtxInfos[i].vBinormal;

        memcpy(&m_pVtxInfos[i].vTexcoord, &_aiMesh->mTextureCoords[0][i], sizeof(_float2));
        _myMesh->mTexcoords[i] = m_pVtxInfos[i].vTexcoord;
    }

    m_iNumBones = _myMesh->mNumBones = _aiMesh->mNumBones;
    m_vecBoneIndicies.reserve(m_iNumBones);
    _myMesh->mNumBoneIndices.reserve(m_iNumBones);

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        aiBone* bone = _aiMesh->mBones[i];
        myBone* mybone = new myBone;

        _int boneindex = _model->Get_BoneIndex(bone->mName.data);
        mybone->mName = bone->mName.data;
        mybone->mBoneIndex = boneindex;
        if (boneindex == -1)
        {
            COUT("본을 찾을 수 없음, 루트 본으로 대체 : " << bone->mName.data);
            boneindex = 0;
        }

        m_vecBoneIndicies.push_back(boneindex);
        _myMesh->mNumBoneIndices.push_back(boneindex);

        _float4x4 offsetMatrix{};
        memcpy(&offsetMatrix, &bone->mOffsetMatrix, sizeof(_float4x4));
        TransposeMatrix(offsetMatrix);
        mybone->mOffsetMatrix = offsetMatrix;
        m_vecOffsetMatrices.push_back(offsetMatrix);

        mybone->mNumWeights = bone->mNumWeights;
        for (_uint j = 0; j < bone->mNumWeights; ++j)
        {
            aiVertexWeight weight = bone->mWeights[j];
            myWeight myWeight = {};
            myWeight.mVertexID = weight.mVertexId;
            myWeight.mWeight = weight.mWeight;
            mybone->mWeight.push_back(myWeight);

            if (m_pVtxInfos[weight.mVertexId].vBlendWeight.x == 0.f)
            {
                m_pVtxInfos[weight.mVertexId].vBlendIndex.x = i;
                m_pVtxInfos[weight.mVertexId].vBlendWeight.x = weight.mWeight;
            }
            else if (m_pVtxInfos[weight.mVertexId].vBlendWeight.y == 0.f)
            {
                m_pVtxInfos[weight.mVertexId].vBlendIndex.y = i;
                m_pVtxInfos[weight.mVertexId].vBlendWeight.y = weight.mWeight;
            }
            else if (m_pVtxInfos[weight.mVertexId].vBlendWeight.z == 0.f)
            {
                m_pVtxInfos[weight.mVertexId].vBlendIndex.z = i;
                m_pVtxInfos[weight.mVertexId].vBlendWeight.z = weight.mWeight;
            }
            else
            {
                m_pVtxInfos[weight.mVertexId].vBlendIndex.w = i;
                m_pVtxInfos[weight.mVertexId].vBlendWeight.w = weight.mWeight;
            }
        }
        _myMesh->mBones.push_back(mybone);
    }

    if (m_iNumBones == 0)
    {
        m_iNumBones = 1;
        _float4x4 offsetmatrix = {};
        XMStoreFloat4x4(&offsetmatrix, XMMatrixIdentity());
        m_vecOffsetMatrices.push_back(offsetmatrix);
        _int boneIndex = _model->Get_BoneIndex(m_strName.c_str());

        // 메쉬 이름으로 본을 못 찾으면 루트 본(0번) 사용
        if (boneIndex == -1)
            boneIndex = 0;

        m_vecBoneIndicies.push_back(boneIndex);
    }

    m_iNumAnimMesh = _myMesh->mNumAnimMeshes = _aiMesh->mNumAnimMeshes;
    _myMesh->mHasNormals = _aiMesh->HasNormals();

    if (m_iNumAnimMesh > 0)
    {
        _myMesh->mAnimMeshs.resize(m_iNumAnimMesh);

        m_bHasMorph = true;
        m_vecMorphs.resize(m_iNumAnimMesh);

        _uint totalCount = m_iNumVertices * m_iNumAnimMesh;
        m_pFlatPosDeltas = new _float3[totalCount]{};
        m_pFlatNorDeltas = new _float3[totalCount]{};

        const _float EPSILON = 0.00001f;

        for (_uint i = 0; i < m_iNumAnimMesh; ++i)
        {
            const aiAnimMesh* anim = _aiMesh->mAnimMeshes[i];
            myAnimMesh& myanim = _myMesh->mAnimMeshs[i];

            m_vecMorphs[i].strName = myanim.mName = anim->mName.data;
            m_umapMorphNameToIndex[anim->mName.data] = i;

            // Sparse 데이터를 위한 임시 벡터
            vector<mySparseMorphVertex> tempSparse;
            tempSparse.reserve(m_iNumVertices / 10);  // 약 10% 예상

            for (_uint vertexindex = 0; vertexindex < m_iNumVertices; ++vertexindex)
            {
                _uint idx = i * m_iNumVertices + vertexindex;

                _float3 originalPos = m_pVertexPositions[vertexindex];
                _float3 targetPos = _float3(
                    anim->mVertices[vertexindex].x,
                    anim->mVertices[vertexindex].y,
                    anim->mVertices[vertexindex].z
                );

                _float3 deltaPos = _float3(
                    targetPos.x - originalPos.x,
                    targetPos.y - originalPos.y,
                    targetPos.z - originalPos.z
                );

                _float3 deltaNor = {};
                if (_aiMesh->HasNormals() && anim->mNormals)
                {
                    deltaNor = _float3(
                        anim->mNormals[vertexindex].x - _aiMesh->mNormals[vertexindex].x,
                        anim->mNormals[vertexindex].y - _aiMesh->mNormals[vertexindex].y,
                        anim->mNormals[vertexindex].z - _aiMesh->mNormals[vertexindex].z
                    );
                }

                // 런타임용 full array에 저장
                m_pFlatPosDeltas[idx] = deltaPos;
                m_pFlatNorDeltas[idx] = deltaNor;

                // 델타가 0이 아닌 경우만 sparse에 추가 (바이너리 저장용)
                _float deltaLength = sqrtf(
                    deltaPos.x * deltaPos.x +
                    deltaPos.y * deltaPos.y +
                    deltaPos.z * deltaPos.z
                );

                if (deltaLength > EPSILON)
                {
                    mySparseMorphVertex sv;
                    sv.mVertexIndex = vertexindex;
                    sv.mDeltaPos = deltaPos;
                    sv.mDeltaNor = deltaNor;
                    tempSparse.push_back(sv);
                }
            }

            // Sparse 데이터 저장 (바이너리 파일용)
            myanim.mNumNonZeroVertices = (_uint)tempSparse.size();
            myanim.mSparseVertices = std::move(tempSparse);
        }
    }

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = m_pVtxInfos;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"메쉬 정점 버퍼 생성에 실패했습니다", L"오류!!!", E_FAIL);
#pragma endregion

    return S_OK;
}
HRESULT Engine::Mesh::Ready_For_AnimationPlayer(const aiMesh* _aiMesh, Model* _model, myMesh* _myMesh)
{
    m_iVertexStride = sizeof(VTXPLAYERANIMMESH);

#pragma region VertexBuffer
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    m_pVtxPlyayerInfo = new VTXPLAYERANIMMESH[m_iNumVertices]{};
    m_pVertexPositions = new _float3[m_iNumVertices]{};

    _myMesh->mVertices.resize(m_iNumVertices);
    _myMesh->mNormals.resize(m_iNumVertices);
    _myMesh->mTangents.resize(m_iNumVertices);
    _myMesh->mBiNormals.resize(m_iNumVertices);
    _myMesh->mTexcoords.resize(m_iNumVertices);
    _myMesh->mTexcoords1.resize(m_iNumVertices);
    _myMesh->mHasTexcoords1 = _aiMesh->HasTextureCoords(1);
    if (_myMesh->mHasTexcoords1)
        _myMesh->mTexcoords1.resize(m_iNumVertices);

     for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&m_pVtxPlyayerInfo[i].vPosition, &_aiMesh->mVertices[i], sizeof(_float3));
        m_pVertexPositions[i] = m_pVtxPlyayerInfo[i].vPosition;
        _myMesh->mVertices[i] = m_pVtxPlyayerInfo[i].vPosition;

        memcpy(&m_pVtxPlyayerInfo[i].vNormal, &_aiMesh->mNormals[i], sizeof(_float3));
        _myMesh->mNormals[i] = m_pVtxPlyayerInfo[i].vNormal;

        memcpy(&m_pVtxPlyayerInfo[i].vTangent, &_aiMesh->mTangents[i], sizeof(_float3));
        _myMesh->mTangents[i] = m_pVtxPlyayerInfo[i].vTangent;

        memcpy(&m_pVtxPlyayerInfo[i].vBinormal, &_aiMesh->mBitangents[i], sizeof(_float3));
        _myMesh->mBiNormals[i] = m_pVtxPlyayerInfo[i].vBinormal;

        if (_aiMesh->HasTextureCoords(0))
        {
            memcpy(&m_pVtxPlyayerInfo[i].vTexcoord, &_aiMesh->mTextureCoords[0][i], sizeof(_float2));
            _myMesh->mTexcoords[i] = m_pVtxPlyayerInfo[i].vTexcoord;
        }

        if (_myMesh->mHasTexcoords1)
        {
            memcpy(&m_pVtxPlyayerInfo[i].vTexcoord1, &_aiMesh->mTextureCoords[1][i], sizeof(_float2));
            _myMesh->mTexcoords1[i] = m_pVtxPlyayerInfo[i].vTexcoord1;
        }
        else
        {
            m_pVtxPlyayerInfo[i].vTexcoord1 = _float2(0.f, 0.f);
        }
    }


    m_iNumBones = _myMesh->mNumBones = _aiMesh->mNumBones;
    m_vecBoneIndicies.reserve(m_iNumBones);
    _myMesh->mNumBoneIndices.reserve(m_iNumBones);

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        aiBone* bone = _aiMesh->mBones[i];
        myBone* mybone = new myBone;

        _int boneindex = _model->Get_BoneIndex(bone->mName.data);
        mybone->mName = bone->mName.data;
        mybone->mBoneIndex = boneindex;
        if (boneindex == -1)
            return E_FAIL;

        m_vecBoneIndicies.push_back(boneindex);
        _myMesh->mNumBoneIndices.push_back(boneindex);

        _float4x4 offsetMatrix{};
        memcpy(&offsetMatrix, &bone->mOffsetMatrix, sizeof(_float4x4));
        TransposeMatrix(offsetMatrix);
        mybone->mOffsetMatrix = offsetMatrix;
        m_vecOffsetMatrices.push_back(offsetMatrix);

        mybone->mNumWeights = bone->mNumWeights;
        for (_uint j = 0; j < bone->mNumWeights; ++j)
        {
            aiVertexWeight weight = bone->mWeights[j];
            myWeight myWeight = {};
            myWeight.mVertexID = weight.mVertexId;
            myWeight.mWeight = weight.mWeight;
            mybone->mWeight.push_back(myWeight); //본마다 weight를 모아

            if (m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight.x == 0.f)
            {
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendIndex.x = i;
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight.x = weight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight.y == 0.f)
            {
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendIndex.y = i;
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight.y = weight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight.z == 0.f)
            {
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendIndex.z = i;
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight.z = weight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight.w == 0.f)
            {
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendIndex.w = i;
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight.w = weight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight2.x == 0.f)
            {
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendIndex2.x = i;
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight2.x = weight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight2.y == 0.f)
            {
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendIndex2.y = i;
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight2.y = weight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight2.z == 0.f)
            {
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendIndex2.z = i;
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight2.z = weight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight2.w == 0.f)
            {
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendIndex2.w = i;
                m_pVtxPlyayerInfo[weight.mVertexId].vBlendWeight2.w = weight.mWeight;
            }

        }
        _myMesh->mBones.push_back(mybone);
    }

    if (m_iNumBones == 0)
    {
        m_iNumBones = 1;
        _float4x4 offsetmatrix = {};
        XMStoreFloat4x4(&offsetmatrix, XMMatrixIdentity());
        m_vecOffsetMatrices.push_back(offsetmatrix);
        _int boneIndex = _model->Get_BoneIndex(m_strName.c_str());

        // 메쉬 이름으로 본을 못 찾으면 루트 본(0번) 사용
        if (boneIndex == -1)
            boneIndex = 0;

        m_vecBoneIndicies.push_back(boneIndex);
    }

    m_iNumAnimMesh = _myMesh->mNumAnimMeshes = _aiMesh->mNumAnimMeshes;
    _myMesh->mHasNormals = _aiMesh->HasNormals();

    if (m_iNumAnimMesh > 0)
    {
        _myMesh->mAnimMeshs.resize(m_iNumAnimMesh);

        m_bHasMorph = true;
        m_vecMorphs.resize(m_iNumAnimMesh);

        _uint totalCount = m_iNumVertices * m_iNumAnimMesh;
        m_pFlatPosDeltas = new _float3[totalCount]{};
        m_pFlatNorDeltas = new _float3[totalCount]{};

        const _float EPSILON = 0.00001f;

        for (_uint i = 0; i < m_iNumAnimMesh; ++i)
        {
            const aiAnimMesh* anim = _aiMesh->mAnimMeshes[i];
            myAnimMesh& myanim = _myMesh->mAnimMeshs[i];

            m_vecMorphs[i].strName = myanim.mName = anim->mName.data;
            m_umapMorphNameToIndex[anim->mName.data] = i;

            // Sparse 데이터를 위한 임시 벡터
            vector<mySparseMorphVertex> tempSparse;
            tempSparse.reserve(m_iNumVertices / 10);  // 약 10% 예상

            for (_uint vertexindex = 0; vertexindex < m_iNumVertices; ++vertexindex)
            {
                _uint idx = i * m_iNumVertices + vertexindex;

                _float3 originalPos = m_pVertexPositions[vertexindex];
                _float3 targetPos = _float3(
                    anim->mVertices[vertexindex].x,
                    anim->mVertices[vertexindex].y,
                    anim->mVertices[vertexindex].z
                );

                _float3 deltaPos = _float3(
                    targetPos.x - originalPos.x,
                    targetPos.y - originalPos.y,
                    targetPos.z - originalPos.z
                );

                _float3 deltaNor = {};
                if (_aiMesh->HasNormals() && anim->mNormals)
                {
                    deltaNor = _float3(
                        anim->mNormals[vertexindex].x - _aiMesh->mNormals[vertexindex].x,
                        anim->mNormals[vertexindex].y - _aiMesh->mNormals[vertexindex].y,
                        anim->mNormals[vertexindex].z - _aiMesh->mNormals[vertexindex].z
                    );
                }

                // 런타임용 full array에 저장
                m_pFlatPosDeltas[idx] = deltaPos;
                m_pFlatNorDeltas[idx] = deltaNor;

                // 델타가 0이 아닌 경우만 sparse에 추가 (바이너리 저장용)
                _float deltaLength = sqrtf(
                    deltaPos.x * deltaPos.x +
                    deltaPos.y * deltaPos.y +
                    deltaPos.z * deltaPos.z
                );

                if (deltaLength > EPSILON)
                {
                    mySparseMorphVertex sv;
                    sv.mVertexIndex = vertexindex;
                    sv.mDeltaPos = deltaPos;
                    sv.mDeltaNor = deltaNor;
                    tempSparse.push_back(sv);
                }
            }

            // Sparse 데이터 저장 (바이너리 파일용)
            myanim.mNumNonZeroVertices = (_uint)tempSparse.size();
            myanim.mSparseVertices = std::move(tempSparse);
        }
    }

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = m_pVtxPlyayerInfo;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"메쉬 정점 버퍼 생성에 실패했습니다", L"오류!!!", E_FAIL);
#pragma endregion

    return S_OK;
}
/******************************************************* 타입별 생성 함수_FBX *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수_Binary ////////////////////////////////////////////////////////
HRESULT Engine::Mesh::Ready_For_NonAnimation_Binary(myMesh* _myMesh, const _fmatrix& _preTransformMatrix)
{
    m_iVertexStride = sizeof(VTXMESH);

#pragma region VertexBuffer
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXMESH* vertices = new VTXMESH[m_iNumVertices]{};

    m_pVertexPositions = new _float3[m_iNumVertices]{};

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        vertices[i].vPosition = _myMesh->mVertices[i];
        XMStoreFloat3(&vertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&vertices[i].vPosition), _preTransformMatrix));
        m_pVertexPositions[i] = vertices[i].vPosition;

        vertices[i].vNormal = _myMesh->mNormals[i];
        XMStoreFloat3(&vertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&vertices[i].vNormal), _preTransformMatrix)));

        vertices[i].vTangent = _myMesh->mTangents[i];
        XMStoreFloat3(&vertices[i].vTangent, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&vertices[i].vTangent), _preTransformMatrix)));


        vertices[i].vBinormal = _myMesh->mBiNormals[i] ;
        XMStoreFloat3(&vertices[i].vBinormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&vertices[i].vBinormal), _preTransformMatrix)));

        vertices[i].vTexcoord = _myMesh->mTexcoords[i];
    }

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = vertices;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"메쉬 정점 버퍼 생성에 실패했습니다", L"오류!!!", E_FAIL);

    Safe_Delete_Array(vertices);

#pragma endregion

    return S_OK;
}

HRESULT Engine::Mesh::Ready_For_PlayerAnimation_Binary(myMesh* _myMesh, Model* _model)
{
    m_iVertexStride = sizeof(VTXPLAYERANIMMESH);

#pragma region VertexBuffer
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    m_pVtxPlyayerInfo = new VTXPLAYERANIMMESH[m_iNumVertices]{};

    m_pVertexPositions = new _float3[m_iNumVertices]{};

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        m_pVtxPlyayerInfo[i].vPosition = _myMesh->mVertices[i];
        m_pVertexPositions[i] = m_pVtxPlyayerInfo[i].vPosition;

        m_pVtxPlyayerInfo[i].vNormal = _myMesh->mNormals[i];

        m_pVtxPlyayerInfo[i].vTangent = _myMesh->mTangents[i];

        m_pVtxPlyayerInfo[i].vBinormal = _myMesh->mBiNormals[i];

        m_pVtxPlyayerInfo[i].vTexcoord = _myMesh->mTexcoords[i];

        if (_myMesh->mHasTexcoords1)
            m_pVtxPlyayerInfo[i].vTexcoord1 = _myMesh->mTexcoords1[i];
        else
            m_pVtxPlyayerInfo[i].vTexcoord1 = _float2(0.f, 0.f);
    }

    m_iNumBones = _myMesh->mNumBones;
    m_vecBoneIndicies.reserve(m_iNumBones);

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        myBone* mybone = _myMesh->mBones[i];

        _int boneindex = mybone->mBoneIndex;
        mybone->mName = mybone->mName;
        if (boneindex == -1)
            return E_FAIL;

        m_vecBoneIndicies.push_back(boneindex);

        m_vecOffsetMatrices.push_back(mybone->mOffsetMatrix);

        /* i번째 뼈는 몇개의 정점에게 영향을 주는가? */
        for (_uint j = 0; j < mybone->mNumWeights; ++j)
        {
            myWeight myWeight = mybone->mWeight[j];

            /* AIWeight.mVertexId : i번째 뼈가 영향을 주는 j번째 정점의 인덱스 */
            if (m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight.x == 0.f)
            {
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendIndex.x = i;
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight.x = myWeight.mWeight;
            }

            else if (m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight.y == 0.f)
            {
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendIndex.y = i;
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight.y = myWeight.mWeight;
            }

            else if (m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight.z == 0.f)
            {
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendIndex.z = i;
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight.z = myWeight.mWeight;
            }
            else if(m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight.w == 0.f)
            {
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendIndex.w = i;
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight.w = myWeight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight2.x == 0.f)
            {
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendIndex2.x = i;
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight2.x = myWeight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight2.y == 0.f)
            {
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendIndex2.y = i;
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight2.y = myWeight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight2.z == 0.f)
            {
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendIndex2.z = i;
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight2.z = myWeight.mWeight;
            }
            else if (m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight2.w == 0.f)
            {
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendIndex2.w = i;
                m_pVtxPlyayerInfo[myWeight.mVertexID].vBlendWeight2.w = myWeight.mWeight;
            }
        }
    }

    if (m_iNumBones == 0)
    {
        m_iNumBones = 1;

        _float4x4 offsetmatrix = {};
        XMStoreFloat4x4(&offsetmatrix, XMMatrixIdentity());

        m_vecOffsetMatrices.push_back(offsetmatrix);
        m_vecBoneIndicies.push_back(_model->Get_BoneIndex(m_strName.c_str()));
    }

    // AnimMesh (Morph) 처리 - Sparse -> Full Array 확장
    m_iNumAnimMesh = _myMesh->mNumAnimMeshes;

    if (m_iNumAnimMesh > 0)
    {
        m_bHasMorph = true;
        m_vecMorphs.resize(m_iNumAnimMesh);

        // 런타임용 full array 생성 (0으로 초기화됨)
        _uint totalCount = m_iNumVertices * m_iNumAnimMesh;
        m_pFlatPosDeltas = new _float3[totalCount]{};
        m_pFlatNorDeltas = new _float3[totalCount]{};

        for (_uint i = 0; i < m_iNumAnimMesh; ++i)
        {
            const myAnimMesh& myanim = _myMesh->mAnimMeshs[i];

            m_vecMorphs[i].strName = myanim.mName;
            m_umapMorphNameToIndex[myanim.mName] = i;

            // Sparse → Full Array 확장
            for (const auto& sv : myanim.mSparseVertices)
            {
                _uint idx = i * m_iNumVertices + sv.mVertexIndex;
                m_pFlatPosDeltas[idx] = sv.mDeltaPos;
                m_pFlatNorDeltas[idx] = sv.mDeltaNor;
            }
        }
    }

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = m_pVtxPlyayerInfo;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"메쉬 정점 버퍼 생성에 실패했습니다", L"오류!!!", E_FAIL);
#pragma endregion

    return S_OK;
}

HRESULT Engine::Mesh::Ready_For_Animation_Binary(myMesh* _myMesh, Model* _model)
{
    m_iVertexStride = sizeof(VTXANIMMESH); //VTXPLAYERMESH

#pragma region VertexBuffer
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    m_pVtxInfos = new VTXANIMMESH[m_iNumVertices]{}; //VTXPLAYERMESH

    m_pVertexPositions = new _float3[m_iNumVertices]{};

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        m_pVtxInfos[i].vPosition = _myMesh->mVertices[i];
        m_pVertexPositions[i] = m_pVtxInfos[i].vPosition;

        m_pVtxInfos[i].vNormal = _myMesh->mNormals[i];

        m_pVtxInfos[i].vTangent = _myMesh->mTangents[i];

        m_pVtxInfos[i].vBinormal = _myMesh->mBiNormals[i];

        m_pVtxInfos[i].vTexcoord = _myMesh->mTexcoords[i];
    }

    m_iNumBones = _myMesh->mNumBones;
    m_vecBoneIndicies.reserve(m_iNumBones);

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        myBone* mybone = _myMesh->mBones[i];

        _int boneindex = mybone->mBoneIndex;
        mybone->mName = mybone->mName;
        if (boneindex == -1)
            return E_FAIL;

        m_vecBoneIndicies.push_back(boneindex);

        m_vecOffsetMatrices.push_back(mybone->mOffsetMatrix);

        /* i번째 뼈는 몇개의 정점에게 영향을 주는가? */
        for (_uint j = 0; j < mybone->mNumWeights; ++j)
        {
            myWeight myWeight = mybone->mWeight[j];
            
            /* AIWeight.mVertexId : i번째 뼈가 영향을 주는 j번째 정점의 인덱스 */
            if (m_pVtxInfos[myWeight.mVertexID].vBlendWeight.x == 0.f)
            {
                m_pVtxInfos[myWeight.mVertexID].vBlendIndex.x = i;
                m_pVtxInfos[myWeight.mVertexID].vBlendWeight.x = myWeight.mWeight;
            }

            else if (m_pVtxInfos[myWeight.mVertexID].vBlendWeight.y == 0.f)
            {
                m_pVtxInfos[myWeight.mVertexID].vBlendIndex.y = i;
                m_pVtxInfos[myWeight.mVertexID].vBlendWeight.y = myWeight.mWeight;
            }

            else if (m_pVtxInfos[myWeight.mVertexID].vBlendWeight.z == 0.f)
            {
                m_pVtxInfos[myWeight.mVertexID].vBlendIndex.z = i;
                m_pVtxInfos[myWeight.mVertexID].vBlendWeight.z = myWeight.mWeight;
            }
            else
            {
                m_pVtxInfos[myWeight.mVertexID].vBlendIndex.w = i;
                m_pVtxInfos[myWeight.mVertexID].vBlendWeight.w = myWeight.mWeight;
            }
            //else if (m_pVtxInfos[myWeight.mVertexID].vBlendWeight2.x == 0.f)
            //{
            //    m_pVtxInfos[myWeight.mVertexID].vBlendIndex2.x = i;
            //    m_pVtxInfos[myWeight.mVertexID].vBlendWeight2.x = myWeight.mWeight;
            //}
            //else if (m_pVtxInfos[myWeight.mVertexID].vBlendWeight2.y == 0.f)
            //{
            //    m_pVtxInfos[myWeight.mVertexID].vBlendIndex2.y = i;
            //    m_pVtxInfos[myWeight.mVertexID].vBlendWeight2.y = myWeight.mWeight;
            //}
            //else if (m_pVtxInfos[myWeight.mVertexID].vBlendWeight2.z == 0.f)
            //{
            //    m_pVtxInfos[myWeight.mVertexID].vBlendIndex2.z = i;
            //    m_pVtxInfos[myWeight.mVertexID].vBlendWeight2.z = myWeight.mWeight;
            //}
            //else if (m_pVtxInfos[myWeight.mVertexID].vBlendWeight2.w == 0.f)
            //{
            //    m_pVtxInfos[myWeight.mVertexID].vBlendIndex2.w = i;
            //    m_pVtxInfos[myWeight.mVertexID].vBlendWeight2.w = myWeight.mWeight;
            //}
        }
    }

    if (m_iNumBones == 0)
    {
        m_iNumBones = 1;

        _float4x4 offsetmatrix = {};
        XMStoreFloat4x4(&offsetmatrix, XMMatrixIdentity());

        m_vecOffsetMatrices.push_back(offsetmatrix);
        m_vecBoneIndicies.push_back(_model->Get_BoneIndex(m_strName.c_str()));
    }

    // AnimMesh (Morph) 처리 - Sparse -> Full Array 확장
    m_iNumAnimMesh = _myMesh->mNumAnimMeshes;

    if (m_iNumAnimMesh > 0)
    {
        m_bHasMorph = true;
        m_vecMorphs.resize(m_iNumAnimMesh);

        // 런타임용 full array 생성 (0으로 초기화됨)
        _uint totalCount = m_iNumVertices * m_iNumAnimMesh;
        m_pFlatPosDeltas = new _float3[totalCount]{};
        m_pFlatNorDeltas = new _float3[totalCount]{};

        for (_uint i = 0; i < m_iNumAnimMesh; ++i)
        {
            const myAnimMesh& myanim = _myMesh->mAnimMeshs[i];

            m_vecMorphs[i].strName = myanim.mName;
            m_umapMorphNameToIndex[myanim.mName] = i;

            // Sparse → Full Array 확장
            for (const auto& sv : myanim.mSparseVertices)
            {
                _uint idx = i * m_iNumVertices + sv.mVertexIndex;
                m_pFlatPosDeltas[idx] = sv.mDeltaPos;
                m_pFlatNorDeltas[idx] = sv.mDeltaNor;
            }
        }
    }

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = m_pVtxInfos;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"메쉬 정점 버퍼 생성에 실패했습니다", L"오류!!!", E_FAIL);
#pragma endregion

    return S_OK;
}
/******************************************************* 타입별 생성 함수_Binary *******************************************************/



//////////////////////////////////////////////////////// 스키닝 메쉬 업데이트 ////////////////////////////////////////////////////////
void Engine::Mesh::Update_Skinned_VertexPositions(const vector<Bone*>& _bones)
{
    if (!m_bIsAnimated) return;

    for (_uint i = 0; i < m_iNumVertices; ++i)
    {
        _vector vFinal = XMVectorZero();
        _float totalWeight = 0.f;

        // VB에서 Blend 정보 가져오기 (Lock 필요시)

        for (int b = 0; b < 4; ++b)
        {
            _float weight = reinterpret_cast<const float*>(&m_pVtxInfos[i].vBlendWeight)[b];
            if (weight <= 0.f) continue;

            _uint boneIdx = m_vecBoneIndicies[(_uint)reinterpret_cast<const float*>(&m_pVtxInfos[i].vBlendIndex)[b]];
            _matrix boneMat = _bones[boneIdx]->Get_CombinedTransformationMatrix();
            _matrix offsetMat = XMLoadFloat4x4(&m_vecOffsetMatrices[(_uint)reinterpret_cast<const float*>(&m_pVtxInfos[i].vBlendIndex)[b]]);

            _matrix skinMat = XMMatrixMultiply(offsetMat, boneMat);
            _vector vBind = XMLoadFloat3(&m_pVertexPositions[i]);  // 고정된 Bind Pose
            _vector vSkinned = XMVector3TransformCoord(vBind, skinMat);

            vFinal = XMVectorAdd(vFinal, XMVectorScale(vSkinned, weight));
            totalWeight += weight;
        }

        if (totalWeight > 0.f)
            vFinal = XMVectorDivide(vFinal, XMVectorReplicate(totalWeight));

        // 메쉬 피킹할때 필요한건데 이제 쉐이더 픽셀 피킹할거니까 필요없어짐
        // 함수는 우선 남겨둠 
        // 아래거 주석 처리 함으로써 기능은 없는거나 다름없지만
        //XMStoreFloat3(&m_pSkinnedVertexPositions[i], vFinal);  // 피킹용만 업데이트!
    }
}

void Engine::Mesh::Calculate_LocalAABB()
{
    // 계산 할 정점이 없으면
    if (m_pVertexPositions == nullptr || m_iNumVertices == 0)
    {
        /* 기본값 */
        m_tLocalAABB.Center = _float3(0.f, 0.f, 0.f);
        m_tLocalAABB.Extents = _float3(0.5f, 0.5f, 0.5f);
        return;
    }

    // 정점들의 min값과 max값 계산
    _float3 vMin = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
    _float3 vMax = _float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    // 모든 정점들을 순회하면서 확인
    for (_uint i = 0; i < m_iNumVertices; ++i)
    {
        const _float3& pos = m_pVertexPositions[i];
        
        // 가장 작은 x, y, z 찾기
        vMin.x = min(vMin.x, pos.x);
        vMin.y = min(vMin.y, pos.y);
        vMin.z = min(vMin.z, pos.z);

        // 가장 큰 x, y, z 찾기
        vMax.x = max(vMax.x, pos.x);
        vMax.y = max(vMax.y, pos.y);
        vMax.z = max(vMax.z, pos.z);
    }

    /* Center = (Min + Max) / 2 */
    m_tLocalAABB.Center = _float3(
        (vMin.x + vMax.x) * 0.5f,
        (vMin.y + vMax.y) * 0.5f,
        (vMin.z + vMax.z) * 0.5f
    );

    /* Extents = (Max - Min) / 2 */
    m_tLocalAABB.Extents = _float3(
        (vMax.x - vMin.x) * 0.5f,
        (vMax.y - vMin.y) * 0.5f,
        (vMax.z - vMin.z) * 0.5f
    );
}
/******************************************************* 스키닝 메쉬 업데이트 *******************************************************/

_uint Engine::Mesh::FallBackMasterBoneIndex(_uint iPartBoneIndex, const vector<_uint>& _RemapBone, const vector<class Bone*>& PartBone)
{
    if (_RemapBone[iPartBoneIndex] != g_INVALID) //리맵본의 인덱스가 g_Invalid가 아니라면 매핑 성공한 파츠들은 바로 return 
        return _RemapBone[iPartBoneIndex];

    //여기서부터 매핑 안된 애들의 경우 부모를 찾아주기 위해서 부모 타고올라가면서 본 찾기

    _int iParentBoneIndex = PartBone[iPartBoneIndex]->Get_ParentBoneIndex(); //이게 이제 파츠본의 인덱스에 접근해서 부모인덱스 가져오고

    while(iParentBoneIndex>=0)
    {
        _uint uiParentBoneIndex = (_uint)iParentBoneIndex; //이름 같아서 ui로 uint 구분해놓음

        if (_RemapBone[uiParentBoneIndex] != g_INVALID) //리맵된본의 부모인덱스가 g_Invalid가 아니라면 매핑 성공한 파츠들
            return _RemapBone[uiParentBoneIndex]; //부모인덱스 넘겨주고 , 결론저긍로 부모 fallback 해주는거고

        iParentBoneIndex = PartBone[uiParentBoneIndex]->Get_ParentBoneIndex(); 
    }

    return 0; // 몾찾으면 Root(0번) 최상위로 강제
}

HRESULT Engine::Mesh::Create_RemapBuffer(Model* pPartModel, Model* pMasterModel, const vector<_uint>& vecRemapBone)
{
    //파츠 바뀔떄는 해당 버퍼를 해제하기 위해서
    Safe_Release(m_pRemapBuffer);
    Safe_Release(m_pReampSRV);
    
    vector<_uint> ReampBone;
    ReampBone.resize(m_iNumBones);

    auto& PartBones = pPartModel->Get_Bones();
    
    for (_uint i = 0; i < m_iNumBones; ++i)
    {

        _uint iPartBoneIndex = m_vecBoneIndicies[i];

        if (iPartBoneIndex >= PartBones.size())
        {
            ReampBone[i] = 0;
            continue;
        }

        _uint iMasterBoneIndex = FallBackMasterBoneIndex(iPartBoneIndex, vecRemapBone, PartBones);

        if (iMasterBoneIndex == g_INVALID)
        {
            //XMStoreFloat4x4(&m_matBoneMatrices[i], XMMatrixIdentity());
            cout << "부모,루트가 아닌 뼈 인덱스" << endl;
        }

        ReampBone[i] = iMasterBoneIndex;

    }

    //ReampBuffer 
    D3D11_BUFFER_DESC Desc = {}; 
    Desc.ByteWidth = sizeof(_uint) * m_iNumBones;
    Desc.Usage = D3D11_USAGE_IMMUTABLE;
    Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    Desc.StructureByteStride = sizeof(_uint);
   
    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = ReampBone.data();

    CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, &m_pRemapBuffer), E_FAIL);

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    SRVDesc.Buffer.FirstElement = 0;
    SRVDesc.Buffer.NumElements = m_iNumBones;
    CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pRemapBuffer, &SRVDesc, &m_pReampSRV), E_FAIL);


    return S_OK;
}

HRESULT Engine::Mesh::Create_StandAloneRemapBuffer(class Model* pMyModel)
{
    Safe_Release(m_pRemapBuffer);
    Safe_Release(m_pReampSRV);

 

    auto& MyBones = pMyModel->Get_Bones();
    _uint iNumBones = MyBones.size();

    vector<_uint> ReampBone;
    ReampBone.resize(iNumBones);

    for (_uint i = 0; i < m_iNumBones; ++i)
    {

        _uint iMyBoneIndex = m_vecBoneIndicies[i];

        if (iMyBoneIndex >= MyBones.size())
        {
            ReampBone[i] = 0;
            continue;
        }

        ReampBone[i] = iMyBoneIndex;
    }


    //ReampBuffer 
    D3D11_BUFFER_DESC Desc = {};
    Desc.ByteWidth = sizeof(_uint) * iNumBones;
    Desc.Usage = D3D11_USAGE_IMMUTABLE;
    Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    Desc.StructureByteStride = sizeof(_uint);

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = ReampBone.data();

    CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, &m_pRemapBuffer), E_FAIL);

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    SRVDesc.Buffer.FirstElement = 0;
    SRVDesc.Buffer.NumElements = iNumBones;
    CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pRemapBuffer, &SRVDesc, &m_pReampSRV), E_FAIL);

    return S_OK;
}

//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Mesh::Bind_Bones(Shader* _shader, const _string& _constantname, const vector<Bone*>& _bones)
{
    //OutputDebugStringA(("Mesh: " + m_strName + "\n").c_str());
    //OutputDebugStringA(("  m_iNumBones: " + std::to_string(m_iNumBones) + "\n").c_str());
    //OutputDebugStringA(("  _bones.size(): " + std::to_string(_bones.size()) + "\n").c_str());

    //for (_uint i = 0; i < m_iNumBones; ++i)
    //{
    //    OutputDebugStringA(("  BoneIndex[" + std::to_string(i) + "]: " + std::to_string(m_vecBoneIndicies[i]) + "\n").c_str());
    //}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&m_matBoneMatrices[i],
			XMLoadFloat4x4(&m_vecOffsetMatrices[i]) * _bones[m_vecBoneIndicies[i]]->Get_CombinedTransformationMatrix());
	}

	return _shader->Bind_Matrices_ByHandle(g_BonesMatrices, m_matBoneMatrices, m_iNumBones);
}

HRESULT Engine::Mesh::Bind_BonesMasterRig(Shader* _shader, const _string& _constantname, const vector<class Bone*>& _MasterRigBones, const vector<_uint>& _RemapBone, const vector<class Bone*>& Bones)
{
    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        //m_vecBonIndicies가 이제 모델이 가지고 있는 뼈 전체에서 이름 비교해서 Ready단계에서 저장해놓은 인덱스이고

        _uint iPartBoneIndex = m_vecBoneIndicies[i]; //메시에 영향을 주는 인덱스이고
        //_uint iMasterRigBoneIndex = _RemapBone[iPartBoneIndex]; //이게 이제 내뼈(파츠)와 마스터본의 뼈와 이름비교해서 리맵한 마스터본의 인덱스

        _uint iMasterRigBoneIndex = FallBackMasterBoneIndex(iPartBoneIndex, _RemapBone, Bones);

        if (iMasterRigBoneIndex == g_INVALID)
        {
            //XMStoreFloat4x4(&m_matBoneMatrices[i], XMMatrixIdentity());
            cout << "부모,루트가 아닌 뼈 인덱스" << endl;
            continue;
        }

        XMStoreFloat4x4(&m_matBoneMatrices[i],
            XMLoadFloat4x4(&m_vecOffsetMatrices[i]) * _MasterRigBones[iMasterRigBoneIndex]->Get_CombinedTransformationMatrix());
    }

    return _shader->Bind_Matrices_ByHandle(g_BonesMatrices, m_matBoneMatrices, m_iNumBones);
}

HRESULT Engine::Mesh::Bind_Morphs(Shader* _shader, const vector<_string>& _morphNames, const vector<_float>& _weights)
{
    _float4 weightsPacked[8] = {};
    _uint4 indicesPacked[8] = {};
    _uint activeCount = 0;

    for (_uint i = 0; i < _morphNames.size() && activeCount < 32; ++i)
    {
        if (_weights[i] <= 0.001f)
            continue;

        // [핵심: Model 이름 → Mesh 로컬 인덱스 변환]
        auto it = m_umapMorphNameToIndex.find(_morphNames[i]);
        if (it == m_umapMorphNameToIndex.end())
            continue;  // 이 Mesh에는 해당 모프 없음

        _uint meshLocalIndex = it->second;  // Mesh의 로컬 인덱스

        _uint vecIdx = activeCount / 4;
        _uint component = activeCount % 4;

        reinterpret_cast<_float*>(&weightsPacked[vecIdx])[component] = _weights[i];
        reinterpret_cast<_uint*>(&indicesPacked[vecIdx])[component] = meshLocalIndex;  // 로컬 인덱스!

        ++activeCount;
    }

    _shader->Bind_RawValue_FullSlot(5, "g_MorphWeightsPacked", weightsPacked, sizeof(_float4) * 8);
    _shader->Bind_RawValue_FullSlot(6, "g_MorphIndicesPacked", indicesPacked, sizeof(_uint4) * 8);



    MorphInfos morphInfos;

    morphInfos.numActiveMorphs = activeCount;
    morphInfos.numVertices = m_iNumVertices;

    _shader->Bind_EntireBuffer_BySlot(7, &morphInfos, sizeof(MorphInfos));

    if (m_bHasMorph && m_pMorphPosSRV && m_pMorphNorSRV)
    {
        m_pContext->VSSetShaderResources(10, 1, &m_pMorphPosSRV);
        m_pContext->VSSetShaderResources(11, 1, &m_pMorphNorSRV);
    }

    return S_OK;
}
HRESULT Engine::Mesh::Bind_RemapSRV(Shader* pShader)
{
    pShader->Bind_SRV_FullSlot(22, m_pReampSRV, stage_VS);

    return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 모프 애니메이션용 쉐이더 버퍼 생성함수 ////////////////////////////////////////////////////////
HRESULT Engine::Mesh::Register_StructuredBuffer(Shader* _shader)
{
    if (m_bHasMorph && m_pMorphPosBuffer == nullptr && m_pMorphPosSRV == nullptr)
    {
        //COUT("=== Creating SRV for Mesh: " << m_strName << " ===");
        //COUT("  NumAnimMeshes: " << m_iNumAnimMesh);
        //COUT("  NumVertices: " << m_iNumVertices);
        //COUT("  Total deltas: " << (m_iNumVertices * m_iNumAnimMesh));

        _uint elementCount = m_iNumVertices * m_iNumAnimMesh;

        HRESULT hr = m_pGameInstance->CreateStructuredBuffer(
            m_pFlatPosDeltas,
            elementCount,
            sizeof(_float3),
            &m_pMorphPosBuffer,
            &m_pMorphPosSRV
        );

        if (FAILED(hr))
        {
            //COUT("Position Buffer 생성 실패!");
            return E_FAIL;
        }

        hr = m_pGameInstance->CreateStructuredBuffer(
            m_pFlatNorDeltas,
            elementCount,
            sizeof(_float3),
            &m_pMorphNorBuffer,
            &m_pMorphNorSRV
        );

        if (FAILED(hr))
        {
            //COUT("Normal Buffer 생성 실패!");
            return E_FAIL;
        }

        //COUT("SRV 생성 성공!");
        //COUT("Pos SRV: " << (void*)m_pMorphPosSRV);
        //COUT("Nor SRV: " << (void*)m_pMorphNorSRV);
    }

    return S_OK;
}

/******************************************************* 모프 애니메이션용 쉐이더 버퍼 생성함수 *******************************************************/



//////////////////////////////////////////////////////// 인스턴싱 ////////////////////////////////////////////////////////
HRESULT Engine::Mesh::Ready_InstanceBuffer(_uint iNumMaxInstance)
{
    if (m_pInstanceBuffer != nullptr)
        return S_OK;

    m_iInstanceBuffer = iNumMaxInstance; //필요한만큼저장하기

    D3D11_BUFFER_DESC tBufferDesc;
    ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

    tBufferDesc.ByteWidth = sizeof(_float4x4) * iNumMaxInstance;
    tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    tBufferDesc.MiscFlags = 0;
    tBufferDesc.StructureByteStride = sizeof(_float4x4);

    if (FAILED(m_pDevice->CreateBuffer(&tBufferDesc, nullptr, &m_pInstanceBuffer)))
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::Mesh::Render_Instancing(const vector<_float4x4>& InstanceData)
{
    if (InstanceData.empty())
        return S_OK;

    m_iNumInstance = (_uint)InstanceData.size();

    //부족하면 자동으로 늘릴수있게
    if (m_pInstanceBuffer == nullptr || m_iNumInstance > m_iInstanceBuffer)
    {
        Safe_Release(m_pInstanceBuffer);
        m_iInstanceBuffer = max(1000u, m_iNumInstance);

        D3D11_BUFFER_DESC tBufferDesc;
        ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

        tBufferDesc.ByteWidth = sizeof(_float4x4) * m_iInstanceBuffer;
        tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        tBufferDesc.MiscFlags = 0;
        tBufferDesc.StructureByteStride = sizeof(_float4x4);

        if (FAILED(m_pDevice->CreateBuffer(&tBufferDesc, nullptr, &m_pInstanceBuffer)))
        {
            return E_FAIL;
        }
    }

    D3D11_MAPPED_SUBRESOURCE tMappedSubResource;
    ZeroMemory(&tMappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMappedSubResource);

    memcpy(tMappedSubResource.pData, InstanceData.data(), sizeof(_float4x4) * m_iNumInstance);

    m_pContext->Unmap(m_pInstanceBuffer, 0);

    ID3D11Buffer* pBuffers[] = { m_pVB, m_pInstanceBuffer };
    _uint iStrides[] = { m_iVertexStride, sizeof(_float4x4) };
    _uint iOffsets[] = { 0, 0 };

    m_pContext->IASetVertexBuffers(0, 2, pBuffers, iStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

    m_pContext->DrawIndexedInstanced(m_iNumIndices, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

set<_uint> Engine::Mesh::Get_ActiveBoneIndices() const
{
    set<_uint> activeIndices = {};

    // PLAYERANIM / ANIM 둘 다 지원
    if (m_pVtxPlyayerInfo != nullptr)
    {
        for (_uint i = 0; i < m_iNumVertices; ++i)
        {
            const _float4& weight = m_pVtxPlyayerInfo[i].vBlendWeight;
            const _uint4& index = m_pVtxPlyayerInfo[i].vBlendIndex;

            if (weight.x > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index.x]);
            if (weight.y > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index.y]);
            if (weight.z > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index.z]);
            if (weight.w > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index.w]);

            // PLAYERANIM은 BlendWeight2도 있음
            const _float4& weight2 = m_pVtxPlyayerInfo[i].vBlendWeight2;
            const _uint4& index2 = m_pVtxPlyayerInfo[i].vBlendIndex2;

            if (weight2.x > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index2.x]);
            if (weight2.y > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index2.y]);
            if (weight2.z > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index2.z]);
            if (weight2.w > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index2.w]);
        }
    }
    else if (m_pVtxInfos != nullptr)
    {
        for (_uint i = 0; i < m_iNumVertices; ++i)
        {
            const _float4& weight = m_pVtxInfos[i].vBlendWeight;
            const _uint4& index = m_pVtxInfos[i].vBlendIndex;

            if (weight.x > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index.x]);
            if (weight.y > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index.y]);
            if (weight.z > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index.z]);
            if (weight.w > 0.f)
                activeIndices.insert(m_vecBoneIndicies[(_uint)index.w]);
        }
    }

    return activeIndices;
}
/******************************************************* 인스턴싱 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Mesh* Engine::Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MODEL _modelType, const aiMesh* _aimesh, class Model* _model, const _fmatrix& _prematrix
    , myMesh* _myMesh)
{
	Mesh* pInstance = new Mesh(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_FBX(_modelType, _aimesh, _model, _prematrix, _myMesh), L"Mesh 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Mesh* Engine::Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL _modelType, Model* _model, myMesh* _myMesh, const _fmatrix& _prematrix)
{
    Mesh* pInstance = new Mesh(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Binary(_modelType, _myMesh, _model, _prematrix), L"Mesh 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Mesh::Free()
{
	__super::Free();

    Safe_Delete_Array(m_pVtxInfos);
    Safe_Delete_Array(m_pVtxPlyayerInfo);
    if(m_pFlatPosDeltas != nullptr)
        Safe_Delete_Array(m_pFlatPosDeltas);
    if(m_pFlatNorDeltas != nullptr)
        Safe_Delete_Array(m_pFlatNorDeltas);

    Safe_Release(m_pMorphPosSRV);
    Safe_Release(m_pMorphNorSRV);
    Safe_Release(m_pMorphPosBuffer);
    Safe_Release(m_pMorphNorBuffer);
    Safe_Release(m_pInstanceBuffer);
    Safe_Release(m_pRemapBuffer);
    Safe_Release(m_pReampSRV);

}
/******************************************************* 객체 반환 함수 *******************************************************/


