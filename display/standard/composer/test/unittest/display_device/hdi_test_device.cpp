/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_test_device.h"
#include <mutex>
#include "v1_0/include/idisplay_buffer.h"
#include "v1_0/display_composer_type.h"
#include "hdi_test_device_common.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace TEST {

using namespace OHOS::HDI::Display::Buffer::V1_0;
HdiTestDevice &HdiTestDevice::GetInstance()
{
    static HdiTestDevice device;
    return device;
}

void HdiTestDevice::HotPlug(uint32_t outputId, bool connected, void *data)
{
    DISPLAY_TEST_LOGD("outputId %d connected %d", outputId, connected);
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((data == nullptr), DISPLAY_TEST_LOGE("the data is null ptr"));
    HdiTestDevice *device = static_cast<HdiTestDevice *>(data);
    if (connected) {
        device->FindDisplayOrCreate(outputId);
    }
    DISPLAY_TEST_LOGD("end");
}

int32_t HdiTestDevice::InitDevice()
{
    int ret = DISPLAY_SUCCESS;
    displayDevice_.reset(IDisplayComposerInterface::Get());
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("DeviceInitialize Failed"));
    DISPLAY_TEST_CHK_RETURN((displayDevice_ == nullptr), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("device funcs is null"));

    gralloc_.reset(IDisplayBuffer::Get());

    displayDevice_->RegHotPlugCallback(HotPlug, static_cast<void *>(this));
    return DISPLAY_SUCCESS;
}

std::shared_ptr<HdiTestDisplay> HdiTestDevice::GetDisplayFromId(uint32_t id)
{
    auto iter = mDisplays.find(id);
    DISPLAY_TEST_CHK_RETURN((iter == mDisplays.end()), nullptr, DISPLAY_TEST_LOGD("can not find the display %d", id));
    return mDisplays[id];
}

std::shared_ptr<HdiTestDisplay> HdiTestDevice::FindDisplayOrCreate(uint32_t id)
{
    int ret;
    std::shared_ptr<HdiTestDisplay> display = GetDisplayFromId(id);
    if (display == nullptr) {
        DISPLAY_TEST_LOGD("the display not find will creat a display");
    }
    display = std::make_shared<HdiTestDisplay>(id, displayDevice_);
    ret = display->Init();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), nullptr, DISPLAY_TEST_LOGE("can not init the display"));
    mDisplays.emplace(id, display);
    return display;
}

std::shared_ptr<HdiTestDisplay> HdiTestDevice::GetFirstDisplay()
{
    DISPLAY_TEST_CHK_RETURN((mDisplays.begin() == mDisplays.end()), nullptr,
        DISPLAY_TEST_LOGE("the mDisplays is empty"));
    return mDisplays.begin()->second;
}

void HdiTestDevice::Clear()
{
    for (auto const & iter : mDisplays) {
        iter.second->Clear();
    }
}
} // OHOS
} // HDI
} // Display
} // TEST
