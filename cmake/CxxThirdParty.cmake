# SPDX-License-Identifier: Unlicense
# SPDX-FileCopyrightText: 2023 NeoFOAM authors

set(NEOFOAM_KOKKOS_CHECKOUT_VERSION
    "4.3.00"
    CACHE STRING "Use specific version of Kokkos")
mark_as_advanced(NEOFOAM_KOKKOS_CHECKOUT_VERSION)

if(NEOFOAM_ENABLE_MPI_SUPPORT)
  find_package(MPI 3.1 REQUIRED)
endif()
find_package(Kokkos ${NEOFOAM_KOKKOS_CHECKOUT_VERSION} QUIET)

if(NOT ${Kokkos_FOUND})
  include(FetchContent)
  include(cmake/AutoEnableDevice.cmake)

  FetchContent_Declare(
    kokkos
    SYSTEM QUITE
    GIT_SHALLOW ON
    GIT_REPOSITORY "https://github.com/kokkos/kokkos.git"
    GIT_TAG ${NEOFOAM_KOKKOS_CHECKOUT_VERSION})

  FetchContent_MakeAvailable(Kokkos)
else()
  message(STATUS "Found Kokkos ${NEOFOAM_KOKKOS_CHECKOUT_VERSION}")
endif()

include(cmake/CPM.cmake)

cpmaddpackage(
  NAME
  cpptrace
  URL
  https://github.com/jeremy-rifkin/cpptrace/archive/refs/tags/v0.7.3.zip
  VERSION
  0.7.3
  SYSTEM)

cpmaddpackage(
  NAME
  nlohmann_json
  URL
  https://github.com/nlohmann/json/releases/download/v3.11.3/include.zip
  VERSION
  3.11.3
  SYSTEM)

cpmaddpackage(
  NAME
  spdlog
  URL
  https://github.com/gabime/spdlog/archive/refs/tags/v1.13.0.zip
  VERSION
  1.13.0
  SYSTEM)

cpmaddpackage(
  NAME
  cxxopts
  URL
  https://github.com/jarro2783/cxxopts/archive/refs/tags/v3.2.0.zip
  VERSION
  3.2.0
  SYSTEM)

if(NEOFOAM_ENABLE_GINKGO)
  set(GINKGO_BUILD_TESTS
      OFF
      CACHE INTERNAL "")
  set(GINKGO_BUILD_BENCHMARKS
      OFF
      CACHE INTERNAL "")
  set(GINKGO_BUILD_EXAMPLES
      OFF
      CACHE INTERNAL "")
  cpmaddpackage(
    NAME
    Ginkgo
    GITHUB_REPOSITORY
    ginkgo-project/ginkgo
    GIT_TAG
    batch-optim
    VERSION
    1.9.0
    SYSTEM)
endif()

if(NEOFOAM_ENABLE_PETSC)
  find_package(PkgConfig REQUIRED)
  pkg_search_module(PETSc REQUIRED IMPORTED_TARGET PETSc)
endif()

if(NEOFOAM_BUILD_TESTS OR NEOFOAM_BUILD_BENCHMARKS)
  cpmaddpackage(
    NAME
    Catch2
    URL
    https://github.com/catchorg/Catch2/archive/refs/tags/v3.4.0.zip
    VERSION
    3.4.0
    SYSTEM)
endif()
