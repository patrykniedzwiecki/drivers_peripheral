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

#include "inputregisterreportcallback_fuzzer.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "input_manager.h"
#include <securec.h>

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        bool result = false;
        int32_t ret;
        const int MAX_DEVICES = 32;
        InputDevDesc sta[MAX_DEVICES];
        InputEventCb g_callback;
        IInputInterface *g_inputInterface;

        (void)memset_s(sta, MAX_DEVICES * sizeof(InputDevDesc), 0, MAX_DEVICES * sizeof(InputDevDesc));
        ret = GetInputInterface(&g_inputInterface);
        if (ret != INPUT_SUCCESS) {
            HDF_LOGE("%s: get input hdi failed, ret %d", __func__, ret);
        }

        ret = g_inputInterface->iInputManager->ScanInputDevice(sta, MAX_DEVICES);
        if (ret) {
            HDF_LOGE("%s: scan device failed, ret %d", __func__, ret);
        }
        for (int32_t i = 0; i < MAX_DEVICES; i++) {
            if (sta[i].devIndex == 0) {
                break;
            }

            ret = g_inputInterface->iInputManager->OpenInputDevice(sta[i].devIndex);
            if (ret != INPUT_SUCCESS) {
                HDF_LOGE("%s: open input device failed, ret %d", __func__, ret);
            }
        }

        ret  = g_inputInterface->iInputReporter->RegisterReportCallback(*(uint32_t *)data, &g_callback);
        if (!ret) {
            result = true;
        }

        ret = g_inputInterface->iInputReporter->UnregisterReportCallback(*(uint32_t *)data);
        if (!ret) {
            result = true;
        }

        for (int32_t i = 0; i < MAX_DEVICES; i++) {
            if (sta[i].devIndex == 0) {
                break;
            }

            ret = g_inputInterface->iInputManager->CloseInputDevice(sta[i].devIndex);
            if (ret != INPUT_SUCCESS) {
                HDF_LOGE("%s: close input device failed, ret %d", __func__, ret);
            }
        }

        ReleaseInputInterface(g_inputInterface);
        return result;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

