/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef RINGTONE_TYPE_H
#define RINGTONE_TYPE_H

namespace OHOS {
namespace Media {

enum ToneType : int32_t {
    TONE_TYPE_INVALID = 0,
    TONE_TYPE_ALARM,
    TONE_TYPE_RINGTONE,
    TONE_TYPE_NOTIFICATION,
    TONE_TYPE_MAX,
};

enum SourceType : int32_t {
    SOURCE_TYPE_INVALID = 0,
    SOURCE_TYPE_PRESET,  // system preset
    SOURCE_TYPE_CUSTOMISED,  // user customised
    SOURCE_TYPE_MAX,
};

enum ResultSetDataType : int32_t {
    TYPE_NULL = 0,
    TYPE_STRING,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_DOUBLE
};

enum MediaType : int32_t {
    MEDIA_TYPE_INVALID = 0,
    MEDIA_TYPE_AUDIO = 2,
    MEDIA_TYPE_MAX
};

enum ToneSettingType : int32_t {
    TONE_SETTING_TYPE_ALARM = 0,
    TONE_SETTING_TYPE_SHOT,
    TONE_SETTING_TYPE_RINGTONE,
    TONE_SETTING_TYPE_NOTIFICATION,
    TONE_SETTING_TYPE_MAX
};

enum ShotToneType : int32_t {
    SHOT_TONE_TYPE_NOT = 0, // not set
    SHOT_TONE_TYPE_SIM_CARD_1, // SimCard 1
    SHOT_TONE_TYPE_SIM_CARD_2, // SimCard 2
    SHOT_TONE_TYPE_SIM_CARD_BOTH, // both 1 and 2
    SHOT_TONE_TYPE_MAX
};

enum NotificationToneType : int32_t {
    NOTIFICATION_TONE_TYPE_NOT = 0, // not set
    NOTIFICATION_TONE_TYPE, // notification tone
    NOTIFICATION_TONE_TYPE_MAX
};

enum RingToneType : int32_t {
    RING_TONE_TYPE_NOT = 0, // not set
    RING_TONE_TYPE_SIM_CARD_1, // SimCard 1
    RING_TONE_TYPE_SIM_CARD_2, // SimCard 2
    RING_TONE_TYPE_SIM_CARD_BOTH, // both 1 and 2
    RING_TONE_TYPE_MAX
};

enum AlarmToneType : int32_t {
    ALARM_TONE_TYPE_NOT = 0, // not set
    ALARM_TONE_TYPE_SIM_CARD_1, // SimCard 1
    ALARM_TONE_TYPE_MAX
};

const char SLASH_CHAR = '/';
constexpr int32_t DEFAULT_INT32 = 0;
constexpr int64_t DEFAULT_INT64 = 0;
const std::string DEFAULT_STR = "";

/** Supported ringtone types */
const std::string RINGTONE_CONTAINER_TYPE_3GA   = "3ga";
const std::string RINGTONE_CONTAINER_TYPE_AC3   = "ac3";
const std::string RINGTONE_CONTAINER_TYPE_A52   = "a52";
const std::string RINGTONE_CONTAINER_TYPE_AMR   = "amr";
const std::string RINGTONE_CONTAINER_TYPE_IMY   = "imy";
const std::string RINGTONE_CONTAINER_TYPE_RTTTL = "rtttl";
const std::string RINGTONE_CONTAINER_TYPE_XMF   = "xmf";
const std::string RINGTONE_CONTAINER_TYPE_RTX   = "rtx";
const std::string RINGTONE_CONTAINER_TYPE_MXMF  = "mxmf";
const std::string RINGTONE_CONTAINER_TYPE_M4A   = "m4a";
const std::string RINGTONE_CONTAINER_TYPE_M4B   = "m4b";
const std::string RINGTONE_CONTAINER_TYPE_M4P   = "m4p";
const std::string RINGTONE_CONTAINER_TYPE_F4A   = "f4a";
const std::string RINGTONE_CONTAINER_TYPE_F4B   = "f4b";
const std::string RINGTONE_CONTAINER_TYPE_F4P   = "f4p";
const std::string RINGTONE_CONTAINER_TYPE_M3U   = "m3u";
const std::string RINGTONE_CONTAINER_TYPE_SMF   = "smf";
const std::string RINGTONE_CONTAINER_TYPE_MKA   = "mka";
const std::string RINGTONE_CONTAINER_TYPE_RA    = "ra";
const std::string RINGTONE_CONTAINER_TYPE_MP3   = "mp3";
const std::string RINGTONE_CONTAINER_TYPE_AAC   = "aac";
const std::string RINGTONE_CONTAINER_TYPE_ADTS  = "adts";
const std::string RINGTONE_CONTAINER_TYPE_ADT   = "adt";
const std::string RINGTONE_CONTAINER_TYPE_SND   = "snd";
const std::string RINGTONE_CONTAINER_TYPE_FLAC  = "flac";
const std::string RINGTONE_CONTAINER_TYPE_MP2   = "mp2";
const std::string RINGTONE_CONTAINER_TYPE_MP1   = "mp1";
const std::string RINGTONE_CONTAINER_TYPE_MPA   = "mpa";
const std::string RINGTONE_CONTAINER_TYPE_M4R   = "m4r";
const std::string RINGTONE_CONTAINER_TYPE_WAV   = "wav";
const std::string RINGTONE_CONTAINER_TYPE_OGG   = "ogg";
} // namespace Media
} // namespace OHOS

#endif  // RINGTONE_TYPE_H