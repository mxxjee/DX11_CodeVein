#include "Engine_Define.h"
#include "UIManager.h"

//#include "GameInstance.h"
#include "UIObject.h"
#include "Mouse.h"

#ifdef _DEBUG
#include "ImguiManager.h"
#endif // _DEBUG


int  UIManager::m_iZOrder = 0;


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::UIManager::UIManager()
{
}

Engine::UIManager::UIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext),m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	//Safe_AddRef(m_pGameInstance);
}

Engine::UIManager::~UIManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::UIManager::Initialize()
{
    Initialize_Global_Actions();

    //특정 창을 찾는건 이 uImanager를통해 전파한다.
    m_pGameInstance->Subscribe<UI_MasterEvent>([this](const UI_MasterEvent& event)
        {

            if (event.m_EventTarget != UI_EVENT_TARGET::SYSTEM)
                return;

            auto iter = m_mapGlobalEvents.find(event.m_ActionName);
            if (iter != m_mapGlobalEvents.end())
            {
                if (iter->second)
                    iter->second(event);
            }
        });
    
    m_pMouse = Mouse::GetInstance();
    Register_UIMode_Rules();

	return S_OK;
}
void Engine::UIManager::Initialize_Global_Actions()
{
    //특정 창 열기
    m_mapGlobalEvents["OpenWindow"] = [this](const UI_MasterEvent& event)
        {
            //매니저가 관리하는 UI 리스트에서 이름(m_Text)으로 찾기
            Push_Window(event.m_Text,event.m_bPersistent,event.m_bFlag);
        };


    m_mapGlobalEvents["CloseWindow"] = [this](const UI_MasterEvent& event)
        {
            //매니저가 관리하는 UI 리스트에서 이름(m_Text)으로 찾기
            Close_Window(event.m_Text);
        };

    m_mapGlobalEvents["ActiveUI"] = [this](const UI_MasterEvent& event)
        {
            //매니저가 관리하는 UI 리스트에서 이름(m_Text)으로 찾기
            Active_UI(event.m_bFlag,event.m_Text);
        };

}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::UIManager::Add_UIObject(UIObject* _ui, bool bReAdd)
{
    if (_ui == nullptr)
    {
        MSG_ON(L"nullptr넣지마", L"어휴");
        BREAK;
        return E_FAIL;
    }

    _wstring uiName = _ui->Get_Name();
    size_t uiNameHash = hash<wstring>{}(uiName);

    if (m_umapUIs.contains(uiNameHash))
    {
        auto iter = m_ObjIDs.find(uiName);
        ++iter->second;

        wstring NewName = uiName + to_wstring(iter->second);
        _ui->Set_Name(NewName);

    

    }
    else
        m_ObjIDs.emplace(uiName, 0);

    m_umapUIs[uiNameHash] = _ui;
    
    
    if (m_pGameInstance->Get_EngineMode() == ENGINEMODE::EDITOR)
    {
        if(!bReAdd)
            _ui->Set_ZOrder((_int)m_umapUIs.size());
    }
        

    m_UIObjs.push_back(_ui);
    
    sort(m_UIObjs.begin(), m_UIObjs.end(), [](UIObject* pA, UIObject* pB)
        {
            return pA->Get_ZOrder() < pB->Get_ZOrder();
        });
  

    Safe_AddRef(_ui);

    return S_OK;
}

void Engine::UIManager::Clear_UIManager()
{
    for (auto& UI : m_umapUIs)
    {
        Safe_Release(UI.second);
    }
    
    m_UIObjs.clear();
    m_umapUIs.clear();
}

