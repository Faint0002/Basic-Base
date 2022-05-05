#pragma once
#include <string>
#include <vector>
#include <map>

namespace Base::Helpers::Label {
	//struct Label { std::string lblName; std::string lblContents; };
	class LabelManager {
	public:
		std::string SetLabel(std::string name, std::string ogContents) {
			for (auto& lbl : m_LabelArray) {
				auto lblTable = lbl.first;
				if (lblTable.first == name)
					return lblTable.second;
				else
					return ogContents;
			}
		}
		void AddLabel(std::string name, std::string contents) {
			m_LabelArray.insert({ { name, contents }, "null" });
		}
		void RemoveLabel(std::string name) {
			for (auto& lbl : m_LabelArray) {
				auto lblTable = lbl.first;
				if (lblTable.first == name)
					m_LabelArray.erase(lblTable);
			}
		}
	public:
		std::map<std::pair<std::string, std::string>, std::string> m_LabelArray;
	};
}

inline Base::Helpers::Label::LabelManager g_LabelManager;