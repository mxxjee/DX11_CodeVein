#include "Client_Define.h"
#include "IMGUI_GameObject.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Mesh.h"
#include "ImGuizmo.h"
#include "PoolingManager.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::IMGUI_GameObject::IMGUI_GameObject()
{
}

Client::IMGUI_GameObject::IMGUI_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: ImguiWindow(pDevice, pContext)
{
	m_pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

Client::IMGUI_GameObject::~IMGUI_GameObject()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::IMGUI_GameObject::Initialize(void* arg)
{
	m_WindowTitle = "GameObject";

	ImGui::SetNextWindowSize(ImVec2(300.0f, 400.0f), ImGuiCond_FirstUseEver);

	// 오브젝트 교체 이벤트 발생시
	m_pGameInstance->Subscribe<IMGUI_EVENT>([this](const IMGUI_EVENT& e) {
		if (e.eType == IMGUI_EVENT_TYPE::CHANGE_OBJ)
		{
			// 현재 사용중인 오브젝트가 있으면 반환
			if (m_pCurrentObject)
			{
				if (e.bDeleteObject)
				{
					// 지금 소환되어있는 오브젝트랑 다른 오브젝트면 Pool에 반납
					if (m_pCurrentObject != e.pGameObject)
						PoolingManager::Get_Instance()->Despawn_Object(m_pCurrentObject);
				}

				Safe_Release(m_pCurrentObject);
			}

			// 교체 후 레퍼런스 카운트 추가
			m_pCurrentObject = e.pGameObject;
			Safe_AddRef(m_pCurrentObject);

			m_bOpen = true;
			m_iMeshNum = -1;
		}
		});

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 그리기 함수 ////////////////////////////////////////////////////////
_uint Client::IMGUI_GameObject::Update_Contents(_float fTimeDelta)
{
	if (m_pCurrentObject == nullptr)
	{
		m_bOpen = false;
		return 0;
	}

	if (m_pCurrentObject->Is_Dead())
	{
		Safe_Release(m_pCurrentObject);
		return 0;
	}

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Render_CameraButtons();
		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Render_Transform();
		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Shader", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Render_Shader();
	}

	if (ImGui::Button("Kill"))
	{
		m_pCurrentObject->Set_Dead(true);
		Safe_Release(m_pCurrentObject);
		return 0;
	}

	if (ImGui::Button("LockOn"))
	{
		CameraEvent event;
		event.eCameraState = CAMERA_STATE::LOCK_ON;
		event.pLockOnTarget = m_pCurrentObject;

		m_pGameInstance->Publish(event);
	}

	return 0;
}

void Client::IMGUI_GameObject::Render_CameraButtons()
{
	if (ImGui::Button("Look_Target")) {
		m_pGameInstance->Camera_LookAt(m_pCurrentObject->Get_Position());
	}
}

void Client::IMGUI_GameObject::Render_Transform()
{
	// 오브젝트에서 월드행렬 추출
	_matrix worldmat = m_pCurrentObject->Get_WorldMatrix();
	_vector scale{}, rotationquat{}, translation{}, degree{};
	if (XMMatrixDecompose(&scale, &rotationquat, &translation, worldmat))
	{
		degree = m_pGameInstance->QuaternionToDegrees(rotationquat);
	}

	// 레이블 너비와 입력 필드 너비 설정
	const float labelWidth = 70.0f;
	const float inputWidth = -FLT_MIN;  // 남은 공간 전부 사용

#pragma region Scale
	_float3 fScale = m_pCurrentObject->Get_Scale();
	_float vScale[3] = { fScale.x, fScale.y, fScale.z };

	// 오른쪽 정렬 레이아웃
	ImGui::Text("Scale");
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(inputWidth);
	ImGui::InputFloat3("##ObjectScale", vScale);

	ImGui::Text(" ");  // 빈 레이블 (정렬용)
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(inputWidth);
	ImGui::DragFloat3("###ObjectScale", vScale, 0.01f, 0.01f, 10.f);

	if (m_vCurrentScale.x != vScale[0] ||
		m_vCurrentScale.y != vScale[1] ||
		m_vCurrentScale.z != vScale[2])
	{
		m_vCurrentScale.x = vScale[0];
		m_vCurrentScale.y = vScale[1];
		m_vCurrentScale.z = vScale[2];
		m_vCurrentScale.w = 1.f;
		m_pCurrentObject->Set_Scale(m_vCurrentScale.x, m_vCurrentScale.y, m_vCurrentScale.z);
	}
#pragma endregion

#pragma region Rotation
	_float3 fDegree = {};
	XMStoreFloat3(&fDegree, degree);
	_float vDegree[3] = { fDegree.x, fDegree.y, fDegree.z };

	// 수정: 오른쪽 정렬 레이아웃
	ImGui::Text("Rotation");
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(inputWidth);
	ImGui::InputFloat3("##ObjectRotaion", vDegree);

	ImGui::Text(" ");
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(inputWidth);
	ImGui::DragFloat3("###ObjectRotaion", vDegree, 0.05f, -360.0f, 360.f);

	if (m_vCurrentRotation.x != vDegree[0] ||
		m_vCurrentRotation.y != vDegree[1] ||
		m_vCurrentRotation.z != vDegree[2])
	{
		m_vCurrentRotation.x = vDegree[0];
		m_vCurrentRotation.y = vDegree[1];
		m_vCurrentRotation.z = vDegree[2];
		m_vCurrentRotation.w = 1.f;
		m_pCurrentObject->Rotation(XMConvertToRadians(m_vCurrentRotation.x), XMConvertToRadians(m_vCurrentRotation.y), XMConvertToRadians(m_vCurrentRotation.z));
	}
#pragma endregion

#pragma region Position
	_float3 worldpos = {};
	XMStoreFloat3(&worldpos, m_pCurrentObject->Get_WorldMatrix().r[3]);
	_float vPosition[4] = { worldpos.x, worldpos.y, worldpos.z, 1.f };
	static _float vTargetPosition[4] = { 0.f, 0.f, 0.f, 0.f };

	// 오른쪽 정렬 레이아웃
	ImGui::Text("Position");
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(inputWidth);
	ImGui::InputFloat3("##ObjectPosition", vPosition);

	ImGui::Text(" ");
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(inputWidth);
	ImGui::DragFloat3("####ObjectPosition", vPosition, 0.01f, -500.0f, 500.0f);

	if (m_vCurrentPos.x != vPosition[0] ||
		m_vCurrentPos.y != vPosition[1] ||
		m_vCurrentPos.z != vPosition[2])
	{
		m_vCurrentPos.x = vPosition[0];
		m_vCurrentPos.y = vPosition[1];
		m_vCurrentPos.z = vPosition[2];
		m_vCurrentPos.w = 1.f;
		m_pCurrentObject->Set_State(DIRECTION::POSITION, m_vCurrentPos);
	}

	ImGui::Text("");
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(inputWidth);
	ImGui::InputFloat3("##PhysxPos", vTargetPosition);

	if (ImGui::Button("Commit PhysxPos", ImVec2(50, 20)))
	{
		physx::PxController* physXController = m_pCurrentObject->Get_Controller();
		if (physXController)
		{
			physXController->setPosition(PxExtendedVec3(vTargetPosition[0], vTargetPosition[1], vTargetPosition[2]));
		}
	}

	//_float4x4 viewMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTS_VIEW);
	//_float4x4 projMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTS_PROJ);
	//_float4x4 worldMatrix = {};
	////XMStoreFloat4x4(&worldMatrix, worldmat);

	//worldMatrix = IdentityMatrix();

	//ImGuizmo::Manipulate(
	//	&viewMatrix._11,        // 카메라 뷰 행렬 (XMFLOAT4X4)
	//	&projMatrix._11,        // 카메라 프로젝션 행렬 (XMFLOAT4X4)
	//	ImGuizmo::TRANSLATE,    // 조작 모드 (TRANSLATE / ROTATE / SCALE)
	//	ImGuizmo::WORLD,        // 좌표 기준 (WORLD / LOCAL)
	//	&worldMatrix._11        // 조작 대상 오브젝트의 월드 행렬
	//);
	//
	//if (ImGuizmo::IsUsing())
	//{
	//	float t[3], r[3], s[3];
	//	ImGuizmo::DecomposeMatrixToComponents(&worldMatrix._11, t, r, s);
	//	// t, r, s를 오브젝트 Transform에 세팅
	//}

#pragma endregion
}

void Client::IMGUI_GameObject::Render_Shader()
{
	// 모델 가져오기
	Model* model = m_pCurrentObject->Get_Model();
	CHECK_JUST_NULL(model);

	// 쉐이더 가져오기
	Shader* shader = m_pCurrentObject->Get_Shader();
	CHECK_JUST_NULL(shader);

	// 쓰고있는 이름 띄우기
	_wstring shadername = m_pCurrentObject->Get_ShaderName();
	_wstring modelname = model->Get_PrototypeName();

	ImGui::Text(wstringToString(modelname).c_str());
	ImGui::Text(wstringToString(shadername).c_str());

	static _uint passNum = {};

	// 오브젝트의 메쉬 선택하기
	vector<Mesh*> meshes = model->Get_Meshes();
	if (ImGui::TreeNode("MeshSelect"))
	{
		if (ImGui::BeginChild("##MeshChild", ImVec2(250, 150), ImGuiChildFlags_Borders))
		{
			for (_uint i = 0; i < model->Get_NumMeshes(); i++)
			{
				string label = to_string(i) + ". " + meshes[i]->Get_Name() + "  Pass : " + to_string(m_pCurrentObject->Get_VecObjPass(i)) + "##" + to_string(i);
				if (ImGui::Selectable(label.c_str(), m_iMeshNum == i))
				{
					m_iMeshNum = i;
					passNum = m_pCurrentObject->Get_VecObjPass(i);
				}
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}

	// 선택된 메쉬가 있으면
	if (m_iMeshNum >= 0)
	{
		// 쉐이더가 가지고 있는 패스의 진입점 이름들을 추출
		vector<_string> items;
		SHADERENTRIES passDesc = shader->Get_ShaderEntries();
		for (int i = 0; i < shader->Get_NumPass(); i++)
		{
			_string tempstring;
			tempstring = passDesc.pEntries[i].vsEntry + ". " + passDesc.pEntries[i].psEntry;
			if (passDesc.pEntries->gsEntry != "")
				tempstring += ". " + passDesc.pEntries->gsEntry;
			if (passDesc.pEntries->dsEntry != "")
				tempstring += ". " + passDesc.pEntries->dsEntry;
			if (passDesc.pEntries->hsEntry != "")
				tempstring += ". " + passDesc.pEntries->hsEntry;

			items.push_back(tempstring);
		}

		// 현재 선택된 패스를 출력
		if (ImGui::BeginCombo("Pass", (to_string(passNum) + ". " + items[passNum]).c_str()))
		{
			for (int i = 0; i < items.size(); i++)
			{
				_bool is_selected = (passNum == i);
				string AddNum = to_string(i) + ". " + items[i];

				// 패스 변경
				if (ImGui::Selectable(AddNum.c_str(), is_selected))
				{
					passNum = i;
					m_pCurrentObject->Set_Pass_VecObjPass(m_iMeshNum, passNum);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}





	model->Get_NumMeshes();

}
/******************************************************* 그리기 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
_int Client::IMGUI_GameObject::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
IMGUI_GameObject* Client::IMGUI_GameObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg)
{
	IMGUI_GameObject* pInstance = new IMGUI_GameObject(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(arg), L"IMGUI_GameObject 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::IMGUI_GameObject::Free()
{
	__super::Free();

	Safe_Release(m_pCurrentObject);
	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/
