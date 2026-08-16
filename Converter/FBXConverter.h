#pragma once
#ifdef _DEBUG
#include "Converter_Struct.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
//////////////////////////////////////////////////////// FBX 컨버터 클래스 ////////////////////////////////////////////////////////
class FBXConverter
{
public:
    // 모델 타입 (엔진과 동일)
    enum class MODEL { NONANIM, ANIM, PLAYERANIM }; // 수정됨 : PLAYERANIM 추가
public:
    FBXConverter();
    ~FBXConverter();
public:
    // 메인 변환 함수
    bool Convert(const string& _inputPath, MODEL _modelType = MODEL::ANIM);

private:
    // 본(노드) 처리
    bool Ready_Bones(const aiNode* _aiNode, _int _parentIndex, myNode* _mynode);
    _int Get_BoneIndex(const _char* _name);

    // 메쉬 처리
    bool Ready_Meshes();
    bool Ready_Mesh_NonAnim(const aiMesh* _aiMesh, myMesh* _myMesh);
    bool Ready_Mesh_Anim(const aiMesh* _aiMesh, myMesh* _myMesh);
    bool Ready_Mesh_PlayerAnim(const aiMesh* _aiMesh, myMesh* _myMesh); // 추가됨 : PLAYERANIM용 메쉬 처리 (UV1 + 8본)

    // 마테리얼 처리
    bool Ready_Materials(const string& _filepath);

    // 애니메이션 처리
    bool Ready_Animations();

    // 유틸리티
    void TransposeMatrix(_float4x4& _matrix);
    wstring StringToWString(const string& _str);
    string WStringToString(const wstring& _wstr);
    void Clear();
    void DeleteNodeTree(myNode* _node);

private:
    Assimp::Importer m_Importer;
    const aiScene* m_pAIScene = {};
    MODEL m_eModelType = {};
    myModel m_MyModel = {};
    // 본 이름 → 인덱스 매핑 (빠른 검색용)
    vector<string> m_vecBoneNames;
    unordered_map<string, _int> m_umapBoneNameToIndex;
};
/******************************************************* FBX 컨버터 클래스 *******************************************************/
#endif // _DEBUG