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

This Python script is used in the library's development environment to clear all files a folder. It is not part of the library itself.
"""

import argparse
import importlib
import importlib.util
import pathlib
import sys
from importlib.machinery import ModuleSpec
from types import ModuleType

send2trash_spec: ModuleSpec | None = importlib.util.find_spec("send2trash")
if send2trash_spec is not None:
    # Install with `pip install send2trash`.
    send2trash: ModuleType = importlib.import_module("send2trash")

    def delete_files_in_folder(folder_path: pathlib.Path) -> None:
        """Recursively delete all files in a folder."""
        for child in folder_path.iterdir():
            if child.is_file():
                send2trash.send2trash(paths=child)
            else:
                delete_files_in_folder(child)
        send2trash.send2trash(folder_path)
else:
    print("Note: Module send2trash not found; deleting files permanently.")

    def delete_files_in_folder(folder_path: pathlib.Path) -> None:
        """Fallback function in case `send2trash` is not installed."""
        for child in folder_path.iterdir():
            if child.is_file():
                child.unlink()
            else:
                delete_files_in_folder(child)
        folder_path.rmdir()


parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
_ = parser.add_argument("folder", help="the folder to clear (relative to this script's location)")
parsed_args: argparse.Namespace = parser.parse_args(args=None if len(sys.argv) > 1 else ["--help"])

folder: pathlib.Path = (pathlib.Path(__file__).parent / parsed_args.folder).resolve()
if not folder.exists():
    print(f"Folder {folder} does not exist; creating empty.")
else:
    try:
        delete_files_in_folder(folder)
        folder.mkdir()
        print(f"Folder {folder} successfully cleared.")
    except Exception as exc:
        sys.exit(f"Error clearing folder {folder}: {exc}")
