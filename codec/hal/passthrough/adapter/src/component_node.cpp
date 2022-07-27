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

#include "component_node.h"
#include <hdf_log.h>
#include <securec.h>
#include <osal_mem.h>
#include <unistd.h>
#include "codec_interface.h"
#include "component_common.h"

#define HDF_LOG_TAG codec_hdi_passthrough

namespace OHOS {
namespace Codec {
namespace CodecAdapter {
int32_t ComponentNode::OnEvent(UINTPTR userData, EventType event, uint32_t length, int32_t eventData[])
{
    ComponentNode *node = reinterpret_cast<ComponentNode *>(userData);
    if (node != nullptr) {
        node->OnEvent(event, length, eventData);
    }

    return HDF_SUCCESS;
}

int32_t ComponentNode::InputBufferAvailable(UINTPTR userData, CodecBuffer *inBuf, int32_t *acquireFd)
{
    ComponentNode *node = reinterpret_cast<ComponentNode *>(userData);
    if (node != nullptr) {
        node->OnEmptyBufferDone(inBuf, acquireFd);
    }

    return HDF_SUCCESS;
}

int32_t ComponentNode::OutputBufferAvailable(UINTPTR userData, CodecBuffer *outBuf, int32_t *acquireFd)
{
    ComponentNode *node = reinterpret_cast<ComponentNode *>(userData);
    if (node != nullptr) {
        node->OnFillBufferDone(outBuf, acquireFd);
    }

    return HDF_SUCCESS;
}

CodecCallback ComponentNode::callbacks_ = {&ComponentNode::OnEvent, &ComponentNode::InputBufferAvailable,
                                           &ComponentNode::OutputBufferAvailable};

ComponentNode::ComponentNode(CODEC_HANDLETYPE handle, CodecExInfo info)
    : comp_(handle),
      omxCallback_(nullptr),
      exInfo_(info),
      appData_(0),
      bufferId_(0),
      setCallbackComplete_(false),
      state_(OMX_StateMax),
      codecType_(info.type),
      inputMode_(ALLOCATE_INPUT_BUFFER_USER_PRESET),
      outputMode_(ALLOCATE_OUTPUT_BUFFER_USER_PRESET)
{
}

int32_t ComponentNode::GetComponentVersion(CompVerInfo &verInfo)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null !", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::SendCommand(OMX_COMMANDTYPE cmd, uint32_t param, int8_t *cmdData, uint32_t cmdDataLen)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t err;
    switch (cmd) {
        case OMX_CommandStateSet:
            err = ChangeComponentState(param);
            break;
        case OMX_CommandFlush:
            err = FlushComponent(param);
            break;

        default: {
            err = HDF_ERR_NOT_SUPPORT;
            HDF_LOGE("%{public}s error, CMD[%{public}d] is not support!", __func__, cmd);
            break;
        }
    }
    return err;
}

int32_t ComponentNode::GetParameter(OMX_INDEXTYPE paramIndex, int8_t *param, uint32_t paramLen)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t paramCnt = 0;
    Param paramOut[PARAM_MAX_NUM] = {};
    int32_t err = SplitParam(paramIndex, param, paramOut, paramCnt, codecType_);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("%{public}s error, paramIndex is not support ", __func__);
        return err;
    }
    err = HDF_FAILURE;
    for (int32_t i = 0; i < paramCnt; i++) {
        int32_t ret = CodecGetParameter(comp_, &paramOut[i], 1);
        if (ret == HDF_SUCCESS) {
            HDF_LOGI("%{public}s CodecGetParameter %{public}d Success ", __func__, paramOut[i].key);
            err = HDF_SUCCESS;
        }
    }

    ParseParam(paramIndex, paramOut, paramCnt, param, exInfo_);
    return err;
}

