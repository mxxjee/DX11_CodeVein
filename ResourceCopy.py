import os
import json
import shutil
from datetime import datetime
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor  # 추가됨 : 동시 스캔용

# ===== 설정 파일 경로 =====
SCRIPT_DIR = Path(__file__).parent
CONFIG_PATH = SCRIPT_DIR / "config.json"
SNAPSHOT_PATH = SCRIPT_DIR / ".last_sync.json"

# ===== 제외할 폴더/파일 목록 (전역) =====
EXCLUDE_FOLDERS = []
EXCLUDE_FILES = []


# exit(1) 대신 사용할 커스텀 예외
class SyncError(Exception):
    pass


def load_config():
    """config.json에서 경로 설정 불러오기"""
    if not CONFIG_PATH.exists():
        # exit(1) 대신 예외 발생
        raise SyncError(
            "config.json 파일이 없습니다!\n"
            "config.json을 만들고 paths 배열을 설정하세요."
        )
    
    # JSON 파싱 실패 처리
    try:
        with open(CONFIG_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    except json.JSONDecodeError as e:
        raise SyncError(f"config.json 파싱 실패: {e}")
    except OSError as e:
        raise SyncError(f"config.json 읽기 실패: {e}")


def find_valid_paths(config):
    """
    paths 배열에서 local_path가 존재하는 첫 번째 경로 쌍 반환
    """
    paths = config.get("paths", [])
    
    for index, path_pair in enumerate(paths):
        local_path = path_pair.get("local_path", "")
        drive_path = path_pair.get("drive_path", "")
        
        if Path(local_path).exists():
            print(f"[경로 발견] paths[{index}] 사용")
            return local_path, drive_path
    
    # exit(1) 대신 예외 발생
    lines = [f"  [{index}] {p.get('local_path', '(없음)')}" for index, p in enumerate(paths)]
    raise SyncError(
        "유효한 local_path를 찾을 수 없습니다!\n"
        "config.json의 paths를 확인하세요:\n" + "\n".join(lines)
    )


def load_snapshot():
    """이전 동기화 스냅샷 불러오기"""
    if not SNAPSHOT_PATH.exists():
        return {}
    
    # 스냅샷 파일 손상 처리
    try:
        with open(SNAPSHOT_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        print(f"[경고] 스냅샷 파일 손상, 초기화합니다: {e}")
        return {}


def save_snapshot(snapshot):
    """현재 상태를 스냅샷으로 저장"""
    with open(SNAPSHOT_PATH, "w", encoding="utf-8") as f:
        json.dump(snapshot, f, ensure_ascii=False, indent=2)


def should_exclude(path):
    """
    해당 경로가 제외 대상인지 확인
    """
    path_obj = Path(path)
    
    # 제외 파일 체크
    if path_obj.name in EXCLUDE_FILES:
        return True
    
    # 제외 폴더 체크
    path_parts = path_obj.parts
    for exclude_folder in EXCLUDE_FOLDERS:
        if exclude_folder in path_parts:
            return True
    
    return False


# 수정됨 : rglob + 별도 stat() 대신 scandir 재귀로 DirEntry stat 캐시 활용
def _scan_recursive(base: Path, current: Path, files: dict):
    """scandir 기반 재귀 스캔 (DirEntry stat 캐시 활용, 추가 syscall 없음)"""
    try:
        with os.scandir(current) as it:
            for entry in it:
                relative = Path(entry.path).relative_to(base).as_posix()
                
                if should_exclude(relative):
                    continue
                
                if entry.is_dir(follow_symlinks=False):
                    _scan_recursive(base, Path(entry.path), files)
                elif entry.is_file(follow_symlinks=False):
                    try:
                        # DirEntry.stat()은 캐시된 결과 반환 (추가 syscall 없음)
                        files[relative] = entry.stat().st_mtime
                    except OSError as e:
                        print(f"  [경고] 파일 접근 실패, 건너뜀: {relative} ({e})")
    except OSError as e:
        print(f"  [경고] 폴더 접근 실패, 건너뜀: {current} ({e})")


def scan_folder(base_path):
    """
    폴더 내 모든 파일의 상대경로와 수정시간을 딕셔너리로 반환
    반환 형식: { "상대경로": 수정시간(timestamp), ... }
    """
    files = {}
    base = Path(base_path)
    
    if not base.exists():
        print(f"[경고] 경로가 존재하지 않음: {base_path}")
        return files
    
    # 수정됨 : rglob 대신 scandir 재귀 호출
    _scan_recursive(base, base, files)
    
    return files


def copy_file(src_base, dst_base, relative_path):
    """파일 복사 (폴더 자동 생성)"""
    src = Path(src_base) / relative_path
    dst = Path(dst_base) / relative_path
    
    # 대상 폴더 생성
    dst.parent.mkdir(parents=True, exist_ok=True)
    
    shutil.copy2(src, dst)  # copy2는 수정시간도 복사
    print(f"  [복사] {relative_path}")


def delete_file(base_path, relative_path):
    """파일 삭제"""
    file_path = Path(base_path) / relative_path
    
    if file_path.exists():
        file_path.unlink()
        print(f"  [삭제] {relative_path}")
        
        # 빈 폴더 정리
        parent = file_path.parent
        while parent != Path(base_path):
            if not any(parent.iterdir()):
                parent.rmdir()
                print(f"  [빈 폴더 삭제] {parent.relative_to(base_path)}")
            else:
                break
            parent = parent.parent


# 드라이브 경로 접근 가능 여부 확인
def check_drive_accessible(drive_path):
    """드라이브 경로가 실제로 접근 가능한지 확인 (네트워크/Google Drive)"""
    path = Path(drive_path)
    
    if not path.exists():
        raise SyncError(
            f"드라이브 경로가 존재하지 않습니다: {drive_path}\n"
            "Google Drive가 실행 중인지 확인하세요."
        )
    
    # 실제 읽기/쓰기 가능한지 테스트
    test_file = path / ".sync_test_probe"
    try:
        test_file.write_text("test", encoding="utf-8")
        test_file.unlink()
    except PermissionError:
        raise SyncError(
            f"드라이브 경로에 쓰기 권한이 없습니다: {drive_path}\n"
            "Google Drive 동기화 상태를 확인하세요."
        )
    except OSError as e:
        raise SyncError(
            f"드라이브 경로 접근 실패: {drive_path}\n"
            f"원인: {e}\n"
            "인터넷 연결 또는 Google Drive 상태를 확인하세요."
        )


def sync():
    """메인 동기화 로직"""
    global EXCLUDE_FOLDERS, EXCLUDE_FILES
    
    print("=" * 50)
    print("      리소스 동기화 시작")
    print("=" * 50)
    
    # 설정 불러오기
    config = load_config()
    
    # 제외 폴더/파일 설정
    EXCLUDE_FOLDERS = config.get("exclude_folders", [])
    EXCLUDE_FILES = config.get("exclude_files", [])
    
    if EXCLUDE_FOLDERS or EXCLUDE_FILES:
        print(f"\n[제외 폴더] {', '.join(EXCLUDE_FOLDERS)}")
        print(f"[제외 파일] {', '.join(EXCLUDE_FILES)}")
    
    # 유효한 경로 찾기
    local_path, drive_path = find_valid_paths(config)
    
    print(f"\n[로컬]     {local_path}")
    print(f"[드라이브] {drive_path}\n")
    
    # 단순 존재 체크 대신 접근 가능 여부까지 확인
    check_drive_accessible(drive_path)
    
    # 수정됨 : 로컬/드라이브 동시 스캔 후 스냅샷 로드
    print("[1/5] 파일 패스트 스캔 중...")
    with ThreadPoolExecutor(max_workers=2) as executor:
        future_local = executor.submit(scan_folder, local_path)
        future_drive = executor.submit(scan_folder, drive_path)
        local_files = future_local.result()
        drive_files = future_drive.result()
    snapshot = load_snapshot()
    
    print(f"  - 로컬: {len(local_files)}개")
    print(f"  - 드라이브: {len(drive_files)}개")
    print(f"  - 스냅샷: {len(snapshot)}개")
    
    # 모든 파일 경로 합집합
    all_files = set(local_files.keys()) | set(drive_files.keys()) | set(snapshot.keys())
    
    # 동기화 작업 분류
    to_copy_to_drive = []   # 로컬 -> 드라이브
    to_copy_to_local = []   # 드라이브 -> 로컬
    to_delete_drive = []    # 드라이브에서 삭제
    to_delete_local = []    # 로컬에서 삭제
    
    print("\n[2/5] 변경사항 분석 중...")
    
    for relative_path in all_files:
        in_local = relative_path in local_files
        in_drive = relative_path in drive_files
        in_snapshot = relative_path in snapshot
        
        if in_local and in_drive:
            # 둘 다 있음 -> 최신 파일로 동기화
            local_mtime = local_files[relative_path]
            drive_mtime = drive_files[relative_path]
            
            # 1초 이상 차이나면 복사 (파일시스템 오차 허용)
            if local_mtime - drive_mtime > 1:
                to_copy_to_drive.append(relative_path)
            elif drive_mtime - local_mtime > 1:
                to_copy_to_local.append(relative_path)
                
        elif in_local and not in_drive:
            if in_snapshot:
                # 스냅샷에 있었는데 드라이브에 없음 -> 드라이브에서 삭제됨 -> 로컬도 삭제
                to_delete_local.append(relative_path)
            else:
                # 새 파일 -> 드라이브로 복사
                to_copy_to_drive.append(relative_path)
                
        elif not in_local and in_drive:
            if in_snapshot:
                # 스냅샷에 있었는데 로컬에 없음 -> 로컬에서 삭제됨 -> 드라이브도 삭제
                to_delete_drive.append(relative_path)
            else:
                # 새 파일 -> 로컬로 복사
                to_copy_to_local.append(relative_path)
                
        # 둘 다 없고 스냅샷에만 있음 -> 이미 양쪽에서 삭제됨, 무시
    
    # 개별 파일 작업 실패 시 건너뛰고 계속 진행
    failed_operations = []
    
    print(f"\n[3/5] 로컬 -> 드라이브 ({len(to_copy_to_drive)}개)")
    for relative_path in to_copy_to_drive:
        try:
            copy_file(local_path, drive_path, relative_path)
        except OSError as e:
            failed_operations.append(f"복사 실패 (-> 드라이브): {relative_path} ({e})")
            print(f"  [실패] {relative_path}: {e}")
    
    print(f"\n[4/5] 드라이브 -> 로컬 ({len(to_copy_to_local)}개)")
    for relative_path in to_copy_to_local:
        try:
            copy_file(drive_path, local_path, relative_path)
        except OSError as e:
            failed_operations.append(f"복사 실패 (-> 로컬): {relative_path} ({e})")
            print(f"  [실패] {relative_path}: {e}")
    
    print(f"\n[5/5] 삭제 처리")
    print(f"  드라이브에서 삭제: {len(to_delete_drive)}개")
    for relative_path in to_delete_drive:
        try:
            delete_file(drive_path, relative_path)
        except OSError as e:
            failed_operations.append(f"삭제 실패 (드라이브): {relative_path} ({e})")
            print(f"  [실패] {relative_path}: {e}")
    
    print(f"  로컬에서 삭제: {len(to_delete_local)}개")
    for relative_path in to_delete_local:
        try:
            delete_file(local_path, relative_path)
        except OSError as e:
            failed_operations.append(f"삭제 실패 (로컬): {relative_path} ({e})")
            print(f"  [실패] {relative_path}: {e}")
    
    # 새 스냅샷 저장 (현재 동기화된 상태)
    new_snapshot = {}
    
    # 수정됨 : 최종 스캔도 동시 실행
    with ThreadPoolExecutor(max_workers=2) as executor:
        future_final_local = executor.submit(scan_folder, local_path)
        future_final_drive = executor.submit(scan_folder, drive_path)
        final_local = future_final_local.result()
        final_drive = future_final_drive.result()
    
    # 양쪽에 있는 파일만 스냅샷에 기록
    for relative_path in final_local:
        if relative_path in final_drive:
            new_snapshot[relative_path] = max(final_local[relative_path], final_drive[relative_path])
    
    save_snapshot(new_snapshot)
    
    # 결과 출력
    print("\n" + "=" * 50)
    print("      동기화 완료!")
    print("=" * 50)
    print(f"  복사: {len(to_copy_to_drive) + len(to_copy_to_local)}개")
    print(f"  삭제: {len(to_delete_drive) + len(to_delete_local)}개")
    print(f"  총 파일: {len(new_snapshot)}개")
    
    # 실패 항목 요약 출력
    if failed_operations:
        print(f"\n  [!] 실패: {len(failed_operations)}개")
        for msg in failed_operations:
            print(f"    - {msg}")
    
    print("=" * 50)


if __name__ == "__main__":
    try:
        sync()
    # 동기화 로직 내 예외를 사유와 함께 출력
    except SyncError as e:
        print(f"\n[동기화 오류] {e}")
    except PermissionError as e:
        print(f"\n[권한 오류] 파일 접근 권한이 없습니다: {e}")
    except ConnectionError as e:
        print(f"\n[연결 오류] 네트워크 연결을 확인하세요: {e}")
    except OSError as e:
        print(f"\n[시스템 오류] {e}")
        # 네트워크 관련 에러코드 안내
        if e.errno in (22, 110, 112, 121):
            print("  -> 네트워크 드라이브 또는 인터넷 연결 상태를 확인하세요.")
    except Exception as e:
        print(f"\n[오류 발생] {e}")
    
    input("\n아무 키나 누르면 종료...")