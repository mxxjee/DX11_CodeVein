#include "Engine_Define.h"
#include "NewTexture.h"
#include "Shader.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::NewTexture::NewTexture()
{
}

Engine::NewTexture::NewTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext)
{
}

Engine::NewTexture::NewTexture(const NewTexture& original)
	: Component(original), m_iNumTextures(original.m_iNumTextures)
{
	m_pTextures = &original.m_vecTextures;
	m_pTextureInfos = &original.m_TextureInfo;

}

Engine::NewTexture::~NewTexture()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::NewTexture::Initialize_Prototype(const _wstring& FolderPath)
{
    string ImpossibleExtension[] = { ".uianim",".fnt" };

   
    vector<fs::path> files;
    for (const auto& entry : fs::recursive_directory_iterator(FolderPath))
    {
        if (!entry.is_regular_file())
            continue;

        files.push_back(entry.path());
    }


    sort(files.begin(), files.end(), [](const fs::path& a, const fs::path& b)
        {
            string folderA = a.parent_path().filename().string();
            string folderB = b.parent_path().filename().string();

            if (folderA != folderB)
                return folderA < folderB;

            string nameA = a.stem().string();
            string nameB = b.stem().string();

            return nameA < nameB; 
        });

 
    for (const auto& path : files)
    {
        string FullPath = path.string();
        string FolderName = path.parent_path().filename().string();
        string Name = path.stem().string();
        string Extension = path.extension().string();
        string TexKey = FolderName + "/" + Name;

        if (m_TextureInfo.find(TexKey) != m_TextureInfo.end())
            continue;

        ID3D11ShaderResourceView* srv = { nullptr };
        HRESULT hr = {};
        bool bSkip = false;

        for (auto& str : ImpossibleExtension)
        {
            if (str == Extension)
            {
                bSkip = true;
                break;
            }
        }
        if (bSkip)
            continue;

        if (Extension == ".dds")
        {
            hr = CreateDDSTextureFromFile(m_pDevice, path.wstring().c_str(), nullptr, &srv);

            if (FAILED(hr))
            {
                fs::path pngPath = path;
                pngPath.replace_extension(".png");

                if (fs::exists(pngPath))
                {
                    hr = CreateWICTextureFromFileEx(
                        m_pDevice,
                        pngPath.wstring().c_str(),
                        0,
                        D3D11_USAGE_DEFAULT,
                        D3D11_BIND_SHADER_RESOURCE,
                        0,
                        0,
                        WIC_LOADER_IGNORE_SRGB,
                        nullptr,
                        &srv
                    );
                }
            }
        }
        else if (Extension == ".bmp")
        {
            continue;
        }
        else if (Extension == ".tga")
        {
            MSG_ON(L"로딩 실패 tga파일 로딩.", L"파일명 문제");
            return E_FAIL;
        }
        else if (Extension == ".png")
        {
            fs::path ddsPath = path;
            ddsPath.replace_extension(".dds");

            if (fs::exists(ddsPath))
                continue;

            hr = CreateWICTextureFromFileEx(
                m_pDevice,
                path.wstring().c_str(),
                0,
                D3D11_USAGE_DEFAULT,
                D3D11_BIND_SHADER_RESOURCE,
                0,
                0,
                WIC_LOADER_IGNORE_SRGB,
                nullptr,
                &srv
            );
        }
        else
        {
            hr = CreateWICTextureFromFileEx(
                m_pDevice,
                path.wstring().c_str(),
                0,
                D3D11_USAGE_DEFAULT,
                D3D11_BIND_SHADER_RESOURCE,
                0,
                0,
                WIC_LOADER_IGNORE_SRGB,
                nullptr,
                &srv
            );
        }

        MSG_FAIL(hr, L"텍스쳐 파일 로딩 실패.", L"경로 문제", E_FAIL);

        m_TextureInfo[TexKey] = (_uint)m_vecTextures.size();
        m_vecTextures.push_back(srv);
    }

    m_iNumTextures = (_uint)m_vecTextures.size();
    return S_OK;
}

