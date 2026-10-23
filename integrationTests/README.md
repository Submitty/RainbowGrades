# RainbowGrades integration tests

These tests exercise RainbowGrades end to end: they compile it, feed it a
course's worth of input, and check the HTML and CSV it produces.

The framework mirrors Submitty's autograding integration tests
(`Submitty/tests/integrationTests/`). Test modules register themselves with 
`@prebuild` and `@testcase` decorators, each decorated function receives a 
`test` object that knows where its module lives, and generated output lands 
in `data/` while golden files live in `validation/`.

The tests are self-contained. They do **not** need a Submitty install, a
database, or a prior "Generate Grade Summaries" run, the raw data is committed
as a fixture, so they run on any PR.

## Running

### Directly on the host

Requires `make`, `python3`, and `clang++`, on macOS these come with the Xcode
command line tools, and on Ubuntu with `apt install clang make python3`. The
build fetches its own pinned nlohmann/json, so nothing else is needed.

```bash
cd integrationTests

./run.py                                  # every module
./run.py extra_credit                     # one module
./run.py extra_credit.all_students_summary_csv
./run.py --update                         # regenerate golden files
```

To build with a different compiler:

```bash
RAINBOW_CXX=g++ ./run.py
```

## Layout

```
integrationTests/
  run.py                     entry point
  lib.py                     decorators, TestcaseWrapper, runner
  docker-compose.yml         image, mount, and user id for local runs
  docker_test.sh             wrapper that passes your user id through
  tests/
    __init__.py              auto-loads every module below
    extra_credit/
      __init__.py            the test code
      README.md              what this module pins down, and why
      customization.json     hand-written config
      raw_data/              mimics a course's reports directory
        base_url.json
        all_grades/*_summary.json
      validation/            golden files
      data/                  scratch, created per run, gitignored
```

## Writing a test module

```python
from lib import prebuild, testcase

@prebuild
def initialize(test):
    test.stage_inputs()

@testcase
def bonus_does_not_change_the_denominator(test):
    assert test.get_grades()["bbeta"]["OVERALL"] == "100.00"

@testcase
def all_students_summary_csv(test):
    test.diff("output.csv")
```

`@prebuild` runs once per module, before RainbowGrades is compiled. Each
`@testcase` is reported and can be selected individually on the command line.

RainbowGrades itself runs at most once per module, lazily, the first time a
testcase asks for output. That means any testcase can be run on its own without
depending on a sibling having run first.

### What `test` gives you

| method | purpose |
|---|---|
| `stage_inputs()` | copy `customization.json` and `raw_data/` into a clean `data/`, and write the Makefile |
| `ensure_run()` | run RainbowGrades if it has not run yet for this module |
| `run_rainbow_grades(sort_order)` | force a run with a specific sort order |
| `get_grades()` | `output.csv` parsed as `{username: {column: value}}` |
| `diff(f1, f2="")` | compare one normalized output file against its golden counterpart |
| `diff_directory(d)` | diff every golden file in a directory, and flag added or removed files |
| `debug_print(f)` | print a file from `data/`; useful for CI failures |

## Two kinds of testcases

Each module is expected to contain both:

- **Semantic checks** that assert on specific numbers via `get_grades()`. When
  one fails it says `expected homework 100.00, got 95.24`, which names the
  behavior that broke.
- **Golden file diffs** via `diff()` and `diff_directory()`, which catch
  everything the semantic checks did not think to look at.

A change to the grade computation fails both; a change to markup or 
wording fails only the diffs. So the shape of the failure tells a 
reviewer whether a PR moved any student's grade.

## Adding a module

1. `mkdir integrationTests/tests/<name>` and write `__init__.py`,
   `customization.json`, and `raw_data/all_grades/*_summary.json`.
2. `./run.py <name> --update` to generate `validation/`.
3. **Read the generated golden files** and confirm the numbers are what you
   intended. `--update` records whatever RainbowGrades currently does, so an
   unreviewed golden file pins in a bug just as well as correct behavior.
4. Add semantic `@testcase` assertions for the numbers the module exists to
   protect, and a short `README.md` explaining the arithmetic.

Nothing needs to be registered: `tests/__init__.py` discovers modules automatically.

## How output is normalized

RainbowGrades output is deterministic apart from two things, both properties of
when and where it ran rather than of the grade computation. `lib.py`
neutralizes both, writing normalized copies to `data/normalized/` and leaving
the raw output alone for debugging:

- The page footer contains the current year, the most recent git tag, and the
  short commit hash. The whole line is replaced with a placeholder.
- The archived all-students report is named
  `output_<month>_<day>_<year>.{html,csv}`. Its contents are compared, the date
  in the filename is replaced with `DATE`.

Everything else, including `Information last updated:`, comes from the input
fixtures, so it stays stable.

Per-student `individual_summary_html/*.json` files are skipped, they hold exam
seating assignments and are `null` unless seating is configured.

## When a test fails

The scratch directory is kept on failure (and its path printed) so the actual
generated files can be inspected, CI uploads it as an artifact.
