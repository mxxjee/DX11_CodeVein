#include "Engine_Define.h"
#include "SubEmitter.h"
#include "GameInstance.h"
#include "ParticleEffect.h"

Engine::SubEmitter::SubEmitter()
    : m_pGameInstance{ GameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

Engine::SubEmitter::SubEmitter(const SubEmitter& original)
    : m_pGameInstance{original.m_pGameInstance}
{
    Safe_AddRef(m_pGameInstance);

    for (auto& SubEmitterDesc : original.m_vecEntries)
    {
        SUBEMITTER_DESC newSubEmitter{};
        newSubEmitter.eType = SubEmitterDesc.eType;
        newSubEmitter.pEffect = SubEmitterDesc.pEffect;
        newSubEmitter.strEffectName = SubEmitterDesc.strEffectName;
        newSubEmitter.fEmitProbability = SubEmitterDesc.fEmitProbability;
        newSubEmitter.bInheritPosition = SubEmitterDesc.bInheritPosition;
        newSubEmitter.bInheritScale = SubEmitterDesc.bInheritScale;

        m_vecEntries.push_back(newSubEmitter);
    }
}

_uint Engine::SubEmitter::Get_EmitterCountByType(SUBEMMITER_TYPE eType) const
{
    _uint iEmitterCount = {};

    for (auto iter = m_vecEntries.begin(); iter != m_vecEntries.end(); iter++) 
    {
        if (eType == iter->eType)
            iEmitterCount++;
    }

    return iEmitterCount;
}

HRESULT Engine::SubEmitter::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::SubEmitter::Initialize()
{
    return S_OK;
}

void Engine::SubEmitter::Update_Priority(const _float fTimeDelta)
{
}

void Engine::SubEmitter::Update(const _float fTimeDelta)
{
}

void Engine::SubEmitter::Update_Late(const _float fTimeDelta)
{
}

void Engine::SubEmitter::Render(const _float fTimeDelta)
{
}

HRESULT Engine::SubEmitter::Add_Entry(const SUBEMITTER_DESC& entry)
{
    // 동일한 이름의 Entry가 있다면 반환
    for (auto& entryDesc : m_vecEntries)
    {
        if (entry.strEffectName == entryDesc.strEffectName)
            return E_FAIL;
    }

    SUBEMITTER_DESC newSubEmitter{};
    newSubEmitter.eType = entry.eType;
    newSubEmitter.pEffect = entry.pEffect;
    newSubEmitter.strEffectName = entry.strEffectName;
    newSubEmitter.fEmitProbability = entry.fEmitProbability;
    newSubEmitter.bInheritPosition = entry.bInheritPosition;
    newSubEmitter.bInheritScale = entry.bInheritScale;

    m_vecEntries.push_back(newSubEmitter);

    return S_OK;
}

HRESULT Engine::SubEmitter::Remove_Entry(const _wstring& effectName)
{
    for (auto iter = m_vecEntries.begin(); iter != m_vecEntries.end(); iter++)
    {
        if (effectName == iter->strEffectName)
        {
            m_vecEntries.erase(iter);
            return S_OK;
        }
    }

    return S_OK;
}

HRESULT Engine::SubEmitter::Remove_EntryByType(SUBEMMITER_TYPE eType)
{
    auto iter = remove_if(m_vecEntries.begin(), m_vecEntries.end(), 
        [eType](SUBEMITTER_DESC& desc) {
        if (eType == desc.eType)
        {
            return true;
        }
        return false;
        });

    m_vecEntries.erase(iter, m_vecEntries.end());

    return S_OK;
}

void Engine::SubEmitter::Clear_Entries()
{
    m_vecEntries.clear();
}

void Engine::SubEmitter::Update_Entry(SUBEMITTER_DESC subEmitterDesc, _uint iEntryIndex)
{
    m_vecEntries[iEntryIndex].eType = subEmitterDesc.eType;
    m_vecEntries[iEntryIndex].pEffect = m_vecEntries[iEntryIndex].pEffect;
    m_vecEntries[iEntryIndex].bInheritPosition = subEmitterDesc.bInheritPosition;
    m_vecEntries[iEntryIndex].bInheritScale = subEmitterDesc.bInheritScale;
    m_vecEntries[iEntryIndex].fEmitProbability = subEmitterDesc.fEmitProbability;
}

void Engine::SubEmitter::Trigger(SUBEMMITER_TYPE eType, const _float4x4& matParentWorld)
{
    for (auto& iter : m_vecEntries)
    {
        if (eType == iter.eType)
            Emit_Effect(iter, matParentWorld);
    }
}

void Engine::SubEmitter::Trigger_AtPosition(SUBEMMITER_TYPE eType, const _float3& vPosition)
{
    _float4x4 matWorld{};
    XMStoreFloat4x4(&matWorld, XMMatrixIdentity());

    matWorld._41 = vPosition.x;
    matWorld._42 = vPosition.y;
    matWorld._43 = vPosition.z;

    Trigger(eType, matWorld);
}

void Engine::SubEmitter::Process_DeadParticles(const vector<_float4x4>& vecDeadParticles)
{
    for (const auto& matWorld : vecDeadParticles)
    {
        Trigger(SUBEMMITER_TYPE::DEATH, matWorld);
    }
}

void Engine::SubEmitter::Emit_Effect(const SUBEMITTER_DESC& emitDesc, const _float4x4& matWorld)
{
    // 확률
    if (emitDesc.fEmitProbability < m_pGameInstance->RandomValue(0.f, 1.f))
        return;

    // 위치 상속을 받는가
    if (emitDesc.bInheritPosition)
    {
        _vector vPosition = XMLoadFloat4x4(&matWorld).r[3];
        emitDesc.pEffect->Set_State(DIRECTION::POSITION, vPosition);

        //_float3 vNewCenter = {};
        //XMStoreFloat3(&vNewCenter, vPosition);
        emitDesc.pEffect->Reset_Particles();
    }

    // 스케일 상속을 받는가
    if (emitDesc.bInheritScale)
    {
        _matrix matrixWorld = XMLoadFloat4x4(&matWorld);

        _vector vRight = matrixWorld.r[0];
        _vector vUp = matrixWorld.r[1];
        _vector vLook = matrixWorld.r[2];

        _float fScaleX = XMVectorGetX(XMVector3Length(vRight));
        _float fScaleY = XMVectorGetX(XMVector3Length(vUp));
        _float fScaleZ = XMVectorGetX(XMVector3Length(vLook));

        emitDesc.pEffect->Set_Scale(fScaleX, fScaleY, fScaleZ);
    }

    emitDesc.pEffect->Set_Active(true);
}

SubEmitter* Engine::SubEmitter::Create()
{
    SubEmitter* pInstance = new SubEmitter();

    MSG_FAIL(pInstance->Initialize_Prototype(), L"SubEmitter 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

SubEmitter* Engine::SubEmitter::Clone()
{
    SubEmitter* pInstance = new SubEmitter(*this);

    MSG_FAIL(pInstance->Initialize(), L"SubEmitter 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}

void Engine::SubEmitter::Free()
{
    __super::Free();

    m_vecEntries.clear();

    Safe_Release(m_pGameInstance);
}
