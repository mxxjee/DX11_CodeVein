#include "VFXTool_Define.h"
#include "VFX_Parsing.h"

#include "GameInstance.h"
#include "Layer.h"
#include "ParticleSystem.h"
#include "ParticleEffect.h"
#include "BasicParticle.h"
#include "TrailEffect.h"
#include "SwordTrail.h"
#include "SubEmitter.h"
#include "MeshEffect.h"
#include "BasicMesh.h"

VFX_Parsing::VFX_Parsing()
{
}

HRESULT VFX_Parsing::Initialize()
{
    m_pGameInstance = GameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

    SAVEEVENT event;
    event.bSaveBinary = false;
    event.bSaveJson = true;
    event.eToolType = TOOLTYPE::VFX_TOOL;

    m_pGameInstance->Subscribe<SAVEEVENT>([this](const SAVEEVENT& e)
        {
            if (e.eToolType != TOOLTYPE::VFX_TOOL)
                return;

            Start_Save(e);
        });

    m_pGameInstance->Subscribe<LOADEVENT>([this](const LOADEVENT& e)
        {
            if (e.eToolType != TOOLTYPE::VFX_TOOL)
                return;

            Start_Load(e);
        });

    return S_OK;
}

void VFX_Parsing::Start_Save(const SAVEEVENT& _event)
{
    COUT("[VFX_Parsing] 저장 시작");

    // 데이터 수집
    Collect_Data();

    // JSON 저장
    if (_event.bSaveJson)
    {
        m_pGameInstance->SaveAsJson(
            _event.strPath + ".json",
            *this,
            TOOLTYPE::VFX_TOOL
        );
        COUT("[VFX_Parsing] 저장 완료");
    }

    COUT("[VFX_Parsing] 저장 완료 - " + to_string(m_vecSystemInfo.size()) + "개 오브젝트");
}

