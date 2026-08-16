#pragma once

namespace Engine {
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
            delete this;
        }
        void Deserialize(fstream& _file)
        {
            _file.read(RCAST(_pchar)(&mNumChildren), sizeof(_uint));
            for (unsigned int i = 0; i < mNumChildren; ++i)
            {
                myNode* children = new myNode;
                mChildren.push_back(children);
                children->Deserialize(_file);
            }
            _file.read(RCAST(_pchar)(&mParentIndex), sizeof(_int));
            _file.read(RCAST(_pchar)(&mTransformation), sizeof(_float4x4));
            _int nameLen{};
            _file.read(RCAST(_pchar)(&nameLen), sizeof(_int));
            mName.resize(nameLen);
            _file.read(&mName[0], nameLen);
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
        _uint mVertexID;
        _float mWeight;

        void SerializeMyWeight(fstream& _file)
        {
            _file.write(RCAST(_cchar)(&mVertexID), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mWeight), sizeof(_float));
        }

        void DeserializeMyWeight(fstream& _file)
        {
            _file.read(RCAST(_pchar)(&mVertexID), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mWeight), sizeof(_float));
        }
    }MYWEIGHT;

    typedef struct myBone {
        _uint mBoneIndex = {};
        _uint mNumWeights = {};
        _string mName = {};
        XMFLOAT4X4 mOffsetMatrix = {};
        vector<myWeight> mWeight;

        void SerializeMyBone(fstream& _file)
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
            delete this;
        }

        void DeserializeMyBone(fstream& _file)
        {
            _file.read(RCAST(_pchar)(&mBoneIndex), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumWeights), sizeof(_uint));

            _int nameLen{};
            _file.read(RCAST(_pchar)(&nameLen), sizeof(_int));
            mName.resize(nameLen);
            _file.read(&mName[0], nameLen);

            _file.read(RCAST(_pchar)(&mOffsetMatrix), sizeof(XMFLOAT4X4));

            for (_uint i = 0; i < mNumWeights; ++i)
            {
                myWeight weight{};
                weight.DeserializeMyWeight(_file);
                mWeight.push_back(weight);
            }
        }
    }MYBONE;

    typedef struct myFace {
        _uint mIndices[3]{};

        void Serialize(std::fstream& _file) const {
            _file.write(RCAST(_cchar)(mIndices), sizeof(_uint) * 3);
        }

        void Deserialize(std::fstream& _file) {
            _file.read(RCAST(_pchar)(mIndices), sizeof(_uint) * 3);
        }
    } MYFACE;

    typedef struct mySparseMorphVertex {
        _uint mVertexIndex;     // 어떤 정점인지
        XMFLOAT3 mDeltaPos;     // 위치 델타
        XMFLOAT3 mDeltaNor;     // 노멀 델타

        void Serialize(std::fstream& _file) const {
            _file.write(RCAST(_cchar)(&mVertexIndex), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mDeltaPos), sizeof(XMFLOAT3));
            _file.write(RCAST(_cchar)(&mDeltaNor), sizeof(XMFLOAT3));
        }

        void Deserialize(std::fstream& _file) {
            _file.read(RCAST(_pchar)(&mVertexIndex), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mDeltaPos), sizeof(XMFLOAT3));
            _file.read(RCAST(_pchar)(&mDeltaNor), sizeof(XMFLOAT3));
        }
    } MYSPARSEMORPHVERTEX;

    typedef struct myAnimMesh {
        string mName = {};
        _uint mNumNonZeroVertices = {};
        vector<mySparseMorphVertex> mSparseVertices;

        void Serialize_AnimMesh(std::fstream& _file) const {
            _int nameLen = (_int)mName.size();
            _file.write(RCAST(_cchar)(&nameLen), sizeof(_int));
            _file.write(mName.c_str(), nameLen);
            _file.write(RCAST(_cchar)(&mNumNonZeroVertices), sizeof(_uint));

            for (const auto& vertex : mSparseVertices) {
                vertex.Serialize(_file);
            }
        }

        void DeSerialize_AnimMesh(std::fstream& _file) {
            _int nameLen{};
            _file.read(RCAST(_pchar)(&nameLen), sizeof(_int));
            mName.resize(nameLen);
            _file.read(&mName[0], nameLen);
            _file.read(RCAST(_pchar)(&mNumNonZeroVertices), sizeof(_uint));

            mSparseVertices.resize(mNumNonZeroVertices);
            for (auto& vertex : mSparseVertices) {
                vertex.Deserialize(_file);
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
        vector<XMFLOAT2> mTexcoords1 = {}; //UV더 받아주기 위해 추가함
        vector<XMFLOAT3> mVertices = {};
        vector<XMFLOAT3> mNormals = {};
        vector<XMFLOAT3> mTangents = {};
        vector<XMFLOAT3> mBiNormals = {};
        vector<myBone*> mBones;
        string mName = {};
        void Serialize(std::fstream& _file) const {
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
            // vector<XMFLOAT3> mVertices
            //_uint verticesSize = (_uint)mVertices.size();
            //_file.write(RCAST(_cchar)(&verticesSize), sizeof(_uint));
            _file.write(RCAST(_cchar)(mVertices.data()), mNumVertices * sizeof(XMFLOAT3));
            // vector<XMFLOAT3> mNormals
            //_uint normalsSize = (_uint)mNormals.size();
            //_file.write(RCAST(_cchar)(&normalsSize), sizeof(_uint));
            _file.write(RCAST(_cchar)(mNormals.data()), mNumVertices * sizeof(XMFLOAT3));
            // vector<XMFLOAT3> mTangents
            //_uint tangentsSize = (_uint)mTangents.size();
            //_file.write(RCAST(_cchar)(&tangentsSize), sizeof(_uint));
            _file.write(RCAST(_cchar)(mTangents.data()), mNumVertices * sizeof(XMFLOAT3));
            // vector<XMFLOAT3> mBiNormals
            //_uint biNormalsSize = (_uint)mBiNormals.size();
            //_file.write(RCAST(_cchar)(&biNormalsSize), sizeof(_uint));
            _file.write(RCAST(_cchar)(mBiNormals.data()), mNumVertices * sizeof(XMFLOAT3));
            // vector<XMFLOAT2> mTexcoords
            //_uint texcoordsSize = (_uint)mTexcoords.size();
            //_file.write(RCAST(_cchar)(&texcoordsSize), sizeof(_uint));
            _file.write(RCAST(_cchar)(mTexcoords.data()), mNumVertices * sizeof(XMFLOAT2));


            if (mHasTexcoords1)
                _file.write(RCAST(_cchar)(mTexcoords1.data()), mNumVertices * sizeof(XMFLOAT2));
            // vector<_uint> mNumBoneIndices
            //_uint boneIndicesSize = (_uint)mNumBoneIndices.size();
            //_file.write(RCAST(_cchar)(&boneIndicesSize), sizeof(_uint));
            _file.write(RCAST(_cchar)(mNumBoneIndices.data()), mNumBones * sizeof(_uint));
            // vector<myBone*> mBones
            //_uint bonesSize = (_uint)mBones.size();
            //_file.write(RCAST(_cchar)(&bonesSize), sizeof(_uint));
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
        void Deserialize(std::fstream& _file) {
            _file.read(RCAST(_pchar)(&mHasNormals), sizeof(_bool));
            _file.read(RCAST(_pchar)(&mHasTexcoords1), sizeof(_bool)); // 추가됨
            _file.read(RCAST(_pchar)(&mNumBones), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mMaterialIndex), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumVertices), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumIndices), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumFaces), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumAnimMeshes), sizeof(_uint));
            _int nameLen{};
            _file.read(RCAST(_pchar)(&nameLen), sizeof(_int));
            mName.resize(nameLen);
            _file.read(&mName[0], nameLen);
            // vector<XMFLOAT3> mVertices
            //_uint verticesSize{};
            //_file.read(RCAST(_pchar)(&verticesSize), sizeof(_uint));
            mVertices.resize(mNumVertices);
            _file.read(RCAST(_pchar)(mVertices.data()), mNumVertices * sizeof(XMFLOAT3));
            // vector<XMFLOAT3> mNormals
            //_uint normalsSize{};
            //_file.read(RCAST(_pchar)(&normalsSize), sizeof(_uint));
            mNormals.resize(mNumVertices);
            _file.read(RCAST(_pchar)(mNormals.data()), mNumVertices * sizeof(XMFLOAT3));
            // vector<XMFLOAT3> mTangents
            //_uint tangentsSize{};
            //_file.read(RCAST(_pchar)(&tangentsSize), sizeof(_uint));
            mTangents.resize(mNumVertices);
            _file.read(RCAST(_pchar)(mTangents.data()), mNumVertices * sizeof(XMFLOAT3));
            // vector<XMFLOAT3> mBiNormals
            //_uint biNormalsSize{};
            //_file.read(RCAST(_pchar)(&biNormalsSize), sizeof(_uint));
            mBiNormals.resize(mNumVertices);
            _file.read(RCAST(_pchar)(mBiNormals.data()), mNumVertices * sizeof(XMFLOAT3));
            // vector<XMFLOAT2> mTexcoords
            //_uint texcoordsSize{};
            //_file.read(RCAST(_pchar)(&texcoordsSize), sizeof(_uint));
            mTexcoords.resize(mNumVertices);
            _file.read(RCAST(_pchar)(mTexcoords.data()), mNumVertices * sizeof(XMFLOAT2));

            if (mHasTexcoords1)
            {
                mTexcoords1.resize(mNumVertices);
                _file.read(RCAST(_pchar)(mTexcoords1.data()), mNumVertices * sizeof(XMFLOAT2));
            }
            // vector<_uint> mNumBoneIndices
            //_uint boneIndicesSize{};
            //_file.read(RCAST(_pchar)(&boneIndicesSize), sizeof(_uint));
            mNumBoneIndices.resize(mNumBones);
            _file.read(RCAST(_pchar)(mNumBoneIndices.data()), mNumBones * sizeof(_uint));
            // vector<myBone*> mBones
            //_uint bonesSize{};
            //_file.read(RCAST(_pchar)(&bonesSize), sizeof(_uint));
            mFaces.resize(mNumFaces);
            for (auto& face : mFaces) {
                face.Deserialize(_file);
            }
            mBones.reserve(mNumBones);
            for (_uint i = 0; i < mNumBones; ++i) {
                myBone* bone = new myBone();
                bone->DeserializeMyBone(_file);
                mBones.push_back(bone);
            }
            mAnimMeshs.resize(mNumAnimMeshes);
            for (auto& animMesh : mAnimMeshs)
            {
                animMesh.DeSerialize_AnimMesh(_file);
            }
        }
        void Delete_Bones() {
            for (auto& bone : mBones)
                delete bone;
        }
    }MYMESH;

    typedef struct myMaterial {
        _uint mNumTexture[27]{};
        vector<_wstring> mTexture[27];

        void Serialize(std::fstream& _file) const {
            // mNumTexture 배열 (고정 27개)
            _file.write(RCAST(_cchar)(mNumTexture), sizeof(_uint) * 27);

            // mTexture 배열 (27개 벡터)
            for (unsigned int i = 0; i < 27; ++i) {
                for (const auto& wstr : mTexture[i]) {
                    _uint strLen = static_cast<_uint>(wstr.size());
                    _file.write(RCAST(_cchar)(&strLen), sizeof(_uint));
                    _file.write(RCAST(_cchar)(wstr.data()), strLen * sizeof(wchar_t));
                }
            }
        }

        void Deserialize(std::fstream& _file) {
            // mNumTexture 배열 (고정 27개)
            _file.read(RCAST(_pchar)(mNumTexture), sizeof(_uint) * 27);

            // mTexture 배열 (27개 벡터)
            for (unsigned int i = 0; i < 27; ++i) {
                mTexture[i].resize(mNumTexture[i]);
                for (auto& wstr : mTexture[i]) {
                    _uint strLen{};
                    _file.read(RCAST(_pchar)(&strLen), sizeof(_uint));

                    wstr.resize(strLen);
                    _file.read(RCAST(_pchar)(wstr.data()), strLen * sizeof(wchar_t));
                }
            }
        }
    }MYMATERIAL;

    typedef struct myKeys {
        _float mTime = {};
        XMFLOAT3 mValue = {};

        void SerializeMyKey(std::fstream& _file) const {
            _file.write(RCAST(_cchar)(&mTime), sizeof(_float));
            _file.write(RCAST(_cchar)(&mValue), sizeof(XMFLOAT3));
        }

        void DeserializeMyKey(std::fstream& _file) {
            _file.read(RCAST(_pchar)(&mTime), sizeof(_float));
            _file.read(RCAST(_pchar)(&mValue), sizeof(XMFLOAT3));
        }
    } MYKEYS;

    typedef struct myRotationKeys {
        _float mTime = {};
        XMFLOAT4 mValue = {};

        void SerializeMyRotKey(std::fstream& _file) const {
            _file.write(RCAST(_cchar)(&mTime), sizeof(_float));
            _file.write(RCAST(_cchar)(&mValue), sizeof(XMFLOAT4));
        }

        void DeserializeMyRotKey(std::fstream& _file) {
            _file.read(RCAST(_pchar)(&mTime), sizeof(_float));
            _file.read(RCAST(_pchar)(&mValue), sizeof(XMFLOAT4));
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

        void Serialize(std::fstream& _file) const {
            _file.write(RCAST(_cchar)(&mChannelIndex), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mNumScalingKeys), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mNumRotationKeys), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mNumPositionKeys), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mNumKeyFrames), sizeof(_uint));

            // vector<myKeys> mScalingKeys
            for (const auto& key : mScalingKeys) {
                key.SerializeMyKey(_file);
            }

            // vector<myRotationKeys> mRotationKeys
            for (const auto& rotKey : mRotationKeys) {
                rotKey.SerializeMyRotKey(_file);
            }

            // vector<myKeys> mPositionKeys
            for (const auto& key : mPositionKeys) {
                key.SerializeMyKey(_file);
            }
        }

        void Deserialize(std::fstream& _file) {
            _file.read(RCAST(_pchar)(&mChannelIndex), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumScalingKeys), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumRotationKeys), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumPositionKeys), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumKeyFrames), sizeof(_uint));

            // vector<myKeys> mScalingKeys
            mScalingKeys.resize(mNumScalingKeys);
            for (auto& key : mScalingKeys) {
                key.DeserializeMyKey(_file);
            }

            // vector<myRotationKeys> mRotationKeys
            mRotationKeys.resize(mNumRotationKeys);
            for (auto& rotKey : mRotationKeys) {
                rotKey.DeserializeMyRotKey(_file);
            }

            // vector<myKeys> mPositionKeys
            mPositionKeys.resize(mNumPositionKeys);
            for (auto& key : mPositionKeys) {
                key.DeserializeMyKey(_file);
            }
        }
    } MYCHANNEL;


    inline void WriteString(fstream& _file, const _string& strName)
    {
        _int nameLen = static_cast<_int>(strName.size());
        _file.write(RCAST(_cchar)(&nameLen), sizeof(_int));
        if (nameLen > 0)
            _file.write(strName.data(), nameLen);
    }

    inline void ReadString(fstream& _file, _string& strName)
    {
        _int nameLen{};
        _file.read(RCAST(_pchar)(&nameLen), sizeof(_int));
        strName.resize(nameLen);
        if (nameLen > 0)
            _file.read(&strName[0], nameLen);
    }

    inline void WriteVariant(fstream& _file, const VariantValue& Value)
    {
        uint8_t tag = static_cast<uint8_t>(Value.index());
        _file.write(RCAST(_cchar)(&tag), sizeof(tag));

        switch (tag)
        {
        case 0: // monostate(초기화용도)
            break;

        case 1: {
            auto x = std::get<_int>(Value);
            _file.write(RCAST(_cchar)(&x), sizeof(x));
        } break;

        case 2: {
            auto x = std::get<_uint>(Value);
            _file.write(RCAST(_cchar)(&x), sizeof(x));
        } break;

        case 3: {
            auto x = std::get<_float>(Value);
            _file.write(RCAST(_cchar)(&x), sizeof(x));
        } break;

        case 4: {
            auto x = std::get<_bool>(Value);
            _file.write(RCAST(_cchar)(&x), sizeof(x));
        } break;

        case 5: {
            const auto& s = std::get<_string>(Value);
            WriteString(_file, s);
        } break;

        case 6: {
            auto x = std::get<_float2>(Value);
            _file.write(RCAST(_cchar)(&x), sizeof(x));
        } break;

        case 7: {
            auto x = std::get<_float3>(Value);
            _file.write(RCAST(_cchar)(&x), sizeof(x));
        } break;

        case 8: {
            auto x = std::get<_float4>(Value);
            _file.write(RCAST(_cchar)(&x), sizeof(x));
        } break;

        default:
            throw std::runtime_error("Invalid variant tag");
        }
    }

    inline void ReadVariant(fstream& _file, VariantValue& Value)
    {
        uint8_t tag{};
        _file.read(RCAST(_pchar)(&tag), sizeof(tag));

        switch (tag)
        {
        case 0: Value = std::monostate{}; break;

        case 1: {
            _int x{};
            _file.read(RCAST(_pchar)(&x), sizeof(x));
            Value = x;
        } break;

        case 2: {
            _uint x{};
            _file.read(RCAST(_pchar)(&x), sizeof(x));
            Value = x;
        } break;

        case 3: {
            _float x{};
            _file.read(RCAST(_pchar)(&x), sizeof(x));
            Value = x;
        } break;

        case 4: {
            _bool x{};
            _file.read(RCAST(_pchar)(&x), sizeof(x));
            Value = x;
        } break;

        case 5: {
            _string s;
            ReadString(_file, s);
            Value = std::move(s);
        } break;

        case 6: {
            _float2 x{};
            _file.read(RCAST(_pchar)(&x), sizeof(x));
            Value = x;
        } break;

        case 7: {
            _float3 x{};
            _file.read(RCAST(_pchar)(&x), sizeof(x));
            Value = x;
        } break;

        case 8: {
            _float4 x{};
            _file.read(RCAST(_pchar)(&x), sizeof(x));
            Value = x;
        } break;

        default:
            throw std::runtime_error("Invalid variant tag");
        }
    }

    typedef struct myNotify {
        _float fFrame = {};
        _float fStartFrame = {};
        _float fEndFrame = {};
        _bool  bActive;

        ANIM_NOTIFY_TYPE eNotify_Type;
        ANIM_EVENT_TYPE eNotify_Event;

        UMAP<_string, VariantValue> UmapEvent;

        _string SocketName;
        _bool bAttached;
        _uint iNotifyId;

        void Serialize(fstream& _file) const
        {
            _file.write(RCAST(_cchar)(&fFrame), sizeof(_float));
            _file.write(RCAST(_cchar)(&fStartFrame), sizeof(_float));
            _file.write(RCAST(_cchar)(&fEndFrame), sizeof(_float));
            _file.write(RCAST(_cchar)(&bActive), sizeof(_bool));
            _file.write(RCAST(_cchar)(&bAttached), sizeof(_bool));
            _file.write(RCAST(_cchar)(&iNotifyId), sizeof(_uint));

            _file.write(RCAST(_cchar)(&eNotify_Type), sizeof(eNotify_Type));
            _file.write(RCAST(_cchar)(&eNotify_Event), sizeof(eNotify_Event));

            _uint iCount = static_cast<_uint>(UmapEvent.size());
            _file.write(RCAST(_cchar)(&iCount), sizeof(_uint));
            for (const auto& [key, value] : UmapEvent)
            {
                WriteString(_file, key);
                WriteVariant(_file, value);
            }

            _int nameLen = (_int)SocketName.size();
            _file.write(RCAST(_cchar)(&nameLen), sizeof(_int));
            _file.write(SocketName.c_str(), nameLen);
        }

        void Deserialize(fstream& _file)
        {
            _file.read(RCAST(_pchar)(&fFrame), sizeof(_float));
            _file.read(RCAST(_pchar)(&fStartFrame), sizeof(_float));
            _file.read(RCAST(_pchar)(&fEndFrame), sizeof(_float));
            _file.read(RCAST(_pchar)(&bActive), sizeof(_bool));
            _file.read(RCAST(_pchar)(&bAttached), sizeof(_bool));
            _file.read(RCAST(_pchar)(&iNotifyId), sizeof(_uint));

            _file.read(RCAST(_pchar)(&eNotify_Type), sizeof(eNotify_Type));
            eNotify_Type = (ANIM_NOTIFY_TYPE)eNotify_Type;

            _file.read(RCAST(_pchar)(&eNotify_Event), sizeof(eNotify_Event));
            eNotify_Event = (ANIM_EVENT_TYPE)eNotify_Event;

            _uint iCount{};
            _file.read(RCAST(_pchar)(&iCount), sizeof(_uint));
            UmapEvent.clear();
            UmapEvent.reserve(iCount);

            for (_uint i = 0; i < iCount; ++i)
            {
                _string key;
                VariantValue value;

                ReadString(_file, key);
                ReadVariant(_file, value);

                UmapEvent.emplace(move(key), move(value));
            }

            _int nameLen{};
            _file.read(RCAST(_pchar)(&nameLen), sizeof(_int));
            SocketName.resize(nameLen);
            _file.read(&SocketName[0], nameLen);
        }

    }MYNOTIFY;

    typedef struct myAnimation {
        _bool mIsLoopAnim = { false };
        _bool mLinkedAnim = { false };
        _bool mCompleteAnim = { false };
        _uint mNumChannels = {};
        _int mNextAnimIndex = {-1};
        _float mDuration = {};
        _float mTickPerSecond = {};
        _float mNextAnimLerpDuration = { 0.2f };
        _string mName = {};
        vector<myChannel> mChannels = {};
        vector<MYNOTIFY> mNotifyes = {};

        void Serialize(std::fstream& _file) const {
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

            // vector<myChannel> mChannels
            for (const auto& channel : mChannels) {
                channel.Serialize(_file);
            }

            _uint notifyCount = (_uint)mNotifyes.size();
            _file.write(RCAST(_cchar)(&notifyCount), sizeof(_uint));

            // 노티파이 저장
            for (const auto& n : mNotifyes)
                n.Serialize(_file);

        }

        void Deserialize(std::fstream& _file) {
            _file.read(RCAST(_pchar)(&mIsLoopAnim), sizeof(_bool));
            _file.read(RCAST(_pchar)(&mLinkedAnim), sizeof(_bool));
            _file.read(RCAST(_pchar)(&mCompleteAnim), sizeof(_bool));

            _file.read(RCAST(_pchar)(&mNumChannels), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNextAnimIndex), sizeof(_int));

            _file.read(RCAST(_pchar)(&mDuration), sizeof(_float));
            _file.read(RCAST(_pchar)(&mTickPerSecond), sizeof(_float));
            _file.read(RCAST(_pchar)(&mNextAnimLerpDuration), sizeof(_float));

            _int nameLen{};
            _file.read(RCAST(_pchar)(&nameLen), sizeof(_int));
            mName.resize(nameLen);
            _file.read(&mName[0], nameLen);

            // vector<myChannel> mChannels
            mChannels.resize(mNumChannels);
            for (auto& channel : mChannels) {
                channel.Deserialize(_file);
            }

            _uint notifyCount{};
            _file.read(RCAST(_pchar)(&notifyCount), sizeof(_uint));
            mNotifyes.resize(notifyCount);
            for (auto& n : mNotifyes)
                n.Deserialize(_file);
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
        

        void Serialize(fstream& _file)
        {
            _file.write(RCAST(_cchar)(&mNumMeshes), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mNumAllBones), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mNumMaterials), sizeof(_uint));
            _file.write(RCAST(_cchar)(&mNumAnimations), sizeof(_uint));
        }

        void Deserialize(fstream& _file)
        {
            _file.read(RCAST(_pchar)(&mNumMeshes), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumAllBones), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumMaterials), sizeof(_uint));
            _file.read(RCAST(_pchar)(&mNumAnimations), sizeof(_uint));
        }
    }MYMODEL;

}