void Engine::UIManager::Delete_DeadUI()
{

    for (auto iter = m_umapUIs.begin(); iter != m_umapUIs.end();)
    {
        if ((*iter).second->Is_Dead())
        {
            //자신과, 자식객체에선택된 개체가있는지 체크

            Safe_Release((*iter).second);
            iter = m_umapUIs.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    //vector에서도 제거해야함
    for (auto iter = m_UIObjs.begin(); iter != m_UIObjs.end();)
    {
        if ((*iter)->Is_Dead())
            iter = m_UIObjs.erase(iter);

        else
            ++iter;
    }



}

void Engine::UIManager::Rename_Object(wstring OldName,UIObject* pObj)
{
    wstring Newname = pObj->Get_Name();
    CHECK_TRUE(OldName == Newname);

    //맵에서삭제
    size_t OldHash = hash<wstring>{}(OldName);
    auto OldIter = m_umapUIs.find(OldHash);
    if (OldIter != m_umapUIs.end())
    {
        m_umapUIs.erase(OldIter);
        Safe_Release(pObj);
    }

    //벡터에서삭제
    for (auto iter = m_UIObjs.begin(); iter != m_UIObjs.end();)
    {
        if ((*iter) == pObj)
        {
            m_UIObjs.erase(iter);
            break;
        }

        else
            ++iter;
    }


    //맵에다시넣어주기
    Add_UIObject(pObj,true);

}

HRESULT Engine::UIManager::Remove_UIObject(UIObject* _ui)
{
    CHECK_NULLPTR(_ui);

    //이름이 변경되었을 수도 있으므로 실제로 객체비교를 통해 삭제한다.
    for (auto iter = m_umapUIs.begin(); iter != m_umapUIs.end(); ++iter)
    {
        if (iter->second)
        {
            if (iter->second == _ui)
            {
                Safe_Release(iter->second);
                m_umapUIs.erase(iter);
                break;
            }
        }
    }
  
    for (auto iter = m_UIObjs.begin(); iter != m_UIObjs.end();)
    {
        if ((*iter) == _ui)
        {
            iter = m_UIObjs.erase(iter);
            return S_OK;
        }

        else
            ++iter;
    }
    return E_FAIL;


}

HRESULT Engine::UIManager::Remove_UIObject(_wstring UIName)
{
    size_t uiNameHash = hash<wstring>{}(UIName);
    auto iter = m_umapUIs.find(uiNameHash);
    if (iter != m_umapUIs.end())
    {
        Safe_Release((*iter).second);
        m_umapUIs.erase(iter);
        return S_OK;
    }

    for (auto iter = m_UIObjs.begin(); iter != m_UIObjs.end();)
    {
        if ((*iter)->Get_Name()==UIName)
        {
            iter = m_UIObjs.erase(iter);
            return S_OK;
        }
    }
    return E_FAIL;
}

void Engine::UIManager::Visible_All_UI(_bool _isVisible)
{
    for (auto& UI : m_umapUIs)
    {
        UI.second->Set_Visible(_isVisible);
    }
}
/******************************************************* 컨테이너 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UIManager::Update_Priority(const _float fTimeDelta)
{
    for (auto& pPair : m_PersistentUIs)
    {
        if (pPair.second->Is_Active())
            pPair.second->Update_Priority(fTimeDelta);

    }

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UIManager::Update(const _float fTimeDelta)
{
    for (auto& pPair : m_PersistentUIs)
    {
        if (pPair.second->Is_Active())
            pPair.second->Update(fTimeDelta);

    }
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UIManager::Update_Late(const _float fTimeDelta)
{
    for (auto& pPair : m_PersistentUIs)
    {
        if (pPair.second->Is_Active())
            pPair.second->Update_Late(fTimeDelta);

    }

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::UIManager::Render(const _float fTimeDelta)
{

   /* m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_NONE);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::NODEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::BLEND);

    sort(m_PersistentVector.begin(), m_PersistentVector.end(), [&](GameObject* a, GameObject* b)
        {
            if (a && b)
            {
                return a->Get_ZOrder() < b->Get_ZOrder();
            }

            return false;
        });

    for (auto& pPersistent : m_PersistentVector)
        if(pPersistent->Is_Active())
            pPersistent->Render(fTimeDelta);*/

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
UIManager* Engine::UIManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UIManager* pInstance = new UIManager(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(), L"UIManager 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/


//////////////////////////////////////////////////////// 피킹 함수 ////////////////////////////////////////////////////////
UIObject* Engine::UIManager::Pick_UI(const POINT& _mousePos)
{

    if (!m_bIsActive)
        return nullptr;

    
    UIObject* pPickedUI = nullptr;
    _int highestZOrder = INT_MIN;

    ENGINEMODE eMode = m_pGameInstance->Get_EngineMode();
    if (eMode == ENGINEMODE::CLIENT)
        pPickedUI = Client_Picking(_mousePos,highestZOrder);

    else
        pPickedUI = Editor_Picking(_mousePos, highestZOrder);

    

    return pPickedUI;
}

void Engine::UIManager::Process_Picking(const POINT& _mousePos, _bool _bClicked)
{
    if (!m_bIsActive)
        return;


    UIObject*   pCurrentHover = Pick_UI(_mousePos);
    if (pCurrentHover||m_pDragTarget)
        m_pMouse->Set_Interaction(true);

    else
        m_pMouse->Set_Interaction(false);

   
    // 클릭 처리
    if (_bClicked && pCurrentHover)
    {
        pCurrentHover->OnClick();
        m_pDragTarget = pCurrentHover;

        if (m_pClickedUI != pCurrentHover)
        {
            if (m_pClickedUI)
            {
                m_pClickedUI->OnClickCancle();

            }

            m_pClickedUI = pCurrentHover;


        }
      
        
    }

    //드래그처리
    if (m_pGameInstance->MousePress(MOUSEKEYSTATE::LB))
    {
        

        if (m_pDragTarget)
        {
            m_pDragTarget->OnDragging();
        }
    }

    if (m_pGameInstance->MouseUp(MOUSEKEYSTATE::LB))
    {
        if (m_pDragTarget)
        {
            m_pDragTarget = nullptr;
        }
    }
    // Hover 상태 변경 처리
    if (m_pHoveredUI != pCurrentHover)
    {
        if (m_pHoveredUI)
        {
             m_pHoveredUI->OnHoverExit();
           
        }
        if (pCurrentHover)
        {
            pCurrentHover->OnHoverEnter();

        }
       
        m_pHoveredUI = pCurrentHover;
    }

    // Hover 중이면 OnHover 호출
    if (m_pHoveredUI)
        m_pHoveredUI->OnHover();


}
int Engine::UIManager::Find_RenderSequence(UIObject* pTarget)
{
    int iIdx = 0;
    for (auto& pUI: m_UIObjs)
    {
        if (pUI == pTarget)
            return iIdx;
        ++iIdx;
    }
    return iIdx;
}
void Engine::UIManager::Active_UI(bool _active, wstring _windowName)
{
    UIObject* pTarget = Find_UI_ByName(_windowName);
    if (pTarget)
    {
        //if(active==true)
        if (_active)
        {
            //꺼져이씅ㄹ때만 켜지기 명령보내기
            pTarget->Set_Active(_active);

        }

        //켜져있을때만 꺼라.
        else
        {
            pTarget->Set_Active(_active);
        }
            
       

    }
}
void Engine::UIManager::Set_Force_HoverUI(UIObject* pObj)
{
    CHECK_JUST_NULL(pObj);

    if (m_pHoveredUI != pObj && m_pHoveredUI != nullptr)
        m_pHoveredUI->OnHoverExit();

    m_pHoveredUI = pObj;
    m_pHoveredUI->OnHoverEnter();
}
UIObject* Engine::UIManager::Client_Picking(const POINT& _mousePos, int highestZOrder)
{
    _vector vRaypos, vRayDir;
    m_pMouse->Get_MouseRay(vRaypos, vRayDir);

    /*부모 z값 */
    for (auto iter = m_UIObjs.rbegin(); iter != m_UIObjs.rend(); ++iter)
    {
        UIObject* pChildPicked = (*iter)->Pick_Recursive(ENGINEMODE::CLIENT, _mousePos, vRaypos, vRayDir);

        if (pChildPicked)
            return pChildPicked; // 가장 위에 있는 걸 찾았으므로 즉시 종료!
    }
    return nullptr;



}
UIObject* Engine::UIManager::Editor_Picking(const POINT& _mousePos, int highestZOrder)
{
    UIObject* pPickedUI = nullptr;

    /*부모 z값 */
    for (auto iter = m_UIObjs.rbegin(); iter != m_UIObjs.rend(); ++iter)
    {
#ifdef _DEBUG
        if ((*iter)->Is_Lock())
            continue;
#endif // _DEBUG


        //부모우선 검사,자식은 부모가 통과되면.
        if ((*iter)->Get_Parent())
            continue;


        //자식중에서 젤 높은애를 가져온다.(역순)
        UIObject* pChildPicked = (*iter)->Pick_Recursive(ENGINEMODE::EDITOR, _mousePos);

        if (pChildPicked)
        {
            // Z-Order가 가장 높은 UI 선택 (가장 위에 그려진 UI)
            if ((*iter)->Get_ZOrder() > highestZOrder)
            {
                highestZOrder = (*iter)->Get_ZOrder();
                pPickedUI = pChildPicked;
            }

        }
    }

    return pPickedUI;

}
/******************************************************* 피킹 함수 *******************************************************/



UIObject* Engine::UIManager::Get_Top_At_WindowStack()
{
    if (m_WindowStack.empty())
        return nullptr;

    else
    {
        auto   pFindUI = m_umapUIs.find(m_WindowStack.top());
        if (pFindUI != m_umapUIs.end())
            return pFindUI->second;

    }

    return nullptr;
}

////////////////////////////////////////////////////////Debug 함수////////////////////////////////////////
#ifdef _DEBUG
void Engine::UIManager::Render_Search()
{
    //검색창
    m_HierarchyFilter.Draw("Search (inc, -exc)", ImGui::GetContentRegionAvail().x - 20.0f);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Filter usage:\n"
            "  \"\"         display all\n"
            "  \"xxx\"      display items containing \"xxx\"\n"
            "  \"xxx,yyy\"  display items containing \"xxx\" OR \"yyy\"\n"
            "  \"-xxx\"     hide items containing \"xxx\"");

    ImGui::Separator();
   
}
void Engine::UIManager::Render_Hierarchy()
{

    for (auto& pUI : m_UIObjs)
    {
        Render_Hierarchy_Recursive(pUI);
    }
    Start_Drag_ToEmpty();
}
void Engine::UIManager::Change_RenderSequence(UIObject* pDst, UIObject* pSrc)
{
    UIObject* pTmp = pDst;

    //pDst의 iterator

    int pDstIdx = Find_RenderSequence(pDst);
    int pSrcIdx = Find_RenderSequence(pSrc);

    swap(m_UIObjs[pDstIdx], m_UIObjs[pSrcIdx]);

    int pDstDepth = m_UIObjs[pDstIdx]->Get_ZOrder();

    m_UIObjs[pDstIdx]->Set_ZOrder( m_UIObjs[pSrcIdx]->Get_ZOrder());
    m_UIObjs[pSrcIdx]->Set_ZOrder(pDstDepth);

  
    

}
void Engine::UIManager::Render_Hierarchy_Recursive(UIObject* pRoot)
{
    string RootNameStr = wstringToString(pRoot->Get_Name());

    //검색중인지(검색활성화인지?)
    bool bIsFiltering = m_HierarchyFilter.IsActive();

    //검색된 이름이랑 맞는게있는지
    bool IsMatched = m_HierarchyFilter.PassFilter(RootNameStr.c_str());

    //자식중에도 있는지 확인
    bool HasChild = Has_Filtered_Child(pRoot);
    if (bIsFiltering && !IsMatched && !HasChild)
        return; // 검색 중인데 나도, 내 자식도 관련 없으면 여기서 렌더링 중단

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (bIsFiltering)
        flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (pRoot->Get_Children()->empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    if (m_pImguiSelectObject == pRoot)
        flags |= ImGuiTreeNodeFlags_Selected;

    //트리 생성
    vector<UIObject*> vecChild = *pRoot->Get_Children();

    char label[256];
    sprintf_s(label, "%s", wstringToString(pRoot->Get_Name()).c_str());
    bool bOpen = ImGui::TreeNodeEx(label, flags);


  
    if (ImGui::IsItemClicked())
        Set_SelectObject(pRoot);


    Start_DragDrop(pRoot);
    Send_DragDrop(pRoot);
 

    if (bOpen)
    {
        // 자식들을 순회하며 다시 자기 자신을 호출(트리구조)
        for (auto& pChild : vecChild)
        {
            Render_Hierarchy_Recursive(pChild);
        }
        ImGui::TreePop();
    }



}
void Engine::UIManager::Start_DragDrop(UIObject* pTarget)
{
    //드래그 앤 드롭으로 자식-부모관계를 처리한다.
    char SelectObjName[256];

    if (ImGui::BeginDragDropSource())
    {
        //SetDragPayload:현재 드래그 중인 오브젝트를 저장한다.
        ImGui::SetDragDropPayload("UI_HIERARCHY_NODE", &pTarget, sizeof(UIObject*));
        sprintf_s(SelectObjName, "%s", wstringToString(pTarget->Get_Name()).c_str());

        // 드래그 중 마우스 옆에 보일 미리보기
        {
            ImGui::BeginTooltip();
            ImGui::Text("%s", SelectObjName);
            ImGui::EndTooltip();
        }

        ImGui::EndDragDropSource();
    }

}
void Engine::UIManager::Send_DragDrop(UIObject* pTarget)
{
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UI_HIERARCHY_NODE"))
        {
            //지금 드래그하고있는 애
            UIObject* pDraggedChild = *(UIObject**)payload->Data;
            
            //Ctrl + 드래그드롭 -> 렌더순서변경
            if (ImGui::GetIO().KeyCtrl)
            {
                Change_RenderSequence(pDraggedChild, pTarget);
            }

            //그냥드래그 드롭 -> 부모자식설정
            else
            {
                // 순환 참조 막기
                if (pDraggedChild != pTarget)
                {
                    // 부모를 구하기위해 드래그를 함
                    if (pDraggedChild->Get_Parent() == nullptr)
                    {
                        if (FAILED(pTarget->Add_Child(pDraggedChild)))
                            return;
                    }

                }
            }
           

            
        }
        ImGui::EndDragDropTarget();
    }
}

void Engine::UIManager::Start_Drag_ToEmpty()
{
    //남은 모든 영역 입력받도록 처리
    ImVec2 vAvail = ImGui::GetContentRegionAvail();
    if (vAvail.y < 50.0f) vAvail.y = 50.0f; // 최소 높이 보장

    ImGui::InvisibleButton("EmptySpaceTarget", vAvail);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UI_HIERARCHY_NODE"))
        {
            UIObject* pDraggedChild = *(UIObject**)payload->Data;
            if (pDraggedChild->Get_Parent()== nullptr)
            {
                // 부모 끊기 로직 (Remove_Child 후 nullptr 설정)
                pDraggedChild->Detach_Child();
            }
        }
        ImGui::EndDragDropTarget();
    }
}

bool Engine::UIManager::Has_Filtered_Child(UIObject* pObj)
{

    wstring SearchWord = stringToWstring(m_HierarchyFilter.InputBuf);

    vector<UIObject*>* pChildren = pObj->Get_Children();
    for (auto& pUI : (*pChildren))
    {
        if (pUI->Get_Name().find(SearchWord) != wstring::npos)
            return true;
    }
    return false;
}


void Engine::UIManager::Set_SelectObject(UIObject * pObj)
{
    if (pObj == nullptr)
    {
        m_pImguiSelectObject = nullptr;
        ImguiManager::GetInstance()->Reset_All_Window();
        return;
    }
    
    //새로운애로 선택됐을경우
    if (pObj != m_pImguiSelectObject && pObj != nullptr)
    {
        //원래꺼 모드 해제
        if (m_pImguiSelectObject)
            m_pImguiSelectObject->Set_SelectState(UIObject::SELECTSTATE::NONESELECT);

        m_pImguiSelectObject = pObj;
        pObj->Set_SelectState(UIObject::SELECTSTATE::SELECT);

    }

}
#endif // _DEBUG
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void Engine::UIManager::Push_Window(wstring _windowName, bool bPersistent, bool bLockInput)
{


    //새로운탑
    UIObject* pNewTop = nullptr;
    
    if(!bPersistent)
        pNewTop = Find_UI_ByName(_windowName);

    else
        pNewTop = Find_PersistentUI_ByName(_windowName);

    //새롭게열려는 창이있을때만 수행
    if (pNewTop)
    {
        if (!m_WindowStack.empty())
            m_pCurrentWindow = m_umapUIs[m_WindowStack.top()];

        if (m_pCurrentWindow)
        {
            //기존의 top이존재한다면, 비활성화시킨다.
            m_pCurrentWindow->On_Active_By_OpenWindow(false);
            m_pCurrentWindow->Set_Active(false);

        }

        m_pCurrentWindow = pNewTop;
        m_WindowStack.push(hash<wstring>{}(_windowName));
        m_pCurrentWindow->Set_Visible(true);
        m_pCurrentWindow->Set_Active(true);

        if (bLockInput)
        {
            //움직이지않는 이벤트전송
            //창열었어요!!!!입력막아주세용
            INPUT_LOCK_EVENT Event;
            Event.bLock = true;
            m_pGameInstance->Publish(Event);

            MOUSELOCK_EVENT mouseEvent;
            mouseEvent.bLock = false;
            m_pGameInstance->Publish(mouseEvent);
        }
  
    }

}

void Engine::UIManager::Close_Window(wstring _windowName)
{
    if (m_WindowStack.empty())
    {
        // 스택이 비어있으면 메뉴 전체 닫기
        Close_All_Menu();
        return;
    }

    //현재 창 끄기

    //m_pCurrentWindow->Set_Visible(false);

    m_pCurrentWindow->On_Active_By_CloseWindow(false);
    m_pCurrentWindow->Set_Active(false);
    m_WindowStack.pop();
    if (m_WindowStack.empty())
    {
        m_pCurrentWindow = nullptr;
        return;
    }


    //스택에서 이전 창 ID를 꺼내서 다시 켜기

    size_t ePrevID = m_WindowStack.top();
    m_pCurrentWindow = m_umapUIs[ePrevID];
    m_pCurrentWindow->Set_Visible(true);
    m_pCurrentWindow->Set_Active(true);

}

void Engine::UIManager::Close_Window()
{
    if (m_pCurrentWindow)
        Close_Window(m_pCurrentWindow->Get_Name());

}

void Engine::UIManager::Close_All_Menu()
{
    m_pCurrentWindow = nullptr;
    while (!m_WindowStack.empty())
    {
        size_t pTopID = m_WindowStack.top();
        auto UIIter = m_umapUIs.find(pTopID);
        if (UIIter != m_umapUIs.end())
        {
            UIObject* pObj = UIIter->second;
            if (pObj)
            {
                pObj->On_Active_By_CloseWindow(false);
                pObj->Set_Active(false);
            }

        }

        m_WindowStack.pop();

    }
    m_pHoveredUI = nullptr;
    m_pClickedUI = nullptr;

}


void Engine::UIManager::Register_Factory(_uint eType, UIObjectInfo Info)
{
    //중복관리
    auto iter = m_Proto_UIObjectInfo.find(eType);
    if (iter != m_Proto_UIObjectInfo.end())
        return;


    m_Proto_UIObjectInfo.emplace(eType, Info);
}

void Engine::UIManager::Add_PoolObject(_uint eType, _uint Size)
{
    CHECK_JUST_NULL(m_PoolingFunc)
    //1.먼저 생성할원본타입이있는지 확인
    auto iter = m_Proto_UIObjectInfo.find(eType);

    //없으면 생성못함. 먼저 원본등록하고오세영
    if (iter == m_Proto_UIObjectInfo.end())
    {
        MSG_ON(L"생성할 원본타입을 모르겠어요!!Register_Factory함수로 추가해주세용", L"폰트 추가 실패!");
        BREAK;
        return;
    }
       

    vector<UIObject*>   pPool;
    pPool.reserve(Size);
    
    for (_uint i = 0; i < Size; ++i)
    {

        UIObject* ppObj = m_PoolingFunc(iter->second);


		if (ppObj)
		{
            ppObj->Set_Name(ppObj->Get_Name() + to_wstring(i));
            ppObj->Set_InPooling(true);
			pPool.push_back(ppObj);
		}

        
    }


    m_UIPools.emplace(eType, pPool);
}

UIObject* Engine::UIManager::Get_PoolObject(_uint eType)
{

    CHECK_NULL_RESULT(m_PoolingFunc, nullptr);

    //풀에서가져오기!
    auto iter = m_UIPools.find(eType);
    CHECK_TRUE_RESULT((iter == m_UIPools.end()), nullptr);


    
    vector<UIObject*>&   Pool = iter->second;


    //풀이비었을 경우 생성해서돌려주기!
    if (Pool.empty())
    {
        //원본타입가져오기
        UIObjectInfo* CloneInfo = Get_Proto_At_UIPool(eType);
        CHECK_TRUE_RESULT(CloneInfo == nullptr,nullptr);

        if (CloneInfo)
        {
            UIObject* ppObj = m_PoolingFunc(*CloneInfo);
            ppObj->Set_Name(ppObj->Get_Name() + to_wstring(Pool.size()));

            Pool.push_back(ppObj);

            m_UIPools.emplace(eType, Pool);
       
            Safe_AddRef(ppObj);
            return ppObj;
        }

    }

    //풀이있을경우 체크해서돌려주기
    else
    {
        for (auto& pObj : Pool)
        {
            if (pObj->Get_IsInPool())
            {
                pObj->Set_Active(true);
                pObj->Set_InPooling(false);
                Safe_AddRef(pObj);

                return pObj;
            
            }
        }


        //다돌았는데 풀에서 돌려줄게없는경우..
        //원본타입가져오기
        UIObjectInfo* ProtoInfo = Get_Proto_At_UIPool(eType);
        CHECK_TRUE_RESULT(ProtoInfo == nullptr,nullptr);

        Base* pObj = m_PoolingFunc(*ProtoInfo);
        if (pObj)
        {
            UIObject* ppObj = dynamic_cast<UIObject*>(pObj);
            ppObj->Set_Name(ppObj->Get_Name() + to_wstring(Pool.size()));
            Pool.push_back(ppObj);
           Safe_AddRef(ppObj);

            return ppObj;
        }

    }

    return nullptr;
}

void Engine::UIManager::Return_PoolObject(_uint eType, UIObject* pTarget)
{
    CHECK_TRUE(pTarget->Get_IsInPool());

    //돌아갈 uipool 구하기
    auto iter = m_UIPools.find(eType);
    if (iter == m_UIPools.end())
        return;

    else
    {
      
        pTarget->Set_InPooling(true);
        pTarget->Set_Active(false);

        

    }
}

UIObjectInfo* Engine::UIManager::Get_Proto_At_UIPool(_uint eType)
{
    auto iter = m_Proto_UIObjectInfo.find(eType);

    //없으면 생성못함. 먼저 원본등록하고오세영
    if (iter == m_Proto_UIObjectInfo.end())
    {
        MSG_ON(L"생성할 원본타입을 모르겠어요!!Register_Factory함수로 추가해주세용", L"폰트 추가 실패!");
        BREAK;
        return nullptr;
    }

    else
        return &iter->second;

}

HRESULT Engine::UIManager::Register_PersistentUI(UIObject* _ui)
{
    if (_ui == nullptr)
    {
        MSG_ON(L"nullptr넣지마", L"어휴");
        BREAK;
        return E_FAIL;
    }

    _wstring uiName = _ui->Get_Name();
    size_t uiNameHash = hash<wstring>{}(uiName);

    if (m_PersistentUIs.contains((_uint)uiNameHash))
    {
        auto iter = m_ObjIDs.find(uiName);
        ++iter->second;

        wstring NewName = uiName + to_wstring(iter->second);
        _ui->Set_Name(NewName);



    }
    else
        m_ObjIDs.emplace(uiName, 0);

    m_PersistentUIs[(_uint)uiNameHash] = _ui;
    m_PersistentVector.push_back(_ui);//순회용
    m_UIObjs.push_back(_ui);     //픽킹검사용
    _ui->Set_Persistent(true);

    //전체관리 map에도 넣어준다.

    m_umapUIs[uiNameHash] = _ui;
    Safe_AddRef(_ui);

    sort(m_UIObjs.begin(), m_UIObjs.end(), [](UIObject* pA, UIObject* pB)
        {
            return pA->Get_ZOrder() < pB->Get_ZOrder();
        });


    sort(m_PersistentVector.begin(), m_PersistentVector.end(), [](UIObject* pA, UIObject* pB)
        {
            return pA->Get_ZOrder() < pB->Get_ZOrder();
        });

   

    return S_OK;
}

inline UIObject* Engine::UIManager::Find_PersistentUI_ByName(const _wstring& _uiName)
{
    size_t UiNameHash = hash<wstring>{}(_uiName);

    auto iter = m_PersistentUIs.find((_uint)UiNameHash);
    if (iter == m_PersistentUIs.end())
    {
        for (auto& pair : m_PersistentUIs)
        {
            if (pair.second)
            {
                UIObject* pChild = pair.second->Get_Child(_uiName);
                if (pChild)
                    return pChild;
            }
        }

        return nullptr;
    }
    return iter->second;
}

void Engine::UIManager::Register_UIMode_Rules()
{
    //UI_RULE 
    // bool m_bIsActive = false;
    //bool m_bUseAnim = false;
    


    //Inventory
    m_mapModeRules[UI_MODE::INVENTORY][L"ShortCut_Menu"] = UI_RULE(false,true);
    m_mapModeRules[UI_MODE::INVENTORY][L"Player_QuickSlot1"] = UI_RULE(false,false);
    m_mapModeRules[UI_MODE::INVENTORY][L"Player_QuickSlot2"] = UI_RULE(false,false);
    m_mapModeRules[UI_MODE::INVENTORY][L"Minimap_Center"] = UI_RULE(false,false);
    m_mapModeRules[UI_MODE::INVENTORY][L"ShortCut_Menu"] = UI_RULE(false,true);
    m_mapModeRules[UI_MODE::INVENTORY][L"ManaInfo"] = UI_RULE(false, false);
    m_mapModeRules[UI_MODE::INVENTORY][L"Haze"] = UI_RULE(false, false);

    //default
    m_mapModeRules[UI_MODE::DEFAULT][L"PlayerHud_Left"] = UI_RULE(true,false);
    m_mapModeRules[UI_MODE::DEFAULT][L"Player_QuickSlot1"] = UI_RULE(true,false);
    m_mapModeRules[UI_MODE::DEFAULT][L"Player_QuickSlot2"] = UI_RULE(true,false);
    m_mapModeRules[UI_MODE::DEFAULT][L"Minimap_Center"] = UI_RULE(true,false);
    m_mapModeRules[UI_MODE::DEFAULT][L"ShortCut_Menu"] = UI_RULE(true,true);
    m_mapModeRules[UI_MODE::DEFAULT][L"ManaInfo"] = UI_RULE(true, true);
    m_mapModeRules[UI_MODE::DEFAULT][L"Haze"] = UI_RULE(true, false);



    //SAVEPOINTMENU(메뉴열떄 뭔가 이상하면 이거체크)
   // m_mapModeRules[UI_MODE::SAVEPOINTMENU][L"PlayerHud_Left"] = UI_RULE(,false);
    m_mapModeRules[UI_MODE::SAVEPOINTMENU][L"Player_QuickSlot1"] = UI_RULE(false,false);
    m_mapModeRules[UI_MODE::SAVEPOINTMENU][L"Player_QuickSlot2"] = UI_RULE(false,false);
    m_mapModeRules[UI_MODE::SAVEPOINTMENU][L"Minimap_Center"] = UI_RULE(false, false);
    m_mapModeRules[UI_MODE::SAVEPOINTMENU][L"ShortCut_Menu"] = UI_RULE(false, true);
    m_mapModeRules[UI_MODE::SAVEPOINTMENU][L"ManaInfo"] = UI_RULE(false, false);
    m_mapModeRules[UI_MODE::SAVEPOINTMENU][L"Haze"] = UI_RULE(false, false);



    //BASE
    m_mapModeRules[UI_MODE::BASE][L"PlayerHud_Left"] = UI_RULE(false, false);
    m_mapModeRules[UI_MODE::BASE][L"Player_QuickSlot1"] = UI_RULE(false, false);
    m_mapModeRules[UI_MODE::BASE][L"Player_QuickSlot2"] = UI_RULE(false, false);
    m_mapModeRules[UI_MODE::BASE][L"Minimap_Center"] = UI_RULE(false, false);
    m_mapModeRules[UI_MODE::BASE][L"ShortCut_Menu"] = UI_RULE(false, true);
    m_mapModeRules[UI_MODE::BASE][L"ManaInfo"] = UI_RULE(false, false);
    m_mapModeRules[UI_MODE::BASE][L"Haze"] = UI_RULE(true, false);

}

void Engine::UIManager::Change_UIMode(UI_MODE eMode, bool bUseActiveAnim)
{
    CHECK_TRUE(eMode == UI_MODE::END);

    auto& rules = m_mapModeRules[eMode];

    for (auto& pair : rules) {
        wstring strGroupKey = pair.first;
        size_t Hash = hash<wstring>{}(strGroupKey);

        UI_RULE Rule = pair.second;


        if (m_umapUIs.find(Hash)!=m_umapUIs.end()) {
            if(!Rule.m_bUseAnim)
                m_umapUIs[Hash]->Set_Active(Rule.m_bIsActive,Rule.m_bUseAnim);

            else
                m_umapUIs[Hash]->Set_Active(Rule.m_bIsActive);
        }
    }
}

void Engine::UIManager::Clear_Scene_UI()
{
     for (auto iter = m_umapUIs.begin(); iter != m_umapUIs.end(); )
    {
        if (iter->second && !iter->second->Get_IsPersistent())
        {
            Safe_Release(iter->second);
            iter->second->Release_Resources();
            iter = m_umapUIs.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    // 2. vector(m_UIObjs)도 동일하게 정리
    for (auto iter = m_UIObjs.begin(); iter != m_UIObjs.end(); )
    {
        if (*iter && !(*iter)->Get_IsPersistent())
        {
            iter = m_UIObjs.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}


UIObject* Engine::UIManager::Find_UI_ByName(const _wstring& _uiName)
{
    
	size_t UiNameHash = hash<wstring>{}(_uiName);

	auto iter = m_umapUIs.find(UiNameHash);
	if (iter == m_umapUIs.end())
	{
		for (auto& pair : m_umapUIs)
		{
            if (pair.second)
            {
                UIObject* pChild = pair.second->Get_Child(_uiName);
                if (pChild) 
                    return pChild;
            }
		}

        return nullptr;
	}
	return iter->second;
    
}



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::UIManager::Free()
{
    __super::Free();

    for (auto& UI : m_umapUIs)
    {
        Safe_Release(UI.second);
    }

    for (auto& Persistent : m_PersistentUIs)
    {
        Safe_Release(Persistent.second);
    }


    for (auto& pPair : m_UIPools)
    {
        vector<UIObject*> vVector = pPair.second;
        for (auto& pObj:vVector)
        {
            Safe_Release(pObj);
        }

        vVector.clear();
    }


    m_PoolingFunc = nullptr;

    m_UIPools.clear();
    m_Proto_UIObjectInfo.clear();

    m_umapUIs.clear();
    m_UIObjs.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    //Safe_Release(m_pGameInstance);
}

/******************************************************* 객체 반환 함수 *******************************************************/
