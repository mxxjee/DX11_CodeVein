#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Material final : public Base
{
private:
	explicit Material();
	explicit Material(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Material();

public:
	HRESULT Initialize_FBX(const aiMaterial* _aiMaterial, const _wstring& _modelFilePath, myMaterial* _myMaterial);
    HRESULT Initialize_Binary(myMaterial* _myMaterial, const _wstring& _modelFilePath);
    
    
	HRESULT Bind_ShaderResources(class Shader* shader, const _string& _constancename, aiTextureType _texturetype, const _uint _textureindex, _uint* bitFlag = nullptr);
	HRESULT Bind_ShaderResources_FullSlot(class Shader* shader, _uint _constancename, aiTextureType _texturetype, const _uint _textureindex, _uint* bitFlag = nullptr);
	_int ShaderPass_Auto_Selection_M();

private:
	_string TextureTypeToString(aiTextureType _texturetype);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*> m_vecSRVs[AI_TEXTURE_TYPE_MAX];
	class GameInstance* m_pGameInstance = { nullptr };
	_bool m_bClone = false;
	vector<_wstring> m_vecFilePath;

public:
	static Material* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* _aiMaterial, const _wstring& _modelFilePath, myMaterial* _myMaterial);
    static Material* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, myMaterial* _myMaterial, const _wstring& _modelFilePath);

public:
	void Free() override final;

};

NS_END
