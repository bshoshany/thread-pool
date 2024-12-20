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

This Python script can be used to compile simple C++ programs (with only a few source and/or header files) using a variety of compilers, C++ standards, and other options. It also includes support for C++20 modules and C++23 Standard Library modules. It is used in the thread pool library's development environment to compile and run the test program using different compilers and C++ standards. It is not part of the library itself, but users of the library may find it useful, especially if they wish to use the library as a C++20 module.
"""

import argparse
import os
import pathlib
import platform
import shutil
import subprocess
import sys
import time

import yaml  # Install with `pip install pyyaml`.

separator: str = "=" * 60


class Args:
    """A class to collect the command line arguments with proper type checking."""

    def __init__(self, parsed_ns: argparse.Namespace) -> None:
        """Store the parsed arguments."""
        self.files: list[str] = parsed_ns.files
        self.arch: str = parsed_ns.arch
        self.as_module: bool = parsed_ns.as_module
        self.compiler: str | None = parsed_ns.compiler
        self.define: list[str] = parsed_ns.define if parsed_ns.define is not None else []
        self.flag: list[str] = parsed_ns.flag if parsed_ns.flag is not None else []
        self.ignore_yaml: bool = parsed_ns.ignore_yaml
        self.include: list[str] = parsed_ns.include if parsed_ns.include is not None else []
        self.module: list[str] = parsed_ns.module if parsed_ns.module is not None else []
        self.output: str | None = parsed_ns.output
        self.pass_args: list[str] = parsed_ns.pass_args if parsed_ns.pass_args is not None else []
        self.run: bool = parsed_ns.run
        self.std_module: str | None = parsed_ns.std_module
        self.std: str = parsed_ns.std
        self.type: str = parsed_ns.type
        self.verbose: bool = parsed_ns.verbose


# Parse the command-line arguments.
parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
_ = parser.add_argument("files", nargs="+", help="the source file(s) to compile")
_ = parser.add_argument("-a", "--arch", choices=["amd64", "arm64"], default="amd64", help="the target architecture (MSVC only)")
_ = parser.add_argument("-c", "--compiler", choices=["cl", "clang++", "g++"], help="which compiler to use (auto determined if not specified)")
_ = parser.add_argument("-d", "--define", action="append", help="macros to define (use multiple times if more than one) [in addition to those in compile_cpp.yaml]")
_ = parser.add_argument("-f", "--flag", action="append", help="extra compiler flags to add (use multiple times if more than one) [in addition to those in compile_cpp.yaml]")
_ = parser.add_argument("-g", "--ignore-yaml", action="store_true", help="ignore the compile_cpp.yaml file")
_ = parser.add_argument("-i", "--include", action="append", help="the include folder to use (use multiple times if more than one) [in addition to those in compile_cpp.yaml]")
_ = parser.add_argument("-l", "--as-module", action="store_true", help="Compile file as module")
_ = parser.add_argument("-m", "--module", action="append", help='C++20 module files to use if desired, in the format "module_name=module_file,dependent_files,..." (use multiple times if more than one) [in addition to those in compile_cpp.yaml]')
_ = parser.add_argument("-o", "--output", help="the output folder (end with / to create, taken from compile_cpp.yaml if not specified) and/or executable name (auto determined if not specified)")
_ = parser.add_argument("-p", "--pass", action="append", dest="pass_args", help="pass command line arguments to the compiled program when running it, if -r is specified (use multiple times if more than one) [in addition to those in compile_cpp.yaml]")
_ = parser.add_argument("-r", "--run", action="store_true", help="run the program after compiling it")
_ = parser.add_argument("-s", "--std", choices=["c++17", "c++20", "c++23"], default="c++23", help="which C++ standard to use")
_ = parser.add_argument("-t", "--type", choices=["debug", "release"], default="debug", help="whether to compile in debug or release mode")
_ = parser.add_argument("-u", "--std-module", help="path to the standard library module (C++23 only, taken from compile_cpp.yaml if not specified, use 'auto' to auto-detect, 'disable' to explicitly disable)")
_ = parser.add_argument("-v", "--verbose", action="store_true", help="whether to print this script's diagnostic messages")
args = Args(parser.parse_args(args=None if len(sys.argv) > 1 else ["--help"]))


def print_if_verbose(message: str) -> None:
    """Print a message, but only if the verbose flag is set."""
    if args.verbose:
        print(message)


# Collect the full path(s) to the source file(s).
source_paths: list[pathlib.Path] = [pathlib.Path(file).resolve() for file in args.files]

# Determine the compiler if it is not given.
compiler: str = ""
vs_pwsh_path: str = r"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1"
if args.compiler is not None:
    compiler = args.compiler
else:  # noqa: PLR5501
    # On Windows, we default to MSVC if the Visual Studio Developer PowerShell script exists, otherwise we fall back to Clang, and then GCC.
    if platform.system() == "Windows":
        if pathlib.Path(vs_pwsh_path).exists():
            compiler = "cl"
        elif shutil.which("clang++") is not None:
            compiler = "clang++"
        elif shutil.which("g++") is not None:
            compiler = "g++"
    # On Linux, we default to GCC if it is available, otherwise we fall back to Clang.
    elif platform.system() == "Linux":
        if shutil.which("g++") is not None:
            compiler = "g++"
        elif shutil.which("clang++") is not None:
            compiler = "clang++"
    # On macOS, we just check if Clang is available.
    elif platform.system() == "Darwin" and shutil.which("clang++") is not None:
        compiler = "clang++"
if compiler == "":
    sys.exit("Error: No compiler found!")

# If a file named `compile_cpp.yaml` exists in the current working directory, read the configuration from it. All options are added to those from the command line, except the output file/folder and path to the standard library module, which are only used if not provided in the command line. Note that all folders should be specified relative to the current working directory.
defines: list[str] = args.define[:]
flags: list[str] = args.flag[:]
includes: list[str] = args.include[:]
modules: dict[str, list[str]] = {name: files.split(",") for module in args.module for name, files in (module.split("="),)}
output: str | None = args.output
pass_args: list[str] = args.pass_args[:]
compile_yaml: pathlib.Path = pathlib.Path.cwd() / "compile_cpp.yaml"
std_module: str | None = args.std_module

if not args.ignore_yaml and compile_yaml.exists():
    with compile_yaml.open("r") as file:
        compile_config = yaml.safe_load(file)
        if "defines" in compile_config:
            defines.extend(compile_config["defines"])
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

# Importing the C++ Standard Library is only available in C++23 mode, and currently only officially supported by MSVC and Clang. If "disable" is specified for the standard library module, we skip it; this is used to avoid infinite recursion.
use_std_module: bool = not (std_module is None or std_module == "disable" or args.std != "c++23" or compiler not in ["cl", "clang++"])

if use_std_module and std_module is not None:
    modules = {"std": [std_module], **modules}

# Figure out the path to the MSVC or Clang std module, if relevant.
if use_std_module and "std" in modules and modules["std"][0].strip() == "auto":
    success: bool = False
    if platform.system() == "Windows" and compiler == "cl":
        vc_version_path: pathlib.Path = pathlib.Path(r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt")
        if vc_version_path.exists():
            with vc_version_path.open("r", encoding="utf-8") as vc_version_file:
                vc_version: str = vc_version_file.read().strip()
                vc_tools_path: str = rf"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\{vc_version}"
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
            # On macOS, the `std` module should be at `/usr/local/Cellar/llvm/<LLVM full version>/share/libc++/v1/std.cppm`. We use `find` to locate it without needing to know the exact LLVM version.
            llvm_path: str = subprocess.check_output(["find", "/usr/local/Cellar/llvm", "-name", "std.cppm"], text=True).strip()
            if llvm_path:
                modules["std"][0] = llvm_path
                success = True
        elif platform.system() == "Linux":
            # On Linux, the `std` module should be at `/usr/lib/llvm-<LLVM major version>/share/libc++/v1/std.cppm`. We use `find` to locate it without needing to know the exact LLVM version.
            llvm_path: str = subprocess.check_output(["find", "/usr/lib", "-name", "std.cppm"], text=True).strip()
            if llvm_path:
                modules["std"][0] = llvm_path
                success = True
    if not success:
        sys.exit('Error: "auto" specified for the standard library module path, but the script could not locate it. Please specify the path manually.')

# Determine the name of the executable file and the build folder.
executable_path: pathlib.Path | None = None
build_folder: pathlib.Path
auto_executable: bool = False
if output is not None:
    # Calculate the output path relative to the current working directory. Note that if the path is absolute, `pathlib` will automatically use the absolute path instead of a relative path.
    output_path: pathlib.Path = (pathlib.Path.cwd() / output).resolve()
    if output.endswith(("/", "\\")) or output_path.is_dir():
        # If the output path is a directory, we use it as the build folder, and automatically determine the name of the executable file.
        build_folder = output_path.resolve()
        auto_executable = True
    elif output_path.is_absolute():
        # If the output path is an absolute path to a file, the build folder is the file's folder.
        build_folder = output_path.parent.resolve()
        executable_path = output_path
    else:
        # If the output path is just a file name, the build folder is the current working directory by default.
        build_folder = pathlib.Path.cwd()
        executable_path = build_folder / output_path
else:
    # If there is no output path at all, the build folder is the current working directory by default, and we automatically determine the name of the executable file.
    build_folder = pathlib.Path.cwd()
    auto_executable = True

# If the user did not provide an output file name, we use the name of the first source file, appending the compiler, mode, and C++ standard.
short_compiler: str = "clang" if compiler == "clang++" else "gcc" if compiler == "g++" else "msvc"
suffix: str = f"{args.type}-{short_compiler}-cpp{args.std[-2:]}"
if auto_executable:
    extension: str = ".exe" if platform.system() == "Windows" else ""
    executable: str = f"{source_paths[0].stem}_{suffix}{extension}"
    executable_path = build_folder / executable
if executable_path is None:
    sys.exit("Error: Could not determine executable file!")

# Determine if the build folder exists, and create it if not.
if not pathlib.Path(build_folder).exists():
    pathlib.Path(build_folder).mkdir()

# If modules are specified, pre-compile them by calling this script recursively, unless the current file is itself a module. As a special case, if the std module is used, we add it even if the current file is a module, in case the module itself wants to import the std module.
module_paths: dict[str, list[pathlib.Path]] = {}
if not args.as_module:
    module_paths = {name: [(pathlib.Path.cwd() / file).resolve() for file in files] for name, files in modules.items()}
elif use_std_module and "std" in modules:
    module_paths = {"std": [(pathlib.Path.cwd() / modules["std"][0]).resolve()]}
if len(modules) > 0 and (args.std in ["c++20", "c++23"]):
    for name, paths in module_paths.items():
        module_flags: list[str] = args.flag[:]
        module_extension: str
        if compiler == "cl":
            module_extension = ".ifc"
            module_flags = ["/interface", "/TP", "/c"]
        elif compiler == "clang++":
            module_extension = ".pcm"
            module_flags = ["--precompile", "-Wno-include-angled-in-module-purview", "-Wno-reserved-module-identifier", "-xc++-module"]
        else:  # compiler == "g++"
            # Note: Creating an object file can be disabled with `-fmodule-only`, and it doesn't seem like the object file is actually needed, only the `.gcm` file. However, we create the object file anyway so we can check if the module is up to date, since there appears to be no way to control the name of the `.gcm` file.
            module_extension = ".o"
            module_flags = ["-fmodules-ts", "-xc++", "-c", f"-fmodule-mapper=|@g++-mapper-server -r{build_folder}"]
        # Only the first file on the list needs to be compiled; the rest are dependencies that are only used to check if the compiled module is up to date.
        module_file: pathlib.Path = paths[0]
        module_output_path: pathlib.Path = build_folder / f"{module_file.stem}_module_{suffix}{module_extension}"
        if compiler == "cl":
            flags.extend(["/reference", f"{name}={module_output_path.resolve()}"])
            if name != "std" or not args.as_module:
                flags.append(str(build_folder / f"{module_output_path.stem}.obj"))
        elif compiler == "clang++":
            flags.append(f"-fmodule-file={name}={module_output_path.resolve()}")
        else:  # compiler == "g++"
            flags.append("-fmodules-ts")
            flags.append(f"-fmodule-mapper=|@g++-mapper-server -r{build_folder}")
        if module_output_path.exists():
            module_output_mod: float = module_output_path.stat().st_mtime
            if not any((path.exists() and path.stat().st_mtime > module_output_mod) for path in paths):
                print_if_verbose(f'Module "{name}" is up to date, skipping compilation.')
                continue
        try:
            command: list[str] = [
                "python" if platform.system() == "Windows" else "python3",
                str(pathlib.Path(__file__).resolve()),
                str(module_file),
                f"-o={module_output_path.resolve()}",
                f"-a={args.arch}",
                f"-c={compiler}",
                f"-s={args.std}",
                f"-t={args.type}",
                # Note: Not adding the extra options from the configuration file, since they will be added by the script anyway.
                *[f"-d={define}" for define in args.define],
                *[f"-i={include}" for include in args.include],
                *[f"-f={flag}" for flag in (args.flag + module_flags)],
                # If compiling the `std` module itself, we need to pass `-u disable` to avoid infinite recursion. Otherwise, we pass along the specified module path if it exists.
                *(["-u=disable"] if name == "std" else [f"-u={std_module}"] if std_module is not None else []),
                "-l",
                *(["-v"] if args.verbose else []),
            ]
            print_if_verbose(separator)
            print_if_verbose(f'Compiling module "{name}" with command: {subprocess.list2cmdline(command)}')
            compile_result: subprocess.CompletedProcess[str] = subprocess.run(
                args=command,
                check=False,
                text=True,
            )
            if compile_result.returncode != 0:
                sys.exit(f"Module compilation failed with return code: {compile_result.returncode}.")
        except Exception as exc:
            sys.exit(f"Could not compile module due to exception: {exc}.")
    print_if_verbose("Compiling program...")

# Collect the full paths to the include folders, relative to the current working directory.
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
        str(executable_path),
        *([] if platform.system() == "Windows" else ["-pthread"]),
        *[item for include in [["-I", str(path.resolve())] for path in include_paths] for item in include],
        *([f"-D{define}" for define in defines]),
    ]
elif compiler == "g++":
    if len(modules) > 0 and (args.std in ["c++20", "c++23"]):
        print_if_verbose(
            "NOTE: GCC v14.2.0 appears to have an internal compiler error when compiling programs containing modules with any optimization flags other than -Og enabled. Until this is fixed, if you wish to use compiler optimizations, please either include the library as a header file or use a different compiler.",
        )
    command = [
        compiler_path if compiler_path is not None else compiler,
        *flags,
        *[str(path.resolve()) for path in source_paths],
        f"-std={args.std}",
        "-ggdb3" if args.type == "debug" else "-O3" if not (len(modules) > 0 and (args.std in ["c++20", "c++23"])) else "-Og",
        "-o",
        str(executable_path),
        *([] if platform.system() == "Windows" else ["-pthread"]),
        *[item for include in [["-I", str(path.resolve())] for path in include_paths] for item in include],
        *([f"-D{define}" for define in defines]),
    ]
else:  # compiler == "cl"
    command = [
        compiler_path if compiler_path is not None else compiler,
        *flags,
        *[str(path.resolve()) for path in source_paths],
        f"/std:{"c++latest" if args.std == "c++23" else args.std}",
        *(["/Zi", f"/Fd:{executable_path.with_suffix(".pdb")}"] if args.type == "debug" else ["/O2"]),
        *([f"/Fe:{executable_path}"] if not args.as_module else ["/ifcOutput", str(executable_path)]),
        f"/Fo:{executable_path.with_suffix(".obj")}",
        *[item for include in [["/I", str(path.resolve())] for path in include_paths] for item in include],
        "/permissive-",
        "/EHsc",
        "/nologo",
        "/Zc:__cplusplus",
        *([f"/D{define}" for define in defines]),
    ]

# For MSVC we also need to invoke the Visual Studio Developer PowerShell script.
if compiler == "cl":
    command = ["pwsh.exe", "-NoProfile", "-ExecutionPolicy", "Bypass", "-Command", f"& '{vs_pwsh_path}' -Arch {args.arch} -HostArch {args.arch} | Out-Null; {subprocess.list2cmdline(command)}"]

print_if_verbose(separator)
print_if_verbose(f"Compiler:        {compiler}")
print_if_verbose(f"C++ Standard:    {args.std.upper()}")
print_if_verbose(f"Type:            {args.type.title()}")
print_if_verbose(f"Source file(s):  [{", ".join([str(path) for path in source_paths])}]")
print_if_verbose(f"Defines:         [{", ".join(defines)}]")
print_if_verbose(f"Flags:           [{", ".join(flags)}]")
print_if_verbose(f"Includes:        [{(", ".join([str(path) for path in include_paths]))}]")
if args.std in ["c++20", "c++23"]:
    print_if_verbose(f"Modules:         [{", ".join(f"{name}={path[0]}" for name, path in module_paths.items()) if not args.as_module else ""}]")
else:
    print_if_verbose("Modules:         <Unused in C++17 mode>")
print_if_verbose(f"Build folder:    {build_folder}")
print_if_verbose(f"Binary file:     {executable_path.name}")
print_if_verbose(f"Command:         {subprocess.list2cmdline(command)}")

# Perform the actual compilation.
print_if_verbose(separator)
print_if_verbose("Compiling...")
success = False
try:
    compile_start: float = time.perf_counter()
    compile_result: subprocess.CompletedProcess[str] = subprocess.run(
        args=command,
        check=False,
        text=True,
    )
    if compile_result.returncode == 0:
        print_if_verbose(f"Compilation completed successfully in {time.perf_counter() - compile_start:.2f} seconds.")
        success = True
    else:
        sys.exit(f"Compilation failed with return code: {compile_result.returncode}.")
except Exception as exc:
    sys.exit(f"Could not compile due to exception: {exc}.")

# If there were compilation errors, delete the executable file if it exists, so the IDE won't mistakenly run the old version anyway.
if not success and executable_path.exists():
    executable_path.unlink()

if args.run:
    # We run the program in the build folder.
    os.chdir(build_folder)
    print_if_verbose(f"Running program{f" with arguments {" ".join(pass_args)}" if len(pass_args) > 0 else ""}...")
    print_if_verbose(separator)
    try:
        run_start: float = time.perf_counter()
        run_result: subprocess.CompletedProcess[str] = subprocess.run(
            args=[executable_path, *pass_args],
            check=False,
            text=True,
        )
        print_if_verbose(separator)
        if run_result.returncode == 0:
            print_if_verbose(f"Program executed successfully in {time.perf_counter() - run_start:.2f} seconds.")
        else:
            sys.exit(f"Program failed with return code: {run_result.returncode}.")
    except Exception as exc:
        print_if_verbose(separator)
        sys.exit(f"Could not run program due to exception: {exc}.")
