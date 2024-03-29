# Copyright (c) m8mble 2021.
# SPDX-License-Identifier: BSL-1.0

import argparse
import datetime
import pathlib
import subprocess
import tempfile
import textwrap


def _sphinx_config(doxygen: pathlib.Path, logo: pathlib.Path, version: str) -> dict:
    return {
        "project": "Clean Test",
        "copyright": f"2020 - {datetime.date.today().year}, m8mble",
        "author": "m8mble",
        "version": version,
        "release": version,
        "rst_prolog": textwrap.dedent(
            """
                .. role:: cpp(code)
                   :language: cpp
            """
        ),
        "primary_domain": "cpp",
        "highlight_language": "cpp",
        "cpp_index_common_prefix": ["clean_test::"],
        "html_theme": "furo",
        "html_title": "Clean Test",
        "html_theme_options": {
            "source_repository": "https://github.com/clean-test/clean-test/",
            "source_branch": "main",
            "source_directory": "doc/",
            "light_logo": "logo-light.svg",
            "dark_logo": "logo-dark.svg",
            "sidebar_hide_name": True,
            "light_css_variables": {
                "color-brand-primary": "#00D423",
            },
            "dark_css_variables": {
                "color-brand-primary": "#00D423",
            },
            "footer_icons": [
                {
                    "name": "GitHub",
                    "url": "https://github.com/clean-test/clean-test",
                    "html": """
                <svg stroke="currentColor" fill="currentColor" stroke-width="0" viewBox="0 0 16 16">
                    <path fill-rule="evenodd" d="M8 0C3.58 0 0 3.58 0 8c0 3.54 2.29 6.53 5.47 7.59.4.07.55-.17.55-.38 0-.19-.01-.82-.01-1.49-2.01.37-2.53-.49-2.69-.94-.09-.23-.48-.94-.82-1.13-.28-.15-.68-.52-.01-.53.63-.01 1.08.58 1.23.82.72 1.21 1.87.87 2.33.66.07-.52.28-.87.51-1.07-1.78-.2-3.64-.89-3.64-3.95 0-.87.31-1.59.82-2.15-.08-.2-.36-1.02.08-2.12 0 0 .67-.21 2.2.82.64-.18 1.32-.27 2-.27.68 0 1.36.09 2 .27 1.53-1.04 2.2-.82 2.2-.82.44 1.1.16 1.92.08 2.12.51.56.82 1.27.82 2.15 0 3.07-1.87 3.75-3.65 3.95.29.25.54.73.54 1.48 0 1.07-.01 1.93-.01 2.2 0 .21.15.46.55.38A8.013 8.013 0 0 0 16 8c0-4.42-3.58-8-8-8z"></path>
                </svg>
            """,
                    "class": "",
                },
            ],
        },
        "html_static_path": [
            str(logo),
        ],
        "html_favicon": str(logo / "favicon.ico"),
        "autosectionlabel_prefix_document": True,
        "extensions": [
            "breathe",
            "sphinx.ext.autosectionlabel",
            "sphinx.ext.githubpages",
        ],
        # breathe configuration
        "breathe_default_project": "clean-test",
        "breathe_projects": {"clean-test": str(doxygen.resolve())},
        "breathe_domain_by_extension": {"h": "cpp"},
    }


def _sphinx_arguments(config: dict, prefix=None) -> list:
    result = []
    if isinstance(config, dict):
        arg_prefix = f"{prefix}." if prefix is not None else ""
        result = [e for l in [_sphinx_arguments(v, prefix=f"{arg_prefix}{k}") for k, v in config.items()] for e in l]
    elif isinstance(config, list):
        result = [f"{prefix}={','.join(str(e) for e in config)}"]
    elif isinstance(config, bool):
        result = [f"{prefix}={int(config)}"]
    else:
        result = [f"{prefix}={config}"]

    if prefix is None:
        result = [f"-D{arg}" for arg in result]
    return result


def _sphinx_module(config: dict) -> str:
    return "\n".join(f"{k} = {repr(v)}" for k, v in config.items())


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
    """Generate documentation for Clean Test into build_dir."""
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
    with tempfile.TemporaryDirectory() as tmp:
        conf = pathlib.Path(tmp) / "conf.py"
        conf.write_text(_sphinx_module(_sphinx_config(doxygen=doxygen, logo=docs / "logo", version=version)))
        subprocess.check_call(
            (
                "sphinx-build",
                "-c",
                tmp,
                "-W",  # -Werror for sphinx
                "--keep-going",
                "--color",
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
