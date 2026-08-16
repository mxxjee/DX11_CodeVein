

NS_BEGIN(Engine)

template<typename T>
inline _bool FileManager::LoadFromJson(const string& _filePath, T& _outData, FileHeader* _outHeader)
{
	// is_base_of_v<Base, Derived> : Derived가 Base를 상속받는지 확인하는 함수 
	static_assert(is_base_of_v<Serializable, T>,	"T must inherit from Serializable");

	try
	{
		// 파일이 있는지 확인
		if (!filesystem::exists(_filePath))
		{
			cerr << _filePath << "\n 경로에 있는 파일이 존재하지 않습니다." << endl;
			return false;
		}

		// 파일 읽기
		ifstream file(_filePath);
		if (!file.is_open())
		{
			cerr << _filePath << "\n 파일을 여는데 실패했습니다." << endl;
			return false;
		}

		// JSON 파싱
		json root = json::parse(file);
		file.close();

		// 헤더 읽기
		if (_outHeader != nullptr && root.contains("header"))
		{
			_outHeader->From_Json(root["header"]);
		}

		// 데이터 읽기
		if (root.contains("data"))
		{
			_outData.From_Json(root["data"]);
		}
		else
		{
			cerr << "data 파일을 여는데 실패했습니다." << endl;
		}

		COUT("로드 완료!");
		
		return true;
	}
	catch (const json::parse_error& e)
	{
		cerr << "Json 파싱 에러 " << e.what() << endl;
	}
	catch (const exception& e)
	{
		cerr << "로드 에러 " << e.what() << endl;
	}

	return true;
}

NS_END