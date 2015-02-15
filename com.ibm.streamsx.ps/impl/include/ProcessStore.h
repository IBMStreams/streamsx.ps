/*
# Licensed Materials - Property of IBM
# Copyright IBM Corp. 2011, 2013
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
*/
#ifndef PROCESS_STORE_H_
#define PROCESS_STORE_H_

#include "ProcessStoreValue.h"
#include "RWLock.h"

#include <SPL/Runtime/Type/Meta/BaseType.h>

#include <tr1/memory>

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace store 
    {
        class KeyValueType;
        class ProcessStore;

        class ProcessStoreEntry
        {
        public:
            ProcessStoreEntry(SPL::boolean synchronized, SPL::rstring const & name);
            ~ProcessStoreEntry();
            SPL::rstring const & getName() const 
            {
                return name_;
            }
            SPL::rstring & getName() 
            {
                return name_;
            }
            ProcessStore const & getStore() const 
            {
                return *store_;
            }
            ProcessStore & getStore() 
            {
                return *store_;
            }
        private:
            std::auto_ptr<ProcessStore> store_;
            SPL::rstring name_;
        };
        
        class ProcessStore
        {
        private:
            mutable lock::RWLock rwlock_;
            typedef std::tr1::unordered_map<ProcessStoreValue, ProcessStoreValue> ProcessStoreMap;
            ProcessStoreMap map_;            
            bool synchronized_;
            static __thread bool iterationInProgress_;

        public:            
            /// Constructor
            /// @param synchronized turn on synchronization if true, off
            /// otherwise (by default all operations are
            /// synchronized via read/write locks)
            ProcessStore(SPL::boolean synchronized);

            /// Is this store synchronized
            bool isSynchronized() const
            {
                return synchronized_;
            }

            /// Put an item into the store
            /// @param key item's key
            /// @param value item's value
            /// @return true if there was no item with the same key in the store,
            /// false otherwise
            template<class T1, class T2>
            SPL::boolean put(T1 const & key, T2 const & value);

            /// Get an item from the store
            /// @param key item's key
            /// @param value item's value
            /// @return true if there was an item with the given key and a matching
            /// type for its value, false otherwise 
            template<class T1, class T2>
            SPL::boolean get(T1 const & key, T2 & value) const;

            /// Get the type of the value associated with the key 
            /// @param key item's key            
            /// @param type out parameter that holds the type of the value 
            /// @return true if there was an item with the given key, false otherwise
            template<class T1>
            SPL::boolean type(T1 const & key, SPL::rstring & type) const;

            /// Compare and swap. If the \a key does not exist, insert it with a
            /// value that is set to \a newValue. If the \a key exists, compare
            /// the current value associated with it to \a value.  If
            /// they are the same, then replace the current value associated
            /// with the key with \a newValue and return true. If they are
            /// different, copy the current value associated with the key into
            /// \a value and return false. If the types of the current value
            /// associated with the key and that of \a value do not match, then
            /// \a value is left untouched. 
            /// @param key item's key
            /// @param value inout parameter; in: the value to compare the
            /// current value with; out: the current value associated with the
            /// key after the cas operation, or untouched (if the current value
            /// is of a different type than \a value)
            /// @param newValue the value to replace the current value
            /// associated with the key
            /// @return true if the current value associated with the key was
            /// same as \a value or did the key did not exist (in both cases the
            /// current value is set to \a newValue); false if the current value
            /// associated with the key was not the same as \a value (in which
            /// case the \a value is replaced with the current value or left
            /// untouched if the types do not match).
            template<class T1, class T2, class T3>
            SPL::boolean cas(T1 const & key, T2 & value, T3 const & newValue);
            
            /// Remove an item from the store
            /// @param key item's key
            /// @return true if there was an item with the given key, false otherwise 
            template<class T1>
            SPL::boolean remove(T1 const & key);
            
            /// Check if an item is in the store
            /// @param key item's key
            /// @return true if there is an item with the given key, false otherwise
            template<class T1>
            SPL::boolean has(T1 const & key) const; 
            
            /// Clear the entire store
            void clear();

            /// Get the size of the store
            /// @return the size of the store
            SPL::uint64 size() const;

            /// Serialize all the items in the store 
            /// that match those of the arguments
            /// @param data blob to serialize into
            void serializeAll(SPL::blob & data) const;
                       
            /// Serialize the items in the store that have key and value types
            /// that match those of the arguments
            /// @param data blob to serialize into
            template<class T1, class T2>
            void serialize(SPL::blob & data) const;

            /// Deserialize the items from the serialized store of key and value
            /// types that match those of the arguments
            /// @param data blob to serialize from
            template<class T1, class T2>
            void deserialize(SPL::blob const & data);       

            /// Serialize the store
            /// @param data blob to serialize into
            /// @param types a tuple that represents the types to be serialized
            /// e.g.: tuple<tuple<int32   key, float32     value> type1,
            ///             tuple<rstring key, list<int32> value> type2>
            void serialize(SPL::blob & data, SPL::Tuple const & types) const;

            /// Deserialize the store
            /// @param data blob to serialize from
            /// @param types a tuple that represents the types to be deserialized
            /// e.g.: tuple<tuple<int32   key, float32     value> type1,
            ///             tuple<rstring key, list<int32> value> type2>
            void deserialize(SPL::blob const & data, SPL::Tuple const & types);       

            /// Begin the iteration. No other operations that can modify
            /// the state can be used until after a matching endIteration()
            /// call. 
            /// @return the iterator
            SPL::uint64 beginIteration() const;
            
            /// Get the next key and value of given types. 
            /// @param iterator the iterator
            /// @param key the key of the current item
            /// @param value the value of the current item
            /// @return true if an item was found, false otherwise.
            template<class T1, class T2>           
            bool getNext(SPL::uint64 iterator, T1 & key, T2 & value) const;

            /// Get the next key and value as well as their types. Key and value
            /// are retreived using their string representations irrespective of
            /// their type. 
            /// @param iterator the iterator
            /// @param key the key of the current item
            /// @param value the value of the current item
            /// @param keyType the key type of the current item
            /// @param valueType the value type of the current item
            /// @return true if an item was found, false otherwise.
            bool getNext(SPL::uint64 iterator, SPL::rstring & key, SPL::rstring & value,
                         SPL::rstring & keyType, SPL::rstring & valueType) const;
            
            /// End the iteration
            /// @param iterator the iterator
            void endIteration(SPL::uint64 iterator) const;
        private:
            void serialize(SPL::blob & data, std::tr1::unordered_set<KeyValueType> const & keyValueTypes) const;
            void collectKeyValueTypes(std::tr1::unordered_set<KeyValueType> & keyValueTypes) const;     
            void checkIterationInProgress(char const * operation) const;
        };

        class KeyValueType
        {
        private:
            SPL::rstring keyType_;
            SPL::rstring valueType_;            
        public:
            KeyValueType() {}
            KeyValueType(SPL::rstring const & keyType,
                         SPL::rstring const & valueType)
                : keyType_(keyType), valueType_(valueType) {}
            SPL::rstring const & getKeyType() const
            {
                return keyType_;
            }
            SPL::rstring & getKeyType() 
            {
                return keyType_;
            }
            SPL::rstring const & getValueType() const
            {
                    return valueType_;
            }
            SPL::rstring & getValueType() 
            {
                return valueType_;
            }
            bool operator==(KeyValueType const & o) const
            {
                return o.keyType_ == keyType_ &&
                    o.valueType_ == valueType_;
            }
        }; 

        class ProcessStoreFactory 
        {
        private:
            ProcessStore commonStore_; // common store for the whole fctory
            lock::RWLock factoryLock_; // lock for the store map and store counter
            SPL::uint64 storeCounter_;
            typedef std::tr1::unordered_map<SPL::rstring, 
                std::tr1::shared_ptr<ProcessStoreEntry> > ProcessStoreFactoryMap;
            ProcessStoreFactoryMap storeMap_;        
        public:
            /// Constructor
            ///
            ProcessStoreFactory();

            /// Create a process store 
            /// @param synchronized are the store operations synchronized
            /// @return store handle
            SPL::uint64 createStore(SPL::boolean synchronized);
            
            /// Create a process store 
            /// @param synchronized are the store operations synchronized
            /// @param name of the store
            /// @return store handle, or 0 if a store with the same name exists
            SPL::uint64 createStore(SPL::rstring const & name, SPL::boolean synchronized);

            /// Create a process store or get it if it already exists
            /// @param synchronized are the store operations synchronized
            /// @param name of the store
            /// @return store handle or 0 if the store exists but has non-matching synhronization
            SPL::uint64 createOrGetStore(SPL::rstring const & name, SPL::boolean synchronized);

            /// Get all store info
            /// @param names names of the stores
            /// @param handles handles of the stores
            void getAllStoreInfo(SPL::list<SPL::rstring> & names, SPL::list<SPL::uint64> & handles); 

            /// Find a process store 
            /// @param name name of the store
            /// @return store handle, or 0 if a store with the given name does
            /// not exist
            SPL::uint64 findStore(SPL::rstring const & name);

            /// Get a process store 
            /// @param handle store handle
            /// @return process store 
            ProcessStore & getStore(SPL::uint64 handle);
     
            /// Remove a process store 
            /// @param handle store handle
            void removeStore(SPL::uint64 handle);
                           
            /// Get the common store
            /// @return the common store
            ProcessStore & getCommonStore();

            /// Get the global store factory
            /// @return the global state factory
            static ProcessStoreFactory & getGlobalFactory();
        private:
            SPL::rstring getStoreName(SPL::rstring const & name);
        };
    }
} } } }

