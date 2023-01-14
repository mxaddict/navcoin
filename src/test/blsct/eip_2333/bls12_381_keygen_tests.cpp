// Copyright (c) 2011-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#define BOOST_UNIT_TEST

#include <boost/test/unit_test.hpp>
#include <test/util/setup_common.h>
#include <util/strencodings.h>
#include <blsct/eip_2333/bls12_381_keygen.h>

BOOST_FIXTURE_TEST_SUITE(bls12_381_keygen_tests, MclTestingSetup)

BOOST_AUTO_TEST_CASE(test_i2osp)
{
    // Size zero is invalid
    {
        MclScalar s(31);
        BOOST_CHECK_THROW(BLS12_381_KeyGen::I2OSP(s, 0), std::runtime_error);
    }
    // 1 byte cases
    {
        MclScalar s(0);
        auto act = BLS12_381_KeyGen::I2OSP(s, 1);
        std::vector<uint8_t> exp = { 0x00 };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(1);
        auto act = BLS12_381_KeyGen::I2OSP(s, 1);
        std::vector<uint8_t> exp = { 0x01 };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(255);
        auto act = BLS12_381_KeyGen::I2OSP(s, 1);
        std::vector<uint8_t> exp = { 0xff };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(256);
        BOOST_CHECK_THROW(BLS12_381_KeyGen::I2OSP(s, 1), std::runtime_error);
    }
    // 2 byte cases
    {
        MclScalar s(0);
        auto act = BLS12_381_KeyGen::I2OSP(s, 2);
        std::vector<uint8_t> exp = { 0x00, 0x00 };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(1);
        auto act = BLS12_381_KeyGen::I2OSP(s, 2);
        std::vector<uint8_t> exp = { 0x00, 0x01 };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(255);
        auto act = BLS12_381_KeyGen::I2OSP(s, 2);
        std::vector<uint8_t> exp = { 0x00, 0xff };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(256);
        auto act = BLS12_381_KeyGen::I2OSP(s, 2);
        std::vector<uint8_t> exp = { 0x01, 0x00 };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(65535);
        auto act = BLS12_381_KeyGen::I2OSP(s, 2);
        std::vector<uint8_t> exp = { 0xff, 0xff };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(65536);
        BOOST_CHECK_THROW(BLS12_381_KeyGen::I2OSP(s, 2), std::runtime_error);
    }
    // 4 byte cases
    {
        MclScalar s(0);
        auto act = BLS12_381_KeyGen::I2OSP(s, 4);
        std::vector<uint8_t> exp = { 0x00, 0x00, 0x00, 0x00 };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(1);
        auto act = BLS12_381_KeyGen::I2OSP(s, 4);
        std::vector<uint8_t> exp = { 0x00, 0x00, 0x00, 0x01 };
        BOOST_CHECK(act == exp);
    }
    {
        MclScalar s(std::numeric_limits<uint32_t>::max());
        auto act = BLS12_381_KeyGen::I2OSP(s, 4);
        std::vector<uint8_t> exp = { 0xff, 0xff, 0xff, 0xff };
        BOOST_CHECK(act == exp);
    }
    {
        uint64_t uint32_t_max = std::numeric_limits<uint32_t>::max();
        MclScalar s(uint32_t_max + 1);
        BOOST_CHECK_THROW(BLS12_381_KeyGen::I2OSP(s, 4), std::runtime_error);
    }

    // 32 byte cases
    {
        MclScalar s(0);
        auto act = BLS12_381_KeyGen::I2OSP(s, 32);
        std::vector<uint8_t> exp = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        BOOST_CHECK(act == exp);
    }
    {
        std::vector<uint8_t> bls12_381_r = {
            0x73,
            0xed,
            0xa7,
            0x53,
            0x29,
            0x9d,
            0x7d,
            0x48,
            0x33,
            0x39,
            0xd8,
            0x08,
            0x09,
            0xa1,
            0xd8,
            0x05,
            0x53,
            0xbd,
            0xa4,
            0x02,
            0xff,
            0xfe,
            0x5b,
            0xfe,
            0xff,
            0xff,
            0xff,
            0xff,
            0x00,
            0x00,
            0x00,
            0x01,
        };
        MclScalar s(bls12_381_r);
        auto act = BLS12_381_KeyGen::I2OSP(s, 32);

        // r is the order and act should become zero
        std::vector<uint8_t> exp = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        BOOST_CHECK(act == exp);
    }
}

