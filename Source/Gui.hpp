#pragma once
#include "Includes.hpp"
#include "Script.hpp"
#include "Natives.hpp"
#define ARRAY(arr) sizeof(arr) / sizeof(*arr) //A simple helper macro for arrays

namespace Base::Core::DirectX {
	namespace Information {
		inline int frameCount;
		inline bool inSessionCheck;
		inline int playerCount;
		inline std::string hostName;
		inline int selfPlayerId;
		inline std::string selfName;
		inline void doInformationTick() {
			frameCount = (int)ImGui::GetIO().Framerate;
			inSessionCheck = NETWORK::NETWORK_IS_SESSION_STARTED();
			playerCount = inSessionCheck ? (*g_Pointers->m_NetworkPlayerMgr)->m_player_count : 0;
			hostName = PLAYER::GET_PLAYER_NAME(NETWORK::NETWORK_GET_HOST_OF_SCRIPT("freemode", -1, 0));
			selfPlayerId = PLAYER::PLAYER_ID();
			selfName = PLAYER::GET_PLAYER_NAME(selfPlayerId);
		}
	}
	namespace Demos {
		static bool demoBool = false;
		static int demoInt = 0;
		static float demoFloat = 1.f;
		static char demoText[64] = "";
		static const char* demoCombo[] = { "One", "Two", "Three" };
		static int demoComboPosition = 0;
		inline void demoTab() {
			if (ImGui::BeginTabItem("Demo(s)")) {
				ImGui::BeginTabBar("DemoBar");
				if (ImGui::BeginTabItem("Basic Demo(s)")) {
					ImGui::Checkbox("Bool", &demoBool);
					ImGui::SliderInt("Int", &demoInt, 0, 10);
					ImGui::SliderFloat("Float", &demoFloat, 0.f, 10.f);
					ImGui::InputText("Text", demoText, sizeof(demoText));
					ImGui::Combo("Combo", &demoComboPosition, demoCombo, ARRAY(demoCombo));
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Advanced Demo(s)")) {
					static const char* selectedContent = "";
					if (ImGui::ListBoxHeader("Lists", ImVec2(200.f, 250.f))) {
						for (auto& content : demoCombo) {
							if (ImGui::Selectable(content, (selectedContent == content))) {
								selectedContent = content;
							}
						}
						ImGui::ListBoxFooter();
					}
					if (ImGui::TreeNode("Sections")) {
						for (std::string content : demoCombo) {
							if (ImGui::TreeNode(std::string("Section " + content).c_str())) {
								if (ImGui::SmallButton(std::string("Small button " + content).c_str()))
									logEntry(LogColor::White, "Tests", "This contains {}.", content);
								if (ImGui::Button(std::string("Normal Button " + content).c_str()))
									logEntry(LogColor::White, "Tests", "This contains {}.", content);
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					if (ImGui::BeginChild("Childs", ImVec2(200.f, 200.f))) {
						for (int i = 0; i < 5; i++) {
							ImGui::Button("Button");
						}
						ImGui::EndChild();
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
				ImGui::EndTabItem();
			}
		}
	}
	class Gui {
	public:
		Gui() = default;
		~Gui() = default;
	public:
		void GuiInit();
		void GuiWindow();
		void GuiTick();
		static void GuiLoop();
	public:
		bool m_Opened;
	public:
		ImFont* m_Font;
		ImFont* m_DefaultImGuiFont;
	};
}
inline std::unique_ptr<Base::Core::DirectX::Gui> g_Gui;