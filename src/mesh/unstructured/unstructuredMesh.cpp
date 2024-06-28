// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 NeoFOAM authors

#include "NeoFOAM/mesh/unstructured.hpp"

namespace NeoFOAM
{
UnstructuredMesh::UnstructuredMesh()
    : exec_(NeoFOAM::CPUExecutor()), points_(exec_, 0), cellVolumes_(exec_, 0),
      cellCentres_(exec_, 0), faceAreas_(exec_, 0), faceCentres_(exec_, 0), magFaceAreas_(exec_, 0),
      faceOwner_(exec_, 0), faceNeighbour_(exec_, 0), nCells_(0), nInternalFaces_(0),
      nBoundaryFaces_(0), nBoundaries_(0), nFaces_(0), boundaryMesh_(), stencilDataBase_() {};

UnstructuredMesh::UnstructuredMesh(
    vectorField points,
    scalarField cellVolumes,
    vectorField cellCentres,
    vectorField faceAreas,
    vectorField faceCentres,
    scalarField magFaceAreas,
    labelField faceOwner,
    labelField faceNeighbour,
    localIdx nCells,
    localIdx nInternalFaces,
    localIdx nBoundaryFaces,
    localIdx nBoundaries,
    localIdx nFaces,
    BoundaryMesh boundaryMesh
)
    : exec_(points.exec()), points_(points), cellVolumes_(cellVolumes), cellCentres_(cellCentres),
      faceAreas_(faceAreas), faceCentres_(faceCentres), magFaceAreas_(magFaceAreas),
      faceOwner_(faceOwner), faceNeighbour_(faceNeighbour), nCells_(nCells),
      nInternalFaces_(nInternalFaces), nBoundaryFaces_(nBoundaryFaces), nBoundaries_(nBoundaries),
      nFaces_(nFaces), boundaryMesh_(boundaryMesh), stencilDataBase_()
{}


const vectorField& UnstructuredMesh::points() const { return points_; }

const scalarField& UnstructuredMesh::cellVolumes() const { return cellVolumes_; }

const vectorField& UnstructuredMesh::cellCentres() const { return cellCentres_; }

const vectorField& UnstructuredMesh::faceCentres() const { return faceCentres_; }

const vectorField& UnstructuredMesh::faceAreas() const { return faceAreas_; }

const scalarField& UnstructuredMesh::magFaceAreas() const { return magFaceAreas_; }

const labelField& UnstructuredMesh::faceOwner() const { return faceOwner_; }

const labelField& UnstructuredMesh::faceNeighbour() const { return faceNeighbour_; }

localIdx UnstructuredMesh::nCells() const { return nCells_; }

localIdx UnstructuredMesh::nInternalFaces() const { return nInternalFaces_; }

localIdx UnstructuredMesh::nBoundaryFaces() const { return nBoundaryFaces_; }

localIdx UnstructuredMesh::nBoundaries() const { return nBoundaries_; }

localIdx UnstructuredMesh::nFaces() const { return nFaces_; }

const BoundaryMesh& UnstructuredMesh::boundaryMesh() const { return boundaryMesh_; }

StencilDataBase& UnstructuredMesh::stencilDB() const { return stencilDataBase_; }

const Executor& UnstructuredMesh::exec() const { return exec_; }

UnstructuredMesh createSingleCellMesh()
{
    Executor exec = CPUExecutor();

    // a 2D mesh in 3D space with left, right, top, bottom boundary faces
    // with the centre at (0.5, 0.5, 0.0)
    // left, top, right, bottom faces
    // and two boundaries one (left, top),  (right, bottom)
    vectorField faceAreas({{-1, 0, 0}, {0, 1, 0}, {1, 0, 0}, {0, -1, 0}});
    vectorField faceCentres({{0.0, 0.5, 0.0}, {0.5, 1.0, 0.0}, {1.0, 0.5, 0.0}, {0.5, 0.0, 0.0}});
    scalarField magFaceAreas({1, 1, 1, 1});

    BoundaryMesh boundaryMesh(
        exec,
        {0, 0, 0, 0},                                                           // faceCells
        faceCentres,                                                            // cf
        faceAreas,                                                              // cn,
        faceAreas,                                                              // sf,
        magFaceAreas,                                                           // magSf,
        faceAreas,                                                              //
        {{1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}}, // delta
        {1, 1, 1, 1},                                                           // weights
        {0.5, 0.5, 0.5, 0.5},                                                   // deltaCoeffs
        {0, 0, 1, 1}                                                            // offset
    );
    return UnstructuredMesh(
        {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}}, // points,
        {1},                                          // cellVolumes
        {{0.5, 0.5, 0.0}},                            // cellCentres
        faceAreas,
        faceCentres,
        magFaceAreas,
        {0, 0, 0, 0}, // faceOwner
        {},           // faceNeighbour,
        1,            // nCells
        0,            // nInternalFaces,
        4,            // nBoundaryFaces,
        2,            // nBoundaries,
        4,            // nFaces,
        boundaryMesh
    );
}
} // namespace NeoFOAM
