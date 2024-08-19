// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 NeoFOAM authors

#define CATCH_CONFIG_RUNNER // Define this before including catch.hpp to create
                            // a custom main
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "NeoFOAM/fields/field.hpp"
#include "NeoFOAM/core/executor/executor.hpp"
#include "NeoFOAM/core/parallelAlgorithms.hpp"


TEST_CASE("parallelFor")
{
    NeoFOAM::Executor exec = GENERATE(
        NeoFOAM::Executor(NeoFOAM::SerialExecutor {}),
        NeoFOAM::Executor(NeoFOAM::CPUExecutor {}),
        NeoFOAM::Executor(NeoFOAM::GPUExecutor {})
    );
    std::string execName = std::visit([](auto e) { return e.print(); }, exec);

    SECTION("parallelFor_" + execName)
    {
        NeoFOAM::Field<NeoFOAM::scalar> fieldA(exec, 5);
        NeoFOAM::fill(fieldA, 0.0);
        NeoFOAM::Field<NeoFOAM::scalar> fieldB(exec, 5);
        auto spanA = fieldA.span();
        auto spanB = fieldB.span();
        NeoFOAM::fill(fieldB, 1.0);
        NeoFOAM::parallelFor(
            exec, {0, 5}, KOKKOS_LAMBDA(const size_t i) { spanA[i] = spanB[i] + 2.0; }
        );
        auto hostSpanA = fieldA.copyToHost().span();
        for (auto value : hostSpanA)
        {
            REQUIRE(value == 3.0);
        }
    }

    SECTION("parallelFor_Vector" + execName)
    {
        NeoFOAM::Field<NeoFOAM::Vector> fieldA(exec, 5);
        NeoFOAM::fill(fieldA, NeoFOAM::Vector(0.0, 0.0, 0.0));
        NeoFOAM::Field<NeoFOAM::Vector> fieldB(exec, 5);
        auto spanA = fieldA.span();
        auto spanB = fieldB.span();
        NeoFOAM::fill(fieldB, NeoFOAM::Vector(1.0, 1.0, 1.0));
        NeoFOAM::parallelFor(
            exec,
            {0, 5},
            KOKKOS_LAMBDA(const size_t i) { spanA[i] = spanB[i] + NeoFOAM::Vector(2.0, 2.0, 2.0); }
        );
        auto hostSpanA = fieldA.copyToHost().span();
        for (auto value : hostSpanA)
        {
            REQUIRE(value == NeoFOAM::Vector(3.0, 3.0, 3.0));
        }
    }

    SECTION("parallelFor_Field_" + execName)
    {
        NeoFOAM::Field<NeoFOAM::scalar> fieldA(exec, 5);
        NeoFOAM::fill(fieldA, 0.0);
        NeoFOAM::Field<NeoFOAM::scalar> fieldB(exec, 5);
        auto spanA = fieldA.span();
        auto spanB = fieldB.span();
        NeoFOAM::fill(fieldB, 1.0);
        NeoFOAM::parallelFor(
            fieldA, KOKKOS_LAMBDA(const size_t i) { return spanB[i] + 2.0; }
        );
        auto hostSpanA = fieldA.copyToHost().span();
        for (auto value : hostSpanA)
        {
            REQUIRE(value == 3.0);
        }
    }

    SECTION("parallelFor_Field_Visit" + execName)
    {
        NeoFOAM::Field<NeoFOAM::scalar> fieldA(exec, 5, 0.0);
        NeoFOAM::Field<NeoFOAM::scalar> fieldB(exec, 5, 1.0);
        auto [spanA, spanB] = NeoFOAM::spans(fieldA, fieldB);
        NeoFOAM::ValueOrSpan<NeoFOAM::scalar> cOrS = spanB;

        NeoFOAM::parallelFor(
            fieldA, KOKKOS_LAMBDA(const size_t i) { return cOrS[i] + 2.0; }
        );

        auto hostSpanA = fieldA.copyToHost();
        for (auto value : hostSpanA.span())
        {
            REQUIRE(value == 3.0);
        }

        cOrS = 1.0;

        NeoFOAM::parallelFor(
            fieldA, KOKKOS_LAMBDA(const size_t i) { return cOrS[i] + 2.0; }
        );

        hostSpanA = fieldA.copyToHost();
        for (auto value : hostSpanA.span())
        {
            REQUIRE(value == 3.0);
        }
    }
};

