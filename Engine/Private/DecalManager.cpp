#include "Engine_Define.h"
#include "DecalManager.h"
#include "GameInstance.h"
#include "Decal.h"

DecalManager::DecalManager()
    : m_pGameInstance( GameInstance::GetInstance() )
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT DecalManager::Initialize_Pool(_uint iPrototypeLevelID, const _wstring wstrPrototypeTag, _uint iLevelID, _uint iPoolSize)
{
    for (_uint i = 0; i < iPoolSize; i++)
    {
        Decal* pDecal = nullptr;
        Decal::DECAL_DESC desc{};
        desc.wstrName = L"Decal_Blood_" + to_wstring(i);

        m_pGameInstance->Add_GameObject_To_Layer(iPrototypeLevelID, wstrPrototypeTag, iLevelID, L"Layer_Decal", RCAST(GameObject**)(&pDecal), &desc);

        if (pDecal)
            m_deqActive.push_back(pDecal);
    }

    return S_OK;
}

void DecalManager::Spawn_Decal(const _float3& vPosition, const _float3& vNormal, _float fScale, _float fLifeTime)
{
    // 비활성화 된 Decal들 회수
    Recycle();

    Decal* pDecal = nullptr;

    if (m_deqActive.empty() == false)
    {
        pDecal = m_deqActive.front();
        m_deqActive.pop_front();
    }
    else if (m_listUsing.empty() == false)
    {
        // 풀 다 썼을 때 제일 오래된 거 재활용
        pDecal = m_listUsing.front();
        m_listUsing.pop_front();
    }
    else return;

    Decal::DECAL_DESC desc{};
    desc.vPosition = vPosition;
    desc.vNormal = vNormal;
    desc.vScale = { fScale, 0.3f, fScale };         // Y는 투영 깊이
    desc.fMaxLifeTime = fLifeTime;

    pDecal->Reset(desc);
    m_listUsing.push_back(pDecal);
}

void DecalManager::Recycle()
{
    // 현재 사용 중인 Decal들을 순회
    for (auto iter = m_listUsing.begin(); iter != m_listUsing.end();)
    {
        if ((*iter)->Is_Active() == false)          // 비활성화 상태라면
        {
            m_deqActive.push_back(*iter);           // 사용 가능 리스트에 삽입
            iter = m_listUsing.erase(iter);         // 사용 중인 리스트에서 제거
        }
        else
            ++iter;
    }
}

DecalManager* DecalManager::Create()
{
    return new DecalManager();
}

void DecalManager::Free()
{
    __super::Free();
    
    m_deqActive.clear();
    m_listUsing.clear();

    Safe_Release(m_pGameInstance);
}
