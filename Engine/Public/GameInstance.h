#pragma once

#include "PrototypeManager.h"

NS_BEGIN(Engine)

class EventManager;
class GameObject;
struct FileHeader;
enum class RenderTargets;
enum class MRT;
class GameClock;

class ENGINE_DLL GameInstance final : public Base
{
	DECLARE_SINGLETON(GameInstance)

private:
	explicit GameInstance();
	virtual ~GameInstance();

public:
	HRESULT Initialize_Device(HWND _hWnd, WINMODE _isWindowed, _float _wincx, _float _wincy, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	HRESULT Initialize(ENGINE_DESC _tDesc);

	void Update_Level(const _float fTimeDelta);

	void Update_Priority(const _float fTimeDelta);
    void Update_Parallel(const _float fTimeDelta);
	void Update(const _float fTimeDelta);
	void Update_Late(const _float fTimeDelta);

    HWND        Get_Hwnd() { return m_hWnd; }

    void Set_Winsize(_float _wincx = 0, _float _wincy = 0) {
        if (_wincx != 0) m_fWincx = _wincx;
        if (_wincy != 0) m_fWincy = _wincy;
    }
    void Get_Winsize(_float* _wincx, _float* _wincy) const {
        if(_wincx) *_wincx = m_fWincx; 
        if(_wincy) *_wincy = m_fWincy;
    }

    MyRay Get_LocalRay(const _fmatrix& _worldMatrix);
    _bool RayTriangleIntersect(const _vector& rayOrigin, const _vector& rayDir,
        const _vector& v0, const _vector& v1, const _vector& v2,
        _float& t, _float& u, _float& v);
    _bool IntersectBuffer(const MyRay& _localRay, VIBuffer* _buffer, const _fmatrix& _worldmatrix
        ,_vector& _outworld, _float& _outDist, _uint* _outFaceIndex = nullptr, _float* _outU = nullptr, _float* _outV = nullptr);

    _string Make_SavefilePath(const _char* _filter = "All Files\0*.*\0", const _char* _defaultExt = nullptr);
    _string Make_OpenfilePath(const _char* _filter = "All Files\0*.*\0");
    HRESULT CreateStructuredBuffer(const void* _data, _uint _elementCount, _uint _structureStride, ID3D11Buffer** _outBuffer, ID3D11ShaderResourceView** _outSRV);
    
    void        Set_EngineMode(ENGINEMODE eMode) { m_eEngineMode = eMode; }
    ENGINEMODE  Get_EngineMode() { return m_eEngineMode; }

    void Load_Folder(_wstring& _folderPath, _uint _level, _matrix _prematrix = XMMatrixIdentity(), _wstring _ext = L".siho");


#pragma region GraphicDevice
	HRESULT Draw_Begin(const _float4* pClearColor);
	HRESULT Draw(const _float fTimeDelta);
	HRESULT Draw_End();
	HRESULT ResizeBuffers(_int _windowWidth, _int _windowHeight, HWND& _hWnd);
    ID3D11RenderTargetView* Get_BackBufferRTV();
    ID3D11DepthStencilView* Get_DepthStencilView();
    void Get_Viewport(D3D11_VIEWPORT* _VP) { *_VP = m_Viewport; }
    HRESULT Swap_OriginalBackbuffer();
#pragma endregion


#pragma region TimerManager
	/* 타이머 이름을 받아 타이머를 추가 */
	HRESULT Add_Timer(const _wstring& _timername);
	/* 타이머 이름을 받아 델타타임을 업데이트 */
	_float	Compute_Timer(const _wstring& _timername);
	/* 타이머 이름을 받아 델타타임을 받아오기 */
	_float	Get_Time(const _wstring& _timername);

    GameClock* Add_Clock(const _wstring& _clockName, _float _targetFPS);
    GameClock* Get_Clock(const _wstring& _clockName);
#pragma endregion TimerManager 


#pragma region LevelManager
	/* 레벨의 이름과 레벨을 받아와서 vector컨테이너에 등록 */
	HRESULT Add_Level(_uint _levelname, class Level* level);
    HRESULT Add_PlayerLevel(class Level* level);
	/* 등록된 레벨로 변경 */
	HRESULT Change_Level(_uint _levelname);
	/* 레벨 삭제 */
	HRESULT Delete_Level(_uint _levelname);
    /* 현재 Update중인 레벨 획득 */
    class Level* Get_Current_Level();

    /* 현재 Update중인 레벨아이디 획득 */
    _uint Get_Current_LevelID();

    /* 생성되어 있고, 삭제되지 않은 Level을 ID로 찾기 */
    class Level* Get_Level(_uint _levelID);


#pragma endregion LevelManager


#pragma region PrototypeManager
	/// <summary>
	/// 특정 레벨의, 특정 이름의 프로토타입을 추가
	/// </summary>
	/// <param name="_levelID"> 레벨의 ID </param>
	/// <param name="prototypename"> 프로토타입의 이름 </param>
	/// <param name="pPrototype"> 완성된 프로토타입 </param>
	/// <returns></returns>
	HRESULT Add_Prototype(const _uint _levelID, const _wstring& prototypename, class Base* pPrototype);

	/// <summary>
	/// 특정 레벨의, 컴포넌트 or 게임오브젝트 형태의 프로토타입을, 이름으로, 내부 설정을 완료 한 후 복사해서 반환
	/// </summary>
	/// <param name="_prototypeID"> 컴포넌트 or 게임오브젝트 </param>
	/// <param name="_levelID"> 레벨 ID </param>
	/// <param name="prototypename"> 프로토타입 이름 </param>
	/// <param name="pArg"> 객체 설정 </param>
	/// <returns></returns>
	class Base* Clone_Prototype(const PROTOTYPE _prototypeID, const _uint _levelID, const _wstring& prototypename, void* pArg = nullptr);

    unordered_map<_wstring, class Base*>* Get_Prototypes();

    HRESULT Clear_Level_Prototype(_uint _levelID);
#pragma endregion PrototypeManager


#pragma region GameObjectManager

	/// <summary>
	/// 게임오브젝트를 특정 레벨의 특정 레이어에 추가하는 함수
	/// </summary>
	/// <param name="_prototypeLevelID"> 복사할 객체가 있는 프로토타입의 레벨ID </param>
	/// <param name="_prototypename"> 복사할 프로토타입의 이름 </param>
	/// <param name="_levelID"> 추가할 레벨의 ID </param>
	/// <param name="_layername"> 추가할 레이어의 이름 </param>
	/// <param name="pArg"> 변수 설정값 </param>
	/// <returns></returns>
	HRESULT Add_GameObject_To_Layer(const _uint _prototypeLevelID, const _wstring& _prototypename, const _uint _levelID, const _wstring& _layername,
        GameObject** _outobject = nullptr, void* pArg = nullptr);

    //이미 생성한 객체를 단지 레이어에 추가.
    HRESULT Add_GameObject(GameObject* pGameObject, const _uint _levelID, const _wstring& _layername);

	/// <summary>
	/// 특정 레벨의, 특정 레이어에, 특정 이름의 오브젝트 검색 및 반환
	/// </summary>
	/// <param name="_levelID"> 레벨 ID </param>
	/// <param name="_layername"> 레이어 이름 </param>
	/// <param name="_gameobjectname"> 게임오브젝트 이름 </param>
	/// <returns></returns>
	class GameObject* Get_GameObject(const _uint _levelID, const _wstring& _layername, const _wstring& _gameobjectname) const;

    unordered_map<_wstring, class Layer*>& Get_Current_Layers();
    class Layer* Get_Layer(const _wstring& _layerName);

    HRESULT Clear_Level_GameObject(_uint _levelID);
    void Set_Parallel_Dirty();
#pragma endregion GameObjectManager


#pragma region Renderer
    void Render_Grid();
	HRESULT Add_RenderObject(RENDER_GROUP _rendergroup, class GameObject* _gameobject);
	HRESULT Clear_Renderer();
#ifdef _DEBUG
    void Add_Debug_Sphere(BoundingSphere _sphere, _float3 _color = _float3{ 1.f, 1.f, 0.f });
    void Add_Debug_Capsule(CAPSULE_DESC _capsule);
    void Add_Debug_Capsule(_float3& _center, _float _radius, _float _halfHeight, _float4& _quaternion, _float3& _color);
    void Add_Debug_LookLine(_float3& _pos, _float3& _target, _float4& _color);
    void Add_Debug_Fan(const DebugFan& _fan);
#endif // _DEBUG

    /* 마우스의 위치를 받아와서 오브젝트의 ID를 받아오는 함수 */
	//_uint	Picking_Object(_int _mousex, _int _mousey);
    
    /* 옥트리 */
    // 옥트리 생성 + 초기화(레벨 시작 시)
    HRESULT Build_OcTree(const _float3& _worldCenter, _float _worldHalfSize, _uint _maxDepth);

    // 정적 오브젝트 OcTree에 추가(레벨 로드할 때 한 번만 호출{등록}함)
    HRESULT Add_StaticObject(RENDER_GROUP _group, class GameObject* _staticobject, const BoundingBox& _worldAABB);
    HRESULT Add_Object_To_Octree(RENDER_GROUP _group, class GameObject* _staticobject, const BoundingBox& _worldAABB);
    // 정적 오브젝트 개별 삭제용
    HRESULT Remove_StaticObject(GameObject* _staticObject);

    /* 범위 쿼리: 특정 AABB와 겹치는 오브젝트들 반환 */
    void Query_AABB(const BoundingBox& _aabb, vector<const STATIC_OBJ_INFO*>& _outInfos) const;


    // 옥트리 정리
    void Clear_OcTree();

    //그림자 설정 조절용
    void Set_HDROption(_float4 _vHDROption);
    void Set_SSAOOption(_float4 _vSSAOOption);
    //칼라그레이딩 + 포그뎁스 + PBR 
    void Set_ShaderDesc(ShaderDesc _vShaderDesc);
    //맵변경용
    void ChangeMap(_float _t);
    void ChangeBaseMap();
    void ChangeCaveMap(_float _t);
    void ChangeCustomize(); //커마씬
    void ChangeMainMap(); //처음시작(동굴)
    void ChangeChurch();
    void ChangeChurchBoss();
    void ChangeEnd();

    void Set_Capture(_bool _bCapture);

    //플레이어 스킨컬러가져오기
    _float4 Get_PlayerSkinColor();
    void Set_PlayerSkinColor(_float3 vColor);

#pragma endregion Renderer


#pragma region PipeLine
	void	Set_Transform(D3DTRANSFORM eD3DTransform, _matrix matrix);
    
    [[deprecated("이 함수는 레거시 함수입니다 Matrix_View, Proj 함수를 사용해주십시오.")]]
	HRESULT Bind_PipeLineMatrix(class Shader* shader, const _string& constant, D3DTRANSFORM transform);
    HRESULT Bind_PipeLineMatrix_View(Shader* _shader);
    HRESULT Bind_PipeLineMatrix_Proj(Shader* _shader);
    [[deprecated("이 함수는 레거시 함수입니다 Matrix_View, Proj 함수를 사용해주십시오.")]]
    HRESULT Bind_PipeLineInverseMatrix(Shader* shader, const _string& constant, D3DTRANSFORM transform);
    HRESULT Bind_PipeLineInverseMatrix_View(Shader* _shader);
    HRESULT Bind_PipeLineInverseMatrix_Proj(Shader* _shader);
	HRESULT Bind_CameraPosition(Shader* shader, const _string& constant = "");
    // Pipeline의 View, Proj, InvView, InvProj, Campos 전부 한번에 바인딩해주는 함수
    HRESULT Bind_PipeLine_All(Shader* _shader);
    void Set_CamNearFar(_float _near = 0, _float _far = 0);
    void Update_PrevMatrices();
    _float4 Get_CameraPosition();
    _float4x4 Get_PipeLineMatrix(D3DTRANSFORM _transform);
    _float4x4 Get_PrevPipeLineMatrix(D3DTRANSFORM _transform);
    _float4x4 Get_PipeLineInversMatrix(D3DTRANSFORM _transfrom);
#pragma endregion PipeLine


#pragma region InputManager
	_byte Get_DIKeyState(_ubyte byKeyID) const;
	_bool KeyPress(_ubyte byKeyID) const;
	_bool KeyDown(_ubyte byKeyID) const;
	_bool KeyUp(_ubyte byKeyID) const;

	_byte Get_DIMouseState(MOUSEKEYSTATE eMouse) const;
	_bool MousePress(MOUSEKEYSTATE eMouse) const;
	_bool MouseDown(MOUSEKEYSTATE eMouse) const;
	_bool MouseUp(MOUSEKEYSTATE eMouse) const;

	_long MouseMove(MOUSEMOVESTATE eMouseState) const;


            //단축키등록,전역으로 등록되니까 조심!!!(공통적인 기능(디버그껐다키기 등) 넣기))
    HRESULT		Register_HotKey(_ubyte Key, bool bCtrl, bool bShift, bool bAlt, HOTKEY_MODE eMode, HotKeyCallback Func);
    _bool		IsAnyKeyPressed();

#pragma endregion InputManager


#pragma region LightManager
	HRESULT Add_Light(const _uint _lightindex, const LIGHT_DESC& _lightdesc);
	HRESULT Delete_Light(const _uint _lightindex);

	LIGHT_DESC* Get_LightDesc(const _uint _lightindex);
	HRESULT Set_LightDesc(const _uint _lightindex, const LIGHT_DESC& _lightdesc);

    void Render_Lights(class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta);

    unordered_map<_uint, class Light*>* Get_LightuMapPtr();
    _int Get_LightCnt();
    class Light* Get_LightPtr(const _uint _lightindex);
#pragma endregion LightManager


#pragma region SamplerManager
	unordered_map<_string, class Sampler*> Get_Sampler_String();
	unordered_map<_uint, class Sampler*> Get_Sampler_Slot();
	class Sampler* Get_Sampler(_uint _slot);
    class Sampler* Get_Sampler(const _string& _name);
	HRESULT Bind_Sampler(_uint _slot, _uint _stageMask);
	HRESULT Bind_Sampler(const _string& _name, _uint _stageMask);
    class SamplerManager* Get_SamplerManager();
#pragma endregion SamplerManager


#pragma region Calculator
    _float RandomValue(_float _min, _float _max);
    _uint RandomValue_int(_uint _min, _uint _max);
    _vector QuaternionToDegrees(_fvector _quaternion);
#pragma endregion Calculator


#pragma region FontManager
    HRESULT Load_Font(const _wstring& _fontName, const _wstring& _fontPath);
    HRESULT Add_BitmapFont(const _wstring& _BitmapFontName, const wstring& strFntPath, _string FontTextureName);
    class BitmapFont* Get_BitmapFont(const _wstring _bitmapName);
    HRESULT    Set_PsOutLineShader();
    

    // 폰트 텍스트 즉시 출력
    void    Draw_Text(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _float& _Rotation,const _fvector _color = XMVectorSet(1.f, 1.f, 1.f, 1.f), const _float2& _size = _float2(1.f, 1.f));
    // 쌓여있는 폰트를 출력(MainApp이나 Renderer에서 하면 됨)
    void    Render_Texts_Stacked();
    void    Draw_Text_Begin();
    void    Draw_Text_Mask_Begin();
    void    Draw_Text_Alpha_Begin();


    void    Draw_BitMapText_Begin();
    void    Draw_Text_End();
    void    Draw_Text_Outline4(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color = XMVectorSet(1.f, 1.f, 1.f, 1.f), const _fvector _outlinecolor = XMVectorSet(0.f, 0.f, 0.f, 1.f));
    void    Draw_Text_Outline8(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color = XMVectorSet(1.f, 1.f, 1.f, 1.f), const _fvector _outlinecolor = XMVectorSet(0.f, 0.f, 0.f, 1.f));
    // 폰트 텍스트 한번에 그리기 위해 적립
    void    Draw_Text_Stack(const TEXT_DESC& _textdesc);
    void    Add_Text_Stack(const TEXT_DESC& _textdesc);
    void    Clear_FontManager();

    //새로추가한함수
    class MyFont* Get_Font(const _wstring& _fontName);
    SpriteBatch* Get_Batch();

    void Draw_Text_Outline_Begin(ID3D11ShaderResourceView* pFont, ID3D11Texture2D* pTex2D, float fThickness, _float4 OutLineColor, _float4 Color);
    void Set_OutlineShader(class Shader* pShader);

#ifdef _DEBUG
    vector<string>      Get_All_FontName();
    vector<string>      Get_All_BitmapFontName();
#endif // _DEBUG


#pragma endregion FontManager


#pragma region CameraManager
    // 카메라들을 관리할 매니저(레벨별로 사용할만큼 리셋해줘야함)
    void CameraManager_Reset(_uint _maxCameraCount);
    // 카메라 매니저에 추가
    HRESULT Add_Camera(_uint _prototypeLevelID, const _wstring& _cameraName, _uint _levelID, _uint _cameraNum, class GameObject** _outCamera = nullptr, void* arg = nullptr);
    void Camera_Change(_uint _cameraNum);
    void Camera_Active(_uint _cameraNum, _bool _isActive = true);
    void Update_Cameras(const _float fTimeDelta);
    // 매니저가 가지고 있는 전체 카메라 가져오기
    vector<class Camera*> Get_CameraVector();
    inline class Camera*  Get_Camera(_uint _cameraNum);
    inline class Camera* Get_CurrentCamera();
    const _float3 Get_PrevCameraPos_Float3();
    const _vector Get_PrevCameraPos_Vector();
    const _float4x4 Get_PrevCamera_Float4x4();
    const _matrix   Get_PrevCamera_Matrix();
    // 화면비 바꼈을때 카메라 비율 바꿔주기
    void Set_Aspect(_float _aspect);
    // 현재 카메라의 보고있는 방향 설정
    void Camera_LookAt(_vector _targetPosition);
#pragma endregion CameraManagerq


#pragma region RenderStateManager
    // 래스터라이저 스테이트
    void Set_RasterizerState(RSTATE _rasterrizerState);
    [[deprecated("이 함수는 레거시 함수입니다 RSSTATE을 매개변수로 받는 함수를 사용해주십시오.")]]
    void Set_RasterizerState(const _string& _name);
    // 뎁스 스텐실 스테이트
    void Set_DepthStencilState(DSSTATE _depthStencilState, _uint _stencilRef = 0);
    [[deprecated("이 함수는 레거시 함수입니다 DSSTATE을 매개변수로 받는 함수를 사용해주십시오.")]]
    void Set_DepthStencilState(const _string& _name, _uint _stencilRef = 0);
    // 블렌드 스테이트
    void Set_BlendState(BSTATE _blendState);
    [[deprecated("이 함수는 레거시 함수입니다 BSTATE을 매개변수로 받는 함수를 사용해주십시오.")]]
    void Set_BlendState(const _string& _name);

    //스테이트 가져오기
    ID3D11RasterizerState* Get_RS(const _string& _name);
    ID3D11DepthStencilState* Get_DSS(const _string& _name);
    ID3D11BlendState* Get_BS(const _string& _name);

    ID3D11RasterizerState*      Get_RS(RSSET _rasterrizerState);
    ID3D11DepthStencilState*    Get_DSS(DSSSET _depthStencilState);
    ID3D11BlendState*           Get_BS(BSSET _blendState);
#pragma endregion RenderStateManager


#pragma region RenderTargetManager
    HRESULT Add_RenderTarget(RenderTargets _targetName, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor);
    HRESULT Add_RenderTarget_Shadow(RenderTargets _targetName, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor);
    HRESULT Add_MultiRenderTarget(MRT _mrtName, RenderTargets _rtvName);
    HRESULT Bind_RT_ShaderResource_FullSlot(RenderTargets _rtvName, class Shader* _shader, _uint _slotNum, _uint _stageMask = stage_VS | stage_PS);
    HRESULT Begin_MRT(MRT _mrtName, _bool DSV = false, _bool _Clear = true);
    HRESULT Begin_MRT_HDR(MRT _mrtName, ID3D11DepthStencilView* _DSV, _bool _Clear = false);
    HRESULT Begin_RT(RenderTargets _targetName, ID3D11DepthStencilView* _DSV = nullptr);
    HRESULT Begin_RT_HDR(RenderTargets _targetName, ID3D11DepthStencilView* _DSV, _bool _Clear = false);
    ID3D11DepthStencilView** Begin_MRT_ShadowCascade(MRT _mrtName, vector<GameObject*> m_vecObjects[], _float _fTimeDelta, _int iNumDSV, RENDER_GROUP _eRGroup);
    HRESULT End_MRT();
    HRESULT Reset_MRT();
    void Copy_Resource(RenderTargets _rtvName, ID3D11Resource* pDest);
    ID3D11ShaderResourceView* Get_ShaderResourceView(RenderTargets _target);
    ID3D11RenderTargetView*   Get_RenderTargetView(RenderTargets _target);

    void Clear_RTV(MRT _mrtName);
#pragma endregion RenderTargetManager


#pragma region Shadow
    HRESULT Add_ShadowLight(const SHADOW_DESC& _shadowDesc);
    inline _float4x4 Get_LightMatrix(D3DTRANSFORM _transform);
    //Cascade그림자용
    HRESULT Add_ShadowCascadeLight(_int _iNumCascade, const SHADOW_DESC& _shadowDesc, _float _Bias[]);
    inline _float4x4* Get_ShadowCascade_Matrix(D3DTRANSFORM _transform);
    _float4& Get_ShadowCascade_Split();
    _float* Get_ShadowCascade_Bias();
    _int Get_ShadowCascade_Num();
    _float4& Get_ShadowLightDir();
    void Follow_ShadowCascade_Matrix(_vector _at);
#pragma endregion Shadow


#pragma region UIManager
    const unordered_map<size_t, UIObject*>&      Get_AllUIObjects();
    HRESULT Add_UIObject(class UIObject* _ui, bool bReAdd=false);
    void    Clear_UIManager();
    void    Delete_DeadUI();
    void    Set_Visible_All_UI(_bool _isVisible);
    void    Process_UI_Picking(const POINT& _mousePos, _bool _bClicked);
    void    Set_UIManager_Active(_bool _isActive);
    bool    Get_UIManager_Active();
    //이름 바꼈을떄 호출
    void        Rename_Object(wstring OldName,UIObject* pObj);
    UIObject*    Find_UI_ByName(const _wstring& _uiName);
    //관리대상에서 제외하는 함수
    HRESULT Remove_UIObject(class UIObject* _ui);
    HRESULT Remove_UIObject(_wstring UIName);

    class UIObject* Get_ClickedUI();
    void        Clear_ClickedUI();

    class UIObject* Get_HoveredUI();
    void        Clear_HoveredUI();

    void        Push_Window(wstring _windowName);
    void        Close_Window(wstring _windowName);
            //맨위의 창 끄기
    void        Close_Window();
    void        Close_All_Menu();
    UIObject* Get_CurrentWindow();
    bool     Is_Empty_WindowStack();
    void    Set_Force_HoverUI(UIObject* pObj);

    size_t      Get_WindowStackCount();
    UIObject*   Get_Top_At_WindowStack();

    void        Set_AllLoadPersistents(bool b);
    bool        Get_AllLoadPersistents();

    void         Clear_Scene_UI();

    void        Change_UIMode(UI_MODE eMode, bool bUseActiveAnim = false);

#pragma region Pooling
    void    Register_Pooling_Func(UIPOOLINGFUNC         Func);

    //풀링정보 등록
    void        Register_Factory(_uint eType, UIObjectInfo Info);

    //풀링정보를 통해서 미리 객체생성해놓기
    void        Add_PoolObject(_uint eType, _uint Size);

    UIObject* Get_PoolObject(_uint eType);
    void        Return_PoolObject(_uint eType, UIObject* pTarget);

    UIObjectInfo*      Get_Proto_At_UIPool(_uint eType);
#pragma endregion


#pragma region Persistent
    HRESULT        Register_PersistentUI(class UIObject* _ui);
    UIObject* Find_PersistentUI_ByName(const _wstring& _uiName);


#pragma endregion
#ifdef _DEBUG
    void                Render_UIHierarchy();
    class UIObject*     Get_UISelectObject();
    void                Set_SelectObject(class UIObject* pObj);
    void                Render_Search();
#endif // _DEBUG
    class UIManager* UIMgr() { return m_pUIManager; }
#pragma endregion UIManager



#pragma region SoundManager
    HRESULT LoadSound(const string& _name, const std::wstring& _filePath, SoundID* _outID = nullptr);
    HRESULT LoadSound_AddGroup(const string& _name, const string& _groupName, const std::wstring& _filePath, SoundID* _outID = nullptr);
    HRESULT LoadSoundFolder(const wstring& _folderPath);
    HRESULT LoadSoundFolder_Recursive(const wstring& _folderPath);

    void Delete_Sound(const _string& _soundName);
    void Delete_Sound(SoundID _ID);

    void Play_Sound(const string& _name, float _volume = 1.f, bool _loop = false);
    void Play_Sound_Unique(const string& _name, float _volume = 1.f, bool _loop = false);
    void Play_Sound_InGroup(const string& _name, const string& _groupName, float _volume = 1.f, bool _loop = false);

    void StopSound(const string& _name);
    void StopGroup(const string& _groupName);

    void SetVolume(const string& _name, float _volume);
    void SetGroupVolume(const string& _groupName, float _volume);
    void SetMasterVolume(float _volume);

    void StopAllSound();
    _bool IsPlaying(const string& _name);
    _bool IsGroupPlaying(const string& _groupName);
    _bool IsGroupFinished(const string& _groupName);

    void AddSoundToGroup(const string& _soundName, const string& _groupName);
    void RemoveSoundFromGroup(const string& _soundName, const string& _groupName);
    void PlayGroup(const string& _groupName, float _volume = 1.f, bool _loop = false);

    _float GetMasterVolume() const;
    _uint GetGroupPlayingCount(const string& _groupName);
    void ClearGroup(const string& _groupName);
    void Clear_SoundManager();
    class SoundManager* SoundMgr() { return m_pSoundManager; }

    // 그룹 내 랜덤 1개 재생
    void Play_Sound_RandomInGroup(const string& _groupName, _float _volume = 1.f, _bool _loop = false);

    // SoundID를 직접 받는 오버로드 (외부에서 해시를 미리 계산해둔 경우 사용)
    void  Play_Sound(SoundID _id, _float _volume = 1.f, _bool _loop = false);
    void  Play_Sound_InGroup(SoundID _id, SoundID _groupID, _float _volume = 1.f, _bool _loop = false);
    void  StopSound(SoundID _id);
    void  StopGroup(SoundID _groupID);
    void  FadeOutSound(const string& _name, _float _fadeDuration);
    _bool IsPlaying(SoundID _id);
    _bool IsGroupPlaying(SoundID _groupID);
    _bool IsGroupFinished(SoundID _groupID);
    void  SetVolume(SoundID _id, _float _volume);
    void  SetGroupVolume(SoundID _groupID, _float _volume);
#pragma endregion SoundManager



#pragma region EventManager
        template <typename Event>
        EventHandle Subscribe(function<void(const Event&)> _callback);
        template<typename Event>
        void Publish(const Event& _eventData);
        template<typename Event>
        void Publish_Stack(const Event& _eventData);
        void UnsubScribe(EventHandle _handlenum);
        class EventManager* EventMgr() { return m_pEventManager; }
#pragma endregion EventManager


#pragma region FileManager
        /// <summary>
        /// 파싱 데이터 저장 함수
        /// </summary>
        /// <param name="_filePath">세이브파일을 저장할 경로 + 저장 파일 이름</param>
        /// <param name="_data">상속받아서 함수 구현한 클래스</param>
        /// <param name="_toolType">어떤 툴에서 저장한건지</param>
        /// <param name="_version">버전 몇인지</param>
        /// <returns></returns>
        _bool SaveAsJson(const _string& _filePath, class Serializable& _data, const TOOLTYPE _toolType, const _string& _version = "1.0.0");
        template<typename T>
        bool LoadFromJson(const string& _filePath, T& _outData, FileHeader* _outHeader = nullptr);
#pragma endregion FileManager


#pragma region PICKING
    _bool PickingObject(_float4* pOut);
#pragma endregion


#pragma region ResourceManager
    /* Material Texture */
    ID3D11ShaderResourceView* Load_Texture(const wstring& _filePath, _bool _bLinear = false);
    void  Release_Texture(const _wstring& _filePath);
    _bool Release_Texture_Safe(const _wstring& _filePath);

    /* Shader */
    HRESULT Add_Shader(const _wstring& _ProtoName, const _wstring& _shaderPath, const D3D11_INPUT_ELEMENT_DESC* _elementsDesc, _uint _numElements, SHADERENTRIES _entries);
    class Shader* Get_Shader_Prototype(const _wstring& _shaderName);

    /* Buffer */
    /* 버퍼 등록(이미 있으면 기존 버퍼 반환) */
    Buffer* Register_Buffer(_uint _slot, Buffer* _pBuffer);
    /* 슬롯 번호로 버퍼 찾기 */
    Buffer* Find_Buffer(_uint _slot);

    // CS 로컬 버퍼 관리
    Buffer* Register_LocalBuffer(const _string& _cbName, _uint _slot, Buffer* _pBuffer);
    Buffer* Find_LocalBuffer(const _string& _cbName);

    // 핸들 자동 등록 시스템
    void Register_ShaderHandle(const _string& _varName, SHADERHANDLE* _pHandle);
    void Update_Handles_From_Buffer(Buffer* _pBuffer, _uint _slot, _bool _bOverwrite = true);

    // 플레이어 정보 빼오기 편하게
    void                Add_Player(GameObject* _player);
    GameObject*         Get_Player();
    void                Release_Player();
    _vector             Get_PlayerPos_Vector();
    _float3             Get_PlayerPos_Float3();
    _float4             Get_PlayerPos_Float4();
    const _float4x4*    Get_Player_Matrix_Ptr();

    // 동행자 정보
    GameObject* Get_Companion();

    void    Add_Companion(GameObject* _companion);
    void    Release_Companion();
    _vector Get_CompanionPos_Vector();
    _float3 Get_CompanionPos_Float3();
    _float4 Get_CompanionPos_Float4();
    const _float4x4* Get_Companion_Matrix_Ptr();
#pragma endregion ResourceManager


#pragma region Frustum
    _bool IsIn_InWorldSpace(_fvector vWorldPos, _float fRange = 0.f);
    _bool IsIn_InLocalSpace(_fvector vLocalPos, _float fRange = 0.f);
    _bool IsIn_Frustum(_fvector _worldPos, _float _range = 0.f);
    _bool IsIn_Frustum_AABB(const BoundingBox& _aabb);
    void  Get_Frustum_Planes(_float4* _outPlanes) const;
#pragma endregion Frustum


#pragma region ObjectFactory
    // 폴더 경로와 생성할 레벨을 받아서 생성(이게 더 편함)
    HRESULT Spawn_LevelData(const _string& _folderPath, _uint _level_for_create, _bool _useOnlyJson = false);
    // 로드 결과로 레벨을 받아서 생성
    HRESULT Spawn_LevelData(LevelLoadResult _result, _uint _level_for_create);
#pragma endregion ObjectFactory


#pragma region Instancing
    void Add_Instance(const wstring& strProtoTag, class Model* pModel, const _float4x4& WorldMatrix);
#pragma endregion


#pragma region PhysXManager
    void Update_PhysX(const _float fTimeDelta);
    // 맵툴에서 사용할 쿠킹만 되는 함수
    HRESULT PhysX_Cooking_Static_Model(PHYSX_ACTOR_DESC& _actorDesc);
    // 클라이언트나 다른 툴에서 사용할 쿠킹된 Actor 불러오기
    [[deprecated]]
    PxRigidStatic* PhysX_Load_Static_Actor(PHYSX_ACTOR_DESC& _actorDesc);
    // 클라이언트나 다른 툴에서 사용할 쿠킹된 Actor 불러오기(Convex)
    [[deprecated]]
    PxRigidStatic* PhysX_Load_Static_Actor_ConvexMesh(PHYSX_ACTOR_DESC& _actorDesc);
    // 클라이언트나 다른 툴에서 사용할 쿠킹된 Actor 불러오기(Triangle, Convex 자동)
    PxRigidStatic* PhysX_Load_Static_Actor_Auto(PHYSX_ACTOR_DESC& _actorDesc);
    // 캐릭터를 움직일 Controller 만들기
    PxController* PhysX_Create_Controller(const PHYSX_CONTROLLER_DESC& _controllerDesc);

    // 광선 발사 함수(총알판정, 마우스 피킹, 바닥 감지 등)
    PHYSX_RAYCAST_RESULT    PhysX_Raycast(const PxVec3& _origin, const PxVec3& _direction, _float _maxDistance);
    // 캡슐 검사 함수(캡슐을 밀어 처음 부딪히는 물체 검출)
    PHYSX_SWEEP_RESULT      PhysX_Sweep_Capsule(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);
    // 캡슐 mullti검사 함수
    PHYSX_MULTI_SWEEP_RESULT PhysX_Sweep_Capsule_Multi(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);
    PHYSX_MULTI_SWEEP_RESULT PhysX_Sweep_Capsule_Multi_ExceptStatic(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);
    // 위와 동일(구로 바뀜, 검사 판정이 조금 더 단순{연산량이 적음})
    PHYSX_SWEEP_RESULT      PhysX_Sweep_Sphere(_float _radius, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);
    PHYSX_OVERLAP_RESULT    PhysX_Overlap_Capsule(_float _radius, _float _halfHeight, const PxTransform& _pose);
    // 다중 히트 Sweep
    PHYSX_MULTI_SWEEP_RESULT PhysX_Sweep_Sphere_Multi(_float _radius, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);
    // 구 범위 안의 객체들 검출(범위 스킬같은거 검사용)
    PHYSX_OVERLAP_RESULT    PhysX_Overlap_Sphere(_float _radius, const PxTransform& _pose);

    PxPhysics* Get_Physics() const;
    PxScene* Get_Scene() const;
    PxControllerManager* Get_ControllerManager() const;

    // 캐릭터 제거
    void PhysX_Remove_Actor(PxRigidActor* _pActor);
    // 컨트롤러 제거(액터랑 연결됨)
    void PhysX_Remove_Controller(PxController* _pController);

    // 컨트롤러 임시 해제
    void PhysX_Disable_Controller(PxController* _pController);
    // 컨트롤러 다시 활성화
    void PhysX_Enable_Controller(PxController* _pController, const PxExtendedVec3& _spawnPos, PX_ACTOR_TYPE _eType);

#pragma region PhysXCloth
    HRESULT PhysX_Create_BoneChain(const PHYSX_BONE_CHAIN_DESC& _desc, class Model* _pMasterModel, const _float4x4& _objectWorldMatrix);
    void    PhysX_Update_BoneChain_Kinematic(_uint _iChainID, const _float4x4& _rootCombinedMatrix, const _float4x4& _objectWorldMatrix);
    void    PhysX_Fetch_BoneChain_Results(_uint _iChainID, const _float4x4& _objectWorldMatrix, vector<pair<_int, _float4x4>>& _outBoneMatrices);
    void    PhysX_Remove_BoneChain(_uint _iChainID);
    void    PhysX_Remove_All_BoneChains();
#pragma endregion PhysXCloth



#ifdef _DEBUG
    void  Enable_DebugVisualization();	// 시각화 파라미터 ON
    void  Disable_DebugVisualization();	// 시각화 파라미터 OFF
    void  Toggle_DebugVisualization();	// 시각화 토글
    _bool Is_DebugVisualization();      // 시각화 상태 확인

    const PxRenderBuffer* Get_PhysXRenderBuffer() const;

    void  Set_VisualizationParam(PxVisualizationParameter::Enum _eParam, _bool _bEnable);
    _bool Get_VisualizationParam(PxVisualizationParameter::Enum _eParam) const;
#endif // _DEBUG

#pragma endregion PhysXManager


#pragma region EFFECT_MANAGER
    vector<_string> const       Get_EffectNames();

    HRESULT						Add_Effect(const _wstring& wstrEffectName, class ParticleSystem* pParticleSystem);
    class ParticleSystem*       Find_Effect(const _wstring& wstrEffectName);

    void						Play_Effect_Position(const _wstring& wstrEffectName, const _float3& vPosition);		// 특정 이펙트를 특정 위치에 출력
    void						Play_Effect_Matrix(const _wstring& wstrEffectName, const _float4x4& matWorld);		// 행렬을 받아와서 출력(주로 뼈)

    void						Stop_Effect(const _wstring& wstrEffectName);
#pragma endregion


#pragma region DECAL_MANAGER
    HRESULT Initialize_Pool(_uint iPrototypeLevelID, const _wstring wstrPrototypeTag, _uint iLevelID, _uint iPoolSize);
    void Spawn_Decal(const _float3& vPosition, const _float3& vNormal, _float fScale = 1.f, _float fLifeTime = 7.f);
#pragma endregion


#pragma region ColliderManager
    /* Collider 등록/해제 */
    /* _groupTag로 충돌 그룹 분류 (예: "PLAYER_BODY", "EVENT_POINT" 등) */
    void Register_Collider(COLLISION_GROUP _groupTag, Collider* _pCollider);
    void Unregister_Collider(COLLISION_GROUP _groupTag, Collider* _pCollider);

    /* 어떤 그룹끼리 충돌 검사할지 등록 */
    /* 예: Add_CollisionPair(L"PLAYER_BODY", L"EVENT_POINT") */
    /* 레벨의 Initialize시점에 등록 */
    void Add_CollisionPair(COLLISION_GROUP _srcGroup, COLLISION_GROUP _dstGroup);
    void Remove_CollisionPair(COLLISION_GROUP _srcGroup, COLLISION_GROUP _dstGroup);

    /* 전체 초기화 (레벨 전환 시) */
    void Clear_ColliderManager();
#pragma endregion ColliderManager




#ifdef _DEBUG
    HRESULT Ready_RT_Debug(RenderTargets _RTName, _float _fX, _float _fY, _float _sizeX, _float _sizeY);
    HRESULT Render_RT(MRT _MRTName, class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta);
    HRESULT Render_RT(RenderTargets _targetName, class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta);
    PrimitiveBatch<VertexPositionColor>* Get_DebugBatch();
    void Add_DebugComponent(class Component* _component);
#endif

private:
	vector<Base*> m_vecCleanupTargets = {};

	class Graphic_Device*		m_pGraphic_Device = { nullptr };
	class TimerManager*			m_pTimerManager = { nullptr };
	class LevelManager*			m_pLevelManager = { nullptr };
	class PrototypeManager*		m_pPrototypeManager = { nullptr };
	class GameObjectManager*	m_pGameObjectManager = { nullptr };
	class Renderer*				m_pRenderer = { nullptr };
	class PipeLine*				m_pPipeLine = { nullptr };
	class InputManager*			m_pInputManager = { nullptr };
	class LightManager*			m_pLightManager = { nullptr };
	class SamplerManager*		m_pSamplerManager = { nullptr };
    class Calculator*           m_pCalculator = { nullptr };
    class FontManager*          m_pFontManager = { nullptr };
    class CameraManager*        m_pCameraManager = { nullptr };
    class RenderStateManager*   m_pRenderStateManager = { nullptr };
    class RenderTargetManager*  m_pRenderTargetManager = { nullptr };
    class Shadow*               m_pShadow = { nullptr };
    class UIManager*            m_pUIManager = { nullptr };
    class SoundManager*         m_pSoundManager = { nullptr };
    EventManager*               m_pEventManager = { nullptr };
    class FileManager*          m_pFileManagaer = { nullptr };
    class Picking*              m_pPicking = { nullptr };
    class Frustum*              m_pFrustum = { nullptr };
    class ObjectFactory*        m_pObjectFactory = { nullptr };
    class PhysXManager*         m_pPhysXManager = { nullptr };
    class ResourceManager*      m_pResourceManager = { nullptr };
    class EffectManager*        m_pEffectManager = { nullptr };
    class DecalManager*         m_pDecalManager = { nullptr };
    class SpawnManager*         m_pSpawnManager = { nullptr };
    class ColliderManager*      m_pColliderManager = { nullptr };


private:
    _float m_fWincx{}, m_fWincy{};
    D3D11_VIEWPORT m_Viewport{};
    HWND m_hWnd{};
    class Mouse* m_pMouse = { nullptr };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	_uint Release_Engine();
	void Free() override final;

private:
    ENGINEMODE      m_eEngineMode = ENGINEMODE::EDITOR;
};

NS_END

#include "GameInstance.inl"