TEST_CASE("ConstantOrSpan Benchmark")
{
    size_t size = GENERATE(1e2, 1e3, 1e4, 1e5, 1e6, 1e7);

    NeoFOAM::Executor exec = GENERATE(
        NeoFOAM::Executor(NeoFOAM::SerialExecutor {}),
        NeoFOAM::Executor(NeoFOAM::CPUExecutor {}),
        NeoFOAM::Executor(NeoFOAM::GPUExecutor {})
    );

    std::string execName = std::visit([](auto e) { return e.print(); }, exec);

    NeoFOAM::Field<NeoFOAM::scalar> fieldA(exec, size, 0.0);
    NeoFOAM::Field<NeoFOAM::scalar> fieldB(exec, size, 1.0);
    auto [spanA, spanB] = NeoFOAM::spans(fieldA, fieldB);
    NeoFOAM::ValueOrSpan<NeoFOAM::scalar> cOrS = spanB;
    REQUIRE(cOrS.useSpan == true);

    BENCHMARK("ValueOrSpan<NeoFOAM::scalar> Span " + execName + " " + std::to_string(size))
    {
        NeoFOAM::parallelFor(
            fieldA, KOKKOS_LAMBDA(const size_t i) { return cOrS[i] + 2.0; }
        );
    };

    cOrS = 1.0;
    REQUIRE(cOrS.useSpan == false);

    BENCHMARK("ValueOrSpan<NeoFOAM::scalar> Constant " + execName + " " + std::to_string(size))
    {
        NeoFOAM::parallelFor(
            fieldA, KOKKOS_LAMBDA(const size_t i) { return cOrS[i] + 2.0; }
        );
    };

    BENCHMARK("ValueOrSpan<NeoFOAM::scalar> Double " + execName + " " + std::to_string(size))
    {
        NeoFOAM::parallelFor(
            fieldA, KOKKOS_LAMBDA(const size_t i) { return 1.0 + 2.0; }
        );
    };
}


TEST_CASE("parallelReduce")
{
    NeoFOAM::Executor exec = GENERATE(
        NeoFOAM::Executor(NeoFOAM::SerialExecutor {}),
        NeoFOAM::Executor(NeoFOAM::CPUExecutor {}),
        NeoFOAM::Executor(NeoFOAM::GPUExecutor {})
    );
    std::string execName = std::visit([](auto e) { return e.print(); }, exec);

    SECTION("parallelReduce_" + execName)
    {
        NeoFOAM::Field<NeoFOAM::scalar> fieldA(exec, 5);
        NeoFOAM::fill(fieldA, 0.0);
        NeoFOAM::Field<NeoFOAM::scalar> fieldB(exec, 5);
        auto spanA = fieldA.span();
        auto spanB = fieldB.span();
        NeoFOAM::fill(fieldB, 1.0);
        NeoFOAM::scalar sum = 0.0;
        NeoFOAM::parallelReduce(
            exec, {0, 5}, KOKKOS_LAMBDA(const size_t i, double& lsum) { lsum += spanB[i]; }, sum
        );

        REQUIRE(sum == 5.0);
    }

    SECTION("parallelReduce_Field_" + execName)
    {
        NeoFOAM::Field<NeoFOAM::scalar> fieldA(exec, 5);
        NeoFOAM::fill(fieldA, 0.0);
        NeoFOAM::Field<NeoFOAM::scalar> fieldB(exec, 5);
        auto spanA = fieldA.span();
        auto spanB = fieldB.span();
        NeoFOAM::fill(fieldB, 1.0);
        NeoFOAM::scalar sum = 0.0;
        NeoFOAM::parallelReduce(
            fieldA, KOKKOS_LAMBDA(const size_t i, double& lsum) { lsum += spanB[i]; }, sum
        );

        REQUIRE(sum == 5.0);
    }
};
