// Copyright (c) 2011-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/util/setup_common.h>
//#include <blsct/arith/range_proof/range_proof.h>
#include <blsct/arith/range_proof/range_proof_orig.h>

#include <boost/test/unit_test.hpp>
#include <util/strencodings.h>

BOOST_FIXTURE_TEST_SUITE(range_proof_tests, MclTestingSetup)

BOOST_AUTO_TEST_CASE(test_range_proof_parallel_test_with_navcoin_core)
{
    G1Point nonce = G1Point::GetBasePoint();
    auto buf = nonce.GetVch();
    printf("G: %s\n", HexStr(buf).c_str());

    Scalar one(1);
    std::vector<Scalar> vs;
    vs.push_back(one);

    //RangeProof range_proof;
    BulletproofsRangeproof range_proof;

    // test one
    {
        range_proof.Prove(vs, nonce, {1, 2, 3, 4});

        std::vector<G1Point> nonces { nonce };
        std::pair<int, BulletproofsRangeproof> proof1(1, range_proof);
        std::vector<std::pair<int, BulletproofsRangeproof>> proofs { proof1 };
        RangeproofEncodedData red;
        std::vector<RangeproofEncodedData> data;

        auto is_valid = VerifyBulletproof(
            proofs,
            data,
            nonces,
            false
        );
        BOOST_CHECK(is_valid);

        BOOST_CHECK(data.size() == 1);
        printf("data gamma=%s\n", data[0].gamma.GetString().c_str());
        printf("data amount=%ld\n", data[0].amount);
        printf("data message=%s\n", data[0].message.c_str());
    }

    // // test each valid value individually
    // int i = 1;
    // for (Scalar v: in_range) {
    //     printf("=====> TEST %d/%ld\n", i++, in_range.size());
    //     Scalars vs;
    //     vs.Add(v);
    //     auto proof = range_proof.Prove(vs, nonce, message, token_id);
    //     auto is_valid = range_proof.Verify(std::vector<Proof> { proof }, token_id);
    //     BOOST_CHECK(is_valid);
    // }

    // test each invalid value individually
    // test all valid values as a batch
    // test all invalid values as a batch
}

// BOOST_AUTO_TEST_CASE(test_range_proof_simplest_prove_and_verify)
// {
//     Scalar one(1);
//     Scalar two(2);
//     Scalar twoPow64 = two << 63; // = 2^64

//     // valid range is [0, 2**64)
//     Scalar lower_bound(0);
//     Scalar upper_bound = twoPow64 - one;

//     std::vector<Scalar> in_range {
//         lower_bound,
//         lower_bound + one,
//         upper_bound - one,
//         upper_bound
//     };
//     std::vector<Scalar> out_of_range {
//         upper_bound + one,
//         upper_bound + two,
//         upper_bound * two,
//         one.Negate()
//     };

//     std::string nonce_str("nonce");
//     G1Point nonce = G1Point::HashAndMap(std::vector<unsigned char> { nonce_str.begin(), nonce_str.end() });

//     std::string msg_str("spagetti meatballs");
//     std::vector<unsigned char> message { msg_str.begin(), msg_str.end() };

//     TokenId token_id(uint256(123));

//     std::vector<Scalar> vs;
//     vs.push_back(one);

//     //RangeProof range_proof;
//     BulletproofsRangeproof range_proof;

//     // test one
//     {
//         range_proof.Prove(vs, nonce, message, token_id);

//         std::vector<G1Point> nonces { nonce };
//         std::pair<int, BulletproofsRangeproof> proof1(1, range_proof);
//         std::vector<std::pair<int, BulletproofsRangeproof>> proofs { proof1 };
//         RangeproofEncodedData red;
//         std::vector<RangeproofEncodedData> data;

//         auto is_valid = VerifyBulletproof(
//             proofs,
//             data,
//             nonces,
//             false,
//             token_id
//         );
//         BOOST_CHECK(is_valid);

//         BOOST_CHECK(data.size() == 1);
//         printf("data gamma=%s\n", data[0].gamma.GetString().c_str());
//         printf("data amount=%ld\n", data[0].amount);
//         printf("data message=%s\n", data[0].message.c_str());
//     }

//     // // test each valid value individually
//     // int i = 1;
//     // for (Scalar v: in_range) {
//     //     printf("=====> TEST %d/%ld\n", i++, in_range.size());
//     //     Scalars vs;
//     //     vs.Add(v);
//     //     auto proof = range_proof.Prove(vs, nonce, message, token_id);
//     //     auto is_valid = range_proof.Verify(std::vector<Proof> { proof }, token_id);
//     //     BOOST_CHECK(is_valid);
//     // }

//     // test each invalid value individually
//     // test all valid values as a batch
//     // test all invalid values as a batch
// }

BOOST_AUTO_TEST_CASE(test_range_proof_inner_product_argument)
{
}

BOOST_AUTO_TEST_CASE(test_range_proof_get_first_power_of_2_greater_or_eq_to)
{
}

BOOST_AUTO_TEST_CASE(test_range_proof_prove)
{
}

BOOST_AUTO_TEST_CASE(test_range_proof_verify)
{
}

BOOST_AUTO_TEST_CASE(test_range_proof_validate_proofs_by_sizes)
{
}

BOOST_AUTO_TEST_CASE(test_range_proof_recovery)
{
    /*
    std::string nonce_str("nonce");
    G1Point nonce = G1Point::HashAndMap(std::vector<unsigned char> { nonce_str.begin(), nonce_str.end() });

    std::string msg_str("spagetti meatballs");
    std::vector<unsigned char> message { msg_str.begin(), msg_str.end() };

    TokenId token_id(uint256(123));

    std::vector<Scalar> vs;
    Scalar amount(1);
    vs.push_back(amount);

    //RangeProof range_proof;
    BulletproofsRangeproof range_proof;

    // test one
    {
        range_proof.Prove(vs, nonce, message, token_id);

        std::vector<G1Point> nonces { nonce };
        std::pair<int, BulletproofsRangeproof> proof1(1, range_proof);
        std::vector<std::pair<int, BulletproofsRangeproof>> proofs { proof1 };
        RangeproofEncodedData red;
        std::vector<RangeproofEncodedData> data;

        auto is_valid = VerifyBulletproof(
            proofs,
            data,
            nonces,
            false,
            token_id
        );
        BOOST_CHECK(is_valid);

        BOOST_CHECK(data.size() == 1);
        BOOST_CHECK(data[0].gamma == nonce.GetHashWithSalt(100));
        BOOST_CHECK(data[0].amount == amount);
    }
    */
}

BOOST_AUTO_TEST_SUITE_END()
