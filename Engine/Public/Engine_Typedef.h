#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__



namespace Engine
{
	typedef		bool						_bool;

	typedef		char						_char;
	typedef		int8_t						_byte;
	typedef		uint8_t						_ubyte;
    typedef     const char*                 _cchar;
    typedef     char*                       _pchar;

	typedef		wchar_t						_tchar;
	typedef		string						_string;
	typedef		wstring						_wstring;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;
	typedef		unsigned long long			_ullong;

	typedef		float						_float;
	typedef		double						_double;

	template<typename Key, typename Value>
	using		UMAP = unordered_map<Key, Value>;

	/* 값 저장용 구조체 */
	typedef		XMFLOAT2					_float2;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;
	typedef		XMFLOAT4X4					_float4x4;

    typedef		XMUINT2						_uint2;
    typedef		XMUINT3						_uint3;
    typedef		XMUINT4						_uint4;

    typedef		XMINT2						_int2;
    typedef		XMINT3						_int3;
    typedef		XMINT4						_int4;

	/* 값 계산용(연산용) 구조체 */
	typedef		XMVECTOR					_vector;
	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;

	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX					_fmatrix;
	typedef		CXMMATRIX					_cmatrix;


	/* SimpleMath용 구조체(수많은 연산자 오버로딩과 함수들) */
	typedef		DirectX::SimpleMath::Vector2		_smvector2;
	typedef		DirectX::SimpleMath::Vector3		_smvector3;
	typedef		DirectX::SimpleMath::Vector4		_smvector4;
	typedef		DirectX::SimpleMath::Matrix			_smmatrix;
	typedef		DirectX::SimpleMath::Quaternion		_smquaternion;
	typedef		DirectX::SimpleMath::Color			_smcolor;
	
	typedef _smvector2		_smvec2;
	typedef	_smvector3		_smvec3;
	typedef	_smvector4		_smvec4;
	typedef	_smmatrix		_smmat;
	typedef	_smquaternion	_smquat;

	typedef _smvector2		_smfloat2;
	typedef	_smvector3		_smfloat3;
	typedef	_smvector4		_smfloat4;
	typedef	_smmatrix		_smfloat4x4;


	//노티파이에서 이벤트 담아줄때 이벤트 마다 필요한 변수를 map에 담을건데, 이때 Value값을 자유롭게 사용하기 위해서
	using VariantValue = variant<monostate, _int, _uint, _float, _bool, _string, _float2, _float3, _float4>; //monostate는 값 초기화용
	


	/*hotKey*/
	using HotKeyCallback = std::function<void()>;


}

#endif // Engine_Typedef_h__
