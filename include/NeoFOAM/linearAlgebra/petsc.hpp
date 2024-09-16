#pragma once

#include <petsc.h>
// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 NeoFOAM authors

#include "NeoFOAM/linearAlgebra/ginkgo.hpp"

namespace NeoFOAM::la::petsc
{

class Matrix
{
public:

    struct SymbolicAssembly
    {
        SymbolicAssembly(Matrix& mat) : rowIdxs(mat.rowIdxs.data()), colIdxs(mat.colIdxs.data()) {}

        KOKKOS_FUNCTION void insert(size_t nnzId, MatrixCoordinate<PetscInt> coordinate) const
        {
            rowIdxs[nnzId] = coordinate.row;
            colIdxs[nnzId] = coordinate.col;
        }

        static Executor getCompatibleExecutor(const Executor& exec)
        {
            if (std::holds_alternative<GPUExecutor>(exec))
            {
                return CPUExecutor {};
            }

            return exec;
        }

    private:

        friend Matrix;

        PetscInt* rowIdxs;
        PetscInt* colIdxs;
    };

    struct NumericAssembly : CompatibleWithAnyExecutor
    {
        NumericAssembly(Matrix& mat) : values(mat.values.data()) {}

        KOKKOS_FUNCTION void insert(size_t nnzId, PetscScalar value) const
        {
            values[nnzId] = value;
        }

    private:

        friend Matrix;

        PetscScalar* values;
    };

    Matrix(Dim dim, size_t nnzEstimation)
        : rowIdxs(CPUExecutor {}, nnzEstimation), colIdxs(CPUExecutor {}, nnzEstimation),
          values(get_executor(), nnzEstimation)
    {
        MatCreate(MPI_COMM_WORLD, &mat);
        MatSetType(mat, MATSEQAIJKOKKOS);
        MatSetSizes(mat, dim.numRows, dim.numCols, dim.numRows, dim.numCols);
    }


    Matrix(
        Dim dim, Field<PetscInt> rowIdxs, Field<PetscInt> colIdxs, const Field<PetscScalar>& values
    )
        : rowIdxs(CPUExecutor {}, 0), colIdxs(CPUExecutor {}, 0), values(get_executor(), 0)
    {
        MatCreate(MPI_COMM_WORLD, &mat);
        MatSetType(mat, MATSEQAIJKOKKOS);
        MatSetSizes(mat, dim.numRows, dim.numCols, dim.numRows, dim.numCols);
        MatSetPreallocationCOO(mat, rowIdxs.size(), rowIdxs.data(), colIdxs.data());
        MatSetValuesCOO(mat, values.data(), INSERT_VALUES);
    }

    SymbolicAssembly startSymbolicAssembly()
    {
        rowIdxs.resize(nnzEstimation);
        colIdxs.resize(nnzEstimation);
        values.resize(nnzEstimation);
        fill(rowIdxs, 0);
        fill(colIdxs, 0);
        fill(values, 0);
        return {*this};
    }

    NumericAssembly startNumericAssembly(SymbolicAssembly&& assembly)
    {
        MatSetPreallocationCOO(mat, rowIdxs.size(), rowIdxs.data(), colIdxs.data());
        rowIdxs.resize(0);
        colIdxs.resize(0);
        assembly.rowIdxs = nullptr;
        assembly.colIdxs = nullptr;
        return {*this};
    }

    void finishNumericAssembly(NumericAssembly&& assembly)
    {
        MatSetValuesCOO(mat, values.data(), INSERT_VALUES);
        values.resize(0);
        assembly.values = nullptr;
    }

    void apply(const Field<PetscScalar>& in, Field<PetscScalar>& out)
    {
        Vec petscIn;
        Vec petscOut;

        VecCreateSeqKokkosWithArray(MPI_COMM_WORLD, 1, in.size(), in.data(), &petscIn);
        VecCreateSeqKokkosWithArray(MPI_COMM_WORLD, 1, out.size(), out.data(), &petscOut);

        MatMult(mat, petscIn, petscOut);
    }

    Executor get_executor() const { return GPUExecutor {}; }

    Mat getUnderlying() { return mat; }

private:

    size_t nnzEstimation;

    Field<PetscInt> rowIdxs;
    Field<PetscInt> colIdxs;
    Field<PetscScalar> values;

    Mat mat;
};

}
