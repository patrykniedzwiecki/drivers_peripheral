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

#include "mapper_service.h"
#include <dlfcn.h>
#include <hdf_base.h>
#include "display_log.h"
#include "hdf_log.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace Buffer {
namespace V1_0 {
extern "C" IMapper *MapperImplGetInstance(void)
{
    return new (std::nothrow) MapperService();
}

MapperService::MapperService()
    : libHandle_(nullptr),
    hwiImpl_(nullptr),
    createHwi_(nullptr),
    destroyHwi_(nullptr)
{
    int32_t ret = LoadHwi();
    if (ret == HDF_SUCCESS) {
        hwiImpl_ = createHwi_();
        CHECK_NULLPOINTER_RETURN(hwiImpl_);
    } else {
        HDF_LOGE("error: LoadHwi failure, lib path:%{public}s", DISPLAY_BUFFER_HWI_LIBRARY_PATH);
    }
}

MapperService::~MapperService()
{
    if (destroyHwi_ != nullptr && hwiImpl_ != nullptr) {
        destroyHwi_(hwiImpl_);
    }
    if (libHandle_ != nullptr) {
        dlclose(libHandle_);
    }
}

int32_t MapperService::LoadHwi()
{
    const char *errStr = dlerror();
    if (errStr) {
        HDF_LOGI("warning, existing dlerror: %{public}s", errStr);
    }
    libHandle_ = dlopen(DISPLAY_BUFFER_HWI_LIBRARY_PATH, RTLD_NOW);
    CHECK_NULLPOINTER_RETURN_VALUE(libHandle_, HDF_FAILURE);

    createHwi_ = reinterpret_cast<CreateDisplayBufferHwiFunc>(dlsym(libHandle_, "CreateDisplayBufferHwi"));
    errStr = dlerror();
    if (errStr) {
        HDF_LOGE("error: %{public}s", errStr);
        return HDF_FAILURE;
    }

    destroyHwi_ = reinterpret_cast<DestroyDisplayBufferHwiFunc>(dlsym(libHandle_, "DestroyDisplayBufferHwi"));
    errStr = dlerror();
    if (errStr) {
        HDF_LOGE("error: %{public}s", errStr);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t MapperService::FreeMem(const sptr<NativeBuffer> &handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(hwiImpl_, HDF_FAILURE);
    hwiImpl_->FreeMem(*handle->Move());
    return HDF_SUCCESS;
}

int32_t MapperService::Mmap(const sptr<NativeBuffer> &handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(hwiImpl_, HDF_FAILURE);
    void *retPtr = hwiImpl_->Mmap(*handle->GetBufferHandle());
    HDF_LOGD("%{public}s@%{public}d virAddr=%{public}p", __func__, __LINE__, handle->GetBufferHandle()->virAddr);
    CHECK_NULLPOINTER_RETURN_VALUE(retPtr, HDF_FAILURE);
    return HDF_SUCCESS;
}

int32_t MapperService::MmapCache(const sptr<NativeBuffer> &handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(hwiImpl_, HDF_FAILURE);
    void *retPtr = hwiImpl_->MmapCache(*handle->GetBufferHandle());
    CHECK_NULLPOINTER_RETURN_VALUE(retPtr, HDF_FAILURE);
    return HDF_SUCCESS;
}

int32_t MapperService::Unmap(const sptr<NativeBuffer> &handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(hwiImpl_, HDF_FAILURE);
    int32_t ec = hwiImpl_->Unmap(*handle->GetBufferHandle());
    return ec;
}

int32_t MapperService::FlushCache(const sptr<NativeBuffer> &handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(hwiImpl_, HDF_FAILURE);
    int32_t ec = hwiImpl_->FlushCache(*handle->GetBufferHandle());
    return ec;
}

int32_t MapperService::FlushMCache(const sptr<NativeBuffer> &handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(hwiImpl_, HDF_FAILURE);
    int32_t ec = hwiImpl_->FlushMCache(*handle->GetBufferHandle());
    return ec;
}

int32_t MapperService::InvalidateCache(const sptr<NativeBuffer> &handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(hwiImpl_, HDF_FAILURE);
    int32_t ec = hwiImpl_->InvalidateCache(*handle->GetBufferHandle());
    return ec;
}
} // namespace V1_0
} // namespace Buffer
} // namespace Display
} // namespace HDI
} // namespace OHOS