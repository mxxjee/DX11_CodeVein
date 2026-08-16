#include "Engine_Define.h"
#include "Material.h"

#include "Model.h"
#include "Shader.h"
#include "GameInstance.h"

// HDR파일(이미지 읽기용) 헤더
#include <DirectXTex/DirectXTex.h>

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Material::Material()
{
}

Engine::Material::Material(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

Engine::Material::~Material()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Material::Initialize_FBX(const aiMaterial* _aiMaterial, const _wstring& _modelFilePath, myMaterial* _myMaterial)
{
    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        bool bLinear = false;
        if (i == aiTextureType_DIFFUSE)
        {
            bLinear = true;
        }

        _uint numTextures = _myMaterial->mNumTexture[i] = _aiMaterial->GetTextureCount(CAST(aiTextureType)(i));
        if (numTextures <= 0)
            continue;

        // 모델의 경로를 추출
        _tchar drive[MAX_PATH] = {};
        _tchar dir[MAX_PATH] = {};
        _tchar modelext[MAX_PATH] = {};
        _wsplitpath_s(_modelFilePath.c_str(), drive, MAX_PATH, dir, MAX_PATH, nullptr, 0, modelext, MAX_PATH);

        for (_uint j = 0; j < numTextures; j++)
        {
            aiString textureFilePath;
            MSG_FAIL(_aiMaterial->GetTexture(CAST(aiTextureType)(i), j, &textureFilePath),
                L"마테리얼에 등록된 텍스쳐를 읽어오는데 실패했습니다.", L"로드 실패", E_FAIL);

            ID3D11ShaderResourceView* SRV = { nullptr };

            if (textureFilePath.data[0] == '*' && g_toolType == TOOLTYPE::SHADER_TOOL)
            {
                COUT("임베디드 텍스쳐");
            }
            else
            {
                _tchar fullpath[MAX_PATH] = {};
                _tchar filename[MAX_PATH] = {};
                _tchar ext[MAX_PATH] = {};
                MultiByteToWideChar(CP_ACP, 0, textureFilePath.data, textureFilePath.length, fullpath, MAX_PATH);
                _wsplitpath_s(fullpath, nullptr, 0, nullptr, 0, filename, MAX_PATH, ext, MAX_PATH);

                if (g_toolType != TOOLTYPE::TOOL_END)//g_toolType == TOOLTYPE::SHADER_TOOL
                {
                    COUT("텍스쳐" << i);
                    COUT(j << "번째" << wstringToString(fullpath).c_str());
                    
                }

                // 바이너리 파일에 정보를 저장
                lstrcpy(fullpath, drive);
                lstrcat(fullpath, dir);
                lstrcat(fullpath, filename);
                lstrcat(fullpath, ext);

                _myMaterial->mTexture[i].push_back(fullpath);

                // ResourceManager::Load_Texture를 통해 텍스처 로드 (중복 캐싱 + 확장자 분기 통합)
                SRV = m_pGameInstance->Load_Texture(fullpath, bLinear);

                if (SRV == nullptr)
                {
                    _wstring name(filename);
                    _wstring message = L"경로에 등록된 텍스쳐" + name + L"\n를 읽어오는데 실패했습니다.";
                    MSG_ON(message.c_str(), L"로드 실패");
                    BREAK;
                    return E_FAIL;
                }

                m_vecFilePath.push_back(fullpath);
            }

            m_vecSRVs[i].push_back(SRV);
        }
    }

    return S_OK;
}

