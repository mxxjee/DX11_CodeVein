#include "ShaderTool_Define.h"
#include "Terrain.h"
#include "GameInstance.h"

Terrain::Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext)
	:GameObject{ _pDevice, _pDeviceContext }
{
}

Terrain::Terrain(const Terrain& _Prototype)
	:GameObject(_Prototype)
{
}


HRESULT Terrain::Initialize_ProtoType(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}
HRESULT Terrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//복사할때마다 생성하는게 맞는거겠지?
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

_int Terrain::Update_Priority(_float _fTimeDelta)
{
	return 0;
}
_int Terrain::Update(_float _fTimeDelta)
{
	return 0;
}
_int Terrain::Update_Late(_float _fTimeDelta)
{
	//그릴려면 렌더러에 추가
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);

	return 0;
}

HRESULT Terrain::Render(const _float fTimeDelta)
{
	//쉐이더 렌더링 부분(나중에 빛들어가면 길어져서 따로 뺌)
	if (FAILED(Bind_Shader()))
		return E_FAIL;

	//비긴(CPU에 있는거랑 HLSL에 있는거랑 정점정보들 연결) 적용
	if (FAILED(m_pShaderCom->Begin(m_vecObjPass[0])))
		return E_FAIL;

	//공통된 쉐이더 b를 던져준다
	CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[0]), E_FAIL);

	//버퍼 바인드 리소스(장치에 버퍼,인덱스 설정)
	if (FAILED(m_pVIBuffer->Bind_Resource()))
		return E_FAIL;

	//버퍼렌더
	if (FAILED(m_pVIBuffer->Render(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}
HRESULT Terrain::Bind_Shader()
{
	//월드 세팅
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	//보이긴하는데 찌그러져 보인다 VIBuffer쪽 봐야될듯
	CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTRANSFORM::D3DTS_VIEW), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTRANSFORM::D3DTS_PROJ), E_FAIL);

	//specular위해서 캠위치 던진다
	CHECK_FAILED(m_pGameInstance->Bind_CameraPosition(m_pShaderCom, "g_vCamPosition"), E_FAIL);

	//빛정보 가져온다
	//const LIGHT_DESC* Desc = m_pGameInstance->Get_LightDesc(0);
	//if (Desc == nullptr)
	//	return E_FAIL;

	//여기 빛 던져준다
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_DirectLight", &Desc->vLightDirect, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_DiffuseLight", &Desc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_AmbientLight", &Desc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_SpecularLight", &Desc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//if(FAILED(m_pGameInstance->Bind))
	//텍스쳐 세팅
	//이게 내기억에는 2개를 동시에 넘겨주는거였는데 흠.. 이름이 이러면 어캄? 저기도 배열로? 흠..
	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	//애를 안하는데?
	/*if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_MaskTexture", 0)))
		return E_FAIL;*/

	return S_OK;
}
HRESULT Terrain::Ready_Component()
{
	//게임오브젝트에서 복사생성
	if (FAILED(__super::Add_Component(_UINT(LEVEL::LOGO), Proto_Com_VITerrain,
		L"Com_VIBuffer", reinterpret_cast<Component**>(&m_pVIBuffer))))
		return E_FAIL;

	//쉐이더
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXNorTex,
		L"Com_Shader", reinterpret_cast<Component**>(&m_pShaderCom))))
		return E_FAIL;

	//텍스쳐 만들기
	if (FAILED(__super::Add_Component(_UINT(LEVEL::LOGO), Proto_Texture(L"Terrain"),
		L"Com_Texture", reinterpret_cast<Component**>(&m_pTexture))))
		return E_FAIL;

	Set_VecObjPassSize();
	Set_AllPass_VecObjPass();
	//텍스쳐 만들기
	//if (FAILED(__super::Add_Component(ENUM_TO_UINT(ENUMLEVEL::EDIT), L"Prototype_Component_Texture_Terrain_Mask",
	//	L"Com_Texture_Mask", reinterpret_cast<Component**>(&m_pTexture))))
	//	return E_FAIL;

	return S_OK;
}

Terrain* Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext, LEVEL _level)
{
	Terrain* m_pInstance = new Terrain(_pDevice, _pDeviceContext);

	if (FAILED(m_pInstance->Initialize_ProtoType(_level)))
	{
		MSG_BOX("Terrain Create Failed");
		Safe_Release(m_pInstance);
	}

	return m_pInstance;
}
GameObject* Terrain::Clone(void* pArg)
{
	Terrain* m_pInstance = new Terrain(*this);

	if (FAILED(m_pInstance->Initialize(pArg)))
	{
		MSG_BOX("Terrain Clone Failed");
		Safe_Release(m_pInstance);
	}

	return m_pInstance;
}
void Terrain::Free()
{
	__super::Free();
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTexture);
	//Safe_Delete_Array(m_pTexture);
}