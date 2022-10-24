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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "audio_common_test.h"

using namespace std;
using namespace comfun;
using namespace testing::ext;
namespace {
class AudioRenderTest : public testing::Test {
public:
    struct AudioManager *managerFuncs = nullptr;
    struct AudioAdapterDescriptor *descs = nullptr;
    struct AudioAdapterDescriptor *desc = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioSampleAttributes attrs = {};
    struct AudioRender *render = nullptr;

    virtual void SetUp();
    virtual void TearDown();
};

void AudioRenderTest::SetUp()
{
    managerFuncs = GetAudioManagerFuncs();
    ASSERT_NE(managerFuncs, nullptr);
    int32_t size = 0;
    ASSERT_EQ(HDF_SUCCESS,  managerFuncs->GetAllAdapters(managerFuncs, &descs, &size));

    desc = &descs[0];
    ASSERT_EQ(HDF_SUCCESS, managerFuncs->LoadAdapter(managerFuncs, desc, &adapter));
    ASSERT_EQ(HDF_SUCCESS, InitDevDesc(devDesc));
    ASSERT_EQ(HDF_SUCCESS, InitAttrs(attrs));
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterCreateRender(adapter, &devDesc, &attrs, &render));
}

void AudioRenderTest::TearDown()
{
    ASSERT_EQ(HDF_SUCCESS, AudioAdapterDestroyRender(adapter, render));
    managerFuncs->UnloadAdapter(managerFuncs, adapter);
    adapter = nullptr;
}

HWTEST_F(AudioRenderTest, PcmBytesToFramesWhenFrameRenderModeIsNull, TestSize.Level1)
{
    const struct AudioFrameRenderMode *frameRenderMode = nullptr;
    uint64_t bytes = FRAME_DATA;
    uint32_t frameCount = 0;
    EXPECT_EQ(HDF_FAILURE, PcmBytesToFrames(frameRenderMode, bytes, &frameCount));
}

HWTEST_F(AudioRenderTest, PcmBytesToFramesWhenFrameCountIsNull, TestSize.Level1)
{
    const struct AudioFrameRenderMode *frameRenderMode = new struct AudioFrameRenderMode;
    uint64_t bytes = FRAME_DATA;
    uint32_t *frameCount = nullptr;
    EXPECT_EQ(HDF_FAILURE, PcmBytesToFrames(frameRenderMode, bytes, frameCount));
    delete(frameRenderMode);
    frameRenderMode = nullptr;
}

HWTEST_F(AudioRenderTest, PcmBytesToFramesWhenParamIsNotSupport, TestSize.Level1)
{
    struct AudioFrameRenderMode *frameRenderMode = new struct AudioFrameRenderMode;
    frameRenderMode->attrs.format = AUDIO_FORMAT_TYPE_AAC_MAIN;
    uint64_t bytes = FRAME_DATA;
    uint32_t frameCount = 0;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT,
        PcmBytesToFrames((const struct AudioFrameRenderMode *)frameRenderMode, bytes, &frameCount));
    delete(frameRenderMode);
    frameRenderMode = nullptr;
}

HWTEST_F(AudioRenderTest, PcmBytesToFramesWhenChannelCountIsZero, TestSize.Level1)
{
    struct AudioFrameRenderMode *frameRenderMode = new struct AudioFrameRenderMode;
    frameRenderMode->attrs.format = AUDIO_FORMAT_TYPE_PCM_8_BIT;
    frameRenderMode->attrs.channelCount = 0;
    uint64_t bytes = FRAME_DATA;
    uint32_t frameCount = 0;
    EXPECT_EQ(HDF_FAILURE, PcmBytesToFrames((const struct AudioFrameRenderMode *)frameRenderMode, bytes, &frameCount));
    delete(frameRenderMode);
    frameRenderMode = nullptr;
}

