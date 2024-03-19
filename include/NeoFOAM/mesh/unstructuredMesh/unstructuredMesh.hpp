// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: 2023 NeoFOAM authors
#pragma once

#include <array>
#include <vector>

#include "NeoFOAM/fields/FieldTypeDefs.hpp"
#include "NeoFOAM/primitives/vector.hpp"
#include "NeoFOAM/mesh/unstructuredMesh/BoundaryMesh.hpp"

namespace NeoFOAM
{

class unstructuredMesh
{
public:

    unstructuredMesh(
        vectorField points,
        scalarField cellVolumes,
        vectorField cellCentres,
        vectorField faceAreas,
        vectorField faceCentres,
        scalarField magFaceAreas,
        labelField faceOwner,
        labelField faceNeighbour,
        int32_t nCells,
        int32_t nInternalFaces,
        BoundaryMesh boundaryMesh
    );

    const vectorField& points() const;

    const scalarField& cellVolumes() const;

    const vectorField& cellCentres() const;

    const vectorField& faceCentres() const;

    const vectorField& faceAreas() const;

    const scalarField& magFaceAreas() const;

    const labelField& faceOwner() const;

    const labelField& faceNeighbour() const;

    int32_t nCells() const;

    int32_t nInternalFaces() const;

    const BoundaryMesh& boundaryMesh() const;


private:

    vectorField points_; // points

    scalarField cellVolumes_; // cell volume
    vectorField cellCentres_; // cell centre

    vectorField faceAreas_;    // face area vector
    vectorField faceCentres_;  // face centre vector
    scalarField magFaceAreas_; // face area

    labelField faceOwner_;     // owner cell
    labelField faceNeighbour_; // neighbour cell

    int32_t nCells_;         // number of cells
    int32_t nInternalFaces_; // number of internal faces

    BoundaryMesh boundaryMesh_; // boundary mesh

};

} // namespace NeoFOAM