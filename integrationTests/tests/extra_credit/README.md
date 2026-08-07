# Test module: `extra_credit`

Test how RainbowGrades handles extra credit gradeables.

## The course

Five gradeables in two buckets:

| bucket | gradeable | `max` | `scale_max` | notes |
|---|---|---|---|---|
| homework (100% of grade) | `hw01` | 100 | — | normal |
| | `hw02` | 100 | — | normal |
| | `hw03_extra_credit` | **0** | 10 | extra credit within the bucket |
| participation (5% bonus) | `participation01` | **0** | 5 | pure extra credit |
| | `participation02` | **0** | 5 | pure extra credit |

## How the two kinds of extra credit work

Both are expressed by setting `"max": 0` and giving the points as
`"scale_max"`. The behavior comes from `Student::GradeablePercent` in
`student.cpp`.

**Within a bucket.** `sum_max` only accumulates items with a nonzero `max`, so
`hw03_extra_credit` never enters the denominator. Each item's weight is
`p = max(max, scale_max) / sum_max`, which gives `hw01` and `hw02` 0.5 each and
`hw03_extra_credit` `10/200 = 0.05`. Because `sum_percentage` also skips
zero-`max` items, it lands at exactly 1.0 and the bucket can exceed its
`percent`:

```
homework % = 100 * (0.5*hw01/100 + 0.5*hw02/100 + 0.05*hw03/10)
           = 105 when everything is earned
```

**A whole bucket.** Every participation item has `max: 0`, so `sum_max == 0`
and a separate branch runs: weights become `scale_max / sum_scaled_max` (0.5
each) and `sum_percentage` is forced to 1.0. The bucket contributes its
`percent` straight on top:

```
participation % = 100 * 0.05 * (0.5*p01/5 + 0.5*p02/5)
                = 5 when everything is earned
```

A student who earns everything therefore finishes at **110**.

## Expected results

| student | hw01 | hw02 | hw03 EC | part01 | part02 | homework % | part. % | **overall** |
|---|---|---|---|---|---|---|---|---|
| `aalpha` | 95 | 88 | 10 | 5 | 5 | 96.50 | 5.00 | **101.50** |
| `bbeta` | 100 | 100 | 0 | 0 | 0 | 100.00 | — | **100.00** |
| `cgamma` | 72 | 64 | 5 | 5 | 0 | 70.50 | 2.50 | **73.00** |
| `ddelta` | 0 | 0 | 10 | 5 | 5 | 5.00 | 5.00 | **10.00** |
| `eepsilon` | 88 | 91 | 0 | 2.5 | 5 | 89.50 | 3.75 | **93.25** |

Each student is doing a job, and each has a matching `@testcase` in
`__init__.py` so that a failure names the behavior rather than just the file:

- **`bbeta`** is the control — `extra_credit_is_excluded_from_the_denominator`.
  Perfect on the two graded homeworks, zero extra credit, and lands on exactly
  100.00. If extra credit ever leaks into the denominator this is the row that
  moves: treating `hw03_extra_credit` as a normal 10 point gradeable drops
  `bbeta` to 95.24.
- **`aalpha`** - `extra_credit_can_push_a_student_above_100`. Earns everything
  and finishes over 100.
- **`ddelta`** - `a_pure_extra_credit_bucket_stands_alone`. Earns *only* extra
  credit, isolating the bonus paths from the graded ones.
- **`cgamma`** and **`eepsilon`** - `partial_extra_credit_is_prorated`. Partial
  values, so a rounding or weighting change shows up instead of cancelling out
  against a full or empty score.
