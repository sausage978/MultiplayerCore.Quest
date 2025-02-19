#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"

#include "GlobalNamespace/QuickPlaySetupData_QuickPlaySongPacksOverride_PredefinedPack.hpp"
#include "GlobalNamespace/QuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPack.hpp"
#include "GlobalNamespace/QuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPackName.hpp"
#include "GlobalNamespace/QuickPlaySetupData_QuickPlaySongPacksOverride.hpp"
#include "GlobalNamespace/QuickPlaySetupData.hpp"

#include "CodegenExtensions/TempBloomFilterUtil.hpp"

//#include "GlobalNamespace/MasterServerQuickPlaySetupModel.hpp"
#include "GlobalNamespace/SongPackMaskModelSO.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/JoinQuickPlayViewController.hpp"
#include "GlobalNamespace/SimpleDialogPromptViewController.hpp"
#include "GlobalNamespace/MultiplayerModeSettings.hpp"

#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

#include "Polyglot/Localization.hpp"
#include "Polyglot/LanguageExtensions.hpp"

#include "GlobalNamespace/QuickPlaySongPacksDropdown.hpp"

using namespace GlobalNamespace;

using MSQSD_QPSPO_PredefinedPack = QuickPlaySetupData::QuickPlaySongPacksOverride::PredefinedPack;
using MSQSD_QPSPO_LocalizedCustomPack = QuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPack;
using MSQD_QPSPO_LocalizedCustomPackName = QuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPackName;

bool gotSongPackOverrides = false;

namespace MultiplayerCore {
    // Check for our custom packs
    MAKE_HOOK_MATCH(QuickPlaySongPacksDropdown_LazyInit, &QuickPlaySongPacksDropdown::LazyInit, void, QuickPlaySongPacksDropdown* self) {
        gotSongPackOverrides = (self->dyn__quickPlaySongPacksOverride() != nullptr);
        QuickPlaySongPacksDropdown_LazyInit(self);
    }

    MAKE_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish, &MultiplayerModeSelectionFlowCoordinator::HandleJoinQuickPlayViewControllerDidFinish, void, MultiplayerModeSelectionFlowCoordinator* self, bool success) {
        Il2CppString* levelPackName = self->dyn__joinQuickPlayViewController()->dyn__multiplayerModeSettings()->dyn_quickPlaySongPackMaskSerializedName();
        if (success && 
            self->dyn__songPackMaskModel()->ToSongPackMask(
                levelPackName
            ).Contains(
                getCustomLevelSongPackMaskStr())
            ) {
            self->dyn__simpleDialogPromptViewController()->Init(
                il2cpp_utils::newcsstr("Custom Song Quickplay"),
                il2cpp_utils::newcsstr("<color=#EB4949>This category includes songs of varying difficulty.\nIt may be more enjoyable to play in a private lobby with friends."),
                il2cpp_utils::newcsstr("Continue"),
                il2cpp_utils::newcsstr("Cancel"),
                il2cpp_utils::MakeDelegate<System::Action_1<int>*>(classof(System::Action_1<int>*), (std::function<void(int)>)[self, success](int btnId) {
                    switch (btnId)
                    {
                    default:
                    case 0: // Continue
                        MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish(self, success);
                        return;
                    case 1: // Cancel
                        //self->DismissViewController(self->dyn__simpleDialogPromptViewController(), HMUI::ViewController::AnimationDirection::Vertical, nullptr, false);
                        self->ReplaceTopViewController(self->dyn__joinQuickPlayViewController(), nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Vertical);
                        return;
                    }
                    }
                )
            );
            self->ReplaceTopViewController(self->dyn__simpleDialogPromptViewController(), nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Vertical);
        } else MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish(self, success);
    }

    //MAKE_HOOK_MATCH(MasterServerQuickPlaySetupModel_GetQuickPlaySetupInternal, &MasterServerQuickPlaySetupModel::GetQuickPlaySetupInternal, System::Threading::Tasks::Task_1<GlobalNamespace::QuickPlaySetupData*>*, MasterServerQuickPlaySetupModel* self) {
    //    getLogger().debug("MasterServerQuickPlaySetupModel::GetQuickPlaySetupInternal");
    //    using T1_MasterServerQuickplaySetupData = System::Threading::Tasks::Task_1<GlobalNamespace::QuickPlaySetupData*>*;
    //    T1_MasterServerQuickplaySetupData result = MasterServerQuickPlaySetupModel_GetQuickPlaySetupInternal(self);
    //    auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void()>)[result] {
    //        QuickPlaySetupData* data = result->get_ResultOnSuccess();
    //        if (data) {
    //            QuickPlaySetupData::QuickPlaySongPacksOverride* overrides = data->dyn_quickPlayAvailablePacksOverride();
    //            getLogger().debug("MasterServerQuickplaySetupData: hasOverride: '%s', Check _quickPlayAvailablePacksOverride Pointer: '%p'", data->get_hasOverride() ? "true" : "false", overrides);
    //            using PredefinedPack = QuickPlaySetupData::QuickPlaySongPacksOverride::PredefinedPack;
    //            for (int i = 0; i < overrides->dyn_predefinedPackIds()->get_Count(); i++) {
    //                PredefinedPack* pack = overrides->dyn_predefinedPackIds()->get_Item(i);
    //                getLogger().debug("predefinedPackIds: order: '%d', packId: '%s'", pack->dyn_order(), to_utf8(csstrtostr(pack->dyn_packId())).c_str());
    //            }
    //            using LocalizedCustomPack = QuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPack;
    //            for (int i = 0; i < overrides->dyn_localizedCustomPacks()->get_Count(); i++) {
    //                LocalizedCustomPack* pack = overrides->dyn_localizedCustomPacks()->get_Item(i);
    //                getLogger().debug("predefinedPackIds: order: '%d', serializedName: '%s'", pack->dyn_order(), to_utf8(csstrtostr(pack->dyn_serializedName())).c_str());
    //                for (int j = 0; j < pack->dyn_packIds()->get_Count(); j++) {
    //                    getLogger().debug("predefinedPackIds packId '%d': packId: '%s'", j, to_utf8(csstrtostr(pack->dyn_packIds()->get_Item(j))).c_str());
    //                }
    //            }
    //        }
    //        else {
    //            getLogger().debug("MasterServerQuickPlaySetupModel::GetQuickPlaySetupInternal: Task does not have Results");
    //        }
    //        }
    //    );
    //    reinterpret_cast<System::Threading::Tasks::Task*>(result)->ContinueWith(action);

    //    return result;
    //}

    void Hooks::QuickplayHooks() {
        INSTALL_HOOK(getLogger(), QuickPlaySongPacksDropdown_LazyInit);
        INSTALL_HOOK(getLogger(), MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish);
        //INSTALL_HOOK(getLogger(), MasterServerQuickPlaySetupModel_GetQuickPlaySetupInternal);
    }
}

