"""
██████  ███████       ████████ ██   ██ ██████  ███████  █████  ██████          ██████   ██████   ██████  ██
██   ██ ██      ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██   ██ ██    ██ ██    ██ ██
██████  ███████          ██    ███████ ██████  █████   ███████ ██   ██         ██████  ██    ██ ██    ██ ██
██   ██      ██ ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██      ██    ██ ██    ██ ██
██████  ███████          ██    ██   ██ ██   ██ ███████ ██   ██ ██████  ███████ ██       ██████   ██████  ███████

`BS::thread_pool`: a fast, lightweight, modern, and easy-to-use C++17/C++20/C++23 thread pool library
v5.1.0 (2026-01-03)
By Barak Shoshany <baraksh@gmail.com> <https://baraksh.com/>
Copyright (c) 2021-2026 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.1016/j.softx.2024.101687, SoftwareX 26 (2024) 101687, arXiv:2105.00613

This Python script can be used to compile simple C++ programs (with only a few source and/or header files) using a variety of compilers, C++ standards, and other options. It also includes support for C++20 modules and C++23 Standard Library modules. It is used in the thread pool library's development environment to compile and run the test program using different compilers and C++ standards. It is not part of the library itself, but users of the library may find it useful, especially if they wish to use the library as a C++20 module.
"""

import argparse
import os
import pathlib
import platform
import re
import shutil
import subprocess
import sys
import time
from enum import Enum
from typing import Any, Never, cast

import yaml  # Install with `pip install pyyaml`.


class Args:
    """A class to collect the command line arguments with proper type checking."""

    def __init__(self, parsed_ns: argparse.Namespace) -> None:
        """Store the parsed arguments."""
        self.files: list[str] = parsed_ns.files
        self.arch: str = parsed_ns.arch
        self.as_module: bool = parsed_ns.as_module
        self.clear_output: bool = parsed_ns.clear_output
        self.compiler: str | None = parsed_ns.compiler
        self.define: list[str] = parsed_ns.define if parsed_ns.define is not None else []
        self.deps: list[str] = parsed_ns.deps if parsed_ns.deps is not None else []
        self.disable_exceptions: str | None = parsed_ns.disable_exceptions
        self.flag: list[str] = parsed_ns.flag if parsed_ns.flag is not None else []
        self.force: bool = parsed_ns.force
        self.ignore_config: bool = parsed_ns.ignore_config
        self.include: list[str] = parsed_ns.include if parsed_ns.include is not None else []
        self.module: list[str] = parsed_ns.module if parsed_ns.module is not None else []
        self.output: str | None = parsed_ns.output
        self.pass_args: list[str] = parsed_ns.pass_args if parsed_ns.pass_args is not None else []
        self.run: bool = parsed_ns.run
        self.std_module: str | None = parsed_ns.std_module
        self.std: str = parsed_ns.std
        self.try_all: bool = parsed_ns.try_all
        self.type: str = parsed_ns.type
        self.verbose: bool = parsed_ns.verbose


