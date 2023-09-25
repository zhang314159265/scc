#pragma once

#include <vector>

class Type {
 public:
  explicit Type(int size) : size_(size) { }
  virtual int size() const { return size_; }
  // type singletons
  static Type INT, DOUBLE, BOOL;
 private:
  int size_;
};

class ArrayType : public Type {
 public:
  explicit ArrayType(Type* baseType, const std::vector<int>& dims, int off = 0) : Type(-1) {
    assert(baseType);
    assert(off < dims.size());
    size_ = dims[off];
    if (off == dims.size() - 1) {
      of_ = baseType;
    } else {
      // TODO avoid mem leak
      of_ = new ArrayType(baseType, dims, off + 1);
    }
  }

  int size() const override {
    return of_->size() * size_;
  }
  Type* of() const {
    return of_;
  }
 private:
  int size_; // number of element
  Type* of_;
};

static inline bool typeEq(Type* lhs, Type* rhs) {
  // TODO can we allow non-singleton types?
  return lhs == rhs;
}

static inline bool isArrayType(Type* ty) {
  return dynamic_cast<ArrayType*>(ty) != nullptr;
}
