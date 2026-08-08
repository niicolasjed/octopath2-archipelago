#include <cstdio>
#include <cstdlib>
#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <UE4SSProgram.hpp>
#include <Input/Handler.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/UScriptStruct.hpp>
#include <Unreal/NameTypes.hpp>
#include <Unreal/Property/FArrayProperty.hpp>
#include <Unreal/Property/FStructProperty.hpp>
#include <Unreal/Property/FMapProperty.hpp>
#include <vector>
#include <set>
#include <string>
#include "chest_table.hpp"
#include <map>
#include "subquest_table.hpp"

using namespace RC;
using namespace RC::Unreal;

class OT2APMod : public RC::CppUserModBase
{
public:
    bool m_hook_installed = false;          // hook coffre installe
    int m_items_given = 0;                  // nb d'items ap_items.txt deja traites (persistance)
    int m_frame_counter = 0;
    bool m_defs_loaded = false;              // ap_item_defs.txt charge
    bool m_start_char_applied = false;       // perso de depart applique (1x par lancement)
    bool m_join_hook_installed = false;      // hook JoinPlayerCharacterToParty installe
    bool m_our_join_call = false;            // vrai pendant qu'on appelle nous-memes join_character
    bool m_pending_recruit_check = false;    // un recrutement externe vient d'etre detecte
    bool m_victory_sent = false;             // check de victoire deja envoye
    int m_natural_start_char = 0;            // perso choisi naturellement par le joueur au depart
    bool m_test_chestloot = false;
    bool m_pending_loot_removal = false;
    std::wstring m_pending_loot_label;
    int32 m_pending_loot_num = 0;
    std::map<int, int> m_backpack_before_open;
    bool m_pending_backpack_check = false;
    int m_backpack_check_delay = 0;
    bool m_our_item_call = false;
    bool m_additem_hook_installed = false;
    bool m_chest_loot_suppress = false;
    std::set<int> m_quests_checked;

    std::map<int, std::pair<std::wstring, std::wstring>> m_item_defs; // ap_id -> (type, valeur)
    std::set<int> m_unlocked_chars;          // persos legitimement debloques (depart + items recus)
    std::set<int> m_pending_recruits;        // persos recrutes normalement, en attente de fin de prologue
    std::map<int, int> m_pending_recruit_baseline; // id -> nb de scenarios clairs au moment du recrutement

    OT2APMod() : CppUserModBase()
    {
        ModName = STR("OT2AP");
        ModVersion = STR("1.0");
        ModDescription = STR("Octopath Traveler 2 Archipelago");
        ModAuthors = STR("Nicolas");

        Output::send<LogLevel::Verbose>(STR("OT2AP charge !\n"));
    }

    ~OT2APMod() override {}

    // ---- Chemin des fichiers de communication (dans Mods\OT2AP\ap\) ----
    auto ap_path(const wchar_t* filename) -> std::wstring
    {
        return std::wstring(STR("Mods\\OT2AP\\ap\\")) + filename;
    }

    // ---- Donner objet / or ----
    auto give_player_item(const wchar_t* item_label, int32 num) -> void
    {
        m_our_item_call = true;
        UObject* mgr = UObjectGlobals::FindFirstOf(STR("KSSaveDataManagerBP_C"));
        if (!mgr) { m_our_item_call = false; return; }
        UFunction* fn = UObjectGlobals::StaticFindObject<UFunction*>(
            nullptr, nullptr, STR("/Script/Majesty.KSSaveDataManager:AddItemToBackpack"));
        if (!fn) { m_our_item_call = false; return; }
        struct { FName ItemLabel; int32 Num; } params{};
        params.ItemLabel = FName(item_label, FNAME_Add);
        params.Num = num;
        mgr->ProcessEvent(fn, &params);
        m_our_item_call = false;
    }