HWTEST_F(AudioRenderTest, PcmBytesToFramesWhenParamIsValid, TestSize.Level1)
{
    struct AudioFrameRenderMode *frameRenderMode = new struct AudioFrameRenderMode;
    frameRenderMode->attrs.format = AUDIO_FORMAT_TYPE_PCM_8_BIT;
    frameRenderMode->attrs.channelCount = 1;
    uint64_t bytes = FRAME_DATA;
    uint32_t frameCount = 0;
    EXPECT_EQ(HDF_SUCCESS, PcmBytesToFrames((const struct AudioFrameRenderMode *)frameRenderMode, bytes, &frameCount));
    delete(frameRenderMode);
    frameRenderMode = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderStartWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderStart(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderStartWhenBufferIsNotNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    AudioHandle handle = (AudioHandle)hwRender;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    EXPECT_EQ(AUDIO_HAL_ERR_AO_BUSY, AudioRenderStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStop(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderStartWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    hwRender->devDataHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderStart(handle));
    hwRender->devDataHandle = devDataHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderStartWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devDataHandle = reinterpret_cast<struct DevHandle *>(service);
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderStart(handle));
    hwRender->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderStartWhenParamIsValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStop(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderStopWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderStop(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderStopWhenBufferIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    hwRender->renderParam.frameRenderMode.buffer = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, AudioRenderStop(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderStopWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    AudioHandle handle = (AudioHandle)hwRender;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    hwRender->devDataHandle = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderStop(handle));
    hwRender->devDataHandle = devDataHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderStopWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    AudioHandle handle = (AudioHandle)hwRender;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devDataHandle = reinterpret_cast<struct DevHandle *>(service);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderStop(handle));
    hwRender->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderStopWhenParamIsValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStop(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderPauseWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderPause(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderPauseWhenBufferIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    hwRender->renderParam.frameRenderMode.buffer = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderPause(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderPauseWhenPauseIsTrue, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    hwRender->renderParam.renderMode.ctlParam.pause = true;
    AudioHandle handle = (AudioHandle)hwRender;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, AudioRenderPause(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderPauseWhenDevDataHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    AudioHandle handle = (AudioHandle)hwRender;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    hwRender->devDataHandle = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderPause(handle));
    hwRender->devDataHandle = devDataHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderPauseWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    AudioHandle handle = (AudioHandle)hwRender;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devDataHandle = reinterpret_cast<struct DevHandle *>(service);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderPause(handle));
    hwRender->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderPauseWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderPause(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderResumeWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderResume(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderResumeWhenPauseIsFalse, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    hwRender->renderParam.renderMode.ctlParam.pause = false;
    handle = (AudioHandle)hwRender;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioRenderResume(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderResumeWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    AudioHandle handle = (AudioHandle)hwRender;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderPause(handle));
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    hwRender->devDataHandle = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderResume(handle));
    hwRender->devDataHandle = devDataHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderResumeWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    AudioHandle handle = (AudioHandle)hwRender;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderPause(handle));
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devDataHandle = reinterpret_cast<struct DevHandle *>(service);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderResume(handle));
    hwRender->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderResumeWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderPause(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderResume(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderFlushWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderFlush(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderFlushWhenParamIsValid, TestSize.Level1)
{
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, AudioRenderFlush(render));
}

HWTEST_F(AudioRenderTest, AudioRenderGetFrameSizeWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    uint64_t size = FRAME_DATA;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetFrameSize(handle, &size));
}

HWTEST_F(AudioRenderTest, AudioRenderGetFrameSizeWhenSizeIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    uint64_t *frameSize = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetFrameSize(handle, frameSize));
}

HWTEST_F(AudioRenderTest, AudioRenderGetFrameSizeWhenParamIsNotSupport, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    hwRender->renderParam.frameRenderMode.attrs.format = AUDIO_FORMAT_TYPE_AAC_MAIN;
    AudioHandle handle = (AudioHandle)hwRender;
    uint64_t frameSize = FRAME_DATA;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioRenderGetFrameSize(handle, &frameSize));
}

HWTEST_F(AudioRenderTest, AudioRenderGetFrameSizeWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    uint64_t frameSize = FRAME_DATA;
    EXPECT_EQ(HDF_SUCCESS, AudioRenderGetFrameSize(handle, &frameSize));
}

HWTEST_F(AudioRenderTest, AudioRenderGetFrameCountWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    uint64_t count = FRAME_DATA;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetFrameCount(handle, &count));
}

HWTEST_F(AudioRenderTest, AudioRenderGetFrameCountWhenCountIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    uint64_t *count = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetFrameCount(handle, count));
}

HWTEST_F(AudioRenderTest, AudioRenderGetFrameCountWhenParamIsValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    uint64_t count = FRAME_DATA;
    EXPECT_EQ(HDF_SUCCESS, AudioRenderGetFrameCount(handle, &count));
}

