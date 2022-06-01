/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <gtest/gtest.h>
#include <securec.h>
#include "hdf_base.h"
#include "osal_time.h"
#include "v1_0/imotion_interface.h"
#include "motion_callback_service.h"

using namespace OHOS::HDI::Motion::V1_0;
using namespace testing::ext;

namespace {
    sptr<IMotionInterface>  g_motionInterface = nullptr;
    sptr<IMotionCallback> g_motionCallback = new MotionCallbackService();
}

class HdfMotionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfMotionTest::SetUpTestCase()
{
    g_motionInterface = IMotionInterface::Get(true);
}

void HdfMotionTest::TearDownTestCase()
{
}

void HdfMotionTest::SetUp()
{
}

void HdfMotionTest::TearDown()
{
}

HWTEST_F(HdfMotionTest, GetMotionClient_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, g_motionInterface);
}

HWTEST_F(HdfMotionTest, RegisterMotionDataCb_001, TestSize.Level1)
{
    if (g_motionInterface == nullptr) {
        ASSERT_NE(nullptr, g_motionInterface);
        return;
    }
    int32_t ret = g_motionInterface->Register(g_motionCallback);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = g_motionInterface->Unregister(g_motionCallback);
    EXPECT_EQ(0, ret);
}

HWTEST_F(HdfMotionTest, EnableMotion_001, TestSize.Level1)
{
    if (g_motionInterface == nullptr) {
        ASSERT_NE(nullptr, g_motionInterface);
        return;
    }
    int32_t ret = g_motionInterface->EnableMotion(HDF_MOTION_TYPE_PICKUP);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

HWTEST_F(HdfMotionTest, DisableMotion_001, TestSize.Level1)
{
    if (g_motionInterface == nullptr) {
        ASSERT_NE(nullptr, g_motionInterface);
        return;
    }
    int32_t ret = g_motionInterface->DisableMotion(HDF_MOTION_TYPE_PICKUP);
    EXPECT_EQ(HDF_SUCCESS, ret);
}