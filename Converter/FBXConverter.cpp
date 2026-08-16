#include "pch.h"
#include "FBXConverter.h"

namespace fs = std::filesystem;

#ifdef _DEBUG




//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
FBXConverter::FBXConverter()
{
}

FBXConverter::~FBXConverter()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 메인 변환 함수 ////////////////////////////////////////////////////////
bool FBXConverter::Convert(const string& _inputPath, MODEL _modelType)
{
    Clear();

    // 일단 전달받은 타입은 임시 저장 (AUTO 판단용)
    MODEL requestedType = _modelType;

    // UTF-8 경로 변환
    string utf8Path = _inputPath;

    int wideSize = MultiByteToWideChar(CP_ACP, 0, _inputPath.c_str(), -1, nullptr, 0);
    if (wideSize > 0)
    {
        wstring widePath(wideSize, 0);
        MultiByteToWideChar(CP_ACP, 0, _inputPath.c_str(), -1, &widePath[0], wideSize);

        int utf8Size = WideCharToMultiByte(CP_UTF8, 0, widePath.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (utf8Size > 0)
        {
            utf8Path.resize(utf8Size);
            WideCharToMultiByte(CP_UTF8, 0, widePath.c_str(), -1, &utf8Path[0], utf8Size, nullptr, nullptr);
        }
    }

    // 자동 감지를 위해 PreTransformVertices 없이 먼저 로드
    _uint flag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    m_pAIScene = m_Importer.ReadFile(utf8Path, flag);
    if (m_pAIScene == nullptr)
    {
        cout << "FBX 로드 실패: " << m_Importer.GetErrorString() << endl;
        return false;
    }

    // 모델 타입 자동 감지
    bool hasAnimation = m_pAIScene->HasAnimations();
    bool hasBones = false;

    // 메쉬 중 하나라도 본이 있는지 확인
    for (_uint i = 0; i < m_pAIScene->mNumMeshes; ++i)
    {
        if (m_pAIScene->mMeshes[i]->mNumBones > 0)
        {
            hasBones = true;
            break;
        }
    }

    // 수정됨 : PLAYERANIM이 명시적으로 요청된 경우 그대로 사용
    if (requestedType == MODEL::PLAYERANIM)
    {
        m_eModelType = MODEL::PLAYERANIM;
    }
    else if (hasAnimation || hasBones)
        m_eModelType = MODEL::ANIM;
    else
        m_eModelType = MODEL::NONANIM;

    cout << "FBX 로드 성공: " << _inputPath << endl;
    cout << "자동 감지 - 애니메이션: " << (hasAnimation ? "있음" : "없음")
        << ", 본: " << (hasBones ? "있음" : "없음") << endl;
    // 수정됨 : PLAYERANIM 출력 추가
    cout << "모델 타입: " << (m_eModelType == MODEL::NONANIM ? "NONANIM" : (m_eModelType == MODEL::PLAYERANIM ? "PLAYERANIM" : "ANIM")) << endl;

    // NONANIM인 경우 PreTransformVertices 플래그로 다시 로드
    if (m_eModelType == MODEL::NONANIM)
    {
        m_Importer.FreeScene();
        flag |= aiProcess_PreTransformVertices;
        m_pAIScene = m_Importer.ReadFile(utf8Path, flag);

        if (m_pAIScene == nullptr)
        {
            cout << "FBX 재로드 실패: " << m_Importer.GetErrorString() << endl;
            return false;
        }
    }

    // 본(노드) 처리
    cout << "본 처리 시작" << endl;
    m_MyModel.mRootNode = new myNode;
    if (!Ready_Bones(m_pAIScene->mRootNode, -1, m_MyModel.mRootNode))
    {
        cout << "본 처리 실패" << endl;
        return false;
    }
    cout << "본 개수: " << m_vecBoneNames.size() << endl;

    // 메쉬 처리
    cout << "메쉬 처리 시작" << endl;
    if (!Ready_Meshes())
    {
        cout << "메쉬 처리 실패" << endl;
        return false;
    }
    cout << "메쉬 개수: " << m_MyModel.mNumMeshes << endl;

    // 마테리얼 처리
    cout << "마테리얼 처리 시작" << endl;
    if (!Ready_Materials(_inputPath))
    {
        cout << "마테리얼 처리 실패" << endl;
        return false;
    }
    cout << "마테리얼 개수: " << m_MyModel.mNumMaterials << endl;

    // 애니메이션 처리
    cout << "애니메이션 처리 시작" << endl;
    if (m_pAIScene->mNumAnimations > 0)
    {
        if (!Ready_Animations())
        {
            cout << "애니메이션 처리 실패" << endl;
            return false;
        }
        cout << "애니메이션 개수: " << m_MyModel.mNumAnimations << endl;
    }

    // 전체 본 개수 저장
    m_MyModel.mNumAllBones = (_uint)m_vecBoneNames.size();

    cout << "파일 저장 시작" << endl;

    // 출력 파일 경로 생성 (.fbx → .siho)
    fs::path inputPath(_inputPath);
    fs::path outputPath = inputPath.parent_path() / (inputPath.stem().string() + ".siho");

    // 바이너리 파일 저장
    fstream file(outputPath, ios::binary | ios::out);
    if (!file.is_open())
    {
        cout << "출력 파일 생성 실패: " << outputPath << endl;
        return false;
    }

    // 매직 넘버
    const _char magic[4] = { 'S', 'I', 'H', 'O' };
    file.write(magic, 4);

    // 모델 정보
    m_MyModel.Serialize(file);

    // 본(노드) 정보
    m_MyModel.mRootNode->Serialize(file);

    // 메쉬 정보
    for (_uint i = 0; i < m_MyModel.mNumMeshes; ++i)
    {
        m_MyModel.mMeshes[i].Serialize(file);
    }

    // 마테리얼 정보
    for (_uint i = 0; i < m_MyModel.mNumMaterials; ++i)
    {
        m_MyModel.mMaterials[i].Serialize(file);
    }

    // 애니메이션 정보
    for (_uint i = 0; i < m_MyModel.mNumAnimations; ++i)
    {
        m_MyModel.mAnimations[i].Serialize(file);
    }

    file.close();

    cout << "[완료] 변환 성공: " << outputPath << endl;

    // 메모리 정리
    for (auto& mesh : m_MyModel.mMeshes)
        mesh.Delete_Bones();

    return true;
}
/******************************************************* 메인 변환 함수 *******************************************************/



//////////////////////////////////////////////////////// 본(노드) 처리 ////////////////////////////////////////////////////////
bool FBXConverter::Ready_Bones(const aiNode* _aiNode, _int _parentIndex, myNode* _mynode)
{
    // 본 이름 저장
    _mynode->mName = _aiNode->mName.data;
    _mynode->mParentIndex = _parentIndex;

    // Transformation 행렬 저장 (Transpose 필요)
    memcpy(&_mynode->mTransformation, &_aiNode->mTransformation, sizeof(_float4x4));
    TransposeMatrix(_mynode->mTransformation);

    // 본 인덱스 매핑 저장
    _int currentIndex = (_int)m_vecBoneNames.size();
    m_vecBoneNames.push_back(_mynode->mName);
    m_umapBoneNameToIndex[_mynode->mName] = currentIndex;

    // 자식 노드 처리
    _mynode->mNumChildren = _aiNode->mNumChildren;

    for (_uint i = 0; i < _aiNode->mNumChildren; ++i)
    {
        myNode* childNode = new myNode;
        Ready_Bones(_aiNode->mChildren[i], currentIndex, childNode);
        _mynode->mChildren.push_back(childNode);
    }

    return true;
}

_int FBXConverter::Get_BoneIndex(const _char* _name)
{
    auto iter = m_umapBoneNameToIndex.find(_name);
    if (iter == m_umapBoneNameToIndex.end())
        return -1;

    return iter->second;
}
/******************************************************* 본(노드) 처리 *******************************************************/



//////////////////////////////////////////////////////// 메쉬 처리 ////////////////////////////////////////////////////////
bool FBXConverter::Ready_Meshes()
{
    m_MyModel.mNumMeshes = m_pAIScene->mNumMeshes;
    m_MyModel.mMeshes.resize(m_MyModel.mNumMeshes);

    for (_uint i = 0; i < m_MyModel.mNumMeshes; ++i)
    {
        const aiMesh* aiMesh = m_pAIScene->mMeshes[i];
        myMesh* myMesh = &m_MyModel.mMeshes[i];

        cout << "  [메쉬] " << aiMesh->mName.data << " (정점: " << aiMesh->mNumVertices << ")" << endl;

        if (m_eModelType == MODEL::NONANIM)
        {
            if (!Ready_Mesh_NonAnim(aiMesh, myMesh))
                return false;
        }
        // 추가됨 : PLAYERANIM 분기
        else if (m_eModelType == MODEL::PLAYERANIM)
        {
            if (!Ready_Mesh_PlayerAnim(aiMesh, myMesh))
                return false;
        }
        else
        {
            if (!Ready_Mesh_Anim(aiMesh, myMesh))
                return false;
        }
    }

    return true;
}

bool FBXConverter::Ready_Mesh_NonAnim(const aiMesh* _aiMesh, myMesh* _myMesh)
{
    _myMesh->mName = _aiMesh->mName.data;
    _myMesh->mMaterialIndex = _aiMesh->mMaterialIndex;
    _myMesh->mNumVertices = _aiMesh->mNumVertices;
    _myMesh->mNumIndices = _aiMesh->mNumFaces * 3;
    _myMesh->mNumFaces = _aiMesh->mNumFaces;
    _myMesh->mNumBones = 0;
    _myMesh->mHasNormals = _aiMesh->HasNormals();

    // 정점 데이터 저장
    _myMesh->mVertices.resize(_myMesh->mNumVertices);
    _myMesh->mNormals.resize(_myMesh->mNumVertices);
    _myMesh->mTangents.resize(_myMesh->mNumVertices);
    _myMesh->mBiNormals.resize(_myMesh->mNumVertices);
    _myMesh->mTexcoords.resize(_myMesh->mNumVertices);

    for (_uint i = 0; i < _myMesh->mNumVertices; ++i)
    {
        memcpy(&_myMesh->mVertices[i], &_aiMesh->mVertices[i], sizeof(_float3));

        // null 체크
        if (_aiMesh->mNormals)
            memcpy(&_myMesh->mNormals[i], &_aiMesh->mNormals[i], sizeof(_float3));
        else
            _myMesh->mNormals[i] = { 0.f, 1.f, 0.f };

        if (_aiMesh->mTangents)
            memcpy(&_myMesh->mTangents[i], &_aiMesh->mTangents[i], sizeof(_float3));
        else
            _myMesh->mTangents[i] = { 1.f, 0.f, 0.f };

        if (_aiMesh->mBitangents)
            memcpy(&_myMesh->mBiNormals[i], &_aiMesh->mBitangents[i], sizeof(_float3));
        else
            _myMesh->mBiNormals[i] = { 0.f, 0.f, 1.f };

        if (_aiMesh->mTextureCoords[0])
            memcpy(&_myMesh->mTexcoords[i], &_aiMesh->mTextureCoords[0][i], sizeof(_float2));
        else
            _myMesh->mTexcoords[i] = { 0.f, 0.f };
    }

    // 인덱스(Face) 데이터 저장
    _myMesh->mFaces.resize(_myMesh->mNumFaces);
    for (_uint i = 0; i < _myMesh->mNumFaces; ++i)
    {
        _myMesh->mFaces[i].mIndices[0] = _aiMesh->mFaces[i].mIndices[0];
        _myMesh->mFaces[i].mIndices[1] = _aiMesh->mFaces[i].mIndices[1];
        _myMesh->mFaces[i].mIndices[2] = _aiMesh->mFaces[i].mIndices[2];
    }

    return true;
}

bool FBXConverter::Ready_Mesh_Anim(const aiMesh* _aiMesh, myMesh* _myMesh)
{
    _myMesh->mName = _aiMesh->mName.data;
    _myMesh->mMaterialIndex = _aiMesh->mMaterialIndex;
    _myMesh->mNumVertices = _aiMesh->mNumVertices;
    _myMesh->mNumIndices = _aiMesh->mNumFaces * 3;
    _myMesh->mNumFaces = _aiMesh->mNumFaces;
    _myMesh->mHasNormals = _aiMesh->HasNormals();

    // 정점 데이터 저장
    _myMesh->mVertices.resize(_myMesh->mNumVertices);
    _myMesh->mNormals.resize(_myMesh->mNumVertices);
    _myMesh->mTangents.resize(_myMesh->mNumVertices);
    _myMesh->mBiNormals.resize(_myMesh->mNumVertices);
    _myMesh->mTexcoords.resize(_myMesh->mNumVertices);
    _myMesh->mTexcoords1.resize(_myMesh->mNumVertices); // 추가됨 : Deserialize 호환을 위해 항상 할당

    for (_uint i = 0; i < _myMesh->mNumVertices; ++i)
    {
        // 위치 (항상 존재)
        memcpy(&_myMesh->mVertices[i], &_aiMesh->mVertices[i], sizeof(_float3));

        // 노멀 - null 체크
        if (_aiMesh->mNormals)
            memcpy(&_myMesh->mNormals[i], &_aiMesh->mNormals[i], sizeof(_float3));
        else
            _myMesh->mNormals[i] = { 0.f, 1.f, 0.f };  // 기본값

        // 탄젠트 - null 체크
        if (_aiMesh->mTangents)
            memcpy(&_myMesh->mTangents[i], &_aiMesh->mTangents[i], sizeof(_float3));
        else
            _myMesh->mTangents[i] = { 1.f, 0.f, 0.f };  // 기본값

        // 바이탄젠트 - null 체크
        if (_aiMesh->mBitangents)
            memcpy(&_myMesh->mBiNormals[i], &_aiMesh->mBitangents[i], sizeof(_float3));
        else
            _myMesh->mBiNormals[i] = { 0.f, 0.f, 1.f };  // 기본값

        // 텍스처 좌표 - null 체크
        if (_aiMesh->mTextureCoords[0])
            memcpy(&_myMesh->mTexcoords[i], &_aiMesh->mTextureCoords[0][i], sizeof(_float2));
        else
            _myMesh->mTexcoords[i] = { 0.f, 0.f };  // 기본값
    }

    // 인덱스(Face) 데이터 저장
    _myMesh->mFaces.resize(_myMesh->mNumFaces);
    for (_uint i = 0; i < _myMesh->mNumFaces; ++i)
    {
        _myMesh->mFaces[i].mIndices[0] = _aiMesh->mFaces[i].mIndices[0];
        _myMesh->mFaces[i].mIndices[1] = _aiMesh->mFaces[i].mIndices[1];
        _myMesh->mFaces[i].mIndices[2] = _aiMesh->mFaces[i].mIndices[2];
    }

    // 본 데이터 저장
    _myMesh->mNumBones = _aiMesh->mNumBones;
    _myMesh->mNumBoneIndices.reserve(_myMesh->mNumBones);

    for (_uint i = 0; i < _myMesh->mNumBones; ++i)
    {
        aiBone* aiBone = _aiMesh->mBones[i];
        myBone* myBone = new MYBONE;

        myBone->mName = aiBone->mName.data;

        // 본 인덱스 찾기
        _int boneIndex = Get_BoneIndex(aiBone->mName.data);
        if (boneIndex == -1)
        {
            cout << "    [경고] 본을 찾을 수 없음: " << aiBone->mName.data << endl;
            delete myBone;
            continue;
        }

        myBone->mBoneIndex = boneIndex;
        _myMesh->mNumBoneIndices.push_back(boneIndex);

        // 오프셋 매트릭스 (Transpose 필요)
        memcpy(&myBone->mOffsetMatrix, &aiBone->mOffsetMatrix, sizeof(_float4x4));
        TransposeMatrix(myBone->mOffsetMatrix);

        // 가중치 데이터
        myBone->mNumWeights = aiBone->mNumWeights;
        myBone->mWeight.reserve(myBone->mNumWeights);

        for (_uint j = 0; j < aiBone->mNumWeights; ++j)
        {
            myWeight weight = {};
            weight.mVertexID = aiBone->mWeights[j].mVertexId;
            weight.mWeight = aiBone->mWeights[j].mWeight;
            myBone->mWeight.push_back(weight);
        }

        _myMesh->mBones.push_back(myBone);
    }

    // 모프 타겟(AnimMesh) 처리
    _myMesh->mNumAnimMeshes = _aiMesh->mNumAnimMeshes;

    if (_myMesh->mNumAnimMeshes > 0)
    {
        _myMesh->mAnimMeshs.resize(_myMesh->mNumAnimMeshes);

        const _float EPSILON = 0.00001f;

        for (_uint i = 0; i < _myMesh->mNumAnimMeshes; ++i)
        {
            const aiAnimMesh* aiAnimMesh = _aiMesh->mAnimMeshes[i];
            myAnimMesh& myAnimMesh = _myMesh->mAnimMeshs[i];

            myAnimMesh.mName = aiAnimMesh->mName.data;

            // Sparse 데이터 생성 (델타가 0이 아닌 정점만 저장)
            vector<mySparseMorphVertex> tempSparse;
            tempSparse.reserve(_myMesh->mNumVertices / 10);

            for (_uint vertexIndex = 0; vertexIndex < _myMesh->mNumVertices; ++vertexIndex)
            {
                _float3 originalPos = _myMesh->mVertices[vertexIndex];
                _float3 targetPos = {
                    aiAnimMesh->mVertices[vertexIndex].x,
                    aiAnimMesh->mVertices[vertexIndex].y,
                    aiAnimMesh->mVertices[vertexIndex].z
                };

                _float3 deltaPos = {
                    targetPos.x - originalPos.x,
                    targetPos.y - originalPos.y,
                    targetPos.z - originalPos.z
                };

                _float3 deltaNor = {};
                if (_aiMesh->HasNormals() && aiAnimMesh->mNormals)
                {
                    deltaNor = {
                        aiAnimMesh->mNormals[vertexIndex].x - _aiMesh->mNormals[vertexIndex].x,
                        aiAnimMesh->mNormals[vertexIndex].y - _aiMesh->mNormals[vertexIndex].y,
                        aiAnimMesh->mNormals[vertexIndex].z - _aiMesh->mNormals[vertexIndex].z
                    };
                }

                // 델타가 0이 아닌 경우만 저장
                _float deltaLength = sqrtf(
                    deltaPos.x * deltaPos.x +
                    deltaPos.y * deltaPos.y +
                    deltaPos.z * deltaPos.z
                );

                if (deltaLength > EPSILON)
                {
                    mySparseMorphVertex sv = {};
                    sv.mVertexIndex = vertexIndex;
                    sv.mDeltaPos = deltaPos;
                    sv.mDeltaNor = deltaNor;
                    tempSparse.push_back(sv);
                }
            }

            myAnimMesh.mNumNonZeroVertices = (_uint)tempSparse.size();
            myAnimMesh.mSparseVertices = std::move(tempSparse);

            cout << "    [모프] " << myAnimMesh.mName << " (Sparse: " << myAnimMesh.mNumNonZeroVertices << "/" << _myMesh->mNumVertices << ")" << endl;
        }
    }

    return true;
}

// 추가됨 : PLAYERANIM용 메쉬 처리 (UV1 채널 + 8본 블렌딩 지원)
bool FBXConverter::Ready_Mesh_PlayerAnim(const aiMesh* _aiMesh, myMesh* _myMesh)
{
    _myMesh->mName = _aiMesh->mName.data;
    _myMesh->mMaterialIndex = _aiMesh->mMaterialIndex;
    _myMesh->mNumVertices = _aiMesh->mNumVertices;
    _myMesh->mNumIndices = _aiMesh->mNumFaces * 3;
    _myMesh->mNumFaces = _aiMesh->mNumFaces;
    _myMesh->mHasNormals = _aiMesh->HasNormals();
    _myMesh->mHasTexcoords1 = _aiMesh->HasTextureCoords(1);

    // 정점 데이터 저장
    _myMesh->mVertices.resize(_myMesh->mNumVertices);
    _myMesh->mNormals.resize(_myMesh->mNumVertices);
    _myMesh->mTangents.resize(_myMesh->mNumVertices);
    _myMesh->mBiNormals.resize(_myMesh->mNumVertices);
    _myMesh->mTexcoords.resize(_myMesh->mNumVertices);
    _myMesh->mTexcoords1.resize(_myMesh->mNumVertices);

    for (_uint i = 0; i < _myMesh->mNumVertices; ++i)
    {
        // 위치 (항상 존재)
        memcpy(&_myMesh->mVertices[i], &_aiMesh->mVertices[i], sizeof(_float3));

        // 노멀
        if (_aiMesh->mNormals)
            memcpy(&_myMesh->mNormals[i], &_aiMesh->mNormals[i], sizeof(_float3));
        else
            _myMesh->mNormals[i] = { 0.f, 1.f, 0.f };

        // 탄젠트
        if (_aiMesh->mTangents)
            memcpy(&_myMesh->mTangents[i], &_aiMesh->mTangents[i], sizeof(_float3));
        else
            _myMesh->mTangents[i] = { 1.f, 0.f, 0.f };

        // 바이탄젠트
        if (_aiMesh->mBitangents)
            memcpy(&_myMesh->mBiNormals[i], &_aiMesh->mBitangents[i], sizeof(_float3));
        else
            _myMesh->mBiNormals[i] = { 0.f, 0.f, 1.f };

        // UV0
        if (_aiMesh->HasTextureCoords(0))
            memcpy(&_myMesh->mTexcoords[i], &_aiMesh->mTextureCoords[0][i], sizeof(_float2));
        else
            _myMesh->mTexcoords[i] = { 0.f, 0.f };

        // UV1
        if (_aiMesh->HasTextureCoords(1))
            memcpy(&_myMesh->mTexcoords1[i], &_aiMesh->mTextureCoords[1][i], sizeof(_float2));
        else
            _myMesh->mTexcoords1[i] = { 0.f, 0.f };
    }

    // 인덱스(Face) 데이터 저장
    _myMesh->mFaces.resize(_myMesh->mNumFaces);
    for (_uint i = 0; i < _myMesh->mNumFaces; ++i)
    {
        _myMesh->mFaces[i].mIndices[0] = _aiMesh->mFaces[i].mIndices[0];
        _myMesh->mFaces[i].mIndices[1] = _aiMesh->mFaces[i].mIndices[1];
        _myMesh->mFaces[i].mIndices[2] = _aiMesh->mFaces[i].mIndices[2];
    }

    // 본 데이터 저장 (ANIM과 동일)
    _myMesh->mNumBones = _aiMesh->mNumBones;
    _myMesh->mNumBoneIndices.reserve(_myMesh->mNumBones);

    for (_uint i = 0; i < _myMesh->mNumBones; ++i)
    {
        aiBone* aiBone = _aiMesh->mBones[i];
        myBone* myBone = new MYBONE;

        myBone->mName = aiBone->mName.data;

        _int boneIndex = Get_BoneIndex(aiBone->mName.data);
        if (boneIndex == -1)
        {
            cout << "    [경고] 본을 찾을 수 없음: " << aiBone->mName.data << endl;
            delete myBone;
            continue;
        }

        myBone->mBoneIndex = boneIndex;
        _myMesh->mNumBoneIndices.push_back(boneIndex);

        // 오프셋 매트릭스 (Transpose 필요)
        memcpy(&myBone->mOffsetMatrix, &aiBone->mOffsetMatrix, sizeof(_float4x4));
        TransposeMatrix(myBone->mOffsetMatrix);

        // 가중치 데이터
        myBone->mNumWeights = aiBone->mNumWeights;
        myBone->mWeight.reserve(myBone->mNumWeights);

        for (_uint j = 0; j < aiBone->mNumWeights; ++j)
        {
            myWeight weight = {};
            weight.mVertexID = aiBone->mWeights[j].mVertexId;
            weight.mWeight = aiBone->mWeights[j].mWeight;
            myBone->mWeight.push_back(weight);
        }

        _myMesh->mBones.push_back(myBone);
    }

    // 모프 타겟(AnimMesh) 처리
    _myMesh->mNumAnimMeshes = _aiMesh->mNumAnimMeshes;

    if (_myMesh->mNumAnimMeshes > 0)
    {
        _myMesh->mAnimMeshs.resize(_myMesh->mNumAnimMeshes);

        const _float EPSILON = 0.00001f;

        for (_uint i = 0; i < _myMesh->mNumAnimMeshes; ++i)
        {
            const aiAnimMesh* aiAnimMesh = _aiMesh->mAnimMeshes[i];
            myAnimMesh& myAnimMesh = _myMesh->mAnimMeshs[i];

            myAnimMesh.mName = aiAnimMesh->mName.data;

            vector<mySparseMorphVertex> tempSparse;
            tempSparse.reserve(_myMesh->mNumVertices / 10);

            for (_uint vertexIndex = 0; vertexIndex < _myMesh->mNumVertices; ++vertexIndex)
            {
                _float3 originalPos = _myMesh->mVertices[vertexIndex];
                _float3 targetPos = {
                    aiAnimMesh->mVertices[vertexIndex].x,
                    aiAnimMesh->mVertices[vertexIndex].y,
                    aiAnimMesh->mVertices[vertexIndex].z
                };

                _float3 deltaPos = {
                    targetPos.x - originalPos.x,
                    targetPos.y - originalPos.y,
                    targetPos.z - originalPos.z
                };

                _float3 deltaNor = {};
                if (_aiMesh->HasNormals() && aiAnimMesh->mNormals)
                {
                    deltaNor = {
                        aiAnimMesh->mNormals[vertexIndex].x - _aiMesh->mNormals[vertexIndex].x,
                        aiAnimMesh->mNormals[vertexIndex].y - _aiMesh->mNormals[vertexIndex].y,
                        aiAnimMesh->mNormals[vertexIndex].z - _aiMesh->mNormals[vertexIndex].z
                    };
                }

                _float deltaLength = sqrtf(
                    deltaPos.x * deltaPos.x +
                    deltaPos.y * deltaPos.y +
                    deltaPos.z * deltaPos.z
                );

                if (deltaLength > EPSILON)
                {
                    mySparseMorphVertex sv = {};
                    sv.mVertexIndex = vertexIndex;
                    sv.mDeltaPos = deltaPos;
                    sv.mDeltaNor = deltaNor;
                    tempSparse.push_back(sv);
                }
            }

            myAnimMesh.mNumNonZeroVertices = (_uint)tempSparse.size();
            myAnimMesh.mSparseVertices = std::move(tempSparse);

            cout << "    [모프] " << myAnimMesh.mName << " (Sparse: " << myAnimMesh.mNumNonZeroVertices << "/" << _myMesh->mNumVertices << ")" << endl;
        }
    }

    return true;
}
/******************************************************* 메쉬 처리 *******************************************************/



//////////////////////////////////////////////////////// 마테리얼 처리 ////////////////////////////////////////////////////////
bool FBXConverter::Ready_Materials(const string& _filepath)
{
    m_MyModel.mNumMaterials = m_pAIScene->mNumMaterials;
    m_MyModel.mMaterials.resize(m_MyModel.mNumMaterials);

    for (_uint i = 0; i < m_MyModel.mNumMaterials; ++i)
    {
        const aiMaterial* aiMat = m_pAIScene->mMaterials[i];
        myMaterial& myMat = m_MyModel.mMaterials[i];

        // 모든 텍스처 타입 순회
        for (_uint type = 0; type < 27; ++type)
        {
            aiTextureType texType = static_cast<aiTextureType>(type);
            _uint texCount = aiMat->GetTextureCount(texType);

            myMat.mNumTexture[type] = texCount;

            for (_uint j = 0; j < texCount; ++j)
            {
                aiString texPath;
                if (aiMat->GetTexture(texType, j, &texPath) == AI_SUCCESS)
                {
                    // 텍스처 경로를 wstring으로 변환
                    string texPathStr = texPath.C_Str();

                    // 파일명만 추출
                    size_t lastSlashTex = texPathStr.find_last_of("\\/");
                    string fileName;
                    if (lastSlashTex != string::npos)
                        fileName = texPathStr.substr(lastSlashTex + 1);
                    else
                        fileName = texPathStr;

                    // wstring으로 변환
                    wstring wFileName;
                    int wSize = MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, nullptr, 0);
                    if (wSize > 0)
                    {
                        wFileName.resize(wSize - 1);  // null 제외
                        MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, &wFileName[0], wSize);
                    }

                    myMat.mTexture[type].push_back(wFileName);
                }
            }
        }
    }

    return true;
}
/******************************************************* 마테리얼 처리 *******************************************************/



