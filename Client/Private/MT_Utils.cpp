#include "Client_Define.h"
#include "MT_Utils.h"
#include "GameInstance.h"

_wstring CMT_Utils::Parse_Model_Tag(const _string& fileName)
{
    vector<_string> tokens;
    stringstream ss(fileName);
    _string temp;
    while (getline(ss, temp, '_')) tokens.push_back(temp);

    _string strTag;
    strTag = "Prototype_Component_Model_" + fileName;

    _wstring wstrTag;
    wstrTag.assign(strTag.begin(), strTag.end());
    return wstrTag;
}

void CMT_Utils::Render_File_Browser(_string& currentPath, const _string& extension, std::function<void(const fs::path&)> onSelect)
{
    ImGui::Text("Path: %s", currentPath.c_str());
    if (ImGui::Button("Parent Directory [..]")) {
        fs::path p(currentPath);
        if (p.has_parent_path()) currentPath = p.parent_path().string();
    }

    ImGui::BeginChild("FileBrowserView", ImVec2(0, 200), true);
    for (const auto& entry : fs::directory_iterator(currentPath)) {
        bool isDir = entry.is_directory();
        _string name = entry.path().filename().string();

        if (!isDir && !extension.empty() && entry.path().extension() != extension) continue;

        _string label = isDir ? "[D] " + name : "[F] " + name;
        if (ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (isDir) currentPath = entry.path().string();
            else if (onSelect) onSelect(entry.path());
        }
    }
    ImGui::EndChild();
}

HRESULT CMT_Utils::Spawn_Static_Object(GameInstance* pGameInstance, _uint iLevel, const _wstring& wstrProtoTag, const OBJ_DESC& desc)
{
    if (!pGameInstance) return E_FAIL;

    return pGameInstance->Add_GameObject_To_Layer(iLevel, L"Prototype_GameObject_Static",
        iLevel, L"Layer_Object", nullptr, (void*)&desc);
}

PxMat44 CMT_Utils::ToPxMatrix(DirectX::XMMATRIX WorldMatrix)
{
    DirectX::XMFLOAT4X4 m;
    DirectX::XMStoreFloat4x4(&m, WorldMatrix);

    physx::PxMat44 pxMat;
    pxMat.column0 = physx::PxVec4(m._11, m._12, m._13, m._14);
    pxMat.column1 = physx::PxVec4(m._21, m._22, m._23, m._24);
    pxMat.column2 = physx::PxVec4(m._31, m._32, m._33, m._34);
    pxMat.column3 = physx::PxVec4(m._41, m._42, m._43, m._44);

    return pxMat;
}
