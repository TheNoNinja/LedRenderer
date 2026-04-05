Import("env")
import os
import platform
import subprocess
import shutil
from pathlib import Path

def get_vcpkg_dirs():
    vcpkg_root = os.environ.get("VCPKG_ROOT")
    if not vcpkg_root:
        return None, None, None

    vcpkg_root = Path(vcpkg_root)
    triplets = ["x64-windows-release", "x64-windows", "x64-linux"]

    for triplet in triplets:
        include_dir = vcpkg_root / "installed" / triplet / "include"
        lib_dir = vcpkg_root / "installed" / triplet / "lib"
        bin_dir = vcpkg_root / "installed" / triplet / "bin"
        if include_dir.exists() and lib_dir.exists():
            return include_dir, lib_dir, bin_dir

    return None, None, None

def check_pkg_config(lib):
    try:
        subprocess.check_call(["pkg-config", "--exists", lib])
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False

if platform.system() == "Windows":
    include_dir, lib_dir, bin_dir = get_vcpkg_dirs()
    if include_dir and lib_dir:
        env.Append(CPPPATH=[str(include_dir)])
        env.Append(LIBPATH=[str(lib_dir)])

    env.Append(LIBS=["SDL3", "opengl32"])

    if bin_dir:
        def copy_sdl_dll(source, target, env):
            dll = bin_dir / "SDL3.dll"
            if dll.exists():
                shutil.copy2(dll, target[0].dir.abspath)
        env.AddPostAction("$BUILD_DIR/${PROGNAME}.exe", copy_sdl_dll)

else:
    found_sdl3 = False
    if check_pkg_config("sdl3"):
        env.ParseConfig("pkg-config --cflags --libs sdl3")
        found_sdl3 = True
    
    if not found_sdl3:
        include_dir, lib_dir = get_vcpkg_dirs()
        if include_dir and lib_dir:
            env.Append(CPPPATH=[str(include_dir)])
            env.Append(LIBPATH=[str(lib_dir)])
            env.Append(LIBS=["SDL3"])
            found_sdl3 = True

    if not found_sdl3:
        env.Append(LIBS=["SDL3"])

    if check_pkg_config("gl"):
        env.ParseConfig("pkg-config --cflags --libs gl")
    else:
        env.Append(LIBS=["GL"])