//////////////////////////////////////////////////////// 애니메이션 처리 ////////////////////////////////////////////////////////
bool FBXConverter::Ready_Animations()
{
    m_MyModel.mNumAnimations = m_pAIScene->mNumAnimations;
    m_MyModel.mAnimations.resize(m_MyModel.mNumAnimations);

    for (_uint i = 0; i < m_MyModel.mNumAnimations; ++i)
    {
        const aiAnimation* aiAnim = m_pAIScene->mAnimations[i];
        myAnimation& myAnim = m_MyModel.mAnimations[i];

        myAnim.mName = aiAnim->mName.data;
        myAnim.mDuration = (_float)aiAnim->mDuration;
        myAnim.mTickPerSecond = (_float)aiAnim->mTicksPerSecond;

        if (myAnim.mTickPerSecond == 0.f)
            myAnim.mTickPerSecond = 30.f;  // 기본값

        myAnim.mNumChannels = aiAnim->mNumChannels;
        myAnim.mChannels.resize(myAnim.mNumChannels);

        cout << "애니메이션 " << myAnim.mName << " (채널: " << myAnim.mNumChannels << ", Duration: " << myAnim.mDuration << ")" << endl;

        for (_uint j = 0; j < aiAnim->mNumChannels; ++j)
        {
            const aiNodeAnim* aiChannel = aiAnim->mChannels[j];
            myChannel& myChannel = myAnim.mChannels[j];

            // 채널이 영향을 주는 본 인덱스 찾기
            _int boneIndex = Get_BoneIndex(aiChannel->mNodeName.data);
            myChannel.mChannelIndex = (boneIndex != -1) ? boneIndex : 0;

            // 스케일 키
            myChannel.mNumScalingKeys = aiChannel->mNumScalingKeys;
            myChannel.mScalingKeys.resize(myChannel.mNumScalingKeys);
            for (_uint k = 0; k < aiChannel->mNumScalingKeys; ++k)
            {
                myChannel.mScalingKeys[k].mTime = (_float)aiChannel->mScalingKeys[k].mTime;
                myChannel.mScalingKeys[k].mValue = {
                    aiChannel->mScalingKeys[k].mValue.x,
                    aiChannel->mScalingKeys[k].mValue.y,
                    aiChannel->mScalingKeys[k].mValue.z
                };
            }

            // 회전 키
            myChannel.mNumRotationKeys = aiChannel->mNumRotationKeys;
            myChannel.mRotationKeys.resize(myChannel.mNumRotationKeys);
            for (_uint k = 0; k < aiChannel->mNumRotationKeys; ++k)
            {
                myChannel.mRotationKeys[k].mTime = (_float)aiChannel->mRotationKeys[k].mTime;
                myChannel.mRotationKeys[k].mValue = {
                    aiChannel->mRotationKeys[k].mValue.x,
                    aiChannel->mRotationKeys[k].mValue.y,
                    aiChannel->mRotationKeys[k].mValue.z,
                    aiChannel->mRotationKeys[k].mValue.w
                };
            }

            // 위치 키
            myChannel.mNumPositionKeys = aiChannel->mNumPositionKeys;
            myChannel.mPositionKeys.resize(myChannel.mNumPositionKeys);
            for (_uint k = 0; k < aiChannel->mNumPositionKeys; ++k)
            {
                myChannel.mPositionKeys[k].mTime = (_float)aiChannel->mPositionKeys[k].mTime;
                myChannel.mPositionKeys[k].mValue = {
                    aiChannel->mPositionKeys[k].mValue.x,
                    aiChannel->mPositionKeys[k].mValue.y,
                    aiChannel->mPositionKeys[k].mValue.z
                };
            }

            // 최대 키프레임 수 계산
            myChannel.mNumKeyFrames = max(max(myChannel.mNumScalingKeys, myChannel.mNumRotationKeys), myChannel.mNumPositionKeys);
        }
    }

    return true;
}
/******************************************************* 애니메이션 처리 *******************************************************/



