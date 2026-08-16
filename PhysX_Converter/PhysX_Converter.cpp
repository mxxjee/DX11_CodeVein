#include "pch.h"
#include "PhysXConverter.h"

#ifdef _DEBUG

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Korean");

    // 모드 결정
#if defined(COOK_CONVEX)
    COOK_MODE mode = COOK_MODE::CONVEX;
    const char* modeName = "Convex";
#elif defined(COOK_TRIANGLE)
    COOK_MODE mode = COOK_MODE::TRIANGLE;
    const char* modeName = "Triangle";
#elif defined(COOK_BOTH)
    COOK_MODE mode = COOK_MODE::CONVEX; // 추가됨 : 나중에 선택으로 교체
    const char* modeName = "Select";
#else
#error "COOK_CONVEX, COOK_TRIANGLE, COOK_BOTH 중 하나를 정의하세요"
#endif

    cout << "========================================" << endl;
#if defined(COOK_BOTH)
    cout << "  FBX -> PhysX 쿠킹 컨버터 (선택형)" << endl;
#else
    cout << "  FBX -> PhysX " << modeName << " 쿠킹 컨버터" << endl;
#endif
    cout << "========================================" << endl;
    cout << endl;

    if (argc < 2)
    {
        cout << "[사용법]" << endl;
        cout << "  FBX 파일을 이 exe 파일에 드래그 앤 드롭" << endl;
        cout << endl;
        system("pause");
        return 1;
    }

    PhysXConverter converter;
    if (!converter.Initialize())
    {
        cout << "[실패] PhysX 초기화 실패" << endl;
        system("pause");
        return 1;
    }

    // 추가됨 : COOK_BOTH일 때 콘솔에서 모드 선택
#if defined(COOK_BOTH)
    cout << "쿠킹 모드를 선택하세요:" << endl;
    cout << "  1. Convex" << endl;
    cout << "  2. Triangle" << endl;
    cout << "입력: ";

    int selection = {};
    cin >> selection;

    if (selection == 1)
        mode = COOK_MODE::CONVEX;
    else if (selection == 2)
        mode = COOK_MODE::TRIANGLE;
    else
    {
        cout << "[실패] 잘못된 선택" << endl;
        system("pause");
        return 1;
    }

    cout << endl;
    cout << "선택된 모드: " << (mode == COOK_MODE::CONVEX ? "Convex" : "Triangle") << endl;
    cout << endl;
#endif

    vector<string> filePaths;

    for (int i = 1; i < argc; ++i)
    {
        fs::path inputPath(argv[i]);

        if (fs::is_directory(inputPath))
        {
            // 추가됨 : 폴더면 내부 FBX 파일 전부 수집
            for (const auto& entry : fs::recursive_directory_iterator(inputPath))
            {
                if (!entry.is_regular_file())
                    continue;

                string ext = entry.path().extension().string();
                for (auto& c : ext) c = tolower(c);

                if (ext == ".fbx" || ext == ".glb" || ext == ".gltf")
                    filePaths.push_back(entry.path().string());
            }
        }
        else
        {
            filePaths.push_back(argv[i]);
        }
    }

    cout << "변환 대상: " << filePaths.size() << "개" << endl;
    cout << endl;

    for (const auto& inputPath : filePaths)
    {
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

        if (converter.Convert(inputPath, mode))
            cout << "[성공]" << endl;
        else
            cout << "[실패]" << endl;
    }

    cout << endl;
    cout << "========================================" << endl;
    cout << "              변환 완료" << endl;
    cout << "========================================" << endl;
    system("pause");
    return 0;
}

#endif // _DEBUG