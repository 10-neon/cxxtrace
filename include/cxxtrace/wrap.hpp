#pragma once
#include <memory>

namespace neon {
template <typename BeforeAction, typename AfterAction>
class ParametricWrapBase {
   protected:
    BeforeAction before_action_;
    AfterAction after_action_;

   public:
    ParametricWrapBase(BeforeAction before, AfterAction after)
        : before_action_(before), after_action_(after) {}

    template <typename U, typename Param>
    struct CallProxy {
        U &obj;
        Param param;
        BeforeAction before_action_;
        AfterAction after_action_;
        CallProxy(U &obj, BeforeAction before, AfterAction after,
                  Param const &param)
            : obj(obj),
              before_action_(before),
              after_action_(after),
              param{param} {
            before_action_(param);
        }
        ~CallProxy() { after_action_(param); }
        U *operator->() { return &obj; }
    };
};

template <typename Pointer, typename BeforeAction, typename AfterAction,
          typename Param>
class ParametricWrapPtr : public ParametricWrapBase<BeforeAction, AfterAction> {
    Pointer pointer;
    Param param;

   public:
    ParametricWrapPtr(BeforeAction before, AfterAction after,
                      Param const &param)
        : ParametricWrapPtr(nullptr, before, after), param{param} {}
    ParametricWrapPtr(Pointer pointer, BeforeAction before, AfterAction after,
                      Param const &param)
        : ParametricWrapBase<BeforeAction, AfterAction>(before, after),
          pointer(std::move(pointer)),
          param{param} {}
    operator Pointer const &() const & { return pointer; }
    operator Pointer &() & { return pointer; }
    operator Pointer &&() && { return std::move(pointer); }
    typename ParametricWrapBase<BeforeAction, AfterAction>::template CallProxy<
        typename std::remove_reference<decltype(*pointer)>::type, Param>
    operator->() {
        return {*pointer, this->before_action_, this->after_action_, param};
    }
    template <typename U>
    ParametricWrapPtr &operator=(U other_pointer) {
        pointer = std::move(other_pointer);
        return *this;
    }
};

}  // namespace neon