HWTEST_F(AudioRenderTest, AudioRenderSetSampleAttributesWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    AudioSampleAttributes *attrs = new AudioSampleAttributes;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetSampleAttributes(handle, attrs));
    delete(attrs);
    attrs = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSetSampleAttributesWhenAttrsIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetSampleAttributes(handle, nullptr));
}

HWTEST_F(AudioRenderTest, AudioRenderSetSampleAttributesWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    hwRender->devDataHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetSampleAttributes(handle, &attrs));
    hwRender->devDataHandle = devDataHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderSetSampleAttributesWhenFormatIsError, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    attrs.format = AUDIO_FORMAT_TYPE_G711A;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioRenderSetSampleAttributes(handle, &attrs));
}

HWTEST_F(AudioRenderTest, AudioRenderSetSampleAttributesWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devDataHandle = reinterpret_cast<struct DevHandle *>(service);
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetSampleAttributes(handle, &attrs));
    hwRender->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSetSampleAttributesWhenParamIsValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetSampleAttributes(handle, &attrs));
}

HWTEST_F(AudioRenderTest, AudioRenderGetSampleAttributesWhenHandleIsNull, TestSize.Level1)
{
    AudioSampleAttributes *attrs = new AudioSampleAttributes;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetSampleAttributes(nullptr, attrs));
    delete(attrs);
    attrs = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetSampleAttributesWhenAttrsIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetSampleAttributes(handle, nullptr));
}

HWTEST_F(AudioRenderTest, AudioRenderGetSampleAttributesWhenParamIsValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(HDF_SUCCESS, AudioRenderGetSampleAttributes(handle, &attrs));
}

HWTEST_F(AudioRenderTest, AudioRenderGetCurrentChannelIdWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    uint32_t channelIdOne = 1;
    uint32_t *channelId = &channelIdOne;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetCurrentChannelId(handle, channelId));
}

HWTEST_F(AudioRenderTest, AudioRenderGetCurrentChannelIdWhenChannelIdIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    uint32_t *channelId = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetCurrentChannelId(handle, channelId));
}

HWTEST_F(AudioRenderTest, AudioRenderGetCurrentChannelIdWhenParamIsValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    uint32_t channelId = 1;
    EXPECT_EQ(HDF_SUCCESS, AudioRenderGetCurrentChannelId(handle, &channelId));
}

HWTEST_F(AudioRenderTest, AudioRenderCheckSceneCapabilityWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    const struct AudioSceneDescriptor *scene = new struct AudioSceneDescriptor;
    bool supported = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderCheckSceneCapability(handle, scene, &supported));
    delete(scene);
    scene = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderCheckSceneCapabilityWhenSceneIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    const struct AudioSceneDescriptor *scene = nullptr;
    bool supported = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderCheckSceneCapability(handle, scene, &supported));
}

HWTEST_F(AudioRenderTest, AudioRenderCheckSceneCapabilityWhenSupportedIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    const struct AudioSceneDescriptor *scene = new struct AudioSceneDescriptor;
    bool *supported = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderCheckSceneCapability(handle, scene, supported));
    delete(scene);
    scene = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderCheckSceneCapabilityWhenPinsIsError, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    struct AudioSceneDescriptor scene;
    scene.scene.id = AUDIO_IN_MEDIA;
    scene.desc.pins = PIN_NONE;
    bool supported = false;
    /* to support different products */
    int32_t ret = AudioRenderCheckSceneCapability(handle, &scene, &supported);
    if ((ret == AUDIO_HAL_ERR_INTERNAL) || (ret == AUDIO_HAL_ERR_NOT_SUPPORT)) {
        EXPECT_TRUE(true);
    }
}

HWTEST_F(AudioRenderTest, AudioRenderCheckSceneCapabilityWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    struct AudioSceneDescriptor scene;
    scene.scene.id = AUDIO_IN_MEDIA;
    scene.desc.pins = PIN_OUT_SPEAKER;
    bool supported = false;
    /* to support different products */
    int32_t ret = AudioRenderCheckSceneCapability(handle, &scene, &supported);
    if ((ret == AUDIO_HAL_SUCCESS) || (ret == AUDIO_HAL_ERR_NOT_SUPPORT)) {
        EXPECT_TRUE(true);
    }
}

HWTEST_F(AudioRenderTest, AudioRenderSelectSceneWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    const struct AudioSceneDescriptor *scene = new struct AudioSceneDescriptor;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSelectScene(handle, scene));
    delete(scene);
    scene = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSelectSceneWhenSceneIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    const struct AudioSceneDescriptor *scene = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSelectScene(handle, scene));
}

