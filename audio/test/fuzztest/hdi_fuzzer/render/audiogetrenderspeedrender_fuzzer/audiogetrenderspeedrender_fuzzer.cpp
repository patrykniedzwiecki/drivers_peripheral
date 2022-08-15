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

#include "audiogetrenderspeedrender_fuzzer.h"
#include "audio_hdi_fuzzer_common.h"

using namespace OHOS::Audio;
namespace OHOS {
namespace Audio {
bool AudioGetrenderspeedRenderFuzzTest(const uint8_t *data, size_t size)
{
    bool result = false;
    TestAudioManager *getRenSpeedFuzzManager = nullptr;
    struct AudioAdapter *getRenSpeedFuzzAdapter = nullptr;
    struct AudioRender *getRenSpeedFuzzRender = nullptr;
    int32_t ret = AudioGetManagerCreateStartRender(getRenSpeedFuzzManager,
        &getRenSpeedFuzzAdapter, &getRenSpeedFuzzRender);
    if (ret < 0 || getRenSpeedFuzzAdapter == nullptr ||
        getRenSpeedFuzzRender == nullptr || getRenSpeedFuzzManager == nullptr) {
        HDF_LOGE("%{public}s: AudioGetManagerCreateStartRender failed \n", __func__);
        return false;
    }
    float speed = 0;

    struct AudioRender *renderFuzz = (struct AudioRender *)data;
    ret = getRenSpeedFuzzRender->GetRenderSpeed(renderFuzz, &speed);
    if (ret == HDF_SUCCESS) {
        result = true;
    }
    getRenSpeedFuzzRender->control.Stop((AudioHandle)getRenSpeedFuzzRender);
    getRenSpeedFuzzAdapter->DestroyRender(getRenSpeedFuzzAdapter, getRenSpeedFuzzRender);
    getRenSpeedFuzzManager->UnloadAdapter(getRenSpeedFuzzManager, getRenSpeedFuzzAdapter);
    getRenSpeedFuzzRender = nullptr;
    return result;
}
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::Audio::AudioGetrenderspeedRenderFuzzTest(data, size);
    return 0;
}
