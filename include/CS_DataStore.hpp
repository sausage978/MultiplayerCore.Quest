#pragma once
#include "custom-types/shared/macros.hpp"
#include <optional>
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/ILevelGameplaySetupData.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore, DataStore, Il2CppObject, 

    DECLARE_CTOR(New, GlobalNamespace::ILevelGameplaySetupData*, GlobalNamespace::IDifficultyBeatmap*);
    
    DECLARE_INSTANCE_FIELD(GlobalNamespace::IDifficultyBeatmap*, loadingDifficultyBeatmap);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::ILevelGameplaySetupData*, loadingGameplaySetupData);

    static DataStore* instance;

public:
    static DataStore* get_Instance();
    static DataStore* CS_Ctor(GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
    static void Clear();
)