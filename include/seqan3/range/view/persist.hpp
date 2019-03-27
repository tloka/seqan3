// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 * \brief Provides seqan3::view::persist.
 */

#pragma once

#include <range/v3/algorithm/copy.hpp>

#include <seqan3/core/metafunction/iterator.hpp>
#include <seqan3/core/metafunction/range.hpp>
#include <seqan3/core/metafunction/transformation_trait_or.hpp>
#include <seqan3/io/exception.hpp>
#include <seqan3/range/concept.hpp>
#include <seqan3/range/shortcuts.hpp>
#include <seqan3/range/container/concept.hpp>
#include <seqan3/range/view/detail.hpp>
#include <seqan3/std/concepts>
#include <seqan3/std/ranges>

namespace seqan3::detail
{

// ============================================================================
//  view_persist
// ============================================================================

/*!\brief The type returned by seqan3::view::persist.
 * \tparam urng_t The type of the underlying range, must model std::ranges::InputRange.
 * \implements std::ranges::View
 * \implements std::ranges::RandomAccessRange
 * \implements std::ranges::SizedRange
 * \ingroup view
 *
 * \details
 *
 * Note that most members of this class are generated by ranges::view_interface which is not yet documented here.
 */
template <std::ranges::InputRange urng_t>
class view_persist : public ranges::view_interface<view_persist<urng_t>>
{
private:
    //!\brief Shared storage of the underlying range.
    std::shared_ptr<urng_t> urange;

public:
    /*!\name Associated types
     * \{
     */
    //!\brief The reference_type.
    using reference         = reference_t<urng_t>;
    //!\brief The const_reference type is equal to the reference type.
    using const_reference   = reference;
    //!\brief The value_type (which equals the reference_type with any references removed).
    using value_type        = value_type_t<urng_t>;
    //!\brief If the underliying range is Sized, this resolves to range_type::size_type, otherwise void.
    using size_type         = detail::transformation_trait_or_t<seqan3::size_type<urng_t>, void>;
    //!\brief A signed integer type, usually std::ptrdiff_t.
    using difference_type   = difference_type_t<urng_t>;
    //!\brief The iterator type of this view (a random access iterator).
    using iterator          = std::ranges::iterator_t<urng_t>;
    //!\brief The const_iterator type is equal to the iterator type.
    using const_iterator    = iterator;
    //!\}

    /*!\name Constructors, destructor and assignment
     * \{
     */
    view_persist() = default;
    constexpr view_persist(view_persist const & rhs) = default;
    constexpr view_persist(view_persist && rhs) = default;
    constexpr view_persist & operator=(view_persist const & rhs) = default;
    constexpr view_persist & operator=(view_persist && rhs) = default;
    ~view_persist() = default;

    /*!\brief Construct from another range.
     * \param[in] _urange The underlying range.
     */
    view_persist(urng_t && _urange) :
        urange{new urng_t{std::move(_urange)}}
    {}
    //!\}

    /*!\name Iterators
     * \{
     */
    /*!\brief Returns an iterator to the first element of the container.
     * \returns Iterator to the first element.
     *
     * If the container is empty, the returned iterator will be equal to end().
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    const_iterator begin() const noexcept
    {
        return seqan3::begin(*urange);
    }

    //!\copydoc begin()
    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    /*!\brief Returns an iterator to the element following the last element of the range.
     * \returns Iterator or sentinel to the end.
     *
     * This element acts as a placeholder; attempting to dereference it results in undefined behaviour.
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    auto end() const noexcept
    {
        return seqan3::end(*urange);
    }

    //!\copydoc end()
    auto cend() const noexcept
    {
        return end();
    }
    //!\}

    /*!\brief Convert this view into a container implicitly.
     * \tparam container_t Type of the container to convert to; must satisfy seqan3::sequence_container_concept and the
     *                     seqan3::reference_t of both must model std::CommonReference.
     * \returns This view converted to container_t.
     */
    template <sequence_container_concept container_t>
    operator container_t() const
    //!\cond
        requires std::CommonReference<reference_t<std::remove_reference_t<container_t>>, reference>
    //!\endcond
    {
        container_t ret;
        std::ranges::copy(begin(), end(), std::back_inserter(ret));
        return ret;
    }
};

//!\brief Template argument type deduction guide that strips references.
//!\relates seqan3::detail::view_persist
template <typename urng_t>
view_persist(urng_t &&) -> view_persist<std::remove_reference_t<urng_t>>;

// ============================================================================
//  persist_fn (adaptor definition)
// ============================================================================

/*!\brief View adaptor definition for view::persist.
 */
class persist_fn : public pipable_adaptor_base<persist_fn>
{
private:
    //!\brief Type of the CRTP-base.
    using base_t = pipable_adaptor_base<persist_fn>;

public:
    //!\brief Inherit the base class's Constructors.
    using base_t::base_t;

private:
    //!\brief Befriend the base class so it can call impl().
    friend base_t;

