#pragma once

#include <iostream>
#include <vector>
#include <memory>

class Emitter;

namespace ast {

class Node {
 public:
 	static void indent(int depth) {
		for (int i = 0; i < depth; ++i) {
			std::cout << " ";
		}
	}
  virtual void dump(int depth = 0) {
		std::cout << "dump... " << typeid(*this).name() << std::endl;
	}
	virtual ~Node() = default;

	/*
	 * The method fatal if pointer of this object can not be converted to type T*
	 */
	template <typename T>
	T* to() {
		#if 1
		T* ptr = (T*) this;
		#else
		T* ptr = dynamic_cast<T*>(this);
		#endif
		assert(ptr); // make sure the type conversion succeed
		return ptr;
	}

  template <typename T>
  static T* to(Node* ptr) {
    return ptr == nullptr ? nullptr : ptr->to<T>();
  }
 private:
};

// TODO enforce ElemType is a Node subclass
// The line below does not work maybe because multi inheritance used in class Expr
// template <typename ElemType, std::enable_if_t<std::is_base_of<Node, ElemType>::value, void>* = nullptr>
template <typename ElemType>
class NodeList {
 public:
  NodeList() { }
	virtual NodeList& addElem(ElemType* elemPtr) {
		auto up = std::unique_ptr<ElemType>(elemPtr);
		list_.emplace_back(std::move(up));
		return *this;
	}
 protected:
 	/*
	 * NodeList is usually used as baseclass for Node subclasses.
	 * tagStr is used to identify the specific Node subclass.
	 */
  void dumpWithTag(const std::string& tagStr, int depth) {
		Node::indent(depth);
		std::cout << tagStr << std::endl;
		for (auto& item : list_) {
			assert(item);
			item->dump(depth + 1);
		}
	}
  std::vector<std::unique_ptr<ElemType>> list_;
};

}