    auto give_player_money(int32 amount) -> void
    {
        UObject* save = UObjectGlobals::FindFirstOf(STR("KSSaveGameBP_C"));
        if (!save) return;
        UFunction* fn = save->GetFunctionByName(STR("AddPlayerMoney"));
        if (!fn) return;
        struct { int32 AddMoney; } params{};
        params.AddMoney = amount;
        save->ProcessEvent(fn, &params);
    }

    // ---- Persistance du nombre d'items deja donnes ----
    auto load_items_given() -> void
    {
        FILE* file = _wfopen(ap_path(STR("ap_given.txt")).c_str(), STR("r"));
        if (file)
        {
            wchar_t line[32];
            if (fgetws(line, 32, file)) m_items_given = _wtoi(line);
            fclose(file);
        }
    }

    auto save_items_given() -> void
    {
        FILE* file = _wfopen(ap_path(STR("ap_given.txt")).c_str(), STR("w"));
        if (file)
        {
            fwprintf(file, STR("%d\n"), m_items_given);
            fclose(file);
        }
    }

    // ---- Charger les definitions d'items (ap_item_defs.txt) ----
    auto load_item_defs() -> void
    {
        FILE* f = _wfopen(ap_path(STR("ap_item_defs.txt")).c_str(), STR("r"));
        if (!f) { Output::send<LogLevel::Verbose>(STR("[OT2AP] pas de ap_item_defs.txt\n")); return; }
        wchar_t line[256];
        while (fgetws(line, 256, f))
        {
            // Format : ID;type;valeur
            std::wstring s = line;
            size_t p1 = s.find(L';');
            if (p1 == std::wstring::npos) continue;
            size_t p2 = s.find(L';', p1 + 1);
            if (p2 == std::wstring::npos) continue;
            int id = _wtoi(s.substr(0, p1).c_str());
            std::wstring type = s.substr(p1 + 1, p2 - p1 - 1);
            std::wstring val = s.substr(p2 + 1);
            while (!val.empty() && (val.back() == L'\n' || val.back() == L'\r')) val.pop_back();
            m_item_defs[id] = { type, val };
        }
        fclose(f);
        Output::send<LogLevel::Verbose>(STR("[OT2AP] {} definitions d'items chargees\n"), (int)m_item_defs.size());
    }

    // ---- Reconstruire m_unlocked_chars a partir de l'historique des items deja recus ----
    // IMPORTANT : sans ca, m_unlocked_chars est vide a chaque relance du jeu, et
    // apply_starting_character() virerait alors des persos legitimement debloques
    // lors d'une session precedente (bug corrige).
    auto load_unlocked_characters_from_history() -> void
    {
        FILE* file = _wfopen(ap_path(STR("ap_items.txt")).c_str(), STR("r"));
        if (!file) return;
        int line_num = 0;
        wchar_t line[64];
        while (fgetws(line, 64, file) && line_num < m_items_given)
        {
            int ap_id = _wtoi(line);
            auto it = m_item_defs.find(ap_id);
            if (it != m_item_defs.end() && it->second.first == STR("character"))
            {
                m_unlocked_chars.insert(_wtoi(it->second.second.c_str()));
            }
            line_num++;
        }
        fclose(file);
        Output::send<LogLevel::Verbose>(STR("[OT2AP] {} persos deja debloques (historique)\n"), (int)m_unlocked_chars.size());
    }

