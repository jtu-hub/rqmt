#e.g. usage: python release.py v0.0.1-alpha
import subprocess
import sys


SOURCE_BRANCH = "main"
REPO_ROOT = subprocess.check_output(
    ["git", "rev-parse", "--show-toplevel"],
    text=True,
).strip()


def run(cmd, capture=False):
    if capture:
        return subprocess.check_output(
            cmd,
            cwd=REPO_ROOT,
            text=True,
        ).strip()

    print(f"> {' '.join(cmd)}")
    subprocess.run(
        cmd,
        cwd=REPO_ROOT,
        check=True,
    )


def fail(msg):
    print(f"\n❌ {msg}")
    sys.exit(1)


def check_git_repo():
    try:
        run(["git", "rev-parse", "--git-dir"], capture=True)
    except Exception:
        fail("Not inside a git repository.")


def check_clean_worktree():
    status = run(["git", "status", "--porcelain"], capture=True)

    if status:
        print(status)
        fail(
            "Working tree is not clean. Commit, stash, or discard changes first."
        )


def get_current_branch():
    return run(
        ["git", "rev-parse", "--abbrev-ref", "HEAD"],
        capture=True,
    )


def branch_exists(branch):
    try:
        run(
            ["git", "show-ref", "--verify", f"refs/heads/{branch}"],
            capture=True,
        )
        return True
    except subprocess.CalledProcessError:
        return False


def tag_exists(tag):
    try:
        run(
            ["git", "show-ref", "--tags", tag],
            capture=True,
        )
        return True
    except subprocess.CalledProcessError:
        return False


def main():
    if len(sys.argv) != 2:
        print("Usage:")
        print("  python release.py v1.0.0")
        sys.exit(1)

    version = sys.argv[1].strip()

    if not version:
        fail("Version cannot be empty.")

    release_branch = f"release/{version}"

    check_git_repo()
    check_clean_worktree()

    current_branch = get_current_branch()

    if current_branch != SOURCE_BRANCH:
        fail(
            f"Current branch is '{current_branch}'. "
            f"Switch to '{SOURCE_BRANCH}' first."
        )

    if branch_exists(release_branch):
        fail(f"Branch already exists: {release_branch}")

    if tag_exists(version):
        fail(f"Tag already exists: {version}")

    print("\nRelease plan")
    print("------------")
    print(f"Source branch : {SOURCE_BRANCH}")
    print(f"Release branch: {release_branch}")
    print(f"Tag           : {version}")
    print()

    confirm = input("Continue? [y/N]: ").strip().lower()

    if confirm != "y":
        print("Cancelled.")
        sys.exit(0)

    try:
        run(["git", "checkout", "-b", release_branch])

        # This commit will fail if nothing changed.
        # That's usually desirable for release branches.
        run(["git", "add", "--all"])
        run(["git", "commit", "-m", f"Release {version}"])

        run(
            [
                "git",
                "tag",
                "-a",
                version,
                "-m",
                f"Release {version}",
            ]
        )

        run(
            [
                "git",
                "push",
                "-u",
                "origin",
                release_branch,
            ]
        )

        run(["git", "push", "origin", version])

        print("\n✅ Release completed successfully.")
        print(f"Branch: {release_branch}")
        print(f"Tag   : {version}")

    except subprocess.CalledProcessError as e:
        fail(f"Git command failed with exit code {e.returncode}")


if __name__ == "__main__":
    main()