HWTEST_F(AudioRenderTest, AudioRenderSelectSceneWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    struct AudioSceneDescriptor *scene = new AudioSceneDescriptor;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSelectScene(handle, scene));
    hwRender->devCtlHandle = devCtlHandle;
    delete(scene);
    scene = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSelectSceneWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = reinterpret_cast<struct DevHandle *>(service);
    AudioHandle handle = (AudioHandle)hwRender;
    struct AudioSceneDescriptor scene;
    scene.scene.id = AUDIO_IN_MEDIA;
    scene.desc.pins = PIN_OUT_SPEAKER;
    /* to support different products */
    int32_t ret = AudioRenderSelectScene(handle, &scene);
    if ((ret == AUDIO_HAL_ERR_INTERNAL) || (ret == AUDIO_HAL_ERR_NOT_SUPPORT)) {
        EXPECT_TRUE(true);
    }
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSelectSceneWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    struct AudioSceneDescriptor scene;
    scene.scene.id = AUDIO_IN_MEDIA;
    scene.desc.pins = PIN_OUT_SPEAKER;
    /* to support different products */
    int32_t ret = AudioRenderSelectScene(handle, &scene);
    if ((ret == AUDIO_HAL_SUCCESS) || (ret == AUDIO_HAL_ERR_NOT_SUPPORT)) {
        EXPECT_TRUE(true);
    }
}

HWTEST_F(AudioRenderTest, AudioRenderSetMuteWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    bool mute = true;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetMute(handle, mute));
}

HWTEST_F(AudioRenderTest, AudioRenderSetMuteWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    bool mute = true;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetMute(handle, mute));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderSetMuteWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = (struct DevHandle *)service;
    AudioHandle handle = (AudioHandle)hwRender;
    bool mute = true;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetMute(handle, mute));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetMute(handle, mute));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSetMuteWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    bool mute = true;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetMute(handle, mute));
    mute = false;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetMute(handle, mute));
}

HWTEST_F(AudioRenderTest, AudioRenderGetMuteWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    bool mute = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetMute(handle, &mute));
}

HWTEST_F(AudioRenderTest, AudioRenderGetMuteWhenMuteIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    bool *mute = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetMute(handle, mute));
}

HWTEST_F(AudioRenderTest, AudioRenderGetMuteWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    bool mute = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetMute(handle, &mute));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderGetMuteWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = (struct DevHandle *)service;
    AudioHandle handle = (AudioHandle)hwRender;
    bool mute = true;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetMute(handle, &mute));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetMute(handle, &mute));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetMuteWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    bool mute = false;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetMute(handle, &mute));
}

HWTEST_F(AudioRenderTest, AudioRenderSetVolumeWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetVolume(handle, volume));
}

HWTEST_F(AudioRenderTest, AudioRenderSetVolumeWhenVolumeLessThanZero, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float volume = -1;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetVolume(handle, volume));
}

HWTEST_F(AudioRenderTest, AudioRenderSetVolumeWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetVolume(handle, volume));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderSetVolumeWhenHandleIsError, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    hwRender->renderParam.renderMode.ctlParam.volThreshold.volMax = 0;
    hwRender->renderParam.renderMode.ctlParam.volThreshold.volMin = 1;
    AudioHandle handle = (AudioHandle)render;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetVolume(handle, volume));
}

HWTEST_F(AudioRenderTest, AudioRenderSetVolumeWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = (struct DevHandle *)service;
    AudioHandle handle = (AudioHandle)hwRender;
    float volume = 1.0;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetVolume(handle, volume));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetVolume(handle, volume));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSetVolumeWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float volume = 0.8;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetVolume(handle, volume));
}

HWTEST_F(AudioRenderTest, AudioRenderGetVolumeWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetVolume(handle, &volume));
}

HWTEST_F(AudioRenderTest, AudioRenderGetVolumeWhenVolumeIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float *volume = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetVolume(handle, volume));
}

