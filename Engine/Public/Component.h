#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL Component abstract : public Base
{
protected:
	explicit Component();
	explicit Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Component(const Component& original);
	virtual ~Component();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* arg);
	virtual _int	Update_Priority(const _float fTimeDelta);
	virtual _int	Update(const _float fTimeDelta);
	virtual _int	Update_Late(const _float fTimeDelta);
	virtual HRESULT Render(const _float fTimeDelta);

	void Set_PrototypeName(const _wstring& _protoname) { m_wstrPrototypeName = _protoname; }
	_wstring Get_PrototypeName() { return m_wstrPrototypeName; }


public:
	virtual void	Save_Data(ordered_json& pJson) {};
																	//세팅이후 넘겨줄값이있다면 설정(UI인경우 컴포넌트 Type판단이필요)
	virtual void	Load_Data(class GameObject* pOwner,ComponentData& Data) {};
	int				Get_SavePriority() { return m_iSavePriority; }
public:
#ifdef _DEBUG
	///유니티나 언리얼처럼 오브젝트 선택시 그 오브젝트의 컴포넌트의 디버그를 띄울수있도록하는 함수.
	//component map순회하면서 Render->Imgui()호출하면 쉬워요
	virtual void	Render_Imgui() {};
	void			Add_DebugRender();
	virtual void	Render_Debug(PrimitiveBatch<DirectX::VertexPositionColor>* _batch) {};
#endif // _DEBUG

protected:
	_bool m_bIsClone = { false };
	_wstring m_wstrPrototypeName = {};

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static Component* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg) = 0;

public:
	void Free() override;

	//저장할때 컴포넌트 순서지정
protected:
	int			m_iSavePriority = 1;
};

NS_END
