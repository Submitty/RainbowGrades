#!/bin/bash
# Wrapper for running the integration tests in Docker.
# 
# Exists mainly to pass the host user's id through, so generated files under
# tests/*/data/ are owned by you rather than by root.
#
#   ./docker_test.sh                    # every module
#   ./docker_test.sh extra_credit       # one module
#   ./docker_test.sh --update           # regenerate golden files
#
# Any arguments are passed straight through to run.py.

set -e
cd "$(dirname "$0")"
if ! docker compose version > /dev/null 2>&1; then
    echo "ERROR: 'docker compose' is not available." >&2
    echo "Install Docker, or run the tests directly with run.py" >&2
    exit 1
fi

export HOST_UID="$(id -u)"
export HOST_GID="$(id -g)"

exec docker compose run --rm tests ./run.py "$@"
