#pragma once

#include "Base.h"
#include "FileParsing_Data.h"

NS_BEGIN(Engine)

// 데이터를 전송하기 위해 상속받아야 하는 클래스
// 이 인터페이스 구현하면 FileManager를 통해 저장 / 로드 가능
class ENGINE_DLL Serializable abstract : public Base
{
protected:
	explicit Serializable() = default;
	virtual ~Serializable();

public:
	// Json 직렬화(디버깅용)
	virtual ordered_json To_Json() = 0;
	virtual void From_Json(const ordered_json& _jsonData) = 0;

	// Binary 직렬화(릴리즈, 대부분 로드용)
	virtual void To_Binary(ofstream& _file) = 0;
	virtual void From_Binary(ifstream& _file) = 0;

public:	
	void Free() override;
};

NS_END
