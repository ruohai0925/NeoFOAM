// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 NeoFOAM authors

#define CATCH_CONFIG_RUNNER // Define this before including catch.hpp to create
                            // a custom main
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "NeoFOAM/NeoFOAM.hpp"

// only needed for msvc
#include "NeoFOAM/finiteVolume/cellCentred/boundary.hpp"
#include "NeoFOAM/finiteVolume/cellCentred/interpolation/linear.hpp"

using NeoFOAM::finiteVolume::cellCentred::SurfaceInterpolation;
using NeoFOAM::finiteVolume::cellCentred::VolumeField;
using NeoFOAM::finiteVolume::cellCentred::SurfaceField;

TEST_CASE("linear")
{
    NeoFOAM::Executor exec = GENERATE(
        NeoFOAM::Executor(NeoFOAM::SerialExecutor {}),
        NeoFOAM::Executor(NeoFOAM::CPUExecutor {}),
        NeoFOAM::Executor(NeoFOAM::GPUExecutor {})
    );

    std::string execName = std::visit([](auto e) { return e.name(); }, exec);
    auto mesh = NeoFOAM::createSingleCellMesh(exec);
    NeoFOAM::Input input = NeoFOAM::TokenList({std::string("linear")});
    auto linear = SurfaceInterpolation(exec, mesh, input);

    auto in = VolumeField<NeoFOAM::scalar>(exec, "in", mesh, {});
    auto out = SurfaceField<NeoFOAM::scalar>(exec, "out", mesh, {});
}
