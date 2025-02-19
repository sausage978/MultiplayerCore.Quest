#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "Networking/MpPacketSerializer.hpp"
#include "UI/LobbySetupPanel.hpp"
#include "UI/DownloadedSongsGSM.hpp"
#include "UI/CenterScreenLoading.hpp"
#include "CS_DataStore.hpp"

#include "Utilities.hpp"

#include "GlobalNamespace/ConnectedPlayerManager.hpp"
#include "CodegenExtensions/TempBloomFilterUtil.hpp"

#include "GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/CenterStageScreenController.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"

// For Hooking Debug Loggers
#include "BGNet/Logging/Debug.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"

//#include "GlobalNamespace/LobbySetupViewController.hpp"
//#include "GlobalNamespace/LobbySetupViewController_CannotStartGameReason.hpp"
using namespace GlobalNamespace;
using namespace System::Threading::Tasks;
using namespace MultiQuestensions;
using namespace MultiplayerCore;

#ifndef VERSION
#warning No Version set
#define VERSION "0.1.0"
#endif

ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* myLogger = new Logger(modInfo, LoggerOptions(false, true));
    return *myLogger;
}


namespace MultiplayerCore {
    // Plugin setup stuff
    GlobalNamespace::MultiplayerSessionManager* _multiplayerSessionManager;
    GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
    //MultiQuestensions::PacketManager* packetManager;
    SafePtr<Networking::MpPacketSerializer> mpPacketSerializer;
    GlobalNamespace::LobbyGameStateController* lobbyGameStateController;

    LobbySetupViewController* lobbySetupView;

    std::string moddedState = "modded";

    StringW customLevelSongPackMaskStr("custom_levelpack_CustomLevels");

    StringW getCustomLevelSongPackMaskStr() {
        static Il2CppString* songPackMaskStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("custom_levelpack_CustomLevels");
        return songPackMaskStr;
    }

    StringW getModdedStateStr() {
        static ConstString moddedStateStr("modded");
        return moddedStateStr;
    }

    StringW getMEStateStr() {
        static Il2CppString* moddedStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("ME_Installed");
        return moddedStateStr;
    }

    StringW getNEStateStr() {
        static Il2CppString* moddedStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("ME_Installed");
        return moddedStateStr;
    }

    StringW getChromaStateStr() {
        static Il2CppString* moddedStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("Chroma_Installed");
        return moddedStateStr;
    }

    bool AllPlayersModded() {
        for (int i = 0; i < _multiplayerSessionManager->dyn__connectedPlayers()->get_Count(); i++) {
            if (!_multiplayerSessionManager->dyn__connectedPlayers()->get_Item(i)->HasState(getModdedStateStr())) return false;
        }
        return true;
    }

    bool AllPlayersHaveNE() {
        for (int i = 0; i < _multiplayerSessionManager->dyn__connectedPlayers()->get_Count(); i++) {
            if (!_multiplayerSessionManager->dyn__connectedPlayers()->get_Item(i)->HasState(getNEStateStr())) return false;
        }
        return true;
    }

    bool AllPlayersHaveME() {
        for (int i = 0; i < _multiplayerSessionManager->dyn__connectedPlayers()->get_Count(); i++) {
            if (!_multiplayerSessionManager->dyn__connectedPlayers()->get_Item(i)->HasState(getMEStateStr())) return false;
        }
        return true;
    }

    bool AllPlayersHaveChroma() {
        for (int i = 0; i < _multiplayerSessionManager->dyn__connectedPlayers()->get_Count(); i++) {
            if (!_multiplayerSessionManager->dyn__connectedPlayers()->get_Item(i)->HasState(getChromaStateStr())) return false;
        }
        return true;
    }


}

//using PD_ValueCollection = System::Collections::Generic::Dictionary_2<Il2CppString*, ILobbyPlayerDataModel*>::ValueCollection;

