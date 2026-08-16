#pragma once
#include "Client_Define.h"
#include "Editor_Utils.h"
#include "GameObject.h"
namespace fs = std::filesystem;

typedef struct tagObjDesc : public GameObject::GAMEOBJECT_DESC
{
    _float3 vScale = { 1.f, 1.f, 1.f };
    _float4 vRotation = { 0.f, 0.f, 0.f, 0.f };
    _wstring prototypename = L"";
} OBJ_DESC;

class CMT_Utils
{
public:
    // 파일명에서 엔진용 프로토타입 태그 생성
    static _wstring Parse_Model_Tag(const _string& fileName);

    // currentPath 현재 경로
    // extension 확장자 뭐 가져올지
    // onSelect 코ㅓㄹ백
    static void Render_File_Browser(_string& currentPath, const _string& extension, std::function<void(const fs::path&)> onSelect);

    static HRESULT Spawn_Static_Object(class GameInstance* pGameInstance, _uint iLevel, const _wstring& wstrProtoTag, const OBJ_DESC& desc);
    
    //physx랑 트랜스폼 동기화
    static PxMat44 ToPxMatrix(DirectX::XMMATRIX WorldMatrix);
    static inline physx::PxVec3 ToPxVec3(_float3 v) { return physx::PxVec3(v.x, v.y, v.z); }
    static inline physx::PxVec3 ToPxVec3(_float4 v) { return physx::PxVec3(v.x, v.y, v.z); }
    static inline physx::PxVec3 ToPxVec3(DirectX::FXMVECTOR v) {
        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, v);
        return physx::PxVec3(f.x, f.y, f.z);
    }

    // XMVECTOR(쿼터니언) - PxQuat으로 변환
    static inline physx::PxQuat ToPxQuat(DirectX::FXMVECTOR vQuat) {
        DirectX::XMFLOAT4 f;
        DirectX::XMStoreFloat4(&f, vQuat);
        return physx::PxQuat(f.x, f.y, f.z, f.w);
    }

    // _float4 - PxQuat으로 변환
    static inline physx::PxQuat ToPxQuat(const _float4& v) {
        return physx::PxQuat(v.x, v.y, v.z, v.w);
    }
};