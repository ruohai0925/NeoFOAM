// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 NeoFOAM authors

#define CATCH_CONFIG_RUNNER // Define this before including catch.hpp to create
                            // a custom main
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "NeoFOAM/finiteVolume/cellCentred/fields/volumeField.hpp"
#include "NeoFOAM/finiteVolume/cellCentred/boundary/volumeBoundaryFactory.hpp"

template<typename T>
using I = std::initializer_list<T>;

TEST_CASE("volumeField")
{
    namespace fvcc = NeoFOAM::finiteVolume::cellCentred;

    NeoFOAM::Executor exec = GENERATE(
        NeoFOAM::Executor(NeoFOAM::SerialExecutor {}),
        NeoFOAM::Executor(NeoFOAM::OMPExecutor {}),
        NeoFOAM::Executor(NeoFOAM::GPUExecutor {})
    );

    std::string execName = std::visit([](auto e) { return e.print(); }, exec);

    SECTION("can instantiate volumeField with fixedValues on: " + execName)
    {
        NeoFOAM::UnstructuredMesh mesh = NeoFOAM::createSingleCellMesh(exec);
        std::vector<std::unique_ptr<fvcc::VolumeBoundary<NeoFOAM::scalar>>> bcs {};
        for (auto patchi : I<size_t> {0, 1, 2, 3})
        {
            NeoFOAM::Dictionary dict;
            dict.insert("type", std::string("fixedValue"));
            dict.insert("fixedValue", 2.0);
            bcs.push_back(
                std::make_unique<fvcc::VolumeBoundary<NeoFOAM::scalar>>(mesh, dict, patchi)
            );
        }
        fvcc::VolumeField<NeoFOAM::scalar> vf(exec, mesh, std::move(bcs));
        NeoFOAM::fill(vf.internalField(), 1.0);
        vf.correctBoundaryConditions();

        REQUIRE(vf.exec() == exec);
        REQUIRE(vf.internalField().size() == 1);

        auto internalValues = vf.internalField().copyToHost();
        for (size_t i = 0; i < internalValues.size(); ++i)
        {
            REQUIRE(internalValues[i] == 1.0);
        }

        auto values = vf.boundaryField().value().copyToHost();
        for (size_t i = 0; i < values.size(); ++i)
        {
            REQUIRE(values[i] == 2.0);
        }

        auto refValue = vf.boundaryField().refValue().copyToHost();
        for (size_t i = 0; i < refValue.size(); ++i)
        {
            REQUIRE(refValue[i] == 2.0);
        }
    }
}
