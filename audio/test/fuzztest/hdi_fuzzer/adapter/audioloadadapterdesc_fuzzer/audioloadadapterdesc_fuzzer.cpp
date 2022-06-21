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

#include "audioloadadapterdesc_fuzzer.h"
#include "securec.h"
#include "audio_hdi_fuzzer_common.h"

using namespace OHOS::Audio;
namespace OHOS {
namespace Audio {
bool AudioLoadadapterDescFuzzTest(const uint8_t *data, size_t size)
{
    bool result = false;
    TestAudioManager *manager = nullptr;
    int32_t ret = GetManager(manager);
    if (ret < 0 || manager == nullptr) {
        HDF_LOGE("%{public}s: GetManager failed \n", __func__);
        return false;
    }

    struct AudioPort port = {};
    if (memcpy_s((void *)&port, sizeof(port), data, sizeof(port)) != 0) {
        return false;
    }

    struct AudioAdapterDescriptor descFuzz = {};
    if (memcpy_s((void *)&descFuzz, sizeof(descFuzz), data, sizeof(descFuzz)) != 0) {
        return false;
    }
    descFuzz.ports = &port;
    struct AudioAdapter *adapter = nullptr;
    ret = manager->LoadAdapter(manager, &descFuzz, &adapter);
    if (ret == HDF_SUCCESS) {
        result = true;
    }
    return result;
}
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::Audio::AudioLoadadapterDescFuzzTest(data, size);
    return 0;
}