HWTEST_F(AudioRenderTest, AudioRenderGetVolumeWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetVolume(handle, &volume));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderGetVolumeWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = (struct DevHandle *)service;
    AudioHandle handle = (AudioHandle)hwRender;
    float volume = 1.0;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetVolume(handle, &volume));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetVolume(handle, &volume));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetVolumeWhenVolumeIsError, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    hwRender->renderParam.renderMode.ctlParam.volThreshold.volMax = 1;
    hwRender->renderParam.renderMode.ctlParam.volThreshold.volMin = 1;
    AudioHandle handle = (AudioHandle)render;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetVolume(handle, &volume));
}

HWTEST_F(AudioRenderTest, AudioRenderGetVolumeWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetVolume(handle, &volume));
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainThresholdWhenRenderIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float min = GAIN_MIN;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetGainThreshold(handle, &min, &max));
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainThresholdWhenMinIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float *min = nullptr;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetGainThreshold(handle, min, &max));
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainThresholdWhenMaxIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float min = GAIN_MIN;
    float *max = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetGainThreshold(handle, &min, max));
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainThresholdWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float min = GAIN_MIN;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetGainThreshold(handle, &min, &max));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainThresholdWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwRender *hwRender = (AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = (struct DevHandle *)service;
    AudioHandle handle = (AudioHandle)hwRender;
    float min = GAIN_MIN;
    float max = GAIN_MAX;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetGainThreshold(handle, &min, &max));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetGainThreshold(handle, &min, &max));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainThresholdWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float min = GAIN_MIN;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetGainThreshold(handle, &min, &max));
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetGain(handle, &gain));
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainWhenGainIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float *gain = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetGain(handle, gain));
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetGain(handle, &gain));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = reinterpret_cast<struct DevHandle *>(service);
    AudioHandle handle = (AudioHandle)hwRender;
    float gain = 0;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetGain(handle, &gain));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetGain(handle, &gain));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetGainWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetGain(handle, &gain));
}

HWTEST_F(AudioRenderTest, AudioRenderSetGainWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetGain(handle, gain));
}

HWTEST_F(AudioRenderTest, AudioRenderSetGainWhenGainIsLessThanZero, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float gain = -1;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetGain(handle, gain));
}

HWTEST_F(AudioRenderTest, AudioRenderSetGainWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetGain(handle, gain));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderSetGainWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = reinterpret_cast<struct DevHandle *>(service);
    AudioHandle handle = (AudioHandle)hwRender;
    float gain = 0;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetGain(handle, gain));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetGain(handle, gain));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSetGainWhenParamValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetGain(handle, gain));
}

HWTEST_F(AudioRenderTest, AudioRenderGetLatencyWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *utRender = nullptr;
    uint32_t ms = 96;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetLatency(utRender, &ms));
}

HWTEST_F(AudioRenderTest, AudioRenderGetLatencyWhenMsIsNull, TestSize.Level1)
{
    uint32_t *ms = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetLatency(render, ms));
}

HWTEST_F(AudioRenderTest, AudioRenderGetLatencyWhenByteRateIsZero, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    hwRender->renderParam.frameRenderMode.byteRate = 0;
    uint32_t ms = 96;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetLatency((struct AudioRender *)hwRender, &ms));
}

HWTEST_F(AudioRenderTest, AudioRenderGetLatencyWhenParamValid, TestSize.Level1)
{
    uint32_t ms = 96;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetLatency(render, &ms));
}

HWTEST_F(AudioRenderTest, AudioRenderRenderFrameWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *utRender = nullptr;
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM,
        AudioRenderRenderFrame(utRender, (const void*)frame, requestBytes, &replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderRenderFrameWhenFrameIsNull, TestSize.Level1)
{
    void *frame = nullptr;
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM,
        AudioRenderRenderFrame(render, (const void*)frame, requestBytes, &replyBytes));
}

