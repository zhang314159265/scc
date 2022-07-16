#pragma once

class Type {
 public:
  explicit Type(int size) : size_(size) { }
  int size() const { return size_; }
  // type singletons
  static Type INT, DOUBLE;
 private:
  int size_;
};
