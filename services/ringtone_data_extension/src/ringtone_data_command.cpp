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

#include "ringtone_data_command.h"

#include "rdb_utils.h"
#include "ringtone_errno.h"
#include "ringtone_log.h"
#include "ringtone_utils.h"

namespace OHOS {
namespace Media {
using namespace std;
using namespace OHOS::NativeRdb;
using namespace OHOS::DataShare;

RingtoneDataCommand::RingtoneDataCommand(const Uri &uri, const string &table, const RingtoneOperationType type)
    : uri_(uri), tableName_(table), oprnType_(type)
{
    toneId_ = -1;
}

RingtoneDataCommand::~RingtoneDataCommand() {}

// set functions
void RingtoneDataCommand::SetDataSharePred(const DataShare::DataSharePredicates &pred)
{
    datasharePred_ = make_unique<const DataSharePredicates>(pred);
}

void RingtoneDataCommand::SetValueBucket(const NativeRdb::ValuesBucket &value)
{
    insertValue_ = value;
}

void RingtoneDataCommand::SetBundleName(const string &bundleName)
{
    bundleName_ = bundleName;
}

void RingtoneDataCommand::SetResult(const string &result)
{
    result_ = result;
}

// get functions
ValuesBucket &RingtoneDataCommand::GetValueBucket()
{
    return insertValue_;
}

AbsRdbPredicates *RingtoneDataCommand::GetAbsRdbPredicates()
{
    if (absRdbPredicates_ == nullptr) {
        absRdbPredicates_ = make_unique<RdbPredicates>(tableName_);
    }
    return absRdbPredicates_.get();
}

const string &RingtoneDataCommand::GetTableName()
{
    return tableName_;
}

const Uri &RingtoneDataCommand::GetUri() const
{
    return uri_;
}

const string &RingtoneDataCommand::GetBundleName()
{
    return bundleName_;
}

const string &RingtoneDataCommand::GetResult()
{
    return result_;
}

int32_t RingtoneDataCommand::GetToneIdFromUri(Uri &uri)
{
    string uriStr = uri.ToString();
    if (uriStr.empty()) {
        return E_INVALID_URI;
    }

    auto toneId = uriStr.substr(RINGTONE_PATH_URI.size(), uriStr.size());
    if (RingtoneUtils::IsNumber(toneId)) {
        RINGTONE_INFO_LOG("Get toneId=%{public}s from Uri", toneId.c_str());
        return stoi(toneId);
    }

    return E_INVALID_URI;
}

RingtoneOperationType RingtoneDataCommand::GetOprnType() const
{
    return oprnType_;
}
} // namespace Media
} // namespace OHOS
