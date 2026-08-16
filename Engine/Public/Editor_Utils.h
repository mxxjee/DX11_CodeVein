#pragma once
#include "NewTexture.h"

namespace EditorUtils
{
	 void Render_Search(ImGuiTextFilter& pHierarchyFilter);

	 void Show_Texture_Big(string KeyName, ID3D11ShaderResourceView* popUpSRV);
	
	/*텍스처 바꾸는버튼 + 검색창 + 텍스처리스트*/
	 void Show_TextureList(const string& popupName, ImGuiTextFilter& pHierarchyFilter, NewTexture* pNewTexCom, string* pOutSelectTexKey, _uint* pOutSelectIdx, int Columns = 10);
	

	/*끝판왕 Resource Selector UI*/
	void Draw_ResourceSelector(const string& label, const string& popupName, ImGuiTextFilter& pHierarchyFilter, NewTexture* pNewTexCom, string* pOutSelectTexKey, _uint* pOutSelectIdx, int Columns = 10);


}
