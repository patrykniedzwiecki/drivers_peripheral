/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef DISPLAY_BUFFER_HWI_IMPL_H
#define DISPLAY_BUFFER_HWI_IMPL_H

#include "buffer_handle.h"
#include "idisplay_buffer_hwi.h"
#include "v1_0/display_buffer_type.h"

namespace OHOS {
namespace Model {
namespace Buffer {
using namespace OHOS::HDI::Display::Buffer::V1_0;

class DisplayBufferHwiImpl : public IDisplayBufferHwi {
public:
    DisplayBufferHwiImpl();
    virtual ~DisplayBufferHwiImpl();

    virtual int32_t AllocMem(const AllocInfo& info, BufferHandle*& handle) const override;
    virtual void FreeMem(const BufferHandle& handle) const override;
    virtual void *Mmap(const BufferHandle& handle) const override;
    virtual void *MmapCache(const BufferHandle& buffer) const override;
    virtual int32_t Unmap(const BufferHandle& handle) const override;
    virtual int32_t FlushCache(const BufferHandle& handle) const override;
    virtual int32_t FlushMCache(const BufferHandle& buffer) const override;
    virtual int32_t InvalidateCache(const BufferHandle& handle) const override;
    virtual int32_t IsSupportedAlloc(const std::vector<VerifyAllocInfo> &infos,
        std::vector<bool> &supporteds) const override;
};
} // namespace Buffer
} // namespace Model
} // namespace OHOS

#endif // DISPLAY_BUFFER_HWI_IMPL_H
