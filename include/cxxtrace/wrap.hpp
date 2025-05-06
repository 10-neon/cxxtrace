#pragma once
#include <memory>
#include <type_traits>
namespace neon {

template <typename Ptr, typename Before, typename After, typename Context>
class WrapPtr {
   protected:
    Ptr ptr_;
    Before before_;
    After after_;
    Context ctx_;

   public:
    WrapPtr(Before&& before, After&& after, Context&& ctx)
        : ptr_{nullptr},
          before_(std::forward<Before>(before)),
          after_(std::forward<After>(after)),
          ctx_(std::forward<Context>(ctx)) {}
    WrapPtr(Ptr ptr, Before&& before, After&& after, Context&& ctx)
        : ptr_{std::forward<Ptr>(ptr)},
          before_(std::forward<Before>(before)),
          after_(std::forward<After>(after)),
          ctx_(std::forward<Context>(ctx)) {}

    struct CallProxy {
        WrapPtr& wrap_;
        CallProxy(WrapPtr& wrap) : wrap_(wrap) { wrap_.before_(wrap_.ctx_); }
        ~CallProxy() { wrap_.after_(wrap_.ctx_); }
        auto operator->() { return &(*wrap_.ptr_); }
    };

    operator bool() const { return ptr_ != nullptr; }
    operator Ptr const&() const& { return ptr_; }
    operator Ptr&() & { return ptr_; }
    operator Ptr&&() && { return std::move(ptr_); }
    Ptr& ptr() { return ptr_; }
    CallProxy operator->() { return {*this}; }
    WrapPtr& operator=(Ptr other) {
        ptr_ = std::move(other);
        return *this;
    }
};

template <typename Ptr, typename Before, typename After, typename Context>
WrapPtr<Ptr, Before, After, Context> wrap(Ptr&& ptr, Before&& before,
                                          After&& after, Context&& ctx) {
    return WrapPtr<Ptr, Before, After, Context>{
        std::forward<Ptr>(ptr), std::forward<Before>(before),
        std::forward<After>(after), std::forward<Context>(ctx)};
}

}  // namespace neon
