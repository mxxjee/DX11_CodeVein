#pragma once
#include "UI_Render.h"


NS_BEGIN(Engine)
class NewTexture;
class VIBuffer_Rect;
class Transform;
class Mouse;


/*New texture가 가진 데이터개수와 현재 스크롤상태조합*/
struct GRID_DESC :public UI_Render::UICOMPDESC
{
	wstring ProtoNewTexKey = L"Prototype_Component_UITexture_CustomizeResource";
	_int iNumColums = 4;//가로 칸 개수
	_int iVisibleRows = 5;	//화면에 보일 세로 칸개수
	_float2 vSlotSize = { 50.f,50.f };	//하나의 슬롯크기
	_float2 vSpacing = { 10.f,10.f };//간격
	_float2 vStartPos = { 0.f,0.f };	//그리드 시작점(로컬)
};

class ENGINE_DLL UI_SlotGrid :
	public UI_Render
{

public:
	struct SLOT_ZONE
	{
		_float2 vWorldPos;
		_uint iDataIdx = 0;
	};

protected:
	explicit UI_SlotGrid();
	explicit UI_SlotGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UI_SlotGrid(const UI_SlotGrid& original);
	virtual ~UI_SlotGrid();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	virtual _int	Update(const _float fTimeDelta);

public:
	static UI_SlotGrid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg);

	virtual			HRESULT		Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket);

	HRESULT			Change_TargetTexture(wstring ProtoTexKey,_uint ProtoLevel,void* pArg, _uint TotalCount=9999);
	void			Set_FocusIdx(_uint i) { m_iFocusIdx = i; }
	_uint			Get_FocusIdx_On_Click(bool bUpdate=true);


	_uint			Get_FocusIdx_On_Hover();
	
#ifdef _DEBUG
	virtual void		Render_Imgui();
#endif

#pragma region parsing
public:
	virtual void	Save_Data(ordered_json& pJson) override;
	virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion

public:
	void Free() override;


public:
	void		Rebuild(_int iTotalCount);		//NewTexture할당 후나 다시재할당 시 호출
	_float2		Scroll_Wheel(_int iStep);	//마우스 휠 입력시 호출


	_uint	Get_BeginDataIdx() { return m_iCurrentRow * m_tGridDesc.iNumColums; }

	//iViewIdx번째의 로컬좌표를반환
	_float2 Get_SlotLocalPos(_uint iViewIdx) {
		_uint iCol = iViewIdx % m_tGridDesc.iNumColums;
		_uint iRow = iViewIdx / m_tGridDesc.iNumColums;
		return _float2(
			m_tGridDesc.vStartPos.x + iCol * (m_tGridDesc.vSlotSize.x + m_tGridDesc.vSpacing.x),
			m_tGridDesc.vStartPos.y + iRow * -(m_tGridDesc.vSlotSize.y + m_tGridDesc.vSpacing.y)
		);
	}

#ifdef _DEBUG
	virtual void		Render_Debug();
#endif // _DEBUG

protected:
	NewTexture* m_pTextureCom = { nullptr };
	VIBuffer_Rect* m_pVIBufferCom = { nullptr };


private:
	GRID_DESC		m_tGridDesc;
	_int			m_iTotalCount = 0;	//전체 데이터개수(텍스처개수랑같음?)
	_int			m_iCurrentRow = 0;	//현재 화면에 표시되고있는 시작줄



	_uint			m_iMaxRows;//최대 줄? max이후로는 row증가불가(clamp용)
	_uint			m_iFocusIdx = -1;	//마우스호버/선택된 인덱스
	_uint			m_iHoverIdx = -1;


	Transform* m_pTransform = nullptr;
	_long dwWheel = 0l;				//마우스 휠 이동량
	_uint iProtoLevel = 0;
private:
	_wstring		m_Proto_NewTexKey = L"";
	Mouse* m_pMouse = nullptr;
	vector < SLOT_ZONE>	m_SlotZones;


};

NS_END