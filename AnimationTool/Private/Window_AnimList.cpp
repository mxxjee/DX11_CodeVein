#include "AnimationTool_Define.h"
#include "Window_AnimList.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Animation.h"
#include "Monster.h"

Window_AnimList::Window_AnimList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ImguiWindow(pDevice, pContext)
{
}

HRESULT Window_AnimList::Initialize(void* pArg)
{
	m_pGameInstance = GameInstance::GetInstance();

	__super::Initialize(pArg);

	return S_OK;
}

_uint Window_AnimList::Update_Contents(_float fTimeDelta)
{
    ImGui::BeginTabBar("Animation_List");

    //추후에 테이블로 무슨 모델 생성할건지에 대해 분리도 해줘야함.
    if (ImGui::BeginTabItem("Animation"))
    {
        Draw_AnimationTab(fTimeDelta);

        if (m_AnimToolData.pModelCom)
            Draw_LeftAnimListPannel();

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Select_Object"))
    {
        Draw_SelectObjectTab();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();



    return 0;
}

void Window_AnimList::Draw_AnimationTab(_float fTimeDelta)
{
    if (ImGui::Button("Create_Player"))
        Ready_Player(fTimeDelta);

    ImGui::SameLine();
    if (ImGui::Button("LookAt_Target") && m_AnimToolData.pSelectedGameObject != nullptr)
        m_pGameInstance->Camera_LookAt(m_AnimToolData.pSelectedGameObject->Get_Position());

    static _bool bHoldObject = false; 
    ImGui::SameLine();
    ImGui::Checkbox("Hold Object To 0,0,0", &bHoldObject);

    if (bHoldObject && m_AnimToolData.pSelectedGameObject)
        m_AnimToolData.pSelectedGameObject->Get_Transform()->Set_State(DIRECTION::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

    // 프로토타입 리스트
    ImGui::Checkbox("Show Prototypes", &m_bShowPrototypes);
    if (m_bShowPrototypes)
    {
        // 오브젝트 생성
        ImGui::SameLine();
        if (ImGui::Button("Create_Character"))
            Ready_Character(fTimeDelta);

        // 프로토타입 리스트 띄우기
        Draw_Prototypes();
    }
}

HRESULT Window_AnimList::Ready_Player(_float fTimeDelta)
{
    _wstring layername = L"Layer_Player";

    CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::MAIN), Proto_GameObject(L"Player"), _UINT(LEVEL::MAIN), layername), E_FAIL);

    m_AnimToolData.pSelectedGameObject = m_pGameInstance->Get_GameObject(_UINT(LEVEL::MAIN), layername, L"Player_0");

    Player* pPlayer = dynamic_cast<Player*>(m_AnimToolData.pSelectedGameObject);

    PartObject* pPartObject = pPlayer->Find_PartObject(TEXT("Part_MasterRig")); //이게 이제 애니메이션 제어하는 파트오브젝트

    Player_MasterRig* pMasterRig = dynamic_cast<Player_MasterRig*>(pPartObject);

    m_AnimToolData.pModelCom = dynamic_cast<Model*>(pMasterRig->Get_Component_FromName(Com_Model));

    //이거 파츠들 마스터본 뼈 행렬 갱신 안해주면 뼈 안붙어서 한프레임 적용해줘야함.

    _uint iNumAnimation = m_AnimToolData.pModelCom->Get_NumAnimations() - 1;

    m_AnimToolData.pModelCom->Set_Animation_CS(iNumAnimation);
    m_AnimToolData.pModelCom->Play_Animation_CS(fTimeDelta); //뼈 행렬 갱신해줘야함

    //생성했을때 애니메이션 재생 x 포즈모드 또는 Idle 만들어줘야할듯 


    return S_OK;
}

void Window_AnimList::Draw_SelectObjectTab()
{
    // 선택된 오브젝트가 없을 때 안내 표시
    if (nullptr == m_AnimToolData.pSelectedGameObject)
    {
        ImGui::TextColored(ImVec4(1.f, 0.6f, 0.2f, 1.f), "No object selected.");
        ImGui::Text("Select an object from the Animation tab first.");
        return; // 수정됨 : early return으로 depth 감소
    }

    // 현재 선택된 오브젝트 이름 표시
    const wstring& wstrName = m_AnimToolData.pSelectedGameObject->Get_Name();
    int iSize = WideCharToMultiByte(CP_UTF8, 0, wstrName.c_str(), -1, nullptr, 0, nullptr, nullptr);
    string strName(iSize - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstrName.c_str(), -1, strName.data(), iSize, nullptr, nullptr);

    ImGui::Text("Selected: %s", strName.c_str());
    ImGui::Separator();
    Select_Object_Tab();
}

