#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))


#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

#define			NS_BEGIN(NAMESPACE)			namespace NAMESPACE {
#define			NS_END						}

#define			USING(NAMESPACE)			using namespace NAMESPACE;

#define			PURE = 0
#define			FINAL override final

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define NO_COPY(CLASSNAME)										\
		private:												\
		CLASSNAME(const CLASSNAME&) = delete;					\
		CLASSNAME& operator = (const CLASSNAME&)= delete;		

#define DECLARE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned int DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned int CLASSNAME::DestroyInstance( void ) {		\
			unsigned int iRefCnt = {0};							\
			if(nullptr != m_pInstance)	{						\
				iRefCnt = m_pInstance->Release();				\
				if(0 == iRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return iRefCnt;										\
		}



// 디버그 모드에서만 실행되는 cout, 릴리즈 모드에서는 실행 안 됨
#ifdef _DEBUG
// COUT(DOS창에 출력하고싶은 변수나 메세지)
#define COUT(CODE) std::cout << CODE << std::endl;
#else
#define COUT(CODE)
#endif


// 디버그 모드에서만 실행되는 wcout, 릴리즈 모드에서는 실행 안 됨
#ifdef _DEBUG
// COUT(DOS창에 출력하고싶은 변수나 메세지)
#define WCOUT(CODE) std::wcout << CODE << std::endl;
#else
#define WCOUT(CODE)
#endif

// 디버그 모드에서만 실행되는 break;
#ifdef _DEBUG
#define BREAK __debugbreak();
#else
#define BREAK
#endif


// FAIL_MESSAGE(쓰고싶은 기능, 띄울 메세지, 캡션, 리턴값); 
#define MSG_FAIL(FUNCTION, MESSAGE, CAPTION, RETURN)					\
		if(FAILED(FUNCTION))											\
		{																\
			MessageBox(nullptr, MESSAGE, CAPTION, MB_OK);				\
			BREAK;												        \
			return RETURN;												\
		}


// NULL_MESSAGE(nullptr체크할 클래스, 띄울 메세지, 캡션, 리턴값); 
#define MSG_NULL(CLASS, MESSAGE, CAPTION, RETURN)						\
		if(CLASS == nullptr)											\
		{																\
			MessageBox(nullptr, MESSAGE, CAPTION, MB_OK);				\
			BREAK;														\
			return RETURN;												\
		}

// NULL_MESSAGE(범위 확인, 띄울 메세지, 캡션, 리턴값);
// EX) MSG_OUTOFINDEX(iIndex < 0 || iIndex >= iMaxCount, L"인덱스 범위 초과", L"경고!!!", E_FAIL);
#define MSG_OUTOFINDEX(COMPARE, MESSAGE, CAPTION, RETURN)				\
		if(COMPARE)														\
		{																\
			MessageBox(nullptr, MESSAGE, CAPTION, MB_OK);				\
			return RETURN;												\
		}

#define BREAK_NULLPTR(INSTANCE)     \
    if (INSTANCE == nullptr)        \
    {                               \
        BREAK;                      \
        return E_FAIL;              \
    }

// MSG_ON(메세지, 캡션);
#define MSG_ON(MESSAGE, CAPTION) MessageBox(nullptr, MESSAGE, CAPTION, MB_OK);

// COUT스타일 디버그 출력
#define DEBUG_LOG(outText) \
    do { \
        ostringstream oss; \
        oss << outText; \
        OutputDebugStringA(oss.str().c_str()); \
    } while(0)

// CHECK_FAILED(기능, 리턴값);
#define CHECK_FAILED(FUNCTION, RETURN) if(FAILED(FUNCTION)) { return RETURN; }

// CHECK_NULL(인스턴스);
#define CHECK_NULLPTR(INSTANCE) if(INSTANCE == nullptr) { return E_FAIL; }

/////////////단순 return 매크로/////////////////////////////////
#define CHECK_TRUE(CONDITION) { if(CONDITION == true) return; }
#define CHECK_FALSE(CONDITION) { if(CONDITION == false) return; }
#define CHECK_JUST_NULL(INSTANCE) { if(INSTANCE == nullptr) return; }

//특정 결과값 리턴 가능  매크로//////
#define CHECK_TRUE_RESULT(CONDITION, RESULT) { if(CONDITION == true) return RESULT; }
#define CHECK_FALSE_RESULT(CONDITION, RESULT) { if(CONDITION == false) return RESULT; }
#define CHECK_NULL_RESULT(INSTANCE, RESULT) { if(INSTANCE == nullptr) return RESULT; }

// 부모 클래스 업데이트 실패시 쭉쭉쭉 리턴
#define CHECK_FUNCTION_FAIL(FUNCTION) { if(FUNCTION < 0) return -1;}
#define CHECK_FUNCTION_FAIL_VOID(FUNCTION) { if(FUNCTION < 0) return;}

// NULL_CONITNUE(인스턴스);
#define NULL_CONTINUE(INSTANCE) { if(INSTANCE == nullptr) continue; }

#define _UINT(VALUE) static_cast<_uint>(VALUE)

#define CAST(TYPE) static_cast<TYPE>

#define DCAST(TYPE) dynamic_cast<TYPE>

#define RCAST(TYPE) reinterpret_cast<TYPE>

#define LSTATIC _UINT(LEVEL::STATIC)

#define DEFAULT_TEXTUREKEY string("UI4_Main/White")

constexpr auto ALPHA = 0xff000000;
constexpr auto RED = 0x00ff0000;
constexpr auto GREEN = 0x0000ff00;
constexpr auto BLUE = 0x000000ff;

constexpr unsigned int stage_VS = 1 << 0;
constexpr unsigned int stage_PS = 1 << 1;
constexpr unsigned int stage_GS = 1 << 2;
constexpr unsigned int stage_HS = 1 << 3;
constexpr unsigned int stage_DS = 1 << 4;
constexpr unsigned int stage_CS = 1 << 5;

constexpr XMFLOAT4X4 FLOAT4x4_IDENTITY =
{
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
};


#define PHYSX <PhysX/PxPhysicsAPI.h>
#define INCLUDEPHYSX #include PHYSX


// Engine_Define.h 네임스페이스 전역 선언용
#define EXTERN_SHADER_HANDLE(varName) \
	ENGINE_DLL extern const SHADERHANDLE& varName;

// ResourceManager.h 클래스 내부 선언용
#define DECLARE_SHADER_HANDLE(varName) \
	public: static const SHADERHANDLE& varName; \
	private: static SHADERHANDLE _real_##varName;

// ResourceManager.cpp 정의용
#define DEFINE_SHADER_HANDLE(varName) \
	Engine::SHADERHANDLE Engine::ResourceManager::_real_##varName = \
		(Get_HandleRegistry().emplace_back(#varName, &Engine::ResourceManager::_real_##varName), Engine::SHADERHANDLE{}); \
	const Engine::SHADERHANDLE& Engine::ResourceManager::varName = Engine::ResourceManager::_real_##varName; \
	const Engine::SHADERHANDLE& Engine::varName = Engine::ResourceManager::varName;

// Initialize_Shader_Value 내부 초기화용
#define INIT_SHADER_HANDLE(varName) \
	_real_##varName = _shader->Get_Handle(#varName);

// ResourceManager::Initialize 내부 레지스트리 등록용
#define REGISTER_SHADER_HANDLE(varName) \
	Register_ShaderHandle(#varName, &_real_##varName);

#endif // Engine_Macro_h__
