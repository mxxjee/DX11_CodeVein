#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

// 타입 정의
typedef unsigned int        _uint;
typedef int                 _int;
typedef float               _float;
typedef bool                _bool;
typedef char                _char;
typedef const char* _cchar;
typedef char* _pchar;
typedef XMFLOAT2            _float2;
typedef XMFLOAT3            _float3;
typedef XMFLOAT4            _float4;
typedef XMFLOAT4X4          _float4x4;

// 캐스팅 매크로
#define RCAST(_type) reinterpret_cast<_type>

//////////////////////////////////////////////////////// 구조체 정의 ////////////////////////////////////////////////////////

typedef struct myNode {
    _uint mNumChildren = {};
    vector<myNode*> mChildren;
    _int mParentIndex = {};
    XMFLOAT4X4 mTransformation = {};
    string mName = {};

    void Serialize(fstream& _file) const {
        _file.write(RCAST(_cchar)(&mNumChildren), sizeof(_uint));
        for (const auto& child : mChildren) {
            child->Serialize(_file);
        }
        _file.write(RCAST(_cchar)(&mParentIndex), sizeof(_int));
        _file.write(RCAST(_cchar)(&mTransformation), sizeof(_float4x4));
        _int nameLen = (_int)mName.size();
        _file.write(RCAST(_cchar)(&nameLen), sizeof(_int));
        _file.write(mName.c_str(), nameLen);
    }

    void Delete_Children()
    {
        for (auto& child : mChildren)
        {
            child->Delete_Children();
        }
        delete this;
    }
}MYNODE;

typedef struct myWeight {
    _uint mVertexID = {};
    _float mWeight = {};

    void SerializeMyWeight(fstream& _file) const
    {
        _file.write(RCAST(_cchar)(&mVertexID), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mWeight), sizeof(_float));
    }
}MYWEIGHT;

typedef struct myBone {
    _uint mBoneIndex = {};
    _uint mNumWeights = {};
    string mName = {};
    XMFLOAT4X4 mOffsetMatrix = {};
    vector<myWeight> mWeight;

    void SerializeMyBone(fstream& _file) const
    {
        _file.write(RCAST(_cchar)(&mBoneIndex), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumWeights), sizeof(_uint));

        _int nameLen = (_int)mName.size();
        _file.write(RCAST(_pchar)(&nameLen), sizeof(_int));
        _file.write(mName.c_str(), nameLen);
        _file.write(RCAST(_cchar)(&mOffsetMatrix), sizeof(XMFLOAT4X4));

        for (_uint i = 0; i < mNumWeights; ++i)
        {
            mWeight[i].SerializeMyWeight(_file);
        }
    }
}MYBONE;

typedef struct myFace {
    _uint mIndices[3] = {};

    void Serialize(fstream& _file) const {
        _file.write(RCAST(_cchar)(mIndices), sizeof(_uint) * 3);
    }
} MYFACE;

typedef struct mySparseMorphVertex {
    _uint mVertexIndex = {};
    XMFLOAT3 mDeltaPos = {};
    XMFLOAT3 mDeltaNor = {};

    void Serialize(fstream& _file) const {
        _file.write(RCAST(_cchar)(&mVertexIndex), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mDeltaPos), sizeof(XMFLOAT3));
        _file.write(RCAST(_cchar)(&mDeltaNor), sizeof(XMFLOAT3));
    }
} MYSPARSEMORPHVERTEX;

typedef struct myAnimMesh {
    string mName = {};
    _uint mNumNonZeroVertices = {};
    vector<mySparseMorphVertex> mSparseVertices;

    void Serialize_AnimMesh(fstream& _file) const {
        _int nameLen = (_int)mName.size();
        _file.write(RCAST(_cchar)(&nameLen), sizeof(_int));
        _file.write(mName.c_str(), nameLen);
        _file.write(RCAST(_cchar)(&mNumNonZeroVertices), sizeof(_uint));

        for (const auto& vertex : mSparseVertices) {
            vertex.Serialize(_file);
        }
    }
} MYANIMMESH;

typedef struct myMesh {
    _bool mHasNormals = {};
    _bool mHasTexcoords1 = {};
    _uint mNumBones = {};
    _uint mMaterialIndex = {};
    _uint mNumVertices = {};
    _uint mNumIndices = {};
    _uint mNumFaces = {};
    _uint mNumAnimMeshes = {};
    vector<myFace> mFaces;
    vector<myAnimMesh> mAnimMeshs;
    vector<_uint> mNumBoneIndices;
    vector<XMFLOAT2> mTexcoords = {};
    vector<XMFLOAT2> mTexcoords1 = {};
    vector<XMFLOAT3> mVertices = {};
    vector<XMFLOAT3> mNormals = {};
    vector<XMFLOAT3> mTangents = {};
    vector<XMFLOAT3> mBiNormals = {};
    vector<myBone*> mBones;
    string mName = {};

    void Serialize(fstream& _file) const {
        _file.write(RCAST(_cchar)(&mHasNormals), sizeof(_bool));
        _file.write(RCAST(_cchar)(&mHasTexcoords1), sizeof(_bool)); // 추가됨
        _file.write(RCAST(_cchar)(&mNumBones), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mMaterialIndex), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumVertices), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumIndices), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumFaces), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumAnimMeshes), sizeof(_uint));
        _int nameLen = (_int)mName.size();
        _file.write(RCAST(_cchar)(&nameLen), sizeof(_int));
        _file.write(mName.c_str(), nameLen);

        _file.write(RCAST(_cchar)(mVertices.data()), mNumVertices * sizeof(XMFLOAT3));
        _file.write(RCAST(_cchar)(mNormals.data()), mNumVertices * sizeof(XMFLOAT3));
        _file.write(RCAST(_cchar)(mTangents.data()), mNumVertices * sizeof(XMFLOAT3));
        _file.write(RCAST(_cchar)(mBiNormals.data()), mNumVertices * sizeof(XMFLOAT3));
        _file.write(RCAST(_cchar)(mTexcoords.data()), mNumVertices * sizeof(XMFLOAT2));
        if (mHasTexcoords1)
            _file.write(RCAST(_cchar)(mTexcoords1.data()), mNumVertices * sizeof(XMFLOAT2));
        _file.write(RCAST(_cchar)(mNumBoneIndices.data()), mNumBones * sizeof(_uint));

        for (const auto& face : mFaces) {
            face.Serialize(_file);
        }
        for (const auto& bone : mBones) {
            bone->SerializeMyBone(_file);
        }
        for (const auto& animmesh : mAnimMeshs) {
            animmesh.Serialize_AnimMesh(_file);
        }
    }

    void Delete_Bones() {
        for (auto& bone : mBones)
            delete bone;
    }
}MYMESH;