    // ---- Lire ap_items.txt et donner les nouveaux items ----
    auto process_received_items() -> void
    {
        FILE* file = _wfopen(ap_path(STR("ap_items.txt")).c_str(), STR("r"));
        if (!file) return;
        int line_num = 0;
        wchar_t line[64];
        while (fgetws(line, 64, file))
        {
            if (line_num >= m_items_given)
            {
                int ap_id = _wtoi(line);
                auto it = m_item_defs.find(ap_id);
                if (it != m_item_defs.end())
                {
                    const std::wstring& type = it->second.first;
                    const std::wstring& val = it->second.second;
                    if (type == STR("gold"))
                    {
                        give_player_money(_wtoi(val.c_str()));
                        Output::send<LogLevel::Verbose>(STR("[OT2AP] Or recu : +{}\n"), val);
                    }
                    else if (type == STR("item"))
                    {
                        give_player_item(val.c_str(), 1);
                        Output::send<LogLevel::Verbose>(STR("[OT2AP] Objet recu : {}\n"), val);
                    }
                    else if (type == STR("character"))
                    {
                        int cid = _wtoi(val.c_str());
                        join_character((uint8_t)cid);
                        m_unlocked_chars.insert(cid);
                        heal_character(cid);
                        Output::send<LogLevel::Verbose>(STR("[OT2AP] Personnage recu : ID {}\n"), val);
                    }
                }
                else
                {
                    Output::send<LogLevel::Verbose>(STR("[OT2AP] Item AP {} non defini\n"), ap_id);
                }
            }
            line_num++;
        }
        fclose(file);
        if (line_num > m_items_given)
        {
            m_items_given = line_num;
            save_items_given();
        }
    }

    // ---- Hook coffre : ouverture -> check Archipelago ----
    auto install_chest_hook() -> void
    {
        UFunction* open_fn = UObjectGlobals::StaticFindObject<UFunction*>(
            nullptr, nullptr,
            STR("/Game/Environment/BP/Object/TreasureBoxBP.TreasureBoxBP_C:Open"));
        if (!open_fn) return;

        UObjectGlobals::RegisterHook(
            open_fn,
            [](UnrealScriptFunctionCallableContext& context, void* custom_data) {
                auto* self = static_cast<OT2APMod*>(custom_data);
                UObject* chest = context.Context;
                if (!chest) return;
                UFunction* fn = chest->GetFunctionByNameInChain(STR("PickItem"));
                if (!fn) return;
                struct { FName ItemLabel; int32 ItemCount; uint8 pad[256]; } params{};
                chest->ProcessEvent(fn, &params);
                self->m_pending_loot_label = params.ItemLabel.ToString();
                self->m_pending_loot_num = params.ItemCount;
                if (self->m_pending_loot_label != L"None" && self->m_pending_loot_num > 0)
                {
                    self->m_pending_loot_removal = true;
                    Output::send<LogLevel::Verbose>(STR("[OT2AP] Butin previsionnel : {} x{}\n"),
                        self->m_pending_loot_label, self->m_pending_loot_num);
                }
            },
            [](UnrealScriptFunctionCallableContext& context, void* custom_data) {
                UObject* chest = context.Context;
                if (!chest) return;
                auto* self = static_cast<OT2APMod*>(custom_data);
                auto* label_prop = chest->GetPropertyByNameInChain(STR("m_PlacementLabel"));
                if (!label_prop) return;
                FName label = *label_prop->ContainerPtrToValuePtr<FName>(chest);
                int loc_id = chest_label_to_location_id(label.ToString());
                if (loc_id >= 0)
                {
                    FILE* cf = _wfopen(STR("Mods\\OT2AP\\ap\\ap_checks.txt"), STR("a"));
                    if (cf) { fwprintf(cf, STR("%d\n"), loc_id); fclose(cf); }
                    Output::send<LogLevel::Verbose>(STR("[OT2AP] Coffre {} -> location {}\n"),
                        label.ToString(), loc_id);
                }
                else
                {
                    Output::send<LogLevel::Verbose>(STR("[OT2AP] Coffre non mappe : {}\n"), label.ToString());
                }
            },
            this);
        m_hook_installed = true;
        Output::send<LogLevel::Verbose>(STR("[OT2AP] Hook coffre installe automatiquement !\n"));
    }

    // ---- Detection : vraiment en jeu (pas au menu) ----
    auto is_in_game() -> bool
    {
        UObject* save = UObjectGlobals::FindFirstOf(STR("KSSaveGameBP_C"));
        if (!save) return false;
        auto* loc_prop = save->GetPropertyByNameInChain(STR("PlayerLocation"));
        if (!loc_prop) return false;
        void* loc_ptr = loc_prop->ContainerPtrToValuePtr<void>(save);
        auto* loc_struct = static_cast<FStructProperty*>(loc_prop)->GetStruct();
        auto* levelname_prop = loc_struct->GetPropertyByNameInChain(STR("levelName"));
        if (!levelname_prop) return false;
        FName level_name = *levelname_prop->ContainerPtrToValuePtr<FName>(loc_ptr);
        return level_name != FName(STR("None"), FNAME_Add);
    }

