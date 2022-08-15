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

#include "audiogetframecountrender_fuzzer.h"
#include "audio_hdi_fuzzer_common.h"

using namespace OHOS::Audio;
namespace OHOS {
namespace Audio {
    bool AudioGetFrameCountRenderFuzzTest(const uint8_t *data, size_t size)
    {
        bool result = false;
        uint64_t count = 0;
        TestAudioManager *FreamRenderManager = nullptr;
        struct AudioAdapter *FreamRenderAdapter = nullptr;
        struct AudioRender *FreamRender = nullptr;
        int32_t ret = AudioGetManagerCreateRender(FreamRenderManager, &FreamRenderAdapter, &FreamRender);
        if (ret < 0 || FreamRenderAdapter == nullptr || FreamRender == nullptr || FreamRenderManager == nullptr) {
            HDF_LOGE("%{public}s: AudioGetManagerCreateRender failed \n", __func__);
            return false;
        }

        struct AudioRender *handle = (struct AudioRender *)data;
        ret = FreamRender->attr.GetFrameCount(handle, &count);
        if (ret == HDF_SUCCESS) {
            result = true;
        }
        FreamRenderAdapter->DestroyRender(FreamRenderAdapter, FreamRender);
        FreamRenderManager->UnloadAdapter(FreamRenderManager, FreamRenderAdapter);
        return result;
    }
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::Audio::AudioGetFrameCountRenderFuzzTest(data, size);
    return 0;
}