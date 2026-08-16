#include "Engine_Define.h"
#include "ObjectFactory.h"

#include "GameInstance.h"
#include "FileParsing_Data.h"
#include "FileManager.h"
#include "Character.h"
#include "MapObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::ObjectFactory::ObjectFactory()
{
}

Engine::ObjectFactory::~ObjectFactory()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ObjectFactory::Initialize()
{
	m_pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



HRESULT Engine::ObjectFactory::Spawn_Object()
{
	return E_NOTIMPL;
}



//////////////////////////////////////////////////////// 레벨 로드 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ObjectFactory::Spawn_LevelData(const _string& _folderPath, _uint _level_for_create, _bool _useOnlyJson)
{
	LevelLoadResult result = FileManager::Load_LevelData(_folderPath, _useOnlyJson);

	Spawn_LevelData(result, _level_for_create);

	return S_OK;
}

HRESULT Engine::ObjectFactory::Spawn_LevelData(LevelLoadResult _result, _uint _level_for_create)
{
#pragma region 디버깅 출력용
	///* 이런게 나왔다 라는 정보 출력용 */
	//for (const auto& mapInfo : result.vecMapObjects)
	//{
	//	COUT("--- 오브젝트: " + mapInfo.strObjectKey + " ---");

	//	// 쉐이더 정보 찾기
	//	auto shaderIter = result.umapShaderInfos.find(mapInfo.strObjectKey);
	//	if (shaderIter != result.umapShaderInfos.end())
	//	{
	//		const ShaderObjectInfo& shaderInfo = shaderIter->second;
	//		COUT("  쉐이더: " + wstringToString(shaderInfo.wstrShaderPrototype));
	//		COUT("  패스 수: " + to_string(shaderInfo.vecMeshPasses.size()));
	//	}
	//	else
	//	{
	//		COUT("  쉐이더 정보 없음");
	//	}
	//}
	///* 이런게 나왔다 라는 정보 출력용 */
#pragma endregion 디버깅 출력용


	UMAP<_string, GameObject::GAMEOBJECT_DESC> umapAnimtoolDesc;
	UMAP<_string, GameObject::GAMEOBJECT_DESC> umapShaderDesc;

	// 애니메이션 정보를 읽었다면
	if (_result.bReadAnimation)
	{
		// 애니메이션(캐릭터) 툴 정보 읽고 저장해두기
		for (const auto& [objKey, animInfo] : _result.umapAnimInfos)
		{
			// 혹시나 있을 중복 방지(가장 앞의 것만 읽음)
			if (umapAnimtoolDesc.contains(objKey))
				continue;

			// 일단 크기, 속도만 저장하게 해둠 나머지는 편한대로 추가
			GameObject::GAMEOBJECT_DESC chardesc;
			chardesc.vScale = animInfo.vScale;
			chardesc.fSpeed = animInfo.fSpeed;
			chardesc.fRotationSpeed = animInfo.fRotationSpeed;

			umapAnimtoolDesc[objKey] = chardesc;
		}
	}
	if (_result.bReadShader)
	{
		// 쉐이더 툴 정보 읽고 저장해두기
		for (const auto& [objKey, shaderInfo] : _result.umapShaderInfos)
		{
			// 혹시나 있을 중복 방지(가장 앞의 것만 읽음)
			if (umapShaderDesc.contains(objKey))
				continue;

			// 일단 쉐이더 이름, 메쉬별 패스번호만 저장해둠 나머지는 편한대로 추가
			GameObject::GAMEOBJECT_DESC shaderdesc;
			shaderdesc.wstrShaderName = shaderInfo.wstrShaderPrototype;
			shaderdesc.vecPasses = shaderInfo.vecMeshPasses;

			umapShaderDesc[objKey] = shaderdesc;
		}
	}


	for (const auto& mapInfo : _result.vecMapObjects)
	{
		_string objKey = mapInfo.strObjectKey;
		// 오브젝트 생성
		GameObject* pObj = nullptr;
		MapObject::MAPOBJECT_DESC desc;
		desc.bSetWorldPos = true;
		desc.matWorldPos = mapInfo.matWorld;
		desc.wstrModelName = ObjKey_To_Model_Tag(objKey);
		desc.jExtraData = mapInfo.jExtraData;

		// 기본으로 사용할 쉐이더
		desc.wstrShaderName = L"Prototype_Component_Shader_VTXMesh";

		// 애니메이션 툴에서의 정보를 가지고 있다면 desc에 등록
		if (umapAnimtoolDesc.contains(objKey))
		{
			desc.vScale = umapAnimtoolDesc[objKey].vScale;
			desc.fSpeed = umapAnimtoolDesc[objKey].fSpeed;
			desc.fRotationSpeed = umapAnimtoolDesc[objKey].fRotationSpeed;
		}
		// 쉐이더 툴에서의 정보를 가지고 있다면 desc에 등록
		if (umapShaderDesc.contains(objKey))
		{
			desc.wstrShaderName = umapShaderDesc[objKey].wstrShaderName;
			desc.vecPasses = umapShaderDesc[objKey].vecPasses;
		}


		// 오브젝트 레이어에 추가
		m_pGameInstance->Add_GameObject_To_Layer(_level_for_create, mapInfo.wstrPrototypeName, _level_for_create, mapInfo.wstrLayerName, &pObj, &desc);

		if (!pObj)
		{
			COUT("오잉??");
			continue;
		}
		// 쉐이더 정보 적용
		//if (_result.bReadShader == true)
		//{
		//	auto shaderIter = _result.umapShaderInfos.find(mapInfo.strObjectKey);
		//	if (shaderIter != _result.umapShaderInfos.end())
		//	{
		//		const ShaderObjectInfo& shaderInfo = shaderIter->second;

		//		// 실제 쉐이더 적용
		//		pObj->Set_Shader(shaderInfo.wstrShaderPrototype);
		//		pObj->Set_Passes(shaderInfo.vecMeshPasses);

		//		COUT("[적용] " + mapInfo.strObjectKey + " <- " + wstringToString(shaderInfo.wstrShaderPrototype));
		//	}
		//}
	}

	return S_OK;
}
/******************************************************* 레벨 로드 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
ObjectFactory* Engine::ObjectFactory::Create()
{
	ObjectFactory* pInstance = new ObjectFactory();

	MSG_FAIL(pInstance->Initialize(), L"ObjectFactory 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

//ObjectFactory* Engine::ObjectFactory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//	ObjectFactory* pInstance = new ObjectFactory(pDevice, pContext);
//
//	MSG_FAIL(pInstance->Initialize_Prototype(), L"ObjectFactory 생성 실패", L"Caution!!!", nullptr);
//
//	return pInstance;
//}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::ObjectFactory::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