    // ---- Ajouter / retirer un personnage de l'equipe ----
    auto join_character(uint8_t char_id) -> void
    {
        m_our_join_call = true;

        UObject* save = UObjectGlobals::FindFirstOf(STR("KSSaveGameBP_C"));
        if (!save) { m_our_join_call = false; return; }
        UFunction* fn = save->GetFunctionByNameInChain(STR("JoinPlayerCharacterToParty"));
        if (!fn) { m_our_join_call = false; return; }

        struct Params {
            uint8_t CharacterID;
            bool OutResult;
            bool outIsAddMainMember;
        } params;
        params.CharacterID = char_id;
        params.OutResult = false;
        params.outIsAddMainMember = false;

        save->ProcessEvent(fn, &params);
        Output::send<LogLevel::Verbose>(STR("[OT2AP] join_character({}) -> result={} main={}\n"),
            (int)char_id, params.OutResult, params.outIsAddMainMember);

        m_our_join_call = false;
    }

    auto leave_character(int char_id) -> void
    {
        UObject* save = UObjectGlobals::FindFirstOf(STR("KSSaveGameBP_C"));
        if (!save) return;
        UFunction* fn = save->GetFunctionByNameInChain(STR("LeaveCharacterToParty"));
        if (!fn) return;
        struct { uint8_t CharaID; } params{};
        params.CharaID = (uint8_t)char_id;
        save->ProcessEvent(fn, &params);
        Output::send<LogLevel::Verbose>(STR("[OT2AP] leave_character({})\n"), char_id);
    }

    auto heal_character(int char_id) -> void
    {
        UObject* mgr = UObjectGlobals::FindFirstOf(STR("KSCharacterManagerBP_C"));
        if (!mgr) { Output::send<LogLevel::Verbose>(STR("[OT2AP] CharacterManager absent\n")); return; }
        UFunction* fn = mgr->GetFunctionByNameInChain(STR("FullRecoveryCharacter"));
        if (!fn) { Output::send<LogLevel::Verbose>(STR("[OT2AP] FullRecoveryCharacter absente\n")); return; }
        struct { int32 CharacterID; } params{};
        params.CharacterID = char_id;
        mgr->ProcessEvent(fn, &params);
        Output::send<LogLevel::Verbose>(STR("[OT2AP] heal_character({}) via FullRecovery\n"), char_id);
    }

    auto is_in_party(int char_id) -> bool
    {
        UObject* mgr = UObjectGlobals::FindFirstOf(STR("KSSaveDataManagerBP_C"));
        if (!mgr) return false;
        UFunction* fn = mgr->GetFunctionByNameInChain(STR("CheckPartyMainMember"));
        if (!fn) return false;

        struct Params {
            uint8_t CharaID;
            bool IsHit;
            uint8_t pad[512]; // marge de securite pour les variables internes du BP
        } params{};
        params.CharaID = (uint8_t)char_id;

        mgr->ProcessEvent(fn, &params);
        return params.IsHit;
    }

    // ---- Nombre total de scenarios principaux termines (Endroll_ClearedMS) ----
    // Tableau de 50 int32 (-1 = vide). On compte simplement combien de slots sont remplis :
    // c'est un signal fiable et generique (contrairement a l'index precis d'un scenario,
    // dont la correspondance s'est averee peu fiable a l'usage).
    auto count_cleared_scenarios() -> int
    {
        UObject* save = UObjectGlobals::FindFirstOf(STR("KSSaveGameBP_C"));
        if (!save) return 0;
        auto* prop = save->GetPropertyByNameInChain(STR("Endroll_ClearedMS"));
        if (!prop) return 0;
        void* arr_ptr = prop->ContainerPtrToValuePtr<void>(save);
        uint8* raw = reinterpret_cast<uint8*>(arr_ptr);
        int32 array_num = *reinterpret_cast<int32*>(raw + 8);
        uint8* data_ptr = *reinterpret_cast<uint8**>(raw);
        int count = 0;
        for (int i = 0; i < array_num; i++)
        {
            if (*reinterpret_cast<int32*>(data_ptr + i * 4) != -1) count++;
        }
        return count;
    }

