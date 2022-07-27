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

#include "vibrator_interface_impl.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include "vibrator_if.h"

#define HDF_LOG_TAG              uhdf_vibrator

namespace OHOS {
namespace HDI {
namespace Vibrator {
namespace V1_1 {
extern "C" IVibratorInterface *VibratorInterfaceImplGetInstance(void)
{
    return new (std::nothrow) VibratorInterfaceImpl();
}

int32_t VibratorInterfaceImpl::StartOnce(uint32_t duration)
{
    const struct VibratorInterface *vibratorInterface = NewVibratorInterfaceInstance();
    if (vibratorInterface == nullptr || vibratorInterface->StartOnce == nullptr) {
        HDF_LOGE("%{public}s: get vibrator Module instance failed", __func__);
        return HDF_FAILURE;
    }
    int32_t ret = vibratorInterface->StartOnce(duration);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, ret);
    }
    return ret;
}

int32_t VibratorInterfaceImpl::Start(const std::string& effectType)
{
    const struct VibratorInterface *vibratorInterface = NewVibratorInterfaceInstance();
    if (vibratorInterface == nullptr || vibratorInterface->Start == nullptr) {
        HDF_LOGE("%{public}s: get vibrator Module instance failed", __func__);
        return HDF_FAILURE;
    }
    int32_t ret = vibratorInterface->Start(effectType.c_str());
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, ret);
    }
    return ret;
}

int32_t VibratorInterfaceImpl::Stop(HdfVibratorMode mode)
{
    const struct VibratorInterface *vibratorInterface = NewVibratorInterfaceInstance();
    if (vibratorInterface == nullptr || vibratorInterface->Stop == nullptr) {
        HDF_LOGE("%{public}s: get vibrator Module instance failed", __func__);
        return HDF_FAILURE;
    }

    VibratorMode tmp;
    if (mode == HDF_VIBRATOR_MODE_ONCE) {
        tmp = VIBRATOR_MODE_ONCE;
    } else if (mode == HDF_VIBRATOR_MODE_PRESET) {
        tmp = VIBRATOR_MODE_PRESET;
    } else if (mode == HDF_VIBRATOR_MODE_BUTT) {
        tmp = VIBRATOR_MODE_BUTT;
    } else {
        HDF_LOGE("%{public}s: invalid param", __func__);
        return HDF_FAILURE;
    }

    int32_t ret = vibratorInterface->Stop(tmp);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, ret);
    }
    return ret;
}

int32_t VibratorInterfaceImpl::GetVibratorInfo(std::vector<HdfVibratorInfo>& vibratorInfo)
{
    const struct VibratorInterface *vibratorInterface = NewVibratorInterfaceInstance();
    if (vibratorInterface == nullptr || vibratorInterface->GetVibratorInfo == nullptr) {
        HDF_LOGE("%{public}s: get vibrator Module instance failed", __func__);
        return HDF_FAILURE;
    }
    HdfVibratorInfo hdfVibratorInfo;
    struct VibratorInfo *tmp = nullptr;

    int32_t ret = vibratorInterface->GetVibratorInfo(&tmp);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, ret);
        return ret;
    }

    if (tmp == nullptr) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, ret);
        return HDF_FAILURE;
    }

    hdfVibratorInfo.isSupportFrequency = tmp->isSupportFrequency;
    hdfVibratorInfo.frequencyMaxValue = tmp->frequencyMaxValue;
    hdfVibratorInfo.frequencyMinValue = tmp->frequencyMinValue;
    hdfVibratorInfo.isSupportIntensity = tmp->isSupportIntensity;
    hdfVibratorInfo.intensityMaxValue = tmp->intensityMaxValue;
    hdfVibratorInfo.intensityMinValue = tmp->intensityMinValue;
    vibratorInfo.push_back(std::move(hdfVibratorInfo));

    return HDF_SUCCESS;
}

int32_t VibratorInterfaceImpl::EnableVibratorModulation(uint32_t duration, int32_t intensity, int32_t frequency)
{
    const struct VibratorInterface *vibratorInterface = NewVibratorInterfaceInstance();
    if (vibratorInterface == nullptr || vibratorInterface->EnableVibratorModulation == nullptr) {
        HDF_LOGE("%{public}s: get vibrator Module instance failed", __func__);
        return HDF_FAILURE;
    }

    int32_t ret = vibratorInterface->EnableVibratorModulation(duration, intensity, frequency);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, ret);
    }
    return ret;
}
} // V1_1
} // Vibrator
} // HDI
} // OHOS