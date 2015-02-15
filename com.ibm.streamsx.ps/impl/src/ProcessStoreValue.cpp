/*
# Licensed Materials - Property of IBM
# Copyright IBM Corp. 2011, 2013
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
*/
#include "ProcessStoreValue.h"

#define VALUE_HANDLE_HASH_N(type) \
{ SPL :: type & v = h; return std::tr1::hash<SPL :: type>()(v); } 

#define VALUE_HANDLE_HASH_M(type) \
{ SPL :: type & v = h; return v.hashCode(); } break; 

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace store 
    {
        SPL::int32 ProcessStoreValue::dummy_ = 0;

        size_t hashCode(SPL::ValueHandle & h)
        {
            switch(h.getMetaType()) {
            case SPL::Meta::Type::BLOB:       VALUE_HANDLE_HASH_N(blob);
            case SPL::Meta::Type::FLOAT32:    VALUE_HANDLE_HASH_N(float32); 
            case SPL::Meta::Type::FLOAT64:    VALUE_HANDLE_HASH_N(float64); 
            case SPL::Meta::Type::DECIMAL32:  VALUE_HANDLE_HASH_N(decimal32); 
            case SPL::Meta::Type::DECIMAL64:  VALUE_HANDLE_HASH_N(decimal64); 
            case SPL::Meta::Type::DECIMAL128: VALUE_HANDLE_HASH_N(decimal128); 
            case SPL::Meta::Type::COMPLEX32:  VALUE_HANDLE_HASH_N(complex32); 
            case SPL::Meta::Type::COMPLEX64:  VALUE_HANDLE_HASH_N(complex64); 
            case SPL::Meta::Type::BOOLEAN:    VALUE_HANDLE_HASH_N(boolean); 
            case SPL::Meta::Type::UINT8:      VALUE_HANDLE_HASH_N(uint8); 
            case SPL::Meta::Type::UINT16:     VALUE_HANDLE_HASH_N(uint16); 
            case SPL::Meta::Type::UINT32:     VALUE_HANDLE_HASH_N(uint32); 
            case SPL::Meta::Type::UINT64:     VALUE_HANDLE_HASH_N(uint64); 
            case SPL::Meta::Type::INT8:       VALUE_HANDLE_HASH_N(int8); 
            case SPL::Meta::Type::INT16:      VALUE_HANDLE_HASH_N(int16); 
            case SPL::Meta::Type::INT32:      VALUE_HANDLE_HASH_N(int32); 
            case SPL::Meta::Type::INT64:      VALUE_HANDLE_HASH_N(int64); 
            case SPL::Meta::Type::RSTRING:    VALUE_HANDLE_HASH_N(rstring); 
            case SPL::Meta::Type::USTRING:    VALUE_HANDLE_HASH_N(ustring); 
            case SPL::Meta::Type::TIMESTAMP:  VALUE_HANDLE_HASH_N(timestamp); 
            case SPL::Meta::Type::BSTRING:    
                { SPL::BString & v = h; return std::tr1::hash<char const *>()(v.getCString()); }
            case SPL::Meta::Type::LIST:       VALUE_HANDLE_HASH_M(List); 
            case SPL::Meta::Type::BLIST:      VALUE_HANDLE_HASH_M(BList);
            case SPL::Meta::Type::MAP:        VALUE_HANDLE_HASH_M(Map);  
            case SPL::Meta::Type::BMAP:       VALUE_HANDLE_HASH_M(BMap);  
            case SPL::Meta::Type::SET:        VALUE_HANDLE_HASH_M(Set);  
            case SPL::Meta::Type::BSET:       VALUE_HANDLE_HASH_M(BSet);  
            case SPL::Meta::Type::TUPLE:      VALUE_HANDLE_HASH_M(Tuple); 
            case SPL::Meta::Type::ENUM:       VALUE_HANDLE_HASH_M(Enum); 
            default: assert(!"cannot happen");
            }            
        }
    }
} } } }

