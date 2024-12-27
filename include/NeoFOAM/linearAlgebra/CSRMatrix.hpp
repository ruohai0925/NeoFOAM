// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 NeoFOAM authors
#pragma once

#include "NeoFOAM/fields/field.hpp"


namespace NeoFOAM::la
{

template<typename ValueType, typename IndexType>
class CSRMatrix
{

public:

    CSRMatrix(
        const Field<ValueType>& values, const Field<IndexType>& cols, const Field<IndexType>& rows
    )
        : values_(values), col_idxs_(cols), row_ptrs_(rows) {};

    ~CSRMatrix() = default;

    [[nodiscard]] Field<ValueType>& values() { return values_; }
    [[nodiscard]] Field<IndexType>& cols() { return col_idxs_; }
    [[nodiscard]] Field<IndexType>& rows() { return row_ptrs_; }

    [[nodiscard]] const Field<ValueType>& values() const { return values_; }
    [[nodiscard]] const Field<IndexType>& cols() const { return col_idxs_; }
    [[nodiscard]] const Field<IndexType>& rows() const { return row_ptrs_; }


private:

    Field<ValueType> values_;
    Field<IndexType> col_idxs_;
    Field<IndexType> row_ptrs_;
};

} // namespace NeoFOAM
