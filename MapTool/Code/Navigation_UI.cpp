#include "MT_Defines.h"
#include "Navigation_UI.h"
#include "GameInstance.h"
#include "SplineMgr.h" 
#include "Layer.h"
#include "GameObject.h"

using namespace Engine;

CNavigation_UI::CNavigation_UI(ID3D11Device* pD, ID3D11DeviceContext* pC)
    : ImguiWindow(pD, pC)
{
}

HRESULT CNavigation_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pGameInstance = GameInstance::GetInstance();
    if (nullptr != m_pGameInstance)
    {
        Safe_AddRef(m_pGameInstance);
    }

    if (nullptr == SplineMgr::GetInstance())
    {
        MSG_BOX("SplineMgr 인스턴스가 없습니다.");
        return E_FAIL;
    }
    SplineMgr::GetInstance()->Ready_SplineMgr();

    // 디버그 렌더링용 이펙트 생성
    m_pEffect = new BasicEffect(m_pDevice);
    m_pEffect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;
    m_pEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    if (FAILED(m_pDevice->CreateInputLayout(
        VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount,
        shaderByteCode,
        byteCodeLength,
        &m_pInputLayout)))
    {
        MSG_BOX("InputLayout 생성 실패");
        return E_FAIL;
    }

    return S_OK;
}

