// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include <iterator>

#include "access.hpp"
#include "addition.hpp"
#include "json.hpp"
#include "state.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< template< typename... > class Traits >
         inline void phase2_set_keys( json::basic_value< Traits >& r, const config::key& k )
         {
            switch( r.type() ) {
               case json::type::ARRAY:
                  for( std::size_t i = 0; i < r.unsafe_get_array().size(); ++i ) {
                     phase2_set_keys( r[ i ], k + i );
                  }
                  break;
               case json::type::OBJECT:
                  for( auto& i : r.unsafe_get_object() ) {
                     phase2_set_keys( i.second, k + i.first );
                  }
                  break;
               default:
                  break;
            }
            r.set_key( k );
         }

         class phase2_impl
         {
         public:
            template< template< typename... > class Traits >
            json::basic_value< Traits > phase2( const entry& root ) const
            {
               json::basic_value< Traits > r = process_object< Traits >( root.get_object() );
               if constexpr( has_set_key< json::basic_value< Traits > >::value ) {
                  phase2_set_keys( r, config::key() );
               }
               return r;
            }

         private:
            template< template< typename... > class Traits >
            json::basic_value< Traits > process_entry( const entry& e ) const
            {
               const phase2_guard p2g( e );

               switch( e.type() ) {
                  case entry::atom:
                     return process_atom< Traits >( e.get_atom() );
                  case entry::array:
                     return process_array< Traits >( e.get_array() );
                  case entry::object:
                     return process_object< Traits >( e.get_object() );
                  case entry::nothing:
                     assert( false );
                  case entry::reference:
                     return process_reference< Traits >( e.position(), e.parent()->parent(), e.get_reference() );
               }
               assert( false );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_list( const concat& l ) const
            {
               assert( !l.entries().empty() );

               json::basic_value< Traits > r = process_entry< Traits >( l.entries().front() );

               for( auto i = std::next( l.entries().begin() ); i != l.entries().end(); ++i ) {
                  addition( r, process_entry< Traits >( *i ), i->position() );
               }
               if constexpr( has_set_position< json::basic_value< Traits > >::value ) {
                  r.set_position( l.p );
               }
               return r;
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_atom( const atom_t& v ) const
            {
               json::events::to_basic_value< Traits > consumer;
               json::events::from_value( consumer, v );
               return std::move( consumer.value );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_array( const array_t& a ) const
            {
               json::basic_value< Traits > t( json::empty_array );

               for( const auto& i : a ) {
                  if( !i.is_temporary() ) {
                     t.emplace_back( process_list< Traits >( i ) );
                  }
               }
               return t;
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_object( const object_t& o ) const
            {
               json::basic_value< Traits > t( json::empty_object );

               for( const auto& i : o ) {
                  if( !i.second.is_temporary() ) {
                     t.emplace( i.first, process_list< Traits >( i.second ) );
                  }
               }
               return t;
            }

            const concat& process_reference_impl( const position& pos, const entry& e, const reference_t& r ) const
            {
               config::key k;

               for( auto& i : r.get_array() ) {
                  if( i.is_array() ) {
                     k.emplace_back( part_from_value( pos, process_list< json::traits >( process_reference_impl( pos, e, i ) ) ) );
                  }
                  else {
                     k.emplace_back( part_from_value( pos, i ) );
                  }
               }
               return access( pos, e, k );
            }

            template< template< typename... > class Traits >
            json::basic_value< Traits > process_reference( const position& pos, const entry& e, const reference_t& r ) const
            {
               return process_list< Traits >( process_reference_impl( pos, e, r ) );
            }
         };

         template< template< typename... > class Traits >
         json::basic_value< Traits > phase2( state& st )
         {
            assert( st.astack.empty() );
            assert( st.lstack.empty() );
            assert( st.rstack.empty() );
            assert( st.ostack.size() == 1 );

            return phase2_impl().phase2< Traits >( st.root );  // TODO: Eliminate phase2_impl instance if it remains stateless.
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