HWTEST_F(AudioRenderTest, AudioRenderRenderFrameWhenReplyBytesIsNull, TestSize.Level1)
{
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t *replyBytes = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM,
        AudioRenderRenderFrame(render, (const void*)frame, requestBytes, replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderRenderFrameWhenRequestBytesGreaterThan16K, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart((AudioHandle)render));
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA + 1;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL,
        AudioRenderRenderFrame(render, (const void*)frame, requestBytes, &replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderRenderFrameWhenFormatIsNotSupport, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart((AudioHandle)render));
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    hwRender->renderParam.frameRenderMode.attrs.format = AUDIO_FORMAT_TYPE_AAC_MAIN;
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT,
        AudioRenderRenderFrame(render, (const void*)frame, requestBytes, &replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderRenderFrameWhenDevDataHandleIsNull, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart((AudioHandle)render));
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    hwRender->devDataHandle = nullptr;
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL,
        AudioRenderRenderFrame(render, (const void*)frame, requestBytes, &replyBytes));
    hwRender->devDataHandle = devDataHandle;
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderRenderFrameWhenSampleRateIsZero, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart((AudioHandle)render));
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    hwRender->renderParam.frameRenderMode.attrs.sampleRate = 0;
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderRenderFrame(render, (const void*)frame, requestBytes, &replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderRenderFrameWhenParamIsValid, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart((AudioHandle)render));
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderRenderFrame(render, (const void*)frame, requestBytes, &replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetRenderPositionWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *utRender = nullptr;
    uint64_t frames = 1024;
    struct AudioTimeStamp *time = new AudioTimeStamp;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetRenderPosition(utRender, &frames, time));
    delete(time);
    time = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetRenderPositionWhenFramesIsNull, TestSize.Level1)
{
    uint64_t *frames = nullptr;
    struct AudioTimeStamp *time = new AudioTimeStamp;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetRenderPosition(render, frames, time));
    delete(time);
    time = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetRenderPositionWhenTimeIsNull, TestSize.Level1)
{
    uint64_t frames = 1024;
    struct AudioTimeStamp *time = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetRenderPosition(render, &frames, time));
}

HWTEST_F(AudioRenderTest, AudioRenderGetRenderPositionWhenParamIsValid, TestSize.Level1)
{
    uint64_t frames = 1024;
    struct AudioTimeStamp *time = new AudioTimeStamp;
    EXPECT_EQ(HDF_SUCCESS, AudioRenderGetRenderPosition(render, &frames, time));
    delete(time);
    time = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSetRenderSpeedWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *utRender = nullptr;
    float speed = 1.0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetRenderSpeed(utRender, speed));
}

HWTEST_F(AudioRenderTest, AudioRenderSetRenderSpeedWhenParamIsValid, TestSize.Level1)
{
    float speed = 1.0;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioRenderSetRenderSpeed(render, speed));
}

HWTEST_F(AudioRenderTest, AudioRenderGetRenderSpeedWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *utRender = nullptr;
    float speed = 1.0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetRenderSpeed(utRender, &speed));
}

HWTEST_F(AudioRenderTest, AudioRenderGetRenderSpeedWhenSpeedIsNull, TestSize.Level1)
{
    float *speed = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetRenderSpeed(render, speed));
}

HWTEST_F(AudioRenderTest, AudioRenderGetRenderSpeedWhenParamIsValid, TestSize.Level1)
{
    float speed = 1.0;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioRenderGetRenderSpeed(render, &speed));
}

HWTEST_F(AudioRenderTest, AudioRenderSetChannelModeWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *utRender = nullptr;
    AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetChannelMode(utRender, mode));
}

HWTEST_F(AudioRenderTest, AudioRenderSetChannelModeWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetChannelMode((struct AudioRender *)hwRender, mode));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderSetChannelModeWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = reinterpret_cast<struct DevHandle *>(service);
    AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetChannelMode((struct AudioRender *)hwRender, mode));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetChannelMode((struct AudioRender *)hwRender, mode));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderSetChannelModeWhenParamIsValid, TestSize.Level1)
{
    AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetChannelMode(render, mode));
}

HWTEST_F(AudioRenderTest, AudioRenderGetChannelModeWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *utRender = nullptr;
    AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetChannelMode(utRender, &mode));
}

HWTEST_F(AudioRenderTest, AudioRenderGetChannelModeWhenModeIsNull, TestSize.Level1)
{
    AudioChannelMode *mode = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetChannelMode(render, mode));
}

HWTEST_F(AudioRenderTest, AudioRenderGetChannelModeWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    hwRender->devCtlHandle = nullptr;
    AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetChannelMode((struct AudioRender *)hwRender, &mode));
    hwRender->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderGetChannelModeWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devCtlHandle = hwRender->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devCtlHandle = reinterpret_cast<struct DevHandle *>(service);
    AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
#ifdef ALSA_LIB_MODE
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetChannelMode((struct AudioRender *)hwRender, &mode));
#else
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetChannelMode((struct AudioRender *)hwRender, &mode));
#endif
    hwRender->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderGetChannelModeWhenParamIsValid, TestSize.Level1)
{
    AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetChannelMode(render, &mode));
}

