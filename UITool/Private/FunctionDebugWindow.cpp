#include "UITool_Define.h"
#include "FunctionDebugWindow.h"
#include "GameInstance.h"
#include <Shlwapi.h>
#include "UIObject.h"
#pragma comment(lib, "Shlwapi.lib")

FunctionDebugWindow::FunctionDebugWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :ImguiWindow(pDevice, pContext),
    pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(pGameInstance);
}

_uint UITool::FunctionDebugWindow::Update_Contents(_float fTimeDelta)
{

    if (ImGui::TreeNode("Default Settings"))
    {

        if (ImGui::Checkbox("ClientMode", &m_bClientDebug))
        {
            if (m_bClientDebug)
                pGameInstance->Set_EngineMode(ENGINEMODE::CLIENT);

            else
                pGameInstance->Set_EngineMode(ENGINEMODE::EDITOR);

        }


        if (ImGui::Checkbox("DrawDebug", &m_bDrawDebug))
        {
            UIComponent::Set_DrawDebug(m_bDrawDebug);

        }

        if (ImGui::Checkbox("Copy_Object_Mode", &bUseCopyObject))
        {
           // UIComponent::Set_DrawDebug(m_bDrawDebug);

        }


        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Save/Load"))
    {
        if (ImGui::Button("Save All Objects"))
        {
            SAVEEVENT event;
            event.bSaveJson = true;
            event.eType = FILETYPE::ALL;
            Select_SavePath(&event.Path);
            event.eToolType = TOOLTYPE::UI_TOOL;

            pGameInstance->Publish(event);

        }


        if (ImGui::Button("Load_LatestFile"))
        {
            LOADEVENT event;
            event.bLoadJson = true;
            event.eToolType = TOOLTYPE::UI_TOOL;
            event.eType = FILETYPE::ALL;
            Select_LoadPath(&event.Path);
            CurrentLoadFile = event.Path;


            pGameInstance->Publish(event);

        }
        ImGui::SameLine();
        ImGui::Text(CurrentLoadFile.c_str());


        ImGui::TreePop();

    }
   
    if (ImGui::TreeNode("Prefabs"))
    {
        

        if (ImGui::Button("Save Prefab"))
        {
            m_bIsOpenSavePrefabMenu = true;

           
        }
       
        if (m_bIsOpenSavePrefabMenu)
        {
            ImGui::SameLine();
            SAVEEVENT event;
            //현재 루트 목록보여주기(combo box)
            UIObject* pObj = nullptr;
            Show_All_Roots(&pObj);

            //어떤거 저장할지 선택되면.
            if (pObj)
            {
                event.bSaveJson = true;
                event.eType = FILETYPE::PREFAB;
                Select_SavePath(&event.Path, "..\\..\\Resources\\UI\\Prefabs");
                event.eToolType = TOOLTYPE::UI_TOOL;
                event.pData = (UIObject*)pObj;

                pGameInstance->Publish(event);


            }
        }

        if (ImGui::Button("Load Prefab"))
        {
           
            LOADEVENT event;
            event.bLoadJson = true;
            event.eToolType = TOOLTYPE::UI_TOOL;
            event.eType = FILETYPE::PREFAB;
            Select_LoadPath(&event.Path, "..\\..\\Resources\\UI\\Prefabs");
            CurrentLoadFile = event.Path;

            pGameInstance->Publish(event);

        }

       
        ImGui::TreePop();

    }


    
    return 0;
}


HRESULT FunctionDebugWindow::Create_UIObject(UITYPE eUIType)
{
    return E_NOTIMPL;
}

void FunctionDebugWindow::Show_All_Roots(UIObject** pOut)
{
    unordered_map<size_t, UIObject*> AllUIs = pGameInstance->Get_AllUIObjects();
    vector<string> NamesTemp;
    

    for (auto& pair : AllUIs)
    {
        string str = wstringToString(pair.second->Get_Name());
        NamesTemp.push_back(str);
    }
        
    AllUINames.clear();
    for (auto& s : NamesTemp)
    {
        AllUINames.push_back(s.c_str());
    }

    //콤보박스로 표시
    if (ImGui::BeginCombo("Objects List", AllUINames[iSelect]))
    {
        for (int i = 0; i < (int)AllUINames.size(); i++)
        {
            const bool bSelected = (iSelect == i);
            if (ImGui::Selectable(AllUINames[i], bSelected))
            {
                iSelect = i; // 값 갱신
                *pOut = pGameInstance->Find_UI_ByName(stringToWstring(AllUINames[iSelect]));
                iSelect = 0;

            }
        }
        ImGui::EndCombo();

       
    }
}


void FunctionDebugWindow::Select_SavePath(string* pOut, string StartPath)
{
  
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);

    // exePath = "C:\Project\Bin\Game.exe"
    PathRemoveFileSpec(exePath);
    // exePath = "C:\Project\Bin"

    PathAppend(exePath, stringToWstring(StartPath).c_str());

    // 파일 열기 다이얼로그
    OPENFILENAME ofn;
    TCHAR szFile[260] = { 0 };
 

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = pGameInstance->Get_Hwnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("Json Files(*.json)\0*.json\0All Files\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = exePath;  // 초기 경로
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileName(&ofn) == TRUE)
    {
        *pOut = wstringToString(szFile);

    }
}

void FunctionDebugWindow::Select_LoadPath(string* pOut, string StartPath )
{
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);

    // exePath = "C:\Project\Bin\Game.exe"
    PathRemoveFileSpec(exePath);
    // exePath = "C:\Project\Bin"

    PathAppend(exePath, stringToWstring(StartPath).c_str());

    // 파일 열기 다이얼로그
    OPENFILENAME ofn;
    TCHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = pGameInstance->Get_Hwnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("Json Files(*.json)\0*.json\0All Files\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = exePath;  // 초기 경로
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        // 파일이 선택되었을 때
        *pOut = wstringToString(szFile);


    }
}

FunctionDebugWindow* UITool::FunctionDebugWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    FunctionDebugWindow* pInstance = new FunctionDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : FunctionDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}


void UITool::FunctionDebugWindow::Free()
{
    Safe_Release(pGameInstance);
    __super::Free();
}
