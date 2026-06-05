import re
import argparse
import subprocess
import os

def git_commit_count(repo_root):
    result = subprocess.run(
        [
            "git",
            "-C",
            repo_root,
            "rev-list",
            "--count",
            "HEAD"
        ],
        capture_output=True,
        text=True,
        check=True
    )

    return int(result.stdout.strip())


def git_hash(repo_root):
    result = subprocess.run(
        [
            "git",
            "-C",
            repo_root,
            "rev-parse",
            "--short",
            "HEAD"
        ],
        capture_output=True,
        text=True,
        check=True
    )

    return result.stdout.strip()


def replace_numeric_define(content, key, value):
    pattern = re.compile(
        rf"#define\s+{key}\s+\d+"
    )

    return pattern.sub(
        f"#define {key} {value}",
        content
    )


def replace_string_define(content, key, value):
    pattern = re.compile(
        rf'#define\s+{key}\s+"[^"]*"'
    )

    return pattern.sub(
        f'#define {key} "{value}"',
        content
    )


def bump_numeric_define(content, key):
    pattern = re.compile(
        rf"#define\s+({key})\s+(\d+)"
    )

    def replacer(match):
        value = int(match.group(2)) + 1
        return f"#define {key} {value}"

    content, count = pattern.subn(
        replacer,
        content
    )

    if count == 0:
        raise ValueError(
            f"Could not find #define {key}"
        )

    return content


def update_revision_from_git(content, repo_root):
    revision = git_commit_count(repo_root)
    hash_value = git_hash(repo_root)

    content = replace_numeric_define(
        content,
        "REVISION",
        revision,
    )

    content = replace_string_define(
        content,
        "GIT_HASH",
        hash_value,
    )

    return content


def bump_version(args):

    with open(args.file, "r", encoding="utf-8") as f:
        content = f.read()

    content = update_revision_from_git(content, args.repo_root)


    content = bump_numeric_define(
        content,
        args.part.upper()
    )

    if args.part == "major":
        content = replace_numeric_define(content, "MINOR", 0)
        content = replace_numeric_define(content, "PATCH", 0)
        content = replace_numeric_define(content, "REVISION", 0)
        content = replace_numeric_define(content, "BUILD", 0)

    elif args.part == "minor":
        content = replace_numeric_define(content, "PATCH", 0)
        content = replace_numeric_define(content, "REVISION", 0)
        content = replace_numeric_define(content, "BUILD", 0)

    elif args.part == "patch":
        content = replace_numeric_define(content, "REVISION", 0)
        content = replace_numeric_define(content, "BUILD", 0)

    with open(args.file, "w", encoding="utf-8") as f:
        f.write(content)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="Update version information."
    )

    parser.add_argument(
        "file",
        help="Path to version.h"
    )

    parser.add_argument(
        "part",
        choices=[
            "major",
            "minor",
            "patch",
            "revision",
            "build"
        ],
        help="Version component to update"
    )

    parser.add_argument(
        "repo_root",
        help="Git repository root"
    )

    args = parser.parse_args()

    bump_version(args)
# %%