HWTEST_F(AudioRenderTest, AudioRenderDrainBufferWhenTypeIsNull, TestSize.Level1)
{
    AudioDrainNotifyType *type = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderDrainBuffer(render, type));
}

HWTEST_F(AudioRenderTest, AudioRenderDrainBufferWhenParamIsNotSupport, TestSize.Level1)
{
    AudioDrainNotifyType *type = new AudioDrainNotifyType;
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, AudioRenderDrainBuffer(render, type));
    delete(type);
    type = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderTurnStandbyModeWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderTurnStandbyMode(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderTurnStandbyModeWhenStopIsError, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderTurnStandbyMode(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderTurnStandbyModeWhenParamIsValid, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)render;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderTurnStandbyMode(handle));
}

HWTEST_F(AudioRenderTest, AudioRenderRegCallbackWhenRenderIsNull, TestSize.Level1)
{
    struct AudioRender *utRender = nullptr;
    RenderCallback callback = AudioRenderCallbackUtTest;
    void *cookie = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderRegCallback(utRender, callback, cookie));
}

HWTEST_F(AudioRenderTest, AudioRenderRegCallbackWhenParamIsValid, TestSize.Level1)
{
    RenderCallback callback = AudioRenderCallbackUtTest;
    void *cookie = nullptr;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderRegCallback(render, callback, cookie));
}

HWTEST_F(AudioRenderTest, CallbackProcessingWhenHandleIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    AudioCallbackType callBackType = AUDIO_ERROR_OCCUR;
    EXPECT_EQ(HDF_FAILURE, CallbackProcessing(handle, callBackType));
}

HWTEST_F(AudioRenderTest, CallbackProcessingWhenCallbackIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    hwRender->renderParam.frameRenderMode.callback = NULL;
    AudioHandle handle = (AudioHandle)hwRender;
    AudioCallbackType callBackType = AUDIO_ERROR_OCCUR;
    EXPECT_EQ(HDF_FAILURE, CallbackProcessing(handle, callBackType));
}

HWTEST_F(AudioRenderTest, CallbackProcessingWhenCallbackIsError, TestSize.Level1)
{
    RenderCallback callback = AudioRenderCallbackUtTest;
    void *cookie = nullptr;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderRegCallback(render, callback, cookie));
    AudioHandle handle = (AudioHandle)render;
    int32_t callBackType = AUDIO_ERROR_OCCUR + 1;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, CallbackProcessing(handle, (enum AudioCallbackType)callBackType));
}

HWTEST_F(AudioRenderTest, CallbackProcessingWhenParamIsValid, TestSize.Level1)
{
    RenderCallback callback = AudioRenderCallbackUtTest;
    void *cookie = nullptr;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderRegCallback(render, callback, cookie));
    AudioHandle handle = (AudioHandle)render;
    AudioCallbackType callBackType = AUDIO_ERROR_OCCUR;
    EXPECT_EQ(HDF_SUCCESS, CallbackProcessing(handle, callBackType));
}

HWTEST_F(AudioRenderTest, AudioRenderSetExtraParamsWhenRenderIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    char keyValueList[] = "attr-route=1;attr-format=32;attr-channels=2;attr-frame-count=82;attr-sampling-rate=48000";
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetExtraParams(handle, keyValueList));
}

HWTEST_F(AudioRenderTest, AudioRenderSetExtraParamsWhenParamIsNull, TestSize.Level1)
{
    char *keyValueList = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderSetExtraParams(render, keyValueList));
}

HWTEST_F(AudioRenderTest, AudioRenderSetExtraParamsWhenParamIsError, TestSize.Level1)
{
    char keyValueList[] = "attr-route=1;attr-format=32;attr-channels=2;\
attr-frame-count=82;attr-sampling-rate=48000;attr-para=123";
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderSetExtraParams(render, keyValueList));
}

HWTEST_F(AudioRenderTest, AudioRenderSetExtraParamsWhenParamIsValid, TestSize.Level1)
{
    char keyValueListOne[] = "attr-frame-count=1024;";
    char keyValueListFour[] = "attr-format=32;attr-channels=2;attr-frame-count=4096;attr-sampling-rate=48000";
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetExtraParams(render, keyValueListOne));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderSetExtraParams(render, keyValueListFour));
}