# Parse the command-line arguments.
parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
_ = parser.add_argument("files", action="store", nargs="*", help="the source file(s) to compile")
_ = parser.add_argument("-a", "--arch", action="store", choices=["amd64", "arm64"], default="amd64", help="the target architecture (MSVC only)")
_ = parser.add_argument("-b", "--clear-output", action="store_true", help="clear the output folder before compiling (if no source files are specified, just clear and exit)")
_ = parser.add_argument("-c", "--compiler", action="store", choices=["cl", "clang++", "g++"], help="which compiler to use (auto determined if not specified)")
_ = parser.add_argument("-d", "--define", action="append", help="macros to define (use multiple times if more than one) [in addition to those in compile_cpp.yaml]")
_ = parser.add_argument("-e", "--force", action="store_true", help="force recompilation even if the compiled file is up to date")
_ = parser.add_argument("-f", "--flag", action="append", help="extra compiler flags to add (use multiple times if more than one) [in addition to those in compile_cpp.yaml]")
_ = parser.add_argument("-g", "--ignore-config", action="store_true", help="ignore the compile_cpp.yaml configuration file")
_ = parser.add_argument("-i", "--include", action="append", help="the include folder to use (use multiple times if more than one) [in addition to those in compile_cpp.yaml]")
_ = parser.add_argument("-l", "--as-module", action="store_true", help="compile file as module")
_ = parser.add_argument("-m", "--module", action="append", help='C++20 module files to use if desired, in the format "module_name=module_file,dependencies,..." (use multiple times if more than one) [in addition to those in compile_cpp.yaml]')
_ = parser.add_argument("-n", "--deps", action="append", help="dependencies used to detect if recompilation is needed (use multiple times if more than one) [in addition to modules and those in compile_cpp.yaml]")
_ = parser.add_argument("-o", "--output", action="store", help="the output folder (end with / to create, taken from compile_cpp.yaml if not specified) and/or binary name (auto determined if not specified)")
_ = parser.add_argument("-p", "--pass", action="append", dest="pass_args", help="pass command line arguments to the compiled program when running it, if -r/--run is specified (use multiple times if more than one) [in addition to those in compile_cpp.yaml]")
_ = parser.add_argument("-r", "--run", action="store_true", help="run the program after compiling it")
_ = parser.add_argument("-s", "--std", action="store", choices=["c++17", "c++20", "c++23"], default="c++23", help="which C++ standard to use")
_ = parser.add_argument("-t", "--type", action="store", choices=["debug", "release"], default="debug", help="whether to compile in debug or release mode")
_ = parser.add_argument("-u", "--std-module", action="store", help="path to the standard library module (C++23 only, taken from compile_cpp.yaml if not specified, use 'auto' to auto-detect, 'disable' to explicitly disable)")
_ = parser.add_argument("-v", "--verbose", action="store_true", help="whether to print this script's diagnostic messages")
_ = parser.add_argument("-x", "--disable-exceptions", action="store", choices=["true", "false"], help="whether to disable exceptions [overrides compile_cpp.yaml]")
_ = parser.add_argument("-y", "--try-all", action="store_true", help="test compilation using all possible combinations of available compilers and C++ standards (also runs each compiled program if -r/--run is specified)")
args = Args(parser.parse_args(args=None if len(sys.argv) > 1 else ["--help"]))


class ANSI(Enum):
    """Enumeration of ANSI color codes for terminal output."""

    reset = 0
    bold = 1
    dim = 2
    italic = 3
    underline = 4
    invert = 7
    strike = 9
    double_underline = 21
    fg_black = 30
    fg_red = 31
    fg_green = 32
    fg_yellow = 33
    fg_blue = 34
    fg_magenta = 35
    fg_cyan = 36
    fg_white = 37
    bg_black = 40
    bg_red = 41
    bg_green = 42
    bg_yellow = 43
    bg_blue = 44
    bg_magenta = 45
    bg_cyan = 46
    bg_white = 47
    fg_bright_black = 90
    fg_bright_red = 91
    fg_bright_green = 92
    fg_bright_yellow = 93
    fg_bright_blue = 94
    fg_bright_magenta = 95
    fg_bright_cyan = 96
    fg_bright_white = 97
    bg_bright_black = 100
    bg_bright_red = 101
    bg_bright_green = 102
    bg_bright_yellow = 103
    bg_bright_blue = 104
    bg_bright_magenta = 105
    bg_bright_cyan = 106
    bg_bright_white = 107


ANSI_INFO = ANSI.fg_bright_blue
ANSI_ERROR = ANSI.fg_bright_red
ANSI_SUCCESS = ANSI.fg_bright_green
ANSI_SEPARATOR = ANSI.fg_bright_yellow
ANSI_TRY_ALL = ANSI.fg_bright_magenta


no_color: bool = os.environ.get("NO_COLOR", "") != ""


def print_ansi(message: str, *codes: ANSI) -> None:
    """Print a message. Uses the given ANSI codes if specified. Does not add newline. Does not print color if the `NO_COLOR` environment variable is set."""
    if len(codes) > 0 and not no_color:
        seq = ";".join(str(c.value) for c in codes)
        print(f"\033[{seq}m{message}\033[{ANSI.reset.value}m", end="")
    else:
        print(message, end="")


def print_if_verbose(message: str, *codes: ANSI) -> None:
    """Print a message followed by a newline, but only if the verbose flag is set. Uses the given ANSI codes if specified."""
    if args.verbose:
        print_ansi(message + "\n", *codes)


def print_separator() -> None:
    """Print a separator line, but only if the verbose flag is set."""
    print_if_verbose("=" * 60, ANSI_SEPARATOR)