HRESULT Window_AnimList::Ready_Character(_float fTimeDelta)
{
    // 기존에 있던거 지우기
    if(m_AnimToolData.pSelectedGameObject)
        m_AnimToolData.pSelectedGameObject->Set_Dead(true);

    // 레이어에 등록
    m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::MAIN), m_wstrSelectedPrototype, _UINT(LEVEL::MAIN), L"Layer_Character", RCAST(GameObject**)(&m_AnimToolData.pSelectedGameObject));

    Change_Character();

    return S_OK;
}

void Window_AnimList::Draw_LeftAnimListPannel()
{
    static _char Search[64] = "";
    ImGui::InputTextWithHint("##SearchAnim", "Search...", Search, IM_ARRAYSIZE(Search));

    ImGui::Separator();

    vector<Animation*>* vecAnimations = m_AnimToolData.pModelCom->Get_AnimationComs();

    if (ImGui::BeginListBox("##AnimList", ImVec2(-1, -1)))
    {
        for (size_t i = 0; i < vecAnimations->size(); ++i)
        {
            _string strAnimName = (*vecAnimations)[i]->Get_AnimationName(); //애니메이션 이름
            _bool bIsSelectIndex = (m_AnimToolData.m_iAnimationIndex == i); //애니메이션 선택했는지

            if (Search[0] != '\0' && strAnimName.find(Search) == _string::npos) continue; //애니메이션 이름으로 입력받아서 찾기

            if (ImGui::Selectable(strAnimName.c_str(), bIsSelectIndex))
            {
                m_AnimToolData.m_iAnimationIndex = (_uint)i;
                m_AnimToolData.pSelectedAnimation = (*vecAnimations)[i];
            }

            if (bIsSelectIndex)
                ImGui::SetItemDefaultFocus(); //선택된 항목 포커스 지정
        }

        ImGui::EndListBox(); // 리스트 박스 종료 (상태 처치 마무리 함수)
    }
}

