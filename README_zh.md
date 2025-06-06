# RingtoneLibrary组件

- [简介](#section1158716411637)
- [目录](#section161941989596)
- [使用说明](#usage-guidelines)
- [相关仓](#section1533973044317)

## 简介<a name="section1158716411637"></a>
**ringtone\_library** 仓库提供了一系列易用的接口用于设定及获取系统铃音信息。
**ringtone\_library** 提供了标准DataShareExtension接口，支持存储及查询通过SystemSoundManager设置的自定义来电/闹钟/短信/系统通知铃音文件。
系统应用及音乐开放能力RingtoneKit通过SystemSoundManager设置及查询自定义铃音，非系统应用通过音乐开放能力RingtoneKit设置及查询自定义铃音。

**图1** ringtonelibrary组件架构图
![](figures/ringtonelibrary-architecture_zh.png "ringtonelibrary-architecture")

支持能力列举如下：
- 读取铃音内容
- 存储和删除自定义铃音
- 读取铃音列表，包含系统铃音和自定义铃音
- 扫描系统预制铃音目录
- 支持应用静默访问铃音库

## 目录<a name="section161941989596"></a>
仓库目录结构如下：
```
/foundation/multimedia/ringtone_library/   # 铃音库组件代码
├── frameworks                             # 框架代码
├── interfaces                             # 接口代码
│   └── inner_api                          # 内部native接口
├── LICENSE                                # 证书文件
├── services                               # 铃音库服务实现
│   ├── ringtone_backup                    # 铃音库备份升级服务
│   ├── ringtone_data_extension            # 扩展插件接口
│   ├── ringtone_dfx                       # DFX实现
│   ├── ringtone_helper                    # 辅助类
│   └── ringtone_scanner                   # 扫描功能实现
└── test                                   # 测试代码
```

## 使用说明<a name="usage-guidelines"></a>
### 概述
提供铃音列表的增、删、改、查等接口，可以通过Uri("datashare:///ringtone/ringtone")对铃音列表进行操作。
接口参数主要有对象类型的Uri、DataSharePredicates和DataShareValuesBucket等。

使用接口功能前，需要先获取DataShareHelper
```cpp
auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
auto remoteObj = saManager->GetSystemAbility(systemAbilityId);
std::shared_ptr<DataShare::DataShareHelper> datashareHelper = DataShare::DataShareHelper::Creator(remoteObj, "datashare:///ringtone/ringtone");
```

### 新增铃音接口
#### datashareHelper->Insert(const Uri &uri, const DataShareValuesBucket &value);

- 参数描述

  | 名称     | 读写属性 | 类型                   | 必填 | 描述                           |
  | -------- | -------- | ---------------------- | ---- | ------------------------------ |
  | uri      | 只读     | Uri&                 | 是   | 铃音库操作Uri("datashare:///ringtone/ringtone")     |
  | value    | 只读     | DataShareValuesBucket& | 是   | 数据库字段key-value对象 |

- 返回值

  来电铃音id

- 示例
    ```cpp
    Uri ringtoneUri("datashare:///ringtone/ringtone");
    int32_t index = 0;
    const int32_t count = 10;
    const int64_t ringtoneSize = 1022;
    const int type = 2;
    const int64_t addedTime = 1559276453;
    const int64_t modifiedTime = 1559276455;
    const int64_t takenTime = 1559276457;
    const int durationTime = 112;
    DataShareValuesBucket valuesBucket;
    valuesBucket.Put(RINGTONE_COLUMN_DATA,
            static_cast<string>("/path_to_target/test_ringtone.ogg"));
    valuesBucket.Put(RINGTONE_COLUMN_SIZE, static_cast<int64_t>(ringtoneSize));
    valuesBucket.Put(RINGTONE_COLUMN_DISPLAY_NAME, static_cast<string>("test_ringtone.ogg");
    valuesBucket.Put(RINGTONE_COLUMN_TITLE, static_cast<string>("test_ringtone"));
    valuesBucket.Put(RINGTONE_COLUMN_MEDIA_TYPE, static_cast<int>(type));
    valuesBucket.Put(RINGTONE_COLUMN_TONE_TYPE, static_cast<int>(type));
    valuesBucket.Put(RINGTONE_COLUMN_MIME_TYPE, static_cast<string>("ogg"));
    valuesBucket.Put(RINGTONE_COLUMN_SOURCE_TYPE, static_cast<int>(1));
    valuesBucket.Put(RINGTONE_COLUMN_DATE_ADDED, static_cast<int64_t>(addedTime));
    valuesBucket.Put(RINGTONE_COLUMN_DATE_MODIFIED, static_cast<int64_t>(modifiedTime));
    valuesBucket.Put(RINGTONE_COLUMN_DATE_TAKEN, static_cast<int64_t>(takenTime));
    valuesBucket.Put(RINGTONE_COLUMN_DURATION, static_cast<int>(durationTime));
    valuesBucket.Put(RINGTONE_COLUMN_SHOT_TONE_TYPE, static_cast<int>(1));
    valuesBucket.Put(RINGTONE_COLUMN_SHOT_TONE_SOURCE_TYPE, static_cast<int>(type));
    valuesBucket.Put(RINGTONE_COLUMN_NOTIFICATION_TONE_TYPE, static_cast<int>(1));
    valuesBucket.Put(RINGTONE_COLUMN_NOTIFICATION_TONE_SOURCE_TYPE, static_cast<int>(type));
    valuesBucket.Put(RINGTONE_COLUMN_RING_TONE_TYPE, static_cast<int>(1));
    valuesBucket.Put(RINGTONE_COLUMN_RING_TONE_SOURCE_TYPE, static_cast<int>(type));
    valuesBucket.Put(RINGTONE_COLUMN_ALARM_TONE_TYPE, static_cast<int>(1));
    valuesBucket.Put(RINGTONE_COLUMN_ALARM_TONE_SOURCE_TYPE, static_cast<int>(type));
    int32_t ret = dataShareHelper->Insert(ringtoneUri, valuesBucket);
    ```

### 删除铃音接口
#### datashareHelper->Delete(const Uri &uri, const DataSharePredicates &predicates);

- Delete参数描述

  | 名称         | 读写属性 | 类型                    | 必填 | 描述                           |
  | --------     | -------- | ---------------------- | ---- | ------------------------------ |
  | uri          | 只读     | Uri&                 | 是   | 铃音库操作Uri("datashare:///ringtone/ringtone")          |
  | condition    | 只读     | DataSharePredicates&    | 是   | 数据库删除条件              |

- 返回值

  删除的铃音数量

- 示例

    ```cpp
    Uri ringtoneUri("datashare:///ringtone/ringtone");
    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.SetWhereClause(RINGTONE_COLUMN_TONE_ID + " = ? ");
    deletePredicates.SetWhereArgs({ to_string(1) });
    int32_t ret = g_dataShareHelper->Delete(ringtoneUri, deletePredicates);
    ```

### 修改铃音接口
#### datashareHelper->Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value);

- Update参数描述

  | 名称         | 读写属性 | 类型                    | 必填 | 描述                           |
  | --------     | -------- | ---------------------- | ---- | ------------------------------ |
  | uri          | 只读     | Uri&                 | 是   | 具体操作的uri          |
  | condition    | 只读     | DataSharePredicates&    | 是   | 数据库更新条件               |
  | value        | 只读     | DataShareValuesBucket& | 是   | 数据库字段key-value对象 |

- 返回值

  修改的铃音数量

- 示例
    ```cpp
    Uri ringtoneUri("datashare:///ringtone/ringtone");
    DataShare::DataSharePredicates updatePredicates;
    updatePredicates.SetWhereClause(RINGTONE_COLUMN_TONE_ID + " = ? ");
    updatePredicates.SetWhereArgs({ to_string(1) });
    DataShareValuesBucket updateValuesBucket;
    updateValuesBucket.Put(RINGTONE_COLUMN_SHOT_TONE_TYPE, 0);
    updateValuesBucket.Put(RINGTONE_COLUMN_SHOT_TONE_SOURCE_TYPE, 0);
    int32_t ret = dataShareHelper->Update(ringtoneUri, deletePredicates, updateValuesBucket);
    ```

### 查询铃音接口
#### datashareHelper->Query(const Uri &uri, const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError);

- Query参数描述

  | 名称             | 读写属性 | 类型                    | 必填  | 描述                           |
  | --------         | -------- | ---------------------- | ----  | ------------------------------ |
  | uri              | 只读     | Uri&                 | 是    | 铃音库操作Uri("datashare:///ringtone/ringtone")        |
  | condition        | 只读     | DataSharePredicates&   | 是    | 查询条件             |
  | resultColumns    | 只读     | std::vector<std::string>& | 是    | 需要查询的列字段名称   |
  | businessError    | 只读     | DatashareBusinessError& | 是   | 异常代码 |

- 返回值
  数据库ResultSet结果集

- 示例
    ```cpp
    Uri ringtoneUri("datashare:///ringtone/ringtone");
    int errCode = 0;
    DatashareBusinessError businessError;
    DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(RINGTONE_COLUMN_SHOT_TONE_TYPE, to_string(1));
    vector<string> columns = {
        { RINGTONE_COLUMN_TONE_ID },
        { RINGTONE_COLUMN_DISPLAY_NAME },
        { RINGTONE_COLUMN_DATE_ADDED },
        { RINGTONE_COLUMN_SHOT_TONE_TYPE }
    };
    auto resultSet = dataShareHelper->Query(ringtoneUri, queryPredicates, columns, &businessError);
    ```

### 静默访问能力
- 能力描述
  
  为了降低数据提供方拉起次数，提高访问速度，OpenHarmony提供了一种不拉起数据提供方直接访问数据库的方式，即静默数据访问。
  铃音库作为数据提供方在module.json5中的proxyData节点定义要共享的表的标识、读写权限、基本信息，在应用侧申请xx权限后，就可以实现不拉起铃音库进程访问铃音库数据

- 数据提供方参数配置

  module.json配置样例:

  当应用适配requiredReadPermission && requiredWritePermission中所需要的权限后，通过uri即可对数据库进行读写操作。

  ```cpp
  "proxyDatas": [
    {
      "uri":"datashareproxy://com.ohos.ringtonelibrary.ringtonelibrarydata/entry/ringtone_library/SimCardSetting",
      "requiredReadPermission":"ohos.permission.ACCESS_CUSTOM_RINGTONE",
      "requiredWritePermission":"ohos.permission.ACCESS_CUSTOM_RINGTONE",
      "metadata": {
        "name": "dataProperties",
        "resource": "$profile:rdb_meta_simcard_setting"
      }
    },
  ]
  ```

  - 铃音库每张表的uri定义请参考ringtone_proxy_uri.h文件
  
  代码示例：
  ```cpp
  const std::string RINGTONE_LIBRARY_PROXY_DATA_URI_SIMCARD_SETTING =
    "datashare:///com.ohos.ringtonelibrary.ringtonelibrarydata/entry/ringtone_library/SimCardSetting?Proxy=true";
  ```
- 数据访问方

  铃音框架静默访问接口CreateDataShareHelperUri

  铃音框架需要先去校验权限应用是否存在ACCESS_CUSTOM_RINGTONE权限，如果不存在该权限，则会使用dataShare方式拉起铃音库进程

  铃音框架静默访问铃音库代码示例：
  ```cpp
    Uri RINGTONEURI_PROXY(RINGTONE_LIBRARY_PROXY_DATA_URI_TONE_FILES + "&user=" +
        std::to_string(SystemSoundManagerUtils::GetCurrentUserId()));
    auto resultSet = dataShareHelper->Query(RINGTONEURI_PROXY, queryPredicates, COLUMNS, &businessError);
    int32_t errCode = businessError.GetCode();
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    int32_t result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller,
        "ohos.permission.ACCESS_CUSTOM_RINGTONE");
    MEDIA_LOGI("GetRingtoneAttrList:errCode:%{public}d, result :%{public}d ", errCode, result);
    if (errCode != 0 || result != Security::AccessToken::PermissionState::PERMISSION_GRANTED  ||
        !SystemSoundManagerUtils::GetScannerFirstParameter(RINGTONE_PARAMETER_SCANNER_FIRST_KEY, RINGTONEPARA_SIZE) ||
        !SystemSoundManagerUtils::CheckCurrentUser()) {
        dataShareHelper = SystemSoundManagerUtils::CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
        CHECK_AND_RETURN_RET_LOG(dataShareHelper != nullptr, ringtoneAttrsArray_,
            "Invalid dataShare, datashare or ringtone library error.");
        resultSet = dataShareHelper->Query(RINGTONEURI, queryPredicates, COLUMNS, &businessError);
    }
  ```

### 标准振动与弱振动
  针对手机铃声，提供不同的振动方式。不同铃声类型支持的振动方式规格如下：
  - 预制铃声：支持同步振动与非同步振动
    - 同步振动
      示例：振动方式与铃声同步，预制铃声选择 "A"，同步振动即为振动时直接使用"A"铃声所对应的标准或弱化振动方式。
    - 非同步振动
      示例：振动方式选中与铃声不同步，可选择振动列表中任何一个，比如：选择"B"，即振动时使用"B"对应的标准或弱化振动方式
  - 自定义铃声
      - 非同步振动
        示例：铃声选择本地音乐，比如铃声为"ringtone.ogg"，选择振动列表中任何一个，比如选择"B"，振动时使用"B"对应的标准或弱化振动方式。

### RingtoneLibrary数据库表结构
- 铃音表结构

|字段名|类型|描述|
|---|---|---|
|ringtone_id|INTEGER|主键，数据库自增|
|data|TEXT|铃音文件存储路径|
|size|BITINT|铃音文件大小|
|display_name|TEXT|title+后缀|
|title|TEXT|铃音名称|
|media_type|TEXT|媒体类型，音频类为2|
|tone_type|INTEGER|ALARM = 0, RINGTONE = 1, NOTIFICATION = 2, NOTIFICATION可以设置为短信和通知|
|mime_type|TEXT|UTI类型|
|source_type|INTEGER|系统预制(1)，用户自定义(2)|
|date_added|BIGINT|时间戳，添加时间|
|date_modified|BIGINT|时间戳，修改时间|
|date_taken|BIGINT|时间戳，创建时间|
|duration|INTEGER|时长，毫秒值|
|shot_tone_type|INTEGER|短信铃音类型： 非短信音(0)，卡1短信音(1)，卡2短信音(2)，双卡短信音(3)|
|shot_tone_source_type|INTEGER|系统设置(1)，用户设置(2)|
|notification_tone_type|INTEGER|通知音铃音类型： 非通知音(0)，通知音(1)|
|notification_tone_source_type|INTEGER|系统设置(1)，用户设置(2)|
|ring_tone_type|INTEGER|通话铃音类型： 非通话铃音(0)，卡1铃音(1)，卡2铃音(2)，双卡铃音(3)|
|ring_tone_source_type|INTEGER|系统设置(1)，用户设置(2)|
|alarm_tone_type|INTEGER|闹钟铃音类型： 非闹钟铃音(0)，闹钟铃音(1)|
|alarm_tone_source_type|INTEGER|系统设置(1)，用户设置(2)|

- 振动表结构

|字段名|类型|描述|
|---|---|---|
|vibrate_id|INTEGER|主键，数据库自增|
|data|TEXT|振动文件存储路径|
|size|BITINT|振动文件大小|
|display_name|TEXT|title+后缀|
|title|TEXT|振动名称|
|vibrate_type|INTEGER|振动类型：经典标准(1), 经典弱化(2)， 闹钟标准(3)， 来电标准(4)， 通知标准(5)， 闹钟弱化(6)， 来电弱化(7)， 通知弱化(8)|
|display_language|TEXT|title语言类型|
|source_type|INTEGER|系统预制(1)，用户自定义(2)|
|date_added|BIGINT|时间戳，添加时间|
|date_modified|BIGINT|时间戳，修改时间|
|date_taken|BIGINT|时间戳，创建时间|
|play_mode|INTEGER|播放模式：同步(1), 经典(2)|
|scanner_flag|INTEGER|扫描标志位：默认为0，扫描过程中将其置1，扫描结束恢复为0|

## 相关仓<a name="section1533973044317"></a>
**[multimedia/ringtone_library](https://gitee.com/openharmony/multimedia_ringtone_library)**
**[multimedia/player_framework](https://gitee.com/openharmony/multimedia_player_framework)**