def print_key_values(key: str, values: list[str], length: int) -> None:
    """Print a key-value pair with the key left-aligned to the given length, but only if the verbose flag is set."""
    if args.verbose:
        print_ansi((key + ":").ljust(length), ANSI.fg_bright_white, ANSI.bold)
        if len(values) == 0:
            print("<None>")
        else:
            print(values[0])
            for value in values[1:]:
                print(" " * length + f"{value}")


def print_error_and_exit(message: str) -> Never:
    """Print an error message followed by a newline in red and exit."""
    print_ansi(message + "\n", ANSI_ERROR)
    sys.exit(1)


def relative_or_full_path(path: pathlib.Path) -> pathlib.Path:
    """If the path is relative to the current working directory, return it as a relative path; otherwise, return it as a full path."""
    return path.relative_to(pathlib.Path.cwd()) if path.is_relative_to(pathlib.Path.cwd()) else path


def parse_llvm_version(path_str: str) -> tuple[int, ...] | None:
    """Extract the LLVM version from a path and returns it as a tuple of integers."""
    match: re.Match[str] | None = re.search(r"/llvm[-/](\d+(?:\.\d+)*)", path_str)
    if match is None:
        return None
    return tuple(int(part) for part in match.group(1).split("."))


def get_llvm_std_module(search: str) -> str | None:
    """Get the path to the LLVM standard library module on Linux or macOS. If multiple paths are found, return the one with the latest LLVM version."""
    try:
        llvm_path: str = subprocess.check_output([f"find {search} -name std.cppm"], text=True, shell=True).strip()
    except subprocess.CalledProcessError:
        llvm_path: str = ""
    all_paths: list[str] = [line for line in llvm_path.splitlines() if line.strip()]
    if len(all_paths) == 0:
        return None
    return max(all_paths, key=lambda p: parse_llvm_version(p) or (0,))


