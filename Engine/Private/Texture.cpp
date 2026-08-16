#include "Engine_Define.h"
#include "Texture.h"

#include "Shader.h"
#include "NewTexture.h"
#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Texture::Texture()
{
}

Engine::Texture::Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext)
{
}

Engine::Texture::Texture(const Texture& original)
	: Component(original), m_iNumTextures(original.m_iNumTextures), m_vecTextures(original.m_vecTextures)
{
}

Engine::Texture::~Texture()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Texture::Initialize_Prototype(const _wstring& textureFilePath, const _uint numTextures)
{
    m_iNumTextures = numTextures;

    for (size_t i = 0; i < m_iNumTextures; i++)
    {
        _tchar filepath[MAX_PATH] = {};
        wsprintf(filepath, textureFilePath.c_str(), i);

        ID3D11ShaderResourceView* pSRV = m_pGameInstance->Load_Texture(filepath);
        if (pSRV == nullptr)
        {
            MSG_ON(L"텍스쳐 파일 로딩 실패.", L"경로 문제");
            return E_FAIL;
        }
        m_vecTextures.push_back(pSRV);
    }

    return S_OK;
}

HRESULT Engine::Texture::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Texture::Bind_ShaderResource(Shader* shader, const _string& constantname, _uint textureindex)
{
	if (textureindex >= m_iNumTextures)
	{
		MSG_ON(L"텍스쳐 인덱스 번호가 가지고 있는 텍스쳐 갯수를 초과합니다.", L"텍스쳐 바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return shader->Bind_SRV(constantname, m_vecTextures[textureindex]);
}

HRESULT Engine::Texture::Bind_ShaderResources(Shader* shader, const _string& constantname)
{
	return shader->Bind_SRVs(constantname, &m_vecTextures.front(), m_iNumTextures);
}

HRESULT Engine::Texture::Bind_ShaderResources_Range(Shader* shader, _uint slot, _uint startIndex, _uint count, _uint stageMask)
{
    if (startIndex + count > m_iNumTextures)
    {
        MSG_ON(L"텍스쳐 범위가 가지고 있는 텍스쳐 갯수를 초과합니다.", L"텍스쳐 바인딩 실패");
        BREAK;
        return E_FAIL;
    }

    // startIndex부터 count개의 SRV 포인터 배열 전달
    return shader->Bind_SRVs_FullSlot(slot, &m_vecTextures[startIndex], count, stageMask);
}

HRESULT Engine::Texture::Bind_ShaderResource_FullSlot(Shader* shader, _uint slot, _uint textureIndex, _uint stageMask)
{
    if (textureIndex >= m_iNumTextures)
    {
        MSG_ON(L"텍스쳐 인덱스 번호가 가지고 있는 텍스쳐 갯수를 초과합니다.", L"텍스쳐 바인딩 실패");
        BREAK;
        return E_FAIL;
    }

    return shader->Bind_SRV_FullSlot(slot, m_vecTextures[textureIndex], stageMask);
}

HRESULT Engine::Texture::Bind_ShaderResources_FullSlot(Shader* shader, _uint slot, _uint stageMask)
{
    return shader->Bind_SRVs_FullSlot(slot, &m_vecTextures.front(), m_iNumTextures, stageMask);
}

HRESULT Engine::Texture::Bind_ShaderResources_Range_FullSlot(Shader* shader, _uint slot, _uint startIndex, _uint count, _uint stageMask)
{
    if (startIndex + count > m_iNumTextures)
    {
        MSG_ON(L"텍스쳐 범위가 가지고 있는 텍스쳐 갯수를 초과합니다.", L"텍스쳐 바인딩 실패");
        BREAK;
        return E_FAIL;
    }

    return shader->Bind_SRVs_FullSlot(slot, &m_vecTextures[startIndex], count, stageMask);
}
/******************************************************* 쉐이더 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Texture* Engine::Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& textureFilePath, const _uint numTextures)
{
	Texture* pInstance = new Texture(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(textureFilePath, numTextures), L"Texture 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::Texture::Clone(void* arg)
{
	Texture* pInstance = this;

	MSG_FAIL(pInstance->Initialize(arg), L"Texture 복사 실패", L"Caution!!!", nullptr);
	Safe_AddRef(pInstance);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Texture::Free()
{
	__super::Free();

	if(!m_bIsClone)
	{
		for (auto& srv : m_vecTextures)
		{
			Safe_Release(srv);
		}
	}

}
/******************************************************* 객체 반환 함수 *******************************************************/

