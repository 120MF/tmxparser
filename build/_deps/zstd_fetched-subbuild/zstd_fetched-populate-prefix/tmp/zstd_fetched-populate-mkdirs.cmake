# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src")
  file(MAKE_DIRECTORY "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-src")
endif()
file(MAKE_DIRECTORY
  "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-build"
  "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-subbuild/zstd_fetched-populate-prefix"
  "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-subbuild/zstd_fetched-populate-prefix/tmp"
  "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-subbuild/zstd_fetched-populate-prefix/src/zstd_fetched-populate-stamp"
  "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-subbuild/zstd_fetched-populate-prefix/src"
  "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-subbuild/zstd_fetched-populate-prefix/src/zstd_fetched-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-subbuild/zstd_fetched-populate-prefix/src/zstd_fetched-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/runner/work/tmxparser/tmxparser/build/_deps/zstd_fetched-subbuild/zstd_fetched-populate-prefix/src/zstd_fetched-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