int32_t ComponentNode::SetParameter(OMX_INDEXTYPE paramIndex, int8_t *param, uint32_t paramLen)
{
    if (comp_ == nullptr || param == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null or param is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t paramCnt = 1;
    Param paramOut[PARAM_MAX_NUM] = {};
    int32_t err = SplitParam(paramIndex, param, paramOut, paramCnt, codecType_);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("%{public}s error, paramIndex is not support ", __func__);
        return err;
    }
    err = HDF_FAILURE;
    for (int32_t i = 0; i < paramCnt; i++) {
        int32_t ret = CodecSetParameter(comp_, &paramOut[i], 1);
        HDF_LOGI("%{public}s CodecSetParameter %{public}d ret[%{public}d] ", __func__, paramOut[i].key, ret);
        if (ret == HDF_SUCCESS) {
            HDF_LOGI("%{public}s CodecSetParameter %{public}d Success ", __func__, paramOut[i].key);
            err = HDF_SUCCESS;
        }
    }
    return err;
}

int32_t ComponentNode::GetConfig(OMX_INDEXTYPE index, int8_t *config, uint32_t configLen)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::SetConfig(OMX_INDEXTYPE index, int8_t *config, uint32_t configLen)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::GetExtensionIndex(const char *parameterName, OMX_INDEXTYPE *indexType)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::SetState(OMX_STATETYPE state)
{
    int32_t err = HDF_FAILURE;
    switch (state) {
        case OMX_StateInvalid:
            err = HDF_SUCCESS;
            break;
        case OMX_StateLoaded: {
            if (state_ == OMX_StateIdle || state_ == OMX_StateWaitForResources || state_ == OMX_StateMax) {
                err = HDF_SUCCESS;
            }
            break;
        }
        case OMX_StateIdle: {
            if (state_ == OMX_StateWaitForResources || state_ == OMX_StateLoaded ||
                    state_ == OMX_StatePause ||state_ == OMX_StateExecuting) {
                err = HDF_SUCCESS;
            }
            break;
        }
        case OMX_StateExecuting: {
            if (state_ == OMX_StateIdle || state_ == OMX_StatePause) {
                err = HDF_SUCCESS;
            }
            break;
        }
        case OMX_StatePause: {
            if (state_ == OMX_StateIdle || state_ == OMX_StateExecuting) {
                err = HDF_SUCCESS;
            }
            break;
        }
        case OMX_StateWaitForResources: {
            if (state_ == OMX_StateLoaded) {
                err = HDF_SUCCESS;
            }
            break;
        }

        default:
            HDF_LOGW("%{public}s warn, unsupport state[%{public}d]", __func__, state);
            break;
    }

    if (err == HDF_SUCCESS) {
        state_ = state;
    }
    HDF_LOGI("%{public}s set state[%{public}d] , current state is [%{public}d]  ", __func__, state, state_);

    return err;
}

