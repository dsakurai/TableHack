"""Build orchestration for the Conan/CMake pipeline, invoked from .vscode/tasks.json."""
import argparse
import os
import subprocess
import sys
import zipfile
from pathlib import Path

WORKSPACE = Path(__file__).resolve().parent.parent
VENV_SCRIPTS = WORKSPACE / ".venv" / "Scripts"


def build_dir(config):
    return WORKSPACE / f"build-{config}"


def run(args, cwd, env=None):
    print("> " + " ".join(str(a) for a in args))
    result = subprocess.run(args, cwd=cwd, env=env)
    if result.returncode != 0:
        sys.exit(result.returncode)


def cmd_build(args):
    config = args.config
    build = build_dir(config)
    build.mkdir(exist_ok=True)

    conan_env = os.environ.copy()
    conan_env["Path"] = f"{VENV_SCRIPTS}{os.pathsep}{conan_env.get('Path', '')}"
    run(
        [
            str(VENV_SCRIPTS / "conan.exe"), "install", "..",
            "--build=missing",
            "-s", f"build_type={config}",
            "-s", f"&:build_type={config}",
            "-c", "tools.env.virtualenv:powershell=powershell.exe",
            "-c", "tools.env:dotenv=true",
        ],
        cwd=build,
        env=conan_env,
    )

    cmake = VENV_SCRIPTS / "cmake.exe"
    run(
        [str(cmake), "--preset", "conan-default", f"-DCMAKE_CONFIGURATION_TYPES={config}"],
        cwd=WORKSPACE,
    )


def cmd_package(args):
    config = args.config
    build = build_dir(config)
    deploy_dir = build / "deploy" / config.lower() / "example-portable"
    zip_path = build / "example-portable.zip"

    if not deploy_dir.is_dir():
        print(f"Deploy folder not found: {deploy_dir}", file=sys.stderr)
        sys.exit(1)

    if zip_path.exists():
        zip_path.unlink()

    # Zip the deploy folder's contents at the archive root, not the folder itself.
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for path in deploy_dir.rglob("*"):
            if path.is_file():
                zf.write(path, path.relative_to(deploy_dir))

    print(f"Created {zip_path}")


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(required=True)

    build_parser = subparsers.add_parser("build", help="Conan install + CMake configure/build")
    build_parser.add_argument("config", choices=["Debug", "Release"])
    build_parser.set_defaults(func=cmd_build)

    package_parser = subparsers.add_parser("package", help="Zip the deployed portable build")
    package_parser.add_argument("config", choices=["Debug", "Release"])
    package_parser.set_defaults(func=cmd_package)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
