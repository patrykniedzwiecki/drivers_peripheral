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

#ifndef OHOS_VDI_AUDIO_V1_0_IAUDIOCALLBACK_H
#define OHOS_VDI_AUDIO_V1_0_IAUDIOCALLBACK_H

#include <stdbool.h>
#include <stdint.h>
#include "v1_0/audio_types_vdi.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IAUDIO_VDI_CALLBACK_MAJOR_VERSION 1
#define IAUDIO_VDI_CALLBACK_MINOR_VERSION 0

struct IAudioCallbackVdi {
    int32_t (*RenderCallback)(struct IAudioCallbackVdi *self, enum AudioCallbackTypeVdi type, int8_t *reserved,
        int8_t *cookie);
    int32_t (*ParamCallback)(struct IAudioCallbackVdi *self, enum AudioExtParamKeyVdi key, const char *condition,
        const char *value, int8_t *reserved, int8_t cookie);
    int32_t (*GetVersion)(struct IAudioCallbackVdi *self, uint32_t *majorVer, uint32_t *minorVer);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OHOS_VDI_AUDIO_V1_0_IAUDIOCALLBACK_H */