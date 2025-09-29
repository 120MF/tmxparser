# CMake generated Testfile for 
# Source directory: /home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests
# Build directory: /home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(fullbench "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-build/tests/fullbench")
set_tests_properties(fullbench PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests/CMakeLists.txt;64;add_test;/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests/CMakeLists.txt;0;")
add_test(fuzzer "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-build/tests/fuzzer")
set_tests_properties(fuzzer PROPERTIES  LABELS "Medium" _BACKTRACE_TRIPLES "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests/CMakeLists.txt;76;add_test;/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests/CMakeLists.txt;0;")
add_test(zstreamtest "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-build/tests/zstreamtest")
set_tests_properties(zstreamtest PROPERTIES  LABELS "Medium" _BACKTRACE_TRIPLES "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests/CMakeLists.txt;91;add_test;/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests/CMakeLists.txt;0;")
add_test(playTests "sh" "-c" "\"/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/../../tests/playTests.sh\" ")
set_tests_properties(playTests PROPERTIES  ENVIRONMENT "ZSTD_BIN=/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-build/programs/zstd;DATAGEN_BIN=/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-build/tests/datagen" LABELS "Medium" _BACKTRACE_TRIPLES "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests/CMakeLists.txt;98;add_test;/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src/build/cmake/tests/CMakeLists.txt;0;")