    // Combien de segments de prologue un perso a (tous 1, sauf Osvald qui en a 2)
    static auto prologue_segments_needed(int char_id) -> int
    {
        return (char_id == 6) ? 2 : 1; // 6 = Osvald
    }

    // ---- Applique le perso de depart voulu par Archipelago ----
    // On attend que le nombre de scenarios termines augmente d'assez (1 ou 2 pour Osvald)
    // depuis le debut de la partie avant de remplacer le perso naturellement choisi par le
    // joueur : ca garantit que son prologue est fini et qu'on ne casse pas une scene scenarisee.
    auto apply_starting_character() -> bool
    {
        if (m_natural_start_char == 0)
        {
            for (int id = 1; id <= 8; id++)
            {
                if (is_in_party(id)) { m_natural_start_char = id; break; }
            }
            if (m_natural_start_char == 0) return false; // pas encore d'equipe formee
        }

        if (count_cleared_scenarios() < prologue_segments_needed(m_natural_start_char)) return false;

        FILE* f = _wfopen(ap_path(STR("ap_start_char.txt")).c_str(), STR("r"));
        if (!f) return false;
        wchar_t line[32];
        int char_id = 0;
        if (fgetws(line, 32, f)) char_id = _wtoi(line);
        fclose(f);
        if (char_id < 1 || char_id > 8) return false;

        join_character((uint8_t)char_id);
        m_unlocked_chars.insert(char_id);
        heal_character(char_id);

        if (m_natural_start_char != char_id) leave_character(m_natural_start_char);

        Output::send<LogLevel::Verbose>(STR("[OT2AP] Perso de depart applique : ID {}\n"), char_id);
        return true;
    }

    // ---- Hook sur JoinPlayerCharacterToParty : detecte un recrutement du JEU (pas de nous) ----
    // Le hook ne fait QUE marquer un drapeau (aucun appel lourd ici) : le traitement reel se
    // fait dans on_update, car appeler une autre fonction Blueprint depuis l'interieur de ce
    // hook peut faire crasher le jeu (reentrance dans le moteur de script).
    auto install_join_hook() -> void
    {
        UObject* save = UObjectGlobals::FindFirstOf(STR("KSSaveGameBP_C"));
        if (!save) return;
        UFunction* fn = save->GetFunctionByNameInChain(STR("JoinPlayerCharacterToParty"));
        if (!fn) return;

        UObjectGlobals::RegisterHook(
            fn,
            [](UnrealScriptFunctionCallableContext& context, void* custom_data) {
                auto* self = static_cast<OT2APMod*>(custom_data);
                UObject* chest = context.Context;
                if (!chest) return;
                UFunction* fn = chest->GetFunctionByNameInChain(STR("PickItem"));
                if (!fn) return;
                struct { FName ItemLabel; int32 ItemCount; uint8 pad[256]; } params{};
                chest->ProcessEvent(fn, &params);
                self->m_pending_loot_label = params.ItemLabel.ToString();
                self->m_pending_loot_num = params.ItemCount;
                if (self->m_pending_loot_label != L"None" && self->m_pending_loot_num > 0)
                {
                    self->m_pending_loot_removal = true;
                    Output::send<LogLevel::Verbose>(STR("[OT2AP] Butin previsionnel : {} x{}\n"),
                        self->m_pending_loot_label, self->m_pending_loot_num);
                }
            },
            [](UnrealScriptFunctionCallableContext& context, void* custom_data) {
                auto* self = static_cast<OT2APMod*>(custom_data);
                if (self->m_our_join_call) return;
                self->m_pending_recruit_check = true;
            },
            this);
        m_join_hook_installed = true;
        Output::send<LogLevel::Verbose>(STR("[OT2AP] Hook join installe\n"));
    }

