#include "AnimationTool_Define.h"
#include "Window_AnimEditor.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Animation.h"
#include "ImguiManager.h"
#include "Window_AnimList.h"
#include "Bone.h"
#include "Player_Weapon.h"
#include "Player_BloodWeapon.h"
#include "Yakumo.h"
#include "Yakumo_Weapon.h"

#include "Monster.h"
#include "Monster_Weapon.h"

inline _bool IsEqual_VariantValue(const VariantValue& lhs, const VariantValue& rhs)
{
    if (lhs.index() != rhs.index())
        return false;

    return std::visit([](const auto& a, const auto& b) -> _bool
        {
            using A = std::decay_t<decltype(a)>;
            using B = std::decay_t<decltype(b)>;

            if constexpr (!std::is_same_v<A, B>)
            {
                return false;
            }
            else if constexpr (std::is_same_v<A, _float2>)
            {
                return (a.x == b.x) && (a.y == b.y);
            }
            else if constexpr (std::is_same_v<A, _float3>)
            {
                return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
            }
            else if constexpr (std::is_same_v<A, _float4>)
            {
                return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
            }
            else
            {
                return a == b;
            }
        }, lhs, rhs);
}

inline _bool IsEqual_EventMap(
    const UMAP<_string, VariantValue>& lhs,
    const UMAP<_string, VariantValue>& rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    for (const auto& [key, value] : lhs)
    {
        auto it = rhs.find(key);
        if (it == rhs.end())
            return false;
        if (!IsEqual_VariantValue(value, it->second))
            return false;
    }

    return true;
}

Window_AnimEditor::Window_AnimEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ImguiWindow(pDevice, pContext)
{
}

HRESULT Window_AnimEditor::Initialize(void* pArg)
{
	m_pGameInstance = GameInstance::GetInstance();

	__super::Initialize(pArg);

	return S_OK;
}

