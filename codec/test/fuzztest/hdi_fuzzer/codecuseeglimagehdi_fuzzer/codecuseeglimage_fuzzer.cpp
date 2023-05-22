/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "codecuseeglimage_fuzzer.h"
#include "codeccommon_fuzzer.h"

#include <securec.h>

namespace {
    struct AllParameters {
        uint32_t portIndex;
        int8_t *eglImage;
        uint32_t eglImageLen;
    };
}

namespace OHOS {
namespace Codec {
    bool CodecUseEglImage(const uint8_t *data, size_t size)
    {
        struct AllParameters params;
        if (data == nullptr) {
            return false;
        }

        if (memcpy_s(reinterpret_cast<void *>(&params), sizeof(params), data, sizeof(params)) != 0) {
            return false;
        }

        bool result = Preconditions();
        if (!result) {
            HDF_LOGE("%{public}s: Preconditions failed\n", __func__);
            return false;
        }

        struct OmxCodecBuffer inbuffer, outbuffer;
        FillDataOmxCodecBuffer(&inbuffer);
        std::vector<int8_t> eglImage;
        ObjectToVector(params.eglImage, eglImage);
        
        int32_t ret = g_component->UseEglImage(params.portIndex, inbuffer, outbuffer, eglImage);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: UseEglImage failed, ret is [%{public}x]\n", __func__, ret);
        }

        result = Destroy();
        if (!result) {
            HDF_LOGE("%{public}s: Destroy failed\n", __func__);
            return false;
        }

        return true;
    }
} // namespace codec
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::Codec::CodecUseEglImage(data, size);
    return 0;
}
