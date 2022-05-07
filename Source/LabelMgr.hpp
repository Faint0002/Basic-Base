#pragma once
#include <string>
#include <vector>
#include <map>

namespace Base::Helpers::Label {
	//struct Label { std::string lblName; std::string lblContents; };
	class LabelManager {
	public:
		void AddLabel(std::string name, std::string contents) {
			m_LabelArray.insert({ name, contents });
		}
		void RemoveLabel(std::string name) {
			m_LabelArray.erase(name);
		}
	public:
		std::map<std::string, std::string> m_LabelArray;
	};
}

inline Base::Helpers::Label::LabelManager g_LabelManager;