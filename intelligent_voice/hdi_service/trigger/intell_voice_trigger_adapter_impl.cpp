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
#include "intell_voice_trigger_adapter_impl.h"
#include "hdf_base.h"
#include "intell_voice_log.h"
#include "securec.h"
#include "scope_guard.h"

#undef HDF_LOG_TAG
#define HDF_LOG_TAG "TriggerAdapterImpl"

using namespace OHOS::HDI::IntelligentVoice::Trigger::V1_0;

namespace OHOS {
namespace IntelligentVoice {
namespace Trigger {
IntellVoiceTriggerCallbackDevice::IntellVoiceTriggerCallbackDevice(OHOS::sptr<IIntellVoiceTriggerCallback> callback)
    : callback_(callback)
{
}

IntellVoiceTriggerCallbackDevice::~IntellVoiceTriggerCallbackDevice()
{
    callback_ = nullptr;
}

void IntellVoiceTriggerCallbackDevice::OnRecognitionHdiEvent(const IntellVoiceRecognitionEvent &event, int32_t cookie)
{
    if (callback_ == nullptr) {
        INTELLIGENT_VOICE_LOGE("callback_ is nullptr");
        return;
    }
    callback_->OnRecognitionHdiEvent(event, cookie);
}

IntellVoiceTriggerAdapterImpl::IntellVoiceTriggerAdapterImpl(std::unique_ptr<ITrigger> adapter)
    : adapter_(std::move(adapter))
{
}

IntellVoiceTriggerAdapterImpl::~IntellVoiceTriggerAdapterImpl()
{
    adapter_ = nullptr;
}

int32_t IntellVoiceTriggerAdapterImpl::GetProperties(IntellVoiceTriggerProperties& properties)
{
    return adapter_->GetProperties(properties);
}

int32_t IntellVoiceTriggerAdapterImpl::LoadModel(const IntellVoiceTriggerModel &model,
    const sptr<IIntellVoiceTriggerCallback> &triggerCallback, int32_t cookie, int32_t &handle)
{
    std::shared_ptr<ITriggerCallback> cb = std::make_shared<IntellVoiceTriggerCallbackDevice>(triggerCallback);
    if (cb == nullptr) {
        INTELLIGENT_VOICE_LOGE("callback is nullptr");
        return HDF_ERR_MALLOC_FAIL;
    }

    if (model.data == nullptr) {
        INTELLIGENT_VOICE_LOGE("model data is nullptr");
        return HDF_ERR_INVALID_PARAM;
    }

    ON_SCOPE_EXIT {
        INTELLIGENT_VOICE_LOGI("close ashmem");
        model.data->UnmapAshmem();
        model.data->CloseAshmem();
    };

    std::vector<uint8_t> modelData;
    if (!GetModelDataFromAshmem(model.data, modelData)) {
        return HDF_ERR_INVALID_PARAM;
    }

    TriggerModel triggerModel(model.type, model.uid, modelData);
    return adapter_->LoadIntellVoiceTriggerModel(triggerModel, cb, cookie, handle);
}

int32_t IntellVoiceTriggerAdapterImpl::UnloadModel(int32_t handle)
{
    return adapter_->UnloadIntellVoiceTriggerModel(handle);
}

int32_t IntellVoiceTriggerAdapterImpl::Start(int32_t handle)
{
    return adapter_->Start(handle);
}

int32_t IntellVoiceTriggerAdapterImpl::Stop(int32_t handle)
{
    return adapter_->Stop(handle);
}

bool IntellVoiceTriggerAdapterImpl::GetModelDataFromAshmem(sptr<Ashmem> ashmem, std::vector<uint8_t> &modelData)
{
    if (ashmem == nullptr) {
        INTELLIGENT_VOICE_LOGE("ashmem is nullptr");
        return false;
    }

    uint32_t size = static_cast<uint32_t>(ashmem->GetAshmemSize());
    if (size == 0) {
        INTELLIGENT_VOICE_LOGE("size is zero");
        return false;
    }

    if (!ashmem->MapReadOnlyAshmem()) {
        INTELLIGENT_VOICE_LOGE("map ashmem failed");
        return false;
    }

    const uint8_t *buffer = static_cast<const uint8_t *>(ashmem->ReadFromAshmem(size, 0));
    if (buffer == nullptr) {
        INTELLIGENT_VOICE_LOGE("read from ashmem failed");
        return false;
    }

    modelData.insert(modelData.begin(), buffer, buffer + size);
    return true;
}
}
}
}