#include "MT_Defines.h"
#include "Asset_Debug_UI.h"
#include "GameInstance.h"
#include "Mouse.h"
#include "StaticObj.h"

CAsset_Debug_UI::CAsset_Debug_UI(ID3D11Device* pD, ID3D11DeviceContext* pC)
    :ImguiWindow(pD, pC)
{

}

HRESULT CAsset_Debug_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pGameInstance = GameInstance::GetInstance();
    if (nullptr != m_pGameInstance)
        Safe_AddRef(m_pGameInstance);

    m_umapLightsPtr = m_pGameInstance->Get_LightuMapPtr();
    m_strCurrentPath = "../../Resources";

    return S_OK;
}

_uint CAsset_Debug_UI::Update_Contents(_float fDT)
{
    ImGui::Text("Asset Browser");
    m_pMouse = Mouse::GetInstance();
    ImGuiIO& io = ImGui::GetIO();

    static int item_selected_idx = 0;

    if (!io.WantCaptureMouse)
    {
        if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
        {
            GameObject* pPickedObject = nullptr;

            if (m_pMouse->Picking_Object_GameObject_Pixel(pPickedObject))
            {
                CStaticObj* pPrev = dynamic_cast<CStaticObj*>(g_pSelectedObject);
                if (pPrev) pPrev->Set_Hovered(false);
                Safe_Release(g_pSelectedObject);

                g_pSelectedObject = pPickedObject;
                Safe_AddRef(g_pSelectedObject);

                CStaticObj* pNew = dynamic_cast<CStaticObj*>(g_pSelectedObject);
                if (pNew)
                {
                    pNew->Set_Hovered(true);

                    LightComponent* pLightCom = pNew->Get_LightPtr();
                    if (pLightCom != nullptr)
                    {
                        item_selected_idx = pLightCom->Get_LightIndex();
                    }
                }
            }
        }
    }

    ObjCreator();
    ObjEditor();

    return 0;
}

