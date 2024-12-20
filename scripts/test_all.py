"""
██████  ███████       ████████ ██   ██ ██████  ███████  █████  ██████          ██████   ██████   ██████  ██
██   ██ ██      ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██   ██ ██    ██ ██    ██ ██
██████  ███████          ██    ███████ ██████  █████   ███████ ██   ██         ██████  ██    ██ ██    ██ ██
██   ██      ██ ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██      ██    ██ ██    ██ ██
██████  ███████          ██    ██   ██ ██   ██ ███████ ██   ██ ██████  ███████ ██       ██████   ██████  ███████

`BS::thread_pool`: a fast, lightweight, modern, and easy-to-use C++17/C++20/C++23 thread pool library
v5.0.0 (2024-12-19)
By Barak Shoshany <baraksh@gmail.com> <https://baraksh.com/>
Copyright (c) 2024 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.1016/j.softx.2024.101687, SoftwareX 26 (2024) 101687, arXiv:2105.00613

This Python script is used in the library's development environment to compile and run the test program using all possible combinations of compilers and C++ standards available in the system, in order to test compatibility.
"""

import argparse
import pathlib
import platform
import shutil
import subprocess
import sys
import time

parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
_ = parser.add_argument("--compile-only", action="store_true", help="only check that the program compiles successfully with all compilers, without running it")
parsed_args: argparse.Namespace = parser.parse_args()


def print_message(message: str) -> None:
    """Print a message with indicator characters to differentiate it from messages from the compile script."""
    print(f"\n          >>>>> {message} <<<<<\n")


# Determine which compilers are available.
vs_pwsh_path: str = r"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1"
compilers: list[str] = []
if pathlib.Path(vs_pwsh_path).exists():
    compilers.append("cl")
if shutil.which("clang++") is not None:
    compilers.append("clang++")
# On macOS, g++ is by default just an alias for clang++, so we skip it.
if shutil.which("g++") is not None and platform.system() != "Darwin":
    compilers.append("g++")

# Compile using all available compilers using all relevant C++ standards.
standards: list[str] = ["c++17", "c++20", "c++23"]
workspace_path: pathlib.Path = pathlib.Path(__file__).parent.parent.resolve()
source_path: pathlib.Path = workspace_path / "tests" / "BS_thread_pool_test.cpp"
script_path: pathlib.Path = workspace_path / "scripts" / "compile_cpp.py"
try:
    compile_start: float = time.perf_counter()
    for compiler in compilers:
        for std in standards:
            print_message(f"Compiling with {compiler} using {std.upper()} standard...")
            warnings_as_errors: list[str] = ["-f/WX"] if compiler == "cl" else ["-f-Werror"]
            command: list[str] = [
                "python" if platform.system() == "Windows" else "python3",
                str(script_path.resolve()),
                str(source_path.resolve()),
                "-c",
                compiler,
                "-s",
                std,
                "-t",
                "release",
                *warnings_as_errors,
                *(["-r"] if not parsed_args.compile_only else []),
                "-p",
                "stdout",
                "-p",
                "log",
                "-p",
                "tests",
                "-v",
            ]
            compile_result: subprocess.CompletedProcess[str] = subprocess.run(
                args=command,
                check=False,
                text=True,
            )
            if compile_result.returncode != 0:
                print_message("Compilation failed, aborting script!")
                sys.exit(compile_result.returncode)
except Exception as exc:
    print_message(f"Could not compile due to exception: {exc}.")
    sys.exit(1)
print_message(f"All tests completed successfully in {time.perf_counter() - compile_start:.2f} seconds.")
