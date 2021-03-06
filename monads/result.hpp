#pragma once

#include <monads/either.hpp>

namespace monad
{
   // clang-format off
   template <class value_, class error_>
      requires (!(std::is_reference_v<value_> || std::is_reference_v<error_>))
   class result;
   // clang-format on

   namespace detail
   {
      template <class in_value_, class in_error_>
      constexpr auto ensure_result_error(const result<in_value_, in_error_>& e, in_error_)
         -> result<in_value_, in_error_>
      {
         return e;
      }
      template <class in_value_, class in_error_>
      constexpr auto ensure_result_error(result<in_value_, in_error_>&& e, in_error_ &&)
         -> result<in_value_, in_error_>
      {
         return e;
      }

      template <class in_value_, class in_error_>
      constexpr auto ensure_result_value(const result<in_value_, in_error_>& e, in_value_)
         -> result<in_value_, in_error_>
      {
         return e;
      }
      template <class in_value_, class in_error_>
      constexpr auto ensure_result_value(result<in_value_, in_error_>&& e, in_value_ &&)
         -> result<in_value_, in_error_>
      {
         return e;
      }
   } // namespace detail

   template <class any_>
   struct value_t
   {
      any_ value;
   };

   template <class any_>
   struct error_t
   {
      any_ value;
   };

   template <class any_>
   constexpr auto make_value(any_&& value) -> value_t<std::decay_t<any_>>
   {
      return value_t<std::decay_t<any_>>{std::forward<any_>(value)};
   }

   template <class any_>
   constexpr auto make_error(any_&& value) -> error_t<std::decay_t<any_>>
   {
      return error_t<std::decay_t<any_>>{std::forward<any_>(value)};
   }