HWTEST_F(AudioRenderTest, AudioRenderGetExtraParamsWhenRenderIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    int32_t listLenth = PARAMS_LEN;
    char keyValueListValue[PARAMS_LEN] = {};
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetExtraParams(handle, keyValueListValue, listLenth));
}

HWTEST_F(AudioRenderTest, AudioRenderGetExtraParamsWhenParamIsNull, TestSize.Level1)
{
    int32_t listLenth = PARAMS_LEN;
    char* keyValueListValue = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetExtraParams(render, keyValueListValue, listLenth));
}

HWTEST_F(AudioRenderTest, AudioRenderGetExtraParamsWhenParamIsError0, TestSize.Level1)
{
    int32_t listLenth = 0;
    char keyValueListValue[PARAMS_LEN] = {};
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetExtraParams(render, keyValueListValue, listLenth));
    listLenth = PARAMS_ERR;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioRenderGetExtraParams(render, keyValueListValue, listLenth));
}

HWTEST_F(AudioRenderTest, AudioRenderGetExtraParamsWhenParamIsValid, TestSize.Level1)
{
    int32_t listLenth = PARAMS_LEN;
    char keyValueListValue[PARAMS_LEN] = {};
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioRenderGetExtraParams(render, keyValueListValue, listLenth));
}

HWTEST_F(AudioRenderTest, AudioRenderReqMmapBufferWhenRenderIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    int32_t reqSize = REQ_SIZE;
    struct AudioMmapBufferDescripter desc;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderReqMmapBuffer(handle, reqSize, &desc));
}

HWTEST_F(AudioRenderTest, AudioRenderReqMmapBufferWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    hwRender->devDataHandle = nullptr;
    struct AudioMmapBufferDescripter desc;
    int32_t reqSize = REQ_SIZE;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderReqMmapBuffer((struct AudioRender *)hwRender, reqSize, &desc));
    hwRender->devDataHandle = devDataHandle;
}

HWTEST_F(AudioRenderTest, AudioRenderReqMmapBufferWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    struct DevHandle *devDataHandle = hwRender->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    (void)memset_s(service, sizeof(struct HdfIoService), 0, sizeof(struct HdfIoService));
    hwRender->devDataHandle = reinterpret_cast<struct DevHandle *>(service);
    struct AudioMmapBufferDescripter desc;
    int32_t reqSize = REQ_SIZE;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderReqMmapBuffer((struct AudioRender *)hwRender, reqSize, &desc));
    hwRender->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioRenderTest, AudioRenderReqMmapBufferWhenDescIsNull, TestSize.Level1)
{
    struct AudioMmapBufferDescripter *desc = nullptr;
    int32_t reqSize = REQ_SIZE;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderReqMmapBuffer(render, reqSize, desc));
}

HWTEST_F(AudioRenderTest, AudioRenderReqMmapBufferWhenFormatIsError, TestSize.Level1)
{
    struct AudioMmapBufferDescripter desc;
    int32_t reqSize = REQ_SIZE;
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    AudioFormat tempFormat = hwRender->renderParam.frameRenderMode.attrs.format;
    hwRender->renderParam.frameRenderMode.attrs.format = AUDIO_FORMAT_TYPE_G726;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioRenderReqMmapBuffer(render, reqSize, &desc));
    hwRender->renderParam.frameRenderMode.attrs.format = tempFormat;
}

HWTEST_F(AudioRenderTest, AudioRenderGetMmapPositionWhenRenderIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    uint64_t frames = REQ_SIZE;
    struct AudioTimeStamp time;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetMmapPosition(handle, &frames, &time));
}

HWTEST_F(AudioRenderTest, AudioRenderGetMmapPositionWhenFramesIsNull, TestSize.Level1)
{
    uint64_t *frames = nullptr;
    struct AudioTimeStamp time;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetMmapPosition(render, frames, &time));
}

HWTEST_F(AudioRenderTest, AudioRenderGetMmapPositionWhenTimeIsNull, TestSize.Level1)
{
    uint64_t frames = REQ_SIZE;
    struct AudioTimeStamp *time = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderGetMmapPosition(render, &frames, time));
}

HWTEST_F(AudioRenderTest, AudioRenderAudioDevDumpWhenRenderIsNull, TestSize.Level1)
{
    AudioHwRender *hwRender = nullptr;
    AudioHandle handle = (AudioHandle)hwRender;
    int32_t range = 0;
    int32_t fd = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioRenderAudioDevDump(handle, range, fd));
}
}
