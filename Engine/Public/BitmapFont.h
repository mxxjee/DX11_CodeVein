#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class GameInstance;
class NewTexture;

/*png사진의 데이터파싱해서 그릴영역을 정의하자*/
class ENGINE_DLL BitmapFont :
    public Base
{
private:
    explicit BitmapFont();
    explicit BitmapFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~BitmapFont();

public:
            //fnt경로와 텍스처 폴더명/이름 
    HRESULT Initialize(const _wstring& strFntPath, _string& FontTextureName);


    //외부에서 이 폰트의 글자를 찾을때
    const BM_CHARACTER* Get_Character(int ch) const;
    ID3D11ShaderResourceView* Get_SRV() {return m_pSRV;}
    float   Get_LineHeight() const { return m_fLineHeight; }

public:
    static BitmapFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFntPath, _string& FontTextureName);

public:
    void Free() override final;

private:
    NewTexture* m_pTextureCom = nullptr;
    int                 m_TexIdx = 0;
    ID3D11ShaderResourceView* m_pSRV = nullptr;
    unordered_map<int, BM_CHARACTER>     m_Characters;//이 비트맵 폰트가 가지고있는 숫자 id와 자를범위 값들.

#pragma region parsing Data
private:
        //A=B 형태에서 B를 추출
    int         Parse_Value(const string& line, const string& key);
private:
    float                           m_fLineHeight = 0.f;

#pragma endregion

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    GameInstance* m_pGameInstance = { nullptr };


};

NS_END