_uint CNavigation_UI::Update_Contents(_float fDT)
{
    SplineMgr* pSplineMgr = SplineMgr::GetInstance();
    if (nullptr == pSplineMgr) return 0;

    //입력 처리
    if (g_eToolMode == TOOL_MODE::SPLINE_EDIT)
    {
        // ImGui 창 위에서는 클릭 무시
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            pSplineMgr->HandleInput(fDT);
        }
    }
    pSplineMgr->Update(fDT);

    // //////////////////////////////////////////////////////////////////////////////////////////////////////

    // 스플라인 편집 모드 토글
    if (ImGui::CollapsingHeader("Spline Edit Mode", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool bIsSplineMode = (g_eToolMode == TOOL_MODE::SPLINE_EDIT);
        if (ImGui::Checkbox("Enable Point Editing", &bIsSplineMode))
        {
            g_eToolMode = bIsSplineMode ? TOOL_MODE::SPLINE_EDIT : TOOL_MODE::NONE;
        }

        if (bIsSplineMode)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "[ EDITING ]");
            ImGui::Text("Left: Add Node / Right: Undo / Space: End Track");
            ImGui::Text("Z: Set Last as Zone");
        }
        else
        {
            if (g_eToolMode != TOOL_MODE::NONE)
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Other tool is active)");
        }
    }

    ImGui::Separator();
    ImGui::Spacing();

    // //////////////////////////////////////////////////////////////////////////////////////////////////////

    // 파일 관리
    if (ImGui::CollapsingHeader("File Manager"))
    {
        const char* szFileNames[] = {
            "Stage01_Spline.dat", "Stage02_Spline.dat",
            "Monster_Patrol_01.dat", "Test_Path.dat"
        };

        ImGui::Text("File Preset:");
        // m_iSelectedFileIndex는 헤더에 선언되어 있다고 가정
        ImGui::Combo("##FileCombo", &m_iSelectedFileIndex, szFileNames, IM_ARRAYSIZE(szFileNames));

        float width = ImGui::GetContentRegionAvail().x;
        if (ImGui::Button("Save", ImVec2(width * 0.5f - 5.f, 0)))
        {
            string strFileName = szFileNames[m_iSelectedFileIndex];
            pSplineMgr->Save_Spline(strFileName);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load", ImVec2(width * 0.5f - 5.f, 0)))
        {
            string strFileName = szFileNames[m_iSelectedFileIndex];
            pSplineMgr->Load_Spline(strFileName);
        }
    }

    ImGui::Spacing();

    // //////////////////////////////////////////////////////////////////////////////////////////////////////

    // 전역 설정
    if (ImGui::CollapsingHeader("Global Settings"))
    {
        // 속도 조절
        static float fSpeed = 5.0f;
        if (ImGui::DragFloat("Base Speed", &fSpeed, 0.1f, 0.1f, 50.0f, "%.1f")) {
            pSplineMgr->Set_Speed(fSpeed);
        }

        // 루프 토글
        if (ImGui::Button("Toggle Loop")) {
            pSplineMgr->ToggleLoop();
        }
        ImGui::SameLine();
        if (pSplineMgr->Is_Loop()) ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "[Loop ON]");
        else ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "[Loop OFF]");

        // 전체 초기화
        if (ImGui::Button("Reset All Path & Data")) {
            pSplineMgr->ResetPath();
        }
    }

    ImGui::Spacing();

    // //////////////////////////////////////////////////////////////////////////////////////////////////////

    //멀티 트랙 관리 및 노드 편집
    if (ImGui::CollapsingHeader("Multi-Track & Nodes", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // UI 상태 관리를 위한 static 변수
        static int iSelectedTrack = -1;

        // 트랙 리스트
        ImGui::BeginChild("TrackList", ImVec2(100, 250), true);
        ImGui::Text("Tracks: %d", pSplineMgr->Get_PathCount());
        ImGui::Separator();

        for (int i = 0; i < pSplineMgr->Get_PathCount(); ++i)
        {
            string strLabel = "Track " + to_string(i);
            if (ImGui::Selectable(strLabel.c_str(), iSelectedTrack == i))
            {
                iSelectedTrack = i;
                // SplineMgr에게 현재 작업할 트랙을 알려줌
                pSplineMgr->Select_Path(i);
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // 선택된 트랙의 노드 상세 정보
        ImGui::BeginChild("NodeDetail", ImVec2(0, 250), true);

        if (iSelectedTrack >= 0 && iSelectedTrack < pSplineMgr->Get_PathCount())
        {
            // 선택된 트랙의 노드 데이터 가져오기
            const auto& nodes = pSplineMgr->Get_Nodes(iSelectedTrack);

            ImGui::Text("Selected Track: %d (Nodes: %d)", iSelectedTrack, (int)nodes.size());
            ImGui::SameLine();
            if (ImGui::SmallButton("Undo")) pSplineMgr->RemoveLastPoint();

            if (ImGui::BeginTable("NodesTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 30.0f);
                ImGui::TableSetupColumn("Position");
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableSetupColumn("Radius", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Del", ImGuiTableColumnFlags_WidthFixed, 30.0f);
                ImGui::TableHeadersRow();

                for (int i = 0; i < (int)nodes.size(); ++i)
                {
                    ImGui::TableNextRow();
                    ImGui::PushID(i);

                    // 1. ID
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", i);

                    // 2. Position
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("(%.1f, %.1f, %.1f)", nodes[i].vPos.x, nodes[i].vPos.y, nodes[i].vPos.z);

                    // 3. Type
                    ImGui::TableSetColumnIndex(2);
                    const char* items[] = { "PATH", "ZONE" };
                    int currentItem = (int)nodes[i].eType;

                    ImGui::SetNextItemWidth(70);
                    if (ImGui::Combo("##Type", &currentItem, items, IM_ARRAYSIZE(items)))
                    {
                        // 편집을 위해 선택된 트랙을 다시 한 번 확실히 지정
                        pSplineMgr->Select_Path(iSelectedTrack);
                        pSplineMgr->SetPointType(i, (POINT_TYPE)currentItem);

                        // ZONE 변경 시 기본 반경 설정
                        if ((POINT_TYPE)currentItem == POINT_TYPE::ZONE && nodes[i].fRadius <= 0.1f)
                        {
                            pSplineMgr->SetPointRadius(i, 5.0f);
                        }
                    }

                    // 4. Radius
                    ImGui::TableSetColumnIndex(3);
                    if (nodes[i].eType == POINT_TYPE::ZONE)
                    {
                        float r = nodes[i].fRadius;
                        ImGui::SetNextItemWidth(60);
                        if (ImGui::DragFloat("##Rad", &r, 0.1f, 0.0f, 100.0f, "%.1f"))
                        {
                            pSplineMgr->Select_Path(iSelectedTrack);
                            pSplineMgr->SetPointRadius(i, r);
                        }
                    }
                    else
                    {
                        ImGui::TextDisabled(" - ");
                    }

                    // 5. Delete
                    ImGui::TableSetColumnIndex(4);
                    if (ImGui::Button("X")) {
                        pSplineMgr->Select_Path(iSelectedTrack);
                        pSplineMgr->DeletePoint(i);
                    }

                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Select a Track from the left list.");
        }

        ImGui::EndChild();
    }

    return 0;
}

void CNavigation_UI::Render_Spline_3D()
{
    SplineMgr* pSplineMgr = SplineMgr::GetInstance();
    if (!pSplineMgr || !m_pEffect || !m_pInputLayout) return;

    PrimitiveBatch<VertexPositionColor>* pBatch = m_pGameInstance->Get_DebugBatch();
    if (!pBatch) return;

    m_pGameInstance->Set_DepthStencilState("DSS_NoDepth");

    _float4x4 viewMat = m_pGameInstance->Get_PipeLineMatrix(D3DTS_VIEW);
    _float4x4 projMat = m_pGameInstance->Get_PipeLineMatrix(D3DTS_PROJ);

    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(XMLoadFloat4x4(&viewMat));
    m_pEffect->SetProjection(XMLoadFloat4x4(&projMat));

    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);

    pBatch->Begin();
    pSplineMgr->Render(pBatch);
    pBatch->End();

    m_pGameInstance->Set_DepthStencilState("DSS_Default");
}

CNavigation_UI* CNavigation_UI::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg)
{
    CNavigation_UI* pInstance = new CNavigation_UI(pD, pC);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CNavigation_UI 생성 실패");
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CNavigation_UI::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

    if (m_pEffect)
    {
        delete m_pEffect;
        m_pEffect = nullptr;
    }

    Safe_Release(m_pInputLayout);
}