HRESULT Engine::Material::Initialize_Binary(myMaterial* _myMaterial, const _wstring& _modelFilePath)
{
    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        bool bLinear = false;
        if (i == aiTextureType_DIFFUSE)
        {
            bLinear = true;
        }

        _uint numTextures = _myMaterial->mNumTexture[i];

        for (_uint j = 0; j < numTextures; j++)
        {
            // 바이너리 파일에서 마테리얼의 등록 정보를 가져옴(경로, 파일명)
            _wstring mtrlFullpath = _myMaterial->mTexture[i][j];

            _tchar filename[MAX_PATH];
            _tchar fileext[MAX_PATH];

            _wsplitpath_s(mtrlFullpath.c_str(), nullptr, 0, nullptr, 0, filename, MAX_PATH, fileext, MAX_PATH);

            // 모델이 있는 경로를 추출
            _tchar modelDrive[MAX_PATH]{};
            _tchar modelDir[MAX_PATH]{};

            _wsplitpath_s(_modelFilePath.c_str(), modelDrive, MAX_PATH, modelDir, MAX_PATH, nullptr, 0, nullptr, 0);
            if (g_toolType != TOOLTYPE::TOOL_END)// if(g_toolType == TOOLTYPE::SHADER_TOOL)
            {
                cout << "텍스쳐" << i << endl;
                cout << j << "번째" << wstringToString(mtrlFullpath).c_str() << endl;
            }

            _tchar mtrlFilePath[MAX_PATH]{};

            lstrcpy(mtrlFilePath, modelDrive);
            lstrcat(mtrlFilePath, modelDir);
            lstrcat(mtrlFilePath, filename);
            lstrcat(mtrlFilePath, fileext);

            ID3D11ShaderResourceView* SRV = { nullptr };

            ////바이너리화 한 파일에 등록되어있는 마테리얼의 확장자가 PNG일때 
            ////같은 이름의 DDS를 찾아서 우선으로 등록하고 없으면 PNG를 탐색 
            //if (!lstrcmpi(fileext, L".png"))
            //{
            //    _tchar ddsPath[MAX_PATH]{};
            //    lstrcpy(ddsPath, modelDrive);
            //    lstrcat(ddsPath, modelDir);
            //    lstrcat(ddsPath, filename);
            //    lstrcat(ddsPath, L".dds");
            //    
            //    hr = CreateDDSTextureFromFile(m_pDevice, ddsPath, nullptr, &SRV);

            //    if (FAILED(hr))
            //    {
            //        hr = CreateWICTextureFromFile(m_pDevice, mtrlFilePath, nullptr, &SRV);
            //    }
            //}
            //else if (!lstrcmp(fileext, L".dds"))
            //{
            //    hr = CreateDDSTextureFromFile(m_pDevice, mtrlFilePath, nullptr, &SRV);
            //}
            //else if (!lstrcmp(fileext, L".tga"))
            //{
            //    MSG_ON(L"마테리얼이 tga파일을 로드합니다.", L"로드 실패");
            //    BREAK;
            //    return E_FAIL;
            //}
            //else if (!lstrcmp(fileext, L".hdr"))
            //{
            //    ScratchImage image; // 이미지 데이터를 담을 컨테이너
            //    hr = LoadFromHDRFile(mtrlFilePath, nullptr, image); // 경로에 있는 이미지를 담고
            //    if (SUCCEEDED(hr))
            //    {
            //        // SRV를 만들어준다
            //        CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &SRV);
            //    }
            //}
            //else
            //{
            //    hr = CreateWICTextureFromFile(m_pDevice, mtrlFilePath, nullptr, &SRV);
            //}

            SRV = m_pGameInstance->Load_Texture(mtrlFilePath, bLinear);

            if (SRV == nullptr)
            {
                _wstring name(filename);
                _wstring message = L"경로에 등록된 텍스쳐" + name + L"\n를 읽어오는데 실패했습니다.";
                MSG_ON(message.c_str(), L"로드 실패");
                BREAK;
                return E_FAIL;
            }

            m_vecFilePath.push_back(mtrlFilePath);
            m_vecSRVs[i].push_back(SRV);
        }
    }

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Material::Bind_ShaderResources(Shader* shader, const _string& _constancename, aiTextureType _texturetype, const _uint _textureindex, _uint* bitFlag)
{
	if (m_vecSRVs[_texturetype].empty())
	{
		//COUT(TextureTypeToString(_texturetype) << " 마테리얼은 비어있습니다.");
        
        //shader->Bind_SRV(_constancename, nullptr); //텍스쳐가 없으면 nullptr(0,0,0,0)값을던진다
		return S_OK;
	}
    //있으면 비트 플래그 증가
    if(bitFlag != nullptr)
        *bitFlag |= (1 <<_UINT(_texturetype));
	return shader->Bind_SRV(_constancename, m_vecSRVs[_texturetype][_textureindex]);
}

HRESULT Engine::Material::Bind_ShaderResources_FullSlot(Shader* _shader, _uint _slotNum, aiTextureType _texturetype, const _uint _textureindex, _uint* bitFlag)
{
    if (m_vecSRVs[_texturetype].empty())
    {
        //COUT(TextureTypeToString(_texturetype) << " 마테리얼은 비어있습니다.");

        //shader->Bind_SRV(_constancename, nullptr); //텍스쳐가 없으면 nullptr(0,0,0,0)값을던진다
        return S_OK;
    }

    //있으면 비트 플래그 증가
    if (bitFlag != nullptr)
        *bitFlag |= (1 << _UINT(_texturetype));
    
    return _shader->Bind_SRV_FullSlot(_slotNum, m_vecSRVs[_texturetype][_textureindex]);
}

