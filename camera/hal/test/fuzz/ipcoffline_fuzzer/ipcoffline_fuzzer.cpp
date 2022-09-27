/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ipcoffline_fuzzer.h"
#include "fuzz_base.h"

class IPCOfflineFuzzer : public OHOS::Camera::OfflineStreamOperatorStub {
public:
    virtual OHOS::Camera::CamRetCode CancelCapture(int captureId)
    {
        static_cast<void>(captureId);
        return OHOS::Camera::NO_ERROR;
    }
    virtual OHOS::Camera::CamRetCode ReleaseStreams(const std::vector<int>& streamIds)
    {
        return OHOS::Camera::NO_ERROR;
    }
    virtual OHOS::Camera::CamRetCode Release()
    {
        return OHOS::Camera::NO_ERROR;
    }
};

static uint32_t U32_AT(const uint8_t *ptr)
{
    // 24, 16 and 8 represent the shift lengths of ptr for different indexes (0, 1, 2), index 3 needn't move.
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

static int32_t onRemoteRequest(uint32_t code, OHOS::MessageParcel &data)
{
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    IPCOfflineFuzzer IPCOfflineSer;
    auto ret = IPCOfflineSer.OnRemoteRequest(code, data, reply, option);
    return ret;
}

static void IpcFuzzService(const uint8_t *data, size_t size)
{
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    OHOS::MessageParcel dataMessageParcel;
    uint32_t code = U32_AT(data);
    const uint8_t *number = data;
    number = number + sizeof(uint32_t);
    if (size > sizeof(uint32_t)) {
        size_t length = size;
        length = length - sizeof(uint32_t);
        dataMessageParcel.WriteInterfaceToken(IPCOfflineFuzzer::OfflineStreamOperatorStub::GetDescriptor());
        dataMessageParcel.WriteBuffer(number, length);
        dataMessageParcel.RewindRead(0);
        onRemoteRequest(code, dataMessageParcel);
    }
}

static void OnRemoteRequestFunc(const uint8_t *data, size_t size)
{
    IpcFuzzService(data, size);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OnRemoteRequestFunc(data, size);
    return 0;
}