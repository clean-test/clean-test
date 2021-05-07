# Copyright (c) m8mble 2021.
# SPDX-License-Identifier: BSL-1.0

import argparse
import datetime
import pathlib
import subprocess
import textwrap


def _sphinx_config(doxygen: pathlib.Path, version: str) -> dict:
    return {
        "project": "Clean Test",
        "copyright": f"2020 - {datetime.date.today().year}, m8mble",
        "author": "m8mble",
        "version": version,
        "release": version,
        "html_theme": "alabaster",
        "rst_prolog": textwrap.dedent(
            """
                .. role:: cpp(code)
                   :language: cpp
            """
        ),
        "extensions": [
            "breathe",
            "sphinx.ext.autosectionlabel",
            "sphinx.ext.githubpages",
        ],
        # breathe configuration
        "breathe_default_project": "clean-test",
        "breathe_projects": {"clean-test": doxygen.resolve()},
        "breathe_domain_by_extension": {"h": "cpp"},
    }


def _sphinx_arguments(config: dict) -> list:
    result = []
    for k, v in config.items():
        if isinstance(v, dict):
            result.extend(f"{k}.{vk}={vv}" for vk, vv in v.items())
        elif isinstance(v, list):
            result.append(f"{k}={','.join(str(e) for e in v)}")
        elif isinstance(v, bool):
            result.append(f"{k}={int(v)}")
        else:
            result.append(f"{k}={v}")
    return [f"-D{arg}" for arg in result]


def _doxygen_config(doxygen: pathlib.Path, include: pathlib.Path) -> str:
    return f"""
        PROJECT_NAME      = Clean Test
        GENERATE_LATEX    = NO
        GENERATE_MAN      = NO
        GENERATE_RTF      = NO
        INPUT             = {" ".join(str(f) for f in include.resolve().glob("**/*.h"))}
        QUIET             = YES
        JAVADOC_AUTOBRIEF = YES
        AUTOLINK_SUPPORT  = NO
        GENERATE_HTML     = NO
        GENERATE_XML      = YES
        XML_OUTPUT        = {doxygen}
    """


def build_docs(build_dir: pathlib.Path, version: str, **kwargs) -> pathlib.Path:
    """ Generate documentation for Clean Test into build_dir. """
    docs = pathlib.Path(__file__).parent.parent.resolve()
    include = docs.parent / "include" / "clean-test"
    doxygen = build_dir / "doxygen"

    build_dir.mkdir(exist_ok=True, parents=True)

    print(
        subprocess.check_output(
            ("doxygen", "-"),
            input=_doxygen_config(doxygen=doxygen, include=include),
            stderr=subprocess.STDOUT,
            encoding="utf-8",
        )
    )

    html = build_dir / "html"
    subprocess.check_call(
        (
            "sphinx-build",
            "-C",  # no conf.py
            "-W",  # -Werror for sphinx
            *_sphinx_arguments(_sphinx_config(doxygen=doxygen, version=version)),
            "-b",
            "html",
            docs,
            html,
        )
    )
    return html


########################################################################################################################


def _load_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser("Generate documentation")
    parser.add_argument("--build-dir", required=True, help="Path to (cmake) toplevel build directory.")
    parser.add_argument("--version", default="dev", type=str, help="Documented version of Clean Test.")
    return parser


def _parse_arguments() -> dict:
    args = vars(_load_parser().parse_args())
    for k in ("build_dir",):
        args[k] = pathlib.Path(args[k]).resolve()
    return args


def main():
    args = _parse_arguments()
    build_docs(**args)
