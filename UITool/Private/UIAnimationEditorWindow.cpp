#include "UITool_Define.h"
#include "UIAnimationEditorWindow.h"
#include "GameInstance.h"
#include "UIObject.h"
#include "Layer.h"
#include "UITool_Macro.h"
#include "UITool_Macro_Component.h"
#include "UIAnimClip.h"
#include "UIAnimation.h"





UIAnimationEditorWindow::UIAnimationEditorWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :ImguiWindow(pDevice, pContext),
    pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(pGameInstance);
    m_bOpen = false;
}

_uint UITool::UIAnimationEditorWindow::Update_Contents(_float fTimeDelta)
{
    UIObject* pSelect = pGameInstance->Get_UISelectObject();
    CHECK_NULL_RESULT(pSelect, 0);

    if (pSelect )
    {
        Component* pComp = pSelect->Get_Component_FromName(Proto_UIAnimation);
        if (pComp)
        {
         

            if (pSelect != m_pSelectObject)
            {
                if (m_pSelectObject)
                    Reset();

                m_pSelectObject = pSelect;
                Copy_To_Original(m_pCurrentClip);
                m_pCurrentClip = nullptr;
            }

            

            if (pComp)
            {


                m_pSelectAnimationComponent = dynamic_cast<UI_Animation*>(pComp);
                Change_Currentclip();
            }

            
        }
    }
    

    CHECK_NULL_RESULT(m_pCloneClip,0);
    //////////////////////Imgui코드//////////////
    CHECK_NULL_RESULT(m_pSelectAnimationComponent,0);


    CHECK_NULL_RESULT(m_pCloneClip,0);

    //클립에 대한 에디터를 띄운다.
    Render_AnimationClip();

    return 0;
}