//MultiQuestensions::Beatmaps::PreviewBeatmapStub* GetExistingPreview(Il2CppString* levelId) {
//    getLogger().debug("GetExistingPreview PlayerCount: %d", sessionManager->dyn__connectedPlayers()->get_Count());
//    for (int i = 0; i < sessionManager->dyn__connectedPlayers()->get_Count(); i++) {
//        ILevelGameplaySetupData* playerData = reinterpret_cast<ILevelGameplaySetupData*>(lobbyPlayersDataModel->dyn__playersData()->
//            get_Item(sessionManager->dyn__connectedPlayers()->get_Item(i)->get_userId()));
//        if (playerData->get_beatmapLevel() != nullptr && csstrtostr(playerData->get_beatmapLevel()->get_levelID()) == csstrtostr(levelId)) {
//            getLogger().debug("GetExistingPreview type: %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(playerData->get_beatmapLevel())->klass).c_str());
//            if (il2cpp_utils::AssignableFrom<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(reinterpret_cast<Il2CppObject*>(playerData->get_beatmapLevel())->klass)) {
//                getLogger().debug(__FILE__ " Line: %d", __LINE__);
//                return reinterpret_cast<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(playerData->get_beatmapLevel());
//            }
//        }
//        else {
//            if (playerData->get_beatmapLevel() != nullptr)
//                getLogger().debug("Check expected levelId: %s, \r\nactual levelId: %s, \r\nlevelIDs match? %s", 
//                    to_utf8(csstrtostr(levelId)).c_str(), 
//                    to_utf8(csstrtostr(playerData->get_beatmapLevel()->get_levelID())).c_str(), 
//                    csstrtostr(playerData->get_beatmapLevel()->get_levelID()) == csstrtostr(levelId) ? "true" : "false");
//            else getLogger().debug("beatmapLevel is null");
//        }
//    }
//    getLogger().debug("GetExistingPreview return nullptr");
//    return nullptr;
//}

// LobbyPlayersDataModel Activate
MAKE_HOOK_MATCH(LobbyPlayersActivate, &LobbyPlayersDataModel::Activate, void, LobbyPlayersDataModel* self) {
    lobbyPlayersDataModel = self;
    LobbyPlayersActivate(lobbyPlayersDataModel);
}

// LobbyPlayersDataModel SetLocalPlayerBeatmapLevel
//MAKE_HOOK_MATCH(LobbyPlayersSetLocalBeatmap, &LobbyPlayersDataModel::SetLocalPlayerBeatmapLevel, void, LobbyPlayersDataModel* self, Il2CppString* levelId, BeatmapDifficulty beatmapDifficulty, BeatmapCharacteristicSO* characteristic) {
//    
//    Il2CppString* hash = LevelIdToHash(levelId);
//    if (hash != nullptr) {
//        getLogger().info("Local user selected song '%s'.", to_utf8(csstrtostr(hash)).c_str());
//        MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview = GetExistingPreview(levelId);
//        if (preview != nullptr) {
//            self->SetPlayerBeatmapLevel(self->get_localUserId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), beatmapDifficulty, characteristic);
//            self->dyn__menuRpcManager()->RecommendBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
//            return;
//        }
//        else {
//            IPreviewBeatmapLevel* localIPreview = self->dyn__beatmapLevelsModel()->GetLevelPreviewForLevelId(levelId);
//            if (localIPreview != nullptr) {
//                try {
//                    using namespace MultiQuestensions::Beatmaps;
//                    PreviewBeatmapStub* previewStub = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(hash, localIPreview, static_cast<PreviewBeatmapPacket*>(nullptr)));
//                    //MultiQuestensions::Beatmaps::PreviewBeatmapStub* previewStub = MultiQuestensions::Beatmaps::PreviewBeatmapStub::New_ctor(hash, localIPreview);
//                    getLogger().debug("Check 'LobbyPlayersSetLocalBeatmap' levelID: %s", to_utf8(csstrtostr(reinterpret_cast<IPreviewBeatmapLevel*>(previewStub)->get_levelID())).c_str());
//                    self->SetPlayerBeatmapLevel(self->get_localUserId(), reinterpret_cast<IPreviewBeatmapLevel*>(previewStub), beatmapDifficulty, characteristic);
//                    PreviewBeatmapPacket* packet = previewStub->GetPacket(characteristic->get_serializedName(), beatmapDifficulty);
//                    self->dyn__menuRpcManager()->RecommendBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
//                    packetManager->Send(packet->ToINetSerializable());
//                    return;
//                }
//                catch (const std::runtime_error& e) {
//                    getLogger().critical("REPORT TO ENDER: An Exception occured during class creation: %s", e.what());
//                }
//            }
//        }
//    }
//    LobbyPlayersSetLocalBeatmap(self, levelId, beatmapDifficulty, characteristic);
//}