int32_t ComponentNode::GetState(OMX_STATETYPE *state)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (state == nullptr) {
        HDF_LOGE("%{public}s error, state is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    *state = state_;
    return HDF_SUCCESS;
}

int32_t ComponentNode::ComponentTunnelRequest(uint32_t port, int32_t omxHandleTypeTunneledComp, uint32_t tunneledPort,
                                              OMX_TUNNELSETUPTYPE *tunnelSetup)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::SetCallbacks(CodecCallbackType *omxCallback, int64_t appData)
{
    int32_t err = HDF_SUCCESS;
    if (!setCallbackComplete_) {
        if (comp_ == nullptr) {
            HDF_LOGE("%{public}s error, comp_ is null ", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        err = CodecSetCallback(comp_, &callbacks_, reinterpret_cast<UINTPTR>(this));
        if (err != HDF_SUCCESS) {
            HDF_LOGE("%{public}s  CodecSetCallback error[0x%{public}x]", __func__, err);
            return err;
        }
        setCallbackComplete_ = true;
    }
    this->omxCallback_ = omxCallback;
    this->appData_ = appData;

    return err;
}

int32_t ComponentNode::UseEglImage(OmxCodecBuffer &buffer, uint32_t portIndex, int8_t *eglImage,
                                   uint32_t eglImageLen)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::ComponentRoleEnum(uint8_t *role, uint32_t roleLen, uint32_t index)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::ComponentDeInit()
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ is null ", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::OnEvent(EventType event, uint32_t length, int32_t eventData[])
{
    if (omxCallback_ == nullptr || omxCallback_->EventHandler == nullptr) {
        HDF_LOGE("%{public}s omxCallback_ or EventHandler is null", __func__);
        return HDF_FAILURE;
    }

    OMX_EVENTTYPE omxEvent;
    EventInfo info = {0};
    info.appData = appData_;
    info.data1 = 0;
    info.data2 = 0;
    info.eventData = nullptr;
    info.eventDataLen = 0;
    switch (event) {
        case EVENT_ERROR: {
            omxEvent = OMX_EventError;
            if (length > 0) {
                info.data1 =  eventData[0];
            }
            break;
        }
        case EVENT_FLUSH_COMPLETE: {
            omxEvent = OMX_EventCmdComplete;
            info.data1 =  OMX_CommandFlush;
            break;
        }
        case EVENT_EOS_COMPLETE: {
            omxEvent = OMX_EventBufferFlag;
            break;
        }

        default: {
            HDF_LOGW("%{public}s unsupport event [%{public}d]", __func__, event);
            omxEvent = OMX_EventMax;
            break;
        }
    }
    HDF_LOGD("%{public}s EventHandler ON", __func__);
    omxCallback_->EventHandler(omxCallback_, omxEvent, &info);

    return HDF_SUCCESS;
}

int32_t ComponentNode::OnEmptyBufferDone(CodecBuffer *inBuf, int32_t *acquireFd)
{
    if (omxCallback_ == nullptr || inBuf == nullptr || omxCallback_->EmptyBufferDone == nullptr) {
        HDF_LOGE("%{public}s error, omxCallback_ or buffer or EmptyBufferDone is null", __func__);
        return HDF_FAILURE;
    }

    OmxCodecBuffer omxCodecBuffer = {0};
    ConvertCodecBufferToOmxCodecBuffer(omxCodecBuffer, *inBuf);
    omxCodecBuffer.size = sizeof(OmxCodecBuffer);
    omxCodecBuffer.fenceFd = *acquireFd;
    omxCodecBuffer.type = READ_ONLY_TYPE;
    HDF_LOGD("%{public}s EmptyBufferDone ON", __func__);
    omxCallback_->EmptyBufferDone(omxCallback_, appData_, &omxCodecBuffer);

    return HDF_SUCCESS;
}

int32_t ComponentNode::OnFillBufferDone(CodecBuffer *outBuf, int32_t *acquireFd)
{
    if (omxCallback_ == nullptr || outBuf == nullptr || omxCallback_->FillBufferDone == nullptr) {
        HDF_LOGE("%{public}s error, omxCallback_ or buffer or FillBufferDone is null", __func__);
        return HDF_FAILURE;
    }

    OmxCodecBuffer omxCodecBuffer = {0};
    ConvertCodecBufferToOmxCodecBuffer(omxCodecBuffer, *outBuf);
    omxCodecBuffer.size = sizeof(OmxCodecBuffer);
    omxCodecBuffer.fenceFd = *acquireFd;
    omxCodecBuffer.type = READ_WRITE_TYPE;
    HDF_LOGD("%{public}s FillBufferDone ON", __func__);
    omxCallback_->FillBufferDone(omxCallback_, appData_, &omxCodecBuffer);

    return HDF_SUCCESS;
}

int32_t ComponentNode::UseBuffer(uint32_t portIndex, OmxCodecBuffer &buffer)
{
    if (comp_ == nullptr || portIndex > OUTPUT_PORTINDEX) {
        HDF_LOGE("%{public}s error, comp_ is null or portIndex[%{public}d] > OUTPUT_PORTINDEX", __func__, portIndex);
        return HDF_ERR_INVALID_PARAM;
    }
    if (buffer.buffer == nullptr) {
        HDF_LOGE("%{public}s error, buffer is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    AllocateBufferMode mode = ALLOCATE_INPUT_BUFFER_USER_PRESET;
    if (portIndex == OUTPUT_PORTINDEX) {
        mode = ALLOCATE_OUTPUT_BUFFER_USER_PRESET;
    }
    int32_t err = SetPortMode(portIndex, buffer, mode);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("%{public}s error, SetPortMode failed", __func__);
        return err;
    }

    CodecBuffer *codecBuffer = (CodecBuffer *)OsalMemCalloc(sizeof(CodecBuffer) + sizeof(BufferInfo));
    if (codecBuffer == nullptr) {
        HDF_LOGE("%{public}s error, codecBuffer is nullptr", __func__);
        return HDF_FAILURE;
    }
    buffer.bufferId = bufferId_++;
    ConvertOmxCodecBufferToCodecBuffer(buffer, *codecBuffer);
    if (portIndex == INPUT_PORTINDEX) {
        err = CodecQueueInput(comp_, codecBuffer, 0, buffer.fenceFd);
        if (err != HDF_SUCCESS) {
            HDF_LOGE("%{public}s error, CodecQueueInput failed", __func__);
            OsalMemFree(codecBuffer);
            return err;
        }
    } else if (portIndex == OUTPUT_PORTINDEX) {
        err = CodecQueueOutput(comp_, codecBuffer, 0, buffer.fenceFd);
        if (err != HDF_SUCCESS) {
            HDF_LOGE("%{public}s error, CodecQueueOutput failed", __func__);
            OsalMemFree(codecBuffer);
            return err;
        }
    }
    ConvertCodecBufferToOmxCodecBuffer(buffer, *codecBuffer);
    OsalMemFree(codecBuffer);
    return err;
}

int32_t ComponentNode::AllocateBuffer(uint32_t portIndex, OmxCodecBuffer &buffer)
{
    if (comp_ == nullptr || portIndex > OUTPUT_PORTINDEX) {
        HDF_LOGE("%{public}s error, comp_ is null or portIndex[%{public}d] > OUTPUT_PORTINDEX", __func__, portIndex);
        return HDF_ERR_INVALID_PARAM;
    }
    if (buffer.buffer == nullptr) {
        HDF_LOGE("%{public}s error, buffer is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    AllocateBufferMode mode = ALLOCATE_INPUT_BUFFER_CODEC_PRESET;
    if (portIndex == OUTPUT_PORTINDEX) {
        mode = ALLOCATE_OUTPUT_BUFFER_CODEC_PRESET;
    }
    int32_t err = SetPortMode(portIndex, buffer, mode);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("%{public}s error, SetPortMode failed", __func__);
        return err;
    }

    CodecBuffer *codecBuffer = (CodecBuffer *)OsalMemCalloc(sizeof(CodecBuffer) + sizeof(BufferInfo));
    if (codecBuffer == nullptr) {
        HDF_LOGE("%{public}s error, codecBuffer is nullptr", __func__);
        return HDF_FAILURE;
    }
    buffer.bufferId = bufferId_++;
    ConvertOmxCodecBufferToCodecBuffer(buffer, *codecBuffer);
    if (portIndex == INPUT_PORTINDEX) {
        err = CodecQueueInput(comp_, codecBuffer, 0, buffer.fenceFd);
        if (err != HDF_SUCCESS) {
            HDF_LOGE("%{public}s error, CodecQueueInput failed", __func__);
            OsalMemFree(codecBuffer);
            return err;
        }
    } else if (portIndex == OUTPUT_PORTINDEX) {
        err = CodecQueueOutput(comp_, codecBuffer, 0, buffer.fenceFd);
        if (err != HDF_SUCCESS) {
            HDF_LOGE("%{public}s error, CodecQueueOutput failed", __func__);
            OsalMemFree(codecBuffer);
            return err;
        }
    }
    ConvertCodecBufferToOmxCodecBuffer(buffer, *codecBuffer);
    OsalMemFree(codecBuffer);
    return HDF_SUCCESS;
}

int32_t ComponentNode::FreeBuffer(uint32_t portIndex, const OmxCodecBuffer &buffer)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ = %{public}p", __func__, comp_);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGW("%{public}s is not support!", __func__);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ComponentNode::EmptyThisBuffer(const OmxCodecBuffer &buffer)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ = %{public}p", __func__, comp_);
        return HDF_ERR_INVALID_PARAM;
    }
    if (buffer.bufferId >= bufferId_) {
        HDF_LOGE("%{public}s error, bufferId = %{public}d is invalid.", __func__, buffer.bufferId);
        return HDF_ERR_INVALID_PARAM;
    }

    CodecBuffer *codecBuffer = (CodecBuffer *)OsalMemCalloc(sizeof(CodecBuffer) + sizeof(BufferInfo));
    if (codecBuffer == nullptr) {
        HDF_LOGE("%{public}s error, codecBuffer is nullptr", __func__);
        return HDF_FAILURE;
    }
    ConvertOmxCodecBufferToCodecBuffer(buffer, *codecBuffer);
    int32_t err = CodecQueueInput(comp_, codecBuffer, 0, buffer.fenceFd);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("%{public}s error, CodecQueueInput failed", __func__);
        OsalMemFree(codecBuffer);
        return err;
    }
    OsalMemFree(codecBuffer);

    return err;
}

int32_t ComponentNode::FillThisBuffer(const OmxCodecBuffer &buffer)
{
    if (comp_ == nullptr) {
        HDF_LOGE("%{public}s error, comp_ = %{public}p", __func__, comp_);
        return HDF_ERR_INVALID_PARAM;
    }
    if (buffer.bufferId >= bufferId_) {
        HDF_LOGE("%{public}s error, bufferId = %{public}d is invalid.", __func__, buffer.bufferId);
        return HDF_ERR_INVALID_PARAM;
    }

    CodecBuffer *codecBuffer = (CodecBuffer *)OsalMemCalloc(sizeof(CodecBuffer) + sizeof(BufferInfo));
    if (codecBuffer == nullptr) {
        HDF_LOGE("%{public}s error, codecBuffer is nullptr", __func__);
        return HDF_FAILURE;
    }
    ConvertOmxCodecBufferToCodecBuffer(buffer, *codecBuffer);
    int32_t err = CodecQueueOutput(comp_, codecBuffer, 0, buffer.fenceFd);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("%{public}s error, CodecQueueOutput failed", __func__);
    }
    OsalMemFree(codecBuffer);

    return err;
}