void CAsset_Debug_UI::ObjCreator()
{
    // 1. 오브젝트 생성
    if (ImGui::CollapsingHeader("Object Creator", ImGuiTreeNodeFlags_DefaultOpen))
    {
        CMT_Utils::Render_File_Browser(m_strCurrentPath, ".siho", [&](const fs::path& selectedFile) {
            _wstring wstrProtoTag = CMT_Utils::Parse_Model_Tag(selectedFile.stem().string());

            MapObject::MAPOBJECT_DESC desc{};
            desc.vPosition = _float4(objPos[0], objPos[1], objPos[2], 1.f);
            desc.vScale = _float3(objScale[0], objScale[1], objScale[2]);

            _vector vQuat = XMQuaternionRotationRollPitchYaw(
                XMConvertToRadians(objRot[0]), XMConvertToRadians(objRot[1]), XMConvertToRadians(objRot[2]));
            XMStoreFloat4(&desc.vRotationQuat, vQuat);

            desc.wstrShaderName = L"Prototype_Component_Shader_VTXMeshInstance";
            desc.wstrName = L"Static_" + std::to_wstring(g_iGlobalObjCount++);
            desc.prototypename = wstrProtoTag;

            m_pGameInstance->Add_GameObject_To_Layer(_uint(LEVEL::TOOL), L"Prototype_GameObject_Static",
                _uint(LEVEL::TOOL), L"Layer_Object", nullptr, &desc);
            });

        ImGui::Spacing();
        ImGui::InputFloat3("Spawn Scale", objScale);
        ImGui::InputFloat3("Spawn Position", objPos);
        ImGui::InputFloat3("Spawn Rotation", objRot);
        ImGui::Separator();
    }

    if (ImGui::CollapsingHeader("Light Creator"))
    {
        static int item_selected_idx = 0;
        static int prev_selected_idx = -1;
        static float fIntensity = 1.0f;
        static _float4 vBaseColor = { 1.f, 1.f, 1.f, 1.f };

        if (g_pSelectedObject != nullptr)
        {
            MapObject* pMapObj = dynamic_cast<MapObject*>(g_pSelectedObject);
            if (pMapObj && pMapObj->Get_LightPtr())
            {
                item_selected_idx = pMapObj->Get_LightPtr()->Get_LightIndex();
            }
        }

        auto ColorEdit4Row = [](const char* label, _float4& v) {
            ImGui::ColorEdit4(label, &v.x, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
            };

        auto DragFloat4Row = [](const char* label, _float4& v, float speed = 0.01f, float vMin = 0.f, float vMax = 0.f) {
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
        if (isize > 0)
        {
            m_DescVec.clear();
            for (int i = 0; i < isize; i++) {
                m_DescVec.push_back(m_pGameInstance->Get_LightDesc(i));
            }

            if (item_selected_idx >= isize) item_selected_idx = 0;

            _int DirectionalCnt = 0;
            _int PointCnt = 0;
            if (ImGui::BeginListBox("Light List (Sync Enabled)"))
            {
                for (int n = 0; n < isize; n++)
                {
                    const bool is_selected = (item_selected_idx == n);
                    string displayName = (m_DescVec[n]->eType == LIGHT::DIRECTIONAL)
                        ? "DIR " + to_string(DirectionalCnt++)
                        : "POINT " + to_string(PointCnt++);

                    if (g_pSelectedObject != nullptr) {
                        MapObject* pMapObj = dynamic_cast<MapObject*>(g_pSelectedObject);
                        if (pMapObj && pMapObj->Get_LightPtr() && pMapObj->Get_LightPtr()->Get_LightIndex() == n) {
                            displayName += " (*) [Owner]";
                        }
                    }

                    if (ImGui::Selectable(displayName.c_str(), is_selected))
                        item_selected_idx = n;

                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            auto& selected = *m_DescVec[item_selected_idx];

            if (prev_selected_idx != item_selected_idx)
            {
                fIntensity = max(selected.vDiffuse.x, max(selected.vDiffuse.y, selected.vDiffuse.z));
                if (fIntensity < 0.1f) fIntensity = 1.0f;
                vBaseColor = { selected.vDiffuse.x / fIntensity, selected.vDiffuse.y / fIntensity, selected.vDiffuse.z / fIntensity, 1.f };
                prev_selected_idx = item_selected_idx;
            }

            ImGui::Separator();
            ImGui::Text("Editing Index: %d (%s)", item_selected_idx, (selected.eType == LIGHT::DIRECTIONAL) ? "DIRECTIONAL" : "POINT");
            ImGui::Spacing();

            bool bChanged = false;
            // 색상 비율 수정
            if (ImGui::ColorEdit4("Diffuse Color", &vBaseColor.x, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR))
                bChanged = true;

            // 강도(밝기) 수정
            if (ImGui::DragFloat("Light Intensity", &fIntensity, 0.1f, 0.01f, 100.f))
                bChanged = true;

            if (bChanged) {
                selected.vDiffuse.x = vBaseColor.x * fIntensity;
                selected.vDiffuse.y = vBaseColor.y * fIntensity;
                selected.vDiffuse.z = vBaseColor.z * fIntensity;
            }

            ImGui::Separator();
            ColorEdit4Row("Specular", selected.vSpecular);
            ColorEdit4Row("Ambient", selected.vAmbient);

            if (selected.eType == LIGHT::DIRECTIONAL) {
                DragFloat4Row("Direction", selected.vDirection, 0.01f, -1.f, 1.f);
            }
            else if (selected.eType == LIGHT::POINT) {
                DragFloat4Row("Position", selected.vPosition);
                ImGui::PushItemWidth(150);
                ImGui::DragFloat("Range", &selected.fRange, 0.1f, 0.01f, 1000.f);
                ImGui::PopItemWidth();
            }

            m_pGameInstance->Set_LightDesc(item_selected_idx, selected);
        }
    }

    if (ImGui::CollapsingHeader("Light Add"))
    {
        AddPointLight();
    }
}


void CAsset_Debug_UI::ObjEditor()
{
    if (ImGui::CollapsingHeader("Object Editor", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (g_pSelectedObject && g_pSelectedObject->Is_Dead())
        {
            CStaticObj* pStatic = dynamic_cast<CStaticObj*>(g_pSelectedObject);
            if (pStatic) pStatic->Set_Hovered(false);
            Safe_Release(g_pSelectedObject);
            g_pSelectedObject = nullptr;
        }

        if (g_pSelectedObject != nullptr)
        {
            ImGui::PushID(g_pSelectedObject);

            static GameObject* pPrevSelectedObject = nullptr;
            if (pPrevSelectedObject != g_pSelectedObject)
            {
                XMStoreFloat3(&m_fPos, g_pSelectedObject->Get_Position());
                m_fScale = g_pSelectedObject->Get_Scale();

                _matrix matWorld = g_pSelectedObject->Get_WorldMatrix();
                _vector vS, vR, vT;
                XMMatrixDecompose(&vS, &vR, &vT, matWorld);

                _float4 q;
                XMStoreFloat4(&q, vR);

                _float3 euler;
                float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
                float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
                euler.x = atan2f(sinr_cosp, cosr_cosp);

                float sinp = 2.0f * (q.w * q.y - q.z * q.x);
                if (fabs(sinp) >= 1.0f) euler.y = copysignf(XM_PI / 2.0f, sinp);
                else euler.y = asinf(sinp);

                float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
                float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
                euler.z = atan2f(siny_cosp, cosy_cosp);

                m_fRotation.x = XMConvertToDegrees(euler.x);
                m_fRotation.y = XMConvertToDegrees(euler.y);
                m_fRotation.z = XMConvertToDegrees(euler.z);

                m_fmulScale = 1.0f;
                m_fBaseScale = g_pSelectedObject->Get_Scale();

                pPrevSelectedObject = g_pSelectedObject;
            }

            if (ImGui::DragFloat3("Edit Position", (float*)&m_fPos, 0.1f)) {
                _float4 fPosWithW = { m_fPos.x, m_fPos.y, m_fPos.z, 1.f };
                g_pSelectedObject->Set_State(DIRECTION::POSITION, XMLoadFloat4(&fPosWithW));
            }

            if (ImGui::DragFloat3("Edit Rotation", (float*)&m_fRotation, 0.5f)) {
                g_pSelectedObject->Rotation(
                    XMConvertToRadians(m_fRotation.x),
                    XMConvertToRadians(m_fRotation.y),
                    XMConvertToRadians(m_fRotation.z)
                );
            }

            bool bScaleChanged = false;
            if (ImGui::DragFloat3("Edit Scale", (float*)&m_fScale, 0.05f, 0.01f, 100.f)) bScaleChanged = true;
            if (ImGui::DragFloat("Multiply Scale", &m_fmulScale, 0.01f, 0.01f, 10.f)) {
                m_fScale.x = m_fBaseScale.x * m_fmulScale;
                m_fScale.y = m_fBaseScale.y * m_fmulScale;
                m_fScale.z = m_fBaseScale.z * m_fmulScale;
                bScaleChanged = true;
            }

            if (bScaleChanged) g_pSelectedObject->Set_Scale(m_fScale.x, m_fScale.y, m_fScale.z);

            if (ImGui::Button("Deselect", ImVec2(-1, 0))) {
                CStaticObj* pStatic = dynamic_cast<CStaticObj*>(g_pSelectedObject);
                if (pStatic) pStatic->Set_Hovered(false);
                Safe_Release(g_pSelectedObject);
                g_pSelectedObject = nullptr;
            }

            ImGui::PopID();
        }
        else {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "No Object Selected.");
        }
    }
}

void CAsset_Debug_UI::AddPointLight()
{
    if (g_pSelectedObject == nullptr)
        return;

    // 하나넣고 두개부터는 굳이? 넣을필요가없긴하네
    if (ImGui::Button("Create PointLight Component"))
    {
        LightComponent*& temp = g_pSelectedObject->Get_LightPtr();
        if (FAILED(g_pSelectedObject->Add_Component(_UINT(LEVEL::TOOL), L"Proto_Component_Point_Light",
            TEXT("Com_Light" + to_wstring(m_pGameInstance->Get_LightCnt())), reinterpret_cast<Component**>(&temp))))
            return;

        _vector Pos = g_pSelectedObject->Get_Transform()->Get_State(DIRECTION::POSITION);

        Bounding_Sphere::BOUNDSPHERE_DESC boundingdesc;
        boundingdesc.fRadius = 1.f;
        boundingdesc.vCenter = _float3(0.f, 0.f, 0.f);
        Collider*& tempColl = g_pSelectedObject->Get_ColliderPtr();
        g_pSelectedObject->Add_Collider(COLLISION_GROUP::INTERACTION, COLLIDER::SPHERE, &tempColl, &boundingdesc);

        temp->Update(0, Pos);

        MapObject* pMapObj = dynamic_cast<MapObject*>(g_pSelectedObject);
        if (pMapObj)
        {
            json jExtra = pMapObj->Get_ExtraData();

            jExtra["HasPointLight"] = true;

            LIGHT_DESC lightDesc = temp->Get_LightDesc();

            // 타입
            jExtra["PointLight"]["eType"] = (int)lightDesc.eType;

            // 색상
            jExtra["PointLight"]["Diffuse"] = { lightDesc.vDiffuse.x,  lightDesc.vDiffuse.y,  lightDesc.vDiffuse.z,  lightDesc.vDiffuse.w };
            jExtra["PointLight"]["Ambient"] = { lightDesc.vAmbient.x,  lightDesc.vAmbient.y,  lightDesc.vAmbient.z,  lightDesc.vAmbient.w };
            jExtra["PointLight"]["Specular"] = { lightDesc.vSpecular.x, lightDesc.vSpecular.y, lightDesc.vSpecular.z, lightDesc.vSpecular.w };

            // 위치, 방향, 범위
            jExtra["PointLight"]["Direction"] = { lightDesc.vDirection.x, lightDesc.vDirection.y, lightDesc.vDirection.z, lightDesc.vDirection.w };
            jExtra["PointLight"]["Position"] = { lightDesc.vPosition.x,  lightDesc.vPosition.y,  lightDesc.vPosition.z,  lightDesc.vPosition.w };
            jExtra["PointLight"]["Range"] = lightDesc.fRange;

            pMapObj->Set_ExtraData(jExtra);
        }
    }
}

CAsset_Debug_UI* CAsset_Debug_UI::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg)
{
    CAsset_Debug_UI* pIns = new CAsset_Debug_UI(pD, pC);
    if (FAILED(pIns->Initialize(pArg)))
    {
        MSG_BOX("imgui_Creater 생성 실패"); Safe_Release(pIns);
    }

    return pIns;
}

void CAsset_Debug_UI::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(g_pSelectedObject);
}

