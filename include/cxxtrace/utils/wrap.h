#pragma once
#include <memory>

namespace cxxtrace {
template <typename BeforeAction, typename AfterAction>
class ParametricWrapBase {
protected:
  BeforeAction before_action_;
  AfterAction after_action_;

public:
  ParametricWrapBase(BeforeAction before, AfterAction after)
      : before_action_(before), after_action_(after) {}

  template <typename U> struct CallProxy {
    U &obj;
    BeforeAction before_action_;
    AfterAction after_action_;
    CallProxy(U &obj, BeforeAction before, AfterAction after)
        : obj(obj), before_action_(before), after_action_(after) {
      before_action_();
    }
    ~CallProxy() { after_action_(); }
    U *operator->() { return &obj; }
  };
};

template <typename Pointer, typename BeforeAction, typename AfterAction>
class ParametricWrapPtr : public ParametricWrapBase<BeforeAction, AfterAction> {
  Pointer pointer;

public:
  ParametricWrapPtr(BeforeAction before, AfterAction after)
      : ParametricWrapPtr(nullptr, before, after) {}
  ParametricWrapPtr(Pointer pointer, BeforeAction before, AfterAction after)
      : ParametricWrapBase<BeforeAction, AfterAction>(before, after),
        pointer(std::move(pointer)) {}
  operator Pointer const &() const & { return pointer; }
  operator Pointer &() & { return pointer; }
  operator Pointer &&() && { return std::move(pointer); }
  typename ParametricWrapBase<BeforeAction, AfterAction>::template CallProxy<
      typename std::remove_reference<decltype(*pointer)>::type>
  operator->() {
    return {*pointer, this->before_action_, this->after_action_};
  }
  template <typename U> ParametricWrapPtr &operator=(U other_pointer) {
    pointer = std::move(other_pointer);
    return *this;
  }
};

template <typename Pointer, typename BeforeAction, typename AfterAction>
ParametricWrapPtr<Pointer, BeforeAction, AfterAction>
makeWrap(Pointer pointer, BeforeAction before, AfterAction after) {
  return {std::move(pointer), std::move(before), std::move(after)};
}

template <typename Pointer, typename BeforeAction, typename AfterAction>
ParametricWrapPtr<Pointer, BeforeAction, AfterAction>
makeWrap(BeforeAction before, AfterAction after) {
  return {nullptr, std::move(before), std::move(after)};
}
} // namespace cxxtrace