def find_vs_path() -> pathlib.Path | None:
    """Find the Visual Studio installation path, if it exists."""
    if platform.system() != "Windows":
        return None
    pf86: str = os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")
    pf: str = os.environ.get("ProgramFiles", r"C:\Program Files")
    try_paths: list[pathlib.Path] = [pathlib.Path(p) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe" for p in [pf86, pf]]
    vswhere: pathlib.Path | None = next((p for p in try_paths if p.exists()), None)
    if vswhere is None:
        return None
    try:
        install_root: str = subprocess.check_output(
            [
                str(vswhere),
                "-latest",
                "-property",
                "installationPath",
            ],
            text=True,
        ).strip()
    except subprocess.CalledProcessError:
        return None
    if install_root == "":
        return None
    return pathlib.Path(install_root)


def get_module_flags(name: str, module_output_path: pathlib.Path) -> list[str]:
    """Get the appropriate flags to import a module with the given name from the given output path."""
    if compiler == "cl":
        return ["/reference", f"{name}={module_output_path.resolve()}", *([str(build_folder / f"{module_output_path.stem}.obj")] if (name != "std" or not args.as_module) else [])]
    if compiler == "clang++":
        return [f"-fmodule-file={name}={module_output_path.resolve()}"]
    # Otherwise, compiler == "g++".
    return ["-fmodules", f"-fmodule-mapper=|@g++-mapper-server -r{build_folder}"]


def compile_module(name: str, paths: list[pathlib.Path], module_output_path: pathlib.Path) -> bool:
    """Compile a module with the given name. The first path in `paths` is the module file, and the rest are its dependencies. Returns True if the module was recompiled, False if it was up to date."""
    module_file: pathlib.Path = paths[0]
    if module_output_path.exists():
        module_output_mod: float = module_output_path.stat().st_mtime
        if not any((path.exists() and path.stat().st_mtime > module_output_mod) for path in paths):
            print_if_verbose(f'Module "{name}" is up to date, skipping compilation.', ANSI_INFO)
            return False
    try:
        module_command: list[str] = [
            sys.executable,
            str(pathlib.Path(__file__).resolve()),
            # In the special case of GCC and the `std` module, `bits/std.cc` must be entered without the full path.
            str(module_file) if not (compiler == "g++" and name == "std") else gcc_std_path,
            f"--output={module_output_path.resolve()}",
            f"--arch={args.arch}",
            f"--compiler={compiler}",
            f"--std={args.std}",
            f"--type={args.type}",
            # Note: Not adding the extra options from the configuration file, since they will be added by the script anyway.
            *[f"--define={define}" for define in args.define],
            *[f"--include={include}" for include in args.include],
            *[f"--flag={flag}" for flag in args.flag],
            # For the `std` module in GCC, we also need to add `-fsearch-include-path`.
            *(["--flag=-fsearch-include-path"] if (compiler == "g++" and name == "std") else []),
            # If compiling the `std` module itself, we need to pass `-u=disable` to avoid infinite recursion. Otherwise, we pass along the specified module path if it exists.
            *(["--std-module=disable"] if name == "std" else [f"--std-module={std_module}"] if std_module is not None else []),
            "--as-module",
            *(["--verbose"] if args.verbose else []),
            *(["--disable-exceptions=true"] if disable_exceptions else ["--disable-exceptions=false"]),
        ]
        print_separator()
        print_if_verbose(f'Compiling module "{name}" with command: {subprocess.list2cmdline(module_command)}', ANSI_INFO)
        _ = sys.stdout.flush()
        module_result = subprocess.run(
            args=module_command,
            check=False,
            text=True,
        )
        if module_result.returncode != 0:
            print_error_and_exit(f"Module compilation failed with return code: {module_result.returncode}.")
    except Exception as exc:
        print_error_and_exit(f"Could not compile module due to exception: {exc}.")
    else:
        return True


# Collect the full path(s) to the source file(s).
source_paths: list[pathlib.Path] = [pathlib.Path(file) for file in args.files]

# If the `try_all` flag is set, run this script recursively using all possible combinations of compilers and C++ standards available in the system.
if args.try_all:
    if "-c" in sys.argv or "--compiler" in sys.argv:
        print_error_and_exit("Error: The -y/--try-all flag cannot be used together with the -c/--compiler flag.")
    if "-s" in sys.argv or "--std" in sys.argv:
        print_error_and_exit("Error: The -y/--try-all flag cannot be used together with the -s/--std flag.")
    # We pass all the arguments to the child processes except for the one that enables this mode, to avoid infinite recursion.
    child_args: list[str] = [arg for arg in sys.argv[1:] if arg not in ("-y", "--try-all")]

    # Determine which compilers are available.
    compilers: list[str] = []
    if find_vs_path() is not None:
        compilers.append("cl")
    if shutil.which("clang++") is not None:
        compilers.append("clang++")
    # On macOS, g++ is by default just an alias for clang++, so we skip it.
    if shutil.which("g++") is not None and platform.system() != "Darwin":
        compilers.append("g++")

    # Compile using all available compilers using all relevant C++ standards.
    standards: list[str] = ["c++17", "c++20", "c++23"]
    workspace_path: pathlib.Path = pathlib.Path(__file__).parent.parent.resolve()
    try:
        compile_start: float = time.perf_counter()
        for compiler in compilers:
            for std in standards:
                warnings_as_errors: list[str] = ["-f/WX"] if compiler == "cl" else ["-f-Werror"]
                command: list[str] = [
                    sys.executable,
                    str(pathlib.Path(__file__).resolve()),
                    "-c",
                    compiler,
                    "-s",
                    std,
                    *warnings_as_errors,
                    *child_args,
                ]
                print_if_verbose(f"Compiling with {compiler} using {std.upper()} standard with command: {subprocess.list2cmdline(command)}", ANSI_TRY_ALL)
                compile_result = subprocess.run(
                    args=command,
                    check=False,
                    text=True,
                )
                if compile_result.returncode != 0:
                    print_if_verbose("Compilation failed, aborting!", ANSI_TRY_ALL)
                    sys.exit(1)
    except Exception as exc:
        print_if_verbose(f"Could not compile due to exception: {exc}, aborting!", ANSI_TRY_ALL)
        sys.exit(1)
    print_if_verbose(f"All compilations completed successfully in {time.perf_counter() - compile_start:.2f} seconds.", ANSI_TRY_ALL)
    sys.exit(0)

# Determine the compiler if it is not given.
compiler: str = ""
vs_path: pathlib.Path | None = find_vs_path()
if args.compiler is not None:
    compiler = args.compiler
elif platform.system() == "Windows":
    # On Windows, we default to MSVC if the Visual Studio installation path exists, otherwise we fall back to Clang, and then GCC.
    if vs_path is not None and vs_path.exists():
        compiler = "cl"
    elif shutil.which("clang++") is not None:
        compiler = "clang++"
    elif shutil.which("g++") is not None:
        compiler = "g++"
elif platform.system() == "Linux":
    # On Linux, we default to GCC if it is available, otherwise we fall back to Clang.
    if shutil.which("g++") is not None:
        compiler = "g++"
    elif shutil.which("clang++") is not None:
        compiler = "clang++"
elif platform.system() == "Darwin" and shutil.which("clang++") is not None:
    # On macOS, we just check if Clang is available.
    compiler = "clang++"
if compiler == "":
    print_error_and_exit("Error: No compiler found!")

# If a file named `compile_cpp.yaml` exists in the current working directory, read the configuration from it. All options are added to those from the command line, except the output file/folder and path to the standard library module, which are only used if not provided in the command line. Note that all folders should be specified relative to the current working directory.
defines: list[str] = args.define[:]
deps: list[str] = args.deps[:]
disable_exceptions: bool = args.disable_exceptions == "true"
flags: list[str] = args.flag[:]
includes: list[str] = args.include[:]
try:
    modules: dict[str, list[str]] = {name: files.split(",") for module in args.module for name, files in (module.split("="),)}
except ValueError:
    print_error_and_exit('Error: Module specification must be in the format "module_name=module_file,dependencies,...".')
output: str | None = args.output
pass_args: list[str] = args.pass_args[:]
std_module: str | None = args.std_module

compile_yaml: pathlib.Path = pathlib.Path.cwd() / "compile_cpp.yaml"
if not args.ignore_config and compile_yaml.exists():
    with compile_yaml.open("r") as file:
        raw_config: dict[str, Any] | Any = yaml.safe_load(file) or {}
        if not isinstance(raw_config, dict):
            print_error_and_exit("Error: compile_cpp.yaml must be a dictionary of options.")
        compile_config = cast("dict[str, Any]", raw_config)
        if "defines" in compile_config:
            defines.extend(compile_config["defines"])
        if "deps" in compile_config:
            deps.extend(compile_config["deps"])
        if args.disable_exceptions is None and "disable_exceptions" in compile_config:
            disable_exceptions = compile_config["disable_exceptions"] is True
        if "flags" in compile_config and compiler in compile_config["flags"]:
            flags.extend(compile_config["flags"][compiler])
        if "includes" in compile_config:
            includes.extend(compile_config["includes"])
        if "modules" in compile_config:
            modules.update(compile_config["modules"])
        if output is None and "output" in compile_config:
            output = compile_config["output"]
        if "pass_args" in compile_config:
            pass_args.extend(compile_config["pass_args"])
        if std_module is None and "std_module" in compile_config and platform.system() in compile_config["std_module"] and compiler in compile_config["std_module"][platform.system()] and len(compile_config["std_module"][platform.system()][compiler]) > 0:
            std_module = compile_config["std_module"][platform.system()][compiler]

# Determine the name of the binary file and the build folder.
binary_path: pathlib.Path | None = None
build_folder: pathlib.Path
auto_binary: bool = False
if output is not None:
    # Calculate the output path relative to the current working directory. Note that if the path is absolute, `pathlib` will automatically use the absolute path instead of a relative path.
    output_path: pathlib.Path = (pathlib.Path.cwd() / output).resolve()
    if output.endswith(("/", "\\")) or output_path.is_dir():
        # If the output path is a directory, we use it as the build folder, and automatically determine the name of the binary file.
        build_folder = output_path.resolve()
        auto_binary = True
    elif output_path.is_absolute():
        # If the output path is an absolute path to a file, the build folder is the file's folder.
        build_folder = output_path.parent.resolve()
        binary_path = output_path
    else:
        # If the output path is just a file name, the build folder is the current working directory by default.
        build_folder = pathlib.Path.cwd()
        binary_path = build_folder / output_path
else:
    # If there is no output path at all, the build folder is the current working directory by default, and we automatically determine the name of the binary file.
    build_folder = pathlib.Path.cwd()
    auto_binary = True

# Clear and recreate the build folder if requested.
if args.clear_output:
    if build_folder == pathlib.Path.cwd():
        print_error_and_exit("Error: Cannot clear the output path if it is the current working directory.")
    if build_folder.exists():
        print_if_verbose(f"Clearing output folder: {relative_or_full_path(build_folder)}", ANSI_INFO)
        shutil.rmtree(build_folder)
        pathlib.Path(build_folder).mkdir(exist_ok=True, parents=True)
        print_if_verbose("Cleared successfully!", ANSI_SUCCESS)
    else:
        print_if_verbose(f"Creating empty output folder: {relative_or_full_path(build_folder)}", ANSI_INFO)
        pathlib.Path(build_folder).mkdir(exist_ok=True, parents=True)
        print_if_verbose("Created successfully!", ANSI_SUCCESS)
    if len(source_paths) == 0:
        # If no source files are specified, just clear and exit.
        sys.exit(0)
else:
    # Otherwise, just create the build folder if it does not exist.
    pathlib.Path(build_folder).mkdir(exist_ok=True, parents=True)

# Add the appropriate flags to disable exceptions if requested.
if disable_exceptions:
    if compiler == "cl":
        flags.extend(["/EHs-c-", "/D_HAS_EXCEPTIONS=0"])
    else:
        flags.append("-fno-exceptions")
elif compiler == "cl":
    flags.append("/EHsc")

# Importing the C++ Standard Library is only available in C++23 mode. If "disable" is specified for the standard library module, we skip it; this is used to avoid infinite recursion.
use_std_module: bool = not (std_module is None or std_module == "disable" or args.std != "c++23")

if use_std_module and std_module is not None:
    modules = {"std": [std_module], **modules}

# Figure out the path to the std module, if relevant.
gcc_std_path: str = "bits/std.cc"
if use_std_module and "std" in modules and modules["std"][0].strip() == "auto":
    success: bool = False
    if platform.system() == "Windows" and compiler == "cl" and vs_path is not None:
        vc_version_path: pathlib.Path = vs_path / "VC" / "Auxiliary" / "Build" / "Microsoft.VCToolsVersion.default.txt"
        if vc_version_path.exists():
            with vc_version_path.open("r", encoding="utf-8") as vc_version_file:
                vc_version: str = vc_version_file.read().strip()
                vc_tools_path: pathlib.Path = vs_path / "VC" / "Tools" / "MSVC" / vc_version
                modules["std"][0] = rf"{vc_tools_path}\modules\std.ixx"
                success = True
    elif compiler == "clang++":
        # Note: The Clang `std` module is only available with libc++.
        if platform.system() == "Windows":
            # On Windows, libc++ is most likely installed via MSYS2, so the `std` module should be at `C:\msys64\clang64\share\libc++\v1\std.cppm`. We calculate it relative to the path where `clang++.exe` is located, in case the MSYS2 installation folder is different.
            clang_path: str | None = shutil.which("clang++")
            if clang_path:
                std_path: pathlib.Path = pathlib.Path(clang_path).parent.parent / "share" / "libc++" / "v1" / "std.cppm"
                if std_path.exists():
                    modules["std"][0] = str(std_path.absolute())
                    success = True
        elif platform.system() == "Darwin":
            # On macOS, the `std` module should be at `/usr/local/Cellar/llvm/<LLVM full version>/share/libc++/v1/std.cppm`.
            llvm_std_path: str | None = get_llvm_std_module("/usr/local/Cellar/llvm")
            if llvm_std_path:
                modules["std"][0] = llvm_std_path
                success = True
        elif platform.system() == "Linux":
            # On Linux, the `std` module should be at `/usr/lib/llvm-<LLVM major version>/share/libc++/v1/std.cppm`.
            llvm_std_path: str | None = get_llvm_std_module("/usr/lib/llvm-*")
            if llvm_std_path:
                modules["std"][0] = llvm_std_path
                success = True
    else:  # compiler == "g++"
        # In GCC the module file is always at `bits/std.cc`.
        modules["std"][0] = gcc_std_path
        success = True
    if not success:
        print_error_and_exit('Error: "auto" specified for the standard library module path, but the script could not locate it. Please specify the path manually.')

# Determine the appropriate extension for modules.
module_extension: str = ""
if compiler == "cl":
    module_extension = ".ifc"
elif compiler == "clang++":
    module_extension = ".pcm"
else:  # compiler == "g++"
    # For GCC, the extension is `.gcm`, but we do not have control over that; however, we also create an object file with the `.o` extension so we can check if the module is up to date (see below).
    module_extension = ".o"

# If we are compiling as a module, add the appropriate flags.
if args.as_module:
    if compiler == "cl":
        flags.extend(["/interface", "/TP", "/c"])
    elif compiler == "clang++":
        flags.extend(["--precompile", "-Wno-include-angled-in-module-purview", "-Wno-reserved-module-identifier", "-xc++-module"])
    else:  # compiler == "g++"
        # Note: Creating an object file can be disabled with `-fmodule-only`, and it doesn't seem like the object file is actually needed, only the `.gcm` file. However, we create the object file anyway so we can check if the module is up to date, since there appears to be no way to control the name of the `.gcm` file.
        flags.extend(["-fmodules", "-c", f"-fmodule-mapper=|@g++-mapper-server -r{build_folder}", "-xc++"])

# If the user did not provide an output file name, we use the name of the first source file, appending the compiler, mode, and C++ standard, as well as the appropriate extension.
short_compiler: str = "clang" if compiler == "clang++" else "gcc" if compiler == "g++" else "msvc"
suffix: str = f"{args.type}-{short_compiler}-cpp{args.std[-2:]}"
if auto_binary:
    extension: str = module_extension if args.as_module else ".exe" if platform.system() == "Windows" else ""
    module_indicator: str = "module_" if args.as_module else ""
    binary_name: str = f"{source_paths[0].stem}_{module_indicator}{suffix}{extension}"
    binary_path = build_folder / binary_name
if binary_path is None:
    print_error_and_exit("Error: Could not determine binary file name!")

# If modules are specified, pre-compile them by calling this script recursively. If using the `std` module, pre-compile it first. If this is a module itself, don't compile any other modules to avoid infinite recursion. (Note: This assumes that modules do not depend on each other.)
recompiled_modules: bool = False
module_paths: dict[str, list[pathlib.Path]] = {name: [(pathlib.Path.cwd() / file).resolve() for file in files] for name, files in modules.items()}
module_output_paths: dict[str, pathlib.Path] = {name: build_folder / f"{path[0].stem}_module_{suffix}{module_extension}" for name, path in module_paths.items()}
if use_std_module and "std" in modules:
    flags.extend(get_module_flags("std", module_output_paths["std"]))
if not args.as_module:
    if use_std_module and "std" in modules:
        recompiled_modules |= compile_module("std", [(pathlib.Path.cwd() / modules["std"][0]).resolve()], module_output_paths["std"])
    if len(modules) > 0 and (args.std in ["c++20", "c++23"]):
        for n, p in module_paths.items():
            if n == "std":
                continue
            flags.extend(get_module_flags(n, module_output_paths[n]))
            recompiled_modules |= compile_module(n, p, module_output_paths[n])
    print_if_verbose("Compiling program...", ANSI_INFO)

# If the output binary already exists, check if we need to recompile based on whether any of the source files or their dependencies have changed. If we recompiled any modules, we need to recompile anyway, so we don't need to check the dependencies. The `force` flag overrides this check.
need_recompile: bool = True
deps_paths: list[pathlib.Path] = [(pathlib.Path.cwd() / file).resolve() for file in deps]
if not args.force and not recompiled_modules and binary_path.exists():
    binary_mod: float = binary_path.stat().st_mtime
    if not any((path.exists() and path.stat().st_mtime > binary_mod) for path in (source_paths + deps_paths)):
        print_if_verbose("Binary is up to date, skipping compilation.", ANSI_INFO)
        need_recompile = False

if need_recompile:
    include_paths: list[pathlib.Path] = [(pathlib.Path.cwd() / folder).resolve() for folder in includes]
    # On macOS, make sure we are using Homebrew Clang, if available, instead of Apple Clang, which does not support C++20 modules.
    compiler_path: str | None
    if compiler == "clang++" and platform.system() == "Darwin":
        compiler_path = "/usr/local/opt/llvm/bin/clang++"
        if not pathlib.Path(compiler_path).exists():
            compiler_path = shutil.which(compiler)
    else:
        compiler_path = shutil.which(compiler)

    # Determine the command to execute based on the chosen compiler and parameters.
    command: list[str]
    if compiler == "clang++":
        command = [
            compiler_path if compiler_path is not None else compiler,
            *flags,
            *[str(path.resolve()) for path in source_paths],
            f"-std={args.std}",
            "-g3" if args.type == "debug" else "-O3",
            "-o",
            str(binary_path),
            *[item for include in [["-I", str(path.resolve())] for path in include_paths] for item in include],
            *([f"-D{define}" for define in defines]),
        ]
    elif compiler == "g++":
        command = [
            compiler_path if compiler_path is not None else compiler,
            *flags,
            # In the special case of GCC and the `std` module, `bits/std.cc` must be entered without the full path.
            *[(str(path.resolve()) if path != pathlib.Path(gcc_std_path) else gcc_std_path) for path in source_paths],
            f"-std={args.std}",
            "-ggdb3" if args.type == "debug" else "-O3",
            "-o",
            str(binary_path),
            *[item for include in [["-I", str(path.resolve())] for path in include_paths] for item in include],
            *([f"-D{define}" for define in defines]),
        ]
    else:  # compiler == "cl"
        command = [
            compiler_path if compiler_path is not None else compiler,
            *flags,
            *[str(path.resolve()) for path in source_paths],
            f"/std:{'c++latest' if args.std == 'c++23' else args.std}",
            *(["/Zi", f"/Fd:{binary_path.with_suffix('.pdb')}"] if args.type == "debug" else ["/O2"]),
            *([f"/Fe:{binary_path}"] if not args.as_module else ["/ifcOutput", str(binary_path)]),
            # If compiling multiple source files, we cannot specify the names of each object file, only the output folder.
            f"/Fo:{binary_path.with_suffix('.obj')}" if len(source_paths) == 1 else f"/Fo:{build_folder}\\",
            *[item for include in [["/I", str(path.resolve())] for path in include_paths] for item in include],
            "/permissive-",
            "/nologo",
            "/Zc:__cplusplus",
            *([f"/D{define}" for define in defines]),
        ]

    # For MSVC we also need to invoke the Visual Studio Developer PowerShell script.
    if compiler == "cl" and vs_path is not None:
        vs_pwsh_path: pathlib.Path = vs_path / "Common7" / "Tools" / "Launch-VsDevShell.ps1"
        command = ["pwsh.exe", "-NoProfile", "-ExecutionPolicy", "Bypass", "-Command", f"& '{vs_pwsh_path}' -Arch {args.arch} -HostArch {args.arch} | Out-Null; {subprocess.list2cmdline(command)}"]

    print_separator()
    max_length = 16
    print_key_values("Compiler", [compiler], max_length)
    print_key_values("C++ Standard", [args.std.upper()], max_length)
    print_key_values("Type", [args.type.title()], max_length)
    print_key_values("Source file(s)", [str(relative_or_full_path(path)) for path in source_paths], max_length)
    print_key_values("Defines", defines, max_length)
    print_key_values("Dependencies", [str(relative_or_full_path(path)) for path in deps_paths], max_length)
    print_key_values("Flags", flags, max_length)
    print_key_values("Includes", ([str(relative_or_full_path(path)) for path in include_paths]), max_length)
    if args.std in ["c++20", "c++23"]:
        print_key_values("Modules", [f"{name}={relative_or_full_path(path[0])}" for name, path in module_paths.items()] if not args.as_module else ["<None>"], max_length)
    else:
        print_key_values("Modules", ["<Unused in C++17 mode>"], max_length)
    print_key_values("Build folder", [str(relative_or_full_path(build_folder))], max_length)
    print_key_values("Binary file", [str(relative_or_full_path(binary_path))], max_length)
    print_key_values("Command", [subprocess.list2cmdline(command)], max_length)
    print_separator()

    # Perform the actual compilation.
    print_if_verbose("Compiling...", ANSI_INFO)
    try:
        _ = sys.stdout.flush()
        compile_start: float = time.perf_counter()
        compile_result = subprocess.run(
            args=command,
            check=False,
            text=True,
        )
        if compile_result.returncode == 0:
            print_if_verbose(f"Compilation completed successfully in {time.perf_counter() - compile_start:.2f} seconds.", ANSI_SUCCESS)
        else:
            print_error_and_exit(f"Compilation failed with return code: {compile_result.returncode}.")
    except Exception as exc:
        print_error_and_exit(f"Could not compile due to exception: {exc}.")

if args.run:
    # We run the program in the build folder.
    os.chdir(build_folder)
    print_if_verbose(f"Running program{f' with arguments {" ".join(pass_args)}' if len(pass_args) > 0 else ''}...", ANSI_INFO)
    print_separator()
    try:
        _ = sys.stdout.flush()
        run_start: float = time.perf_counter()
        run_result: subprocess.CompletedProcess[str] = subprocess.run(
            args=[binary_path, *pass_args],
            check=False,
            text=True,
        )
        print_separator()
        if run_result.returncode == 0:
            print_if_verbose(f"Program executed successfully in {time.perf_counter() - run_start:.2f} seconds.", ANSI_SUCCESS)
        else:
            print_error_and_exit(f"Program failed with return code: {run_result.returncode}.")
    except Exception as exc:
        print_separator()
        print_error_and_exit(f"Could not run program due to exception: {exc}.")
