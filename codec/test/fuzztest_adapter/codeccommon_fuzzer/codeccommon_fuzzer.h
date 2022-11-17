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

#ifndef CODECCOMMON_FUZZER_H
#define CODECCOMMON_FUZZER_H

#include "codec_callback_type_stub.h"
#include "codec_component_manager.h"
#include "codec_component_type.h"
#include "codec_omx_ext.h"

#include <hdf_log.h>
#include <osal_mem.h>
namespace OHOS {
namespace Codec {
extern CodecComponentType *g_component;
extern CodecCallbackType *g_callback;

void FillDataOmxCodecBuffer(struct OmxCodecBuffer *dataFuzz);
bool Preconditions();
bool Destroy();
} // namespace Codec
} // namespace OHOS
#endif // CODECCOMMON_FUZZER_H