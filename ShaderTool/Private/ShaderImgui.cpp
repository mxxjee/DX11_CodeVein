#include "ShaderTool_Define.h"
#include "ShaderImgui.h"
#include "GameInstance.h"
#include "Mouse.h"
#include "GameObject.h"
#include "Shader.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Parsing_Loader.h"
#include "LightComponent.h"
#include "Player.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderImgui::ShaderImgui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :ImguiWindow(pDevice, pContext), m_pMouse(Mouse::GetInstance()), m_iMeshNum(-1)
{
}

HRESULT ShaderTool::ShaderImgui::Initialize(void* pArg)
{
    m_pGameInstance = GameInstance::GetInstance();
    __super::Initialize(pArg);

    m_umapLightsPtr = m_pGameInstance->Get_LightuMapPtr();

    m_vStaticShadowDesc.vPosition = _float4{ 0.f, 14.f, -20.f, 1.f };
    m_vStaticShadowDesc.vAt = _float4{ -155.61f, 32.14f, 65.48f, 1.f };
    m_vStaticShadowDesc.vOrthSize = 500;//XMConvertToRadians(45.0f); //1000
    m_vStaticShadowDesc.fNearZ = 0.1f;
    m_vStaticShadowDesc.fFarZ = 500.f;
    m_vStaticShadowDesc.fDepth = 500; //1000

    m_vDynamicShadowDesc.vPosition = _float4{ 0.f, 1.f, -1.f, 1.f }; //높이서 내려다봄
    m_vDynamicShadowDesc.vAt = _float4{ 0.f, 0.f, 0.f, 1.f };
    m_vDynamicShadowDesc.vOrthSize = 10.f;
    m_vDynamicShadowDesc.fNearZ = 0.1f;
    m_vDynamicShadowDesc.fFarZ = 500.f;
    m_vDynamicShadowDesc.fDepth = 200.f;

    //HDR
    m_vHDROption.x = 0.1f; //"BloomIntensity"
    m_vHDROption.y = 0.7f; //"HDR_Exposure", 
    m_vHDROption.z = 2.2f; //"Gamma"
    m_vHDROption.w = 0.8f; //"BrightExtract"

    //SSAO
    m_vSSAOOption.x = 1.f; //ON/OFF
    m_vSSAOOption.y = 0.5f; //SSAORadius
    m_vSSAOOption.z = 0.01f; //SSAOBias

    //칼라그레딩용
    m_vShaderDesc.g_fEnableColorGrading = 1.f; //칼라그레딩 켜고 끄기
    m_vShaderDesc.vShadowTintColor = _float3(0.85f, 0.85f, 1.15f);  //_float3(0.85f, 0.85f, 1.15f);
    m_vShaderDesc.fShadowTintWeight = 0.6f;
    m_vShaderDesc.vMidtoneTintColor = _float3(0.92f, 0.90f, 1.05f); //_float3(0.92f, 0.90f, 1.05f);
    m_vShaderDesc.fMidtoneTintWeight = 0.5f;
    m_vShaderDesc.vHighlightTintColor = _float3(0.95f, 0.93f, 1.08f); //_float3(0.95f, 0.93f, 1.08f);
    m_vShaderDesc.fHighlightTintWeight = 0.7f;
    m_vShaderDesc.fShadowRange = 0.3f;
    m_vShaderDesc.fHighLightRange = 0.5f;

    //포그용
    m_vShaderDesc.fFogEnable = 1.f; //ON/OFF
    m_vShaderDesc.vFogColor = _float3(0.05f, 0.1f, 0.25f); //색깔 (0.55f, 0.48f, 0.38f)
    m_vShaderDesc.fFogStartDist = 20.f; //포그 시작거리 //20
    m_vShaderDesc.fFogEndDist = 300.f; //포그 적용거리 //300
    m_vShaderDesc.fFogDensity = 0.00015f; //포그 빽빽함
    m_vShaderDesc.fogType = 0.f; //포그타입 0.5로 나뉨
    m_vShaderDesc.fFogBaseHeight = -62.1f; //포그 가장 짙은 높이
    m_vShaderDesc.fFogFadeHeight = 42.8f; //포그 사라질 위치
    m_vShaderDesc.fHeightFogIntensity = 2.44f; //높이 포그 강도
    m_vShaderDesc.fFogIntensity = 0.73f; //전체 포그 강도

    //Light
    m_vShaderDesc.fLightMultiplier = 1.0f; //전체조명세기
    m_vShaderDesc.fAmbientStrength = 0.15f; //앰비언트세기
    m_vShaderDesc.vToonShadowRange = _float2(0.05f, 0.25f);  //X~Y까지 부드럽게 전환(X이하값 0 Y이상값 1)
    m_vShaderDesc.vToonBrightnessRange = _float2(0.05f, 1.f); //그림자 밝은면 최소/최대(x 어두운면 y밝은면)
    m_vShaderDesc.vAmbientFloorMin = _float2(0.08f, 0.02f); //최소 앰비언트(비례값, 절대값) 둘중 큰거 사용됨 비례는 곱하는거고 절대값은 무조건 그값이상

    //SSS/SKIN
    m_vShaderDesc.vSSSColor = _float3(0.9f, 0.82f, 0.72f); //SSS색상
    m_vShaderDesc.fSSSPower = 4.f; //SSS투과정도(올리면 영역 좁아짐)
    m_vShaderDesc.fSSSIntensity = 0.1f; //SSS강도(올리면 얇은부분 더 빛남)
    m_vShaderDesc.vSkinTint = XMFLOAT3(0.873f, 0.790f, 0.736f); //피부톤
    m_vShaderDesc.fSkinRoughnessMin = 0.5f; //피부 Rough최소값

    //RimLight
    m_vShaderDesc.fRimPower = 1.f; //씀
    m_vShaderDesc.fRimIntensity = 3.f; //림 강도(올리면 윤곽선 강해짐)
    m_vShaderDesc.vRimMaskRange = _float2(0.1f, 0.5f); //림 마스크 범위(안씀)

    //Specular
    m_vShaderDesc.vSpecBoostRange = _float2(4.f, 1.f);//x축 매끈한면 부스트 y축 거친면 부스트 //0.07 0.3
    m_vShaderDesc.fSpecularIntensity = 1.f; //전체 스펙 세기 0.7
    m_vShaderDesc.vSpecBosstMapRange = _float2(2.f, 2.f); //맵 스페큘러 부스트
    m_vShaderDesc.fSpecularMapIntensity = 1.f; //맵 스페큘러 세기
    m_vShaderDesc.fUnderMapZeroDirSpecular = 0.005f; //지하에는 directionlight 안되게
    m_vShaderDesc.fMinDiffuse = 0.05f; //디퓨즈 보장(메탈릭이라도 까맣게 안되게?)
    m_vShaderDesc.fSpecSoftClamp = 0.2f; //옷에 spec 감소(상한선)

    //PBR관련
    m_vShaderDesc.fKValue = 8.f; //올리면 스페큘러 넓어짐
    m_vShaderDesc.fMtrlRoughnessMin = 0.15f; //물체최소 Rough

    //Shadow
    m_vShaderDesc.vBackFaceShaowRange = _float2(-0.2f, 0.3f); //뒷면그림자범위
    m_vShaderDesc.fShadowBright = 0.9f; //그림자 밝기
    m_vShaderDesc.fShadowBackBright = 0.3f; //뒷면 그림자 밝기
    m_vShaderDesc.fBackFillStrength = 0.7f; //뒷면 보정강도

    //GodRaty 진짜 설정용
    m_vShaderDesc.fGodRayDensity = 1.0f;    // 빛줄기 길이 (높을수록 길어짐)
    m_vShaderDesc.fGodRayDecay = 0.98f;   // 거리 감쇠 (1에 가까울수록 멀리까지)
    m_vShaderDesc.fGodRayWeight = 0.1f;    // 샘플당 기여도
    m_vShaderDesc.fGodRayExposure = 0.5f;    // 최종 밝기
    m_vShaderDesc.vGodRayColor = _float4{ 1.f, 1.f, 1.f, 1.f };   // 갓레이 색
    m_vShaderDesc.vGodRayPosition = _float4{ -56.23, 27.98f, -16.22f, 1.f }; //_float4{ -122.29f, 105.36f, -153.458f, 1.f };

    //GodRay메쉬 설정용
    m_vShaderDesc.fGodRayMeshScrollSpeed = 0.02f;    // UV 스크롤 속도
    m_vShaderDesc.fGodRayMeshSwayFreq = 1.f;      // 좌우 흔들림 빈도
    m_vShaderDesc.fGodRayMeshSwayPhase = 1.f;      // 높이별 위상 차이
    m_vShaderDesc.fGodRayMeshSwayAmp = 0.01f;    // 좌우 흔들림 폭
    m_vShaderDesc.vGodRayMeshColor = _float3{ 1.f, 0.9f, 0.7f }; // 빛 색상
    m_vShaderDesc.fGodRayMeshContrast = 2.0f;     // 노이즈 대비
    m_vShaderDesc.fGodRayMeshIntensity = 2.5f;      // 전체 밝기
    m_vShaderDesc.fGodRayMeshEdgeFade = 0.6f;    // 가장자리 페이드 폭
    m_vShaderDesc.fSoftParticleRange = 5.f;    //경계페이드
    m_vShaderDesc.fGodRayMeshAngleFadePower = 3.5f; //옆에서 보면 흐려지는정도
    m_vShaderDesc.fGodRayMeshFadeNear = 1.f;
    m_vShaderDesc.fGodRayMeshFadeFar = 3.5f;

    //죽순밝기
    m_vShaderDesc.fBambooEmission = 30.f;
    m_vShaderDesc.fUnderBambooEmission = 30.f;
    m_vShaderDesc.fCaveMapWet = 0.8f;
    
    //캠모션블러
    m_vShaderDesc.fCamMotionBlurIntensity = 0.3f;
    m_vShaderDesc.fCamMotionBlurMaxVelocity = 0.05f;
    m_vShaderDesc.fCamMotionBlurSamples = 12.f; //이거 loop 써야해서 최적화때문에 빼둠

    return S_OK;
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 ////////////////////////////ㅈㅈㅈ////////////////////////////
_uint ShaderTool::ShaderImgui::Update_Contents(_float fTimeDelta)
{
    ChangeMapColor();
    m_pGameInstance->Set_HDROption(m_vHDROption);
    m_pGameInstance->Set_SSAOOption(m_vSSAOOption);
    m_pGameInstance->Set_ShaderDesc(m_vShaderDesc);

    PickGameObject();
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("ETC"))
        {
            CameraTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("GameObject"))
        {
            ImGui::Text("GameObject");
            GameObjectTab();
            //ShaderTab();
            AddPointLight();
            AddSpotLight();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Light"))
        {
            ImGui::Text("Light");
            LightTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("PBR"))
        {
            ImGui::Text("PBR");
            PBRTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("HDR"))
        {
            ImGui::Text("HDR");
            HDRTab();
            ColorTab();
            FogTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Shadow"))
        {
            ImGui::Text("Shadow");
            //ShadowTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("GodRay"))
        {
            ImGui::Text("GodRay");
            GodRayTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Player"))
        {
            ImGui::Text("Options");
            PlayerCustomTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Options"))
        {
            ImGui::Text("Options");
            SaveLoadShader();
            ImGui::EndTabItem();
        }

       /* if (ImGui::BeginTabItem("LimLight"))
        {
            ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }*/
        ImGui::EndTabBar();
    }
    ImGui::Separator();

    return 0;
}
/******************************************************* 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 생성자////////////////////////////////////////////////////////
ShaderImgui* ShaderImgui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    ShaderImgui* pInstance = new ShaderImgui(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : ShaderImgui");
        Safe_Release(pInstance);
    }


    return pInstance;
}
/******************************************************* 생성자 *******************************************************/



//////////////////////////////////////////////////////// 게임오브젝트피킹  ////////////////////////////////////////////////////////
void ShaderTool::ShaderImgui::PickGameObject()
{

    if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
    {
        GameObject* pObj = {};
        if(m_pMouse->Picking_Object_GameObject_Pixel(pObj))
        {
                m_pNowSelectGameObj = pObj;
        }
    }
}
/******************************************************* 게임오브젝트피킹 *******************************************************/



//////////////////////////////////////////////////////// 게임오브젝트탭 ////////////////////////////////////////////////////////
void ShaderTool::ShaderImgui::GameObjectTab()
{
    if (m_pNowSelectGameObj == nullptr)
        return;

    //잘하면 메쉬도 나누겠는데?
   _string ObjName = wstringToString(m_pNowSelectGameObj->Get_Name());
   ImGui::Text(ObjName.c_str());
   ImGui::Separator();
   _float4 fPos = {};
   if (m_pNowSelectGameObj->Get_Transform() != nullptr)
   {
       XMStoreFloat4(&fPos, m_pNowSelectGameObj->Get_Transform()->Get_State(DIRECTION::POSITION));

       ImGui::Text("Postion");
       ImGui::PushItemWidth(80);
       ImGui::Text("X"); ImGui::SameLine();
       ImGui::DragFloat("##objX##1", &fPos.x, 0.01f); ImGui::SameLine();
       ImGui::Text("Y"); ImGui::SameLine();
       ImGui::DragFloat("##objY##1", &fPos.y, 0.01f); ImGui::SameLine();
       ImGui::Text("Z"); ImGui::SameLine();
       ImGui::DragFloat("##objZ##1", &fPos.z, 0.01f); ImGui::SameLine();
       ImGui::Text("W"); ImGui::SameLine();
       ImGui::DragFloat("##objW##1", &fPos.w, 0.01f);
       ImGui::PopItemWidth();

       m_pNowSelectGameObj->Get_Transform()->Set_State(DIRECTION::POSITION  , fPos);
   }
  m_vShaderDesc.vGodRayPosition = fPos;
  /* ImGui::DragFloat3("GodRay Position",
       reinterpret_cast<float*>(&m_vShaderDesc.vGodRayPosition),
       0.1f, -200.f, 200.f, "%.2f");*/
   ImGui::Separator();
}
void ShaderTool::ShaderImgui::AddPointLight()
{
    if (m_pNowSelectGameObj == nullptr)
        return;

    //하나넣고 두개부터는 굳이? 넣을필요가없긴하네
    if (ImGui::Button("Create PointLight Component"))
    {
        LightComponent*& temp = m_pNowSelectGameObj->Get_LightPtr();
        if (FAILED(m_pNowSelectGameObj->Add_Component(_UINT(LEVEL::STATIC), L"Proto_Component_Point_Light", //Proto_Com_Shader_VTXAnimMesh Proto_Com_Shader_VTXMesh L"Prototype_Component_Shader_VTXPlayerMesh"
            TEXT("Com_Light" + to_wstring(m_pGameInstance->Get_LightCnt())), reinterpret_cast<Component**>(&temp))))
            return;

        _vector Pos = m_pNowSelectGameObj->Get_Transform()->Get_State(DIRECTION::POSITION);

        Bounding_Sphere::BOUNDSPHERE_DESC boundingdesc;
        boundingdesc.fRadius = 0.5f; 
        boundingdesc.vCenter = _float3(0.f, 0.f, 0.f);
        Collider*& tempColl = m_pNowSelectGameObj->Get_ColliderPtr();
        m_pNowSelectGameObj->Add_Collider(COLLISION_GROUP::INTERACTION, COLLIDER::SPHERE, &tempColl, &boundingdesc);

        //처음 배치할때만 위치
        temp->Update(0, Pos);
    }
}
void ShaderTool::ShaderImgui::AddSpotLight()
{
    if (m_pNowSelectGameObj == nullptr)
        return;

    //하나넣고 두개부터는 굳이? 넣을필요가없긴하네
    if (ImGui::Button("Create SpotLight Component"))
    {
        LightComponent*& temp = m_pNowSelectGameObj->Get_LightPtr();
        if (FAILED(m_pNowSelectGameObj->Add_Component(_UINT(LEVEL::STATIC), L"Proto_Component_Spot_Light", //Proto_Com_Shader_VTXAnimMesh Proto_Com_Shader_VTXMesh L"Prototype_Component_Shader_VTXPlayerMesh"
            TEXT("Com_Light" + to_wstring(m_pGameInstance->Get_LightCnt())), reinterpret_cast<Component**>(&temp))))
            return;

        _vector Pos = m_pNowSelectGameObj->Get_Transform()->Get_State(DIRECTION::POSITION);

        Bounding_Sphere::BOUNDSPHERE_DESC boundingdesc;
        boundingdesc.fRadius = 0.5f;
        boundingdesc.vCenter = _float3(0.f, 0.f, 0.f);
        Collider*& tempColl = m_pNowSelectGameObj->Get_ColliderPtr();
        m_pNowSelectGameObj->Add_Collider(COLLISION_GROUP::INTERACTION, COLLIDER::SPHERE, &tempColl, &boundingdesc);

        //처음 배치할때만 위치
        temp->Update(0, Pos);
    }
}
void ShaderTool::ShaderImgui::PlayerCustomTab()
{
    GameObject* tempPlayer = m_pGameInstance->Get_Player();
    if (tempPlayer == nullptr)
        return;

    m_vPlayerShaderDesc = static_cast<Player*>(tempPlayer)->Get_PlayerShaderDesc();
    m_pMeshTex = static_cast<Player*>(tempPlayer)->Get_PlayerMeshTexNumPtr();

    // ======================== Body ========================
    if (ImGui::CollapsingHeader("Body"))
    {
        ImGui::DragInt("Body MeshNum", m_pMeshTex->pPlayerBodyNum, 0.1f, 0, 6);

        // [Edit] InnerColor 6개 — 피부 톤 레이어
        ImGui::ColorEdit4("InnerColor L1", (float*)&m_vPlayerShaderDesc.m_tBodyDesc.vInnerColor[0]);
        ImGui::ColorEdit4("InnerColor L2", (float*)&m_vPlayerShaderDesc.m_tBodyDesc.vInnerColor[1]);
        ImGui::ColorEdit4("InnerColor L3", (float*)&m_vPlayerShaderDesc.m_tBodyDesc.vInnerColor[2]);
        ImGui::ColorEdit4("InnerColor L4", (float*)&m_vPlayerShaderDesc.m_tBodyDesc.vInnerColor[3]);
        ImGui::ColorEdit4("InnerColor L5", (float*)&m_vPlayerShaderDesc.m_tBodyDesc.vInnerColor[4]);
        ImGui::ColorEdit4("InnerColor L6", (float*)&m_vPlayerShaderDesc.m_tBodyDesc.vInnerColor[5]);
        ImGui::SliderFloat("Color Strength", &m_vPlayerShaderDesc.m_tBodyDesc.fColorStrength, 0.0f, 5.0f);
    }

    // ======================== Hair ========================
    if (ImGui::CollapsingHeader("Hair"))
    {
        ImGui::DragInt("Hair MeshNum", m_pMeshTex->pPlayerHairNum, 0.1f, 0, 4);
        ImGui::ColorEdit4("Hair Color", (float*)&m_vPlayerShaderDesc.m_tHairDesc.vPlayerHairColor);
        ImGui::DragFloat("Highlight1 Shift", &m_vPlayerShaderDesc.m_tHairDesc.fPlayerHighlight1Shift, 0.01f, -2.0f, 2.0f);
        ImGui::DragFloat("Highlight1 Strength", &m_vPlayerShaderDesc.m_tHairDesc.fPlayerHighlight1Strength, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Highlight2 Shift", &m_vPlayerShaderDesc.m_tHairDesc.fPlayerHighlight2Shift, 0.01f, -2.0f, 2.0f);
        ImGui::DragFloat("Highlight2 Strength", &m_vPlayerShaderDesc.m_tHairDesc.fPlayerHighlight2Strength, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Highlight Power", &m_vPlayerShaderDesc.m_tHairDesc.fPlayerHighlightPower, 0.01f, 0.1f, 500.0f);
        ImGui::DragFloat("Metallic", &m_vPlayerShaderDesc.m_tHairDesc.fPlayerMetallic, 0.005f, 0.0f, 1.0f);
        ImGui::DragFloat("Roughness Min", &m_vPlayerShaderDesc.m_tHairDesc.fPlayerRoughnessMin, 0.005f, 0.0f, 1.0f);
    }

    // ======================== Brow ========================
    if (ImGui::CollapsingHeader("Brow"))
    {
        ImGui::DragInt("Brow MeshNum", &m_pMeshTex->tHeadTexture->iBrowNum, 0.1f, 0, 490);
        ImGui::DragInt("EyeLash MeshNum", &m_pMeshTex->tHeadTexture->iEyelashNum, 0.25f, 0, 43);
        ImGui::ColorEdit4("Brow Color", (float*)&m_vPlayerShaderDesc.m_tBrowDesc.vPlayerBrowColor);
        ImGui::ColorEdit4("EyeLine Color", (float*)&m_vPlayerShaderDesc.m_tBrowDesc.vPlayerEyeLineColor);
        ImGui::DragFloat("EyeLine Alpha", &m_vPlayerShaderDesc.m_tBrowDesc.fEyeLineAlpha, 0.01f, 0.f, 1.0f);
        ImGui::DragFloat2("Brow UV Offset", (float*)&m_vPlayerShaderDesc.m_tBrowDesc.vPlayerBrowUVOffset, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat2("Brow UV Scale", (float*)&m_vPlayerShaderDesc.m_tBrowDesc.fPlayerBrowUVScale, 0.01f, 0.1f, 5.0f);
        ImGui::Checkbox("Use Second UV", (bool*)&m_vPlayerShaderDesc.m_tBrowDesc.iPlayerUseSecondUV);
        // [Edit] 라디안 ↔ 도 변환하여 표시
        float fDegrees = XMConvertToDegrees(m_vPlayerShaderDesc.m_tBrowDesc.fPlayerBrowUVRotation);
        if (ImGui::SliderFloat("Brow Rotation (deg)", &fDegrees, -180.0f, 180.0f))
            m_vPlayerShaderDesc.m_tBrowDesc.fPlayerBrowUVRotation = XMConvertToRadians(fDegrees);
        ImGui::DragFloat("Brow X Gap", &m_vPlayerShaderDesc.m_tBrowDesc.fPlayerBrowXGap, 0.01f, -1.0f, 1.0f);
        ImGui::ColorEdit4("Lip Color", (float*)&m_vPlayerShaderDesc.m_tBrowDesc.vPlayerLipColor);
        ImGui::DragFloat("Lip Alpha", &m_vPlayerShaderDesc.m_tBrowDesc.fLipAlpha, 0.01f, 0.f, 1.0f);
    }

    // ======================== Eye ========================
    if (ImGui::CollapsingHeader("Eye"))
    {
        ImGui::DragInt("EyeBackGround MeshNum", &m_pMeshTex->tHeadTexture->iEyeWhiteNum, 0.1f, 0, 11);
        ImGui::DragInt("EyeDetail MeshNum", &m_pMeshTex->tHeadTexture->iEyeDetailNum, 0.1f, 0, 66);
        ImGui::DragInt("EyeHighlight MeshNum", &m_pMeshTex->tHeadTexture->iEyeHighlightNum, 0.1f, 0, 69);

        ImGui::ColorEdit4("Eye Base Color", (float*)&m_vPlayerShaderDesc.m_tEyeDesc.vPlayerEyeBaseColor);
        ImGui::ColorEdit4("Eye Append Color", (float*)&m_vPlayerShaderDesc.m_tEyeDesc.vPlayerEyeAppendColor);
        ImGui::SliderFloat("Eye Scale", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerEyeScale, 0.1f, 3.0f);
        ImGui::SliderFloat("Individual Scale Offset", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerEyeIndividualScaleOffset, -1.0f, 1.0f);
        ImGui::DragFloat("Eye Offset U", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerEyeOffsetU, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat("Eye Offset V", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerEyeOffsetV, 0.01f, -1.0f, 1.0f);

        ImGui::SliderFloat("Highlight Strength", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerEyeHighlightStrength, 0.0f, 5.0f);
    }
    if (ImGui::CollapsingHeader("Tatto"))
    {
        ImGui::DragInt("Tatto MeshNum", &m_pMeshTex->tHeadTexture->iTattoPaintNum, 0.1f, 0, 60);
        ImGui::SliderFloat("Tatto ON/OFF", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerTatooOnOff, 0.0f, 1.0f);
        ImGui::SliderFloat("Tatto Alpha", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerTattoAlpha, 0.0f, 1.0f);
        ImGui::SliderFloat("Tatto Scale", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerTattoScale, 0.0f, 5.0f);
        ImGui::DragFloat("Tatto Offset U", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerTattoOffsetU, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat("Tatto Offset V", &m_vPlayerShaderDesc.m_tEyeDesc.fPlayerTattoOffsetV, 0.01f, -1.0f, 1.0f);
        ImGui::ColorEdit4("Eye Append Color", (float*)&m_vPlayerShaderDesc.m_tEyeDesc.vPlayerTattoColor);

    }

    static_cast<Player*>(tempPlayer)->Set_PlayerShaderDesc(m_vPlayerShaderDesc);

}

/******************************************************* 게임오브젝트탭 *******************************************************/



//////////////////////////////////////////////////////// 카메라탭 ////////////////////////////////////////////////////////
void ShaderTool::ShaderImgui::CameraTab()
{
    _float4 Campos = m_pGameInstance->Get_CameraPosition();
    Imgui:TEXT("CamPosition");
    ImGui::Text("Float4 Value: %.2f, %.2f, %.2f, %.2f", Campos.x, Campos.y, Campos.z, Campos.w);
    ImGui::DragFloat("BlurIntensity", &m_vShaderDesc.fCamMotionBlurIntensity, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("BlurMaxVelocity", &m_vShaderDesc.fCamMotionBlurMaxVelocity, 0.0001f, 0.0f, 1.f);
    ImGui::DragFloat("Samples", &m_vShaderDesc.fCamMotionBlurSamples, 0.1f, 0.0f, 100.f);
}
/******************************************************* 카메라탭 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더탭 ////////////////////////////////////////////////////////
void ShaderTool::ShaderImgui::ShaderTab()
{
    if (m_pNowSelectGameObj == nullptr)
        return;
    //여기서 쉐이더 변경가능하도록

    //이건쉐이더 바꾸는거
    //unordered_map<_wstring, Component*>&  compnentmap = m_pNowSelectGameObj->Get_umapComponents();
    //auto it = compnentmap.find(L"Com_Shader");
    //Safe_Release(compnentmap[L"Com_Shader"]);
    //compnentmap.erase(it);
    //unordered_map<_wstring, Component*>& aasdasd = m_pNowSelectGameObj->Get_umapComponents();

    //if (FAILED(m_pNowSelectGameObj->Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXMesh, //Proto_Com_Shader_VTXAnimMesh
    //    TEXT("Com_Shader"), reinterpret_cast<Component**>(&tempShader))))
    //    return;

    Shader* ObjShader = m_pNowSelectGameObj->Get_Shader();
    SHADERENTRIES passDesc = ObjShader->Get_ShaderEntries();
    ImGui::SetNextItemWidth(250.0f);


    //메쉬별로 띄워서 패스 선택가능하도록하기
    //이거 모델 없는 애들도 생각하기
    vector<_string> Meshes;
    Model* ObjModel = m_pNowSelectGameObj->Get_Model();

    //cout << m_pPrevSelectGameObj << " " << m_pNowSelectGameObj << endl;
    //이걸로 게임오브젝트 바뀌면 메쉬 번호 다르게
    if (m_pPrevSelectGameObj != m_pNowSelectGameObj)
        m_iMeshNum = -1;

    static int iPassNum = {};
    //일단 메쉬이름 띄우기
    if (ObjModel == nullptr)
    {
       //모델이 없으면 아래꺼 안하고 0번고정으로?
        m_iMeshNum = 0;
    }
    else
    {
        vector<Mesh*> ObjMeshes = ObjModel->Get_Meshes();
        if (ImGui::TreeNode("MeshSelect"))
        {
            for (int i = 0; i < ObjMeshes.size(); i++)
            {
                string label = to_string(i) + ". " + ObjMeshes[i]->Get_Name() + "  Pass : " + to_string(m_pNowSelectGameObj->Get_VecObjPass(i)) + "##" + to_string(i);
                if (ImGui::Selectable(label.c_str(), m_iMeshNum == i))
                {
                    m_iMeshNum = i;
                    iPassNum = m_pNowSelectGameObj->Get_VecObjPass(i);
                }   
            }
            ImGui::TreePop();
        }
    }

    //패스별 어떤게 있는지 
    vector<_string> items;
    for (int i = 0; i < ObjShader->Get_NumPass(); i++)
    {
        _string tempstring;
        tempstring = passDesc.pEntries[i].vsEntry + ". " + passDesc.pEntries[i].psEntry;
        items.push_back(tempstring);
    }
    
    //패스 선택
    if (items.size() <= iPassNum)
    {
        iPassNum = 0;
        return;
    }
    if (ImGui::BeginCombo("Pass", (to_string(iPassNum)  + ". " + items[iPassNum]).c_str()))
    {
        for (int i = 0; i < items.size(); i++)
        {
            bool is_selected = (iPassNum == i);
            string AddNum = to_string(i) + ". " + items[i];
            if (ImGui::Selectable(AddNum.c_str(), is_selected))
                iPassNum = i;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
        //메쉬별 패스 변경
        //첨에 0 0 이라서
        
    }
    cout << m_iMeshNum << endl;
    m_pNowSelectGameObj->Set_Pass_VecObjPass(m_iMeshNum, iPassNum);

    //현재패스에 필요한것들 정보들 나옴
    vector<Pass> NowPass = ObjShader->Get_VecPass();

    if (ImGui::TreeNode("Constant Buffer"))
    {
        unordered_map<_string, Buffer*> ConstBuffer = ObjShader->Get_umapBuffer();
        for (auto& it : ConstBuffer)
        {
            if (ImGui::TreeNode(it.first.c_str()))
            {
                unordered_map<_string, _uint>  Buffer = it.second->Get_UmapVariablenames(); //각 상수버퍼마다 있는게 들어있음
                for (auto& it2 : Buffer)
                {
                    //상수버퍼안에 있는것들
                    if (it2.first == "Object_Padding")
                        continue;
                    ImGui::Text(it2.first.c_str());
                }
                ImGui::TreePop();
            }
            //   m_vecVAriables 가져와야함       
        }
        ImGui::TreePop();
    }
    //g_ 들어있는곳
    if (ImGui::TreeNode("Variable"))
    {
        for (int i = 0; i < NowPass[iPassNum].vecSRVs.size(); i++)
        {
            ImGui::Text(NowPass[iPassNum].vecSRVs[i].name.c_str());
        }
        ImGui::TreePop();
    }

    //샘플러 들어있는곳
    if (ImGui::TreeNode("Sampler"))
    {
        for (int i = 0; i < NowPass[iPassNum].vecSamplers.size(); i++)
        {
            ImGui::Text(NowPass[iPassNum].vecSamplers[i].name.c_str());
        }
        ImGui::TreePop();
    }
    m_pPrevSelectGameObj = m_pNowSelectGameObj;
}
void ShaderTool::ShaderImgui::SaveLoadShader()
{
    if (ImGui::Button("Save Lights"))
    {
      /*  SAVEEVENT save;
        save.eToolType = TOOLTYPE::MAP_TOOL;
        m_pGameInstance->Publish<SAVEEVENT>(save);

        save.eToolType = TOOLTYPE::SHADER_TOOL;
        m_pGameInstance->Publish<SAVEEVENT>(save);*/
        json jLights = json::array();

        for (const auto& Lights : *m_umapLightsPtr)
        {
            LIGHT_DESC* desc = Lights.second->Get_LightDesc();
            jLights.push_back({
                { "Position", { desc->vPosition.x, desc->vPosition.y, desc->vPosition.z, desc->vPosition.w } },
                { "Range",    desc->fRange },
                { "Diffuse",  { desc->vDiffuse.x,  desc->vDiffuse.y,  desc->vDiffuse.z,  desc->vDiffuse.w } },
                { "Ambient",  { desc->vAmbient.x,  desc->vAmbient.y,  desc->vAmbient.z,  desc->vAmbient.w } },
                { "Specular", { desc->vSpecular.x, desc->vSpecular.y, desc->vSpecular.z, desc->vSpecular.w } }
                });
        }

        ofstream ofs(L"../../DataFiles/Lights/ST1_Lights.json");
        if (!ofs.is_open())
            return;

        ofs << jLights.dump(4);
        ofs.close();

        return;
    }
    if (ImGui::Button("Load"))
    {
        ////LOADEVENT load;
        //  //load.eToolType = TOOLTYPE::MAP_TOOL;
        //  //m_pGameInstace->Publish<LOADEVENT>(load);

        //COUT("조합 로드");

        //LevelLoadResult result = Parsing_Loader::Load_LevelData("../../DataFiles/Level_Sample");

        ////COUT("=== 조합 결과 ===");
        ////COUT("맵 오브젝트: " + to_string(result.vecMapObjects.size()) + "개");
        ////COUT("쉐이더 설정: " + to_string(result.umapShaderInfos.size()) + "개");

        //for (const auto& mapInfo : result.vecMapObjects)
        //{
        //    COUT("--- 오브젝트: " + mapInfo.strObjectKey + " ---");

        //    // 쉐이더 정보 찾기
        //    auto shaderIter = result.umapShaderInfos.find(mapInfo.strObjectKey);
        //    if (shaderIter != result.umapShaderInfos.end())
        //    {
        //        const ShaderObjectInfo& shaderInfo = shaderIter->second;
        //        COUT("  쉐이더: " + wstringToString(shaderInfo.wstrShaderPrototype));
        //        COUT("  패스 수: " + to_string(shaderInfo.vecMeshPasses.size()));
        //        result.bReadShader = true;
        //    }
        //    else
        //    {
        //        COUT("  쉐이더 정보 없음");
        //    }
        //}
        //for (const auto& mapInfo : result.vecMapObjects)
        //{
        //    // 오브젝트 생성
        //    GameObject* pObj = nullptr;
        //    GameObject::GAMEOBJECT_DESC desc;
        //    desc.bSetWorldPos = true;
        //    desc.matWorldPos = mapInfo.matWorld;

        //    m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::LOGO), mapInfo.wstrPrototypeName, _UINT(LEVEL::LOGO), mapInfo.wstrLayerName, &pObj, &desc);

        //    if (!pObj)
        //        continue;

        //    // 쉐이더 정보 적용
        //    if (result.bReadShader == true)
        //    {
        //        auto shaderIter = result.umapShaderInfos.find(mapInfo.strObjectKey);
        //        if (shaderIter != result.umapShaderInfos.end())
        //        {
        //            const ShaderObjectInfo& shaderInfo = shaderIter->second;

        //            // 실제 쉐이더 적용
        //            pObj->Set_Shader(shaderInfo.wstrShaderPrototype);
        //            pObj->Set_Passes(shaderInfo.vecMeshPasses);

        //            COUT("[적용] " + mapInfo.strObjectKey + " <- " + wstringToString(shaderInfo.wstrShaderPrototype));
        //        }
        //    }
        //}
        ifstream ifs(L"../../DataFiles/Lights/ST1_Lights.json");
        if (!ifs.is_open())
            return;

        json jLights;
        ifs >> jLights;
        ifs.close();

        for (const auto& jLight : jLights)
        {
            LIGHT_DESC Desc = {};
            Desc.eType = LIGHT::POINT;

            auto& p = jLight["Position"];
            Desc.vPosition = _float4(p[0], p[1], p[2], p[3]);

            Desc.fRange = jLight["Range"];

            auto& d = jLight["Diffuse"];
            Desc.vDiffuse = _float4(d[0], d[1], d[2], d[3]);

            auto& a = jLight["Ambient"];
            Desc.vAmbient = _float4(a[0], a[1], a[2], a[3]);

            auto& s = jLight["Specular"];
            Desc.vSpecular = _float4(s[0], s[1], s[2], s[3]);

            /* [EDIT] Add_Light 시그니처에 맞게 수정 */
            _int isize = (_int)m_umapLightsPtr->size();
            m_pGameInstance->Add_Light(isize, Desc);
        }

        return;
    }
}
/******************************************************* 쉐이더탭 *******************************************************/



//////////////////////////////////////////////////////// HDR탭 ////////////////////////////////////////////////////////
void ShaderTool::ShaderImgui::HDRTab()
{
    //imgui로 건드릴것들 
    /*float fBloomIntensity = 0.1f;
    float exposure = 1.2f;
    ldrColor = pow(ldrColor, 1.0 / 2.2); 감마보정값
    //브라이트니스 0.8f
        float4 로 일단 하게끔*/
    ImGui::DragFloat("BambooEmission", &m_vShaderDesc.fBambooEmission, 0.1f);
    ImGui::DragFloat("BloomIntensity", &m_vHDROption.x, 0.001f);
    ImGui::DragFloat("HDR_Exposure", &m_vHDROption.y, 0.001f);
    ImGui::DragFloat("Gamma", &m_vHDROption.z, 0.001f);
    ImGui::DragFloat("BrightExtract", &m_vHDROption.w, 0.001f);
    ImGui::Separator();
    ImGui::Text("SSAO");
    ImGui::DragFloat("SSAO ON/OFF", &m_vSSAOOption.x, 0.001f);
    ImGui::DragFloat("SSAORadius", &m_vSSAOOption.y, 0.001f);
    ImGui::DragFloat("SSAOBias", &m_vSSAOOption.z, 0.001f);
   
}
/******************************************************* HDR탭 *******************************************************/



//////////////////////////////////////////////////////// 쉐도우탭 ////////////////////////////////////////////////////////
void ShaderTool::ShaderImgui::ShadowTab()
{
    auto DragFloat4Row = [](const char* label, _float4& v, float speed = 0.01f)
    {
        ImGui::Text("%s", label);
        ImGui::PushItemWidth(80);
        ImGui::DragFloat(("##" + std::string(label) + "X").c_str(), &v.x, speed); ImGui::SameLine();
        ImGui::DragFloat(("##" + std::string(label) + "Y").c_str(), &v.y, speed); ImGui::SameLine();
        ImGui::DragFloat(("##" + std::string(label) + "Z").c_str(), &v.z, speed); ImGui::SameLine();
        ImGui::DragFloat(("##" + std::string(label) + "W").c_str(), &v.w, speed);
        ImGui::PopItemWidth();
    };

    // [ADDED] 섀도우 Desc 공통 UI
    auto DrawShadowDesc = [&DragFloat4Row](const char* title, auto& desc)
    {
        ImGui::Separator();
        ImGui::Text("%s", title);
        ImGui::Spacing();

        DragFloat4Row((std::string(title) + " Position").c_str(), desc.vPosition);
        DragFloat4Row((std::string(title) + " At").c_str(), desc.vAt);

        ImGui::PushItemWidth(150);
        std::string id = std::string("##") + title;
        ImGui::DragFloat(("Orth Size" + id).c_str(), &desc.vOrthSize, 0.001f);
        ImGui::DragFloat(("Near" + id).c_str(), &desc.fNearZ, 0.001f);
        ImGui::DragFloat(("Far" + id).c_str(), &desc.fFarZ, 0.001f);
        ImGui::DragFloat(("Depth Bias" + id).c_str(), &desc.fDepth, 0.001f);
        ImGui::PopItemWidth();
    };

    DrawShadowDesc("Static Shadow", m_vStaticShadowDesc);
    DrawShadowDesc("Dynamic Shadow", m_vDynamicShadowDesc);

    m_pGameInstance->Add_ShadowLight(m_vStaticShadowDesc);
    const _int NumCascade = 4;
    _float bias[NumCascade] = { 0.001f, 0.002f, 0.003f, 0.005f };
    m_pGameInstance->Add_ShadowCascadeLight(NumCascade, m_vDynamicShadowDesc, bias);

}
void ShaderTool::ShaderImgui::ColorTab()
{
    ImGui::Separator();
    ImGui::Text("Color Grading");

    // [EDIT] ON/OFF → Checkbox
    bool bColorGrading = (m_vShaderDesc.g_fEnableColorGrading >= 1.f);
    if (ImGui::Checkbox("Enable ColorGrading", &bColorGrading))
        m_vShaderDesc.g_fEnableColorGrading = bColorGrading ? 1.f : 0.f;

    ImGui::PushItemWidth(150);

    ImGui::Text("Shadow Tint");
    ImGui::ColorEdit3("##ShadowTint", &m_vShaderDesc.vShadowTintColor.x,
        ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::DragFloat("Shadow Weight", &m_vShaderDesc.fShadowTintWeight, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Midtone Tint");
    ImGui::ColorEdit3("##MidtoneTint", &m_vShaderDesc.vMidtoneTintColor.x,
        ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::DragFloat("Midtone Weight", &m_vShaderDesc.fMidtoneTintWeight, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Highlight Tint");
    ImGui::ColorEdit3("##HighlightTint", &m_vShaderDesc.vHighlightTintColor.x,
        ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::DragFloat("Highlight Weight", &m_vShaderDesc.fHighlightTintWeight, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::DragFloat("Shadow Range", &m_vShaderDesc.fShadowRange, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Highlight Range", &m_vShaderDesc.fHighLightRange, 0.01f, 0.0f, 1.0f);

    ImGui::PopItemWidth();
}
void ShaderTool::ShaderImgui::FogTab()
{
    ImGui::Separator();
    ImGui::Text("Fog Color");
    ImGui::PushItemWidth(80);
    ImGui::Text("R"); ImGui::SameLine();
    ImGui::DragFloat("##fogR", &m_vShaderDesc.vFogColor.x, 0.01f); ImGui::SameLine();
    ImGui::Text("G"); ImGui::SameLine();
    ImGui::DragFloat("##fogG", &m_vShaderDesc.vFogColor.y, 0.01f); ImGui::SameLine();
    ImGui::Text("B"); ImGui::SameLine();
    ImGui::DragFloat("##fogB", &m_vShaderDesc.vFogColor.z, 0.01f);
    ImGui::PopItemWidth();

    bool bFogEnabled = (m_vShaderDesc.fFogEnable >= 1.f);
    if (ImGui::Checkbox("Fog Enable", &bFogEnabled))
        m_vShaderDesc.fFogEnable = bFogEnabled ? 1.f : 0.f;

    ImGui::Separator();
    ImGui::Text("Distance Fog");
    ImGui::PushItemWidth(150);
    ImGui::DragFloat("Fog Start", &m_vShaderDesc.fFogStartDist, 0.1f);
    ImGui::DragFloat("Fog End", &m_vShaderDesc.fFogEndDist, 0.1f);
    ImGui::DragFloat("Fog Density", &m_vShaderDesc.fFogDensity, 0.00001f, 0.0f, 1.0f, "%.6f");

    int fogType = (m_vShaderDesc.fogType < 0.5f) ? 0 : 1;
    if (ImGui::Combo("Fog Type", &fogType, "Linear\0Exponential\0"))
        m_vShaderDesc.fogType = (fogType == 0) ? 0.f : 1.f;
    ImGui::PopItemWidth();

    ImGui::Separator();
    ImGui::Text("Height Fog");
    ImGui::PushItemWidth(150);
    ImGui::DragFloat("Base Height", &m_vShaderDesc.fFogBaseHeight, 0.1f);
    ImGui::DragFloat("Fade Height", &m_vShaderDesc.fFogFadeHeight, 0.1f);
    ImGui::DragFloat("Height Intensity", &m_vShaderDesc.fHeightFogIntensity, 0.01f);
    ImGui::DragFloat("Max Opacity", &m_vShaderDesc.fFogIntensity, 0.01f, 0.0f, 1.0f);
    ImGui::PopItemWidth();
}

/******************************************************* 쉐도우탭 *******************************************************/

//////////////////////////////////////////////////////// PBR탭 ////////////////////////////////////////////////////////
void ShaderTool::ShaderImgui::PBRTab()
{
    ImGui::Separator();
    ImGui::Text("PBR Toon Parameters");
    ImGui::PushItemWidth(150);

    // [EDITED] 조명 - X/Y 개별 분리 + 역할 표시
    ImGui::Text("Lighting");
    ImGui::DragFloat("Light Multiplier", &m_vShaderDesc.fLightMultiplier, 0.01f, 0.0f, 20.0f);
    ImGui::DragFloat("Ambient Strength", &m_vShaderDesc.fAmbientStrength, 0.01f, 0.0f, 2.0f);
    ImGui::DragFloat("Toon Shadow (X Start)", &m_vShaderDesc.vToonShadowRange.x, 0.01f, 0.0f, 1.0f); //X~Y까지 부드럽게 전환(이하값 0 이상값 1)
    ImGui::DragFloat("Toon Shadow (Y End)", &m_vShaderDesc.vToonShadowRange.y, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Toon Bright (X Dark)", &m_vShaderDesc.vToonBrightnessRange.x, 0.01f, 0.0f, 2.0f);
    ImGui::DragFloat("Toon Bright (Y Bright)", &m_vShaderDesc.vToonBrightnessRange.y, 0.01f, 0.0f, 2.0f);
    ImGui::DragFloat("Ambient Floor (X Albedo)", &m_vShaderDesc.vAmbientFloorMin.x, 0.001f, 0.0f, 0.5f);
    ImGui::DragFloat("Ambient Floor (Y Absolute)", &m_vShaderDesc.vAmbientFloorMin.y, 0.001f, 0.0f, 0.5f);
    ImGui::DragFloat("Back Fill Strength", &m_vShaderDesc.fBackFillStrength, 0.01f, 0.0f, 2.0f);

    ImGui::Separator();
    ImGui::Text("SSS / Skin");
    ImGui::ColorEdit3("SSS Color", &m_vShaderDesc.vSSSColor.x,
        ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::DragFloat("SSS Power", &m_vShaderDesc.fSSSPower, 0.1f, 0.1f, 20.0f);
    ImGui::DragFloat("SSS Intensity", &m_vShaderDesc.fSSSIntensity, 0.01f, 0.0f, 2.0f);
    ImGui::ColorEdit3("Skin Tint", &m_vShaderDesc.vSkinTint.x,
        ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::DragFloat("Skin Roughness Min", &m_vShaderDesc.fSkinRoughnessMin, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Rim Light");
    ImGui::DragFloat("Rim Power", &m_vShaderDesc.fRimPower, 0.1f, 0.1f, 20.0f);
    ImGui::DragFloat("Rim Intensity", &m_vShaderDesc.fRimIntensity, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("Rim Mask (X Light Start)", &m_vShaderDesc.vRimMaskRange.x, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Rim Mask (Y Light End)", &m_vShaderDesc.vRimMaskRange.y, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Specular");
    ImGui::DragFloat("Spec Boost (X Smooth)", &m_vShaderDesc.vSpecBoostRange.x, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Spec Boost (Y Rough)", &m_vShaderDesc.vSpecBoostRange.y, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Specular Intensity", &m_vShaderDesc.fSpecularIntensity, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("Spec Boost Map (X Smooth)", &m_vShaderDesc.vSpecBosstMapRange.x, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Spec Boost Map (Y Rough)", &m_vShaderDesc.vSpecBosstMapRange.y, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Spec Boost Monster (X Smooth)", &m_vShaderDesc.vSpecMonsterBoostRange.x, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Spec Boost Monster (Y Rough)", &m_vShaderDesc.vSpecMonsterBoostRange.y, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Specular Map Intensity", &m_vShaderDesc.fSpecularMapIntensity, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("Under Zero Spec(Dir)", &m_vShaderDesc.fUnderMapZeroDirSpecular, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Min Diffuse", &m_vShaderDesc.fMinDiffuse, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Cloth Spec SoftClamp", &m_vShaderDesc.fSpecSoftClamp, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Cave Wet", &m_vShaderDesc.fCaveMapWet, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("PBR Correction");
    ImGui::DragFloat("K Value", &m_vShaderDesc.fKValue, 0.1f, 1.0f, 16.0f);
    ImGui::DragFloat("Material Roughness Min", &m_vShaderDesc.fMtrlRoughnessMin, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Shadow");
    ImGui::DragFloat("BackFace Shadow (X Start)", &m_vShaderDesc.vBackFaceShaowRange.x, 0.01f, -1.0f, 1.0f);
    ImGui::DragFloat("BackFace Shadow (Y End)", &m_vShaderDesc.vBackFaceShaowRange.y, 0.01f, -1.0f, 1.0f);
    ImGui::DragFloat("Shadow Bright (Min)", &m_vShaderDesc.fShadowBright, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Shadow BackFace (Min)", &m_vShaderDesc.fShadowBackBright, 0.01f, 0.0f, 1.0f);

    ImGui::PopItemWidth();
}
void ShaderTool::ShaderImgui::ChangeMapColor()
{
    if (m_pGameInstance->Get_DIKeyState(DIK_N)) //지상
    {
        //테스트용으로 이동하고 바로값
// ---- HDR ----
        m_vShaderDesc.fBambooEmission = 25.f;
        m_vHDROption.x = 0.1f;   // BloomIntensity
        m_vHDROption.y = 0.7f;   // HDR_Exposure
        m_vHDROption.z = 2.2f;   // Gamma
        m_vHDROption.w = 0.8f;   // BrightExtract

        // ---- SSAO ----
        m_vSSAOOption.x = 1.f;    // ON/OFF
        m_vSSAOOption.y = 0.5f;   // SSAORadius
        m_vSSAOOption.z = 0.01f;  // SSAOBias

        // ---- Color Grading ----
        m_vShaderDesc.g_fEnableColorGrading = 1.f;
        m_vShaderDesc.vShadowTintColor = _float3(1.1f, 0.95f, 0.8f);
        m_vShaderDesc.fShadowTintWeight = 0.6f;
        m_vShaderDesc.vMidtoneTintColor = _float3(1.15f, 1.05f, 0.85f);
        m_vShaderDesc.fMidtoneTintWeight = 0.5f;
        m_vShaderDesc.vHighlightTintColor = _float3(1.15f, 1.07f, 0.78f);
        m_vShaderDesc.fHighlightTintWeight = 0.7f;
        m_vShaderDesc.fShadowRange = 0.3f;
        m_vShaderDesc.fHighLightRange = 0.5f;

        // ---- Fog ----
        m_vShaderDesc.vFogColor = _float3(0.55f, 0.48f, 0.38f);
        m_vShaderDesc.fFogEnable = 1.f;

        // Distance Fog
        m_vShaderDesc.fFogStartDist = 50.f;
        m_vShaderDesc.fFogEndDist = 300.f;
        m_vShaderDesc.fFogDensity = 0.00015f;
        m_vShaderDesc.fogType = 1.f;  // Exponential

        // Height Fog
        m_vShaderDesc.fFogBaseHeight = -62.1f;
        m_vShaderDesc.fFogFadeHeight = 42.8f;
        m_vShaderDesc.fHeightFogIntensity = 2.44f;
        m_vShaderDesc.fFogIntensity = 0.73f;

        // ---- Lighting ----
        m_vShaderDesc.fLightMultiplier = 2.f;
        m_vShaderDesc.fAmbientStrength = 0.3f;
        m_vShaderDesc.vToonShadowRange = _float2(0.05f, 0.25f);
        m_vShaderDesc.vToonBrightnessRange = _float2(0.05f, 1.f);
        m_vShaderDesc.vAmbientFloorMin = _float2(0.08f, 0.02f);
        m_vShaderDesc.fBackFillStrength = 1.2f;

        // ---- SSS / Skin ----
        m_vShaderDesc.vSSSColor = _float3(0.9f, 0.82f, 0.72f);
        m_vShaderDesc.fSSSPower = 4.f;
        m_vShaderDesc.fSSSIntensity = 0.1f;
        m_vShaderDesc.vSkinTint = XMFLOAT3(0.873f, 0.79f, 0.736f);
        m_vShaderDesc.fSkinRoughnessMin = 0.5f;

        // ---- Rim Light ----
        m_vShaderDesc.fRimPower = 4.f;        // 미사용 (임시)
        m_vShaderDesc.fRimIntensity = 0.8f;
        m_vShaderDesc.vRimMaskRange = _float2(0.1f, 0.5f);

        // ---- Specular ----
        m_vShaderDesc.vSpecBoostRange = _float2(4.f, 1.f);
        m_vShaderDesc.fSpecularIntensity = 1.f;
        m_vShaderDesc.vSpecBosstMapRange = _float2(5.f, 1.f);
        m_vShaderDesc.fSpecularMapIntensity = 1.f;
        m_vShaderDesc.fUnderMapZeroDirSpecular = 1.f;
        m_vShaderDesc.fMinDiffuse = 0.05f;
        m_vShaderDesc.fSpecSoftClamp = 0.2f;
        m_vShaderDesc.fCaveMapWet = 0.1f;

        // ---- PBR Correction ----
        m_vShaderDesc.fKValue = 8.f;
        m_vShaderDesc.fMtrlRoughnessMin = 0.15f;

        // ---- Shadow ----
        m_vShaderDesc.vBackFaceShaowRange = _float2(-0.2f, 0.3f);
        m_vShaderDesc.fShadowBright = 0.9f;
        m_vShaderDesc.fShadowBackBright = 0.3f;

        // ---- GodRay ----
        m_vShaderDesc.fGodRayDensity = 1.f;
        m_vShaderDesc.fGodRayDecay = 0.98f;
        m_vShaderDesc.fGodRayWeight = 0.1f;
        m_vShaderDesc.fGodRayExposure = 0.5f;
        m_vShaderDesc.vGodRayColor = _float4{ 1.f, 1.f, 1.f, 1.f };

        // ---- GodRay Mesh ----
        m_vShaderDesc.fGodRayMeshScrollSpeed = 0.02f;
        m_vShaderDesc.fGodRayMeshSwayFreq = 1.f;
        m_vShaderDesc.fGodRayMeshSwayPhase = 1.f;
        m_vShaderDesc.fGodRayMeshSwayAmp = 0.01f;
        m_vShaderDesc.fGodRayMeshContrast = 2.f;
        m_vShaderDesc.fGodRayMeshIntensity = 2.5f;
        m_vShaderDesc.fGodRayMeshEdgeFade = 0.6f;
        m_vShaderDesc.vGodRayMeshColor = _float3{ 1.f, 0.9f, 0.7f };
        m_vShaderDesc.fSoftParticleRange = 5.f;
        m_vShaderDesc.fGodRayMeshAngleFadePower = 3.5f;
        m_vShaderDesc.fGodRayMeshFadeNear = 1.f;
        m_vShaderDesc.fGodRayMeshFadeFar = 3.5f;

        // ---- Camera Motion Blur ----
        m_vShaderDesc.fCamMotionBlurIntensity = 0.3f;
        m_vShaderDesc.fCamMotionBlurMaxVelocity = 0.05f;
        m_vShaderDesc.fCamMotionBlurSamples = 12.f;

        // ---- Monster Specular ----
        m_vShaderDesc.fMonsterSpecularIntensity = 1.f;
        m_vShaderDesc.vSpecMonsterBoostRange = _float2(0.6f, 0.6f);

    }
    else if(m_pGameInstance->Get_DIKeyState(DIK_M)) //지하
    {
        //float4로 일단 HDR 값 조절하게끔
        m_vHDROption.y = 0.7f; //"HDR_Exposure", 
        m_vShaderDesc.fLightMultiplier = 1.f; //전체조명세기  
        m_vShaderDesc.fAmbientStrength = 0.15f; //앰비언트세기

        m_vShaderDesc.vFogColor = _float3(0.05f, 0.1f, 0.25f);
        m_vShaderDesc.fFogStartDist = 20.f; //포그 시작거리
        m_vShaderDesc.fogType = 0.f; //포그타입 0.5로 나뉨
        m_vShaderDesc.vShadowTintColor = _float3(0.85f, 0.85f, 1.15f);
        m_vShaderDesc.vMidtoneTintColor = _float3(0.92f, 0.90f, 1.05f);
        m_vShaderDesc.vHighlightTintColor = _float3(0.95f, 0.93f, 1.08f);

        m_vShaderDesc.vSpecBoostRange = XMFLOAT2(2.f, 2.f); //x축 매끈한면 부스트 y축 거친면 부스트 //1.2 0.3
        m_vShaderDesc.fSpecularIntensity = 1.f; //전체 스펙 세기 //0.7
        m_vShaderDesc.vSpecBosstMapRange = XMFLOAT2(4.f, 1.f); //x축 매끈한면 부스트 y축 거친면 부스트 //1.2 0.3
        m_vShaderDesc.fSpecularMapIntensity = 1.f; //전체 스펙 세기 //0.7
        m_vShaderDesc.fUnderMapZeroDirSpecular = 0.005f;

        m_vShaderDesc.fBambooEmission = 30.f;
        m_vShaderDesc.fCaveMapWet = 0.8f; 
    }
    else if (m_pGameInstance->Get_DIKeyState(DIK_B))
    {
        //테스트용으로 이동하고 바로값
    //테스트용으로 이동하고 바로값
    // ---- HDR ----
    m_vShaderDesc.fBambooEmission = 20.f;
    m_vShaderDesc.fUnderBambooEmission = 20.f;
    m_vHDROption.x = 0.1f;   // BloomIntensity
    m_vHDROption.y = 0.7f;   // HDR_Exposure
    m_vHDROption.z = 2.2f;   // Gamma
    m_vHDROption.w = 0.8f;   // BrightExtract

    // ---- SSAO ----
    m_vSSAOOption.x = 1.f;    // ON/OFF
    m_vSSAOOption.y = 0.5f;   // SSAORadius
    m_vSSAOOption.z = 0.01f;  // SSAOBias

    // ---- Color Grading ----
    m_vShaderDesc.g_fEnableColorGrading = 1.f;
    m_vShaderDesc.vShadowTintColor = _float3(1.1f, 0.95f, 0.8f);
    m_vShaderDesc.fShadowTintWeight = 0.6f;
    m_vShaderDesc.vMidtoneTintColor = _float3(1.15f, 1.05f, 0.85f);
    m_vShaderDesc.fMidtoneTintWeight = 0.5f;
    m_vShaderDesc.vHighlightTintColor = _float3(1.15f, 1.07f, 0.78f);
    m_vShaderDesc.fHighlightTintWeight = 0.7f;
    m_vShaderDesc.fShadowRange = 0.3f;
    m_vShaderDesc.fHighLightRange = 0.5f;

    m_vShaderDesc.vFogColor = _float3(0.6f, 0.6f, 0.6f);
    m_vShaderDesc.fFogEnable = 1.f;

    // Distance Fog
    m_vShaderDesc.fFogStartDist = 50.f;
    m_vShaderDesc.fFogEndDist = 300.f;
    m_vShaderDesc.fFogDensity = 0.00015f;
    m_vShaderDesc.fogType = 1.f;  // Exponential

    // Height Fog
    m_vShaderDesc.fFogBaseHeight = -64.f;
    m_vShaderDesc.fFogFadeHeight = 50.2f;
    m_vShaderDesc.fHeightFogIntensity = 7.f;
    m_vShaderDesc.fFogIntensity = 1.f;

    // ---- Lighting ----
    m_vShaderDesc.fLightMultiplier = 2.f;
    m_vShaderDesc.fAmbientStrength = 0.3f;
    m_vShaderDesc.vToonShadowRange = _float2(0.05f, 0.25f);
    m_vShaderDesc.vToonBrightnessRange = _float2(0.05f, 1.f);
    m_vShaderDesc.vAmbientFloorMin = _float2(0.08f, 0.02f);
    m_vShaderDesc.fBackFillStrength = 0.7f;

    // ---- SSS / Skin ----
    m_vShaderDesc.vSSSColor = _float3(0.9f, 0.82f, 0.72f);
    m_vShaderDesc.fSSSPower = 4.f;
    m_vShaderDesc.fSSSIntensity = 0.1f;
    m_vShaderDesc.vSkinTint = XMFLOAT3(0.873f, 0.79f, 0.736f);
    m_vShaderDesc.fSkinRoughnessMin = 0.5f;

    // ---- Rim Light ----
    m_vShaderDesc.fRimPower = 4.f;        // 미사용 (임시)
    m_vShaderDesc.fRimIntensity = 0.8f;
    m_vShaderDesc.vRimMaskRange = _float2(0.1f, 0.5f);

    // ---- Specular ----
    m_vShaderDesc.vSpecBoostRange = _float2(4.f, 1.f);
    m_vShaderDesc.fSpecularIntensity = 1.f;
    m_vShaderDesc.vSpecBosstMapRange = _float2(3.f, 1.f);
    m_vShaderDesc.fSpecularMapIntensity = 1.f;
    m_vShaderDesc.fUnderMapZeroDirSpecular = 1.f;
    m_vShaderDesc.fMinDiffuse = 0.05f;
    m_vShaderDesc.fSpecSoftClamp = 0.2f;
    m_vShaderDesc.fCaveMapWet = 0.1f;

    // ---- PBR Correction ----
    m_vShaderDesc.fKValue = 8.f;
    m_vShaderDesc.fMtrlRoughnessMin = 0.15f;

    // ---- Shadow ----
    m_vShaderDesc.vBackFaceShaowRange = _float2(-0.2f, 0.3f);
    m_vShaderDesc.fShadowBright = 0.9f;
    m_vShaderDesc.fShadowBackBright = 0.3f;

    // ---- GodRay ----
    m_vShaderDesc.fGodRayDensity = 1.f;
    m_vShaderDesc.fGodRayDecay = 0.98f;
    m_vShaderDesc.fGodRayWeight = 0.1f;
    m_vShaderDesc.fGodRayExposure = 0.5f; //다단계 블러먹이면서 1.5->0.5
    m_vShaderDesc.vGodRayColor = _float4{ 1.f, 0.9f, 0.7f, 1.f };
    m_vShaderDesc.vGodRayPosition = _float4{ -56.23, 27.98f, -16.22f, 1.f }; //_float4{ -296.82, 102.69f, -69.81f, 1.f };

    // ---- GodRay Mesh ----
    m_vShaderDesc.fGodRayMeshScrollSpeed = 0.02f;
    m_vShaderDesc.fGodRayMeshSwayFreq = 1.f;
    m_vShaderDesc.fGodRayMeshSwayPhase = 1.f;
    m_vShaderDesc.fGodRayMeshSwayAmp = 0.14f;
    m_vShaderDesc.fGodRayMeshContrast = 1.5f;
    m_vShaderDesc.fGodRayMeshIntensity = 1.1f;
    m_vShaderDesc.fGodRayMeshEdgeFade = 0.79f;
    m_vShaderDesc.vGodRayMeshColor = _float3{ 1.f, 0.9f, 0.7f };
    m_vShaderDesc.fSoftParticleRange = 0.1f;
    m_vShaderDesc.fGodRayMeshAngleFadePower = 2.0f;
    m_vShaderDesc.fGodRayMeshFadeNear = 3.6f;
    m_vShaderDesc.fGodRayMeshFadeFar = 13.4f;

    // ---- Camera Motion Blur ----
    m_vShaderDesc.fCamMotionBlurIntensity = 0.3f;
    m_vShaderDesc.fCamMotionBlurMaxVelocity = 0.05f;
    m_vShaderDesc.fCamMotionBlurSamples = 12.f;

    // ---- Monster Specular ----
    m_vShaderDesc.fMonsterSpecularIntensity = 1.f;
    m_vShaderDesc.vSpecMonsterBoostRange = _float2(0.5f, 1.f);

    }
}
void ShaderTool::ShaderImgui::GodRayTab()
{
    ImGui::DragFloat("Density", &m_vShaderDesc.fGodRayDensity, 0.01f, 0.f, 3.f, "%.2f");
    ImGui::DragFloat("Decay", &m_vShaderDesc.fGodRayDecay, 0.001f, 0.9f, 1.f, "%.3f");
    ImGui::DragFloat("Weight", &m_vShaderDesc.fGodRayWeight, 0.01f, 0.f, 1.f, "%.3f");
    ImGui::DragFloat("Exposure", &m_vShaderDesc.fGodRayExposure, 0.1f, 0.f, 10.f, "%.1f");
    ImGui::ColorEdit4("God Ray Color", (float*)&m_vShaderDesc.vGodRayColor);

    static int iSelectedGodRay = 0;
    const char* godRayNames[] = { "GodRayMesh_0", "GodRayMesh_1", "GodRayMesh_2", "GodRayMesh_3", "GodRayMesh_4", "GodRayMesh_5",
    "GodRayMesh_6", "GodRayMesh_7", "GodRayMesh_8" , "GodRayMesh_9", "GodRayMesh_10", "GodRayMesh_11", "GodRayMesh_12", "GodRayMesh_13", "GodRayMesh_14" };
    const wchar_t* godRayKeys[] = { L"GodRayMesh_0", L"GodRayMesh_1", L"GodRayMesh_2", L"GodRayMesh_3", L"GodRayMesh_4", L"GodRayMesh_5",
    L"GodRayMesh_6", L"GodRayMesh_7", L"GodRayMesh_8" , L"GodRayMesh_9", L"GodRayMesh_10", L"GodRayMesh_11", L"GodRayMesh_12", L"GodRayMesh_13", L"GodRayMesh_14" };

    ImGui::Combo("Select GodRay", &iSelectedGodRay, godRayNames, IM_ARRAYSIZE(godRayNames));

    GameObject* pSelected = m_pGameInstance->Get_GameObject(_UINT(LEVEL::LOGO), Layer_GameObject, godRayKeys[iSelectedGodRay]);
    ImGui::Separator();
    _float4 fPos = {};
    _float3 fScale = {};
    if (pSelected->Get_Transform() != nullptr)
    {
        Transform* tempTran = pSelected->Get_Transform();
        _vector vScale{}, vQuater{}, vTrans{};
        XMMatrixDecompose(&vScale, &vQuater, &vTrans, tempTran->Get_WorldMatrix());

        // /* 쿼터니온 → 오일러 각도 (도) */
        _float3 vRadian{};
        SimpleMath::Quaternion q = vQuater;
        XMStoreFloat3(&vRadian, q.ToEuler() * (180.0f / XM_PI));

        // /* Rotation */
        ImGui::Text("Rotation");
        ImGui::PushItemWidth(80);
        ImGui::Text("X"); ImGui::SameLine();
        ImGui::DragFloat("##rotX", &vRadian.x, 0.5f, -89.f, 89.f, "%.1f"); ImGui::SameLine();
        ImGui::Text("Y"); ImGui::SameLine();
        ImGui::DragFloat("##rotY", &vRadian.y, 0.5f, -360.f, 360.f, "%.1f"); ImGui::SameLine();
        ImGui::Text("Z"); ImGui::SameLine();
        ImGui::DragFloat("##rotZ", &vRadian.z, 0.5f, -360.f, 360.f, "%.1f");
        ImGui::PopItemWidth();

        vRadian.x = min(max(vRadian.x, -89.f), 89.f);
        tempTran->Rotation(XMConvertToRadians(vRadian.x), XMConvertToRadians(vRadian.y), XMConvertToRadians(vRadian.z));

        // /* Scale */
        _float3 fScale{};
        XMStoreFloat3(&fScale, vScale);

        ImGui::Text("Scale");
        ImGui::PushItemWidth(80);
        ImGui::Text("X"); ImGui::SameLine();
        ImGui::DragFloat("##scaleX", &fScale.x, 0.01f, 0.01f, 100.f, "%.2f"); ImGui::SameLine();
        ImGui::Text("Y"); ImGui::SameLine();
        ImGui::DragFloat("##scaleY", &fScale.y, 0.01f, 0.01f, 100.f, "%.2f"); ImGui::SameLine();
        ImGui::Text("Z"); ImGui::SameLine();
        ImGui::DragFloat("##scaleZ", &fScale.z, 0.01f, 0.01f, 100.f, "%.2f");
        ImGui::PopItemWidth();

        tempTran->Set_Scale(fScale.x, fScale.y, fScale.z);

        // /* Position */
        _float3 fPos{};
        XMStoreFloat3(&fPos, vTrans);

        ImGui::Text("Position");
        ImGui::PushItemWidth(80);
        ImGui::Text("X"); ImGui::SameLine();
        ImGui::DragFloat("##posX", &fPos.x, 0.1f, -1000.f, 1000.f, "%.1f"); ImGui::SameLine();
        ImGui::Text("Y"); ImGui::SameLine();
        ImGui::DragFloat("##posY", &fPos.y, 0.1f, -1000.f, 1000.f, "%.1f"); ImGui::SameLine();
        ImGui::Text("Z"); ImGui::SameLine();
        ImGui::DragFloat("##posZ", &fPos.z, 0.1f, -1000.f, 1000.f, "%.1f");
        ImGui::PopItemWidth();

        tempTran->Set_State(DIRECTION::POSITION, _float4{ fPos.x, fPos.y, fPos.z, 1.f });
    }

    ImGui::Separator();
    // /* UV 스크롤 */
    ImGui::DragFloat("Scroll Speed", &m_vShaderDesc.fGodRayMeshScrollSpeed, 0.001f, 0.f, 1.f, "%.3f");
    // /* 좌우 흔들림 */
    ImGui::DragFloat("Sway Freq", &m_vShaderDesc.fGodRayMeshSwayFreq, 0.1f, 0.f, 20.f, "%.2f");
    ImGui::DragFloat("Sway Phase", &m_vShaderDesc.fGodRayMeshSwayPhase, 0.1f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Sway Amp", &m_vShaderDesc.fGodRayMeshSwayAmp, 0.001f, 0.f, 1.f, "%.3f");
    // /* 노이즈 & 밝기 */
    ImGui::DragFloat("Contrast", &m_vShaderDesc.fGodRayMeshContrast, 0.1f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Intensity", &m_vShaderDesc.fGodRayMeshIntensity, 0.1f, 0.f, 20.f, "%.2f");
    // /* 가장자리 페이드 */
    ImGui::DragFloat("Edge Fade", &m_vShaderDesc.fGodRayMeshEdgeFade, 0.01f, 0.f, 1.f, "%.3f");
    // /* 색상 */
    ImGui::ColorEdit3("Color", &m_vShaderDesc.vGodRayMeshColor.x);
    ImGui::DragFloat("Soft Particle", &m_vShaderDesc.fSoftParticleRange, 0.1f, 0.1f, 50.f, "%.1f");
    ImGui::DragFloat("Angle Fade Power", &m_vShaderDesc.fGodRayMeshAngleFadePower, 0.1f, 0.1f, 10.f, "%.1f");
    ImGui::DragFloat("Fade Near", &m_vShaderDesc.fGodRayMeshFadeNear, 0.1f, 0.f, 100.f, "%.1f");
    ImGui::DragFloat("Fade Far", &m_vShaderDesc.fGodRayMeshFadeFar, 0.1f, 0.f, 100.f, "%.1f");
}
/******************************************************* PBR탭 *******************************************************/

//////////////////////////////////////////////////////// 라이트탭 ////////////////////////////////////////////////////////
void ShaderTool::ShaderImgui::LightTab()
{
    auto ColorEdit4Row = [](const char* label, _float4& v)
    {
        ImGui::ColorEdit4(label, &v.x, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    };

    auto DragFloat4Row = [](const char* label, _float4& v, float speed = 0.01f,
        float vMin = 0.f, float vMax = 0.f)
    {
        ImGui::Text("%s", label);
        ImGui::PushItemWidth(80);
        string id = string("##") + label;
        ImGui::DragFloat((id + "X").c_str(), &v.x, speed, vMin, vMax); ImGui::SameLine();
        ImGui::DragFloat((id + "Y").c_str(), &v.y, speed, vMin, vMax); ImGui::SameLine();
        ImGui::DragFloat((id + "Z").c_str(), &v.z, speed, vMin, vMax); ImGui::SameLine();
        ImGui::DragFloat((id + "W").c_str(), &v.w, speed, vMin, vMax);
        ImGui::PopItemWidth();
    };

    _int isize = (_int)m_umapLightsPtr->size();
    if (isize <= 0)
        return;

    m_DescVec.clear();
    for (int i = 0; i < isize; i++)
    {
        m_DescVec.push_back(m_pGameInstance->Get_LightDesc(i));
    }

    static int item_selected_idx = 0;
    _int DirectionalCnt = 0;
    _int PointCnt = 0;
    _int SpotCnt = 0;

    if (ImGui::BeginListBox("Light"))
    {
        for (int n = 0; n < isize; n++)
        {
            const bool is_selected = (item_selected_idx == n);

            string displayName;
            if (m_DescVec[n]->eType == LIGHT::DIRECTIONAL)
                displayName = "DIRECTIONAL " + to_string(DirectionalCnt++);
            else if (m_DescVec[n]->eType == LIGHT::POINT)
                displayName = "POINT " + to_string(PointCnt++);
            else if (m_DescVec[n]->eType == LIGHT::SPOTLIGHT)
                displayName = "SPOT " + to_string(SpotCnt++);

            if (ImGui::Selectable(displayName.c_str(), is_selected))
                item_selected_idx = n;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));

    if (ImGui::Button("Create PointLight", ImVec2(150, 30)))
    {
        CreatePointLight((_int)m_DescVec.size());
    }

    ImGui::PopStyleColor(3);

    if (item_selected_idx >= isize)
        item_selected_idx = 0;

    auto& selected = *m_DescVec[item_selected_idx];

    ImGui::Separator();
    const char* typeName = "UNKNOWN";
    if (selected.eType == LIGHT::DIRECTIONAL)
        typeName = "DIRECTIONAL";
    else if (selected.eType == LIGHT::POINT)
        typeName = "POINT";
    else if (selected.eType == LIGHT::SPOTLIGHT)
        typeName = "SPOT";

    ImGui::Text("%s", typeName);
    ImGui::Spacing();

    ColorEdit4Row("Diffuse", selected.vDiffuse);
    ColorEdit4Row("Specular", selected.vSpecular);
    ColorEdit4Row("Ambient", selected.vAmbient);

    if (selected.eType == LIGHT::DIRECTIONAL)
    {
        DragFloat4Row("Direction", selected.vDirection, 0.01f, -1.f, 1.f);
    }
    else if (selected.eType == LIGHT::POINT)
    {
        DragFloat4Row("Position", selected.vPosition);

        ImGui::PushItemWidth(150);
        ImGui::DragFloat("Range", &selected.fRange, 0.1f, 0.01f, 1000.f);
        ImGui::PopItemWidth();
    }
    else if (selected.eType == LIGHT::SPOTLIGHT)
    {
        DragFloat4Row("Position", selected.vPosition);

        ImGui::PushItemWidth(150);
        DragFloat4Row("Direction", selected.vDirection, 0.01f, -1.f, 1.f);
        ImGui::DragFloat("Range", &selected.fRange, 0.1f, 0.01f, 1000.f);
        float innerAngle = XMConvertToDegrees(acos(selected.fInnerCone));
        float outerAngle = XMConvertToDegrees(acos(selected.fOuterCone));

        if (ImGui::DragFloat("Inner Angle", &innerAngle, 0.5f, 1.f, 89.f, "%.1f deg"))
            selected.fInnerCone = cos(XMConvertToRadians(innerAngle));

        if (ImGui::DragFloat("Outer Angle", &outerAngle, 0.5f, 1.f, 89.f, "%.1f deg"))
            selected.fOuterCone = cos(XMConvertToRadians(outerAngle));
        ImGui::PopItemWidth();
    }

    m_pGameInstance->Set_LightDesc(item_selected_idx, selected);
}
/******************************************************* 라이트탭 *******************************************************/



//////////////////////////////////////////////////////// 포인트라이트생성 ////////////////////////////////////////////////////////
HRESULT ShaderTool::ShaderImgui::CreatePointLight(_int PointCnt)
{
    LIGHT_DESC Desc;
    Desc.eType = LIGHT::POINT;
    Desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fRange = 10.f;
    Desc.vDiffuse = _float4(1.f, 1.0f, 1.0f, 1.f);
    Desc.vAmbient = _float4(1.0f, 1.0f, 1.0f, 1.f);
    Desc.vSpecular = Desc.vDiffuse;

    return S_OK;
}

void ShaderTool::ShaderImgui::Free()
{
    __super::Free();
}
/******************************************************* 포인트라이트생성 *******************************************************/