   // clang-format off
   template <class value_, class error_> 
      requires (!(std::is_reference_v<value_> || std::is_reference_v<error_>))
   class result
   // clang-format on
   {
      template <class first_, class second_, class dummy_ = void>
      class storage
      {
      public:
         using value_type = first_;
         using error_type = second_;

      private:
         using storage_type =
            std::array<std::byte, detail::max(sizeof(value_type), sizeof(error_type))>;

         // clang-format off
         static inline constexpr bool is_nothrow_default_constructible =
            std::is_nothrow_default_constructible_v<value_type> && 
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_value_constructible =
            std::is_nothrow_copy_assignable_v<value_type> &&
            std::is_nothrow_copy_constructible_v<value_type> && 
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_move_value_constructible =
            std::is_nothrow_move_assignable_v<value_type> &&
            std::is_nothrow_move_constructible_v<value_type> &&
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_error_constructible =
            std::is_nothrow_copy_assignable_v<error_type> &&
            std::is_nothrow_copy_constructible_v<error_type> && 
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_move_error_constructible = 
            std::is_nothrow_move_assignable_v<error_type> &&
            std::is_nothrow_move_constructible_v<error_type> &&
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_constructible =
            is_nothrow_copy_value_constructible && 
            is_nothrow_copy_error_constructible;

         static inline constexpr bool is_nothrow_move_constructible =
            is_nothrow_move_value_constructible && 
            is_nothrow_move_error_constructible;

         static inline constexpr bool is_nothrow_destructible = 
            std::is_nothrow_destructible_v<value_type> &&
            std::is_nothrow_destructible_v<error_type> &&
            std::is_nothrow_destructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_assignable =
            is_nothrow_copy_constructible &&
            is_nothrow_destructible;

         static inline constexpr bool is_nothrow_move_assignable =
            is_nothrow_move_constructible &&
            is_nothrow_destructible;

         static inline constexpr bool is_nothrow_value_value_copyable =
            std::is_nothrow_copy_assignable_v<value_type> &&
            std::is_nothrow_copy_constructible_v<value_type>;

         static inline constexpr bool is_nothrow_value_value_movable =
            std::is_nothrow_move_assignable_v<value_type> &&
            std::is_nothrow_move_constructible_v<value_type>;

         static inline constexpr bool is_nothrow_error_value_copyable =
            std::is_nothrow_copy_assignable_v<error_type> &&
            std::is_nothrow_copy_constructible_v<error_type>;

         static inline constexpr bool is_nothrow_error_value_movable =
            std::is_nothrow_move_assignable_v<error_type> &&
            std::is_nothrow_move_constructible_v<error_type>;
         // clang-format on

      public:
         constexpr storage() noexcept(is_nothrow_default_constructible)
         {
            std::construct_at(std::addressof(value()), value_type{});
         };
         constexpr storage(const value_t<value_type>& l) noexcept(
            is_nothrow_copy_value_constructible)
         {
            std::construct_at(std::addressof(value()), l.value);
         }
         constexpr storage(value_t<value_type>&& l) noexcept(is_nothrow_move_value_constructible)
         {
            std::construct_at(std::addressof(value()), std::move(l.value));
         }
         constexpr storage(const error_t<error_type>& r) noexcept(
            is_nothrow_copy_error_constructible) :
            m_is_error{true}
         {
            std::construct_at(std::addressof(error()), r.value);
         }
         constexpr storage(error_t<error_type>&& r) noexcept(is_nothrow_move_error_constructible) :
            m_is_error{true}
         {
            std::construct_at(std::addressof(error()), std::move(r.value));
         }
         constexpr storage(const storage& rhs) noexcept(is_nothrow_copy_constructible) :
            m_is_error{rhs.m_is_error}
         {
            if (is_value())
            {
               std::construct_at(std::addressof(value()), rhs.value());
            }
            else
            {
               std::construct_at(std::addressof(error()()), rhs.error());
            }
         }
         constexpr storage(storage&& rhs) noexcept(is_nothrow_move_constructible) :
            m_is_error{rhs.m_is_error}
         {
            if (is_value())
            {
               std::construct_at(std::addressof(value()), std::move(rhs.value()));
            }
            else
            {
               std::construct_at(std::addressof(error()), std::move(rhs.error()));
            }

            rhs.m_is_error = false;
         }
         ~storage() noexcept(is_nothrow_destructible)
         {
            if (is_value())
            {
               std::destroy_at(std::addressof(value()));
            }
            else
            {
               std::destroy_at(std::addressof(error()));
            }
         }

         constexpr auto operator=(const storage& rhs) -> storage&
         {
            if (this != &rhs)
            {
               if (is_value())
               {
                  std::destroy_at(std::addressof(value()));
               }
               else
               {
                  std::destroy_at(std::addressof(error()));
               }

               m_is_error = rhs.m_is_error;

               if (is_value())
               {
                  std::construct_at(std::addressof(value()), rhs.value());
               }
               else
               {
                  std::construct_at(std::addressof(error()), rhs.error());
               }
            }

            return *this;
         }
         constexpr auto operator=(storage&& rhs) noexcept(is_nothrow_move_assignable) -> storage&
         {
            if (this != &rhs)
            {
               if (is_value())
               {
                  std::destroy_at(std::addressof(value()));
               }
               else
               {
                  std::destroy_at(std::addressof(error()));
               }

               m_is_error = rhs.m_is_error;
               rhs.m_is_error = false;

               if (is_value())
               {
                  std::construct_at(std::addressof(value()), rhs.value());
               }
               else
               {
                  std::construct_at(std::addressof(error()), rhs.error());
               }
            }

            return *this;
         }

         [[nodiscard]] constexpr auto is_value() const noexcept -> bool { return !m_is_error; };

         constexpr auto value() & noexcept -> value_type& { return *v_pointer(); }
         constexpr auto value() const& noexcept(is_nothrow_value_value_copyable)
            -> const value_type&
         {
            return *v_pointer();
         }
         constexpr auto value() && noexcept(is_nothrow_value_value_movable) -> value_type&&
         {
            return std::move(*v_pointer());
         }
         constexpr auto value() const&& noexcept(is_nothrow_value_value_movable)
            -> const value_type&&
         {
            return std::move(*v_pointer());
         }

         constexpr auto error() & noexcept -> error_type& { return *e_pointer(); }
         constexpr auto error() const& noexcept(is_nothrow_error_value_copyable)
            -> const error_type&
         {
            return *e_pointer();
         }
         constexpr auto error() && noexcept(is_nothrow_error_value_movable) -> error_type&&
         {
            return std::move(*e_pointer());
         }
         constexpr auto error() const&& noexcept(is_nothrow_error_value_movable)
            -> const error_type&&
         {
            return std::move(*e_pointer());
         }

      private:
         constexpr auto v_pointer() noexcept -> value_type*
         {
            return reinterpret_cast<value_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto v_pointer() const noexcept -> const value_type*
         {
            return reinterpret_cast<const value_type*>(m_bytes.data()); // NOLINT
         }

         constexpr auto e_pointer() noexcept -> error_type*
         {
            return reinterpret_cast<error_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto e_pointer() const noexcept -> const error_type*
         {
            return reinterpret_cast<const error_type*>(m_bytes.data()); // NOLINT
         }

      private:
         alignas(detail::max(alignof(value_type), alignof(error_type))) storage_type m_bytes{};
         bool m_is_error{false};
      };

      template <class first_, class second_>
      class storage<first_, second_, std::enable_if_t<trivial<first_> && trivial<second_>>>
      {
      public:
         using value_type = first_;
         using error_type = second_;

      private:
         using storage_type =
            std::array<std::byte, detail::max(sizeof(value_type), sizeof(error_type))>;

      public:
         constexpr storage() noexcept { std::construct_at(std::addressof(value()), value_type{}); };
         constexpr storage(const value_t<value_type>& l) noexcept
         {
            std::construct_at(std::addressof(value()), l.value);
         }
         constexpr storage(value_t<value_type>&& l) noexcept
         {
            std::construct_at(std::addressof(value()), std::move(l.value));
         }
         constexpr storage(const error_t<error_type>& r) noexcept : m_is_error{true}
         {
            std::construct_at(std::addressof(error()), r.value);
         }
         constexpr storage(error_t<error_type>&& r) noexcept : m_is_error{true}
         {
            std::construct_at(std::addressof(error()), std::move(r.value));
         }

         constexpr auto value() & noexcept -> value_type& { return *v_pointer(); }
         constexpr auto value() const& noexcept -> const value_type& { return *v_pointer(); }
         constexpr auto value() && noexcept -> value_type&& { return std::move(*v_pointer()); }
         constexpr auto value() const&& noexcept -> const value_type&&
         {
            return std::move(*v_pointer());
         }

         constexpr auto error() & noexcept -> error_type& { return *e_pointer(); }
         constexpr auto error() const& noexcept -> const error_type& { return *e_pointer(); }
         constexpr auto error() && noexcept -> error_type&& { return std::move(*e_pointer()); }
         constexpr auto error() const&& noexcept -> const error_type&&
         {
            return std::move(*e_pointer());
         }

         [[nodiscard]] constexpr auto is_value() const noexcept -> bool { return !m_is_error; };

      private:
         constexpr auto v_pointer() noexcept -> value_type*
         {
            return reinterpret_cast<value_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto v_pointer() const noexcept -> const value_type*
         {
            return reinterpret_cast<const value_type*>(m_bytes.data()); // NOLINT
         }

         constexpr auto e_pointer() noexcept -> error_type*
         {
            return reinterpret_cast<error_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto e_pointer() const noexcept -> const error_type*
         {
            return reinterpret_cast<const error_type*>(m_bytes.data()); // NOLINT
         }

      private:
         alignas(detail::max(alignof(value_type), alignof(error_type))) storage_type m_bytes{};
         bool m_is_error{false};
      };

   public:
      using value_type = value_;
      using error_type = error_;

   private:
      using storage_type = storage<value_type, error_type>;

      static constexpr bool is_nothrow_value_copy_constructible =
         std::is_nothrow_constructible_v<storage_type, value_t<value_type>>;

      static constexpr bool is_nothrow_value_move_constructible =
         std::is_nothrow_constructible_v<storage_type, value_t<value_type>&&>;

      static constexpr bool copyable = std::copyable<value_type> && std::copyable<error_type>;
      static constexpr bool movable = std::movable<value_type> && std::movable<error_type>;

      template <class any_>
      using map_value_type = std::invoke_result_t<any_, value_type>;

      template <class fun_>
      using map_value_result = result<map_value_type<fun_>, error_type>;

      template <class any_>
      using map_error_type = std::invoke_result_t<any_, error_type>;

      template <class fun_>
      using map_error_result = result<value_type, map_error_type<fun_>>;

      template <class value_fun, class error_fun>
      using join_result = std::common_type_t<map_value_type<value_fun>, map_error_type<error_fun>>;

   public:
      constexpr result(const value_t<value_type>& value) noexcept(
         is_nothrow_value_copy_constructible) :
         m_storage{value}
      {}
      constexpr result(value_t<value_type>&& value) noexcept(is_nothrow_value_move_constructible) :
         m_storage{std::move(value)}
      {}
      constexpr result(const error_t<error_type>& error) : m_storage{error} {}
      constexpr result(error_t<error_type>&& error) : m_storage{std::move(error)} {}

      [[nodiscard]] constexpr auto is_value() const -> bool { return m_storage.is_value(); }
      constexpr operator bool() const { return is_value(); }

      constexpr auto value() const& -> maybe<value_type> requires copyable
      {
         return is_value() ? make_maybe(m_storage.value()) : none;
      }
      constexpr auto value() & -> maybe<value_type> requires copyable
      {
         return is_value() ? make_maybe(m_storage.value()) : none;
      }
      constexpr auto value() const&& -> maybe<value_type> requires movable
      {
         return is_value() ? make_maybe(std::move(m_storage.value())) : none;
      }
      constexpr auto value() && -> maybe<value_type> requires movable
      {
         return is_value() ? make_maybe(std::move(m_storage.value())) : none;
      }

      constexpr auto error() const& -> maybe<error_type> requires copyable
      {
         return is_value() ? none : make_maybe(m_storage.error());
      }
      constexpr auto error() & -> maybe<error_type> requires copyable
      {
         return is_value() ? none : make_maybe(m_storage.error());
      }
      constexpr auto error() const&& -> maybe<error_type> requires movable
      {
         return is_value() ? none : make_maybe(std::move(m_storage.error()));
      }
      constexpr auto error() && -> maybe<error_type> requires movable
      {
         return is_value() ? none : make_maybe(std::move(m_storage.error()));
      }

      constexpr auto
      map(const std::invocable<value_type> auto& fun) const& -> map_value_result<decltype(fun)>
      {
         if (is_value())
         {
            return make_value(std::invoke(fun, m_storage.value()));
         }
         else
         {
            return make_error(m_storage.error());
         }
      }
      constexpr auto
      map(const std::invocable<value_type> auto& fun) & -> map_value_result<decltype(fun)>
      {
         if (is_value())
         {
            return make_value(std::invoke(fun, m_storage.value()));
         }
         else
         {
            return make_error(m_storage.error());
         }
      }
      constexpr auto
      map(const std::invocable<value_type> auto& fun) const&& -> map_value_result<decltype(fun)>
      {
         if (is_value())
         {
            return make_value(std::invoke(fun, std::move(m_storage.value())));
         }
         else
         {
            return make_error(std::move(m_storage.error()));
         }
      }
      constexpr auto
      map(const std::invocable<value_type> auto& fun) && -> map_value_result<decltype(fun)>
      {
         if (is_value())
         {
            return make_value(std::invoke(fun, std::move(m_storage.value())));
         }
         else
         {
            return make_error(std::move(m_storage.error()));
         }
      }

      constexpr auto map_error(
         const std::invocable<error_type> auto& fun) const& -> map_error_result<decltype(fun)>
      {
         if (is_value())
         {
            return make_value(m_storage.value());
         }
         else
         {
            return make_error(std::invoke(fun, m_storage.error()));
         }
      }
      constexpr auto
      map_error(const std::invocable<error_type> auto& fun) & -> map_error_result<decltype(fun)>
      {
         if (is_value())
         {
            return make_value(m_storage.value());
         }
         else
         {
            return make_error(std::invoke(fun, m_storage.error()));
         }
      }
      constexpr auto map_error(
         const std::invocable<error_type> auto& fun) const&& -> map_error_result<decltype(fun)>
      {
         if (is_value())
         {
            return make_value(std::move(m_storage.value()));
         }
         else
         {
            return make_error(std::invoke(fun, std::move(m_storage.error())));
         }
      }
      constexpr auto
      map_error(const std::invocable<error_type> auto& fun) && -> map_error_result<decltype(fun)>
      {
         if (is_value())
         {
            return make_value(std::move(m_storage.value()));
         }
         else
         {
            return make_error(std::invoke(fun, std::move(m_storage.error())));
         }
      }

      template <class inner_value_ = value_type, class inner_error_ = error_type>
      constexpr auto
      join() const& -> std::common_type_t<inner_value_, inner_error_> requires copyable
      {
         return is_value() ? m_storage.value() : m_storage.error();
      }
      template <class inner_value_ = value_type, class inner_error_ = error_type>
      constexpr auto join() & -> std::common_type_t<inner_value_, inner_error_> requires copyable
      {
         return is_value() ? m_storage.value() : m_storage.error();
      }

      template <class inner_value_ = value_type, class inner_error_ = error_type>
      constexpr auto
      join() const&& -> std::common_type_t<inner_value_, inner_error_> requires movable
      {
         return is_value() ? std::move(m_storage.value()) : std::move(m_storage.error());
      }
      template <class inner_value_ = value_type, class inner_error_ = error_type>
      constexpr auto join() && -> std::common_type_t<inner_value_, inner_error_> requires movable
      {
         return is_value() ? std::move(m_storage.value()) : std::move(m_storage.error());
      }

      constexpr auto join(const std::invocable<value_type> auto& l_fun,
                          const std::invocable<error_type> auto& r_fun)
         const& -> join_result<decltype(l_fun), decltype(r_fun)>
      {
         return is_value() ? std::invoke(l_fun, m_storage.value())
                           : std::invoke(r_fun, m_storage.error());
      }
      constexpr auto join(const std::invocable<value_type> auto& l_fun,
                          const std::invocable<error_type> auto&
                             r_fun) & -> join_result<decltype(l_fun), decltype(r_fun)>
      {
         return is_value() ? std::invoke(l_fun, m_storage.value())
                           : std::invoke(r_fun, m_storage.error());
      }
      constexpr auto join(const std::invocable<value_type> auto& l_fun,
                          const std::invocable<error_type> auto& r_fun)
         const&& -> join_result<decltype(l_fun), decltype(r_fun)>
      {
         return is_value() ? std::invoke(l_fun, std::move(m_storage.value()))
                           : std::invoke(r_fun, std::move(m_storage.error()));
      }

      constexpr auto join(const std::invocable<value_type> auto& l_fun,
                          const std::invocable<error_type> auto&
                             r_fun) && -> join_result<decltype(l_fun), decltype(r_fun)>
      {
         return is_value() ? std::invoke(l_fun, std::move(m_storage.value()))
                           : std::invoke(r_fun, std::move(m_storage.error()));
      }

   private:
      storage<value_type, error_type> m_storage{};

   public:
      constexpr auto and_then(const std::invocable<value_type> auto& fun) const& -> decltype(
         detail::ensure_result_error(std::invoke(fun, m_storage.value()), m_storage.error()))
      {
         if (is_value())
         {
            return std::invoke(fun, m_storage.value());
         }
         else
         {
            return make_error(m_storage.error());
         }
      }
      constexpr auto and_then(const std::invocable<value_type> auto& fun) & -> decltype(
         detail::ensure_result_error(std::invoke(fun, m_storage.value()), m_storage.error()))
      {
         if (is_value())
         {
            return std::invoke(fun, m_storage.value());
         }
         else
         {
            return make_error(m_storage.error());
         }
      }
      constexpr auto and_then(const std::invocable<value_type> auto& fun) const&& -> decltype(
         detail::ensure_result_error(std::invoke(fun, std::move(m_storage.value())),
                                     std::move(m_storage.error())))
      {
         if (is_value())
         {
            return std::invoke(fun, std::move(m_storage.value()));
         }
         else
         {
            return make_error(std::move(m_storage.error()));
         }
      }
      constexpr auto and_then(const std::invocable<value_type> auto& fun) && -> decltype(
         detail::ensure_result_error(std::invoke(fun, std::move(m_storage.value())),
                                     std::move(m_storage.error())))
      {
         if (is_value())
         {
            return std::invoke(fun, std::move(m_storage.value()));
         }
         else
         {
            return make_error(std::move(m_storage.error()));
         }
      }

      constexpr auto or_else(const std::invocable<error_type> auto& fun) const& -> decltype(
         detail::ensure_result_value(std::invoke(fun, m_storage.error()), m_storage.value()))
      {
         if (is_value())
         {
            return make_value(m_storage.value());
         }
         else
         {
            return std::invoke(fun, m_storage.error());
         }
      }
      constexpr auto or_else(const std::invocable<error_type> auto& fun) & -> decltype(
         detail::ensure_result_value(std::invoke(fun, m_storage.error()), m_storage.value()))
      {
         if (is_value())
         {
            return make_value(m_storage.value());
         }
         else
         {
            return std::invoke(fun, m_storage.error());
         }
      }
      constexpr auto or_else(const std::invocable<error_type> auto& fun) const&& -> decltype(
         detail::ensure_result_value(std::invoke(fun, std::move(m_storage.error())),
                                     std::move(m_storage.value())))
      {
         if (is_value())
         {
            return make_value(std::move(m_storage.value()));
         }
         else
         {
            return std::invoke(fun, std::move(m_storage.error()));
         }
      }
      constexpr auto or_else(const std::invocable<error_type> auto& fun) && -> decltype(
         detail::ensure_result_value(std::invoke(fun, std::move(m_storage.error())),
                                     std::move(m_storage.value())))
      {
         if (is_value())
         {
            return make_value(std::move(m_storage.value()));
         }
         else
         {
            return std::invoke(fun, std::move(m_storage.error()));
         }
      }
   };
} // namespace monad
