# CUDD 

CUDD is a C library for the manipulation of decision diagrams. It supports binary decision diagrams (BDDs), algebraic decision diagrams (ADDs), and Zero-Suppressed BDDs (ZDDs). The CUDD package also includes a C++ object-oriented wrapper for the C library.

## Organization History

The current main branch contains the source code from the original `3.0.0` release (2015). This and earlier versions were mirrored on GitHub by @ivmai, and his repository became one of the most widely used sources for `cudd`, used by Arch Linux and Bazel repositories.

In 2025, we established a dedicated GitHub organization, @cuddorg, to collaboratively maintain and evolve the CUDD project. As part of this effort, the `ivmai/cudd` mirror was transferred to the organization.

## Road to Release 4.0

Ongoing development takes place on the develop branch, which already includes several fixes and API enhancements contributed by decision diagram researchers. We aim to release a `4.0` release with these improvements and layout refactoring without a major change affecting functionality.

The primary focus, however, is on modernizing the repository layout and build system. The proposed CMake-based workflow is as follows:

```
git clone --branch 4.0.0-rc1 https://github.com/cuddorg/cudd

cmake -S ./cudd -B /tmp/cudd/build
cmake --build /tmp/cudd/build
cmake --install /tmp/cudd/build
```

CUDD can also be integrated into CMake projects using `FetchContent`:

```
include(FetchContent)
# set(CUDD_BUILD_SHARED_LIBS ON)
# set(CUDD_BUILD_WITH_STATS OFF)
FetchContent_Declare(
  cudd
  GIT_REPOSITORY https://github.com/cuddorg/cudd.git
  GIT_TAG        4.0.0-rc1
)
FetchContent_MakeAvailable(cudd)
```