HRESULT Engine::NewTexture::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::NewTexture::Bind_ShaderResource(Shader* shader, const _string& constantname, _uint NewTextureindex)
{
	if (NewTextureindex >= m_iNumTextures)
	{
		MSG_ON(L"텍스쳐 인덱스 번호가 가지고 있는 텍스쳐 갯수를 초과합니다.", L"텍스쳐 바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return shader->Bind_SRV(constantname, m_pTextures->at(NewTextureindex));
}

HRESULT Engine::NewTexture::Bind_ShaderResources(Shader* shader, const _string& constantname)
{
	/* 텍스쳐 하나만 넘어가는 버그 있음 */
	// ID3D11ShaderResourceView* pSRV = m_pTextures->at(0);
	// return shader->Bind_SRVs(constantname, &pSRV, m_iNumTextures);

	return shader->Bind_SRVs(constantname, (ID3D11ShaderResourceView**)m_pTextures->data(), m_iNumTextures);
}

HRESULT Engine::NewTexture::Bind_ShaderResources_Range(Shader* shader, _uint slot, _uint startIndex, _uint count, _uint stageMask)
{
	if (startIndex + count > m_iNumTextures)
	{
		MSG_ON(L"텍스쳐 범위가 가지고 있는 텍스쳐 갯수를 초과합니다.", L"텍스쳐 바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	// startIndex부터 count개의 SRV 포인터 배열 전달
	ID3D11ShaderResourceView* pSRV = m_pTextures->at(startIndex);
	return shader->Bind_SRVs_FullSlot(slot, &pSRV, count, stageMask);
}

HRESULT Engine::NewTexture::Bind_ShaderResource_FullSlot(Shader* shader, _uint slot, _uint NewTextureIndex, _uint stageMask)
{
	if (NewTextureIndex >= m_iNumTextures)
	{
		MSG_ON(L"텍스쳐 인덱스 번호가 가지고 있는 텍스쳐 갯수를 초과합니다.", L"텍스쳐 바인딩 실패");
		BREAK;
		return E_FAIL;
	}


	return shader->Bind_SRV_FullSlot(slot, m_pTextures->at(NewTextureIndex), stageMask);
}

HRESULT Engine::NewTexture::Bind_ShaderResources_FullSlot(Shader* shader, _uint slot, _uint stageMask)
{
	ID3D11ShaderResourceView* pSRV = m_pTextures->front();
	return shader->Bind_SRVs_FullSlot(slot, &pSRV, m_iNumTextures, stageMask);
}

HRESULT Engine::NewTexture::Bind_ShaderResources_Range_FullSlot(Shader* shader, _uint slot, _uint startIndex, _uint count, _uint stageMask)
{
	if (startIndex + count > m_iNumTextures)
	{
		MSG_ON(L"텍스쳐 범위가 가지고 있는 텍스쳐 갯수를 초과합니다.", L"텍스쳐 바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return shader->Bind_SRVs_FullSlot(slot, &m_vecTextures[startIndex], count, stageMask);
}
string Engine::NewTexture::Get_TextureName(int TexIdx)
{
	for (auto& Info : *m_pTextureInfos)
	{
		if (Info.second == TexIdx)
			return Info.first;
	}
	return "";

}
_uint Engine::NewTexture::Get_TextureIdx(string& TextureName)
{
	auto iter = m_pTextureInfos->find(TextureName);
	if (iter != m_pTextureInfos->end())
		return (iter->second);

	return 0;
}
/******************************************************* 쉐이더 바인딩 함수 *******************************************************/
ID3D11ShaderResourceView* Engine::NewTexture::Get_SRV(_uint index) const
{
	if (index >= m_iNumTextures)
		return nullptr;

	return (*m_pTextures)[index];
}

ID3D11ShaderResourceView* Engine::NewTexture::Get_SRV(string& NewTextureName, int* OutTexIdx)
{
	auto iter = m_pTextureInfos->find(NewTextureName);
	if (iter == m_pTextureInfos->end())
		return nullptr;

	if (OutTexIdx)
		*OutTexIdx = iter->second;

	return Get_SRV(iter->second);
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
NewTexture* Engine::NewTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& NewTextureFilePath)
{
	NewTexture* pInstance = new NewTexture(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(NewTextureFilePath), L"NewTexture 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::NewTexture::Clone(void* arg)
{
	NewTexture* pInstance = new NewTexture(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"NewTexture 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::NewTexture::Free()
{
	__super::Free();

	if (!m_bIsClone)
	{
		for (auto& srv : m_vecTextures)
		{
			Safe_Release(srv);
		}
	}

}
/******************************************************* 객체 반환 함수 *******************************************************/
