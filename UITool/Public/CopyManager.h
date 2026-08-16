#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class UIObject;
class GameInstance;

NS_END


NS_BEGIN(UITool)

class Sample_Pars_UItool;

class CopyManager :
    public Base
{
	DECLARE_SINGLETON(CopyManager);

protected:
	explicit CopyManager();
	explicit CopyManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CopyManager();

public:
	virtual HRESULT Initialize();

	_int	Update_Late(const _float fTimeDelta);

	//Ctrl+c
	void		Set_CopyObject(UIObject* pObj);


	//Ctrl+v
	void		Paste_CopyObject();


	void		Set_Parser(Sample_Pars_UItool* pTool) { m_pParser = pTool; }
public:
	static CopyManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
public:
	void Free() override;

private:
	GameInstance* m_pGameInstance = nullptr;
	UIObjectInfo	m_SaveInfo;
	Sample_Pars_UItool* m_pParser;

	static int CopyNumber;

};
NS_END