    // ---- Detection de la victoire (chapitre final + epilogue termines) ----
    // Endroll_ClearedMS est un tableau de 50 int32 (-1 = vide). L'index 49 correspond a
    // MS_EPI_00 (l'epilogue, tout dernier evenement du jeu, apres avoir vaincu Vide the Wicked).
    auto is_game_completed() -> bool
    {
        UObject* save = UObjectGlobals::FindFirstOf(STR("KSSaveGameBP_C"));
        if (!save) return false;
        auto* prop = save->GetPropertyByNameInChain(STR("Endroll_ClearedMS"));
        if (!prop) return false;
        void* arr_ptr = prop->ContainerPtrToValuePtr<void>(save);
        uint8* raw = reinterpret_cast<uint8*>(arr_ptr);
        int32 array_num = *reinterpret_cast<int32*>(raw + 8);
        uint8* data_ptr = *reinterpret_cast<uint8**>(raw);
        for (int i = 0; i < array_num; i++)
        {
            if (*reinterpret_cast<int32*>(data_ptr + i * 4) == 49) return true;
        }
        return false;
    }

    auto remove_item(const std::wstring& item_label, int32 num) -> void
    {
        UObject* mgr = UObjectGlobals::FindFirstOf(STR("KSSaveDataManagerBP_C"));
        if (!mgr) return;
        UFunction* fn = mgr->GetFunctionByNameInChain(STR("SubItemFromBackpack"));
        if (!fn) return;
        struct { FName ItemLabel; int32 Num; bool ReturnValue; } params{};
        params.ItemLabel = FName(item_label, FNAME_Add);
        params.Num = num;
        mgr->ProcessEvent(fn, &params);
        Output::send<LogLevel::Verbose>(STR("[OT2AP] Objet retire : {} x{} -> success={}\n"), item_label, num, params.ReturnValue);
    }

    auto get_mission_state(int area, int position) -> int
    {
        UObject* save = UObjectGlobals::FindFirstOf(STR("KSSaveGameBP_C"));
        if (!save) return -1;
        auto* prop = save->GetPropertyByNameInChain(STR("SubMissionData"));
        if (!prop) return -1;
        void* arr_ptr = prop->ContainerPtrToValuePtr<void>(save);
        uint8* raw = reinterpret_cast<uint8*>(arr_ptr);
        int32 array_num = *reinterpret_cast<int32*>(raw + 8);
        if (area < 0 || area >= array_num) return -1;
        uint8* data_ptr = *reinterpret_cast<uint8**>(raw);
        uint8* category_ptr = data_ptr + area * 64;
        int32 ms_num = *reinterpret_cast<int32*>(category_ptr + 8);
        if (position < 0 || position >= ms_num) return -1;
        uint8* ms_data = *reinterpret_cast<uint8**>(category_ptr);
        return *reinterpret_cast<int32*>(ms_data + position * 4);
    }

    auto load_quests_checked_from_history() -> void
    {
        FILE* file = _wfopen(ap_path(STR("ap_quests_sent.txt")).c_str(), STR("r"));
        if (!file) return;
        wchar_t line[64];
        while (fgetws(line, 64, file))
        {
            m_quests_checked.insert(_wtoi(line));
        }
        fclose(file);
        Output::send<LogLevel::Verbose>(STR("[OT2AP] {} quetes deja checkees (historique)\n"), (int)m_quests_checked.size());
    }