//// LobbyPlayersDataModel HandleMenuRpcManagerSelectedBeatmap (DONT REMOVE THIS, without it a player's selected map will be cleared)
//MAKE_HOOK_MATCH(LobbyPlayersSelectedBeatmap, &LobbyPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap, void, LobbyPlayersDataModel* self, Il2CppString* userId, BeatmapIdentifierNetSerializable* beatmapId) {
//    getLogger().debug("HandleMenuRpcManagerRecommendBeatmap: LevelID: %s", to_utf8(csstrtostr(beatmapId->get_levelID())).c_str());
//    MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview = GetExistingPreview(beatmapId->get_levelID());
//    if (preview != nullptr) {
//        getLogger().debug("HandleMenuRpcManagerRecommendBeatmap: Preview exists, SetPlayerBeatmapLevel");
//        BeatmapCharacteristicSO* characteristic = self->dyn__beatmapCharacteristicCollection()->GetBeatmapCharacteristicBySerializedName(beatmapId->get_beatmapCharacteristicSerializedName());
//        self->SetPlayerBeatmapLevel(userId, reinterpret_cast<IPreviewBeatmapLevel*>(preview), beatmapId->get_difficulty(), characteristic);
//        return;
//    }
//    else {
//        getLogger().debug("HandleMenuRpcManagerRecommendBeatmap: Preview doesn't exist use localPreview, SetPlayerBeatmapLevel");
//        IPreviewBeatmapLevel* localPreview = self->dyn__beatmapLevelsModel()->GetLevelPreviewForLevelId(beatmapId->get_levelID());
//        if (localPreview != nullptr) {
//            BeatmapCharacteristicSO* characteristic = self->dyn__beatmapCharacteristicCollection()->GetBeatmapCharacteristicBySerializedName(beatmapId->get_beatmapCharacteristicSerializedName());
//            //self->SetPlayerBeatmapLevel(userId, localPreview, beatmapId->get_difficulty(), characteristic);
//            LobbyPlayersSelectedBeatmap(self, userId, beatmapId);
//        }
//        else {
//            // TODO: fetch beatmap data from beatsaver
//            getLogger().debug("No Info found, need to fetch from BeatSaver");
//            //BeatSaver::API::GetBeatmapByHashAsync(GetHash((beatmapId->levelID),
//            //    [](std::optional<BeatSaver::Beatmap> beatmap) {
//            //        QuestUI::MainThreadScheduler::Schedule(
//            //            [beatmap] {
//            //                if (beatmap.has_value()) {
//            //                    self->SetPlayerBeatmapLevel(userId, bmPreview, beatmapId);
//            //                // Create our own Preview or something?
//            //                }
//            //                else {
//            //                // Well doesn't exist on BeatSaver then
//            //                }
//            //            }
//            //        );
//            //    }
//            //);
//        }
//    }
//}

MAKE_HOOK_MATCH(LobbySetupViewController_DidActivate, &LobbySetupViewController::DidActivate, void, LobbySetupViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    lobbySetupView = self;
    LobbySetupViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (getConfig().config["autoDelete"].GetBool() && !DownloadedSongIds.empty()) {
        using namespace RuntimeSongLoader::API;
        std::string hash = DownloadedSongIds.back();
        getLogger().debug("AutoDelete Song with Hash '%s'", hash.c_str());
        std::optional<CustomPreviewBeatmapLevel*> levelOpt = GetLevelByHash(hash);
        if (levelOpt.has_value()) {
            std::string songPath = to_utf8(csstrtostr(levelOpt.value()->get_customLevelPath()));
            getLogger().info("Deleting Song: %s", songPath.c_str());
            DeleteSong(songPath, [&] {
                RefreshSongs(false);
                DownloadedSongIds.pop_back();
            });
            if (lobbyGameStateController) lobbyGameStateController->dyn__menuRpcManager()->SetIsEntitledToLevel(levelOpt.value()->get_levelID(), EntitlementsStatus::NotOwned);
        }
    }
    if (firstActivation) {
        MultiQuestensions::UI::LobbySetupPanel::AddSetupPanel(self->get_rectTransform(), _multiplayerSessionManager);
    }
}