//////////////////////////////////////////////////////// 유틸리티 함수 ////////////////////////////////////////////////////////
void FBXConverter::TransposeMatrix(_float4x4& _matrix)
{
    XMMATRIX mat = XMLoadFloat4x4(&_matrix);
    mat = XMMatrixTranspose(mat);
    XMStoreFloat4x4(&_matrix, mat);
}

wstring FBXConverter::StringToWString(const string& _str)
{
    if (_str.empty()) return wstring();

    int size = MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), (_int)_str.size(), nullptr, 0);
    wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), (_int)_str.size(), &result[0], size);
    return result;
}

string FBXConverter::WStringToString(const wstring& _wstr)
{
    if (_wstr.empty()) return string();

    int size = WideCharToMultiByte(CP_UTF8, 0, _wstr.c_str(), (_int)_wstr.size(), nullptr, 0, nullptr, nullptr);
    string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, _wstr.c_str(), (_int)_wstr.size(), &result[0], size, nullptr, nullptr);
    return result;
}
/******************************************************* 유틸리티 함수 *******************************************************/



//////////////////////////////////////////////////////// 초기화 함수 ////////////////////////////////////////////////////////
void FBXConverter::DeleteNodeTree(myNode* _node)
{
    if (_node == nullptr)
        return;

    for (auto& child : _node->mChildren)
    {
        DeleteNodeTree(child);
    }

    delete _node;
}

void FBXConverter::Clear()
{
    // 본 매핑 초기화
    m_vecBoneNames.clear();
    m_umapBoneNameToIndex.clear();

    // 노드 트리 메모리 해제
    if (m_MyModel.mRootNode)
    {
        DeleteNodeTree(m_MyModel.mRootNode);
        m_MyModel.mRootNode = nullptr;
    }

    // 메쉬 본 메모리 해제
    for (auto& mesh : m_MyModel.mMeshes)
    {
        mesh.Delete_Bones();
    }

    // myModel 초기화
    m_MyModel = {};

    // 모델 타입 초기화
    m_eModelType = MODEL::ANIM;

    // Importer는 ReadFile 호출 시 자동으로 이전 씬 해제됨
    m_pAIScene = nullptr;
}

/******************************************************* 초기화 함수 *******************************************************/

#endif // _DEBUG