    /*!\brief       For ranges that are viewable, delegate to std::view::all.
     * \returns     An instance of std::view::all.
     */
    template <std::ranges::ViewableRange urng_t>
    static auto impl(urng_t && urange)
    {
        return std::view::all(std::forward<urng_t>(urange));
    }

    /*!\brief       For ranges that are not views and not lvalue-references, call view_persist's constructor.
     * \returns     An instance of seqan3::detail::view_persist.
     */
    template <std::ranges::Range urng_t>
    static auto impl(urng_t && urange)
    {
        static_assert(!std::is_lvalue_reference_v<urng_t>, "BUG: lvalue-reference in persist_fn::impl().");
        return view_persist{std::move(urange)};
    }
};

} // namespace seqan3::detail

// ============================================================================
//  view::persist (adaptor instance definition)
// ============================================================================

namespace seqan3::view
{

/*!\name General purpose views
 * \{
 */

/*!\brief               A view adaptor that wraps rvalue references of non-views.
 * \tparam urng_t       The type of the range being processed. See below for requirements. [template parameter is
 *                      omitted in pipe notation]
 * \param[in] urange    The range being processed. [parameter is omitted in pipe notation]
 * \returns             A range wrapped in a view (even if it doesn't model std::ranges::ViewableRange).
 * \ingroup view
 *
 * \details
 *
 * For ranges that model std::ranges::ViewableRange, this adaptor just returns std::view::all. However this adaptor
 * can also take ranges that are not "viewable", e.g. temporaries of containers. It wraps them in a shared pointer
 * internally so all view requirements like constant copy are satisfied. However construction and copying might be
 * slightly slower, because of reference counting.
 *
 * ### View properties
 *
 * | range concepts and reference_t  | `urng_t` (underlying range type) | `rrng_t` (returned range type) |
 * |---------------------------------|:--------------------------------:|:------------------------------:|
 * | std::ranges::InputRange         | *required*                       | *preserved*                    |
 * | std::ranges::ForwardRange       |                                  | *preserved*                    |
 * | std::ranges::BidirectionalRange |                                  | *preserved*                    |
 * | std::ranges::RandomAccessRange  |                                  | *preserved*                    |
 * | std::ranges::ContiguousRange    |                                  | *preserved*                    |
 * |                                 |                                  |                                |
 * | std::ranges::ViewableRange      | ***not required***               | *guaranteed*                   |
 * | std::ranges::View               |                                  | *guaranteed*                   |
 * | std::ranges::SizedRange         |                                  | *preserved*                    |
 * | std::ranges::CommonRange        |                                  | *preserved*                    |
 * | std::ranges::OutputRange        |                                  | *preserved*                    |
 * | seqan3::const_iterable_concept  |                                  | *preserved*                    |
 * |                                 |                                  |                                |
 * | seqan3::reference_t             |                                  | seqan3::reference_t<urng_t>    |
 *
 * See the \link view view submodule documentation \endlink for detailed descriptions of the view properties.
 *
 * ### Example
 *
 * \snippet test/snippet/range/view/persist.cpp usage
 *
 * \hideinitializer
 */
inline auto constexpr persist = detail::persist_fn{};

//!\}

} // namespace seqan3::view