namespace std 
{ 
    namespace tr1 
    {
        template <>
        struct hash<com::ibm::streamsx::process::store::KeyValueType> {  
            inline size_t operator()(com::ibm::streamsx::process::store::KeyValueType const & s) const 
            {
                SPL::rstring const & keyType = s.getKeyType();
                SPL::rstring const & valueType = s.getValueType();
                std::tr1::hash<SPL::rstring> hasher;
                size_t res = 17;
                res = 37 * res + hasher(keyType);
                res = 37 * res + hasher(valueType);
                return res;
            }
        };
    }
}        

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace store 
    {
        template<class T1, class T2>
        SPL::boolean ProcessStore::put(T1 const & key, T2 const & value) 
        {            
            checkIterationInProgress("put");
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::WRITE);
            ProcessStoreValue skey(key, ProcessStoreValue::DontCopyDontOwn);
            ProcessStoreMap::iterator it = map_.find(skey);
            if(it==map_.end()) {
                ProcessStoreValue skeyNew(key, ProcessStoreValue::CopyAndOwn);
                ProcessStoreValue svalue(value, ProcessStoreValue::CopyAndOwn);
                map_.insert(std::make_pair(skeyNew, svalue));
                return true;
            } else {
                ProcessStoreValue & svalue = it->second;
                svalue.assign(value);
                return false;
            }            
        }

        template<class T1, class T2>
        SPL::boolean ProcessStore::get(T1 const & key, T2 & value) const
        {
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::READ);
            ProcessStoreValue skey(key, ProcessStoreValue::DontCopyDontOwn);
            ProcessStoreMap::const_iterator it = map_.find(skey);
            if(it==map_.end()) 
                return false;
            ProcessStoreValue const & svalue = it->second;
            SPL::ValueHandle valueHandle(value);
            return valueHandle.assignFrom(svalue.getConstHandle());
        }

        template<class T1>
        SPL::boolean ProcessStore::type(T1 const & key, SPL::rstring & type) const
        {
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::READ);
            ProcessStoreValue skey(key, ProcessStoreValue::DontCopyDontOwn);
            ProcessStoreMap::const_iterator it = map_.find(skey);
            if(it==map_.end()) 
                return false;
             ProcessStoreValue const & svalue = it->second;
             SPL::ConstValueHandle vhandle = svalue.getConstHandle();
             SPL::Meta::BaseType const & mtype = SPL::Meta::BaseType::makeType(vhandle);
             type = mtype.getName();
             return true;
        }

        template<class T1, class T2, class T3>
        SPL::boolean ProcessStore::cas(T1 const & key, T2 & value, T3 const & newValue)
        {
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::WRITE);
            ProcessStoreValue skey(key, ProcessStoreValue::DontCopyDontOwn);
            ProcessStoreMap::iterator it = map_.find(skey);
            if(it==map_.end()) {
                ProcessStoreValue skeyNew(key, ProcessStoreValue::CopyAndOwn);
                ProcessStoreValue svalue(newValue, ProcessStoreValue::CopyAndOwn);
                map_.insert(std::make_pair(skeyNew, svalue));
                return true;
            } else {
                ProcessStoreValue & currentValue = it->second;
                SPL::ValueHandle currentValueHandle = currentValue.getHandle();
                SPL::ValueHandle valueHandle(value);
                if(currentValueHandle.equals(valueHandle)) {
                    currentValue.assign(newValue);
                    return true;
                } else {
                    valueHandle.assignFrom(currentValueHandle);
                    return false;
                }                 
            }
        }

        template<class T1>
        SPL::boolean ProcessStore::remove(T1 const & key) 
        {
            checkIterationInProgress("remove");
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::WRITE);
            ProcessStoreValue skey(key, ProcessStoreValue::DontCopyDontOwn);
            ProcessStoreMap::iterator it = map_.find(skey);
            if(it==map_.end()) 
                return false;
            map_.erase(it);
            return true;
        }
            
        template<class T1>
        SPL::boolean ProcessStore::has(T1 const & key) const
        {
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::READ);
            ProcessStoreValue skey(key, ProcessStoreValue::DontCopyDontOwn);
            ProcessStoreMap::const_iterator it = map_.find(skey);
            return(it!=map_.end());
        }

        template<class T1, class T2>
        void ProcessStore::serialize(SPL::blob & data) const
        {
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::READ);
            std::tr1::unordered_set<KeyValueType> keyValueTypes;
            KeyValueType keyValueType(typeid(T1).name(), typeid(T2).name());
            keyValueTypes.insert(keyValueType);
            serialize(data, keyValueTypes);
        }
        
        template<class T1, class T2>
        void ProcessStore::deserialize(SPL::blob const & data)
        {                
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::WRITE);
            map_.clear();
            T1 key;
            T2 value;
            uint64_t size;
            unsigned char const * rdata = data.getData(size);
            SPL::NativeByteBuffer nbf(const_cast<unsigned char*>(rdata), size);           

            SPL::rstring keyType;
            SPL::rstring valueType;
            uint64_t endCursor;
            while(nbf.getNRemainingBytes()>0) {
                nbf >> keyType;
                nbf >> valueType;
                nbf >> endCursor;
                endCursor += nbf.getOCursor();
                if (keyType!=typeid(T1).name() || valueType!=typeid(T2).name()) {
                    nbf.setOCursor(endCursor);
                    continue;
                }
                while (nbf.getOCursor()<endCursor) {
                    nbf >> key;
                    nbf >> value;
                    ProcessStoreValue skey(key, ProcessStoreValue::CopyAndOwn);
                    ProcessStoreValue svalue(value, ProcessStoreValue::CopyAndOwn);
                    map_.insert(std::make_pair(skey, svalue));
                }           
                break;
            }
        }

        template<class T1, class T2>           
        bool ProcessStore::getNext(SPL::uint64 iterator, T1 & key, T2 & value) const
        {
            ProcessStoreMap::const_iterator & it = 
                * reinterpret_cast<ProcessStoreMap::const_iterator *>(iterator);
            SPL::ValueHandle keyHandle(key);
            SPL::ValueHandle valueHandle(value);
            while(true) {
                if(it==map_.end()) 
                    return false; 
                ProcessStoreValue const & skey = it->first;
                ProcessStoreValue const & svalue = it->second;
                it++;
                if(keyHandle.assignFrom(skey.getConstHandle()) &&
                   valueHandle.assignFrom(svalue.getConstHandle()))
                    return true;                    
            }
        }   
    }
} } } }
#endif /* PROCESS_STORE_H_ */
