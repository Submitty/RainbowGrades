"""Extra credit gradeables.

Covers both kinds of extra credit that Rainbow Grades supports:

- an extra credit gradeable inside a weighted bucket
- a bucket that is entirely extra credit, layered on top of the 100%


See README.md in this directory for the arithmetic and for what each student
in the fixture is meant to catch.
"""

from lib import prebuild, testcase


############################################################################
@prebuild
def initialize(test):
    # Copies customization.json and raw_data/ into a clean data/ directory and
    # writes the appropriate Makefile that points to the new directory.
    test.stage_inputs()

############################################################################
# Each test case below reuses the single run staged and compiled above

@testcase
def rainbow_grades_runs(test):
    """Rainbow Grades completes and produces output."""
    test.ensure_run()


@testcase
def extra_credit_is_excluded_from_the_denominator(test):
    """A student with no extra credit isn't penalized.
    
    bbeta scores 100 on both graded homeworks and nothing else. If an extra
    credit gradeable ever leaks into the bucket denominator, this drops below
    100. Treating hw03_extra_credit as a normal 10 point gradeable puts it
    at 95.24.
    """
    grades = test.get_grades()["bbeta"]
    assert grades["HOMEWORK %"] == "100.00", \
        f"expected homework 100.00, got {grades['HOMEWORK %']}"
    assert grades["OVERALL"] == "100.00", \
        f"expected overall 100.00, got {grades['OVERALL']}"


@testcase
def extra_credit_can_push_a_student_above_100(test):
    """aalpha earns everything, including all the extra credit
    
    Homework is 96.50 and participation add its full 5.00.
    """
    grades = test.get_grades()["aalpha"]
    assert grades["HOMEWORK %"] == "96.50", \
        f"expected homework 96.50, got {grades['HOMEWORK %']}"
    assert grades["PARTICIPATION %"] == "5.00", \
        f"expected participation 5.00, got {grades['PARTICIPATION %']}"
    assert grades["OVERALL"] == "101.50", \
        f"expected overall to be 101.50, got {grades['OVERALL']}"


@testcase
def a_pure_extra_credit_bucket_stands_alone(test):
    """ddelta earns extra credit and nothing else.
    
    Isolates the two bonus paths from the graded paths. 5.00 from
    the extra credit homework and 5.00 from the extra credit bucket.
    """
    grades = test.get_grades()["ddelta"]
    assert grades["HOMEWORK %"] == "5.00", \
        f"expected homework 5.00, got {grades['HOMEWORK %']}"
    assert grades["PARTICIPATION %"] == "5.00", \
        f"expected participation 5.00, got {grades['PARTICIPATION %']}"
    assert grades["OVERALL"] == "10.00", \
        f"expected overall 10.00, got {grades['OVERALL']}"


@testcase
def partial_extra_credit_is_prorated(test):
    """cgamma and eepsilon take partial credit.
    
    Partial values are here so that a weight or rounding change
    shows up instead of cancelling out against a full or empty score.
    """
    grades = test.get_grades()

    # half of the extra credit homework
    assert grades["cgamma"]["OVERALL"] == "73.00", \
        f"expected cgamma overall 73.00, got {grades['cgamma']['OVERALL']}"
    assert grades["cgamma"]["PARTICIPATION %"] == "2.50", \
        f"expected cgamma participation 2.50, got {grades['cgamma']['PARTICIPATION %']}"

    # one and a half participation gradeables
    assert grades["eepsilon"]["OVERALL"] == "93.25", \
        f"expected eepsilon overall 93.25, got {grades['eepsilon']['OVERALL']}"
    assert grades["eepsilon"]["PARTICIPATION %"] == "3.75", \
        f"expected eepsilon participation 3.75, got {grades['eepsilon']['PARTICIPATION %']}"


@testcase
def all_students_summary_csv(test):
    test.diff("output.csv")
    test.diff_directory("all_students_summary_csv")


@testcase
def all_students_summary_html(test):
    test.diff("output.html")
    test.diff_directory("all_students_summary_html")


@testcase
def individual_summary_html(test):
    test.diff_directory("individual_summary_html")