MAKE_HOOK_MATCH(MultiplayerLobbyConnectionController_CreateParty, &MultiplayerLobbyConnectionController::CreateParty, void, MultiplayerLobbyConnectionController* self, CreateServerFormData data) {
    data.songPacks = SongPackMask::get_all() | SongPackMask(BloomFilterUtil::ToBloomFilter<BitMask128>(getCustomLevelSongPackMaskStr(), 2, 13)); // TODO: Figure out why the Il2CppString constructor crashes
    MultiplayerLobbyConnectionController_CreateParty(self, data);
}

//MAKE_HOOK_MATCH(MultiplayerLobbyConnectionController_ConnectToMatchmaking, &MultiplayerLobbyConnectionController::ConnectToMatchmaking, void, MultiplayerLobbyConnectionController* self, BeatmapDifficultyMask beatmapDifficultyMask, SongPackMask songPackMask, bool allowSongSelection) {
//    if (!gotSongPackOverrides) {
//        songPackMask = songPackMask | SongPackMask(getCustomLevelSongPackMaskStr());
//    }
//    MultiplayerLobbyConnectionController_ConnectToMatchmaking(self, beatmapDifficultyMask, songPackMask, allowSongSelection);
//}

// Show the custom levels tab in multiplayer
MAKE_HOOK_MATCH(LevelSelectionNavigationController_Setup, &LevelSelectionNavigationController::Setup, void, LevelSelectionNavigationController* self,
    SongPackMask songPackMask, BeatmapDifficultyMask allowedBeatmapDifficultyMask, ::ArrayW<BeatmapCharacteristicSO*> notAllowedCharacteristics, 
    bool hidePacksIfOneOrNone, bool hidePracticeButton, ::StringW actionButtonText, IBeatmapLevelPack* levelPackToBeSelectedAfterPresent, 
    SelectLevelCategoryViewController::LevelCategory startLevelCategory, IPreviewBeatmapLevel* beatmapLevelToBeSelectedAfterPresent, bool enableCustomLevels) {
    getLogger().info("LevelSelectionNavigationController_Setup setting custom songs . . .");
    LevelSelectionNavigationController_Setup(self, songPackMask, allowedBeatmapDifficultyMask, notAllowedCharacteristics, hidePacksIfOneOrNone, hidePracticeButton,
        actionButtonText, levelPackToBeSelectedAfterPresent, startLevelCategory, beatmapLevelToBeSelectedAfterPresent, songPackMask.Contains(getCustomLevelSongPackMaskStr()) /*songPackMask.Contains(BloomFilterUtil::ToBloomFilter<BitMask128>(getCustomLevelSongPackMaskStr(), 2, 13))*/);
}

static bool isMissingLevel = false;

// This hook makes sure to grey-out the play button so that players can't start a level that someone doesn't have.
// This prevents crashes.
MAKE_HOOK_MATCH(LobbySetupViewController_SetPlayersMissingLevelText , &LobbySetupViewController::SetPlayersMissingLevelText, void, LobbySetupViewController* self, StringW playersMissingLevelText) {
    getLogger().info("LobbySetupViewController_SetPlayersMissingLevelText");
    if (!missingLevelText.empty()) {
        getLogger().info("Disabling start game as entitlements missing level text exists . . .");
        isMissingLevel = true;
        playersMissingLevelText = il2cpp_utils::newcsstr(missingLevelText);
        self->SetStartGameEnabled(CannotStartGameReason::DoNotOwnSong);
        LobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
        return;
    }
    else if (playersMissingLevelText) {
        getLogger().info("Disabling start game as missing level text exists . . .");
        isMissingLevel = true;
        self->SetStartGameEnabled(CannotStartGameReason::DoNotOwnSong);
    }
    else {
        getLogger().info("Enabling start game as missing level text does not exist . . .");
        isMissingLevel = false;
        self->SetStartGameEnabled(CannotStartGameReason::None);
    }


    LobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
}

// Prevent the button becoming shown when we're force disabling it, as pressing it would crash
MAKE_HOOK_MATCH(LobbySetupViewController_SetStartGameEnabled, &LobbySetupViewController::SetStartGameEnabled, void, LobbySetupViewController* self, CannotStartGameReason cannotStartGameReason) {
    getLogger().info("LobbySetupViewController_SetStartGameEnabled. Reason: %d", (int)cannotStartGameReason);
    if (isMissingLevel && cannotStartGameReason == CannotStartGameReason::None) {
        getLogger().info("Game attempted to enable the play button when the level was missing, stopping it!");
        cannotStartGameReason = CannotStartGameReason::DoNotOwnSong;
    }
    LobbySetupViewController_SetStartGameEnabled(self, cannotStartGameReason);
}