void VFX_Parsing::Collect_Data()
{
    m_vecSystemInfo.clear();
    ParticleSystem* pSystem = { nullptr };

    // ImGui에서 저장한 레이어에서 가져온다.
    const auto& umapObjects = m_pGameInstance->Get_Layer(L"Layer_Effect")->Get_GameObjects();
    for (auto& [tag, pObj] : umapObjects)
    {
        if (pObj == nullptr ||
            dynamic_cast<ParticleSystem*>(pObj) == nullptr)
            continue;

        pSystem = dynamic_cast<ParticleSystem*>(pObj);

        ParticleSystemInfo systemInfo{};
        systemInfo.wstrSystemName = pSystem->Get_Name();
        systemInfo.bIsAutoPlay = pSystem->Is_AutoPlay();
        systemInfo.bIsLoop = pSystem->Is_Loop();
        systemInfo.fTotalDuration = pSystem->Get_TotalDuration();
        systemInfo.fDelayedTime = pSystem->Get_DelayedTime();
        systemInfo.vRotationSpeed = pSystem->Get_RotationSpeed();

        vector<ParticleSystem::EFFECT_ENTRY> vecEffects = pSystem->Get_Effects();
        if (vecEffects.size() == 0)
            continue;

        for (_uint i = 0; i < vecEffects.size(); i++)
        {
            ParticleEffectInfo effectInfo{};

            // Effect가 nullptr이면 건너뛰기
            if (vecEffects[i].pEffect == nullptr)
                continue;

            effectInfo.vEffectRotation = vecEffects[i].pEffect->Get_EffectRotation();
            effectInfo.wstrEffectName = vecEffects[i].strEffectName;
            effectInfo.fStartDelay = pSystem->Get_StartDelay(i);
            effectInfo.eEffectType = vecEffects[i].eEffectType;                                 // 0이면 BasicParticle, 1이면 SwordTrail, 2면 MeshEffect
            effectInfo.strTextureName = vecEffects[i].pEffect->Get_TextureName();
            effectInfo.strMaskTextureName = vecEffects[i].pEffect->Get_MaskTextureName();
            effectInfo.strAlphaMaskTextureName = vecEffects[i].pEffect->Get_AlphaMaskTextureName();
            effectInfo.strNoiseTextureName = vecEffects[i].pEffect->Get_NoiseTextureName();
            effectInfo.strShaderTag = wstringToString(vecEffects[i].pEffect->Get_ShaderName());
            effectInfo.strComputeShaderTag = vecEffects[i].pEffect->Get_ShaderComputeTag();
            effectInfo.iMaskSampler = vecEffects[i].pEffect->Get_MaskSampler();
            effectInfo.iPassIndex = vecEffects[i].pEffect->Get_PassIndex();
            effectInfo.vColor = vecEffects[i].pEffect->Get_Color();
            effectInfo.fDistortionStrength = vecEffects[i].pEffect->Get_DistortionStrength();
            effectInfo.bUseEdgeGlow = vecEffects[i].pEffect->Get_UseEdgeGlow();
            effectInfo.bBackCulling = vecEffects[i].pEffect->Get_BackCulling();
            effectInfo.bEffectLoop = vecEffects[i].pEffect->Get_EffectLoop();
            effectInfo.bEmissive = vecEffects[i].pEffect->Get_IsEmissive();
            effectInfo.fGlowStrength = vecEffects[i].pEffect->Get_GlowStrength();
            effectInfo.fEffectLifeTime = vecEffects[i].pEffect->Get_LifeTime();
            effectInfo.fMaskIntensity = vecEffects[i].pEffect->Get_MaskIntensity();
            effectInfo.vDiffuseUVScroll = vecEffects[i].pEffect->Get_DiffuseUVScroll();
            effectInfo.vFadeInOut = vecEffects[i].pEffect->Get_FadeInOut();
            effectInfo.fHotColor = vecEffects[i].pEffect->Get_HotFactor();
            effectInfo.vHotColor = vecEffects[i].pEffect->Get_HotColor();
            effectInfo.vEffectOffset = vecEffects[i].pEffect->Get_EffectOffset();

            switch (effectInfo.eEffectType)
            {
            case EFFECTTYPE::EFFECT_PARTICLE:     // BasicParticle 일 경우
            {
                VIBuffer_Particle::PARTICLE_DESC desc{};
                desc = vecEffects[i].pEffect->Get_ParticleDesc();

                effectInfo.IsBillboard = desc.IsBillboard;
                effectInfo.IsLoop = desc.IsLoop;
                effectInfo.IsVerticalFade = desc.IsVerticalFade;
                effectInfo.IsVerticalShrink = desc.IsVerticalShrink;
                effectInfo.IsRadialRotation = desc.IsRadialRotation;
                effectInfo.iNumInstance = desc.iNumInstance;
                effectInfo.vSize = desc.vSize;
                effectInfo.vSpeed = desc.vSpeed;
                effectInfo.vLifeTime = desc.vLifeTime;
                effectInfo.vStartDelay = desc.vStartDelay;
                effectInfo.vRotation = desc.vRotation;
                effectInfo.vCenter = desc.vCenter;
                effectInfo.vRange = desc.vRange;
                effectInfo.vPivot = desc.vPivot;
                effectInfo.fGravity = desc.fGravity;
                effectInfo.fAlpha = desc.fAlpha;
                effectInfo.iFrameCount = desc.iFrameCount;
                effectInfo.iCountX = desc.iCountX;
                effectInfo.iCountY = desc.iCountY;
                effectInfo.iTransparentIndex = desc.iTransparentIndex;
                effectInfo.iMaskStartFrame = desc.iMaskStartFrame;
                effectInfo.iMaskFrameCount = desc.iMaskFrameCount;
                effectInfo.iMaskCountX = desc.iMaskCountX;
                effectInfo.iMaskCountY = desc.iMaskCountY;
                effectInfo.vMaskUVScroll = desc.vMaskUVScroll;
                effectInfo.fShakeStrength = desc.fShakeStrength;
                effectInfo.fParticleScale = desc.fParticleScale;
                effectInfo.iDirectionalScale = desc.iDirectionalScale;
                effectInfo.fConvergeMaxDist = desc.fConvergeMaxDist;

                break;
            }

            case EFFECTTYPE::EFFECT_TRAIL:     // SwordTrail 일 경우
            {
                VIBuffer_Trail::TRAIL_DESC trailDesc{};
                trailDesc = DCAST(TrailEffect*)(vecEffects[i].pEffect)->Get_TrailDesc();

                effectInfo.iMaxPoints = trailDesc.iMaxPoints;
                effectInfo.fLifeTime = trailDesc.fLifeTime;
                effectInfo.fMinDistance = trailDesc.fMinDistance;
                effectInfo.fTexRotation = trailDesc.fTexRotation;

                break;
            }

            case EFFECTTYPE::EFFECT_MESH:     // MeshEffect일 경우
            {
                MeshEffect::MESHEFFECT_DESC meshDesc{};
                meshDesc = DCAST(MeshEffect*)(vecEffects[i].pEffect)->Get_MeshDesc();

                effectInfo.fLifeTime = meshDesc.fLifeTime;
                effectInfo.strModelName = wstringToString(vecEffects[i].pEffect->Get_ModelName());
                effectInfo.vMeshPosition = meshDesc.vPosition;
                effectInfo.vMeshScale = meshDesc.vScale;
                effectInfo.fMeshScale = meshDesc.fMeshScale;
                effectInfo.fScaleDelay = meshDesc.fScaleDelay;
                effectInfo.iMeshScaleAxis = meshDesc.iScaleAxis;
                effectInfo.iMeshRotateRight = meshDesc.iRotateRight;
                effectInfo.iMeshRotateUp = meshDesc.iRotateUp;
                effectInfo.iMeshRotateLook = meshDesc.iRotateLook;
                effectInfo.iFrameCount = meshDesc.iFrameCount;
                effectInfo.iCountX = meshDesc.iCountX;
                effectInfo.iCountY = meshDesc.iCountY;
                effectInfo.fDisplaceStrength = meshDesc.fDisplaceStrength;
                effectInfo.iMeshMaskStartFrame = meshDesc.iMaskStartFrame;
                effectInfo.iMeshMaskFrameCount = meshDesc.iMaskFrameCount;
                effectInfo.iMeshMaskCountX = meshDesc.iMaskCountX;
                effectInfo.iMeshMaskCountY = meshDesc.iMaskCountY;
                effectInfo.vMeshMaskUVScroll = meshDesc.vMaskUVScroll;
                effectInfo.vMeshMaskUVScale = meshDesc.vMaskUVScale;
                effectInfo.vMeshMaskUVStartOffset = meshDesc.vMaskUVStartOffset;
                effectInfo.bDirectionalDissolve = meshDesc.bDirectionalDissolve;
                effectInfo.bDissolveFlipY = meshDesc.bDissolveFlipY;
                effectInfo.fNoiseWeight = meshDesc.fNoiseWeight;
                effectInfo.fDissolveEdgeWidth = meshDesc.fDissolveEdgeWidth;
                effectInfo.vDissolveEdgeColor = meshDesc.vDissolveEdgeColor;

                effectInfo.bAlignToPlayerLook = meshDesc.bAlignToPlayerLook;

                break;
            }
            }

            // SubEmitter 수집
            SubEmitter* pSubEmitter = vecEffects[i].pEffect->Get_SubEmitter();
            if (pSubEmitter != nullptr)
            {
                _uint iEntryCount = pSubEmitter->Get_EmitterCount();
                for (_uint j = 0; j < iEntryCount; j++)
                {
                    SubEmitter::SUBEMITTER_DESC* pDesc = pSubEmitter->Get_EmitterDescByIndex(j);
                    if (pDesc == nullptr)
                        continue;

                    SubEmitterInfo subInfo{};
                    subInfo.eType = static_cast<_uint>(pDesc->eType);
                    subInfo.wstrEffectName = pDesc->strEffectName;
                    subInfo.fEmitProbability = pDesc->fEmitProbability;
                    subInfo.bInheritPosition = pDesc->bInheritPosition;
                    subInfo.bInheritScale = pDesc->bInheritScale;

                    effectInfo.vecSubEmitters.push_back(subInfo);
                }
            }

            // 이펙트를 시스템에 추가
            systemInfo.vecEffects.push_back(effectInfo);
        }

        // 시스템을 최종 부모 목록에 추가
        m_vecSystemInfo.push_back(systemInfo);
    }
}

