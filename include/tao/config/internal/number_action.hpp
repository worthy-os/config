// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_NUMBER_ACTION_HPP
#define TAO_CONFIG_INTERNAL_NUMBER_ACTION_HPP

#include "json.hpp"
#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct number_action
            : public pegtl::nothing< Rule >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::hexnum< false > >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::hexnum< false > >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::hexnum< true > >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::hexnum< true > >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::zero< false > >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::zero< false > >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::zero< true > >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::zero< true > >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::nan >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::nan >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::infinity< false > >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::infinity< false > >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::infinity< true > >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::infinity< true > >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::esign >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::esign >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::idigits >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::idigits >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::fdigits >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::fdigits >
         {
         };

         template<>
         struct number_action< json::jaxn::internal::rules::edigits >
            : public json::jaxn::internal::action< json::jaxn::internal::rules::edigits >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
