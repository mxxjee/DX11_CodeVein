#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL StructuredBuffer final : public Base
{
private:
	explicit StructuredBuffer();
	explicit StructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~StructuredBuffer();

public:
	HRESULT Initialize(SBUSAGE _usage, _uint _numElements, _uint _stride, const void* _initialData);

	// CPU -> GPU 갱신(SBUSAGE::DYNAMIC 전용)
	HRESULT Update(const void* _data, _uint _byteSize);

	// GPU -> CPU 결과 받기(SBUSAGE::Staging 용)
	HRESULT ReadBack(void* _outData, _uint _byteSize);

	// 쉐이더 바인딩
	HRESULT Bind_SRV(_uint _slot, _uint _stageMask);
	HRESULT Bind_UAV(_uint _slot);

	// 쉐이더 언바인딩
	void Unbind_SRV(_uint _slot, _uint _stageMask);
	void Unbind_UAV(_uint _slot);

public:
	inline ID3D11ShaderResourceView*	Get_SRV()			const { return m_pSRV; }
	inline ID3D11UnorderedAccessView*	Get_UAV()			const { return m_pUAV; }
	inline ID3D11Buffer*				Get_Buffer()		const { return m_pBuffer; }
	inline _uint						Get_Stride()		const { return m_iStride; }
	inline _uint						Get_NumElements()	const { return m_iNumElements; }
	inline _uint						Get_ByteWidth()		const { return m_iByteWidth; }

	
private:
	/* Initialize시점에 내부적으로 돌아가는 Create함수 */
	HRESULT Create_SRV();
	HRESULT Create_UAV();
	HRESULT Create_StagingBuffer();

private:
	ID3D11Buffer* m_pBuffer = { nullptr };
	ID3D11Buffer* m_pStagingBuffer = { nullptr };	// ReadBack용 버퍼(지연 생성)
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	ID3D11UnorderedAccessView* m_pUAV = { nullptr };

	SBUSAGE m_eUsage = { SBUSAGE::END };
	_uint m_iStride = {};
	_uint m_iNumElements = {};
	_uint m_iByteWidth = {};

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	/// <summary>
	/// Structured Buffer 타입별로 생성해주는 클래스
	/// </summary>
	/// <param name="pDevice"></param>
	/// <param name="pContext"></param>
	/// <param name="_usage">읽기 전용인지, 쓰기만 할지, 읽고 쓰기 다 할지</param>
	/// <param name="_numElements">원소의 개수</param>
	/// <param name="_stride">원소 1개의 바이트 크기</param>
	/// <param name="_initialData">초기 데이터</param>
	/// <returns></returns>
	static StructuredBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, SBUSAGE _usage, _uint _numElements, _uint _stride, const void* _initialData = nullptr);

public:
	void Free() override final;

};

NS_END
