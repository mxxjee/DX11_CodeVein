#include "Engine_Define.h"
#include "LightManager.h"

#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "GameClock.h"

_int Engine::LightManager::m_iLightIndex = 0;

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::LightManager::LightManager()
{
}

Engine::LightManager::LightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
	m_iLightIndex = _uint(m_umapLights.size());
}

Engine::LightManager::~LightManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
LightManager* Engine::LightManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	LightManager* pInstance = new LightManager(pDevice, pContext);

	MSG_NULL(pInstance, L"LightManager 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::LightManager::Add_Light(const _uint _lightindex, const LIGHT_DESC& _lightdesc)
{
    _uint safeIndex = _lightindex;
    if (m_umapLights.contains(safeIndex))
    {
        safeIndex = m_iLightIndex;
        while (m_umapLights.contains(safeIndex))
        {
            safeIndex++;
        }
    }

	Light* light = Light::Create(m_pDevice, m_pContext, _lightdesc);
	CHECK_NULLPTR(light);

	m_umapLights.emplace(safeIndex, light);

    m_iLightIndex = max(m_iLightIndex, (_int)(safeIndex + 1)); //라이트 자동추가할때 무조건++뒤에 만들어지게

	return S_OK;
}

HRESULT Engine::LightManager::Delete_Light(const _uint _lightindex)
{
	if(!m_umapLights.contains(_lightindex))
	{
        COUT(to_string(_lightindex) + "번 조명 없음");
		//MSG_ON((to_wstring(_lightindex) + L"번 조명이 존재하지 않습니다").c_str(), L"조명 삭제 실패");
		//BREAK;
		return S_OK;
	}

	Safe_Release(m_umapLights.at(_lightindex));
    m_umapLights.erase(_lightindex);

	return S_OK;
}
/******************************************************* 컨테이너 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 조명 정보 획득, 수정 함수 ////////////////////////////////////////////////////////
LIGHT_DESC* Engine::LightManager::Get_LightDesc(const _uint _lightindex)
{
	if(!m_umapLights.contains(_lightindex))
	{
		MSG_ON((to_wstring(_lightindex) + L"번 조명이 존재하지 않습니다").c_str(), L"조명 검색 실패");
		BREAK;
		return nullptr;
	}

	return (m_umapLights.at(_lightindex)->Get_LightDesc());
}

HRESULT Engine::LightManager::Set_LightDesc(const _uint _lightindex, const LIGHT_DESC& _lightdesc)
{
	if(!m_umapLights.contains(_lightindex))
	{
		MSG_ON((to_wstring(_lightindex) + L"번 조명이 존재하지 않습니다").c_str(), L"조명 검색 실패");
		BREAK;
		return E_FAIL;
	}

	return m_umapLights.at(_lightindex)->Set_LightDesc(_lightdesc);
}
/******************************************************* 조명 정보 획득, 수정 함수 *******************************************************/



void Engine::LightManager::Update_LightPlayer_Distance()
{
    if (!g_bClient && g_toolType != TOOLTYPE::MAP_TOOL && g_toolType != TOOLTYPE::SHADER_TOOL)
        return;

    if (m_pPlayer == nullptr)
    {
        static _float Timer = 0.f;
        Timer += m_pGameInstance->Get_Clock(L"Clock_Default")->Get_DeltaTime();
        if(Timer >= 10.f)
        {
            Timer = 0.f;
            m_pPlayer = m_pGameInstance->Get_Player();
            if (m_pPlayer == nullptr)
                return;
        }
        else
            return;
    }

    _vector vPlayerPos = m_pGameInstance->Get_PlayerPos_Vector();
    for (auto& Lights : m_umapLights)
    {
        Light* temp = Lights.second;

        if (temp->Get_LightDesc()->bIsVisible == false)
            continue;

        if (temp->Get_LightDesc()->eType == LIGHT::POINT)
        {
            temp->Update_LightPlayer_Distance(vPlayerPos);
        }
    }
}

//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
//void Engine::LightManager::Render(Shader* _shader, VIBuffer_Rect* _buffer, const _float fTimeDelta)
//{
//    for(auto& light : m_umapLights)
//    {
//        light.second->Render(_shader, _buffer, fTimeDelta);
//    }
//}
//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
void Engine::LightManager::Render(Shader* _shader, VIBuffer_Rect* _buffer, const _float fTimeDelta)
{
    const _uint BATCH_SIZE = 64;

    _float4 Lights_vDiffuse[BATCH_SIZE];
    _float4 Lights_vAmbient[BATCH_SIZE];
    _float4 Lights_vSpecular[BATCH_SIZE];
    _float4 Lights_vPosition[BATCH_SIZE];
    _float4 Lights_vRangeAndType[BATCH_SIZE];
    _uint LightCount = 0;
    m_vecPointLightsScratch.clear();

    for (auto& pair : m_umapLights)
    {
        Light* pLight = pair.second;
        LIGHT_DESC* pDesc = pLight->Get_LightDesc();

        if (pDesc->bIsVisible == false)
            continue;

        if (pDesc->eType == LIGHT::DIRECTIONAL)
        {
            pLight->Render(_shader, _buffer, fTimeDelta);
        }
        if (pDesc->eType == LIGHT::SPOTLIGHT)
        {
            pLight->Render(_shader, _buffer, fTimeDelta);
        }
        //if (pDesc->eType == LIGHT::POINT)
        //{
        //    pLight->Render(_shader, _buffer, fTimeDelta);
        //    //m_PointLights.push_back(pLight);
        //}
        //else if (pDesc->eType == LIGHT::POINT)
        //{
        //    lights_vDiffuse[lightCount] = pDesc->vDiffuse;
        //    lights_vAmbient[lightCount] = pDesc->vAmbient;
        //    lights_vSpecular[lightCount] = pDesc->vSpecular;
        //    lights_vPosition[lightCount] = pDesc->vPosition;
        //    lights_vRangeAndType[lightCount] = _float4(pDesc->fRange, 0.f, 0.f, 0.f);

        //    lightCount++;

        //    if (lightCount == BATCH_SIZE)
        //    {
        //        //여기서 정렬
        //        _shader->Bind_RawValue_FullSlot(BUFFER_LIGHT, "lights_vDiffuse", lights_vDiffuse, sizeof(_float4) * BATCH_SIZE);
        //        _shader->Bind_RawValue_FullSlot(BUFFER_LIGHT, "lights_vAmbient", lights_vAmbient, sizeof(_float4) * BATCH_SIZE);
        //        _shader->Bind_RawValue_FullSlot(BUFFER_LIGHT, "lights_vSpecular", lights_vSpecular, sizeof(_float4) * BATCH_SIZE);
        //        _shader->Bind_RawValue_FullSlot(BUFFER_LIGHT, "lights_vPosition", lights_vPosition, sizeof(_float4) * BATCH_SIZE);
        //        _shader->Bind_RawValue_FullSlot(BUFFER_LIGHT, "lights_vRangeAndType", lights_vRangeAndType, sizeof(_float4) * BATCH_SIZE);
        //        _shader->Bind_RawValue_FullSlot(BUFFER_LIGHT, "lightCount", &lightCount, sizeof(_uint));

        //        _uint passIndex = _UINT(LIGHT::POINT);
        //        _shader->Begin(passIndex);
        //        _shader->Bind_Resources(passIndex);

        //        _buffer->Bind_Resource();
        //        _buffer->Render(fTimeDelta);

        //        lightCount = 0;
        //    }
        //}
    }

    //프러스텀 컬링?
    _float4x4 InvviewMatrix = m_pGameInstance->Get_PipeLineInversMatrix(D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTS_PROJ);

    BoundingFrustum frustum;
    BoundingFrustum::CreateFromMatrix(frustum, XMLoadFloat4x4(&projMatrix)); //절두체 만들어줌(카메라뷰)
    frustum.Transform(frustum, XMLoadFloat4x4(&InvviewMatrix)); //절두체 모양 유지한채 위치와 방향만 옮겨줌

    for (auto& pair : m_umapLights)
    {
        Light* pLight = pair.second;
        LIGHT_DESC* pDesc = pLight->Get_LightDesc();

        if (pDesc->bIsVisible == false)
            continue;

        if (pDesc->eType != LIGHT::POINT)
            continue;

        BoundingSphere sphere(
            XMFLOAT3(pDesc->vPosition.x, pDesc->vPosition.y, pDesc->vPosition.z), //position을 기준으로 원을 만들어줌
            pDesc->fRange
        );

        ContainmentType result = frustum.Contains(sphere); //위에서 만든 절두체로 포함되있는지 확인
        
        if (g_toolType != TOOLTYPE::SHADER_TOOL) //TOOLTYPE::MAP_TOOL
        {
            if (result == DISJOINT) // 완전히 밖 클라 빛 테스트 위해서 꺼놓음(나중에 켜기)
                continue;

            if (pLight->Get_LightDesc()->fPlayerDistance <= 0.f)// 완전히 밖 클라 빛 테스트 위해서 꺼놓음(나중에 켜기)
                continue;
        }

        m_vecPointLightsScratch.push_back(pLight);  
    }

    if (g_toolType != TOOLTYPE::SHADER_TOOL) //TOOLTYPE::MAP_TOOL
    {
        sort(m_vecPointLightsScratch.begin(), m_vecPointLightsScratch.end(), [](Light* a, Light* b)
            {
                return a->Get_LightDesc()->fPlayerDistance < b->Get_LightDesc()->fPlayerDistance;
            });
        _int maxLightSize = 22;
        if (m_vecPointLightsScratch.size() > maxLightSize)//60 클라빛 테스트위해서 25->60으로 해둠(나중에 수정)
            m_vecPointLightsScratch.resize(maxLightSize);
    }
    else//맵툴
    {
        if (m_vecPointLightsScratch.size() > BATCH_SIZE)
            m_vecPointLightsScratch.resize(BATCH_SIZE);
    }

        for (auto& Lights : m_vecPointLightsScratch)
    {
        LIGHT_DESC* pDesc = Lights->Get_LightDesc();

        Lights_vDiffuse[LightCount] = pDesc->vDiffuse;
        Lights_vAmbient[LightCount] = pDesc->vAmbient;
        Lights_vSpecular[LightCount] = pDesc->vSpecular;
        Lights_vPosition[LightCount] = pDesc->vPosition;
        Lights_vRangeAndType[LightCount] = _float4(pDesc->fRange, 0.f, 0.f, 0.f);
        LightCount++;
    }
    //여기서 정렬 거리별로 정렬한다음에 앞에서 16개정도만 짜르고 넣기
    if (LightCount > 0)
    {
        _shader->Bind_RawValue_ByHandle(lights_vDiffuse, Lights_vDiffuse, sizeof(_float4) * BATCH_SIZE);
        _shader->Bind_RawValue_ByHandle(lights_vAmbient, Lights_vAmbient, sizeof(_float4) * BATCH_SIZE);
        _shader->Bind_RawValue_ByHandle(lights_vSpecular, Lights_vSpecular, sizeof(_float4) * BATCH_SIZE);
        _shader->Bind_RawValue_ByHandle(lights_vPosition, Lights_vPosition, sizeof(_float4) * BATCH_SIZE);
        _shader->Bind_RawValue_ByHandle(lights_vRangeAndType, Lights_vRangeAndType, sizeof(_float4) * BATCH_SIZE);
        _shader->Bind_RawValue_ByHandle(lightCount, &LightCount, sizeof(_uint));

        _uint passIndex = _UINT(LIGHT::POINT);
        _shader->Begin(passIndex);
        _shader->Bind_Resources(passIndex);

        _buffer->Bind_Resource();
        _buffer->Render(fTimeDelta);
    }
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::LightManager::Free()
{
	__super::Free();
    Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& light : m_umapLights)
	{
		Safe_Release(light.second);
	}
	m_umapLights.clear();

}
/******************************************************* 객체 반환 함수 *******************************************************/