_uint Window_AnimEditor::Update_Contents(_float fTimeDelta)
{
    if (!ImGui::BeginTabBar("AnimTool"))
        return 0;

    _bool bSucceed = Update_AnimToolData();


    if (ImGui::BeginTabItem("Play_Animation"))
    {
        if(bSucceed==true)
        Draw_PlaybackControl();

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Create_Notify"))
    {
        if (bSucceed == true)
        Draw_CreateNotify(m_pAnimToolData->pModelCom, m_pAnimToolData->pSelectedAnimation);

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Edit_Notify"))
    {
        if (bSucceed == true)
        Draw_EditNotify(m_pAnimToolData->pModelCom, m_pAnimToolData->pSelectedAnimation);

        ImGui::EndTabItem();
    }


    if (ImGui::BeginTabItem("Save"))
    {
        Save_Animation();
        ImGui::EndTabItem();
    }

    if (m_pGameInstance->KeyDown(DIK_SPACE))
    {
         m_pAnimToolData->bIsAnimPlaying = !m_pAnimToolData->bIsAnimPlaying;
    }

    ImGui::EndTabBar();

    return 0;
}

void Window_AnimEditor::Draw_PlaybackControl()
{
    ImGui::SeparatorText("Playback"); //재생하다

    ImGui::Checkbox("AnimLoop", &m_pAnimToolData->bIsLoop);
    //ImGui::SameLine();
    ImGui::SliderFloat("LerpDuration", &m_pAnimToolData->fLerpDuration, 0.0f, 1.0f);
    //ImGui::SameLine();
    ImGui::SliderFloat("AnimSpeed", &m_pAnimToolData->fAnimationSpeed, 0.1f, 2.0f);
    //ImGui::SameLine();

    if (ImGui::Button("Play_Animation"))
    {
        m_pAnimToolData->bIsAnimPlaying = true;
    }
    if (ImGui::Button("Stop_Animation"))
    {
        m_pAnimToolData->bIsAnimPlaying = false;
    }

    if (ImGui::Button("Set_Animation"))
    {
        m_pAnimToolData->pModelCom->Set_Animation_CS(m_pAnimToolData->m_iAnimationIndex, m_pAnimToolData->bIsLoop, m_pAnimToolData->fLerpDuration, m_pAnimToolData->fAnimationSpeed);
    }

}

void Window_AnimEditor::Draw_CreateNotify(Model* pModel, Animation* pAnimation)
{
    if (pModel == nullptr || pAnimation == nullptr)
        return;

    if (!m_bIntializeNotify)
    {
        m_NotifyDescForAdd = {};
        m_NotifyDescForAdd.eNotify_Event = ANIM_EVENT_TYPE::END;
        m_NotifyDescForAdd.eNotify_Type = ANIM_NOTIFY_TYPE::NOTIFY;
        m_NotifyDescForAdd.fFrame = 0.f;
        m_NotifyDescForAdd.fStartFrame = 0.f;
        m_NotifyDescForAdd.fEndFrame = 0.f;
        m_NotifyDescForAdd.SocketName.clear();
        m_NotifyDescForAdd.bAttached = false;
        m_bIntializeNotify = true;
    }

    ImGui::SeparatorText("Create Notify");

    //\0은 문자열 구분 + \0\0은 문자열 끝 / 노티파이 타입 및 이벤트 추가되면 여기에도 추가해줘야함! 
    ImGui::Combo("NotifyType", (_int*)&m_NotifyDescForAdd.eNotify_Type, "NOTIFY\0NOTIFY_STATE\0\0");
    ImGui::Combo("EventType", (_int*)&m_NotifyDescForAdd.eNotify_Event, "PLAY_SOUND\0SPAWN_PARTICLE\0ACTIVE_COLLIDER\0CAMERA\0SPAWN_TRAIL\0PLAYER_ANIM\0MONSTER_ANIM\0DISSOLVE_EVENT\0\0");

    if (m_NotifyDescForAdd.eNotify_Type == ANIM_NOTIFY_TYPE::NOTIFY)
    {
        ImGui::InputFloat("Frame", &m_NotifyDescForAdd.fFrame, 1.f, 10.f, "%.4f");
    }
    else
    {
        ImGui::InputFloat("StartFrame", &m_NotifyDescForAdd.fStartFrame, 1.f, 10.f, "%.3f");
        ImGui::InputFloat("EndFrame", &m_NotifyDescForAdd.fEndFrame, 1.f, 10.f, "%.3f");
    }
    
    _bool bPrevAttached = m_NotifyDescForAdd.bAttached;
    if (!bPrevAttached && m_NotifyDescForAdd.bAttached) //이전 장착여부가 false이고 이번 생성시점에 장착 여부를 true로 바꿨따면 동기화여부 false로해서 초기화가능하게끔
        m_bSocketBufSync = false;

    ImGui::Checkbox("Weapon Attached", &m_NotifyDescForAdd.bAttached);
    if (!m_NotifyDescForAdd.bAttached)
    {
        Draw_BonePicker(pModel, m_NotifyDescForAdd.SocketName);
    }
    else
    {
        Player* pPlayer = DCAST(Player*)(m_pAnimToolData->pSelectedGameObject);
        Yakumo* pYakumo = DCAST(Yakumo*)(m_pAnimToolData->pSelectedGameObject);
        if (pPlayer)
        {
            m_pPlayerWeapon = pPlayer->Get_ActivePlayerWeapon();

            if (m_pPlayerWeapon && m_pPlayerWeapon->Get_Model())
            {
                Draw_BonePicker(m_pPlayerWeapon->Get_Model(), m_NotifyDescForAdd.SocketName);
            }
        }
        else if (pYakumo)
        {
            m_pYakumoWeapon = pYakumo->Get_ActivePlayerWeapon();
            {
                if (m_pYakumoWeapon && m_pYakumoWeapon->Get_Model())
                {
                    Draw_BonePicker(m_pYakumoWeapon->Get_Model(), m_NotifyDescForAdd.SocketName);
                }
            }
        }
        else
        {
            Monster* pMonster = DCAST(Monster*)(m_pAnimToolData->pSelectedGameObject);
            if (pMonster)
            {
                m_pMonsterWeapon = pMonster->Get_ActiveWeapon();
                if (m_pMonsterWeapon && m_pMonsterWeapon->Get_Model())
                {
                    Draw_BonePicker(m_pMonsterWeapon->Get_Model(), m_NotifyDescForAdd.SocketName);
                }
            }
        }
    }
    Event_Edit(m_NotifyDescForAdd); //이벤트 편집


    if (ImGui::Button("Add Notify"))
    {
        pAnimation->Add_Notify(m_NotifyDescForAdd);

        m_NotifyDescForAdd = {};
        m_bIntializeNotify = false;

        m_bSocketBufSync = false;     // 버퍼 동기화 리셋
        m_szSocketFillter[0] = '\0'; //검색창도 초기화
    }

}

void Window_AnimEditor::Draw_EditNotify(Model* pModel, Animation* pAnimation)
{
    //선택한 노티파이 수정용

    if (pModel == nullptr || pAnimation == nullptr)
        return;

    vector<ANIMNOTIFY_DESC>& Notifies = m_pAnimToolData->pSelectedAnimation->Get_Notifies();
    
    //등록되어있는 노티파이 리스트 띄우기 
    if (ImGui::BeginListBox("##NotifyList", ImVec2(-1, 120))) //가로세로 창 길이 -1은 전체를 채운다.
    {
        for (size_t i = 0; i < Notifies.size(); ++i)
        {
            _bool bIsSelectIndex = (m_pAnimToolData->m_iSelectedNotifyId == Notifies[i].iNotifyId); //노티파이 선택했는지

            char label[256];
            if (Notifies[i].eNotify_Type == ANIM_NOTIFY_TYPE::NOTIFY)
            {
                snprintf(label, sizeof(label),
                    "ID:%u  [OneShot]  Frame:%.1f  Event:%d",
                    Notifies[i].iNotifyId, Notifies[i].fFrame, (int)Notifies[i].eNotify_Event);
            }
            else
            {
                snprintf(label, sizeof(label),
                    "ID:%u  [State]  %.1f~%.1f  Event:%d",
                    Notifies[i].iNotifyId, Notifies[i].fStartFrame, Notifies[i].fEndFrame, (int)Notifies[i].eNotify_Event);
            }

            if (ImGui::Selectable(label, bIsSelectIndex))
            {
                m_pAnimToolData->m_iSelectedNotifyId = Notifies[i].iNotifyId;
                m_bSocketBufSync = false;
            }

            if (bIsSelectIndex)
                ImGui::SetItemDefaultFocus(); //선택된 항목 포커스 지정
        }

        ImGui::EndListBox(); // 리스트 박스 종료 (상태 처치 마무리 함수)
    }


    if (m_pAnimToolData->m_iSelectedNotifyId == g_INVALID) { ImGui::TextDisabled("Select Notify"); return; } //선택한 노티파이 없으면 선택해야함

    ANIMNOTIFY_DESC* pSelectedNotify = m_pAnimToolData->pSelectedAnimation->Find_Notify(m_pAnimToolData->m_iSelectedNotifyId);
    if (pSelectedNotify == nullptr)
    {
        ImGui::TextDisabled("No Notify Selected");
        return;
    }

    ImGui::SeparatorText("Edit Notify");
    ImGui::Text("Id : %u", pSelectedNotify->iNotifyId);

    //\0은 문자열 구분 + \0\0은 문자열 끝 / 노티파이 타입 및 이벤트 추가되면 여기에도 추가해줘야함! 
    ImGui::Combo("NotifyType", (_int*)&pSelectedNotify->eNotify_Type, "NOTIFY\0NOTIFY_STATE\0\0");
    ImGui::Combo("EventType", (_int*)&pSelectedNotify->eNotify_Event, "PLAY_SOUND\0SPAWN_PARTICLE\0ACTIVE_COLLIDER\0CAMERA\0SPAWN_TRAIL\0PLAYER_ANIM\0MONSTER_ANIM\0DISSOLVE_EVENT\0\0");
    if (pSelectedNotify->eNotify_Type == ANIM_NOTIFY_TYPE::NOTIFY) //단발
    {
        ImGui::InputFloat("Frame(Tick)", &pSelectedNotify->fFrame);
    }
    else //지속
    {
        ImGui::InputFloat("StartFrame(Tick)", &pSelectedNotify->fStartFrame);
        ImGui::InputFloat("EndFrame(Tick)", & pSelectedNotify->fEndFrame);
    }

    ImGui::Checkbox("Attached", &pSelectedNotify->bAttached);
    if (!pSelectedNotify->bAttached)
    {
        Draw_BonePicker(pModel, pSelectedNotify->SocketName);
    }
    else
    {
        Player* pPlayer = DCAST(Player*)(m_pAnimToolData->pSelectedGameObject);
        if (pPlayer)
        {
            m_pPlayerWeapon = pPlayer->Get_ActivePlayerWeapon();
            if (m_pPlayerWeapon && m_pPlayerWeapon->Get_Model())
            {
                Draw_BonePicker(m_pPlayerWeapon->Get_Model(), pSelectedNotify->SocketName);
            }
        }
        else
        {
            Monster* pMonster = DCAST(Monster*)(m_pAnimToolData->pSelectedGameObject);
            if (pMonster)
            {
                m_pMonsterWeapon = pMonster->Get_ActiveWeapon();
                if (m_pMonsterWeapon && m_pMonsterWeapon->Get_Model())
                {
                    Draw_BonePicker(m_pMonsterWeapon->Get_Model(), pSelectedNotify->SocketName);
                }
            }
        }
    }

    Event_Edit(*pSelectedNotify); //이벤트 편집

    if (ImGui::Button("Remove Notify"))
    {
        pAnimation->Remove_Notify(pSelectedNotify->iNotifyId);

        m_pAnimToolData->m_iSelectedNotifyId = g_INVALID;
    }



}

_bool Window_AnimEditor::Update_AnimToolData()
{
    ImguiWindow* pWindow = ImguiManager::GetInstance()->Find_Window("Animation_List");
    if (pWindow == nullptr)
        return false;

    Window_AnimList* pAnimList = dynamic_cast<Window_AnimList*>(pWindow);
    if (pAnimList == nullptr)
        return false;

    AnimToolData* pData = pAnimList->Get_AnimToolData();
    if (pData == nullptr || pData->pModelCom == nullptr)
        return false;

    m_pAnimToolData = pData;

    if (m_pAnimToolData->pModelCom == nullptr)
        return false;

    if (m_PrevSelectedId != m_pAnimToolData->m_iSelectedNotifyId) //이전 노티파이와 현재 노티파이 아이디가 다르면 
    {
        m_PrevSelectedId = m_pAnimToolData->m_iSelectedNotifyId;

        m_bSocketBufSync = false;    // 버퍼 동기화 리셋
        m_szSocketFillter[0] = '\0'; //검색창도 초기화
    }

    // 승우가 26.02.20에 추가함
#ifdef _DEBUG
    // 디버그 할 본이 있으면 표시
    if (m_pAnimToolData->pModelCom->Has_DebugBone())
        m_pAnimToolData->pModelCom->Render_DebugBones(m_pAnimToolData->pSelectedGameObject->Get_WorldMatrix());

    //if (m_pPlayerWeapon == nullptr)
    //    return true;

    //if (m_pPlayerWeapon->Get_Model()->Has_DebugBone())
    //    m_pPlayerWeapon->Get_Model()->Render_DebugBones(m_pAnimToolData->pSelectedGameObject->Get_WorldMatrix());
#endif // _DEBUG

    return true;
}

void Window_AnimEditor::Edit_ModelParts()
{
    //이거 파츠오브젝트 순회해야할듯

}

HRESULT Window_AnimEditor::Save_Animation()
{
    if (ImGui::Button("SaveAnim", { 0,0 }))
    {
        _string filepath{};

        filepath = m_pGameInstance->Make_SavefilePath("Siho Files(*.sihoani)\0*.sihoani;", "sihoani");

        fstream file(filepath, ios::binary | ios::out);
        if (!file.is_open()) {
            std::cerr << "파일 생성 실패: " << filepath << std::endl;
            // 에러 이유 확인: perror() 사용
            perror("Error details");
            return E_FAIL;
        }

        const char magic[4] = { 'S', 'I', 'H', 'O' };
        file.write(magic, 4);

        MYMODEL myModel;
        myModel.mNumAnimations = 1;
        myModel.Serialize(file);

        MYANIMATION myAnim = m_pAnimToolData->pSelectedAnimation->Get_AnimationData();

        myAnim.Serialize(file);

        file.close();

        return S_OK;
    }

    return S_OK;
}

void Window_AnimEditor::BoneCache(Model* pModel)
{
    if (pModel == nullptr) return;

    if (m_PickModelBoneName.pTargetModel != pModel)  // 모델이 바뀜
    {
        m_PickModelBoneName.pTargetModel = pModel;
        m_PickModelBoneName.vecBoneNames.clear();

        auto& Bones = pModel->Get_Bones();
        m_PickModelBoneName.vecBoneNames.reserve(Bones.size());
        for (auto* Bone : Bones)
            m_PickModelBoneName.vecBoneNames.push_back(Bone->Get_BoneName());
    }
}

_bool Window_AnimEditor::Draw_BonePicker(Model* pModel, _string& InOutSocketName)
{
    if (pModel == nullptr)
        return false;

    BoneCache(pModel); //m_PickBoneName에 선택된 모델의 뼈 이름들 다 가져오고

    if (!m_bSocketBufSync) //뼈 이름 동기화 안된 상태일때 / 이거 없으면 계속 덮어 씌움
    {
        //인자로 들어온 소켓이름을 m_SocketEditBuf에 복사저장
        strncpy_s(m_szSocketEditBuf, sizeof(m_szSocketEditBuf),
            InOutSocketName.c_str(), _TRUNCATE);

        m_bSocketBufSync = true; //동기화 완료
    }

    _bool bChanged = false;

    ImGui::SeparatorText("Socket/Bone");

    if (ImGui::InputText("SocketName", m_szSocketEditBuf, IM_ARRAYSIZE(m_szSocketEditBuf), ImGuiInputTextFlags_EnterReturnsTrue)) //엔터했을때만 true로 입력변경
    {
        InOutSocketName = m_szSocketEditBuf;
        bChanged = true;
    }

    ImGui::InputTextWithHint("##BoneFilter", "Search bone keyword", m_szSocketFillter, IM_ARRAYSIZE(m_szSocketFillter)); //뼈 검색
    ImGui::SameLine();
    if (ImGui::Button("Clear")) //문자열 초기화
        m_szSocketFillter[0] = '\0';


    ImGui::BeginChild("BoneList", ImVec2(0, 220), true);

    for (size_t i = 0; i < m_PickModelBoneName.vecBoneNames.size(); ++i) //현재 모델의 뼈 개수만큼
    {

        if (m_szSocketFillter[0] != '\0')
        {
            if (m_PickModelBoneName.vecBoneNames[i].find(m_szSocketFillter) == _string::npos) //npos는 No Position 찾으려던 글자가 검색 대상 안에 없으면 건너뛴다.
                continue;
        }

        _bool bSelected = (m_PickModelBoneName.vecBoneNames[i] == InOutSocketName);
        if (ImGui::Selectable(m_PickModelBoneName.vecBoneNames[i].c_str(), bSelected)) //현재 선택한 뼈와 ImOutSocketName이 같으면 파란색으로 칠하기
        {
            if (!bSelected)
            {
                InOutSocketName = m_PickModelBoneName.vecBoneNames[i];

                // SocketName -> buf도 즉시 반영
                strncpy_s(m_szSocketEditBuf, sizeof(m_szSocketEditBuf),
                    InOutSocketName.c_str(), _TRUNCATE);

                bChanged = true;

                // 본 위치 디버그 ON
                static _int beforeboneID = -1;
                pModel->Toggle_DebugBone(beforeboneID);
                beforeboneID = pModel->Toggle_DebugBone(InOutSocketName);
            }
        }
    }

    ImGui::EndChild();

    return bChanged;
}

void AnimationTool::Window_AnimEditor::Event_Edit(ANIMNOTIFY_DESC& AnimDesc)
{
    // 이 전 프레임의 애니메이션 이벤트 데이터 저장중
    auto prevEvent = AnimDesc.UmapEvent;
    

    switch (AnimDesc.eNotify_Event)
    {
    case ANIM_EVENT_TYPE::PLAY_SOUND:
    {
        //키가 존재하지 않으면 초기화값 설정
        if (!AnimDesc.UmapEvent.count("SoundName"))     AnimDesc.UmapEvent["SoundName"] = _string("");
        if (!AnimDesc.UmapEvent.count("GroupName"))     AnimDesc.UmapEvent["GroupName"] = _string("");
        if (!AnimDesc.UmapEvent.count("Volume"))        AnimDesc.UmapEvent["Volume"] = 1.f;
        if (!AnimDesc.UmapEvent.count("Loop"))          AnimDesc.UmapEvent["Loop"] = false;
        if (!AnimDesc.UmapEvent.count("InGroup"))       AnimDesc.UmapEvent["InGroup"] = false;
        if (!AnimDesc.UmapEvent.count("RandomPlay"))    AnimDesc.UmapEvent["RandomPlay"] = false;
        if (!AnimDesc.UmapEvent.count("GroupPlay"))    AnimDesc.UmapEvent["GroupPlay"] = false;

        //variant에서 값을 가져오면 get 또는 get_If 로 접근해야함.
        string SoundName    = std::get<_string>(AnimDesc.UmapEvent["SoundName"]);
        string GroupName    = std::get<_string>(AnimDesc.UmapEvent["GroupName"]);
        float fVolume       = std::get<_float>(AnimDesc.UmapEvent["Volume"]);
        bool  bLoop         = std::get<_bool>(AnimDesc.UmapEvent["Loop"]);
        bool  bInGroup      = std::get<_bool>(AnimDesc.UmapEvent["InGroup"]);
        bool  bRandomPlay   = std::get<_bool>(AnimDesc.UmapEvent["RandomPlay"]);
        bool  bGroupPlay    = std::get<_bool>(AnimDesc.UmapEvent["GroupPlay"]);

        DrawString("SoundName", SoundName);
        DrawString("GroupName", GroupName);
        DrawFloat("Volume", fVolume);
        DrawBool("Loop", bLoop);
        DrawBool("InGroup", bInGroup);
        DrawBool("RandomPlay", bRandomPlay);
        DrawBool("GroupPlay", bGroupPlay);

        AnimDesc.UmapEvent["SoundName"] = SoundName;
        AnimDesc.UmapEvent["GroupName"] = GroupName;
        AnimDesc.UmapEvent["Volume"] = fVolume;
        AnimDesc.UmapEvent["Loop"] = bLoop;
        AnimDesc.UmapEvent["InGroup"] = bInGroup;
        AnimDesc.UmapEvent["RandomPlay"] = bRandomPlay;
        AnimDesc.UmapEvent["GroupPlay"] = bGroupPlay;

        break;
    }
    case ANIM_EVENT_TYPE::SPAWN_PARTICLE:
    {
        //키가 존재하지 않으면 초기화값 설정
        if (!AnimDesc.UmapEvent.count("ParticleSystemName")) AnimDesc.UmapEvent["ParticleSystemName"] = _string("");
        if (!AnimDesc.UmapEvent.count("Attached"))     AnimDesc.UmapEvent["Attached"] = AnimDesc.bAttached; // 동기화 옵션
        if (!AnimDesc.UmapEvent.count("SocketName"))   AnimDesc.UmapEvent["SocketName"] = AnimDesc.SocketName;
        if (!AnimDesc.UmapEvent.count("EndSocketName"))   AnimDesc.UmapEvent["EndSocketName"] = _string("");
        if (!AnimDesc.UmapEvent.count("Follow"))     AnimDesc.UmapEvent["Follow"] = false;
        if (!AnimDesc.UmapEvent.count("ParticleFollow"))     AnimDesc.UmapEvent["ParticleFollow"] = false;
        if (!AnimDesc.UmapEvent.count("UseOnlyPosition"))     AnimDesc.UmapEvent["UseOnlyPosition"] = false;
        if (!AnimDesc.UmapEvent.count("UseOwnerRotation"))     AnimDesc.UmapEvent["UseOwnerRotation"] = false;

        string ParticleSystemName = std::get<_string>(AnimDesc.UmapEvent["ParticleSystemName"]);
        string strSocketName = std::get<_string>(AnimDesc.UmapEvent["SocketName"]);
        string strEndSocketName = std::get<_string>(AnimDesc.UmapEvent["EndSocketName"]);
        _bool bAttached = std::get<_bool>(AnimDesc.UmapEvent["Attached"]);
        _bool bFollow = std::get<_bool>(AnimDesc.UmapEvent["Follow"]);
        _bool bParticleFollow = std::get<_bool>(AnimDesc.UmapEvent["ParticleFollow"]);
        _bool bOnlyPosition = std::get<_bool>(AnimDesc.UmapEvent["UseOnlyPosition"]);
        _bool bOwnerRotation = std::get<_bool>(AnimDesc.UmapEvent["UseOwnerRotation"]);

        // Combo로 선택
        _string strEffectName = std::get<_string>(AnimDesc.UmapEvent["ParticleSystemName"]);

        // 이펙트 파일 목록
        vector<_string> vecEffectFiles;
        vecEffectFiles = m_pGameInstance->Get_EffectNames();

        _int iSelectedNum = -1;
        for (_int i = 0; i < vecEffectFiles.size(); i++)
        {
            if (vecEffectFiles[i] == ParticleSystemName)
                iSelectedNum = i;
        }

        _string strComboBox = {};
        for (auto& effectName : vecEffectFiles)
            strComboBox += effectName + '\0';
        strComboBox += '\0';

        if (ImGui::Combo("ParticleSystem", &iSelectedNum, strComboBox.c_str()))
        {
            if (iSelectedNum >= 0 && iSelectedNum < vecEffectFiles.size())
                ParticleSystemName = vecEffectFiles[iSelectedNum];
        }

        ImGui::Checkbox("Follow", &bFollow);
        ImGui::Checkbox("ParticleFollow", &bParticleFollow);
        ImGui::Checkbox("Follow OnlyPosition?", &bOnlyPosition);
        ImGui::Checkbox("Use OwnerRotation?", &bOwnerRotation);
        DrawString("EndSocketName", strEndSocketName);

        AnimDesc.UmapEvent["ParticleSystemName"] = ParticleSystemName;
        AnimDesc.UmapEvent["SocketName"] = AnimDesc.SocketName;
        AnimDesc.UmapEvent["EndSocketName"] = strEndSocketName;
        AnimDesc.UmapEvent["Attached"] = AnimDesc.bAttached;
        AnimDesc.UmapEvent["Follow"] = bFollow;
        AnimDesc.UmapEvent["ParticleFollow"] = bParticleFollow;
        AnimDesc.UmapEvent["UseOnlyPosition"] = bOnlyPosition;
        AnimDesc.UmapEvent["UseOwnerRotation"] = bOwnerRotation;

        break;
    }
    case ANIM_EVENT_TYPE::ACTIVE_COLLIDER:
    {
        if (!AnimDesc.UmapEvent.count("ColGroup"))         AnimDesc.UmapEvent["ColGroup"] = (_int)COLGROUP::END;
        if (!AnimDesc.UmapEvent.count("AttackRadius"))     AnimDesc.UmapEvent["AttackRadius"] = 0.5f;
        if (!AnimDesc.UmapEvent.count("AttackDamage"))     AnimDesc.UmapEvent["AttackDamage"] = 10.f;
        if (!AnimDesc.UmapEvent.count("AttackHalfHeight")) AnimDesc.UmapEvent["AttackHalfHeight"] = 0.f;
        if (!AnimDesc.UmapEvent.count("KnockbackForce"))   AnimDesc.UmapEvent["KnockbackForce"] = 0.f;

        _int    iColGroup           = std::get<_int>  (AnimDesc.UmapEvent["ColGroup"]);
        _float  fAttackRadius       = std::get<_float>(AnimDesc.UmapEvent["AttackRadius"]);
        _float  fAttackDamage       = std::get<_float>(AnimDesc.UmapEvent["AttackDamage"]);
        _float  fAttackHalfHeight   = std::get<_float>(AnimDesc.UmapEvent["AttackHalfHeight"]);
        _float  fKnockbackForce     = std::get<_float>(AnimDesc.UmapEvent["KnockbackForce"]);

        ImGui::Combo("ColGroup", &iColGroup, "PLAYER\0PLAYER_WEAPON\0MONSTER\0MONSTER_WEAPON\0SPAWN_TRIGGER\0INTERACTION\0COL_END\0\0");
        ImGui::SliderFloat("AttackRadius", &fAttackRadius, 0.1f, 5.f);
        ImGui::InputFloat("AttackDamage", &fAttackDamage, 1.f, 10.f);
        ImGui::SliderFloat("AttackHalfHeight", &fAttackHalfHeight, 0.f, 3.f);
        ImGui::InputFloat("KnockbackForce", &fKnockbackForce, 0.1f, 1.f);

        AnimDesc.UmapEvent["ColGroup"] = iColGroup;
        AnimDesc.UmapEvent["AttackRadius"] = fAttackRadius;
        AnimDesc.UmapEvent["AttackDamage"] = fAttackDamage;
        AnimDesc.UmapEvent["AttackHalfHeight"] = fAttackHalfHeight;
        AnimDesc.UmapEvent["KnockbackForce"] = fKnockbackForce;

        break;
    }
    case ANIM_EVENT_TYPE::CAMERA:
    {
        // 카메라 액션?
        if (!AnimDesc.UmapEvent.count("Action")) AnimDesc.UmapEvent["Action"] = (_int)CAMERA_ACTION::CAMERAATCION_END;
        // 액션 이름별로 int형으로 바꾸기
        _int iAction = std::get<_int>(AnimDesc.UmapEvent["Action"]);
        ImGui::Combo("Action", &iAction, "SHAKE\0ZOOM_IN\0ZOOM_OUT\0CAMERAATCION_END\0\0");
        AnimDesc.UmapEvent["Action"] = iAction;

        // 선택된 액션이 SHAKE일 때 - Preset/Modify 모드 분기
        if (iAction == (_int)CAMERA_ACTION::SHAKE)
        {
            // 모드 선택 (0 = Preset, 1 = Modify)
            if (!AnimDesc.UmapEvent.count("ShakeMode")) AnimDesc.UmapEvent["ShakeMode"] = (_int)0;
            _int iMode = std::get<_int>(AnimDesc.UmapEvent["ShakeMode"]);
            ImGui::Combo("ShakeMode", &iMode, "Preset\0Modify\0\0");
            AnimDesc.UmapEvent["ShakeMode"] = iMode;

            ImGui::Separator();

            if (iMode == 0)
            {
                // Preset 모드
                if (!AnimDesc.UmapEvent.count("ShakePreset")) AnimDesc.UmapEvent["ShakePreset"] = (_int)0;
                _int iPreset = std::get<_int>(AnimDesc.UmapEvent["ShakePreset"]);
                ImGui::Combo("ShakePreset", &iPreset, ShakePreset::ShakePresetComboStr());
                AnimDesc.UmapEvent["ShakePreset"] = iPreset;

                // 선택된 프리셋 미리보기 (읽기 전용)
                CameraShake tPreview = ShakePreset::GetPresetShake((ShakePreset::SHAKE_PRESET)iPreset);
                ImGui::BeginDisabled();
                ImGui::Text("-- Preview --");
                ImGui::DragFloat("Duration##pv", &tPreview.fDuration);
                ImGui::DragFloat("BlendOut##pv", &tPreview.fBlendOutTime);
                ImGui::DragFloat("AmpX##pv", &tPreview.fAmpX);
                ImGui::DragFloat("AmpY##pv", &tPreview.fAmpY);
                ImGui::DragFloat("AmpZ##pv", &tPreview.fAmpZ);
                ImGui::DragFloat("AmpPitch##pv", &tPreview.fAmpPitch);
                ImGui::DragFloat("AmpYaw##pv", &tPreview.fAmpYaw);
                ImGui::DragFloat("AmpFov##pv", &tPreview.fAmpFov);
                ImGui::EndDisabled();
            }
            else
            {
                // Modify 모드 - 모든 수치 직접 편집
                // Priority
                if (!AnimDesc.UmapEvent.count("ShakePriority")) AnimDesc.UmapEvent["ShakePriority"] = (_int)SHAKE_PRIORITY::NONE;
                _int iPriority = std::get<_int>(AnimDesc.UmapEvent["ShakePriority"]);
                ImGui::Combo("ShakePriority", &iPriority, "NONE\0NORMAL_ATTACK\0STRONG_ATTACK\0EARTH_QUAKE\0\0");
                AnimDesc.UmapEvent["ShakePriority"] = iPriority;

                // Duration / BlendOut
                if (!AnimDesc.UmapEvent.count("ShakeDuration"))  AnimDesc.UmapEvent["ShakeDuration"] = 1.f;
                if (!AnimDesc.UmapEvent.count("ShakeBlendOut"))   AnimDesc.UmapEvent["ShakeBlendOut"] = 0.05f;
                _float fDuration = std::get<_float>(AnimDesc.UmapEvent["ShakeDuration"]);
                _float fBlendOut = std::get<_float>(AnimDesc.UmapEvent["ShakeBlendOut"]);
                ImGui::SliderFloat("ShakeDuration", &fDuration, 0.f, 5.f);
                ImGui::SliderFloat("ShakeBlendOut", &fBlendOut, 0.f, 1.f);
                AnimDesc.UmapEvent["ShakeDuration"] = fDuration;
                AnimDesc.UmapEvent["ShakeBlendOut"] = fBlendOut;

                // 축별 위치 진동
                ImGui::Separator();
                ImGui::Text("Position Shake");

                auto SliderPair = [&](const char* _szAmpKey, const char* _szFreqKey,
                    const char* _szAmpLabel, const char* _szFreqLabel,
                    _float _fAmpDefault, _float _fFreqDefault,
                    _float _fAmpMax, _float _fFreqMax)
                    {
                        if (!AnimDesc.UmapEvent.count(_szAmpKey))  AnimDesc.UmapEvent[_szAmpKey] = _fAmpDefault;
                        if (!AnimDesc.UmapEvent.count(_szFreqKey)) AnimDesc.UmapEvent[_szFreqKey] = _fFreqDefault;
                        _float fAmp = std::get<_float>(AnimDesc.UmapEvent[_szAmpKey]);
                        _float fFreq = std::get<_float>(AnimDesc.UmapEvent[_szFreqKey]);
                        ImGui::SliderFloat(_szAmpLabel, &fAmp, 0.f, _fAmpMax);
                        ImGui::SliderFloat(_szFreqLabel, &fFreq, 0.f, _fFreqMax);
                        AnimDesc.UmapEvent[_szAmpKey] = fAmp;
                        AnimDesc.UmapEvent[_szFreqKey] = fFreq;
                    };

                SliderPair("ShakeAmpX", "ShakeFreqX", "AmpX", "FreqX", 0.f, 40.f, 2.f, 200.f);
                SliderPair("ShakeAmpY", "ShakeFreqY", "AmpY", "FreqY", 0.f, 100.f, 2.f, 200.f);
                SliderPair("ShakeAmpZ", "ShakeFreqZ", "AmpZ", "FreqZ", 0.f, 100.f, 2.f, 200.f);

                // 회전 진동
                ImGui::Separator();
                ImGui::Text("Rotation Shake");
                SliderPair("ShakeAmpPitch", "ShakeFreqPitch", "AmpPitch", "FreqPitch", 0.f, 100.f, 0.1f, 200.f);
                SliderPair("ShakeAmpYaw", "ShakeFreqYaw", "AmpYaw", "FreqYaw", 0.f, 50.f, 0.1f, 200.f);

                // FOV 진동
                ImGui::Separator();
                ImGui::Text("FOV Shake");
                SliderPair("ShakeAmpFov", "ShakeFreqFov", "AmpFov", "FreqFov", 0.f, 60.f, 5.f, 200.f);

                // 임팩트 킥
                ImGui::Separator();
                ImGui::Text("Impact Kick");
                if (!AnimDesc.UmapEvent.count("ShakeKickStrength")) AnimDesc.UmapEvent["ShakeKickStrength"] = 0.f;
                if (!AnimDesc.UmapEvent.count("ShakeKickDecay"))    AnimDesc.UmapEvent["ShakeKickDecay"] = 15.f;
                _float fKickStr = std::get<_float>(AnimDesc.UmapEvent["ShakeKickStrength"]);
                _float fKickDecay = std::get<_float>(AnimDesc.UmapEvent["ShakeKickDecay"]);
                ImGui::SliderFloat("KickStrength", &fKickStr, 0.f, 10.f);
                ImGui::SliderFloat("KickDecay", &fKickDecay, 0.f, 50.f);
                AnimDesc.UmapEvent["ShakeKickStrength"] = fKickStr;
                AnimDesc.UmapEvent["ShakeKickDecay"] = fKickDecay;
            }
        }
        break;
    }
    case ANIM_EVENT_TYPE::SPAWN_TRAIL:
    {
        if (!AnimDesc.UmapEvent.count("TrailEffectName"))   AnimDesc.UmapEvent["TrailEffectName"] = _string("");
        if (!AnimDesc.UmapEvent.count("RootBoneName"))      AnimDesc.UmapEvent["RootBoneName"] = _string("");
        if (!AnimDesc.UmapEvent.count("TipBoneName"))       AnimDesc.UmapEvent["TipBoneName"] = _string("");
        if (!AnimDesc.UmapEvent.count("bEnable"))           AnimDesc.UmapEvent["bEnable"] = true;

        _string TrailEffectName = std::get<_string>(AnimDesc.UmapEvent["TrailEffectName"]);
        _string RootBoneName = std::get<_string>(AnimDesc.UmapEvent["RootBoneName"]);
        _string TipBoneName = std::get<_string>(AnimDesc.UmapEvent["TipBoneName"]);
        _bool bEnable = std::get<_bool>(AnimDesc.UmapEvent["bEnable"]);

        // Combo로 선택
        _string strTrailEffectName = std::get<_string>(AnimDesc.UmapEvent["TrailEffectName"]);

        // 이펙트 파일 목록
        vector<_string> vecEffectFiles;
        vecEffectFiles = m_pGameInstance->Get_EffectNames();

        _int iSelectedNum = -1;
        for (_int i = 0; i < vecEffectFiles.size(); i++)
        {
            if (vecEffectFiles[i] == TrailEffectName)
                iSelectedNum = i;
        }

        _string strComboBox = {};
        for (auto& effectName : vecEffectFiles)
            strComboBox += effectName + '\0';
        strComboBox += '\0';

        if (ImGui::Combo("TrailEffect", &iSelectedNum, strComboBox.c_str()))
        {
            if (iSelectedNum >= 0 && iSelectedNum < vecEffectFiles.size())
                TrailEffectName = vecEffectFiles[iSelectedNum];
        }

        if (ImGui::Button("Load Effect"))
        {
        }

        AnimDesc.UmapEvent["TrailEffectName"] = TrailEffectName;
        DrawString("RootBoneName", RootBoneName);
        DrawString("TipBoneName", TipBoneName);
        DrawBool("Enable", bEnable);

        AnimDesc.UmapEvent["RootBoneName"] = RootBoneName;
        AnimDesc.UmapEvent["TipBoneName"] = TipBoneName;
        AnimDesc.UmapEvent["bEnable"] = bEnable;

        break;
    }
    case ANIM_EVENT_TYPE::PLAYER_ANIM:
    {
        if (!AnimDesc.UmapEvent.count("bInputArea"))                        AnimDesc.UmapEvent["bInputArea"] = false;
        if (!AnimDesc.UmapEvent.count("bCanMove"))                          AnimDesc.UmapEvent["bCanMove"] = false;
        if (!AnimDesc.UmapEvent.count("bCanCombo"))                         AnimDesc.UmapEvent["bCanCombo"] = false;
        if (!AnimDesc.UmapEvent.count("bCanEscape"))                        AnimDesc.UmapEvent["bCanEscape"] = false;
        if (!AnimDesc.UmapEvent.count("bSuperArmor"))                       AnimDesc.UmapEvent["bSuperArmor"] = false;
        if (!AnimDesc.UmapEvent.count("fFalterResistance"))                 AnimDesc.UmapEvent["fFalterResistance"] = 0.f;
        if (!AnimDesc.UmapEvent.count("bInvincible"))                       AnimDesc.UmapEvent["bInvincible"] = false;
        if (!AnimDesc.UmapEvent.count("fStaminaCost"))                      AnimDesc.UmapEvent["fStaminaCost"] = 0.f;
        if (!AnimDesc.UmapEvent.count("bLockOnHomingRotation"))             AnimDesc.UmapEvent["bLockOnHomingRotation"] = false;
        if (!AnimDesc.UmapEvent.count("fHomingRotationSpeedRatio"))         AnimDesc.UmapEvent["fHomingRotationSpeedRatio"] = 0.f;
        if (!AnimDesc.UmapEvent.count("bBlockRegenStamina"))                AnimDesc.UmapEvent["bBlockRegenStamina"] = false;
        if (!AnimDesc.UmapEvent.count("bUseItem"))                          AnimDesc.UmapEvent["bUseItem"] = false;
        if (!AnimDesc.UmapEvent.count("bWeaponVisible"))                    AnimDesc.UmapEvent["bWeaponVisible"] = false;
        if (!AnimDesc.UmapEvent.count("bBloodWeaponVisible"))               AnimDesc.UmapEvent["bBloodWeaponVisible"] = false;
        if (!AnimDesc.UmapEvent.count("bCanParry"))                         AnimDesc.UmapEvent["bCanParry"] = false;
        if (!AnimDesc.UmapEvent.count("bCreateProjectile"))                 AnimDesc.UmapEvent["bCreateProjectile"] = false;
        if (!AnimDesc.UmapEvent.count("bChangeEquipWeapon"))                AnimDesc.UmapEvent["bChangeEquipWeapon"] = false;
        if (!AnimDesc.UmapEvent.count("fAnimationSpeed"))                   AnimDesc.UmapEvent["fAnimationSpeed"] = 1.0f;
        if (!AnimDesc.UmapEvent.count("bControlAnimSpeed"))                 AnimDesc.UmapEvent["bControlAnimSpeed"] = false;
        if (!AnimDesc.UmapEvent.count("bKetsugiStart"))                     AnimDesc.UmapEvent["bKetsugiStart"] = false;
        if (!AnimDesc.UmapEvent.count("bInjectionVisible"))                 AnimDesc.UmapEvent["bInjectionVisible"] = false;

        _bool bInputArea = std::get<_bool>(AnimDesc.UmapEvent["bInputArea"]);
        _bool bCanMove = std::get<_bool>(AnimDesc.UmapEvent["bCanMove"]);
        _bool bCanCombo = std::get<_bool>(AnimDesc.UmapEvent["bCanCombo"]);
        _bool bCanEscape = std::get<_bool>(AnimDesc.UmapEvent["bCanEscape"]);
        _bool bSuperArmor = std::get<_bool>(AnimDesc.UmapEvent["bSuperArmor"]);
        _float fFalterResistance = std::get<_float>(AnimDesc.UmapEvent["fFalterResistance"]);
        _bool bInvincible = std::get<_bool>(AnimDesc.UmapEvent["bInvincible"]);
        _float fStaminaCost = std::get<_float>(AnimDesc.UmapEvent["fStaminaCost"]);
        _bool bLockOnHomingRotation = std::get<_bool>(AnimDesc.UmapEvent["bLockOnHomingRotation"]);
        _float fHomingRotationSpeedRatio = std::get<_float>(AnimDesc.UmapEvent["fHomingRotationSpeedRatio"]);
        _bool bBlockRegenStamina = std::get<_bool>(AnimDesc.UmapEvent["bBlockRegenStamina"]);
        _bool bUseItem = std::get<_bool>(AnimDesc.UmapEvent["bUseItem"]);
        _bool bWeaponVisible = std::get<_bool>(AnimDesc.UmapEvent["bWeaponVisible"]);
        _bool bBloodWeaponVisible = std::get<_bool>(AnimDesc.UmapEvent["bBloodWeaponVisible"]);
        _bool bCanParry = std::get<_bool>(AnimDesc.UmapEvent["bCanParry"]);
        _bool bCreateProjectile = std::get<_bool>(AnimDesc.UmapEvent["bCreateProjectile"]);
        _bool bChangeEquipWeapon = std::get<_bool>(AnimDesc.UmapEvent["bChangeEquipWeapon"]);
        _float fAnimationSpeed = std::get<_float>(AnimDesc.UmapEvent["fAnimationSpeed"]);
        _bool bControlAnimSpeed = std::get<_bool>(AnimDesc.UmapEvent["bControlAnimSpeed"]);
        _bool bKetsugiStart = std::get<_bool>(AnimDesc.UmapEvent["bKetsugiStart"]);
        _bool bInjectionVisible = std::get<_bool>(AnimDesc.UmapEvent["bInjectionVisible"]);

        DrawBool("InputArea", bInputArea);
        DrawBool("CanMove", bCanMove);
        DrawBool("CanCombo", bCanCombo);
        DrawBool("CanEscape", bCanEscape);

        DrawBool("bSuperArmor", bSuperArmor);
        DrawFloat("fFalterResistance", fFalterResistance);
        DrawBool("bInvincible", bInvincible);
        DrawFloat("fStaminaCost", fStaminaCost);
        DrawBool("bLockOnHomingRotation", bLockOnHomingRotation);
        DrawFloat("fHomingRotationSpeedRatio", fHomingRotationSpeedRatio);
        DrawBool("bBlockRegenStamina", bBlockRegenStamina);
        DrawBool("bUseItem", bUseItem);

        DrawBool("bWeaponVisible", bWeaponVisible);
        DrawBool("bBloodWeaponVisible", bBloodWeaponVisible);
        DrawBool("bCanParry", bCanParry);
        DrawBool("bCreateProjectile", bCreateProjectile);
        DrawBool("bChangeEquipWeapon", bChangeEquipWeapon);
        DrawFloat("fAnimationSpeed", fAnimationSpeed);
        DrawBool("bControlAnimSpeed", bControlAnimSpeed);
        DrawBool("bKetsugiStart", bKetsugiStart);
        DrawBool("bInjectionVisible", bInjectionVisible);

        AnimDesc.UmapEvent["bInputArea"] = bInputArea;
        AnimDesc.UmapEvent["bCanMove"] = bCanMove;
        AnimDesc.UmapEvent["bCanCombo"] = bCanCombo;
        AnimDesc.UmapEvent["bCanEscape"] = bCanEscape;
        AnimDesc.UmapEvent["bSuperArmor"] = bSuperArmor;
        AnimDesc.UmapEvent["fFalterResistance"] = fFalterResistance;
        AnimDesc.UmapEvent["bInvincible"] = bInvincible;
        AnimDesc.UmapEvent["fStaminaCost"] = fStaminaCost;
        AnimDesc.UmapEvent["bLockOnHomingRotation"] = bLockOnHomingRotation;
        AnimDesc.UmapEvent["fHomingRotationSpeedRatio"] = fHomingRotationSpeedRatio;
        AnimDesc.UmapEvent["bBlockRegenStamina"] = bBlockRegenStamina;
        AnimDesc.UmapEvent["bUseItem"] = bUseItem;
        AnimDesc.UmapEvent["bWeaponVisible"] = bWeaponVisible;
        AnimDesc.UmapEvent["bBloodWeaponVisible"] = bBloodWeaponVisible;
        AnimDesc.UmapEvent["bCanParry"] = bCanParry;
        AnimDesc.UmapEvent["bCreateProjectile"] = bCreateProjectile;
        AnimDesc.UmapEvent["bChangeEquipWeapon"] = bChangeEquipWeapon;
        AnimDesc.UmapEvent["fAnimationSpeed"] = fAnimationSpeed;
        AnimDesc.UmapEvent["bControlAnimSpeed"] = bControlAnimSpeed;
        AnimDesc.UmapEvent["bKetsugiStart"] = bKetsugiStart;
        AnimDesc.UmapEvent["bInjectionVisible"] = bInjectionVisible;

        break;
    }
    case ANIM_EVENT_TYPE::MONSTER_ANIM:
    {
        // 초기값 세팅
        if (!AnimDesc.UmapEvent.count("bCancelable"))        AnimDesc.UmapEvent["bCancelable"] = true;
        if (!AnimDesc.UmapEvent.count("bChangeValue"))       AnimDesc.UmapEvent["bChangeValue"] = false;
        if (!AnimDesc.UmapEvent.count("bEnableSuperArmor"))  AnimDesc.UmapEvent["bEnableSuperArmor"] = false;
        if (!AnimDesc.UmapEvent.count("fResistance"))        AnimDesc.UmapEvent["fResistance"] = 0.f;
        if (!AnimDesc.UmapEvent.count("bDisableLookAt"))     AnimDesc.UmapEvent["bDisableLookAt"] = false;
        if (!AnimDesc.UmapEvent.count("bCacheTargetPos"))    AnimDesc.UmapEvent["bCacheTargetPos"] = false;
        if (!AnimDesc.UmapEvent.count("fMaxMoveDistance"))   AnimDesc.UmapEvent["fMaxMoveDistance"] = 0.f;
        if (!AnimDesc.UmapEvent.count("fTargetOffset"))      AnimDesc.UmapEvent["fTargetOffset"] = 0.f;
        if (!AnimDesc.UmapEvent.count("fHomingSpeedDegree")) AnimDesc.UmapEvent["fHomingSpeedDegree"] = 0.f;
        if (!AnimDesc.UmapEvent.count("bEnableSlide"))       AnimDesc.UmapEvent["bEnableSlide"] = false;
        if (!AnimDesc.UmapEvent.count("bEnableHoming"))      AnimDesc.UmapEvent["bEnableHoming"] = false;
        if (!AnimDesc.UmapEvent.count("fYawThreshold"))      AnimDesc.UmapEvent["fYawThreshold"] = 0.f;
        if (!AnimDesc.UmapEvent.count("fTurnSpeed"))         AnimDesc.UmapEvent["fTurnSpeed"] = 0.f;
        if (!AnimDesc.UmapEvent.count("fTimeLimit"))         AnimDesc.UmapEvent["fTimeLimit"] = 0.f;
        if (!AnimDesc.UmapEvent.count("iProbabilityRate"))   AnimDesc.UmapEvent["iProbabilityRate"] = (_int)0;
        if (!AnimDesc.UmapEvent.count("fComboMaxDistance"))  AnimDesc.UmapEvent["fComboMaxDistance"] = 0.f;
        if (!AnimDesc.UmapEvent.count("bComboAttack"))       AnimDesc.UmapEvent["bComboAttack"] = false;
        if (!AnimDesc.UmapEvent.count("iNextState"))         AnimDesc.UmapEvent["iNextState"] = (_int)0;
        if (!AnimDesc.UmapEvent.count("strMarkerID"))        AnimDesc.UmapEvent["strMarkerID"] = _string("");

        // 값 꺼내오기
        _bool   bCancelable = std::get<_bool>(AnimDesc.UmapEvent["bCancelable"]);
        _bool   bChangeValue = std::get<_bool>(AnimDesc.UmapEvent["bChangeValue"]);
        _bool   bEnableSuperArmor = std::get<_bool>(AnimDesc.UmapEvent["bEnableSuperArmor"]);
        _float  fResistance = std::get<_float>(AnimDesc.UmapEvent["fResistance"]);
        _bool   bDisableLookAt = std::get<_bool>(AnimDesc.UmapEvent["bDisableLookAt"]);
        _bool   bCacheTargetPos = std::get<_bool>(AnimDesc.UmapEvent["bCacheTargetPos"]);
        _float  fMaxMoveDistance = std::get<_float>(AnimDesc.UmapEvent["fMaxMoveDistance"]);
        _float  fTargetOffset = std::get<_float>(AnimDesc.UmapEvent["fTargetOffset"]);
        _float  fHomingSpeedDegree = std::get<_float>(AnimDesc.UmapEvent["fHomingSpeedDegree"]);
        _bool   bEnableSlide = std::get<_bool>(AnimDesc.UmapEvent["bEnableSlide"]);
        _bool   bEnableHoming = std::get<_bool>(AnimDesc.UmapEvent["bEnableHoming"]);
        _float  fYawThreshold = std::get<_float>(AnimDesc.UmapEvent["fYawThreshold"]);
        _float  fTurnSpeed = std::get<_float>(AnimDesc.UmapEvent["fTurnSpeed"]);
        _float  fTimeLimit = std::get<_float>(AnimDesc.UmapEvent["fTimeLimit"]);
        _int    iProbabilityRate = std::get<_int>(AnimDesc.UmapEvent["iProbabilityRate"]);
        _float  fComboMaxDistance = std::get<_float>(AnimDesc.UmapEvent["fComboMaxDistance"]);
        _bool   bComboAttack = std::get<_bool>(AnimDesc.UmapEvent["bComboAttack"]);
        _int    iNextState = std::get<_int>(AnimDesc.UmapEvent["iNextState"]);
        _string strMarkerID = std::get<_string>(AnimDesc.UmapEvent["strMarkerID"]);

        ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "[ Monster AI Notify Settings ]");

        if (ImGui::CollapsingHeader("Base & Stiff", ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawBool("Cancelable (Stiff)", bCancelable);
            DrawBool("ComboAttack", bComboAttack);
            DrawBool("ValueChange", bChangeValue);
            DrawBool("Disable LookAt", bDisableLookAt);
        }

        if (ImGui::CollapsingHeader("Super Armor")) {
            DrawBool("Enable SuperArmor", bEnableSuperArmor);
            DrawFloat("Resistance", fResistance);
        }

        if (ImGui::CollapsingHeader("Movement Control")) {
            DrawBool("Enable Slide", bEnableSlide);
            DrawFloat("Max Slide Distance", fMaxMoveDistance);
            DrawFloat("Target Offset", fTargetOffset);
            DrawBool("Enable Homing", bEnableHoming);
            DrawFloat("Homing Speed (Deg/s)", fHomingSpeedDegree);
            DrawBool("Cache Target Pos", bCacheTargetPos);
        }

        if (ImGui::CollapsingHeader("Turn & Combo")) {
            DrawFloat("Turn Yaw Threshold", fYawThreshold);
            DrawFloat("Turn Speed", fTurnSpeed);
            DrawFloat("Turn Time Limit", fTimeLimit);
            ImGui::Separator();
            DrawInt("Combo Prob (%)", iProbabilityRate);
            DrawFloat("Combo Max Dist", fComboMaxDistance);
            DrawInt("Next State Index", iNextState);
        }

        if (ImGui::CollapsingHeader("Marker")) {
            DrawString("Marker ID", strMarkerID);
        }

        AnimDesc.UmapEvent["bCancelable"] = bCancelable;
        AnimDesc.UmapEvent["bChangeValue"] = bChangeValue;
        AnimDesc.UmapEvent["bEnableSuperArmor"] = bEnableSuperArmor;
        AnimDesc.UmapEvent["fResistance"] = fResistance;
        AnimDesc.UmapEvent["bDisableLookAt"] = bDisableLookAt;
        AnimDesc.UmapEvent["bCacheTargetPos"] = bCacheTargetPos;
        AnimDesc.UmapEvent["fMaxMoveDistance"] = fMaxMoveDistance;
        AnimDesc.UmapEvent["fTargetOffset"] = fTargetOffset;
        AnimDesc.UmapEvent["fHomingSpeedDegree"] = fHomingSpeedDegree;
        AnimDesc.UmapEvent["bEnableSlide"] = bEnableSlide;
        AnimDesc.UmapEvent["bEnableHoming"] = bEnableHoming;
        AnimDesc.UmapEvent["fYawThreshold"] = fYawThreshold;
        AnimDesc.UmapEvent["fTurnSpeed"] = fTurnSpeed;
        AnimDesc.UmapEvent["fTimeLimit"] = fTimeLimit;
        AnimDesc.UmapEvent["iProbabilityRate"] = iProbabilityRate;
        AnimDesc.UmapEvent["fComboMaxDistance"] = fComboMaxDistance;
        AnimDesc.UmapEvent["bComboAttack"] = bComboAttack;
        AnimDesc.UmapEvent["iNextState"] = iNextState;
        AnimDesc.UmapEvent["strMarkerID"] = strMarkerID;

        break;
    }
    case ANIM_EVENT_TYPE::DISSOLVE_FLAG:
    {
        if (!AnimDesc.UmapEvent.count("Dissolve"))              AnimDesc.UmapEvent["Dissolve"] = false;
        if (!AnimDesc.UmapEvent.count("Weapon_Dissolve"))       AnimDesc.UmapEvent["Weapon_Dissolve"] = false;
        if (!AnimDesc.UmapEvent.count("Weapon_Target"))       AnimDesc.UmapEvent["Weapon_Target"] = false;
        if (!AnimDesc.UmapEvent.count("Dissolve Time"))        AnimDesc.UmapEvent["Dissolve Time"] = 0.f;

        _bool bDissolve = std::get<_bool>(AnimDesc.UmapEvent["Dissolve"]);
        _bool bWeapon_Dissolve = std::get<_bool>(AnimDesc.UmapEvent["Weapon_Dissolve"]);
        _bool bWeapon_Target = std::get<_bool>(AnimDesc.UmapEvent["Weapon_Target"]);
        _float fDissolveTime = std::get<_float>(AnimDesc.UmapEvent["Dissolve Time"]);

        ImGui::Checkbox("Dissolve", &bDissolve);
        ImGui::Checkbox("Weapon_Dissolve", &bWeapon_Dissolve);
        ImGui::Checkbox("Weapon_Target", &bWeapon_Target);
        DrawFloat("Dissolve Time", fDissolveTime);

        AnimDesc.UmapEvent["Dissolve"] = bDissolve;
        AnimDesc.UmapEvent["Weapon_Dissolve"] = bWeapon_Dissolve;
        AnimDesc.UmapEvent["Weapon_Target"] = bWeapon_Target;
        AnimDesc.UmapEvent["Dissolve Time"] = fDissolveTime;

        break;
    }
    };

    // 값이 변경되었을 때만 캐시 무효화
    if (!IsEqual_EventMap(prevEvent, AnimDesc.UmapEvent))
    {
        Safe_Delete(AnimDesc.pCachedEvent);
    }
}

_bool Window_AnimEditor::DrawString(const _char* Label, _string& str)
{
    char buf[256];
    strncpy_s(buf, str.c_str(), _TRUNCATE);

    bool bChanged = ImGui::InputText(Label, buf, IM_ARRAYSIZE(buf));
    if (bChanged) str = buf;
    return bChanged;
}

_bool Window_AnimEditor::DrawInt(const _char* Label, _int& iValue, _float fStep)
{
    ImGui::SetNextItemWidth(100.0f);
    return ImGui::InputInt(Label, &iValue, fStep);
}

_bool Window_AnimEditor::DrawFloat(const _char* Label, _float& fValue, _float fStep)
{
    ImGui::SetNextItemWidth(100.0f);
    return ImGui::InputFloat(Label, &fValue, fStep);
}

_bool Window_AnimEditor::DrawBool(const _char* Label, _bool& bValue)
{
    return ImGui::Checkbox(Label, &bValue);
}

Window_AnimEditor* Window_AnimEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    Window_AnimEditor* pInstance = new Window_AnimEditor(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : Window_AnimEditor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Window_AnimEditor::Free()
{
    __super::Free();

}
