/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_RANGE_CONCEPTS_HPP
#define RANGES_V3_RANGE_CONCEPTS_HPP

#include <utility>
#include <type_traits>
#include <initializer_list>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/data.hpp>
#include <range/v3/size.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/range_traits.hpp>

#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
// Non-portable forward declarations of standard containers
RANGES_BEGIN_NAMESPACE_STD
    template<class Key, class Compare /*= less<Key>*/, class Alloc /*= allocator<Key>*/>
    class set;

    template<class Key, class Compare /*= less<Key>*/, class Alloc /*= allocator<Key>*/>
    class multiset;

    template<class Key, class Hash /*= hash<Key>*/, class Pred /*= equal_to<Key>*/, class Alloc /*= allocator<Key>*/>
    class unordered_set;

    template<class Key, class Hash /*= hash<Key>*/, class Pred /*= equal_to<Key>*/, class Alloc /*= allocator<Key>*/>
    class unordered_multiset;
RANGES_END_NAMESPACE_STD
#else
#include <set>
#include <unordered_set>
#endif

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            struct view_predicate_;
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{

        // Specialize this if the default is wrong.
        template<typename T>
        struct enable_view
        {};

        ///
        /// Range concepts below
        ///
        CONCEPT_def
        (
            template(typename T)
            concept Range,
                //Sentinel<sentinel_t<T>, iterator_t<T>>
                requires (T& t)
                (
                    ranges::end(t)
                )
        );

        CONCEPT_def
        (
            template(typename T, typename V)
            concept OutputRange,
                Range<T> && OutputIterator<iterator_t<T>, V>
        );

        CONCEPT_def
        (
            template(typename T)
            concept InputRange,
                Range<T> && InputIterator<iterator_t<T>>
        );

        CONCEPT_def
        (
            template(typename T)
            concept ForwardRange,
                InputRange<T> && ForwardIterator<iterator_t<T>>
        );

        CONCEPT_def
        (
            template(typename T)
            concept BidirectionalRange,
                ForwardRange<T> && BidirectionalIterator<iterator_t<T>>
        );

        CONCEPT_def
        (
            template(typename T)
            concept RandomAccessRange,
                BidirectionalRange<T> && RandomAccessIterator<iterator_t<T>>
        );

        /// \cond
        namespace detail
        {
            template<typename Rng>
            using data_reference_t = decltype(*data(std::declval<Rng&>()));

            template<typename Rng>
            using element_t = meta::_t<std::remove_reference<data_reference_t<Rng>>>;
        }
        /// \endcond

        CONCEPT_def
        (
            template(typename T)
            concept ContiguousRange,
                RandomAccessRange<T> &&
                Same<range_value_type_t<T>, meta::_t<std::remove_cv<detail::element_t<T>>>> &&
                Same<detail::data_reference_t<T>, range_reference_t<T>>
        );

        CONCEPT_def
        (
            template(typename T)
            concept BoundedRange,
                Range<T> &&
                Same<iterator_t<T>, sentinel_t<T>>
        );

        CONCEPT_def
        (
            template(typename T)
            concept SizedRange,
                requires (T &t)
                (
                    size(t),
                    concepts::requires_<Integral<decltype(size(t))>>
                ) &&
                Range<T> && !disable_sized_range<uncvref_t<T>>::value
        );

        ///
        /// View concepts below
        ///

        CONCEPT_def
        (
            template(typename T)
            concept View,
                Range<T> && Movable<T> && DefaultConstructible<T> &&
                detail::view_predicate_<T>::value
        );

        CONCEPT_def
        (
            template(typename T, typename V)
            concept OutputView,
                View<T> && OutputRange<T, V>
        );

        CONCEPT_def
        (
            template(typename T)
            concept InputView,
                View<T> && InputRange<T>
        );

        CONCEPT_def
        (
            template(typename T)
            concept ForwardView,
                View<T> && ForwardRange<T>
        );

        CONCEPT_def
        (
            template(typename T)
            concept BidirectionalView,
                View<T> && BidirectionalRange<T>
        );

        CONCEPT_def
        (
            template(typename T)
            concept RandomAccessView,
                View<T> && RandomAccessRange<T>
        );

        CONCEPT_def
        (
            template(typename T)
            concept ContiguousView,
                RandomAccessView<T> && ContiguousRange<T>
        );

        // Additional concepts for checking additional orthogonal properties
        CONCEPT_def
        (
            template(typename T)
            concept BoundedView,
                View<T> && BoundedRange<T>
        );

        CONCEPT_def
        (
            template(typename T)
            concept SizedView,
                View<T> && SizedRange<T>
        );

        ////////////////////////////////////////////////////////////////////////////////////////////
        // range_tag
        using range_tag = ::concepts::tag<RangeConcept>;
        using input_range_tag = ::concepts::tag<InputRangeConcept, range_tag>;
        using forward_range_tag = ::concepts::tag<ForwardRangeConcept, input_range_tag>;
        using bidirectional_range_tag = ::concepts::tag<BidirectionalRangeConcept, forward_range_tag>;
        using random_access_range_tag = ::concepts::tag<RandomAccessRangeConcept, bidirectional_range_tag>;
        using contiguous_range_tag = ::concepts::tag<ContiguousRangeConcept, random_access_range_tag>;

        template<typename T>
        using range_tag_of =
            ::concepts::tag_of<
                meta::list<
                    ContiguousRangeConcept,
                    RandomAccessRangeConcept,
                    BidirectionalRangeConcept,
                    ForwardRangeConcept,
                    InputRangeConcept,
                    RangeConcept>,
                T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // bounded_range_tag_of
        using bounded_range_tag = ::concepts::tag<BoundedRangeConcept, range_tag>;

        template<typename T>
        using bounded_range_tag_of =
            ::concepts::tag_of<
                meta::list<
                    BoundedRangeConcept,
                    RangeConcept>,
                T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // sized_range_concept
        using sized_range_tag = ::concepts::tag<SizedRangeConcept, range_tag>;

        template<typename T>
        using sized_range_tag_of =
            ::concepts::tag_of<
                meta::list<
                    SizedRangeConcept,
                    RangeConcept>,
                T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // bounded_view_tag_of
        using view_tag = ::concepts::tag<ViewConcept, range_tag>;
        using bounded_view_tag = ::concepts::tag<BoundedViewConcept, view_tag>;

        template<typename T>
        using bounded_view_tag_of =
            ::concepts::tag_of<
                meta::list<
                    BoundedViewConcept,
                    ViewConcept,
                    RangeConcept>,
                T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // sized_view_tag_of
        using sized_view_tag = ::concepts::tag<SizedViewConcept, view_tag>;

        template<typename T>
        using sized_view_tag_of =
            ::concepts::tag_of<
                meta::list<
                    SizedViewConcept,
                    ViewConcept,
                    RangeConcept>,
                T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // view_concept
        template<typename T>
        using view_tag_of =
            ::concepts::tag_of<
                meta::list<
                    ViewConcept,
                    RangeConcept>,
                T>;

        /// @}

        /// \cond
        namespace detail
        {
            template<typename T>
            std::is_same<reference_t<iterator_t<T>>, reference_t<iterator_t<T const>>>
            view_like_(int);

            template<typename T>
            std::true_type
            view_like_(long);

            template<typename T>
            using view_like = decltype(detail::view_like_<T>(42));

            // Something is a view if it's a Range and either:
            //  - It doesn't look like a container, or
            //  - It's derived from view_base
            template<typename T>
            struct view_predicate_
              : meta::_t<meta::if_<
                    meta::is_trait<enable_view<T>>,
                    enable_view<T>,
                    meta::bool_<view_like<T>() || (bool) DerivedFrom<T, view_base>>>>
            {};

            template<typename T>
            struct view_predicate_<std::initializer_list<T>>
              : std::false_type
            {};

            template<class Key, class Compare, class Alloc>
            struct view_predicate_<std::set<Key, Compare, Alloc>>
              : std::false_type
            {};

            template<class Key, class Compare, class Alloc>
            struct view_predicate_<std::multiset<Key, Compare, Alloc>>
              : std::false_type
            {};

            template<class Key, class Hash, class Pred, class Alloc>
            struct view_predicate_<std::unordered_set<Key, Hash, Pred, Alloc>>
              : std::false_type
            {};

            template<class Key, class Hash, class Pred, class Alloc>
            struct view_predicate_<std::unordered_multiset<Key, Hash, Pred, Alloc>>
              : std::false_type
            {};

            template<typename T>
            struct is_view_
              : meta::bool_<(bool)View<T>>
            {};

            template<typename T>
            struct is_range_
              : meta::bool_<(bool)Range<T>>
            {};
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{

        template<typename T>
        using is_view
            RANGES_DEPRECATED("If you need to override the logic of the View concept, please use ranges::enable_view."
                              "Otherwise, please use the View concept directly.") =
                detail::is_view_<T>;

        /// @}
    }
}

#endif
