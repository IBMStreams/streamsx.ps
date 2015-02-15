/*
# Licensed Materials - Property of IBM
# Copyright IBM Corp. 2011, 2013
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
*/
#ifndef PROCESS_STORE_WRAPPERS_H_
#define PROCESS_STORE_WRAPPERS_H_

#include "ProcessStore.h"

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace store 
    {
        size_t hashCode(SPL::ValueHandle & handle);
    }
} } } }

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace store 
    {
        /// Put an item into the store
        /// @param key item's key
        /// @param value item's value
        /// @return true if there was no item with the same key in the store,
        /// false otherwise
        template<class T1, class T2>
        SPL::boolean psPut(T1 const & key, T2 const & value) 
        {            
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().put(key, value);
        }

        /// Get an item from the store
        /// @param key item's key
        /// @param value item's value
        /// @return true if there was an item with the given key and a matching
        /// type for its value, false otherwise 
        template<class T1, class T2>
        SPL::boolean psGet(T1 const & key, T2 & value) 
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().get(key, value);
        }

        /// Get the type of the value associated with the key 
        /// @param key item's key            
        /// @param type out parameter that holds the type of the value 
        /// @return true if there was an item with the given key, false otherwise
        template<class T1>
        SPL::boolean psTypeOf(T1 const & key, SPL::rstring & type)
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().type(key, type);
        }      

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
        SPL::boolean psCas(T1 const & key, T2 & value, T3 const & newValue)
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().cas(key, value, newValue);
        }

        /// Remove an item from the store
        /// @param key item's key
        /// @return true if there was an item with the given key, false otherwise 
        template<class T1>
        SPL::boolean psRemove(T1 const & key) 
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().remove(key);
        }

        /// Check if an item is in the store
        /// @param key item's key
        /// @return true if there is an item with the given key, false otherwise
        template<class T1>
        SPL::boolean psHas(T1 const & key) 
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().has(key);
        }

        /// Clear the entire store
        inline void psClear() 
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().clear();
        }

        /// Get the size of the store
        /// @return the size of the store
        inline SPL::uint64 psSize() 
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().size();
        }
     
        /// Serialize all the items in the store 
        /// @param data blob to serialize into
        inline void psSerializeAll(SPL::blob & data)
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().serializeAll(data);
        }

        /// Serialize the items in the store that have key and value types
        /// that match those of the arguments
        /// @param data blob to serialize into
        template<class T1, class T2>
        void psSerialize(SPL::blob & data, T1 const & dummyKey, T2 const & dummyValue)
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().serialize<T1, T2>(data);
        }

        /// Serialize the store
        /// @param data blob to serialize into
        /// @param types a tuple that represents the types to be serialized
        /// e.g.: tuple<tuple<int32   key, float32     value> type1,
        ///             tuple<rstring key, list<int32> value> type2>
        inline void psSerialize(SPL::blob & data, SPL::Tuple const & types) 
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().serialize(data, types);
        }

        /// Deserialize the items from the serialized store of key and value
        /// types that match those of the arguments
        /// @param data blob to serialize from
        template<class T1, class T2>
        void psDeserialize(SPL::blob const & data, T1 const & dummyKey, T2 const & dummyValue)
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().deserialize<T1, T2>(data);
        }

        /// Deserialize the store
        /// @param data blob to serialize from
        /// @param types a tuple that represents the types to be deserialized
        /// e.g.: tuple<tuple<int32   key, float32     value> type1,
        ///             tuple<rstring key, list<int32> value> type2>
        inline void psDeserialize(SPL::blob const & data, SPL::Tuple const & types)
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().deserialize(data, types);
        }
        
        /// Begin the iteration on the store. No other operations that can
        /// modify the state can be used until after a matching endIteration()
        /// call.
        /// @return the iterator
        inline SPL::uint64 psBeginIteration()
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().beginIteration();
        }
       
        /// Get the next key and value of given types in the store
        /// @param iterator the iterator
        /// @param key the key of the current item
        /// @param value the value of the current item
        /// @return true if an item was found, false otherwise.
        template<class T1, class T2>           
        bool psGetNext(SPL::uint64 iterator, T1 & key, T2 & value) 
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().getNext(iterator, key, value);
        }

        /// Get the next key and value as well as their types. Key and value
        /// are retreived using their string representations irrespective of
        /// their type. 
        /// @param iterator the iterator
        /// @param key the key of the current item
        /// @param value the value of the current item
        /// @param keyType the key type of the current item
        /// @param valueType the value type of the current item
        /// @return true if an item was found, false otherwise.
        inline bool psGetNext(SPL::uint64 iterator, SPL::rstring & key, SPL::rstring & value,
                              SPL::rstring & keyType, SPL::rstring & valueType) 
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().getNext(iterator, key, value, keyType, valueType);
        }

        /// End the iteration on the store
        /// @param iterator the iterator
        inline void psEndIteration(SPL::uint64 iterator)
        {
            return ProcessStoreFactory::getGlobalFactory().getCommonStore().endIteration(iterator);
        }

        /// Create a process store 
        /// @return store handle
        inline SPL::uint64 psCreateStore()
        {
            return ProcessStoreFactory::getGlobalFactory().createStore(true);
        }

        /// Create a process store 
        /// @param synchronized are the store operations synchronized
        /// @return store handle
        inline SPL::uint64 psCreateStore(SPL::boolean synchronized)
        {
            return ProcessStoreFactory::getGlobalFactory().createStore(synchronized);
        }
        
        /// Create a process store 
        /// @param name of the store
        /// @return store handle, or 0 if a store with the same name exists
        inline SPL::uint64 psCreateStore(SPL::rstring const & name)
        {
            return ProcessStoreFactory::getGlobalFactory().createStore(name, true);
        }

        /// Create a process store 
        /// @param name of the store
        /// @param synchronized are the store operations synchronized
        /// @return store handle, or 0 if a store with the same name exists
        inline SPL::uint64 psCreateStore(SPL::rstring const & name, SPL::boolean synchronized)
        {
            return ProcessStoreFactory::getGlobalFactory().createStore(name, synchronized);
        }

        /// Create a process store or get it if it already exists
        /// @param name of the store
        /// @return store handle
        inline SPL::uint64 psCreateOrGetStore(SPL::rstring const & name)
        {
            return ProcessStoreFactory::getGlobalFactory().createOrGetStore(name, true);
        }

        /// Create a process store or get it if it already exists
        /// @param name of the store
        /// @param synchronized are the store operations synchronized
        /// @return store handle or 0 if the store exists but has non-matching synhronization
        inline SPL::uint64 psCreateOrGetStore(SPL::rstring const & name, SPL::boolean synchronized)
        {
            return ProcessStoreFactory::getGlobalFactory().createOrGetStore(name, synchronized);
        }

        /// Get all store info
        /// @param names names of the stores (out parameter)
        /// @param handles handles of the stores (out parameter)
        inline void psGetAllStoreInfo(SPL::list<SPL::rstring> & names, SPL::list<SPL::uint64> & handles) 
        {
            return ProcessStoreFactory::getGlobalFactory().getAllStoreInfo(names, handles);
        }
        
        /// Find a process store 
        /// @param name of the store
        /// @return store handle, or 0 if a store with the given name does
        /// not exist
        inline SPL::uint64 psFindStore(SPL::rstring const & name)
        {
            return ProcessStoreFactory::getGlobalFactory().findStore(name);
        }            
        
        /// Remove a process store 
        /// @param handle store handle
        inline void psRemoveStore(SPL::uint64 store)
        {
            ProcessStoreFactory::getGlobalFactory().removeStore(store);
        }

        /// Put an item into the given store
        /// @param store store handle
        /// @param key item's key
        /// @param value item's value
        /// @return true if there was no item with the same key in the store,
        /// false otherwise
        template<class T1, class T2>
        SPL::boolean psPut(SPL::uint64 store, T1 const & key, T2 const & value) 
        {            
            return ProcessStoreFactory::getGlobalFactory().getStore(store).put(key, value);
        }

        /// Get an item from the given store
        /// @param store store handle
        /// @param key item's key
        /// @param value item's value
        /// @return true if there was an item with the given key and a matching
        /// type for its value, false otherwise 
        template<class T1, class T2>
        SPL::boolean psGet(SPL::uint64 store, T1 const & key, T2 & value) 
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).get(key, value);
        }

        /// Get the type of the value associated with the key 
        /// @param store store handle
        /// @param key item's key            
        /// @param type out parameter that holds the type of the value 
        /// @return true if there was an item with the given key, false otherwise
        template<class T1>
        SPL::boolean psTypeOf(SPL::uint64 store, T1 const & key, SPL::rstring & type)
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).type(key, type);
        }

        /// Compare and swap. If the \a key does not exist, insert it with a
        /// value that is set to \a newValue. If the \a key exists, compare
        /// the current value associated with it to \a value.  If
        /// they are the same, then replace the current value associated
        /// with the key with \a newValue and return true. If they are
        /// different, copy the current value associated with the key into
        /// \a value and return false. If the types of the current value
        /// associated with the key and that of \a value do not match, then
        /// \a value is left untouched. 
        /// @param store store handle
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
        SPL::boolean psCas(SPL::uint64 store, T1 const & key, T2 & value, T3 const & newValue)
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).cas(key, value, newValue);
        }


        /// Remove an item from the given store
        /// @param store store handle
        /// @param key item's key
        /// @return true if there was an item with the given key, false otherwise 
        template<class T1>
        SPL::boolean psRemove(SPL::uint64 store, T1 const & key) 
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).remove(key);
        }

        /// Check if an item is in the given store
        /// @param store store handle
        /// @param key item's key
        /// @return true if there is an item with the given key, false otherwise
        template<class T1>
        SPL::boolean psHas(SPL::uint64 store, T1 const & key) 
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).has(key);
        }

        /// Clear the given store 
        /// @param store store handle
        inline void psClear(SPL::uint64 store) 
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).clear();
        }

        /// Get the size of the given store
        /// @param store store handle
        /// @return the size of the store
        inline SPL::uint64 psSize(SPL::uint64 store) 
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).size();
        }

        /// Serialize all the items in the store 
        /// that match those of the arguments
        /// @param store store handle
        /// @param data blob that will contain the data
        inline void psSerializeAll(SPL::uint64 store, SPL::blob & data)
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).serializeAll(data);
        }

        /// Serialize the items in the store that have key and value types
        /// that match those of the arguments
        /// @param store store handle
        /// @param data blob that will contain the data
        /// @param dummyKey represents the key type
        /// @param dummyValue represents the value type
        template<class T1, class T2>
        void psSerialize(SPL::uint64 store, SPL::blob & data, T1 const & dummyKey, T2 const & dummyValue)
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).serialize<T1, T2>(data);
        }

        /// Serialize the store
        /// @param store store handle
        /// @param data blob to serialize into
        /// @param types a tuple that represents the types to be serialized
        /// e.g.: tuple<tuple<int32   key, float32     value> type1,
        ///             tuple<rstring key, list<int32> value> type2>
        inline void psSerialize(SPL::uint64 store, SPL::blob & data, SPL::Tuple const & types) 
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).serialize(data, types);
        }

        /// Deserialize the items from the serialized store of key and value
        /// types that match those of the arguments
        /// @param store store handle
        /// @param data blob to serialize from
        template<class T1, class T2>
        void psDeserialize(SPL::uint64 store, SPL::blob const & data, T1 const & dummyKey, T2 const & dummyValue)
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).deserialize<T1, T2>(data);
        }

        /// Deserialize the store
        /// @param store store handle
        /// @param data blob to serialize from
        /// @param types a tuple that represents the types to be deserialized
        /// e.g.: tuple<tuple<int32   key, float32     value> type1,
        ///             tuple<rstring key, list<int32> value> type2>
        inline void psDeserialize(SPL::uint64 store, SPL::blob const & data, SPL::Tuple const & types)
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).deserialize(data, types);
        }

        /// Begin the iteration on the given store. No other operations that can
        /// modify the state can be used until after a matching endIteration()
        /// call.
        /// @param store store handle
        /// @return the iterator
        inline SPL::uint64 psBeginIteration(SPL::uint64 store)
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).beginIteration();
        }
       
        /// Get the next key and value of given types in the given store
        /// @param store store handle
        /// @param iterator the iterator
        /// @param key the key of the current item
        /// @param value the value of the current item
        /// @return true if an item was found, false otherwise.
        template<class T1, class T2>           
        bool psGetNext(SPL::uint64 store, SPL::uint64 iterator, T1 & key, T2 & value) 
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).getNext(iterator, key, value);
        }

        /// Get the next key and value as well as their types in the given
        /// store. Key and value are retreived using their string
        /// representations irrespective of their type.
        /// @param iterator the iterator
        /// @param key the key of the current item
        /// @param value the value of the current item
        /// @param keyType the key type of the current item
        /// @param valueType the value type of the current item
        /// @return true if an item was found, false otherwise.
        inline bool psGetNext(SPL::uint64 store, SPL::uint64 iterator, SPL::rstring & key, SPL::rstring & value,
                              SPL::rstring & keyType, SPL::rstring & valueType) 
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).getNext(iterator, key, value, keyType, valueType);
        }

        /// End the iteration on the given store
        /// @param store store handle
        /// @param iterator the iterator
        inline void psEndIteration(SPL::uint64 store, SPL::uint64 iterator)
        {
            return ProcessStoreFactory::getGlobalFactory().getStore(store).endIteration(iterator);
        }        
    }
} } } } 

#endif /* PROCESS_STORE_WRAPPERS_H_ */