_string Engine::Material::TextureTypeToString(aiTextureType _texturetype)
{
	_string aitextureType = {};
	switch (_texturetype)
	{
	case aiTextureType_NONE: aitextureType = "aiTextureType_NONE"; break;
	case aiTextureType_DIFFUSE: aitextureType = "aiTextureType_DIFFUSE"; break;
	case aiTextureType_SPECULAR: aitextureType = "aiTextureType_SPECULAR"; break;
	case aiTextureType_AMBIENT: aitextureType = "aiTextureType_AMBIENT"; break;
	case aiTextureType_EMISSIVE: aitextureType = "aiTextureType_EMISSIVE"; break;
	case aiTextureType_HEIGHT: aitextureType = "aiTextureType_HEIGHT"; break;
	case aiTextureType_NORMALS: aitextureType = "aiTextureType_NORMALS"; break;
	case aiTextureType_SHININESS: aitextureType = "aiTextureType_SHININESS"; break;
	case aiTextureType_OPACITY: aitextureType = "aiTextureType_OPACITY"; break;
	case aiTextureType_DISPLACEMENT: aitextureType = "aiTextureType_DISPLACEMENT"; break;
	case aiTextureType_LIGHTMAP: aitextureType = "aiTextureType_LIGHTMAP"; break;
	case aiTextureType_REFLECTION: aitextureType = "aiTextureType_REFLECTION"; break;
	case aiTextureType_BASE_COLOR: aitextureType = "aiTextureType_BASE_COLOR"; break;
	case aiTextureType_NORMAL_CAMERA: aitextureType = "aiTextureType_NORMAL_CAMERA"; break;
	case aiTextureType_EMISSION_COLOR: aitextureType = "aiTextureType_EMISSION_COLOR"; break;
	case aiTextureType_METALNESS: aitextureType = "aiTextureType_METALNESS"; break;
	case aiTextureType_DIFFUSE_ROUGHNESS: aitextureType = "aiTextureType_DIFFUSE_ROUGHNESS"; break;
	case aiTextureType_AMBIENT_OCCLUSION: aitextureType = "aiTextureType_AMBIENT_OCCLUSION"; break;
	case aiTextureType_UNKNOWN: aitextureType = "aiTextureType_UNKNOWN"; break;
	case aiTextureType_SHEEN: aitextureType = "aiTextureType_SHEEN"; break;
	case aiTextureType_CLEARCOAT: aitextureType = "aiTextureType_CLEARCOAT"; break;
	case aiTextureType_TRANSMISSION: aitextureType = "aiTextureType_TRANSMISSION"; break;
	case aiTextureType_MAYA_BASE: aitextureType = "aiTextureType_MAYA_BASE"; break;
	case aiTextureType_MAYA_SPECULAR: aitextureType = "aiTextureType_MAYA_SPECULAR"; break;
	case aiTextureType_MAYA_SPECULAR_COLOR: aitextureType = "aiTextureType_MAYA_SPECULAR_COLOR"; break;
	case aiTextureType_MAYA_SPECULAR_ROUGHNESS: aitextureType = "aiTextureType_MAYA_SPECULAR_ROUGHNESS"; break;
	case aiTextureType_ANISOTROPY: aitextureType = "aiTextureType_ANISOTROPY"; break;
	case aiTextureType_GLTF_METALLIC_ROUGHNESS: aitextureType = "aiTextureType_GLTF_METALLIC_ROUGHNESS"; break;
	}

	return aitextureType;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 패스 선택 함수 ////////////////////////////////////////////////////////
_int Engine::Material::ShaderPass_Auto_Selection_M()
{
    _int iPassNum = {};
    //한개면 투명pass로
    if (!m_vecSRVs[aiTextureType_DIFFUSE].empty())
    {
        iPassNum = 0; //일단 0번패스
    }
    if (!m_vecSRVs[aiTextureType_DIFFUSE].empty() && !m_vecSRVs[aiTextureType_NORMALS].empty() && !m_vecSRVs[aiTextureType_SHININESS].empty())
    {
        iPassNum = 3; //더추가?
    }

    return iPassNum;
}
/******************************************************* 쉐이더 패스 선택 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Material* Engine::Material::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* _aiMaterial, const _wstring& _modelFilePath, myMaterial* _myMaterial)
{
	Material* pInstance = new Material(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_FBX(_aiMaterial, _modelFilePath, _myMaterial), L"Material 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Material* Engine::Material::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, myMaterial* _myMaterial, const _wstring& _modelFilePath)
{
    Material* pInstance = new Material(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Binary(_myMaterial, _modelFilePath), L"Material 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Material::Free()
{
	__super::Free();

    for (auto& path : m_vecFilePath)
    {
        m_pGameInstance->Release_Texture(path);
    }

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		for (auto& srv : m_vecSRVs[i])
		{
			Safe_Release(srv);
		}
		m_vecSRVs[i].clear();
	}


}
/******************************************************* 객체 반환 함수 *******************************************************/
