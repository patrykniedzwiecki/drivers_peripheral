/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uvc_node.h"
#include <unistd.h>
#include "metadata_controller.h"

namespace OHOS::Camera {
UvcNode::UvcNode(const std::string& name, const std::string& type) : SourceNode(name, type), NodeBase(name, type)
{
    CAMERA_LOGI("%s enter, type(%s)\n", name_.c_str(), type_.c_str());
    RetCode rc = RC_OK;
    constexpr int ITEM_CAPACITY_SIZE = 30;
    constexpr int DATA_CAPACITY_SIZE = 1000;
    deviceManager_ = IDeviceManager::GetInstance();
    if (deviceManager_ == nullptr) {
        CAMERA_LOGE("Get device manager failed.");
        return;
    }
    rc = GetDeviceController();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("GetDeviceController failed.");
        return;
    }
    meta_ = std::make_shared<CameraMetadata>(ITEM_CAPACITY_SIZE, DATA_CAPACITY_SIZE);
}

UvcNode::~UvcNode()
{
    CAMERA_LOGI("~Uvc Node exit.");
}

RetCode UvcNode::GetDeviceController()
{
    CameraId cameraId = CAMERA_FIRST;
    sensorController_ = std::static_pointer_cast<SensorController>
        (deviceManager_->GetController(cameraId, DM_M_SENSOR, DM_C_SENSOR));
    if (sensorController_ == nullptr) {
        CAMERA_LOGE("Get device controller failed");
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode UvcNode::Init(const int32_t streamId)
{
    return RC_OK;
}

RetCode UvcNode::Flush(const int32_t streamId)
{
    RetCode rc = RC_OK;

    if (sensorController_ != nullptr) {
        rc = sensorController_->Flush(streamId);
        CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, RC_ERROR);
    }
    rc = SourceNode::Flush(streamId);

    return rc;
}

RetCode UvcNode::Start(const int32_t streamId)
{
    RetCode rc = RC_OK;
    std::vector<std::shared_ptr<IPort>> outPorts = GetOutPorts();
    for (const auto& it : outPorts) {
        DeviceFormat format;
        format.fmtdesc.pixelformat = V4L2_PIX_FMT_YUYV;
        format.fmtdesc.width = it->format_.w_;
        format.fmtdesc.height = it->format_.h_;
        int bufCnt = it->format_.bufferCount_;
        rc = sensorController_->Start(bufCnt, format);
        if (rc == RC_ERROR) {
            CAMERA_LOGE("Start failed.");
            return RC_ERROR;
        }
    }
    if (meta_ != nullptr) {
        sensorController_->ConfigFps(meta_);
    }
    rc = SourceNode::Start(streamId);
    return rc;
}

RetCode UvcNode::Stop(const int32_t streamId)
{
    RetCode rc = RC_OK;

    if (sensorController_ != nullptr) {
        rc = sensorController_->Stop();
        CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, RC_ERROR);
    }

    return SourceNode::Stop(streamId);
}

RetCode UvcNode::SetCallback()
{
    MetadataController &metaDataController = MetadataController::GetInstance();
    metaDataController.AddNodeCallback([this](const std::shared_ptr<CameraMetadata> &metadata) {
        OnMetadataChanged(metadata);
    });
    return RC_OK;
}

int32_t UvcNode::GetStreamId(const CaptureMeta &meta)
{
    common_metadata_header_t *data = meta->get();
    if (data == nullptr) {
        CAMERA_LOGE("Data is nullptr");
        return RC_ERROR;
    }
    camera_metadata_item_t entry;
    int32_t streamId = -1;
    int rc = FindCameraMetadataItem(data, OHOS_CAMERA_STREAM_ID, &entry);
    if (rc == 0) {
        streamId = *entry.data.i32;
    }
    return streamId;
}

void UvcNode::GetUpdateFps(const std::shared_ptr<CameraMetadata>& metadata)
{
    common_metadata_header_t *data = metadata->get();
    camera_metadata_item_t entry;
    int ret = FindCameraMetadataItem(data, OHOS_CONTROL_FPS_RANGES, &entry);
    if (ret == 0) {
        std::vector<int32_t> fpsRange;
        for (int i = 0; i < entry.count; i++) {
            fpsRange.push_back(*(entry.data.i32 + i));
        }
        meta_->addEntry(OHOS_CONTROL_FPS_RANGES, fpsRange.data(), fpsRange.size());
    }
}

void UvcNode::OnMetadataChanged(const std::shared_ptr<CameraMetadata>& metadata)
{
    if (metadata == nullptr) {
        CAMERA_LOGE("Meta is nullptr");
        return;
    }
    constexpr uint32_t DEVICE_STREAM_ID = 0;
    if (sensorController_ != nullptr) {
        if (GetStreamId(metadata) == DEVICE_STREAM_ID) {
            sensorController_->Configure(metadata);
        }
    } else {
        CAMERA_LOGE("UvcNode sensorController_ is null");
    }
    GetUpdateFps(metadata);
}

void UvcNode::SetBufferCallback()
{
    sensorController_->SetNodeCallBack([&](std::shared_ptr<FrameSpec> frameSpec) {
        OnPackBuffer(frameSpec);
    });
    return;
}

void UvcNode::DeliverBuffer(std::shared_ptr<IBuffer>& buffer)
{
    if (buffer == nullptr) {
        CAMERA_LOGE("UvcNode::DeliverBuffer frameSpec is null");
        return;
    }

    SourceNode::DeliverBuffer(buffer);
    return;
}


RetCode UvcNode::ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    CAMERA_LOGI("Provide buffers enter.");
    if (sensorController_->SendFrameBuffer(frameSpec) == RC_OK) {
        CAMERA_LOGD("Sendframebuffer success bufferpool id = %llu", frameSpec->bufferPoolId_);
        return RC_OK;
    }
    return RC_ERROR;
}
REGISTERNODE(UvcNode, {"uvc"})
} // namespace OHOS::Camera