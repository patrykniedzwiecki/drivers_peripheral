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

#ifndef HUKS_HDI_FUZZER_COMMON_H
#define HUKS_HDI_FUZZER_COMMON_H
#include "huks_hdi_passthrough_adapter.h"

#define BLOB_SIZE 10

#ifdef __cplusplus
extern "C" {
#endif

int32_t InitHuksCoreEngine(struct HuksHdi **coreEngine);

int32_t HuksFreshParamSet(struct HksParamSet *paramSet, bool isCopy);

#ifdef __cplusplus
}
#endif


#endif // HUKS_HDI_FUZZER_COMMON_H
