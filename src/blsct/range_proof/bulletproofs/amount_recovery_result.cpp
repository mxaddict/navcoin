// Copyright (c) 2022 The Navio developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <blsct/range_proof/bulletproofs/amount_recovery_result.h>
#include <blsct/arith/mcl/mcl.h>

namespace bulletproofs {

template <typename T>
AmountRecoveryResult<T> AmountRecoveryResult<T>::failure()
{
    return {
        false,
        std::vector<range_proof::RecoveredData<T>>()
    };
}
template AmountRecoveryResult<Mcl> AmountRecoveryResult<Mcl>::failure();

} // namespace bulletproofs
