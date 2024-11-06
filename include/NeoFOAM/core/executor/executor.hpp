// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 NeoFOAM authors
#pragma once

#include "NeoFOAM/core/executor/serialExecutor.hpp"
#include "NeoFOAM/core/executor/GPUExecutor.hpp"
#include "NeoFOAM/core/executor/CPUExecutor.hpp"
#include "NeoFOAM/core/error.hpp"
#include <string>
#include <variant>

namespace NeoFOAM
{

using Executor = std::variant<CPUExecutor, GPUExecutor, SerialExecutor>;

/**
 * @brief Checks if two executors are equal, i.e. they are of the same type.
 * @param lhs The first executor.
 * @param rhs The second executor.
 * @return True if the executors are equal, false otherwise.
 */
[[nodiscard]] inline bool operator==(const Executor& lhs, const Executor& rhs)
{
    return std::visit(
        []<typename ExecLhs,
           typename ExecRhs>([[maybe_unused]] const ExecLhs&, [[maybe_unused]] const ExecRhs&)
        {
            if constexpr (std::is_same_v<ExecLhs, ExecRhs>)
            {
                return typename ExecLhs::exec() == typename ExecRhs::exec();
            }
            else
            {
                return false;
            }
        },
        lhs,
        rhs
    );
};

/**
 * @brief Checks if two executors are not equal, i.e. they are not of the same
 * type.
 * @param lhs The first executor.
 * @param rhs The second executor.
 * @return True if the executors not are equal, false otherwise.
 */
[[nodiscard]] inline bool operator!=(const Executor& lhs, const Executor& rhs)
{
    return !(lhs == rhs);
};

} // namespace NeoFOAM

// extension of the standard namespace so we can specialise to_string.
namespace std
{
/**
 * @brief Returns the name of the executor in std::string from.
 * @param executor The Executor whose name will be converted to a std::string.
 */
[[nodiscard]] inline std::string to_string(const NeoFOAM::Executor& executor)
{
    if (std::holds_alternative<NeoFOAM::CPUExecutor>(executor))
    {
        return std::get<NeoFOAM::CPUExecutor>(executor).name();
    }
    else if (std::holds_alternative<NeoFOAM::GPUExecutor>(executor))
    {
        return std::get<NeoFOAM::GPUExecutor>(executor).name();
    }
    else if (std::holds_alternative<NeoFOAM::SerialExecutor>(executor))
    {
        return std::get<NeoFOAM::SerialExecutor>(executor).name();
    }
    else
    {
        return "Unknown Executor.";
    }
}
}
