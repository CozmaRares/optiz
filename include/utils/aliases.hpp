#pragma once

#include <llvm/Support/Casting.h>

#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace als {
    inline auto& StdOut = std::cout;
    inline auto& StdErr = std::cerr;

    template <typename T>
    using Vec = std::vector<T>;

    using String = std::string;

    template <typename T>
    using Box = std::unique_ptr<T>;

    template <typename _Tp, typename... _Args>
    _GLIBCXX23_CONSTEXPR inline std::__detail::__unique_ptr_t<_Tp>
    New(_Args&&... __args) { return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...)); }

    template <typename _Tp>
    [[__nodiscard__, __gnu__::__always_inline__]]
    constexpr typename std::remove_reference<_Tp>::type&&
    Move(_Tp&& __t) noexcept { return static_cast<typename std::remove_reference<_Tp>::type&&>(__t); }

    template <typename _Tp>
    [[__nodiscard__, __gnu__::__always_inline__]]
    constexpr _Tp&&
    Forward(typename std::remove_reference<_Tp>::type& __t) noexcept { return static_cast<_Tp&&>(__t); }

    template <typename... Args>
    inline void Print(std::ostream& out, Args&&... args) { (out << ... << std::forward<Args>(args)); }

    template <typename... Args>
    inline void Print(Args&&... args) { Print(std::cout, std::forward<Args>(args)...); }

    template <typename... Args>
    inline void EPrint(Args&&... args) { Print(std::cerr, std::forward<Args>(args)...); }

    template <typename... Args>
    inline void PrintLn(std::ostream& out, Args&&... args) { (out << ... << std::forward<Args>(args)) << std::endl; }

    template <typename... Args>
    inline void PrintLn(Args&&... args) { PrintLn(std::cout, std::forward<Args>(args)...); }

    template <typename... Args>
    inline void EPrintLn(Args&&... args) { PrintLn(std::cerr, std::forward<Args>(args)...); }

    using OutStream = std::ostream;

    template <typename T>
    using Optional             = std::optional<T>;
    inline constexpr auto None = std::nullopt;

    template <typename To, typename From>
    inline bool IsA(const From& Val) {
        return llvm::isa<To>(Val);
    }

    template <typename To, typename... Ts>
    inline bool IsA(const std::variant<Ts...>& v) {
        return std::holds_alternative<To>(v);
    }

    template <typename To, typename From>
    inline auto Cast(std::unique_ptr<From>&& Val) {
        return llvm::cast<To>(std::move(Val));
    }

    template <typename To, typename... Ts>
    inline To& Cast(std::variant<Ts...>& v) {
        assert(IsA<To>(v) && "Cast<To>(v) called on a variant that does not hold a To");
        return std::get<To>(v);
    }

    template <typename To, typename... Ts>
    inline const To& Cast(const std::variant<Ts...>& v) {
        assert(IsA<To>(v) && "Cast<To>(v) called on a variant that does not hold a To");
        return std::get<To>(v);
    }

    template <typename... Ts>
    using Sum = std::variant<Ts...>;

    template <typename T1, typename T2>
    using Pair = std::pair<T1, T2>;

    template <typename T>
    using ListConstructor = std::initializer_list<T>;

    template <typename K, typename V>
    using Map = std::unordered_map<K, V>;

    using OutStringStream = std::ostringstream;

    inline bool IsAlpha(char c) { return std::isalpha(c); }
    inline bool IsNumeric(char c) { return std::isdigit(c); }
    inline bool IsSpace(char c) { return std::isspace(c); }
    inline bool IsAlphaNumeric(char c) { return std::isalnum(c); }
    inline double ToDouble(als::String str) { return std::stod(str); }
    inline uint ToUint(als::String str) { return std::stoul(str); }

    template <typename E>
    struct Failure {
        E val;
        explicit Failure(E v) : val(std::move(v)) {}
    };

    template <typename T, typename E>
    class Result {
        Sum<T, Failure<E>> m_Data;

        explicit Result(T&& val) : m_Data(Move(val)) {}
        explicit Result(Failure<E>&& err) : m_Data(Move(err)) {}

        Result(Result&&)                 = default;
        Result& operator=(Result&&)      = default;
        Result(const Result&)            = delete;
        Result& operator=(const Result&) = delete;

    public:
        static Result Ok(T val) {
            return Result(Move(val));
        }

        static Result Err(E err) {
            return Result(Failure<E>(Move(err)));
        }

        bool IsOk() const { return IsA<T>(m_Data); }

        T& Value() {
            assert(IsOk() && "Called Value() on an Err result");
            return std::get<T>(m_Data);
        }

        E& Error() {
            assert(!IsOk() && "Called Error() on an Ok result");
            return std::get<Failure<E>>(m_Data).val;
        }
    };

}  // namespace als