typedef struct myMaterial {
    _uint mNumTexture[27] = {};
    vector<wstring> mTexture[27];

    void Serialize(fstream& _file) const {
        _file.write(RCAST(_cchar)(mNumTexture), sizeof(_uint) * 27);

        for (unsigned int i = 0; i < 27; ++i) {
            for (const auto& wstr : mTexture[i]) {
                _uint strLen = static_cast<_uint>(wstr.size());
                _file.write(RCAST(_cchar)(&strLen), sizeof(_uint));
                _file.write(RCAST(_cchar)(wstr.data()), strLen * sizeof(wchar_t));
            }
        }
    }
}MYMATERIAL;

typedef struct myKeys {
    _float mTime = {};
    XMFLOAT3 mValue = {};

    void SerializeMyKey(fstream& _file) const {
        _file.write(RCAST(_cchar)(&mTime), sizeof(_float));
        _file.write(RCAST(_cchar)(&mValue), sizeof(XMFLOAT3));
    }
} MYKEYS;

typedef struct myRotationKeys {
    _float mTime = {};
    XMFLOAT4 mValue = {};

    void SerializeMyRotKey(fstream& _file) const {
        _file.write(RCAST(_cchar)(&mTime), sizeof(_float));
        _file.write(RCAST(_cchar)(&mValue), sizeof(XMFLOAT4));
    }
} MYROTKEYS;

typedef struct myChannel {
    _uint mChannelIndex = {};
    _uint mNumScalingKeys = {};
    _uint mNumRotationKeys = {};
    _uint mNumPositionKeys = {};
    _uint mNumKeyFrames = {};
    vector<myKeys> mScalingKeys;
    vector<myRotationKeys> mRotationKeys;
    vector<myKeys> mPositionKeys;

    void Serialize(fstream& _file) const {
        _file.write(RCAST(_cchar)(&mChannelIndex), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumScalingKeys), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumRotationKeys), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumPositionKeys), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumKeyFrames), sizeof(_uint));

        for (const auto& key : mScalingKeys) {
            key.SerializeMyKey(_file);
        }
        for (const auto& rotKey : mRotationKeys) {
            rotKey.SerializeMyRotKey(_file);
        }
        for (const auto& key : mPositionKeys) {
            key.SerializeMyKey(_file);
        }
    }
} MYCHANNEL;

typedef struct myAnimation {
    _bool mIsLoopAnim = { false };
    _bool mLinkedAnim = { false };
    _bool mCompleteAnim = { false };
    _uint mNumChannels = {};
    _int mNextAnimIndex = { -1 };
    _float mDuration = {};
    _float mTickPerSecond = {};
    _float mNextAnimLerpDuration = { 0.2f };
    string mName = {};
    vector<myChannel> mChannels = {};

    void Serialize(fstream& _file) const {
        _file.write(RCAST(_cchar)(&mIsLoopAnim), sizeof(_bool));
        _file.write(RCAST(_cchar)(&mLinkedAnim), sizeof(_bool));
        _file.write(RCAST(_cchar)(&mCompleteAnim), sizeof(_bool));

        _file.write(RCAST(_cchar)(&mNumChannels), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNextAnimIndex), sizeof(_int));

        _file.write(RCAST(_cchar)(&mDuration), sizeof(_float));
        _file.write(RCAST(_cchar)(&mTickPerSecond), sizeof(_float));
        _file.write(RCAST(_cchar)(&mNextAnimLerpDuration), sizeof(_float));

        _int nameLen = (_int)mName.size();
        _file.write(RCAST(_cchar)(&nameLen), sizeof(_int));
        _file.write(mName.c_str(), nameLen);

        for (const auto& channel : mChannels) {
            channel.Serialize(_file);
        }
    }
} MYANIMATION;

typedef struct myModel {
    _uint mNumMeshes = {};
    _uint mNumAllBones = {};
    _uint mNumMaterials = {};
    _uint mNumAnimations = {};

    myNode* mRootNode = {};
    vector<myMesh> mMeshes;
    vector<myMaterial> mMaterials;
    vector<myAnimation> mAnimations;

    void Serialize(fstream& _file) const
    {
        _file.write(RCAST(_cchar)(&mNumMeshes), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumAllBones), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumMaterials), sizeof(_uint));
        _file.write(RCAST(_cchar)(&mNumAnimations), sizeof(_uint));
    }
}MYMODEL;

/******************************************************* 구조체 정의 *******************************************************/