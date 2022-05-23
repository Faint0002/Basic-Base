#include "Gui.hpp"
#include "Script.hpp"
#include "Globals.hpp"
#include "Memory/PatternMgr.hpp"

namespace Base::Core::DirectX {
	void Gui::GuiInit() {
		auto Io = ImGui::GetIO();
		auto Style = ImGui::GetStyle();
		auto Colors = Style.Colors;
		ImGui::StyleColorsClassic();
		ImFontConfig Config; Config.FontDataOwnedByAtlas = false;
		//Arial Font Names: Bold Italic = ArialBI | Bold = ArialBD | Black = ArialBLK | Italic = ArialI | Regular = Arial
		m_Font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 17.f, &Config);
		m_DefaultImGuiFont = ImGui::GetIO().Fonts->AddFontDefault(&Config);
	}
	bool setSpawnGlobal = false;
	void Gui::GuiWindow() {
		if (ImGui::Begin("Base", &g_Gui->m_Opened)) {
			ImGui::BeginTabBar("MainBar");
			if (ImGui::BeginTabItem("Self")) {
				static int entAlpha = 255;
				if (ImGui::SliderInt("Alpha", &entAlpha, 0, 255)) {
					JobStart(=) {
						ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), entAlpha, FALSE);
					} JobEnd;
				}
				if (ImGui::Button("Refill health")) {
					JobStart(=) {
						ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), 330, NULL);
					} JobEnd;
				}
				if (ImGui::Button("Suicide")) {
					JobStart(=) {
						ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), 0, NULL);
					} JobEnd;
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Players")) {
				static CNetGamePlayer* selectedPlayer;
				if (ImGui::ListBoxHeader("##PlayerList", ImVec2(200.f, 250.f))) {
					auto plyrMgr = *g_Pointers->m_NetworkPlayerMgr;
					if (plyrMgr != nullptr) {
						for (auto plyr : plyrMgr->m_player_list) {
							if (plyr != nullptr) {
								if (plyr->is_valid()) {
									if (ImGui::Selectable(plyr->get_name(), (selectedPlayer == plyr))) {
										selectedPlayer = plyr;
										g_SelectedPlayer = plyr->m_player_id;
									}
								}
							}
						}
					}
					else {
						ImGui::Text("No valid players!");
					}
					ImGui::ListBoxFooter();
				}
				if (selectedPlayer != nullptr) {
					ImGui::SameLine();
					if (ImGui::BeginChild("##PlayerOptions", ImVec2(200.f, 250.f))) {
						ImGui::BeginTabBar("PlayerOptionsBar");
						if (ImGui::BeginTabItem("Teleports")) {
							if (ImGui::Button("Teleport To Player")) {
								JobStart(=) {
									Entity handle;
									auto coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(g_SelectedPlayer), false);
									PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false) ?
										handle = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()) :
										handle = PLAYER::PLAYER_PED_ID();
									ENTITY::SET_ENTITY_COORDS(handle, coords, true, true, true, false);
								} JobEnd;
							}
							if (ImGui::Button("Desync Player")) {
								JobStart(=) {
									auto cPed = selectedPlayer->m_player_info->m_ped;
									auto pPed = g_Pointers->m_ConvertHandle(cPed);
									(*g_Pointers->m_NetworkPlayerMgr)->RemovePlayer(selectedPlayer);
									WEAPON::REMOVE_ALL_PED_WEAPONS(pPed, true);
								} JobEnd;
							}
							ImGui::EndTabItem();
						}
						ImGui::EndTabBar();
						ImGui::EndChild();
					}
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Vehicle")) {
				static char vehicleName[64] = "";
				static char plateText[9] = "";
				ImGui::Checkbox("Set MP Vehicle(s) Bypass", &setSpawnGlobal);
				ImGui::Separator();
				ImGui::InputText("Plate Text", plateText, sizeof(plateText));
				ImGui::InputText("Vehicle Name", vehicleName, sizeof(vehicleName));
				ImGui::Separator();
				if (ImGui::Button("Spawn")) {
					JobStart(=) {
						if (setSpawnGlobal)
							*globals(4533757).as<bool*>() = setSpawnGlobal;
						auto hash = rage::joaat(vehicleName);
						while (!STREAMING::HAS_MODEL_LOADED(hash)) {
							STREAMING::REQUEST_MODEL(hash);
							fiber::cur()->wait(10ms);
						}
						auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
						auto veh = VEHICLE::CREATE_VEHICLE(hash, pos, 0.f, true, false);
						if (NETWORK::NETWORK_IS_SESSION_ACTIVE()) {
							DECORATOR::DECOR_SET_INT(veh, "MPBitset", 0);
							DECORATOR::DECOR_SET_INT(veh, "Player_Vehicle", NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(PLAYER::PLAYER_ID()));
							ENTITY::SET_ENTITY_CLEANUP_BY_ENGINE_(veh, TRUE);
							auto netId = NETWORK::VEH_TO_NET(veh);
							if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(veh))
								NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(netId, true);
						}
						VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh, std::string(plateText).c_str());
						VEHICLE::SET_VEHICLE_ENGINE_ON(veh, true, true, true);
						PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), veh, -1);
						VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);
						for (int i = 0; i < 49; i++) {
							if (i > 16 && i < 23)
								continue;
							VEHICLE::SET_VEHICLE_MOD(veh, i, VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
						}
						STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
					} JobEnd;
				}
				ImGui::EndTabItem();
			}
			Demos::demoTab();
			if (ImGui::BeginTabItem("Settings")) {
				if (ImGui::TreeNode("Information")) {
					ImGui::Text("FPS: %i", Information::frameCount);
					if (Information::inSessionCheck) {
						ImGui::Text("In Session: Yes");
						ImGui::Text(fmt::format("Player Count: {}/{}", Information::playerCount, 32).c_str());
						ImGui::Text(fmt::format("Host Name: {}", Information::hostName).c_str());
					}
					else {
						ImGui::Text("In Session: No");
						ImGui::Text("Player Count: Offline");
						ImGui::Text("Host Name: Offline");
						ImGui::Text(fmt::format("Your PID: {}", Information::selfPlayerId).c_str());
						ImGui::Text(fmt::format("Your Name: {}", Information::selfName).c_str());
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Wait Function")) {
					static int time = 0;
					ImGui::InputInt("Time (s)", &time, 1);
					ImGui::Separator();
					if (ImGui::Button("Send Sleep")) {
						Sleep(time * 1000);
					}
					ImGui::TreePop();
				}
				ImGui::Separator();
				if (ImGui::Button("Unload")) {
					g_Running = false;
				}
				if (ImGui::Button("Exit")) {
					exit(0);
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
			ImGui::End();
		}
	}
	void Gui::GuiTick() {
		if (m_Opened) {
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
		}
		Information::doInformationTick();
	}
	void Gui::GuiLoop() {
		while (true) {
			g_Gui->GuiTick();
			fiber::cur()->wait();
		}
	}
}