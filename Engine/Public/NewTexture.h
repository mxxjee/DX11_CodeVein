#pragma once
#include "Component.h"

NS_BEGIN(Engine)

/*원래 Texture컴포넌트와 차이점 
//1.복사생성자에서 얕은복사를 수행(포인터참조)
//2. 파일 상위경로넣으면 재귀함수로 다 읽어주는  함수 추가
//3. 멤버로 <string,_uint> 추가

*/
class ENGINE_DLL NewTexture final:
    public Component
{
private:
	explicit NewTexture();
	explicit NewTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit NewTexture(const NewTexture& original);
	virtual ~NewTexture();

public:
            //상대경로 넣으면 거기서부터 하위폴더까지 싹 읽으면서 모든 텍스처읽어옴
	HRESULT Initialize_Prototype(const _wstring& FolderPath);
	HRESULT Initialize(void* arg);

    /* 쉐이더 컴포넌트, 값을 넘길 변수 이름, 텍스쳐 번호 */
    HRESULT Bind_ShaderResource(class Shader* shader, const _string& constantname, _uint textureindex = 0);
    /* 쉐이더 컴포넌트, 값을 넘길 변수 이름, 텍스쳐 번호 */
    HRESULT Bind_ShaderResources(Shader* shader, const _string& constantname);
    /* 특정 범위의 텍스처만 바인딩 */
    HRESULT Bind_ShaderResources_Range(Shader* shader, _uint slot, _uint startIndex, _uint count, _uint stageMask = stage_PS);
    /* 슬롯 직접 지정하여 단일 텍스처 바인딩 */
    HRESULT Bind_ShaderResource_FullSlot(Shader* shader, _uint slot, _uint textureIndex, _uint stageMask = stage_PS);
    /* 슬롯 직접 지정하여 전체 텍스처 바인딩 */
    HRESULT Bind_ShaderResources_FullSlot(Shader* shader, _uint slot, _uint stageMask = stage_PS);
    /* 슬롯 직접 지정하여 범위 텍스처 바인딩 */
    HRESULT Bind_ShaderResources_Range_FullSlot(Shader* shader, _uint slot, _uint startIndex, _uint count, _uint stageMask = stage_PS);
    /* 특정 인덱스의 SRV 반환 */
    ID3D11ShaderResourceView* Get_SRV(_uint index) const;

public:
                //텍스처이름을 넣으면 이 텍스처컴포넌트의 vector의 몇번쨰있는지 out으로리턴해줌
    ID3D11ShaderResourceView* Get_SRV(string& TextureName,int* OutTexIdx);
    string                    Get_TextureName(int TexIdx);
    inline _uint                   Get_NumTextures() { return m_iNumTextures; }
    _uint                       Get_TextureIdx(string& TextureName);

private:
    _uint								m_iNumTextures = {};

    vector<ID3D11ShaderResourceView*>	m_vecTextures = {}; //실제 srv(원본만소유함)
    const vector<ID3D11ShaderResourceView*>* m_pTextures = nullptr;

private:
    unordered_map<string,_uint >     m_TextureInfo;     //(원본)텍스처의 파일이름,벡터 인덱스
    const unordered_map<string, _uint >*     m_pTextureInfos;     //(사본) 원본꺼 ptr

public:
    //상대경로 넣으면 거기서부터 하위폴더까지 싹 읽으면서 모든 텍스처읽어옴
    static NewTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& textureFilePath);
    virtual Component* Clone(void* arg);

public:
    void Free() override final;
};

NS_END