_int UITool::UIAnimationEditorWindow::Reset()
{
    if (m_pSelectAnimationComponent)
    {
        m_pSelectAnimationComponent->Reset_CurrentClipIdx();
        m_pSelectAnimationComponent->Set_PreviewClip(nullptr);

    }
    m_pSelectObject = nullptr;
    m_pSelectAnimationComponent = nullptr;
    m_pCurrentClip = nullptr;

    if (m_pCloneClip)
    {
        m_Garbage.push_back(m_pCloneClip);
        //Safe_Release(m_pCloneClip);
    }
    m_pCloneClip = nullptr;
    return 0;
}
void UIAnimationEditorWindow::Render_AnimationClip()
{
    //////상단 컨트롤바
    char pName[128];
    strcpy_s(pName, m_pCloneClip->Get_ClipName().c_str());

    if (ImGui::InputText("ClipName:", pName,sizeof(pName)))
    {        m_pCloneClip->Get_OriginalClip()->Set_ClipName(pName);
    }

    if (ImGui::Button("Play")) 
    {
        m_pSelectAnimationComponent->Set_PreviewClip(m_pCloneClip);
        m_pSelectAnimationComponent->Play_AnimClip();
    }

    ImGui::SameLine();
    
    if (ImGui::Button("Stop"))
    {
        m_pSelectAnimationComponent->Stop_Animation();

    }

    ImGui::SameLine();

    if (ImGui::Button("Init"))
    {
        vector<UIAnimTrack*>* Tracks = m_pCloneClip->Get_Tracks();
        for (auto& Track : *Tracks)
        {
            if (Track->m_fStartTime == 0)
            {
                switch (Track->m_eAnimType)
                {
                case UIANIMTYPE::POSITION:
                {
                    m_pSelectObject->Set_Position(Track->m_vStartValue.x, Track->m_vStartValue.y);
                }
                    break;

                case UIANIMTYPE::SCALE:
                {
                    m_pSelectObject->Set_Size(Track->m_vStartValue.x, Track->m_vStartValue.y);
                }

                break;

                case UIANIMTYPE::ROTATION:
                {
                    m_pSelectObject->Set_Rotation(Track->m_vStartValue.z);
                }

                break;


                case UIANIMTYPE::ALPHA:
                {
                    m_pSelectObject->Set_Alpha(Track->m_vStartValue.x);
                }

                case UIANIMTYPE::COLOR:
                {
                    m_pSelectObject->Set_Color(m_pSelectObject->Get_OriginColor());
                }

                case UIANIMTYPE::PROGRESS :
                {
                    m_pSelectObject->Set_AnimValue(UIANIMTYPE::PROGRESS,Track->m_vStartValue);
                }

                break;
                default:
                    break;
                }
            }
        }
        

    }

    ImGui::SameLine();

    bool bLoop = m_pCloneClip->Is_Loop();
    if (ImGui::Checkbox("Loop",&bLoop))
    {
        m_pCloneClip->Set_Loop(bLoop);
    }

    ImGui::SameLine();

    bool bPingPong = m_pCloneClip->Is_Pingpong();
    if (ImGui::Checkbox("Pinpong", &bPingPong))
    {
        m_pCloneClip->Set_PingPoing(bPingPong);
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);

    _float fClipDuration = m_pCloneClip->Get_TotalDuration();

    //총 시간을 표시
    ImGui::Text("Clip Duration  : %f", fClipDuration);

    ImGui::Separator();

    Render_EditArea();
}
void UIAnimationEditorWindow::Render_EditArea()
{
    /*이미존재하는 트랙에 대한 편집창*/
    if (ImGui::BeginTable("TimelineTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
    {
        ImGui::TableSetupColumn("Track Settings", ImGuiTableColumnFlags_WidthFixed, 250.0f);
        ImGui::TableSetupColumn("Timeline", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        auto* tracks = m_pCloneClip->Get_Tracks();
        int iTrackToDelete = -1; // 삭제 체크용

        int iIdx = 0;
        for (auto iter= tracks->begin(); iter!= tracks->end();)
        {
            ImGui::TableNextRow();

            // [왼쪽: 트랙 상세 설정]
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(iIdx);
            
            /*트랙 삭제 체크*/
            if (Render_TrackSettings(tracks->at(iIdx), iIdx))
                iTrackToDelete = iIdx;

            ImGui::PopID();

            // [오른쪽: 시각적 타임라인 바]
            ImGui::TableSetColumnIndex(1);
         //   Render_TimelineBar(tracks[i], m_pCurrentClip->Get_);

             //삭제처리
            if (iTrackToDelete != -1) {
                // 메모리 해제
                auto Eraseiter = tracks->begin() + iIdx;

                Safe_Delete(*Eraseiter);
                iter = tracks->erase(Eraseiter);
            }

            else
            {
                ++iter;
                ++iIdx;
            }
        }
        ImGui::EndTable();

       
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    //트랙추가영역
    Render_AddTrack(m_pCloneClip);


}
bool UIAnimationEditorWindow::Render_TrackSettings(UIAnimTrack* pTrack,int iIndex)
{
    int Result = 0;

    string treeLabel = "[" + to_string(iIndex) + "] " + Get_AnimTypeName(pTrack);
    if (ImGui::CollapsingHeader(treeLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent(); // 약간 들여쓰기

        // 애니메이션 타입 선택
        const char* animTypes[] = { "Scale", "Rotation", "Position", "Alpha", "UVScroll","Color","Progress"};
        ImGui::SetNextItemWidth(-1); // 너비 꽉 채우기

        int iCurrentAnimType = (int)pTrack->m_eAnimType;
        ImGui::Combo("##Type", &iCurrentAnimType, animTypes, IM_ARRAYSIZE(animTypes));
        {
            pTrack->m_eAnimType = (UIANIMTYPE)iCurrentAnimType;

        }
        // 시간 설정 (Start, Duration)
        ImGui::Text("Time (Start / Dur)");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.45f);
        if (ImGui::DragFloat("##Start", &pTrack->m_fStartTime, 0.01f, 0.01f, 100.f, "D: %.2f"))
        {
            m_pCloneClip->Update_TotalDuration();
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
        if (ImGui::DragFloat("##Dur", &pTrack->m_fDuration, 0.01f, 0.01f, 100.f, "D: %.2f"))
        {
            m_pCloneClip->Update_TotalDuration();
        }

        // 3. 값 설정 (StartValue, EndValue)
        ImGui::Text("Values (Start -> End)");

        // 타입에 따라 Vector3 전체를 쓸지, float 하나만 쓸지 결정
        if (pTrack->m_eAnimType == UIANIMTYPE::ALPHA) {
            ImGui::SliderFloat("Start A", &pTrack->m_vStartValue.x, 0.f, 1.f);
            ImGui::SliderFloat("End A", &pTrack->m_vEndValue.x, 0.f, 1.f);
        }


        else if (pTrack->m_eAnimType == UIANIMTYPE::COLOR) {
            ImGui::ColorEdit3("Start", (_float*)&pTrack->m_vStartValue, 0.1f);
            ImGui::SameLine();
            if (ImGui::Button("GetCurrentValue##1"))
            {
                pTrack->m_vStartValue = Get_CurrentValue(pTrack->m_eAnimType);
            }
            ImGui::ColorEdit3("End", (_float*)&pTrack->m_vEndValue, 0.1f);
            ImGui::SameLine();
            if (ImGui::Button("GetCurrentValue##2"))
            {
                pTrack->m_vEndValue = Get_CurrentValue(pTrack->m_eAnimType);
            }
        }

        else {
            ImGui::DragFloat3("Start", (_float*)&pTrack->m_vStartValue, 0.1f);
            ImGui::SameLine();
            if (ImGui::Button("GetCurrentValue##1"))
            {
                pTrack->m_vStartValue = Get_CurrentValue(pTrack->m_eAnimType);
            }
            ImGui::DragFloat3("End", (_float*)&pTrack->m_vEndValue, 0.1f);
            ImGui::SameLine();
            if (ImGui::Button("GetCurrentValue##2"))
            {
                pTrack->m_vEndValue = Get_CurrentValue(pTrack->m_eAnimType);
            }
        }

        // 4. 보간 설정 (Ease)
        const char* easeTypes[] = { "Linear","Smooth","Imme","Shake","Current"};
        int iSelect = static_cast<int>(pTrack->m_eEase);
        ImGui::Text("Interpolation");
        ImGui::SetNextItemWidth(-1);
        ImGui::Combo("##Ease", &iSelect, easeTypes, IM_ARRAYSIZE(easeTypes));
        {
             pTrack->m_eEase = EASETYPE(iSelect);
        }
        // 5. 트랙 삭제 버튼 (빨간색)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("Delete Track", ImVec2(-1, 0))) {

            Result = 1;
        }
        ImGui::PopStyleColor();

        ImGui::Unindent();
        ImGui::Spacing();
    }

    return Result;
}
void UIAnimationEditorWindow::Render_TimelineBar()
{
}
void UIAnimationEditorWindow::Render_AddTrack(UIAnimClip* pClip)
{
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Add New Track");
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f)); // 초록색 버튼

    /*타입별 트랙 추가*/
    if (ImGui::Button("+ Add Position Track", ImVec2(120, 30))) {
        UIAnimTrack* pNewTrack=Create_NewTrack(UIANIMTYPE::POSITION);
        m_pCloneClip->Add_AnimTrack(pNewTrack);
    }

    ImGui::SameLine();
    if (ImGui::Button("+ Add Scale Track", ImVec2(120, 30))) {
        UIAnimTrack* pNewTrack = Create_NewTrack(UIANIMTYPE::SCALE);
        m_pCloneClip->Add_AnimTrack(pNewTrack);
    }

    ImGui::SameLine();
    if (ImGui::Button("+ Add Alpha Track", ImVec2(120, 30))) {
        UIAnimTrack* pNewTrack = Create_NewTrack(UIANIMTYPE::ALPHA);
        m_pCloneClip->Add_AnimTrack(pNewTrack);
    }

    ImGui::SameLine();
    if (ImGui::Button("+ Add UVScroll Track", ImVec2(120, 30))) {
        UIAnimTrack* pNewTrack = Create_NewTrack(UIANIMTYPE::UVSCROLL);
        m_pCloneClip->Add_AnimTrack(pNewTrack);
    }

    ImGui::SameLine();
    if (ImGui::Button("+ Add Colr Track", ImVec2(120, 30))) {
        UIAnimTrack* pNewTrack = Create_NewTrack(UIANIMTYPE::COLOR);
        m_pCloneClip->Add_AnimTrack(pNewTrack);
    }

    ImGui::SameLine();
    if (ImGui::Button("+ Add Progress Track", ImVec2(120, 30))) {
        UIAnimTrack* pNewTrack = Create_NewTrack(UIANIMTYPE::PROGRESS);
        m_pCloneClip->Add_AnimTrack(pNewTrack);
    }

    ImGui::PopStyleColor();

    // 저장 버튼 (Clone -> Original 반영)
    ImGui::Spacing();
    if (ImGui::Button("SAVE TO ORIGINAL", ImVec2(-1, 40))) {
        if (m_pCloneClip->Get_OriginalClip())
            m_pCloneClip->Get_OriginalClip()->Copy_From(m_pCloneClip);
       
        // 파일 저장 로직 호출
    }
}
UIAnimTrack* UIAnimationEditorWindow::Create_NewTrack(UIANIMTYPE eType)
{
    UIAnimTrack* pTrack = new UIAnimTrack();
    
    if (pTrack)
    {
        pTrack->m_eAnimType = eType;
        pTrack->m_vStartValue = _float3(0.f, 0.f, 0.f);
        pTrack->m_vEndValue = _float3(0.f, 0.f, 0.f);

        switch (eType)
        {
        case Engine::UIANIMTYPE::SCALE:
        {
            _float3 vScale = _float3(m_pSelectObject->Get_SizeX(), m_pSelectObject->Get_SizeY(),1.f);
            pTrack->m_vStartValue = _float3(vScale);

        }
            break;
        case Engine::UIANIMTYPE::ROTATION:
        {
            _float3 vRotation = _float3(0.f, 0.f, m_pSelectObject->Get_Rotation());
            pTrack->m_vStartValue = _float3(vRotation);

        }
            break;
        case Engine::UIANIMTYPE::POSITION:
        {
            _float3 vPosition = _float3(m_pSelectObject->Get_X(), m_pSelectObject->Get_Y(), 0.f);
            pTrack->m_vStartValue = _float3(vPosition);

        }
            break;
        case Engine::UIANIMTYPE::ALPHA:
        {
            _float3 vAlpha = _float3(m_pSelectObject->Get_Alpha(), 0.f, 0.f);
            pTrack->m_vStartValue = _float3(vAlpha);

        }
            break;
        case Engine::UIANIMTYPE::UVSCROLL:
            _float3 vSCroll = _float3(m_pSelectObject->Get_UVOffSet().x, m_pSelectObject->Get_UVOffSet().y, 0.f);
            pTrack->m_vStartValue = _float3(vSCroll);
            break;

        case Engine::UIANIMTYPE::COLOR:
            _float3 vColor = _float3(m_pSelectObject->Get_Color().x, m_pSelectObject->Get_Color().y, m_pSelectObject->Get_Color().z);
            pTrack->m_vStartValue = _float3(vColor);
            break;

        default:
            break;
        }
    }
    return pTrack;
}
_float3 UIAnimationEditorWindow::Get_CurrentValue(UIANIMTYPE eType)
{
    switch (eType)
    {
    case Engine::UIANIMTYPE::SCALE:
        return _float3(m_pSelectObject->Get_SizeX(), m_pSelectObject->Get_SizeY(), 1.f);

        break;
    case Engine::UIANIMTYPE::ROTATION:
        return _float3(0.f, 0.f, m_pSelectObject->Get_Rotation());

        break;
    case Engine::UIANIMTYPE::POSITION:
        return _float3(m_pSelectObject->Get_X(), m_pSelectObject->Get_Y(),0.f);
        break;
    case Engine::UIANIMTYPE::ALPHA:
        return _float3(m_pSelectObject->Get_Alpha(), 0.f, 0.f);
        break;
    case Engine::UIANIMTYPE::UVSCROLL:
        return _float3(m_pSelectObject->Get_UVOffSet().x, m_pSelectObject->Get_UVOffSet().y, 0.f);
        break;

    case Engine::UIANIMTYPE::COLOR:
        return _float3(m_pSelectObject->Get_Color().x, m_pSelectObject->Get_Color().y, m_pSelectObject->Get_Color().z);
        break;
    default:
        break;
    }
    return _float3();
}
void UIAnimationEditorWindow::Copy_To_Original(UIAnimClip* pCopy)
{


    CHECK_JUST_NULL(m_pCurrentClip);
    CHECK_JUST_NULL(m_pCloneClip);
    if (m_pCurrentClip == m_pCloneClip->Get_OriginalClip())
        return;

    m_pCurrentClip->Copy_From(m_pCloneClip);
    m_pSelectAnimationComponent->Set_PreviewClip(m_pCurrentClip);
    m_pSelectAnimationComponent->Update_All_ClipName();
}
string UIAnimationEditorWindow::Get_AnimTypeName(UIAnimTrack* pTrack)
{
    switch (pTrack->m_eAnimType)
    {
    case UIANIMTYPE::POSITION:
        return "Position";
        break;

    case UIANIMTYPE::ROTATION:
        return "Rotation";
        break;


    case UIANIMTYPE::SCALE:
        return "Scale";
        break;

    case UIANIMTYPE::ALPHA:
        return "Alpha";
        break;

    case UIANIMTYPE::UVSCROLL:
        return "UVScroll";
        break;

    case UIANIMTYPE::COLOR:
        return "UVScroll";
        break;
    default:
        break;
    }
    return string();
}
void UIAnimationEditorWindow::Change_Currentclip()
{
    //원본clip
    CHECK_JUST_NULL(m_pSelectAnimationComponent);
    UIAnimClip* pClip = m_pSelectAnimationComponent->Get_CurrentClip();
    CHECK_JUST_NULL(pClip);
    
    if (m_pCurrentClip)
    {
        //이전에선택되었던 클립과 현재 클립이름같으면 바뀐게아니므로 리턴
        if (m_pCurrentClip && m_pCurrentClip == pClip->Get_OriginalClip())
            return;

        if (m_pCurrentClip->Get_ClipName() == m_pCurrentClip->Get_ClipName())
            return;
    }
 

    if (m_pCloneClip)
    {
        Safe_Release(m_pCloneClip);
        m_pCloneClip = nullptr;
    }

    m_pCurrentClip = pClip;
    m_pCloneClip = m_pCurrentClip->Clone(); // 깊은 복사가 일어남
    m_pSelectAnimationComponent->Set_PreviewClip(m_pCloneClip);
   

}
UIAnimationEditorWindow* UITool::UIAnimationEditorWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    UIAnimationEditorWindow* pInstance = new UIAnimationEditorWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : UIAnimationEditorWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}


void UITool::UIAnimationEditorWindow::Free()
{
    for (auto& pClip : m_Garbage)
        Safe_Release(pClip);

    Safe_Release(m_pCloneClip);
    Safe_Release(pGameInstance);
    __super::Free();
}
