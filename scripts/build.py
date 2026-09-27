"""Build orchestration for the Conan/CMake pipeline, invoked from .vscode/tasks.json."""
import argparse
import os
import subprocess
import sys
import zipfile
from pathlib import Path

WORKSPACE = Path(__file__).resolve().parent.parent
VENV_SCRIPTS = WORKSPACE / ".venv" / "Scripts"
BUILD_DIR = WORKSPACE / "build"
DEPLOY_DIR = BUILD_DIR / "deploy" / "release" / "example-portable"
ZIP_PATH = BUILD_DIR / "example-portable.zip"


def run(args, cwd, env=None):
    print("> " + " ".join(str(a) for a in args))
    result = subprocess.run(args, cwd=cwd, env=env)
    if result.returncode != 0:
        sys.exit(result.returncode)


def cmd_build(_args):
    BUILD_DIR.mkdir(exist_ok=True)

    conan_env = os.environ.copy()
    conan_env["Path"] = f"{VENV_SCRIPTS}{os.pathsep}{conan_env.get('Path', '')}"
    run(
        [
            str(VENV_SCRIPTS / "conan.exe"), "install", "..",
            "--build=missing",
            "-s", "build_type=Release",
            "-s", "&:build_type=Release",
            "-c", "tools.env.virtualenv:powershell=powershell.exe",
            "-c", "tools.env:dotenv=true",
        ],
        cwd=BUILD_DIR,
        env=conan_env,
    )

    cmake = VENV_SCRIPTS / "cmake.exe"
    run(
        [str(cmake), "--preset", "conan-default", "-DCMAKE_CONFIGURATION_TYPES=Release"],
        cwd=WORKSPACE,
    )


def cmd_package(_args):
    if not DEPLOY_DIR.is_dir():
        print(f"Deploy folder not found: {DEPLOY_DIR}", file=sys.stderr)
        sys.exit(1)

    if ZIP_PATH.exists():
        ZIP_PATH.unlink()

    # Zip the deploy folder's contents at the archive root, not the folder itself.
    with zipfile.ZipFile(ZIP_PATH, "w", zipfile.ZIP_DEFLATED) as zf:
        for path in DEPLOY_DIR.rglob("*"):
            if path.is_file():
                zf.write(path, path.relative_to(DEPLOY_DIR))

    print(f"Created {ZIP_PATH}")


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(required=True)

    build_parser = subparsers.add_parser("build", help="Conan install + CMake configure/build")
    build_parser.set_defaults(func=cmd_build)

    package_parser = subparsers.add_parser("package", help="Zip the deployed portable build")
    package_parser.set_defaults(func=cmd_package)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
