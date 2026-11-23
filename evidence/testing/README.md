# TOOL USAGE EVIDENCE FOLDER

This folder contains the **real logs and documents** that prove tool usage, as required by the course. No screenshots are used.

## 1. GitHub / Git
- The group creates its own GitHub repository and **commits each member's own part manually**, according to the assignment table in `Bang_Chung_Cong_Cu_Va_Phan_Cong.docx`.
- Evidence: the repository link and the **Commits** tab on GitHub.

## 2. Testing tool (Google Test)
- `testing/test_results.txt`: real Google Test output — `31 tests from 7 test suites ran`, `[ PASSED ] 31 tests`.
- `../Testing_Document_Group01.xlsx`: 100 test cases (5 cases per function, using the instructor's exact template).
- Test source: `tests/test_suite.cpp` uses Google Test 1.17, vendored at `third_party/googletest`.

## 3. Docker
- `docker/docker_build.txt`: `--no-cache` build log, including the Google Test run (31 tests) during the image build.
- `docker/docker_ps.txt`: container `healthy`, mapping `0.0.0.0:8080->8080/tcp`.

## 4. Defect tracking
- `../docs/DEFECT_TRACKING_LOG.md`: 9 defects found and resolved (all Fixed/Verified), covering the "defect tracking tool" requirement.

## 5. Submission
All log files above are referenced directly in `Bang_Chung_Cong_Cu_Va_Phan_Cong.docx` (Appendix 3). Simply compress the whole `ProgAndTest_Group01` folder into `ProgAndTest_Group01.zip` and submit.
