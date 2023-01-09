/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "adaptor_algorithm_test.h"

#include "adaptor_algorithm.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
using namespace testing;
using namespace testing::ext;

void AdaptorAlgorithmTest::SetUpTestCase()
{
}

void AdaptorAlgorithmTest::TearDownTestCase()
{
}

void AdaptorAlgorithmTest::SetUp()
{
}

void AdaptorAlgorithmTest::TearDown()
{
}

/**
 * @tc.name: KeyPair test
 * @tc.desc: verify KeyPair
 * @tc.type: FUNC
 * @tc.require: #I64XCB
 */
HWTEST_F(AdaptorAlgorithmTest, KeyPair_Test, TestSize.Level0)
{
    DestoryKeyPair(nullptr);

    std::vector<uint8_t> dataTest(32, 1);
    Buffer *data = CreateBufferByData(&dataTest[0], 32);
    EXPECT_NE(data, nullptr);
    KeyPair keyPair2 = {};
    keyPair2.pubKey = nullptr;
    keyPair2.priKey = CopyBuffer(data);
    EXPECT_NE(keyPair2.priKey, nullptr);
    bool result = IsEd25519KeyPairValid(&keyPair2);
    EXPECT_EQ(result, false);
    DestoryBuffer(keyPair2.priKey);

    KeyPair keyPair3 = {};
    keyPair3.priKey = nullptr;
    keyPair3.pubKey = CopyBuffer(data);
    EXPECT_NE(keyPair3.pubKey, nullptr);
    result = IsEd25519KeyPairValid(&keyPair3);
    EXPECT_EQ(result, false);
    DestoryBuffer(keyPair3.pubKey);
    DestoryBuffer(data);

    KeyPair *keyPair4 = GenerateEd25519KeyPair();
    EXPECT_NE(keyPair4, nullptr);
    result = IsEd25519KeyPairValid(keyPair4);
    EXPECT_EQ(result, true);
    DestoryKeyPair(keyPair4);
}

/**
 * @tc.name: Ed25519Sign test
 * @tc.desc: sign Ed25519
 * @tc.type: FUNC
 * @tc.require: #I64XCB
 */
HWTEST_F(AdaptorAlgorithmTest, Ed25519Sign_Test, TestSize.Level0)
{
    constexpr uint32_t keyLen = 32;
    std::vector<uint8_t> dataTest(keyLen, 1);
    Buffer *data = CreateBufferByData(&dataTest[0], keyLen);
    EXPECT_NE(data, nullptr);
    Buffer *signContent = nullptr;
    KeyPair *keyPair = GenerateEd25519KeyPair();
    EXPECT_NE(keyPair, nullptr);

    int32_t result = Ed25519Sign(nullptr, data, &signContent);
    EXPECT_EQ(result, RESULT_BAD_PARAM);

    result = Ed25519Sign(keyPair, nullptr, &signContent);
    EXPECT_EQ(result, RESULT_BAD_PARAM);

    result = Ed25519Sign(keyPair, data, nullptr);
    EXPECT_EQ(result, RESULT_BAD_PARAM);

    result = Ed25519Sign(keyPair, data, &signContent);
    EXPECT_EQ(result, RESULT_SUCCESS);

    DestoryBuffer(signContent);
    DestoryKeyPair(keyPair);
    DestoryBuffer(data);
}

/**
 * @tc.name: Ed25519Verify test
 * @tc.desc: verify Ed25519
 * @tc.type: FUNC
 * @tc.require: #I64XCB
 */
HWTEST_F(AdaptorAlgorithmTest, Ed25519Verify_Test, TestSize.Level0)
{
    constexpr uint32_t constLen = 64;
    std::vector<uint8_t> dataTest(constLen, 1);
    Buffer *data = CreateBufferByData(&dataTest[0], constLen);
    KeyPair *keyPair = GenerateEd25519KeyPair();
    EXPECT_NE(keyPair, nullptr);
    Buffer *signContent = nullptr;

    int32_t result = Ed25519Sign(keyPair, data, &signContent);
    EXPECT_EQ(result, RESULT_SUCCESS);

    result = Ed25519Verify(nullptr, data, signContent);
    EXPECT_EQ(result, RESULT_BAD_PARAM);

    result = Ed25519Verify(keyPair->pubKey, nullptr, signContent);
    EXPECT_EQ(result, RESULT_BAD_PARAM);

    result = Ed25519Verify(keyPair->pubKey, data, nullptr);
    EXPECT_EQ(result, RESULT_BAD_PARAM);

    result = Ed25519Verify(keyPair->priKey, data, signContent);
    EXPECT_EQ(result, RESULT_GENERAL_ERROR);

    result = Ed25519Verify(keyPair->pubKey, data, signContent);
    EXPECT_EQ(result, RESULT_SUCCESS);

    DestoryBuffer(signContent);
    DestoryKeyPair(keyPair);
    DestoryBuffer(data);
}

HWTEST_F(AdaptorAlgorithmTest, TestHmacSha256, TestSize.Level0)
{
    Buffer *hmacKey = nullptr;
    Buffer *data = nullptr;
    Buffer **hmac = nullptr;
    EXPECT_EQ(HmacSha256(hmacKey, data, hmac), RESULT_BAD_PARAM);
    Buffer *temp = nullptr;
    hmac = &temp;
    EXPECT_EQ(HmacSha256(hmacKey, data, hmac), RESULT_GENERAL_ERROR);
}

HWTEST_F(AdaptorAlgorithmTest, TestSecureRandom, TestSize.Level0)
{
    uint8_t *buffer = nullptr;
    EXPECT_EQ(SecureRandom(buffer, 10), RESULT_BAD_PARAM);
    uint8_t num = 0;
    buffer = &num;
    EXPECT_EQ(SecureRandom(buffer, 1), RESULT_SUCCESS);
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS