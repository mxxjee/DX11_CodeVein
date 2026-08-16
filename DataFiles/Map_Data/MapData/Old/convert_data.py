import json
import math
import os

def euler_to_quaternion(pitch, roll, yaw):
    """
    오일러 각도(Degree)를 쿼터니언(X, Y, Z, W)으로 변환하는 함수입니다.
    입력: Pitch, Roll, Yaw (Degrees)
    출력: {'X': ..., 'Y': ..., 'Z': ..., 'W': ...}
    """
    # 도(Degree)를 라디안(Radian)으로 변환
    # 회전 순서나 축 정의에 따라 계산식이 달라질 수 있으나, 일반적인 변환식을 적용합니다.
    # Unreal Engine 기준 (Roll=X, Pitch=Y, Yaw=Z) 변환을 가정합니다.
    
    r = math.radians(roll)  # Roll (X축 회전)
    p = math.radians(pitch) # Pitch (Y축 회전)
    y = math.radians(yaw)   # Yaw (Z축 회전)

    cy = math.cos(y * 0.5)
    sy = math.sin(y * 0.5)
    cp = math.cos(p * 0.5)
    sp = math.sin(p * 0.5)
    cr = math.cos(r * 0.5)
    sr = math.sin(r * 0.5)

    # 쿼터니언 계산
    w = cr * cp * cy + sr * sp * sy
    x = sr * cp * cy - cr * sp * sy
    y = cr * sp * cy + sr * cp * sy
    z = cr * cp * sy - sr * sp * cy

    return {"X": x, "Y": y, "Z": z, "W": w}

def convert_json_structure(input_filename, output_filename):
    # 입력 파일 확인
    if not os.path.exists(input_filename):
        print(f"오류: '{input_filename}' 파일을 찾을 수 없습니다.")
        return

    try:
        with open(input_filename, 'r', encoding='utf-8') as f:
            source_data = json.load(f)
    except Exception as e:
        print(f"파일 읽기 오류: {e}")
        return

    converted_data = []

    for item in source_data:
        # 데이터가 딕셔너리인지 확인
        if not isinstance(item, dict):
            continue

        model_info = item.get("Model Info", {})
        srt = item.get("SRT", {})
        
        # 1. MeshName 매핑 (Model Info -> Name)
        mesh_name = model_info.get("Name", "")

        # 2. ObjectName 매핑 (Model Info -> Mateiral Info -> Name)
        # 원본 데이터에 오타("Mateiral Info")가 있어 이를 그대로 사용하되, 없으면 올바른 철자도 확인
        material_info_list = model_info.get("Mateiral Info") or model_info.get("Material Info")
        
        object_name = ""
        if isinstance(material_info_list, list) and len(material_info_list) > 0:
            # 리스트의 첫 번째 항목의 Name을 가져옴
            first_mat = material_info_list[0]
            if isinstance(first_mat, dict):
                object_name = first_mat.get("Name", "")
        
        # 3. Translation 매핑 (SRT -> Position)
        pos = srt.get("Position", {})
        translation = {
            "X": pos.get("X", 0.0),
            "Y": pos.get("Y", 0.0),
            "Z": pos.get("Z", 0.0)
        }

        # 4. Rotation 매핑 (SRT -> Rotation -> Quaternion 변환)
        rot = srt.get("Rotation", {})
        pitch = rot.get("Pitch", 0.0)
        roll = rot.get("Roll", 0.0)
        yaw = rot.get("Yaw", 0.0)
        
        quaternion = euler_to_quaternion(pitch, roll, yaw)

        # 5. Scale 매핑 (SRT -> Scale)
        scale_raw = srt.get("Scale", {})
        scale = {
            "X": scale_raw.get("X", 1.0),
            "Y": scale_raw.get("Y", 1.0),
            "Z": scale_raw.get("Z", 1.0)
        }

        # 새로운 구조 생성
        new_entry = {
            "MeshName": mesh_name,
            "ObjectName": object_name,
            "Translation": translation,
            "Rotation": quaternion,
            "Scale": scale
        }
        converted_data.append(new_entry)

    # 결과 파일 저장
    try:
        with open(output_filename, 'w', encoding='utf-8') as f:
            json.dump(converted_data, f, indent=4)
        print(f"변환 완료! 결과가 '{output_filename}'에 저장되었습니다.")
    except Exception as e:
        print(f"파일 저장 오류: {e}")

if __name__ == "__main__":
    input_file = "LV_Inner_Cathedral_SecretRoom_Converted.json"
    output_file = "converted_scene_data.json"
    
    print(f"'{input_file}' 변환을 시작합니다...")
    convert_json_structure(input_file, output_file)