void VFX_Parsing::Start_Load(const LOADEVENT& _event)
{
    COUT("[VFX_Parsing] 로드 시작");

    m_vecSystemInfo.clear();

    // JSON 파일에서 데이터 로드
    if (m_pGameInstance->LoadFromJson(_event.m_Path, *this))
    {
        COUT("[VFX_Parsing] JSON 로드 성공 - " + to_string(m_vecSystemInfo.size()) + "개 오브젝트");

        // 이펙트 정보 받아와서 씬에 클론
        Load_EffectDatas();
    }
    else
    {
        COUT("[VFX_Parsing] JSON 로드 실패!");
    }

    COUT("[VFX_Parsing] 로드 완료");
}

void VFX_Parsing::Load_EffectDatas()
{
    COUT("[VFX_Parsing] ParticleSystem 생성 시작");

    m_vecCreatedSystem.clear();

    for (const auto& systemInfo : m_vecSystemInfo)
    {
        // ParticleSystem 먼저 생성
        ParticleSystem::PARTICLESYSTEM_DESC systemDesc{};
        systemDesc.wstrName = systemInfo.wstrSystemName;
        systemDesc.bIsAutoPlay = systemInfo.bIsAutoPlay;
        systemDesc.bIsLoop = systemInfo.bIsLoop;
        systemDesc.fTotalDuration = systemInfo.fTotalDuration;
        systemDesc.fRotationSpeed = systemInfo.fRotationSpeed;
        systemDesc.fDelayedTime = systemInfo.fDelayedTime;
        systemDesc.vRotationSpeed = systemInfo.vRotationSpeed;

        GameObject* pGameObject = nullptr;
        m_pGameInstance->Add_GameObject_To_Layer(
            _UINT(LEVEL::MAIN), L"Prototype_GameObject_ParticleSystem",
            _UINT(LEVEL::MAIN), L"Layer_Effect", &pGameObject, &systemDesc);

        ParticleSystem* pSystem = dynamic_cast<ParticleSystem*>(pGameObject);
        if (pSystem == nullptr)
            continue;

        // ParticleEffect 생성
        for (const auto& effectInfo : systemInfo.vecEffects)
        {
            ParticleEffect* pEffect = nullptr;

            if (effectInfo.eEffectType == EFFECTTYPE::EFFECT_PARTICLE)            // 이펙트 타입이 BasicParticle일 경우
            {
                // ParticleEffect 객체 먼저 클론
                pEffect = CAST(BasicParticle*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::MAIN),
                    TEXT("Prototype_GameObject_BasicParticle")));

                // PARTICLE_DESC 채우기
                VIBuffer_Particle::PARTICLE_DESC particleDesc{};
                particleDesc.IsLoop = effectInfo.IsLoop;
                particleDesc.IsBillboard = effectInfo.IsBillboard;
                particleDesc.IsVerticalFade = effectInfo.IsVerticalFade;
                particleDesc.IsVerticalShrink = effectInfo.IsVerticalShrink;
                particleDesc.IsRadialRotation = effectInfo.IsRadialRotation;
                particleDesc.iNumInstance = effectInfo.iNumInstance;
                particleDesc.vSize = effectInfo.vSize;
                particleDesc.vSpeed = effectInfo.vSpeed;
                particleDesc.vLifeTime = effectInfo.vLifeTime;
                particleDesc.vStartDelay = effectInfo.vStartDelay;
                particleDesc.vRotation = effectInfo.vRotation;
                particleDesc.vCenter = effectInfo.vCenter;
                particleDesc.vRange = effectInfo.vRange;
                particleDesc.vPivot = effectInfo.vPivot;
                particleDesc.fGravity = effectInfo.fGravity;
                particleDesc.fAlpha = effectInfo.fAlpha;
                particleDesc.iFrameCount = effectInfo.iFrameCount;
                particleDesc.iCountX = effectInfo.iCountX;
                particleDesc.iCountY = effectInfo.iCountY;
                particleDesc.iTransparentIndex = effectInfo.iTransparentIndex;
                particleDesc.iMaskStartFrame = effectInfo.iMaskStartFrame;
                particleDesc.iMaskFrameCount = effectInfo.iMaskFrameCount;
                particleDesc.iMaskCountX = effectInfo.iMaskCountX;
                particleDesc.iMaskCountY = effectInfo.iMaskCountY;
                particleDesc.vMaskUVScroll = effectInfo.vMaskUVScroll;
                particleDesc.fShakeStrength = effectInfo.fShakeStrength;
                particleDesc.fParticleScale = effectInfo.fParticleScale;
                particleDesc.iDirectionalScale = effectInfo.iDirectionalScale;
                particleDesc.fConvergeMaxDist = effectInfo.fConvergeMaxDist;

                // 파티클 정보 업데이트
                static_cast<VIBuffer_Particle*>(pEffect->Get_Component_FromName(L"Com_VIBuffer"))->Update_Particles(particleDesc);

                // 텍스처 업데이트
                CAST(BasicParticle*)(pEffect)->Change_Texture(stringToWstring(effectInfo.strTextureName));
                CAST(BasicParticle*)(pEffect)->Change_MaskTexture(stringToWstring(effectInfo.strMaskTextureName));
                CAST(BasicParticle*)(pEffect)->Change_AlphaMaskTexture(stringToWstring(effectInfo.strAlphaMaskTextureName));
                CAST(BasicParticle*)(pEffect)->Change_NoiseTexture(stringToWstring(effectInfo.strNoiseTextureName));

                // 쉐이더 업데이트
                CAST(BasicParticle*)(pEffect)->Change_Shader(stringToWstring(effectInfo.strShaderTag));
                CAST(BasicParticle*)(pEffect)->Change_Compute_Shader(stringToWstring(effectInfo.strComputeShaderTag));
            }
            else if (effectInfo.eEffectType == EFFECTTYPE::EFFECT_TRAIL)       // 이펙트 타입이 SwordTrail 일 경우
            {
                // SwordTrail 객체 먼저 클론
                pEffect = CAST(SwordTrail*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::MAIN),
                    TEXT("Prototype_GameObject_TrailEffect")));

                // TRAIL_DESC 채우기
                VIBuffer_Trail::TRAIL_DESC trailDesc{};
                trailDesc.iMaxPoints = effectInfo.iMaxPoints;
                trailDesc.fLifeTime = effectInfo.fLifeTime;
                trailDesc.fMinDistance = effectInfo.fMinDistance;
                trailDesc.fTexRotation = effectInfo.fTexRotation;

                // TRAIL 정보 업데이트
                CAST(SwordTrail*)(pEffect)->Set_TrailDesc(trailDesc);

                CAST(SwordTrail*)(pEffect)->Change_Texture(stringToWstring(effectInfo.strTextureName));
                CAST(SwordTrail*)(pEffect)->Change_NoiseTexture(stringToWstring(effectInfo.strNoiseTextureName));
                CAST(SwordTrail*)(pEffect)->Change_MaskTexture(stringToWstring(effectInfo.strMaskTextureName));
                CAST(SwordTrail*)(pEffect)->Change_Shader(stringToWstring(effectInfo.strShaderTag));
            }
            else if (effectInfo.eEffectType == EFFECTTYPE::EFFECT_MESH)         // 이펙트 타입이 MeshEffect 일 경우
            {
                // BasicMesh 객체 먼저 클론
                pEffect = CAST(BasicMesh*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::MAIN),
                    TEXT("Prototype_GameObject_BasicMesh")));

                // MESHEFFECT_DESC 채우기
                MeshEffect::MESHEFFECT_DESC meshDesc{};
                meshDesc.fLifeTime = effectInfo.fEffectLifeTime;
                meshDesc.vPosition = effectInfo.vMeshPosition;
                meshDesc.vScale = effectInfo.vMeshScale;
                meshDesc.fMeshScale = effectInfo.fMeshScale;
                meshDesc.fScaleDelay = effectInfo.fScaleDelay;
                meshDesc.iScaleAxis = effectInfo.iMeshScaleAxis;
                meshDesc.iRotateRight = effectInfo.iMeshRotateRight;
                meshDesc.iRotateUp = effectInfo.iMeshRotateUp;
                meshDesc.iRotateLook = effectInfo.iMeshRotateLook;
                meshDesc.iFrameCount = effectInfo.iFrameCount;
                meshDesc.iCountX = effectInfo.iCountX;
                meshDesc.iCountY = effectInfo.iCountY;
                meshDesc.iMaskStartFrame = effectInfo.iMeshMaskStartFrame;
                meshDesc.iMaskFrameCount = effectInfo.iMeshMaskFrameCount;
                meshDesc.iMaskCountX = effectInfo.iMeshMaskCountX;
                meshDesc.iMaskCountY = effectInfo.iMeshMaskCountY;
                meshDesc.fDisplaceStrength = effectInfo.fDisplaceStrength;
                meshDesc.vMaskUVScroll = effectInfo.vMeshMaskUVScroll;
                meshDesc.vMaskUVScale = effectInfo.vMeshMaskUVScale;
                meshDesc.vMaskUVStartOffset = effectInfo.vMeshMaskUVStartOffset;

                meshDesc.bDirectionalDissolve = effectInfo.bDirectionalDissolve;
                meshDesc.bDissolveFlipY = effectInfo.bDissolveFlipY;
                meshDesc.fNoiseWeight = effectInfo.fNoiseWeight;
                meshDesc.fDissolveEdgeWidth = effectInfo.fDissolveEdgeWidth;
                meshDesc.vDissolveEdgeColor = effectInfo.vDissolveEdgeColor;

                meshDesc.bAlignToPlayerLook = effectInfo.bAlignToPlayerLook;

                CAST(MeshEffect*)(pEffect)->Set_MeshDesc(meshDesc);

                // 모델 및 텍스처 업데이트
                CAST(BasicMesh*)(pEffect)->Change_Model(stringToWstring(effectInfo.strModelName));
                CAST(BasicMesh*)(pEffect)->Change_Texture(stringToWstring(effectInfo.strTextureName));
                CAST(BasicMesh*)(pEffect)->Change_MaskTexture(stringToWstring(effectInfo.strMaskTextureName));
                CAST(BasicMesh*)(pEffect)->Change_AlphaMaskTexture(stringToWstring(effectInfo.strAlphaMaskTextureName));
                CAST(BasicMesh*)(pEffect)->Change_NoiseTexture(stringToWstring(effectInfo.strNoiseTextureName));

                // 쉐이더 업데이트
                CAST(BasicMesh*)(pEffect)->Change_Shader(stringToWstring(effectInfo.strShaderTag));
            }

            if (pEffect == nullptr)
                continue;

            pEffect->Set_DiffuseUVScroll(effectInfo.vDiffuseUVScroll);
            pEffect->Set_FadeInOut(effectInfo.vFadeInOut);
            pEffect->Set_EffectRotation(effectInfo.vEffectRotation);
            pEffect->Set_LifeTime(effectInfo.fEffectLifeTime);
            pEffect->Set_EdgeGlow(effectInfo.bUseEdgeGlow);
            pEffect->Set_BackCulling(effectInfo.bBackCulling);
            pEffect->Set_EffectLoop(effectInfo.bEffectLoop);
            pEffect->Set_IsEmissive(effectInfo.bEmissive);
            pEffect->Set_GlowStrength(effectInfo.fGlowStrength);
            pEffect->Set_DistortionStrength(effectInfo.fDistortionStrength);
            pEffect->Set_PassIndex(effectInfo.iPassIndex);
            pEffect->Set_MaskSampler(effectInfo.iMaskSampler);
            pEffect->Set_Color(effectInfo.vColor);
            pEffect->Set_MaskIntensity(effectInfo.fMaskIntensity);
            pEffect->Set_HotColor(effectInfo.vHotColor, effectInfo.fHotColor);
            pEffect->Set_EffectOffset(effectInfo.vEffectOffset);

            pSystem->Add_Effect(effectInfo.wstrEffectName, pEffect, effectInfo.eEffectType, effectInfo.fStartDelay);
            Safe_Release(pEffect);
        }

        // 모든 ParticleEffect들 다 추가하고 SubEmitter 연결하기
        for (const auto& effectInfo : systemInfo.vecEffects)
        {
            if (effectInfo.vecSubEmitters.empty())
                continue;

            ParticleEffect* pOwnerEffect = pSystem->Find_Effect(effectInfo.wstrEffectName);
            if (pOwnerEffect == nullptr)
                continue;

            // SubEmitter를 먼저 생성
            SubEmitter* pSubEmitter = pOwnerEffect->Create_SubEmitter();
            for (const auto& subInfo : effectInfo.vecSubEmitters)
            {
                ParticleEffect* pTargetEffect = pSystem->Find_Effect(subInfo.wstrEffectName);
                if (pTargetEffect)
                {
                    SubEmitter::SUBEMITTER_DESC subEmitterDesc{};
                    subEmitterDesc.eType = CAST(SubEmitter::SUBEMMITER_TYPE)(subInfo.eType);
                    subEmitterDesc.strEffectName = subInfo.wstrEffectName;
                    subEmitterDesc.pEffect = pTargetEffect;
                    subEmitterDesc.fEmitProbability = subInfo.fEmitProbability;
                    subEmitterDesc.bInheritPosition = subInfo.bInheritPosition;
                    subEmitterDesc.bInheritScale = subInfo.bInheritScale;

                    if (pSubEmitter != nullptr)
                    {
                        pSubEmitter->Add_Entry(subEmitterDesc);
                        pTargetEffect->Set_SubEmitter(true);
                    }
                }
            }
        }

        // 마지막으로 ParticleSystem을 추가
        m_vecCreatedSystem.push_back(pSystem);
    }
}

ordered_json VFX_Parsing::To_Json()
{
    ordered_json dataArray = ordered_json::array();
    for (const auto& info : m_vecSystemInfo)
    {
        dataArray.push_back(info.To_Json());
    }

    return dataArray;
}

void VFX_Parsing::From_Json(const ordered_json& _jsonData)
{
    m_vecSystemInfo.clear();

    for (const auto& objJson : _jsonData)
    {
        ParticleSystemInfo info;
        info.From_Json(objJson);
        m_vecSystemInfo.push_back(info);
    }
}

VFX_Parsing* VFX_Parsing::Create()
{
    VFX_Parsing* pInstance = new VFX_Parsing();

    MSG_FAIL(pInstance->Initialize(), L"VFX_Parsing 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

void VFX_Parsing::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
