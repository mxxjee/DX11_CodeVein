#include "Engine_Define.h"
#include "Editor_Utils.h"

 void EditorUtils::Render_Search(ImGuiTextFilter& pHierarchyFilter)
{
	pHierarchyFilter.Draw("Search (inc, -exc)");
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Filter usage:\n"
			"  \"\"         display all\n"
			"  \"xxx\"      display items containing \"xxx\"\n"
			"  \"xxx,yyy\"  display items containing \"xxx\" OR \"yyy\"\n"
			"  \"-xxx\"     hide items containing \"xxx\"");

	ImGui::Separator();
}

 void EditorUtils::Show_Texture_Big(string KeyName, ID3D11ShaderResourceView* popUpSRV)
{

	ImGui::BeginTooltip();

	// 파일 이름
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[ Preview : %s ]", KeyName.c_str());
	ImGui::Separator();

	// 확대된 이미지 출력
	float previewSize = 200.0f;
	ImGui::Image((ImTextureID)popUpSRV, ImVec2(200, 200), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0.5f));


	ImGui::EndTooltip();
}

/*텍스처 바꾸는버튼 + 검색창 + 텍스처리스트*/
 void EditorUtils::Show_TextureList(const string& popupName, ImGuiTextFilter& pHierarchyFilter, NewTexture* pNewTexCom, string* pOutSelectTexKey, _uint* pOutSelectIdx, int Columns)
{
    if (ImGui::BeginPopup(popupName.c_str()))
    {
        Render_Search(pHierarchyFilter);
        ImGui::Separator();

      
        // map<폴더명, vector<텍스처인덱스>>
        std::map<string, std::vector<size_t>> folderGroups;

        for (size_t k = 0; k < pNewTexCom->Get_NumTextures(); ++k)
        {
            string fullPath = pNewTexCom->Get_TextureName(CAST(int)(k));
            if (!pHierarchyFilter.PassFilter(fullPath.c_str())) continue;

            size_t slashPos = fullPath.find_last_of('/');
            string folderName = (slashPos == string::npos) ? "Root" : fullPath.substr(0, slashPos);
            folderGroups[folderName].push_back(k);
        }

     
        float thumbnailSize = 50.0f;

        for (auto& pair : folderGroups)
        {
            const string& folderName = pair.first;
            const auto& indices = pair.second;

            // 폴더 트리 노드 열기
            if (ImGui::TreeNodeEx(folderName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
            {
                ImGui::Indent();

                for (size_t i = 0; i < indices.size(); ++i)
                {
                    size_t k = indices[i];
                    string fullPath = pNewTexCom->Get_TextureName(CAST(int)(k));
                    ID3D11ShaderResourceView* pSRV = pNewTexCom->Get_SRV(CAST(int)(k));

                    if (pSRV)
                    {
                        ImGui::PushID((int)k);
                        if (ImGui::ImageButton("##TexBtn", (ImTextureID)pSRV, ImVec2(thumbnailSize, thumbnailSize)))
                        {
                            *pOutSelectTexKey = fullPath;
                            *pOutSelectIdx = (_uint)k;
                            ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::IsItemHovered())
                            Show_Texture_Big(fullPath, pSRV);
                        ImGui::PopID();

                        // 그리드 정렬: 폴더 내부에서만 작동
                        if ((i + 1) % Columns != 0 && (i + 1) < indices.size())
                            ImGui::SameLine();
                    }
                }

                ImGui::Unindent();
                ImGui::TreePop(); // 폴더 닫기
            }
        }

        ImGui::EndPopup();
    }
}
/*끝판왕 Resource Selector UI*/
void EditorUtils::Draw_ResourceSelector(const string& label, const string& popupName, ImGuiTextFilter& pHierarchyFilter, NewTexture* pNewTexCom, string* pOutSelectTexKey, _uint* pOutSelectIdx, int Columns)
{
	ImGui::PushID(popupName.c_str()); // ID 충돌 방지

	ImGui::AlignTextToFramePadding();
	ImGui::Text("%s", label.c_str());
	ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f); // 40% 지점에서 버튼 시작

	string currentName = *pOutSelectTexKey;
	if (currentName == "") currentName = "None (Texture)";

	//버튼
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // 약간 어두운 회색 (유니티 느낌)
	if (ImGui::Button(currentName.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
	{
		ImGui::OpenPopup(popupName.c_str());
	}
	ImGui::PopStyleColor();

	Show_TextureList(popupName,pHierarchyFilter, pNewTexCom, pOutSelectTexKey, pOutSelectIdx, Columns);
	ImGui::PopID();
}