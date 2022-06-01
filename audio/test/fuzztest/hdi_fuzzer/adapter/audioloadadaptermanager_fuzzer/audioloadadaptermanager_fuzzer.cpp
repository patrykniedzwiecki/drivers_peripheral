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

#include "audioloadadaptermanager_fuzzer.h"
#include "audio_hdi_fuzzer_common.h"

using namespace OHOS::Audio;
namespace OHOS {
namespace Audio {
bool AudioLoadadapterManagerFuzzTest(const uint8_t *data, size_t size)
{
    bool result = false;
    TestAudioManager *manager = nullptr;
    int32_t ret = GetManager(manager);
    if (ret < 0 || manager == nullptr) {
        return false;
    }
    struct AudioAdapterDescriptor *descs = nullptr;
    int sizeone = 0;
    ret = manager->GetAllAdapters(manager, &descs, &sizeone);
    if (ret < 0 || manager == nullptr) {
        return false;
    }
    struct AudioAdapterDescriptor *desc = &descs[0];
    struct AudioAdapter *adapter = nullptr;
    TestAudioManager *managerFuzz = (TestAudioManager *)data;
    ret = manager->LoadAdapter(managerFuzz, desc, &adapter);
    if (ret == HDF_SUCCESS) {
        result = true;
    }
    return result;
}
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::Audio::AudioLoadadapterManagerFuzzTest(data, size);
    return 0;
}