int32_t ComponentNode::SetPortMode(uint32_t portIndex, OmxCodecBuffer &buffer, AllocateBufferMode mode)
{
    DirectionType direct = INPUT_TYPE;
    if (portIndex == INPUT_PORTINDEX) {
        inputMode_ = mode;
    } else if (portIndex == OUTPUT_PORTINDEX) {
        direct = OUTPUT_TYPE;
        outputMode_ = mode;
    }
    BufferType type;
    int32_t err = ConvertOmxBufferTypeToBufferType(buffer.bufferType, type);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("%{public}s error, ConvertOmxBufferTypeToBufferType failed", __func__);
        return err;
    }

    err = CodecSetPortMode(comp_, direct, mode, type);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("%{public}s error, CodecSetPortMode failed", __func__);
        return err;
    }
    return err;
}

int32_t ComponentNode::ChangeComponentState(uint32_t param)
{
    int32_t err;
    switch (param) {
        case OMX_StateInvalid:
        case OMX_StateLoaded:
        case OMX_StateIdle:
        case OMX_StateWaitForResources: {
            err = SetState((OMX_STATETYPE)param);
            break;
        }
        case OMX_StateExecuting: {
            err = CodecStart(comp_);
            if (err == HDF_SUCCESS) {
                err = SetState((OMX_STATETYPE)param);
            }
            break;
        }
        case OMX_StatePause: {
            err = CodecStop(comp_);
            if (err == HDF_SUCCESS) {
                err = SetState((OMX_STATETYPE)param);
            }
            break;
        }

        default: {
            HDF_LOGW("%{public}s warn, unsupport state[%{public}d]", __func__, param);
            err = HDF_ERR_NOT_SUPPORT;
            break;
        }
    }

    if (err == HDF_SUCCESS) {
        if (omxCallback_->EventHandler != nullptr) {
            OMX_EVENTTYPE omxEvent;
            EventInfo info = {0};
            info.appData = appData_;
            info.eventData = nullptr;
            info.eventDataLen = 0;
            omxEvent = OMX_EventCmdComplete;
            info.data1 = OMX_CommandStateSet;
            info.data2 = param;
            omxCallback_->EventHandler(omxCallback_, omxEvent, &info);
        }
    } else {
        HDF_LOGE("%{public}s error, state = %{public}d", __func__, param);
    }
    return err;
}

int32_t ComponentNode::FlushComponent(uint32_t param)
{
    DirectionType directType = (DirectionType)param;
    return CodecFlush(comp_, directType);
}
}  // namespace CodecAdapter
}  // namespace Codec
}  // namespace OHOS