void Window_AnimList::Draw_Prototypes()
{
    // MAIN레벨로 들어오지 않았으면 return
    if (m_pGameInstance->Get_Current_LevelID() != _UINT(LEVEL::MAIN))
        return;

    // 리스트가 없으면
    if (m_pPrototypes.empty())
    {
        // 프로토타입 리스트 가져오기
        UMAP<_wstring, class Base*>* prototypes = m_pGameInstance->Get_Prototypes();
        // 그래도 없으면 리턴
        CHECK_NULL_RESULT(prototypes, );

        // 순회하면서 게임오브젝트만 가져오기
        for (auto& [name, base] : prototypes[_UINT(LEVEL::MAIN)])
        {
            Character* gameObject = DCAST(Character*)(base);
            // 게임오브젝트이고(컴포넌트가 아니고), 파트오브젝트가 아닐경우에 등록
            if (gameObject && !gameObject->Is_PartObj())
            {
                m_pPrototypes.emplace(name, gameObject);
            }
        }
    }

    // 프로토타입 리스트 ImGui로 표시
    static char searchPrototype[256] = {};
    ImGui::InputText("##PrototypeSearch", searchPrototype, IM_ARRAYSIZE(searchPrototype));

    // ListBox로 프로토타입 리스트 표시
    if (ImGui::BeginListBox("##PrototypeList", ImVec2(-1, 200)))
    {
        for (auto& [name, base] : m_pPrototypes)
        {
            string narrowName(wstringToString(name));
            // 검색 필터링
            if (searchPrototype[0] != '\0' && narrowName.find(searchPrototype) == string::npos) continue;
            _bool bIsSelected = (m_pSelectedPrototype == base);
            if (ImGui::Selectable(narrowName.c_str(), bIsSelected))
            {
                m_pSelectedPrototype = base;
                m_wstrSelectedPrototype = m_pSelectedPrototype->Get_PrototypeName();
            }
            if (bIsSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

}

void Window_AnimList::Change_Character()
{
    // 오브젝트 타입별로 어떤 모델을 AnimToolData에 등록할지 정하기
    switch (m_AnimToolData.pSelectedGameObject->Get_OBJType())
    {
    case OBJTYPE::TYPE_CONTAINER:
    {
        // 파트오브젝트중 마스터리그를 모델로 등록
        PartObject* partObject = CAST(ContainerObject*)(m_AnimToolData.pSelectedGameObject)->Find_PartObject(L"Part_MasterRig");
        m_AnimToolData.pModelCom = partObject->Get_Model();
        break;
    }
    default:
    {
        m_AnimToolData.pModelCom = m_AnimToolData.pSelectedGameObject->Get_Model();
        break;
    }
    }

    // 초기 애니메이션 세팅
    m_AnimToolData.pModelCom->Set_Animation_CS(1, true);
    m_AnimToolData.pModelCom->Play_Animation_CS(0.f);

    m_AnimToolData.pModelCom->Set_Animation_CS(0, true);
    m_AnimToolData.pModelCom->Play_Animation_CS(0.f);
}

void Window_AnimList::Select_Object_Tab()
{
    GameObject* pObject = m_AnimToolData.pSelectedGameObject;
    if (nullptr == pObject)
        return;

    // 오브젝트에서 월드행렬 추출
    _matrix worldmat = pObject->Get_WorldMatrix();
    _vector scale = {}, rotationquat = {}, translation = {}, degree = {};
    if (XMMatrixDecompose(&scale, &rotationquat, &translation, worldmat))
    {
        degree = m_pGameInstance->QuaternionToDegrees(rotationquat);
    }

#pragma region Scale
    _float3 fScale = pObject->Get_Scale();
    _float vScale[3] = { fScale.x, fScale.y, fScale.z };

    if (Render_TransformField("Scale", "##AnimScale_Input", "##AnimScale_Drag",
        vScale, m_vCurrentScale, 0.01f, 0.01f, 10.f))
    {
        pObject->Set_Scale(m_vCurrentScale.x, m_vCurrentScale.y, m_vCurrentScale.z);
    }
#pragma endregion

#pragma region Rotation
    _float3 fDegree = {};
    XMStoreFloat3(&fDegree, degree);
    _float vDegree[3] = { fDegree.x, fDegree.y, fDegree.z };

    if (Render_TransformField("Rotation", "##AnimRotation_Input", "##AnimRotation_Drag",
        vDegree, m_vCurrentRotation, 0.05f, -360.0f, 360.f))
    {
        pObject->Rotation(
            XMConvertToRadians(m_vCurrentRotation.x),
            XMConvertToRadians(m_vCurrentRotation.y),
            XMConvertToRadians(m_vCurrentRotation.z));
    }
#pragma endregion

#pragma region Position
    _float3 worldpos = {};
    XMStoreFloat3(&worldpos, worldmat.r[3]);
    _float vPosition[3] = { worldpos.x, worldpos.y, worldpos.z };

    if (Render_TransformField("Position", "##AnimPosition_Input", "##AnimPosition_Drag",
        vPosition, m_vCurrentPos, 0.01f, -500.0f, 500.0f))
    {
        pObject->Set_State(DIRECTION::POSITION, m_vCurrentPos);
    }
#pragma endregion
}

bool Window_AnimList::Render_TransformField(const char* _label, const char* _inputID, const char* _dragID,
    _float* _values, _float4& _cached, _float _dragSpeed, _float _dragMin, _float _dragMax)
{
    // 공통 Transform 필드 렌더링
    const float labelWidth = 70.0f;
    const float inputWidth = -FLT_MIN;

    ImGui::Text(_label);
    ImGui::SameLine(labelWidth);
    ImGui::SetNextItemWidth(inputWidth);
    ImGui::InputFloat3(_inputID, _values);

    ImGui::Text(" ");
    ImGui::SameLine(labelWidth);
    ImGui::SetNextItemWidth(inputWidth);
    ImGui::DragFloat3(_dragID, _values, _dragSpeed, _dragMin, _dragMax);

    // epsilon 기반 비교로 변경 감지
    bool bChanged = (fabsf(_cached.x - _values[0]) > TRANSFORM_EPSILON ||
        fabsf(_cached.y - _values[1]) > TRANSFORM_EPSILON ||
        fabsf(_cached.z - _values[2]) > TRANSFORM_EPSILON);

    if (bChanged)
    {
        _cached.x = _values[0];
        _cached.y = _values[1];
        _cached.z = _values[2];
        _cached.w = 1.f;
    }

    return bChanged;
}

Window_AnimList* Window_AnimList::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    Window_AnimList* pInstance = new Window_AnimList(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : Window_AnimList");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Window_AnimList::Free()
{
	__super::Free();

}
