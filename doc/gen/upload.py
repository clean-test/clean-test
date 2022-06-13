import argparse
import pathlib
import shutil
import subprocess
import tempfile
import os


def _lstrip(source: str, prefix: str) -> str:
    """ Strip prefix from source iff the latter starts with the former. """
    if source.startswith(prefix):
        return source[len(prefix) :]
    return source


def update(source: pathlib.Path, repository: str, key: str, branch: str, is_ci: bool, **kwargs):
    with tempfile.TemporaryDirectory() as work:
        target = pathlib.Path(work) / branch
        url = f"https://{f'{key}@' if key else ''}github.com/{repository}.git"

        def _git(*args, check=True):
            ev = subprocess.check_call if check else subprocess.call
            return ev(("git",) + args, cwd=work)

        # Prepare repo
        _git("clone", "--single-branch", "--branch=main", url, ".")
        shutil.rmtree(target, ignore_errors=True)
        shutil.copytree(source, target, ignore=shutil.ignore_patterns(".*"))
        subprocess.check_call(["ls", "-ltrah", str(target)])

        # Prepare git
        _git("config", "user.name", "doc-bot")
        _git("config", "user.email", "doc-bot@clean-test.io")
        _git("config", "commit.gpgsign", "false")

        # Push changes (if any)
        _git("add", branch)
        if _git("commit", "-m", f"Updating doc for {branch}", check=False) == 0:
            p = subprocess.run(
                ("git", "push", "origin"), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding="utf-8", cwd=work
            )
            if key:
                p.stdout = p.stdout.replace(key, "$key")
            print(p.stdout)
            p.check_returncode()


########################################################################################################################


def _load_parser():
    p = argparse.ArgumentParser("Update online documentation of Clean Test.")
    p.add_argument("--source", required=True, type=str, help="Locally generated documentation.")
    p.add_argument("--repository", default="clean-test/clean-test.github.io", help="Repository for pages.")
    return p


def _parse_args():
    args = vars(_load_parser().parse_args())
    for k in ("source",):
        args[k] = pathlib.Path(args[k])
    return args


def main():
    args = _parse_args()
    branch = _lstrip(os.environ.get("GITHUB_REF", "main"), "refs/heads/")
    is_ci = "CI" in os.environ
    key = os.environ.get("KEY", None)
    if is_ci and branch != "main":
        print(f"Skipping upload for branch {branch}.")
        return
    if is_ci and not key:
        print("No key for uploading.")
        return
    update(branch=branch, is_ci=is_ci, key=key, **args)
