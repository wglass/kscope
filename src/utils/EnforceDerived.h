template<class Type, class Base>
struct EnforceDerived {
  static void constraints(Type * ptr) {
    Base * base_ptr = ptr;
  }
  EnforceDerived() {
    void(*ptr)(Type*) = constraints;
  }
};
