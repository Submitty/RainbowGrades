#!/usr/bin/env python3
""" Entry point for Rainbow Grades integration tests.

    ./run.py                                run every test module
    ./run.py extra_credit                   run one module
    ./run.py extra_credit.all_students_summary_csv
                                            run one testcase within a module
    ./run.py --update                       regenerate all the expected output files

--update records whatever Rainbow Grades currently does, so always check the
regenerated files before committing them. The new files could pin a bug as
correct behavior.
"""

import os
import sys

# Ensure the lib and tests packages can be imported regardless of
# the directory where this was called from
sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))

import lib


if __name__ == "__main__":
    # multiprocessing's "spawn" start method (the macOS/Windows default)
    # re-imports this file as __main__ in each worker process, so anything
    # that starts a Pool (lib.run_tests, for more than one module) must be
    # guarded here rather than run at module scope.
    arguments = [a for a in sys.argv[1:] if not a.startswith("-")]
    flags = [a for a in sys.argv[1:] if a.startswith("-")]


    for flag in flags:
        if flag in ("--update", "-u"):
            lib.UPDATE_VALIDATION = True
        elif flag in ("--help", "-h"):
            print(__doc__)
            sys.exit(0)
        else:
            print(f"Unknown option: {flag}\n")
            print(__doc__)
            sys.exit(1)

    # Load all test packages, which will populate the registry in the lib module
    import tests    # noqa: E402

    if arguments:
        lib.run_tests(arguments)
    else:
        lib.run_all()