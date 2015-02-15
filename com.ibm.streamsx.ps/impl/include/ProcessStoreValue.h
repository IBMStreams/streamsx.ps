/*
# Licensed Materials - Property of IBM
# Copyright IBM Corp. 2011, 2013
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
*/
#ifndef PROCESS_STORE_VALUE_H_
#define PROCESS_STORE_VALUE_H_

#include <SPL/Runtime/Type/SPLType.h>

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace store 
    {
        class ProcessStoreValue
        {
        public:
            enum Mode { DontCopyDontOwn, CopyAndOwn };
            
            ProcessStoreValue()
                : mode_(DontCopyDontOwn), value_(dummy_)
            {}

            ProcessStoreValue(ProcessStoreValue const & other)
                : mode_(DontCopyDontOwn), value_(dummy_)
            {
                this->operator=(other);
            }

            template<class T>
            ProcessStoreValue(T const & value, Mode mode) 
                : mode_(mode), value_(dummy_)
            {
                if(mode==DontCopyDontOwn)
                    value_ = SPL::ValueHandle(const_cast<T&>(value));
                else
                    value_ = SPL::ValueHandle(*(new T(value)));
            }

            ProcessStoreValue(SPL::ConstValueHandle handle) 
                : mode_(CopyAndOwn), value_(handle.newValue())
            {}

            ~ProcessStoreValue()
            {
                if(mode_==CopyAndOwn)
                    value_.deleteValue();
            }

            SPL::ValueHandle & getHandle() 
            {
                return value_;
            }

            SPL::ConstValueHandle getConstHandle() const
            {
                return SPL::ConstValueHandle(value_);
            }

            template <class T>
            void assign(T const & value) 
            {
                if(!value_.assignFrom(value)) {
                    if(CopyAndOwn) {
                        value_.deleteValue();
                        value_ = SPL::ValueHandle(*(new T(value)));
                    } else {
                        assert(!"cannot happen");
                    }
                }
            }

            void operator=(ProcessStoreValue const & other) 
            {
                if(other.mode_==CopyAndOwn) {
                    if(mode_==CopyAndOwn) {
                        SPL::ConstValueHandle oHandle(other.value_);
                        if(!value_.assignFrom(oHandle)) {
                            value_.deleteValue();
                            value_ = other.value_.newValue();  
                        }
                    } else {
                        value_ = other.value_.newValue();  
                    }
                } else {
                    if(mode_==CopyAndOwn)
                        value_.deleteValue();
                    value_ = other.value_;
                }
                mode_ = other.mode_;
            }

            bool operator==(ProcessStoreValue const & other) const 
            {
                return value_.equals(const_cast<ProcessStoreValue&>(other).getHandle());
            }

        private:
            Mode mode_;
            SPL::ValueHandle value_;
            static SPL::int32 dummy_;
        }; 

        size_t hashCode(SPL::ValueHandle & handle);
    }
} } } }

namespace std 
{ 
    namespace tr1 
    {
        template <>
        struct hash<com::ibm::streamsx::process::store::ProcessStoreValue> {  
            inline size_t operator()(com::ibm::streamsx::process::store::ProcessStoreValue const & s) const 
            {
                SPL::ValueHandle & h = const_cast<com::ibm::streamsx::process::store::ProcessStoreValue &>(s).getHandle();
                return com::ibm::streamsx::process::store::hashCode(h);
            }
        };
    }
} 


#endif /*PROCESS_STORE_VALUE_H_*/

