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

#include <hdf_base.h>
#include <hdf_device_desc.h>
#include <hdf_sbuf_ipc.h>
#include "v1_0/face_auth_interface_stub.h"
#include "iam_logger.h"

#define LOG_LABEL OHOS::UserIAM::Common::LABEL_FACE_AUTH_HDI

using namespace OHOS::HDI::FaceAuth::V1_0;

struct HdfFaceAuthInterfaceHost {
    struct IDeviceIoService ioService;
    OHOS::sptr<OHOS::IRemoteObject> stub;
};

static int32_t FaceAuthInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    IAM_LOGI("start");
    auto *hdfFaceAuthInterfaceHost = CONTAINER_OF(client->device->service,
        struct HdfFaceAuthInterfaceHost, ioService);

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageOption option;

    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        IAM_LOGE("%{public}s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
        IAM_LOGE("%{public}s:invalid reply sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return hdfFaceAuthInterfaceHost->stub->SendRequest(cmdId, *dataParcel, *replyParcel, option);
}

static int HdfFaceAuthInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    IAM_LOGI("start");
    return HDF_SUCCESS;
}

static int HdfFaceAuthInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    IAM_LOGI("start");
    auto *hdfFaceAuthInterfaceHost = new (std::nothrow) HdfFaceAuthInterfaceHost;
    if (hdfFaceAuthInterfaceHost == nullptr) {
        IAM_LOGE("%{public}s: failed to create create HdfFaceAuthInterfaceHost object", __func__);
        return HDF_FAILURE;
    }

    hdfFaceAuthInterfaceHost->ioService.Dispatch = FaceAuthInterfaceDriverDispatch;
    hdfFaceAuthInterfaceHost->ioService.Open = NULL;
    hdfFaceAuthInterfaceHost->ioService.Release = NULL;

    auto serviceImpl = IFaceAuthInterface::Get(true);
    if (serviceImpl == nullptr) {
        IAM_LOGE("%{public}s: failed to get of implement service", __func__);
        return HDF_FAILURE;
    }

    hdfFaceAuthInterfaceHost->stub = OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(serviceImpl,
        IFaceAuthInterface::GetDescriptor());
    if (hdfFaceAuthInterfaceHost->stub == nullptr) {
        IAM_LOGE("%{public}s: failed to get stub object", __func__);
        return HDF_FAILURE;
    }

    deviceObject->service = &hdfFaceAuthInterfaceHost->ioService;
    IAM_LOGI("success");
    return HDF_SUCCESS;
}

static void HdfFaceAuthInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    IAM_LOGI("start");
    auto *hdfFaceAuthInterfaceHost = CONTAINER_OF(deviceObject->service,
        struct HdfFaceAuthInterfaceHost, ioService);
    delete hdfFaceAuthInterfaceHost;
    IAM_LOGI("success");
}

static struct HdfDriverEntry g_faceAuthInterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "faceauth_interface_service",
    .Bind = HdfFaceAuthInterfaceDriverBind,
    .Init = HdfFaceAuthInterfaceDriverInit,
    .Release = HdfFaceAuthInterfaceDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_faceAuthInterfaceDriverEntry);
#ifndef __cplusplus
}
#endif

