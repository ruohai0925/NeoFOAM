// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: 2023 NeoFOAM authors
#pragma once

#include "NeoFOAM/fields/FieldTypeDefs.hpp"
#include "NeoFOAM/core/executor/executor.hpp"
#include "NeoFOAM/cellCentredFiniteVolume/surfaceInterpolation/surfaceInterpolation.hpp"
#include "NeoFOAM/mesh/unstructuredMesh/unstructuredMesh.hpp"
#include "NeoFOAM/mesh/stencil/FvccGeometryScheme.hpp"
#include "Kokkos_Core.hpp"
#include <functional>


namespace NeoFOAM
{

class linear :
    public surfaceInterpolationKernel
{

public:

    linear(const executor& exec, const unstructuredMesh& mesh);

    void operator()(const GPUExecutor& exec, fvccSurfaceField<scalar>& surfaceField, const fvccVolField<scalar>& volField);
    void operator()(const OMPExecutor& exec, fvccSurfaceField<scalar>& surfaceField, const fvccVolField<scalar>& volField);
    void operator()(const CPUExecutor& exec, fvccSurfaceField<scalar>& surfaceField, const fvccVolField<scalar>& volField);

private:

    const unstructuredMesh& mesh_;
    // const FvccGeometryScheme geometryScheme_;
    const std::shared_ptr<FvccGeometryScheme> geometryScheme_;

};


} // namespace NeoFOAM