    auto scan_subquests() -> void
    {
        for (int i = 0; i < subquest_table_count; i++)
        {
            const auto& q = subquest_table[i];
            if (m_quests_checked.count(q.location_id)) continue;
            int state = get_mission_state(q.area, q.position);
            if (state == 2)
            {
                m_quests_checked.insert(q.location_id);
                FILE* cf = _wfopen(ap_path(STR("ap_checks.txt")).c_str(), STR("a"));
                if (cf) { fwprintf(cf, STR("%d\n"), q.location_id); fclose(cf); }
                FILE* qf = _wfopen(ap_path(STR("ap_quests_sent.txt")).c_str(), STR("a"));
                if (qf) { fwprintf(qf, STR("%d\n"), q.location_id); fclose(qf); }
                Output::send<LogLevel::Verbose>(STR("[OT2AP] Quete terminee -> location {}\n"), q.location_id);
            }
        }
    }

    // ---- Boucle principale ----
    auto on_update() -> void override
    {
        m_frame_counter++;
        if (m_frame_counter < 120) return; // ~2 secondes
        m_frame_counter = 0;
        if (!is_in_game()) return; // au menu -> on ne fait rien

        if (!m_defs_loaded)
        {
            load_items_given();
            load_item_defs();
            load_unlocked_characters_from_history(); // AVANT apply_starting_character !
            load_quests_checked_from_history();
            m_defs_loaded = true;
        }
        if (!m_start_char_applied)
        {
            m_start_char_applied = apply_starting_character(); // ne reste "fait" que si vraiment reussi
        }
        if (!m_join_hook_installed && m_start_char_applied) install_join_hook();

        // Un recrutement externe vient d'etre detecte : on identifie qui, mais on NE LE VIRE PAS
        // tout de suite (son propre prologue pourrait en avoir besoin).
        if (m_pending_recruit_check)
        {
            for (int id = 1; id <= 8; id++)
            {
                if (is_in_party(id) && !m_unlocked_chars.count(id) && !m_pending_recruits.count(id))
                {
                    m_pending_recruits.insert(id);
                    m_pending_recruit_baseline[id] = count_cleared_scenarios();
                    Output::send<LogLevel::Verbose>(STR("[OT2AP] Recrutement en attente (prologue) : ID {}\n"), id);
                    break;
                }
            }
            m_pending_recruit_check = false;
        }

        // Pour chaque perso en attente, on verifie si le compteur a assez augmente depuis
        // son recrutement -> alors son propre prologue est termine, on peut le retirer.
        for (auto it = m_pending_recruits.begin(); it != m_pending_recruits.end(); )
        {
            int id = *it;
            int baseline = m_pending_recruit_baseline[id];
            int needed = prologue_segments_needed(id);
            if (count_cleared_scenarios() >= baseline + needed)
            {
                leave_character(id);
                int loc_id = 6560100 + id;
                FILE* cf = _wfopen(ap_path(STR("ap_checks.txt")).c_str(), STR("a"));
                if (cf) { fwprintf(cf, STR("%d\n"), loc_id); fclose(cf); }
                Output::send<LogLevel::Verbose>(STR("[OT2AP] Check recrutement -> location {}\n"), loc_id);
                m_pending_recruit_baseline.erase(id);
                it = m_pending_recruits.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Victoire : chapitre final + epilogue termines
        if (!m_victory_sent && is_game_completed())
        {
            FILE* cf = _wfopen(ap_path(STR("ap_checks.txt")).c_str(), STR("a"));
            if (cf) { fwprintf(cf, STR("%d\n"), 6560200); fclose(cf); }
            Output::send<LogLevel::Verbose>(STR("[OT2AP] VICTOIRE DETECTEE ! Check envoye.\n"));
            m_victory_sent = true;
        }

        if (m_pending_loot_removal)
        {
            remove_item(m_pending_loot_label, m_pending_loot_num);
            m_pending_loot_removal = false;
        }

        scan_subquests();

        process_received_items();
        if (!m_hook_installed) install_chest_hook();
    }

    auto on_unreal_init() -> void override {}
};

#define OT2AP_API __declspec(dllexport)
extern "C"
{
    OT2AP_API RC::CppUserModBase* start_mod() { return new OT2APMod(); }
    OT2AP_API void uninstall_mod(RC::CppUserModBase* mod) { delete mod; }
}