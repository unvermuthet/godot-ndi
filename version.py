import subprocess

commit_hash = ""
commit_tag = ""

try:
    commit_hash = subprocess.check_output(
        ["git", "rev-parse", "HEAD"]
    ).strip().decode("utf-8")
except subprocess.CalledProcessError:
    print("Failed to get the latest commit hash.")

try:
    commit_tag = subprocess.check_output(
        ["git", "describe", "--tags", "--exact-match"]
    ).strip().decode("utf-8")
except subprocess.CalledProcessError:
    print("No tag found for the current commit.")