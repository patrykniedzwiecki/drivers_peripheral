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

#include "v1_0/motion_callback_service.h"
#include <hdf_base.h>

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_0 {
int32_t MotionCallbackService::OnDataEvent(const HdfMotionEvent &event)
{
    return HDF_SUCCESS;
}
} // V1_0
} // Motion
} // HDI
} // OHOS