namespace MultiplayerCore {
    bool HasSong(const std::string& levelId) {
        return static_cast<bool>(RuntimeSongLoader::API::GetLevelById(levelId));
    }
}

std::vector<std::string> DownloadedSongIds;

#include "System/Threading/CancellationTokenSource.hpp"
#include "GlobalNamespace/LevelGameplaySetupData.hpp"

MAKE_HOOK_MATCH(MultiplayerLevelLoader_LoadLevel, &MultiplayerLevelLoader::LoadLevel, void, MultiplayerLevelLoader* self, ILevelGameplaySetupData* gameplaySetupData, float initialStartTime) {
    try {
        if (gameplaySetupData && gameplaySetupData->get_beatmapLevel()) {
            getLogger().debug("LoadLevel PreviewDifficultyBeatmap type: %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(gameplaySetupData->get_beatmapLevel())->klass).c_str());
            if (gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()) {
                getLogger().debug("LoadLevel IPreviewBeatmapLevel type: %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(gameplaySetupData->get_beatmapLevel()->get_beatmapLevel())->klass).c_str());
            }
        }
        std::string levelId = static_cast<std::string>(gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID());
        getLogger().info("MultiplayerLevelLoader_LoadLevel: %s", levelId.c_str());
        MultiplayerCore::UI::CenterScreenLoading* cslInstance = MultiplayerCore::UI::CenterScreenLoading::get_Instance();
        if (IsCustomLevel(levelId)) {
            if (HasSong(levelId)) {
                getLogger().debug("MultiplayerLevelLoader_LoadLevel, HasSong, calling original");
                MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
                return;
            }
            else {
                std::string hash = Utilities::GetHash(levelId);
                BeatSaver::API::GetBeatmapByHashAsync(hash,
                    [self, gameplaySetupData, initialStartTime, hash, levelId, cslInstance](std::optional<BeatSaver::Beatmap> beatmapOpt) {
                        if (beatmapOpt.has_value()) {
                            auto beatmap = beatmapOpt.value();
                            auto beatmapName = beatmap.GetName();
                            getLogger().info("Downloading map: %s", beatmap.GetName().c_str());
                            BeatSaver::API::DownloadBeatmapAsync(beatmap,
                                [self, gameplaySetupData, initialStartTime, beatmapName, hash, levelId, beatmap, cslInstance](bool error) {
                                    if (error) {
                                        getLogger().info("Failed downloading map retrying: %s", beatmapName.c_str());
                                        BeatSaver::API::DownloadBeatmapAsync(beatmap,
                                            [self, gameplaySetupData, initialStartTime, beatmapName, hash, levelId](bool error) {
                                                if (error) {
                                                    getLogger().info("Failed downloading map: %s", beatmapName.c_str());
                                                }
                                                else {
                                                    getLogger().info("Downloaded map: %s", beatmapName.c_str());
                                                    DownloadedSongIds.emplace_back(hash);
                                                    QuestUI::MainThreadScheduler::Schedule(
                                                        [self, gameplaySetupData, initialStartTime, hash, levelId] {
                                                            RuntimeSongLoader::API::RefreshSongs(false,
                                                                [self, gameplaySetupData, initialStartTime, hash, levelId](const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
                                                                    auto* downloadedSongsGSM = MultiQuestensions::UI::DownloadedSongsGSM::get_Instance();
                                                                    if (!getConfig().config["autoDelete"].GetBool() && downloadedSongsGSM) downloadedSongsGSM->InsertCell(hash);
                                                                    else {
                                                                        getLogger().warning("DownloadedSongsGSM was null, adding to queue");
                                                                        MultiQuestensions::UI::DownloadedSongsGSM::mapQueue.push_back(hash);
                                                                    }
                                                                    getLogger().debug("Pointer Check before loading level: self='%p', gameplaySetupData='%p'", self, gameplaySetupData);
                                                                    self->dyn__loaderState() = MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::NotLoading;
                                                                    getLogger().debug("MultiplayerLevelLoader_LoadLevel, Downloaded, calling original");
                                                                    MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
                                                                    reinterpret_cast<LevelGameplaySetupData*>(gameplaySetupData)->dyn__beatmapLevel()->set_beatmapLevel(self->dyn__beatmapLevelsModel()->GetLevelPreviewForLevelId(StringW(levelId.c_str())));
                                                                    self->dyn__getBeatmapLevelResultTask() = self->dyn__beatmapLevelsModel()->GetBeatmapLevelAsync(StringW(levelId.c_str()), self->dyn__getBeatmapCancellationTokenSource()->get_Token());
                                                                    return;
                                                                }
                                                            );
                                                        }
                                                    );
                                                }
                                            }, [cslInstance](float downloadProgress) {
                                                if (cslInstance) {
                                                    QuestUI::MainThreadScheduler::Schedule(
                                                        [cslInstance, downloadProgress] {
                                                            cslInstance->ShowDownloadingProgress(downloadProgress);
                                                        });
                                                }
                                            }
                                            );
                                    }
                                    else {
                                        getLogger().info("Downloaded map: %s", beatmapName.c_str());
                                        DownloadedSongIds.emplace_back(hash);
                                        QuestUI::MainThreadScheduler::Schedule(
                                            [self, gameplaySetupData, initialStartTime, hash, levelId] {
                                                RuntimeSongLoader::API::RefreshSongs(false,
                                                    [self, gameplaySetupData, initialStartTime, hash, levelId](const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
                                                        auto* downloadedSongsGSM = MultiQuestensions::UI::DownloadedSongsGSM::get_Instance();
                                                        if (!getConfig().config["autoDelete"].GetBool() && downloadedSongsGSM) downloadedSongsGSM->InsertCell(hash);
                                                        else {
                                                            getLogger().warning("DownloadedSongsGSM was null, adding to queue");
                                                            MultiQuestensions::UI::DownloadedSongsGSM::mapQueue.push_back(hash);
                                                        }
                                                        getLogger().debug("Pointer Check before loading level: self='%p', gameplaySetupData='%p'", self, gameplaySetupData);
                                                        self->dyn__loaderState() = MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::NotLoading;
                                                        getLogger().debug("MultiplayerLevelLoader_LoadLevel, Downloaded, calling original");
                                                        MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
                                                        reinterpret_cast<LevelGameplaySetupData*>(gameplaySetupData)->dyn__beatmapLevel()->set_beatmapLevel(self->dyn__beatmapLevelsModel()->GetLevelPreviewForLevelId(StringW(levelId.c_str())));
                                                        self->dyn__getBeatmapLevelResultTask() = self->dyn__beatmapLevelsModel()->GetBeatmapLevelAsync(StringW(levelId.c_str()), self->dyn__getBeatmapCancellationTokenSource()->get_Token());
                                                        return;
                                                    }
                                                );
                                            }
                                        );
                                    }
                                }, [cslInstance](float downloadProgress) {
                                    if (cslInstance) {
                                        QuestUI::MainThreadScheduler::Schedule(
                                            [cslInstance, downloadProgress] {
                                                cslInstance->ShowDownloadingProgress(downloadProgress);
                                            });
                                    }
                                }
                                );
                        }
                    }
                );
            }
        }
        else {
            getLogger().debug("MultiplayerLevelLoader_LoadLevel, calling original");
            MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
        }
    }
    catch (il2cpp_utils::RunMethodException const& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
    catch (const std::exception& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
    catch (...) {
        getLogger().warning("REPORT TO ENDER: An Unknown exception was thrown");
    }

}

// Checks entitlement and stalls lobby until fullfilled, unless a game is already in progress.
MAKE_HOOK_MATCH(LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished, &LobbyGameStateController::HandleMultiplayerLevelLoaderCountdownFinished, void, LobbyGameStateController* self, ::GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, ::GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap) {
    getLogger().debug("LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished");
    lobbyGameStateController = self;

    DataStore* instance = DataStore::get_Instance();
    if (!instance) instance = DataStore::CS_Ctor(gameplaySetupData, difficultyBeatmap);

    instance->loadingDifficultyBeatmap = difficultyBeatmap;
    instance->loadingGameplaySetupData = gameplaySetupData;
    
    //self->dyn__multiplayerLevelLoader()->dyn__loaderState() = MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::NotLoading;
    bool entitlementStatusOK = true;
    std::string LevelID = static_cast<std::string>(gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID());
    // Checks each player, to see if they're in the lobby, and if they are, checks their entitlement status.
    MultiplayerCore::UI::CenterScreenLoading::playersReady = 0;
    for (int i = 0; i < _multiplayerSessionManager->dyn__connectedPlayers()->get_Count(); i++) {
        StringW csUserID = _multiplayerSessionManager->dyn__connectedPlayers()->get_Item(i)->get_userId();
        std::string UserID =  static_cast<std::string>(csUserID);
        if (reinterpret_cast<::System::Collections::Generic::IReadOnlyDictionary_2<::StringW, ::GlobalNamespace::ILobbyPlayerData*>*>(self->dyn__lobbyPlayersDataModel())->get_Item(csUserID)->get_isInLobby()) {
            if (entitlementDictionary[UserID][LevelID] != EntitlementsStatus::Ok) entitlementStatusOK = false;
            else MultiplayerCore::UI::CenterScreenLoading::playersReady++;
        }
    }
    getLogger().debug("[HandleMultiplayerLevelLoaderCountdownFinished] Players ready: '%d'", MultiplayerCore::UI::CenterScreenLoading::playersReady + 1);
    self->dyn__menuRpcManager()->SetIsEntitledToLevel(gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID(), EntitlementsStatus::Ok);
    if (entitlementStatusOK) {
        DataStore::Clear();

        // call original method
        LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished(self, gameplaySetupData, difficultyBeatmap);
        //entitlementDictionary.clear();
    }
}

MAKE_HOOK_MATCH(LobbyGameStateController_Activate, &LobbyGameStateController::Activate, void, LobbyGameStateController* self) {
    lobbyGameStateController = self;
    LobbyGameStateController_Activate(self);
}

// TODO: This allows for overwriting entitlement as it doesn't check entitlement for other players selection
MAKE_HOOK_MATCH(GameServerPlayerTableCell_SetData, &GameServerPlayerTableCell::SetData, void, GameServerPlayerTableCell* self, IConnectedPlayer* connectedPlayer, ILobbyPlayerData* playerData, bool hasKickPermissions, bool allowSelection, System::Threading::Tasks::Task_1<AdditionalContentModel::EntitlementStatus>* getLevelEntitlementTask) {
    getLogger().debug("GameServerPlayerTableCell_SetData Set Entitlement Owned");
    getLevelEntitlementTask = Task_1<AdditionalContentModel::EntitlementStatus>::New_ctor(AdditionalContentModel::EntitlementStatus::Owned);
    //if (playerData && reinterpret_cast<ILevelGameplaySetupData*>(playerData)->get_beatmapLevel()) {
    //    std::string UserID = to_utf8(csstrtostr(connectedPlayer->get_userId()));
    //    std::string LevelID = to_utf8(csstrtostr(reinterpret_cast<ILevelGameplaySetupData*>(playerData)->get_beatmapLevel()->get_levelID()));
    //    if (!playerData->get_isInLobby() && entitlementDictionary[UserID][LevelID] != EntitlementsStatus::Ok) getLevelEntitlementTask->TrySetResult(AdditionalContentModel::EntitlementStatus::NotOwned);
    //}
    GameServerPlayerTableCell_SetData(self, connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
}

MAKE_HOOK_MATCH(CenterStageScreenController_Setup, &CenterStageScreenController::Setup, void, CenterStageScreenController* self, bool showModifiers) {
    CenterStageScreenController_Setup(self, showModifiers);
    if (!self->get_gameObject()->GetComponent<MultiplayerCore::UI::CenterScreenLoading*>())
        self->get_gameObject()->AddComponent<MultiplayerCore::UI::CenterScreenLoading*>();
}


void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& config = getConfig().config;

    if (/*config.HasMember("color") && config["color"].IsString() &&*/
        config.HasMember("autoDelete") && config["autoDelete"].IsBool() &&
        config.HasMember("LagReducer") && config["LagReducer"].IsBool() &&
        config.HasMember("MaxPlayers") && config["MaxPlayers"].IsInt()) {
        getLogger().info("Config file already exists.");
        return;
    }  

    //config.RemoveAllMembers();
    if (!config.IsObject())
        config.SetObject();
    auto& allocator = config.GetAllocator();

    if (!(config.HasMember("MaxPlayers") && config["MaxPlayers"].IsInt()))
        config.AddMember("MaxPlayers", 10, allocator);
    if (!(config.HasMember("LagReducer") && config["LagReducer"].IsBool()))
        config.AddMember("LagReducer", false, allocator);
    if (!(config.HasMember("autoDelete") && config["autoDelete"].IsBool()))
        config.AddMember("autoDelete", false, allocator);
    //if (!(config.HasMember("color") && config["color"].IsString()))
    //    config.AddMember("color", "#08C0FF", allocator);

    getConfig().Write();
    getLogger().info("Config file created.");
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

    //getConfig().Load();
    saveDefaultConfig();

    getLogger().info("Completed setup!");
}

MAKE_HOOK_MATCH(BGNetDebug_Log, &BGNet::Logging::Debug::Log, void, StringW message) {
    getLogger().WithContext("BGNetDebug::Log").debug("%s", to_utf8(csstrtostr(message)).c_str());
    BGNetDebug_Log(message);
}

MAKE_HOOK_MATCH(BGNetDebug_LogError, &BGNet::Logging::Debug::LogError, void, StringW message) {
    getLogger().WithContext("BGNetDebug::LogError").error("%s", to_utf8(csstrtostr(message)).c_str());
    BGNetDebug_LogError(message);
}

MAKE_HOOK_MATCH(BGNetDebug_LogException, &BGNet::Logging::Debug::LogException, void, ::System::Exception* exception, StringW message) {
    getLogger().WithContext("BGNetDebug::LogWarning").critical("%s", to_utf8(csstrtostr(message)).c_str());
    BGNetDebug_LogException(exception, message);
}

MAKE_HOOK_MATCH(BGNetDebug_LogWarning, &BGNet::Logging::Debug::LogWarning, void, StringW message) {
    getLogger().WithContext("BGNetDebug::LogWarning").warning("%s", to_utf8(csstrtostr(message)).c_str());
    BGNetDebug_LogWarning(message);
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    custom_types::Register::AutoRegister();

    QuestUI::Register::RegisterGameplaySetupMenu<MultiQuestensions::UI::DownloadedSongsGSM*>(modInfo, "MP Downloaded", QuestUI::Register::Online);

    getLogger().info("Installing hooks...");
    Hooks::Install_Hooks();
    INSTALL_HOOK(getLogger(), LobbyPlayersActivate);
    //INSTALL_HOOK(getLogger(), LobbyPlayersSetLocalBeatmap);
    //INSTALL_HOOK(getLogger(), LobbyPlayersSelectedBeatmap);

    INSTALL_HOOK_ORIG(getLogger(), MultiplayerLevelLoader_LoadLevel);
    //INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_GetPlayerLevelEntitlementsAsync);
    if (Modloader::getMods().find("BeatTogether") != Modloader::getMods().end()) {
        getLogger().info("Hello BeatTogether!");
    }
    else getLogger().warning("BeatTogether was not found! Is Multiplayer modded?");

    INSTALL_HOOK_ORIG(getLogger(), LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished);
    INSTALL_HOOK(getLogger(), LobbyGameStateController_Activate);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_SetPlayersMissingLevelText);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_SetStartGameEnabled);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_DidActivate);

    INSTALL_HOOK(getLogger(), MultiplayerLobbyConnectionController_CreateParty);
    //INSTALL_HOOK(getLogger(), MultiplayerLobbyConnectionController_ConnectToMatchmaking);

    INSTALL_HOOK(getLogger(), LevelSelectionNavigationController_Setup);
    INSTALL_HOOK(getLogger(), CenterStageScreenController_Setup);

#pragma region Debug Hooks
    //INSTALL_HOOK(getLogger(), MenuRpcManager_InvokeSetCountdownEndTime);
    //INSTALL_HOOK(getLogger(), MenuRpcManager_InvokeStartLevel);
    //INSTALL_HOOK(getLogger(), LobbyGameStateController_HandleMenuRpcManagerSetCountdownEndTime);
    //INSTALL_HOOK(getLogger(), ConnectedPlayerManager_HandleSyncTimePacket);

    INSTALL_HOOK(getLogger(), BGNetDebug_Log);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogError);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogException);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogWarning);
#pragma endregion


    INSTALL_HOOK(getLogger(), GameServerPlayerTableCell_SetData);

    getLogger().info("Installed all hooks!");
}