#include "pch.h"
#include "FBXConverter.h"

#ifdef _DEBUG


namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Korean");

    cout << "========================================" << endl;
    cout << "       FBX → SIHO 바이너리 컨버터" << endl;
    cout << "========================================" << endl;
    cout << endl;

    if (argc < 2)
    {
        cout << "[사용법]" << endl;
        cout << "  FBX 파일을 이 exe 파일에 드래그 앤 드롭" << endl;
        cout << "  (모델 타입은 자동 감지됩니다)" << endl;
        cout << endl;
        system("pause");
        return 1;
    }

    FBXConverter converter;

    for (int i = 1; i < argc; ++i)
    {
        string inputPath = argv[i];

        fs::path filePath(inputPath);
        string ext = filePath.extension().string();

        for (auto& c : ext) c = tolower(c);

        if (ext != ".fbx" && ext != ".glb" && ext != ".gltf")
        {
            cout << "[건너뜀] 지원하지 않는 확장자: " << inputPath << endl;
            continue;
        }

        cout << endl;
        cout << "----------------------------------------" << endl;
        cout << "[변환 시작] " << inputPath << endl;
        cout << "----------------------------------------" << endl;

        // [수정] 타입 인자 제거 - 자동 감지
        if (converter.Convert(inputPath))
        {
            cout << "[성공]" << endl;
        }
        else
        {
            cout << "[실패]" << endl;
        }
    }

    cout << endl;
    cout << "========================================" << endl;
    cout << "              변환 완료" << endl;
    cout << "========================================" << endl;

    system("pause");
    return 0;
}

#endif // _DEBUG
