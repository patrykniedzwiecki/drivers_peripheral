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

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Defines audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver adapter, and rendering and capturing audios.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file i_audio_manager.h
 *
 * @brief Declares APIs for audio adapter management and loading.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef I_AUDIO_MANAGER_H
#define I_AUDIO_MANAGER_H

#include "i_audio_types.h"
#include "i_audio_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Manages audio adapters through a specific adapter driver program loaded based on the given audio
 * adapter descriptor.
 *
 * @see AudioHwiAdapter
 * @since 1.0
 * @version 1.0
 */
struct AudioHwiManager {
    /**
     * @brief Obtains the list of all adapters supported by an audio driver.
     *
     * @param manager Indicates the pointer to the audio adapter manager to operate.
     * @param descs Indicates the double pointer to the audio adapter list.
     * @param size Indicates the pointer to the length of the list.
     * @return Returns <b>0</b> if the list is obtained successfully; returns a negative value otherwise.
     * @see LoadAdapter
     */
    int32_t (*GetAllAdapters)(struct AudioHwiManager *manager, struct AudioAdapterHwiDescriptor **descs, int32_t *size);

    /**
     * @brief Loads the driver for an audio adapter.
     *
     * For example, to load a USB driver, you may need to load a dynamic-link library (*.so) in specific implementation.
     *
     * @param manager Indicates the pointer to the audio adapter manager to operate.
     * @param desc Indicates the pointer to the descriptor of the audio adapter.
     * @param adapter Indicates the double pointer to the audio adapter.
     * @return Returns <b>0</b> if the driver is loaded successfully; returns a negative value otherwise.
     * @see GetAllAdapters
     * @see UnloadAdapter
     */
    int32_t (*LoadAdapter)(struct AudioHwiManager *manager, const struct AudioAdapterHwiDescriptor *desc,
                           struct AudioHwiAdapter **adapter);

    /**
     * @brief Unloads the driver of an audio adapter.
     *
     * @param manager Indicates the pointer to the audio adapter manager to operate.
     * @param adapter Indicates the pointer to the audio adapter whose driver will be unloaded.
     * @see LoadAdapter
     */
    void (*UnloadAdapter)(struct AudioHwiManager *manager, struct AudioHwiAdapter *adapter);

    /**
     * @brief Release the AudioHwiManager Object.
     *
     * @param object Indicates the pointer to the audio adapter manager to operate.
     * @return Returns <b>true</b> if the Object is released; returns <b>false</b> otherwise.
     */
    bool (*ReleaseAudioManagerObject)(struct AudioHwiManager *object);
};

/**
 * @brief Obtains the operation function list of the {@link AudioHwiManager} class.
 *
 * @return Returns the pointer to the <b>AudioHwiManager</b> object if the list is obtained; returns <b>NULL</b>
 * otherwise.
 */
struct AudioHwiManager *GetAudioManagerFuncs(void);

#ifdef __cplusplus
}
#endif

#endif /* I_AUDIO_MANAGER_H */
/** @} */