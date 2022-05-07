#pragma once

namespace rage {
	class datBase {
	public:
		virtual ~datBase() = default;
	};
	class pgBase {
	public:
		virtual ~pgBase() = default;
	private:
		void* m_pgunk;
	};
	template <typename T, typename Base = datBase>
	class atDNode : public Base {
	public:
		T m_data;
		void* m_unk;
		atDNode<T, Base>* m_next;
	};
	template <typename Node>
	class atDList {
	public:
		Node* m_head;
		Node* m_tail;
	};
}