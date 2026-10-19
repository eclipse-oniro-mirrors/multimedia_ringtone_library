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

#ifndef RINGTONE_SETTING_MANAGER_H
#define RINGTONE_SETTING_MANAGER_H

#include <map>
#include <string>
#include <unordered_set>

#include "result_set.h"
#include "rdb_store.h"
#include "ringtone_metadata.h"
#include "ringtone_type.h"

namespace OHOS {
namespace Media {
#define EXPORT __attribute__ ((visibility ("default")))
class RingtoneSettingManager final {
public:
    EXPORT RingtoneSettingManager(std::shared_ptr<NativeRdb::RdbStore> rdb);
    EXPORT ~RingtoneSettingManager() = default;
    EXPORT int32_t CommitSetting(int32_t toneId, std::string &tonePath, int32_t settingType,
        int32_t toneType, int32_t sourceType);
    EXPORT void FlushSettings();
    EXPORT int32_t TravelQueryResultSet(std::string querySql,
        std::function<bool (std::shared_ptr<RingtoneMetadata> &)> func);
    EXPORT int32_t Update(int &changedRows, const NativeRdb::ValuesBucket &values,
        const NativeRdb::AbsRdbPredicates &predicates);
    EXPORT void SetForceFlush(bool forceFlush);
private:
    struct SettingItem {
        int32_t toneId = TONE_ID_DEFAULT;
        int32_t settingType = TONE_SETTING_TYPE_DEFAULT;
        int32_t toneType = TONE_TYPE_DEFAULT;
        int32_t sourceType = SOURCE_TYPE_DEFAULT;
    };
    EXPORT int32_t CommitSettingCompare(int32_t settingType, int32_t toneType, int32_t sourceType);
    EXPORT void TravelSettings(std::function<int32_t (std::string &, SettingItem &)> func);
    EXPORT int32_t PopulateMetadata(const std::shared_ptr<NativeRdb::ResultSet> &resultSet,
        std::unique_ptr<RingtoneMetadata> &metaData);
    EXPORT int32_t GetMetaDataFromResultSet(std::shared_ptr<NativeRdb::ResultSet> resultSet,
        std::vector<std::shared_ptr<RingtoneMetadata>> &metaDatas);
    EXPORT int32_t TryMergeExistingSetting(const std::string &tonePath, int32_t settingType,
        int32_t toneType, int32_t sourceType);
    EXPORT int32_t MergeCardToneType(SettingItem &item, int32_t toneType);
    EXPORT void ExtractMetaFromColumn(const std::shared_ptr<NativeRdb::ResultSet> &resultSet,
        std::unique_ptr<RingtoneMetadata> &metadata, const std::string &col);
    // 检查本机指定类型+卡位是否已有自定义(source_type=2)的记录
    EXPORT bool IsCardAlreadyCustomised(int32_t settingType, int32_t cardMask);
    // 通过tonePath找到local记录, 设置对应卡位bit + source_type
    EXPORT int32_t ApplyCardSetting(const std::string &tonePath, int32_t settingType,
        int32_t cardMask, int32_t sourceType);
    // 清理local DB中预置记录(source_type=1)的指定卡位bit
    EXPORT int32_t ClearPresetCardBit(int32_t settingType, int32_t cardMask);
    // 处理NOTIFICATION/ALARM: 设置tone_type + source_type
    EXPORT int32_t ApplyNonCardSetting(const std::string &tonePath, int32_t settingType,
        int32_t toneType, int32_t sourceType);
    // 逐卡位处理SHOT/RINGTONE设置
    EXPORT void ApplyCardSettings(const std::string &tonePath, const SettingItem &item);
    // 通过tonePath查询local DB中该记录的source_type
    EXPORT int32_t GetTargetSourceType(const std::string &tonePath);
    // FlushSettings开始前全量缓存本机SHOT/RINGTONE预置记录的卡位分布
    void CachePresetCardBits();
    // 查询指定类型的PRESET记录, 按tone_type含哪些卡位bit建立 cardMask->(toneId, tonePath) 映射
    void BuildPresetCardBitsCache(const std::string &typeColumn, const std::string &sourceColumn,
        std::map<int32_t, PresetCardBitEntry> &cache);
    // 判断tonePath是否是缓存中承担卡位X的预置记录的tonePath(特殊场景识别)
    bool IsPresetTonePath(int32_t settingType, int32_t cardMask, const std::string &tonePath);
    // 反查settings_和本机DB, 判断卡位X是否有tonePath之外的自定义记录
    bool HasOtherCustomisedForCard(const std::string &tonePath, int32_t settingType, int32_t cardMask);
private:
    // 预置卡位缓存项: FlushSettings开始时承担某卡位的预置记录toneId与tonePath
    struct PresetCardBitEntry {
        int32_t toneId = 0;
        std::string tonePath;
    };
    bool forceFlush_ = false;
    std::shared_ptr<NativeRdb::RdbStore> ringtoneRdb_ = nullptr;
    std::unordered_multimap<std::string, SettingItem> settings_ = {};
    // 预置卡位缓存: cardMask -> 预置记录(toneId + tonePath)，SHOT/RINGTONE分两个缓存
    // 用于ApplyCardSettings识别"自定义铃声tonePath与预置同名"的特殊场景
    std::map<int32_t, PresetCardBitEntry> presetShotCardBitsCache_;
    std::map<int32_t, PresetCardBitEntry> presetRingCardBitsCache_;
};
} // namespace Media
} // namespace OHOS

#endif // RINGTONE_SETTING_MANAGER_H
