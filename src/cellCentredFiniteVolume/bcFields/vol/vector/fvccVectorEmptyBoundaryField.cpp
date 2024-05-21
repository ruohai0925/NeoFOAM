// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 NeoFOAM authors

#include "NeoFOAM/cellCentredFiniteVolume/bcFields/vol/vector/fvccVectorEmptyBoundaryField.hpp"

namespace NeoFOAM
{
fvccVectorEmptyBoundaryField::fvccVectorEmptyBoundaryField(
    const UnstructuredMesh& mesh, int patchID
)
    : fvccBoundaryField<Vector>(mesh, patchID)
{}

void fvccVectorEmptyBoundaryField::correctBoundaryConditions(
    BoundaryFields<Vector>& bfield, const Field<Vector>& internalField
)
{
    // do nothing
}


}