BOOST_AUTO_TEST_CASE(test_bytes_split)
{
    {
        const size_t vec_size = 255 * BLS12_381_KeyGen::DigestSize;
        std::array<uint8_t,8160> a;
        for (size_t i=0; i<vec_size; ++i) {
            a[i] = i % 256;
        }
        auto chunks = BLS12_381_KeyGen::bytes_split(a);
        uint8_t v = 0;
        for (auto chunk : chunks) {
            for (auto act = chunk.cbegin(); act != chunk.cend(); ++act) {
                auto exp = v++ % 256;
                BOOST_CHECK(*act == exp);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(test_hkdf_extract)
{
    std::array<uint8_t,BLS12_381_KeyGen::DigestSize> exp {
        43,
        23,
        99,
        253,
        223,
        63,
        1,
        181,
        120,
        5,
        193,
        11,
        56,
        249,
        98,
        154,
        102,
        20,
        245,
        175,
        122,
        233,
        106,
        140,
        119,
        232,
        23,
        172,
        230,
        54,
        206,
        220,
    };
    std::vector<uint8_t> IKM { 0x01, 0x02 };
    std::vector<uint8_t> salt {123};
    auto act = BLS12_381_KeyGen::HKDF_Extract(salt, IKM);
    BOOST_CHECK(act == exp);
}

BOOST_AUTO_TEST_CASE(test_hkdf_expand_48)
{
    const auto L = 48;
    std::array<uint8_t,L> exp {
        186,41,169,168,113,140,11,144,18,203,192,15,53,250,248,163,204,206,73,147,200,232,246,246,171,251,181,40,53,162,86,241,99,116,242,235,29,115,236,9,207,52,181,119,225,157,68,150,
    };
    std::vector<uint8_t> IKM {
        1,2,3,4,5,6,7,8,9,10,
        11,12,13,14,15,16,17,18,19,20,
        21,22,23,24,25,26,27,28,29,30,
        31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,
    };
    std::vector<uint8_t> salt {123};
    auto PRK = BLS12_381_KeyGen::HKDF_Extract(salt, IKM);
    auto info = BLS12_381_KeyGen::I2OSP(L, 2);
    auto act = BLS12_381_KeyGen::HKDF_Expand<L>(PRK, info);
    BOOST_CHECK(act == exp);
}


BOOST_AUTO_TEST_CASE(test_hkdf_expand_255_times_32)
{
    const auto L = BLS12_381_KeyGen::NumLamportChunks * BLS12_381_KeyGen::DigestSize;
    std::array<uint8_t,L> exp {
        145,10,207,118,73,4,226,97,194,119,105,33,7,139,135,244,247,50,222,213,162,127,243,137,57,153,145,154,177,228,71,216,172,32,40,221,214,152,32,169,8,195,50,165,254,50,80,173,251,126,88,45,180,91,35,255,42,50,123,42,232,217,136,4,231,168,26,241,150,82,233,29,137,214,204,244,125,98,129,225,163,140,72,168,230,47,49,116,190,193,52,2,221,121,67,202,126,214,183,128,189,203,146,177,243,253,166,49,202,130,167,186,61,179,49,3,219,47,203,175,200,24,100,207,214,29,62,166,163,35,129,245,94,55,61,239,78,154,216,97,45,211,151,55,85,199,196,108,179,188,210,232,52,100,164,190,244,70,171,249,37,125,39,56,159,212,104,254,222,132,159,213,137,155,246,104,70,238,212,46,75,79,163,84,193,201,254,49,105,72,8,241,148,136,83,185,84,120,61,37,194,186,72,218,194,62,178,107,143,182,216,179,107,64,201,146,118,181,23,247,207,156,90,37,187,238,157,43,244,171,154,159,8,122,178,176,131,42,44,215,2,35,165,140,201,31,206,208,69,128,95,222,138,207,123,251,25,204,29,94,238,94,156,122,207,170,47,135,220,116,135,205,18,83,84,246,152,119,179,164,38,162,148,227,16,85,24,55,155,6,104,202,114,28,106,22,226,203,10,244,57,147,68,224,114,32,230,228,34,29,215,75,147,100,184,9,84,175,206,234,255,121,110,11,188,4,104,108,147,4,22,56,161,83,2,161,137,50,206,71,201,86,73,119,177,40,76,238,124,37,176,216,5,49,153,195,119,80,9,165,185,122,173,160,204,246,113,106,66,134,99,73,3,233,246,212,79,138,241,56,185,148,42,96,23,123,58,102,188,209,106,31,193,66,37,233,35,142,112,152,224,2,86,98,137,67,193,64,210,124,59,175,244,72,161,210,217,154,101,147,211,152,20,241,139,129,94,113,126,5,193,56,216,86,32,14,169,31,224,99,65,107,24,88,176,245,31,13,141,243,9,6,101,227,166,175,218,117,63,192,161,59,75,157,215,243,168,165,56,131,205,139,148,103,182,129,109,135,19,104,90,28,246,167,66,211,18,186,190,163,97,79,103,108,115,135,40,63,167,250,106,15,112,26,46,186,139,85,222,87,37,200,54,221,78,95,109,181,129,89,184,133,120,63,59,178,55,241,23,126,132,104,194,116,251,167,9,50,31,3,23,29,171,129,39,225,90,41,155,220,18,79,109,218,115,63,48,141,153,62,58,144,63,84,187,64,26,153,55,178,32,204,41,83,174,222,54,36,115,153,59,111,208,5,245,211,67,191,33,110,241,44,4,171,21,4,14,127,84,35,47,246,183,77,104,158,139,110,141,42,250,184,139,187,40,15,92,113,106,147,239,126,150,49,116,162,153,210,237,45,213,92,138,135,207,174,40,225,58,30,165,138,164,138,65,140,135,56,179,121,148,90,147,147,136,159,6,121,229,227,242,11,143,110,240,60,67,121,138,43,231,194,250,125,238,112,156,252,253,187,202,10,33,214,190,253,173,46,232,124,198,132,201,190,53,239,195,115,120,202,72,181,42,86,94,92,189,13,220,2,190,102,146,96,58,253,188,211,90,221,172,31,209,112,70,236,51,131,211,10,51,51,127,240,252,59,21,181,164,147,205,49,126,72,54,64,178,126,209,42,144,182,248,170,119,54,192,74,253,115,183,237,154,227,21,113,192,82,7,234,30,40,12,6,11,221,226,14,217,50,249,167,65,136,165,167,6,122,231,105,209,77,69,215,216,208,92,177,27,181,193,89,19,29,6,199,177,144,119,196,203,203,124,225,235,129,15,144,244,112,38,248,62,108,188,237,45,117,183,164,104,64,92,91,208,88,183,15,44,3,75,236,212,141,145,138,10,136,233,144,20,174,69,109,245,64,148,88,193,50,168,17,186,132,231,206,236,210,249,112,160,85,66,55,52,81,170,14,50,171,53,213,93,127,33,226,102,36,76,1,58,24,129,200,193,205,25,152,52,248,91,72,255,210,80,29,101,203,130,64,50,73,238,38,56,95,56,113,190,42,231,199,135,221,128,245,134,253,183,54,16,146,191,52,193,141,11,143,118,105,37,38,118,182,126,123,76,36,141,252,83,69,50,90,134,168,250,74,152,239,237,251,3,91,55,200,120,244,53,109,207,187,205,230,48,169,113,45,24,151,76,2,20,87,87,251,214,67,185,98,161,17,87,202,122,2,202,59,73,208,56,51,150,37,230,15,216,185,113,2,230,114,14,169,126,43,194,224,183,159,186,155,144,30,0,68,215,21,192,215,132,8,141,251,170,45,134,121,251,156,165,224,222,250,166,120,199,116,17,157,164,202,102,64,167,223,183,208,57,178,41,98,141,161,187,91,156,171,202,68,150,27,233,21,153,17,14,157,111,243,100,246,101,127,254,57,117,220,135,216,165,32,144,84,132,63,151,54,215,25,74,187,231,62,16,2,150,143,63,199,196,52,145,174,236,238,145,152,12,113,63,159,246,185,145,41,141,85,88,246,219,244,185,107,237,197,37,136,152,183,250,96,85,24,226,185,103,241,109,165,43,213,249,42,166,100,189,209,189,248,151,178,189,201,232,157,252,233,95,152,192,185,147,247,113,12,222,196,158,255,118,51,71,124,52,58,61,7,58,102,36,212,131,219,183,195,252,206,106,212,118,1,227,15,225,93,255,83,40,96,26,233,181,157,252,164,69,97,86,66,227,214,83,44,90,168,96,135,119,14,160,88,208,123,236,179,104,8,94,71,201,254,50,127,94,39,229,35,116,229,249,62,24,220,7,130,198,5,187,140,22,0,45,112,183,15,41,179,19,93,172,210,135,201,201,178,0,50,161,134,5,86,168,95,222,188,106,118,129,23,228,245,236,254,137,221,236,151,104,108,124,229,242,248,9,161,161,49,198,188,171,26,47,200,135,204,124,235,209,110,12,14,9,171,144,244,38,232,95,248,222,215,121,58,248,17,14,98,164,218,126,141,167,218,152,227,188,82,164,39,161,49,60,249,4,12,104,127,7,7,13,94,95,215,87,20,123,57,87,29,38,125,124,188,59,202,245,82,235,233,167,12,243,26,118,176,187,132,19,67,190,238,122,24,7,242,76,135,88,90,131,73,50,109,71,129,216,149,226,93,114,135,85,211,213,236,111,11,50,89,30,103,151,226,81,166,211,104,89,210,19,1,139,125,237,180,87,224,213,187,164,30,123,135,254,50,104,105,162,223,81,101,5,57,202,92,49,125,148,110,149,129,65,72,176,189,76,51,212,122,6,11,61,138,60,167,63,70,194,222,188,55,223,198,157,154,125,114,253,146,174,171,252,246,243,155,0,227,49,106,170,23,131,75,248,180,54,167,196,4,109,135,79,188,246,204,107,122,28,26,16,112,168,10,120,17,185,241,117,52,181,225,233,31,81,90,220,174,37,95,117,190,194,0,63,71,122,207,79,230,209,168,190,50,216,54,67,255,97,73,234,243,72,249,38,222,145,37,65,243,71,141,35,201,87,127,59,211,24,121,192,187,64,202,44,173,87,226,151,8,96,82,4,154,28,28,99,134,109,106,188,86,83,134,76,218,122,65,226,47,253,178,209,101,67,72,172,244,102,48,224,207,10,164,50,202,62,104,114,29,139,251,27,210,153,47,83,26,192,12,113,209,252,251,42,30,10,12,186,238,144,92,189,216,151,218,136,66,170,86,18,47,2,25,207,9,220,1,17,43,121,87,143,131,0,242,242,132,196,38,86,236,171,199,242,175,142,66,206,231,91,118,213,70,10,115,78,162,52,65,6,160,118,32,226,2,91,172,28,43,240,169,62,102,250,132,221,147,147,71,122,166,113,16,122,89,72,233,123,80,199,67,127,67,216,191,183,101,254,164,70,47,143,200,113,96,186,229,200,17,9,253,229,9,179,36,75,130,172,47,119,99,128,43,206,225,178,216,63,113,13,167,24,233,205,86,200,18,97,215,167,155,70,217,233,207,232,194,90,175,170,131,31,149,186,91,53,93,151,192,26,159,224,24,27,43,7,79,111,189,29,111,216,22,83,177,157,220,178,129,210,7,0,78,49,232,187,215,171,225,31,218,77,19,244,193,215,42,248,184,186,142,63,184,55,16,140,74,33,171,49,64,46,235,35,72,234,120,199,187,88,48,127,93,48,54,197,125,26,28,19,249,189,127,114,172,65,41,159,234,245,91,249,37,4,103,93,19,24,3,250,189,194,130,72,24,212,8,68,0,4,18,168,199,199,93,134,119,12,172,247,234,185,230,28,46,97,206,114,0,133,111,178,166,111,208,197,133,179,96,184,136,108,106,228,19,2,21,34,53,94,116,53,63,225,215,56,170,200,186,186,131,243,66,142,131,38,127,125,193,158,16,117,190,146,125,114,137,135,147,58,15,7,8,68,128,159,252,97,169,139,22,202,8,6,225,137,179,125,166,65,237,170,59,92,211,53,76,250,224,159,213,173,102,211,202,24,162,52,230,156,200,167,152,221,235,107,231,137,213,177,51,99,85,67,37,210,102,203,133,21,165,80,198,61,148,134,154,66,193,181,231,194,0,76,31,8,212,182,19,36,40,28,192,195,160,74,16,26,198,142,85,25,22,241,17,68,77,201,141,106,222,151,150,58,78,84,100,245,162,39,28,136,13,109,94,208,184,2,15,56,28,19,157,204,121,62,50,115,132,103,249,150,226,103,27,140,239,45,231,236,6,58,140,143,3,94,61,122,235,163,67,69,207,94,220,229,36,150,35,104,48,88,253,153,123,10,72,144,105,143,48,195,47,196,74,27,14,138,98,72,146,155,71,109,204,31,0,224,62,250,191,171,62,248,81,101,132,116,184,51,190,115,62,15,177,177,194,34,199,125,168,197,242,189,234,36,82,206,185,80,51,74,35,170,245,99,107,30,251,1,0,39,198,220,55,199,68,237,156,199,45,149,209,40,219,134,126,211,107,205,190,167,150,107,163,230,74,191,154,199,190,165,173,232,112,202,52,230,13,199,49,89,18,151,209,113,166,209,48,26,216,162,236,167,107,226,124,18,206,171,93,220,190,112,209,47,45,5,103,121,77,192,164,40,108,225,214,147,182,45,198,124,158,72,84,18,218,74,151,74,24,72,185,23,91,155,247,92,171,231,151,181,16,76,214,91,107,71,239,140,219,144,62,207,46,161,168,15,10,0,72,248,158,5,251,71,29,33,9,252,80,220,193,61,237,112,110,37,190,29,82,14,229,110,178,211,134,243,38,221,53,185,236,90,119,46,169,188,49,188,32,244,179,96,36,168,103,9,4,85,52,108,20,120,215,164,241,50,43,13,212,84,113,5,25,190,178,7,156,53,254,41,116,170,45,43,54,89,160,70,247,184,80,248,103,84,182,6,149,206,87,54,226,180,91,194,159,223,204,18,115,64,41,233,92,206,103,229,60,33,242,38,158,229,19,117,3,100,66,153,55,116,113,227,226,56,121,240,170,118,134,204,205,178,162,70,237,171,61,168,91,227,240,69,229,219,205,170,182,205,179,31,59,1,179,228,73,94,64,3,147,124,71,2,249,204,231,58,140,174,136,83,49,84,14,206,252,61,143,121,242,193,114,244,114,81,195,149,59,145,175,34,76,226,155,7,96,100,183,228,107,154,100,119,229,214,130,66,99,46,98,192,94,37,124,191,106,37,12,171,149,238,8,117,165,125,7,193,62,110,233,94,126,224,1,114,99,209,146,60,77,213,34,252,96,92,226,223,201,90,184,189,131,23,31,163,122,97,0,93,148,168,69,194,87,239,216,12,226,34,21,106,70,254,131,107,121,114,36,67,122,155,186,95,112,40,219,84,13,26,26,181,177,128,58,35,216,195,121,255,1,182,151,200,170,135,184,158,107,120,217,59,37,1,66,37,80,167,162,56,221,220,146,101,190,138,103,21,182,91,187,172,226,194,10,93,214,200,169,190,123,134,174,187,130,66,152,20,171,20,165,99,239,194,60,225,55,90,99,190,93,92,171,18,228,3,124,168,237,171,137,113,196,52,188,245,234,208,130,43,202,198,138,254,239,210,60,53,208,255,164,42,10,224,145,94,27,229,74,130,137,216,6,34,201,151,14,218,175,39,36,249,54,184,64,10,180,115,133,47,114,207,35,171,56,35,161,104,213,87,188,5,65,235,57,216,35,225,22,234,169,9,42,216,249,226,130,113,173,122,252,160,65,164,243,123,179,57,103,159,133,66,63,37,186,80,9,224,9,201,218,194,202,78,21,165,33,193,31,28,180,136,209,13,198,132,217,116,35,168,184,178,248,174,161,146,49,207,13,19,154,49,225,37,29,211,212,169,52,0,214,225,121,80,163,226,18,170,33,232,40,74,228,152,8,53,103,216,223,186,12,222,95,102,165,147,125,57,7,199,189,4,71,31,189,13,166,159,181,215,134,100,101,5,95,116,213,4,204,97,38,41,223,216,50,4,175,147,220,184,33,146,122,10,186,195,52,151,171,179,5,131,52,183,51,89,80,172,77,30,226,170,109,33,242,182,207,63,224,87,107,147,200,152,101,125,218,215,207,206,61,58,144,7,162,170,111,100,8,138,175,37,99,108,82,179,89,61,103,165,48,77,210,21,103,166,111,162,202,81,169,238,127,79,77,59,101,220,151,8,252,134,169,40,171,48,8,106,33,205,25,8,240,113,81,165,38,24,221,4,122,148,137,74,146,167,243,30,232,58,5,91,59,50,30,220,184,188,132,100,45,104,37,197,45,118,254,229,68,76,124,224,90,124,18,79,225,10,157,231,29,176,82,189,204,121,34,166,184,40,166,172,240,106,137,110,46,175,7,220,213,237,6,98,178,150,18,38,59,235,109,189,195,190,39,57,130,115,54,231,200,57,5,14,123,124,68,128,186,246,83,86,176,188,183,153,0,51,76,94,83,53,114,13,223,244,53,215,134,111,127,143,44,179,44,184,178,71,44,160,235,233,198,24,154,244,78,99,19,41,42,217,163,216,107,52,203,122,115,201,83,100,161,132,0,2,44,88,32,14,117,58,18,226,40,47,238,74,58,212,2,136,18,104,4,22,171,174,36,99,198,117,212,54,149,205,250,152,61,113,240,116,30,86,57,17,244,199,78,122,251,71,212,126,135,145,162,217,25,246,154,144,58,234,129,149,108,95,65,201,245,39,152,117,161,27,130,125,9,41,164,229,60,129,246,227,78,203,11,11,206,115,219,117,98,149,144,185,161,204,150,251,241,209,148,147,95,204,45,155,49,35,199,100,162,123,37,157,4,206,195,174,208,182,24,49,15,124,134,250,50,148,219,53,0,67,47,63,28,82,30,18,123,52,100,63,190,108,198,45,34,121,227,27,20,62,136,110,119,117,183,81,219,166,70,199,177,42,90,250,2,28,46,185,120,226,84,157,156,215,210,51,68,11,3,33,153,42,101,145,79,162,20,224,131,26,179,255,181,82,99,121,54,187,116,137,53,32,105,55,144,71,8,26,212,3,91,27,206,130,99,46,66,8,65,169,184,236,24,5,152,179,38,175,62,138,34,238,105,227,67,158,244,108,250,185,173,78,241,105,152,148,115,67,135,129,21,120,142,208,174,96,145,13,226,249,80,52,204,43,254,222,235,207,91,170,0,109,136,120,112,240,213,75,18,82,231,15,151,219,109,187,15,54,132,178,63,37,128,37,184,149,244,12,207,53,139,41,178,1,89,222,234,56,55,252,25,133,208,29,10,18,68,38,47,51,145,163,24,210,120,23,203,229,164,243,185,142,9,249,203,196,38,217,122,224,103,189,54,240,228,114,33,119,212,162,14,230,186,146,72,145,234,111,216,173,132,11,39,35,254,48,18,248,242,11,37,168,156,224,171,216,98,107,194,184,130,164,108,234,123,204,188,132,119,159,62,219,12,19,152,31,117,233,51,48,43,75,236,249,39,140,245,145,233,238,239,123,240,77,197,120,149,127,49,212,132,199,203,149,243,234,147,14,153,195,202,164,154,88,51,43,83,1,59,240,88,63,96,89,25,27,122,185,232,224,135,226,16,158,28,169,59,14,179,54,125,232,25,172,90,185,157,172,57,10,153,211,197,138,90,99,24,241,203,35,150,160,160,64,128,242,83,148,130,36,5,193,177,122,119,123,105,144,170,49,180,69,66,95,252,36,35,251,157,39,80,140,6,182,20,168,55,106,99,195,172,101,92,217,104,143,62,164,6,137,232,59,81,228,69,38,165,16,185,124,97,117,113,243,156,122,182,140,56,137,66,72,28,43,79,211,27,238,102,28,6,166,43,123,240,62,160,166,198,141,79,64,142,221,44,34,87,65,73,116,139,145,53,137,39,118,120,192,103,15,89,139,73,142,12,205,128,81,184,105,72,202,231,130,146,230,205,111,65,34,87,13,127,156,176,124,22,186,222,1,95,29,198,78,176,121,6,10,118,199,226,0,23,162,196,99,17,34,26,171,168,55,242,157,206,10,135,244,243,58,178,188,92,198,166,4,88,61,98,195,149,20,10,224,123,126,122,161,148,4,127,137,40,170,159,222,147,112,136,178,186,253,22,238,80,202,199,98,96,205,241,110,158,59,69,252,208,238,17,222,102,38,97,129,85,167,246,54,248,36,56,14,73,18,162,248,42,148,118,31,177,136,92,124,28,15,236,248,207,138,71,11,54,122,212,130,45,155,130,211,251,105,184,189,28,251,251,191,92,148,59,32,42,225,174,112,71,134,58,239,72,105,26,105,245,209,87,57,51,176,151,189,30,221,51,196,246,32,142,243,180,41,213,56,115,36,54,182,167,131,185,140,61,26,62,7,90,101,168,219,243,194,251,16,48,141,75,49,105,113,53,107,225,81,62,194,67,45,42,228,188,95,89,155,89,141,26,195,18,76,185,93,5,226,83,220,173,186,25,249,190,90,227,199,152,193,56,212,181,146,141,26,197,59,86,99,14,120,98,141,141,228,211,24,218,123,172,12,7,56,52,5,177,253,102,216,187,40,54,60,250,254,95,93,216,157,211,27,176,73,81,139,107,178,149,145,29,85,151,104,251,252,91,53,20,213,109,244,156,91,130,227,174,253,144,188,192,47,24,24,38,89,135,65,226,49,169,245,201,119,228,73,72,176,124,91,219,22,182,234,185,188,19,134,162,172,131,126,69,69,65,54,56,85,249,228,234,120,118,236,136,181,52,193,41,36,175,127,76,106,3,57,93,109,202,156,51,235,201,23,26,114,53,211,216,183,76,214,124,183,212,60,164,135,194,65,219,79,173,111,244,194,67,65,216,72,179,248,30,109,62,39,46,225,117,192,255,148,67,241,128,219,20,221,5,6,26,179,58,42,53,215,226,129,109,6,160,17,17,249,188,1,217,21,162,51,134,15,138,64,181,22,159,126,242,22,184,9,52,25,145,235,29,101,5,17,189,146,223,76,83,34,129,157,107,138,33,48,127,2,152,196,150,248,12,0,194,4,221,119,72,9,248,91,201,137,105,41,32,217,74,166,229,195,78,48,47,79,79,91,102,57,37,179,25,157,142,60,109,2,205,83,114,90,61,66,20,112,255,80,146,168,158,62,96,179,62,13,31,62,92,233,229,23,76,95,217,245,58,161,228,49,70,54,33,141,27,78,52,128,18,63,102,233,105,143,72,53,157,16,184,0,188,206,187,117,43,243,220,97,113,121,125,178,90,18,97,64,223,40,135,31,169,31,13,237,95,144,11,176,82,23,213,179,167,105,31,168,189,81,255,16,145,213,37,212,127,214,104,47,204,233,71,225,127,219,41,199,183,176,95,243,204,227,77,145,46,34,189,82,168,180,27,95,224,211,207,111,149,160,29,14,150,8,168,195,143,97,46,228,154,3,81,56,170,48,42,122,125,15,177,81,247,75,71,182,117,129,147,166,117,9,203,238,174,144,76,134,133,126,146,247,137,77,206,193,52,13,76,222,239,190,1,66,121,199,37,162,196,136,59,102,181,104,110,91,163,131,252,71,232,41,213,174,230,5,9,132,23,26,13,239,79,209,161,3,254,110,175,39,244,121,193,37,72,141,45,141,16,116,240,250,187,122,183,208,131,227,226,210,232,131,232,246,133,199,239,136,124,148,245,128,251,187,197,61,175,45,190,104,115,114,85,249,196,246,16,73,35,183,142,201,82,133,139,207,230,196,239,94,98,162,21,203,9,174,130,34,162,165,173,145,40,131,155,7,198,220,56,245,220,61,152,25,160,65,95,2,171,12,106,157,33,60,236,250,105,201,33,206,79,208,68,159,20,69,86,76,178,36,247,53,223,123,126,51,221,26,171,191,208,125,20,163,236,231,244,190,137,189,29,217,35,186,33,129,223,22,179,111,199,69,137,200,211,195,202,210,150,203,164,15,234,70,179,62,208,209,7,187,181,110,240,210,55,200,102,227,67,208,163,234,43,21,194,201,240,215,215,101,255,188,29,67,75,183,49,131,151,183,150,53,238,37,119,123,205,165,125,81,14,150,169,159,94,172,126,20,17,119,77,186,247,91,70,114,209,127,93,25,196,115,73,218,95,250,197,156,224,60,206,50,58,101,138,35,189,52,11,62,156,28,121,28,31,69,190,216,189,151,150,28,3,111,108,130,235,197,192,20,154,26,126,156,129,177,46,21,137,11,160,32,168,133,179,243,203,26,164,37,199,50,111,152,176,28,209,67,110,231,184,190,105,249,143,223,51,123,228,179,142,205,134,174,58,162,148,59,189,94,6,70,61,13,121,94,200,162,108,255,90,180,197,33,83,180,93,91,117,197,125,150,91,169,83,17,253,240,159,212,66,125,50,123,176,232,110,29,68,232,2,12,91,141,21,213,99,192,64,188,45,181,127,102,123,252,185,166,203,25,130,217,69,32,38,245,136,27,51,185,156,238,37,225,25,100,170,92,217,151,59,225,74,197,217,223,249,237,209,142,80,50,0,133,134,245,35,95,166,242,193,7,2,127,25,182,249,131,208,31,96,70,239,107,213,184,236,238,56,14,151,13,254,136,148,29,97,61,204,252,90,20,57,43,143,179,16,3,254,128,152,239,254,162,212,31,91,226,46,47,153,240,146,161,21,244,254,241,131,200,124,168,118,164,254,60,63,22,55,161,230,24,74,47,130,173,66,133,255,133,88,39,197,74,94,182,222,226,39,184,54,172,176,45,190,6,252,131,85,110,100,45,92,61,0,102,155,4,214,50,177,94,105,79,253,203,60,78,48,47,29,228,71,88,81,200,230,129,53,127,171,128,0,157,153,196,254,243,3,230,172,233,58,252,223,246,210,252,241,130,141,74,246,142,132,68,227,218,11,71,22,197,119,178,181,112,140,225,68,107,166,111,179,98,161,167,187,162,49,73,174,120,192,58,216,254,169,73,132,201,240,206,157,183,248,81,139,141,95,183,230,70,244,13,100,234,75,187,206,189,205,123,142,123,21,42,100,148,76,203,201,213,129,159,40,183,149,198,4,203,79,226,117,1,213,202,159,63,102,42,100,163,154,41,201,160,131,169,72,56,44,162,190,11,62,53,96,71,27,242,115,212,119,220,89,36,186,253,75,66,81,102,190,250,252,238,120,144,96,33,207,124,34,212,59,57,56,66,201,86,14,48,151,111,236,135,127,223,61,56,228,44,74,53,135,98,73,93,142,48,205,9,50,216,178,111,185,122,83,4,118,92,70,34,118,74,83,69,7,145,148,106,210,129,207,77,124,82,114,87,250,207,157,134,122,97,137,132,240,50,152,42,54,138,222,122,105,227,77,107,28,46,166,12,170,103,29,93,75,122,152,176,232,190,194,188,47,168,88,119,247,80,230,0,190,171,57,78,62,83,195,130,203,172,21,218,40,34,163,153,9,207,165,96,188,153,93,188,17,128,199,203,229,153,220,139,51,220,228,89,168,164,76,215,44,153,22,98,210,251,102,182,16,34,230,239,44,84,126,181,146,210,135,132,9,131,56,195,179,201,235,179,158,3,205,212,112,158,181,116,189,68,222,152,120,56,107,207,30,61,28,90,95,137,185,78,45,16,72,137,164,48,72,129,49,165,111,2,169,75,150,226,16,241,6,27,170,75,142,162,191,225,89,102,120,47,105,242,24,22,169,23,212,61,67,143,99,200,216,85,11,179,97,171,214,140,178,167,62,132,218,157,190,3,164,243,221,58,39,34,101,51,61,160,175,47,109,178,90,55,78,211,158,92,231,112,28,227,248,250,62,61,44,152,108,91,59,13,9,141,153,55,102,231,137,249,56,5,177,11,138,226,182,78,238,203,102,4,236,14,102,244,134,186,243,136,95,164,226,209,24,59,22,229,222,181,79,122,148,166,199,153,170,175,32,243,62,28,92,104,127,78,75,176,60,223,21,72,27,244,222,71,161,148,85,9,241,139,164,162,245,149,97,15,172,86,157,229,157,196,32,17,150,250,234,247,151,102,204,132,1,159,22,72,169,72,151,18,24,232,178,150,102,173,173,160,207,252,227,156,180,51,122,2,188,61,184,48,154,61,221,181,63,42,88,41,107,66,147,119,23,114,190,182,41,72,48,118,228,205,112,2,65,179,157,178,254,191,44,214,29,100,225,172,158,77,51,171,175,224,189,91,231,14,118,77,1,227,41,49,192,176,160,152,71,118,85,144,53,199,54,229,215,34,121,60,211,231,45,236,202,13,223,162,224,193,184,8,117,21,205,254,43,3,136,196,86,100,215,176,83,155,74,42,94,41,163,21,186,147,173,166,191,105,43,178,121,201,152,122,164,148,17,179,78,20,54,59,2,165,8,162,93,133,148,136,205,185,44,218,193,131,157,23,146,146,153,65,92,18,189,176,219,15,92,216,57,136,17,151,85,153,193,252,3,215,40,160,93,168,225,226,252,211,210,79,94,12,208,173,178,91,111,230,64,145,174,6,203,83,29,140,6,133,168,173,220,123,35,208,241,60,161,124,121,210,116,123,3,110,18,180,31,168,168,133,59,59,144,83,164,90,107,148,129,18,141,221,230,91,202,52,215,107,163,211,217,50,242,229,81,25,100,97,156,50,135,113,217,193,230,155,81,42,66,179,77,16,156,245,11,213,196,20,98,77,67,86,153,248,52,158,250,95,198,195,179,83,184,149,90,119,206,29,63,55,51,105,243,8,86,167,193,239,164,185,123,237,91,204,159,153,112,98,106,182,135,198,88,11,90,228,204,104,41,36,109,126,14,41,244,41,170,233,101,165,173,93,183,26,125,54,203,155,134,135,166,192,144,87,81,121,159,50,92,129,187,41,62,245,84,76,173,18,125,165,184,15,82,42,13,192,11,52,175,128,213,204,67,190,221,74,36,145,212,169,228,203,166,243,251,155,61,207,240,60,253,132,24,154,141,92,132,87,196,126,210,252,29,63,90,113,52,21,117,238,85,241,23,77,239,192,38,125,236,81,53,62,212,153,78,112,32,231,250,55,89,142,159,169,80,16,15,96,131,199,152,148,86,44,46,201,71,147,211,67,191,219,173,201,222,195,203,221,147,208,91,237,101,225,196,252,113,196,149,202,114,197,112,11,13,124,111,159,100,0,159,230,38,198,177,17,1,33,166,170,95,128,202,126,25,131,110,165,36,159,133,13,42,220,67,126,111,137,140,18,68,140,155,254,155,113,7,254,250,190,224,220,221,16,212,89,100,99,209,121,249,144,47,68,76,211,200,107,214,63,146,36,192,97,34,87,103,17,255,102,114,230,242,116,70,68,109,207,244,167,210,3,115,148,62,113,221,117,129,147,70,195,98,153,205,216,227,176,186,147,95,78,55,19,251,41,116,160,201,214,218,191,201,53,239,128,116,6,41,156,45,118,196,218,165,235,36,21,109,25,182,120,238,29,98,130,84,180,201,56,180,238,53,90,196,237,176,119,166,35,70,56,69,184,79,242,152,214,164,36,118,250,211,159,234,56,202,72,87,231,112,13,236,76,79,106,114,252,26,60,181,230,22,81,103,228,174,185,237,224,144,222,124,60,107,154,242,14,39,133,228,50,46,74,122,242,177,180,84,132,186,65,9,247,49,183,73,215,70,111,187,248,21,119,112,154,198,188,138,131,212,37,141,25,196,206,7,178,92,5,92,55,100,49,163,15,198,77,40,188,153,76,1,247,140,209,33,83,123,3,59,194,35,130,46,173,78,66,148,78,137,231,143,2,99,14,113,167,187,11,189,97,102,172,252,209,149,111,215,255,37,91,225,112,129,132,208,243,215,236,173,170,113,176,149,20,240,143,85,64,163,40,137,98,62,211,25,118,201,194,119,250,211,221,153,250,234,164,174,129,153,84,141,68,36,184,71,11,8,160,212,135,117,89,249,30,184,3,49,172,121,86,193,135,34,233,133,176,30,246,78,25,13,183,17,205,96,213,139,122,143,252,225,231,69,91,120,25,53,192,161,36,123,59,213,210,249,106,237,202,184,82,83,34,85,12,82,120,232,41,8,115,131,126,87,147,48,254,14,191,255,14,243,131,149,36,33,152,181,95,180,138,97,222,156,249,213,49,71,84,239,109,42,178,251,214,17,117,160,136,255,10,141,167,180,146,150,247,14,21,14,52,86,223,191,23,11,79,51,89,242,76,106,40,214,137,252,242,188,17,53,19,209,203,190,147,95,158,121,22,246,90,228,117,148,77,138,80,241,34,43,163,11,26,218,247,116,225,251,56,28,106,23,229,67,78,156,210,100,215,19,126,213,195,117,109,143,119,106,219,206,240,216,146,105,195,252,7,19,180,124,144,78,112,90,28,109,17,15,15,177,68,57,209,9,117,148,165,251,184,52,146,79,81,37,190,197,49,154,127,224,248,49,154,66,21,39,201,27,108,149,148,198,51,115,42,142,195,76,59,5,149,87,23,221,198,82,72,160,165,14,237,4,220,32,41,115,72,230,38,157,153,209,177,252,134,110,148,157,180,23,85,143,58,62,87,29,58,70,231,202,83,17,148,146,64,162,240,155,54,146,221,218,230,190,53,40,252,245,67,137,137,24,183,81,126,130,98,124,203,230,0,143,225,10,178,22,138,163,54,71,77,142,221,104,232,129,85,157,179,211,218,239,132,190,204,12,61,17,157,82,236,43,250,225,253,129,253,217,245,156,110,87,180,6,192,210,26,214,65,198,104,177,108,147,154,244,242,190,27,4,139,101,30,19,130,58,188,174,28,153,52,51,168,110,15,82,78,155,119,101,54,172,141,196,115,101,44,221,232,34,146,83,115,244,41,240,233,246,46,91,226,55,208,10,200,24,68,244,8,99,202,243,178,62,83,189,71,159,149,83,113,255,60,208,119,0,2,159,6,253,8,23,36,17,72,139,4,154,2,167,186,11,65,129,158,245,247,182,47,188,15,222,39,108,190,219,53,68,44,42,135,47,224,213,24,132,46,114,203,254,39,75,157,141,196,22,24,255,142,179,151,125,130,198,222,240,252,44,235,90,71,202,115,9,43,209,15,187,90,129,20,110,84,101,146,105,23,192,190,177,76,59,19,250,234,44,111,163,48,15,110,19,60,5,31,30,184,102,98,244,117,74,66,186,31,123,229,52,255,248,9,90,97,83,68,56,122,251,65,37,188,164,24,197,4,66,205,181,189,225,74,219,167,192,231,40,247,2,15,100,253,179,4,112,155,196,225,224,0,4,235,62,234,98,216,35,66,186,230,33,94,111,111,213,60,160,175,99,223,147,111,188,114,84,119,68,29,56,117,176,149,159,35,54,63,75,75,2,77,64,220,13,152,251,205,192,217,207,186,14,202,113,30,228,104,156,105,218,175,118,128,79,69,247,43,29,77,129,1,170,179,204,162,153,243,223,195,42,249,188,106,158,97,247,233,131,47,137,203,45,155,165,55,64,65,92,235,87,75,127,66,29,177,211,50,133,20,75,193,161,146,151,94,134,161,19,9,118,126,60,15,152,207,141,113,195,213,7,37,44,49,0,204,145,173,106,71,101,16,117,128,14,92,146,26,216,242,129,105,193,23,164,228,130,130,248,28,100,253,168,244,71,179,100,102,139,176,195,203,139,16,160,7,40,116,155,200,197,243,137,212,124,149,114,92,167,49,73,10,210,75,121,218,57,252,14,203,187,151,203,131,83,130,21,233,148,115,235,120,184,139,185,252,91,104,253,211,61,243,115,7,62,174,60,223,5,227,77,4,100,47,3,57,66,111,75,184,188,193,92,177,101,103,195,195,43,26,120,66,176,112,54,40,243,41,170,213,83,156,156,25,143,128,125,237,161,221,86,182,210,135,213,44,201,188,104,107,219,213,249,147,26,250,121,29,110,182,186,47,147,108,244,101,88,134,251,98,189,87,154,42,100,14,246,44,167,112,67,25,196,197,149,69,74,242,227,197,165,211,225,82,87,162,67,237,88,28,95,183,130,143,170,208,255,31,68,22,139,133,119,79,176,225,214,55,6,252,127,136,112,12,30,98,221,172,143,56,5,247,62,157,216,85,179,178,169,72,5,147,78,42,77,8,2,37,130,252,39,181,158,144,157,183,53,169,249,249,251,184,106,112,242,7,236,111,34,148,167,123,80,152,169,11,185,240,137,86,147,110,249,132,107,159,1,106,191,39,162,30,75,192,98,62,90,55,96,4,53,124,47,203,208,127,92,13,129,241,240,132,46,70,27,166,128,151,246,14,150,92,171,172,246,40,247,179,108,167,0,20,57,84,105,32,49,202,95,188,70,111,21,231,121,228,244,249,211,113,161,5,186,223,252,37,203,13,5,153,100,169,249,182,168,185,239,84,73,201,222,248,195,121,25,115,180,242,37,53,224,198,189,94,183,145,215,203,111,185,113,58,52,217,39,86,124,13,252,50,55,52,143,248,190,70,240,118,51,10,255,190,187,149,63,207,149,63,25,0,17,12,127,106,80,87,209,175,249,185,121,37,151,99,15,244,127,231,13,129,177,162,11,9,131,37,123,165,101,22,91,223,123,117,233,152,128,178,25,183,130,108,189,223,22,112,200,101,241,34,44,186,128,30,70,165,161,254,126,130,203,143,164,156,89,190,46,31,107,65,103,39,225,105,24,242,201,184,243,170,55,217,34,173,167,124,194,186,49,191,81,15,77,223,63,71,48,236,155,26,239,121,235,182,126,87,238,164,100,158,187,188,64,102,76,144,32,170,143,5,14,252,52,200,23,172,235,11,145,90,134,158,21,206,8,186,142,133,122,153,180,195,231,115,118,151,189,107,101,31,222,234,219,62,164,141,61,255,119,60,252,166,193,110,27,92,21,27,138,244,45,30,103,176,87,135,254,141,38,66,247,21,138,85,146,177,219,172,212,76,2,152,207,103,231,3,83,233,156,81,130,225,106,185,58,136,43,149,63,79,113,67,46,39,19,60,238,147,124,3,51,208,159,252,192,138,134,126,228,242,56,134,104,253,113,57,130,119,134,23,141,250,65,191,72,116,181,150,6,114,193,253,153,105,49,238,172,205,97,199,189,151,230,3,26,138,148,20,40,88,101,30,219,173,250,81,108,178,77,52,8,68,218,134,41,100,120,156,35,54,101,146,60,16,56,62,69,209,3,133,1,232,145,139,182,205,155,135,245,210,192,80,9,18,49,125,90,115,82,208,47,184,161,181,142,95,95,202,242,16,240,160,158,47,61,54,228,247,37,161,144,84,150,21,117,80,45,118,250,227,3,162,84,190,252,111,140,2,251,221,2,15,255,103,27,29,117,189,16,143,187,47,75,78,203,204,36,94,244,55,213,200,69,128,241,186,108,151,5,235,48,121,64,96,174,4,200,13,211,184,108,142,242,251,195,254,146,89,169,171,151,20,225,148,129,230,151,58,32,83,145,107,251,72,45,77,47,20,116,10,245,215,78,32,213,229,152,232,118,221,144,54,166,166,28,88,71,78,202,61,5,135,107,152,211,26,241,213,119,79,64,50,102,99,54,24,27,206,254,80,60,117,231,210,32,235,187,16,33,93,77,47,218,215,202,0,51,106,127,195,142,95,143,108,136,83,62,168,241,176,70,79,199,39,19,230,156,89,58,93,56,75,143,226,123,86,63,81,116,107,244,13,98,49,159,119,193,166,49,12,101,203,81,110,52,121,114,39,181,178,38,171,236,4,129,106,2,255,218,38,199,122,80,11,29,184,84,185,229,65,238,73,2,213,180,15,201,58,189,200,217,94,123,119,95,253,214,217,91,35,33,62,253,192,32,249,181,60,255,109,37,204,159,163,162,203,250,189,222,243,199,208,169,180,134,12,72,115,127,170,22,104,215,19,7,176,110,41,225,135,91,236,162,234,165,137,71,115,119,45,96,57,64,225,185,16,164,201,249,
    };

    std::vector<uint8_t> IKM { 0x01, 0x02 };
    std::vector<uint8_t> salt {123};
    auto PRK = BLS12_381_KeyGen::HKDF_Extract(salt, IKM);
    std::vector<uint8_t> info;
    auto act = BLS12_381_KeyGen::HKDF_Expand<L>(PRK, info);
    BOOST_CHECK(act == exp);
}

BOOST_AUTO_TEST_CASE(test_flip_bits)
{
    std::vector<uint8_t> input {
        13, 115,  89, 213, 121,  99, 171, 143,
        187, 222,  24,  82, 220, 245,  83, 254,
        219, 195,  31,  70,  77, 128, 238, 125,
        64, 174, 104,  49,  34, 180,  80, 112,
    };
    std::vector<uint8_t> exp {
        242, 140, 166,  42, 134, 156,  84, 112,
        68,  33, 231, 173,  35,  10, 172,   1,
        36,  60, 224, 185, 178, 127,  17, 130,
        191,  81, 151, 206, 221,  75, 175, 143
    };
    auto act = BLS12_381_KeyGen::flip_bits(input);
    BOOST_CHECK(act == exp);
}

void RunTestCase(
    const std::string& seed_str,
    const std::string& exp_master_str,
    const uint32_t& child_index,
    const std::string& exp_child_str
) {
    auto seed = ParseHex(seed_str);
    auto act_master = BLS12_381_KeyGen::derive_master_SK(seed);
    MclScalar exp_master(exp_master_str, 10);
    BOOST_CHECK(act_master == exp_master);

    MclScalar exp_child(exp_child_str, 10);
    auto act_child = BLS12_381_KeyGen::derive_child_SK(act_master, child_index);
    BOOST_CHECK(act_child == exp_child);

}

BOOST_AUTO_TEST_CASE(test_test_case_0)
{
    RunTestCase(
        "c55257c360c07c72029aebc1b53c05ed0362ada38ead3e3e9efa3708e53495531f09a6987599d18264c1e1c92f2cf141630c7a3c4ab7c81b2f001698e7463b04",
        "6083874454709270928345386274498605044986640685124978867557563392430687146096",
        0,
        "20397789859736650942317412262472558107875392172444076792671091975210932703118"
    );
}

BOOST_AUTO_TEST_CASE(test_test_case_1)
{
    RunTestCase(
        "3141592653589793238462643383279502884197169399375105820974944592",
        "29757020647961307431480504535336562678282505419141012933316116377660817309383",
        3141592653,
        "25457201688850691947727629385191704516744796114925897962676248250929345014287"
    );
}

BOOST_AUTO_TEST_CASE(test_test_case_2)
{
    RunTestCase(
        "0099FF991111002299DD7744EE3355BBDD8844115566CC55663355668888CC00",
        "27580842291869792442942448775674722299803720648445448686099262467207037398656",
        4294967295,
        "29358610794459428860402234341874281240803786294062035874021252734817515685787"
    );
}

BOOST_AUTO_TEST_CASE(test_test_case_3)
{
    RunTestCase(
        "d4e56740f876aef8c010b86a40d5f56745a118d0906a34e69aec8c0db1cb8fa3",
        "19022158461524446591288038168518313374041767046816487870552872741050760015818",
        42,
        "31372231650479070279774297061823572166496564838472787488249775572789064611981"
    );
}

BOOST_AUTO_TEST_CASE(test_test_vector_with_intermediate_values)
{
    // not testing for the intermediate values - lamport0, lamport1 and compressed_lamport_PK
    RunTestCase(
        "c55257c360c07c72029aebc1b53c05ed0362ada38ead3e3e9efa3708e53495531f09a6987599d18264c1e1c92f2cf141630c7a3c4ab7c81b2f001698e7463b04",
        "6083874454709270928345386274498605044986640685124978867557563392430687146096",
        0,
        "20397789859736650942317412262472558107875392172444076792671091975210932703118"
    );
}

BOOST_AUTO_TEST_SUITE_END()
