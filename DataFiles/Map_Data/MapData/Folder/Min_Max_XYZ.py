import json
import sys

def analyze_translations(file_path):
    # JSON 파일 읽기
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    # 데이터가 리스트가 아니면 리스트로 감싸기
    if not isinstance(data, list):
        data = [data]
    
    # X, Y, Z 값 추출
    x_values = []
    y_values = []
    z_values = []
    
    for item in data:
        if 'Translation' in item:
            trans = item['Translation']
            x_values.append(trans['X'])
            y_values.append(trans['Y'])
            z_values.append(trans['Z'])
    
    # 결과 계산 및 출력
    print(f"총 오브젝트 수: {len(x_values)}")
    print()
    
    print("=== X 좌표 ===")
    print(f"  최소값: {min(x_values):.6f}")
    print(f"  최대값: {max(x_values):.6f}")
    print(f"  중앙값: {(min(x_values) + max(x_values)) / 2:.6f}")
    print()
    
    print("=== Y 좌표 ===")
    print(f"  최소값: {min(y_values):.6f}")
    print(f"  최대값: {max(y_values):.6f}")
    print(f"  중앙값: {(min(y_values) + max(y_values)) / 2:.6f}")
    print()
    
    print("=== Z 좌표 ===")
    print(f"  최소값: {min(z_values):.6f}")
    print(f"  최대값: {max(z_values):.6f}")
    print(f"  중앙값: {(min(z_values) + max(z_values)) / 2:.6f}")
    print()
    
    # 전체 중심점
    center_x = (min(x_values) + max(x_values)) / 2
    center_y = (min(y_values) + max(y_values)) / 2
    center_z = (min(z_values) + max(z_values)) / 2
    print(f"=== 전체 중심점 ===")
    print(f"  ({center_x:.6f}, {center_y:.6f}, {center_z:.6f})")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("JSON 파일을 이 스크립트에 드래그 앤 드롭하세요.")
        input("아무 키나 누르면 종료...")
        sys.exit(1)
    
    file_path = sys.argv[1]
    
    try:
        analyze_translations(file_path)
    except Exception as e:
        print(f"오류 발생: {e}")
    
    # 결과 확인 후 종료할 수 있도록 대기
    input("\n아무 키나 누르면 종료...")