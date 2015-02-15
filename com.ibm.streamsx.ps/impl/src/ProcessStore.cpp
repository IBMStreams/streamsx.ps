/*
# Licensed Materials - Property of IBM
# Copyright IBM Corp. 2011, 2013
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
*/
#include "ProcessStore.h"

#include <vector>

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace store 
    {
        using namespace process::lock;

        __thread bool ProcessStore::iterationInProgress_ = false; 
        
        ProcessStoreEntry::ProcessStoreEntry(SPL::boolean synchronized, SPL::rstring const & name)
            : store_(new ProcessStore(synchronized)), name_(name) {}

        ProcessStoreEntry::~ProcessStoreEntry() {}

        ProcessStoreFactory::ProcessStoreFactory() 
            : commonStore_(true), storeCounter_(0) {}

        SPL::uint64 ProcessStoreFactory::createStore(SPL::boolean synchronized)
        {
            return createStore("", synchronized);
        }
        
        SPL::rstring ProcessStoreFactory::getStoreName(SPL::rstring const & name)
        {
            if (name.empty()) {
                std::ostringstream ostr;
                ostr << "unnamed_" << storeCounter_++;
                return ostr.str();
            } else {
                return "named_" + name;
            }            
        }

        SPL::uint64 ProcessStoreFactory::createStore(SPL::rstring const & name, SPL::boolean synchronized)
        {
            AutoLock al(factoryLock_, RWLock::WRITE);
            SPL::rstring modName = getStoreName(name);
            if(storeMap_.count(modName)!=0)
                return 0;
            std::tr1::shared_ptr<ProcessStoreEntry> entryPtr(new ProcessStoreEntry(synchronized, modName));
            storeMap_.insert(std::make_pair(modName, entryPtr));
            return reinterpret_cast<SPL::uint64>(entryPtr.get());
        }
        
        SPL::uint64 ProcessStoreFactory::createOrGetStore(SPL::rstring const & name, SPL::boolean synchronized)
        {
            AutoLock al(factoryLock_, RWLock::WRITE);
            SPL::rstring modName = getStoreName(name);
            ProcessStoreFactoryMap::iterator it = storeMap_.find(modName);
            if(it==storeMap_.end()) {
                std::tr1::shared_ptr<ProcessStoreEntry> entryPtr(new ProcessStoreEntry(synchronized, modName));
                storeMap_.insert(std::make_pair(modName, entryPtr));
                return reinterpret_cast<SPL::uint64>(entryPtr.get());
            } else {
                std::tr1::shared_ptr<ProcessStoreEntry> entryPtr = it->second;
                if(entryPtr->getStore().isSynchronized()!=synchronized)
                    return 0;
                return reinterpret_cast<SPL::uint64>(entryPtr.get());                
            }
        }

        void ProcessStoreFactory::getAllStoreInfo(SPL::list<SPL::rstring> & names, SPL::list<SPL::uint64> & handles)
        {
            names.clear();
            handles.clear();
            for(ProcessStoreFactoryMap::iterator it = storeMap_.begin(); it != storeMap_.end(); ++it) {
                std::tr1::shared_ptr<ProcessStoreEntry> entryPtr = it->second;
                SPL::uint64 handle = reinterpret_cast<SPL::uint64>(entryPtr.get());
                std::string name = entryPtr->getName();
                name = name.substr(name.find("_")+1);
                names.push_back(name);
                handles.push_back(handle);
            }
        }

        SPL::uint64 ProcessStoreFactory::findStore(SPL::rstring const & name)
        {
            AutoLock al(factoryLock_, RWLock::READ);
            SPL::rstring modName = "named_" + name;
            ProcessStoreFactoryMap::iterator it = storeMap_.find(modName);
            if(it==storeMap_.end())
                return 0;                
            std::tr1::shared_ptr<ProcessStoreEntry> entryPtr = it->second;
            return reinterpret_cast<SPL::uint64>(entryPtr.get());
        }            

        ProcessStore & ProcessStoreFactory::getStore(SPL::uint64 handle)
        {
            AutoLock al(factoryLock_, RWLock::READ);
            ProcessStoreEntry * entry = reinterpret_cast<ProcessStoreEntry *>(handle);
            return entry->getStore();
        } 
     
        void ProcessStoreFactory::removeStore(SPL::uint64 handle)
        {
            AutoLock al(factoryLock_, RWLock::WRITE);
            ProcessStoreEntry * entry = reinterpret_cast<ProcessStoreEntry *>(handle);
            SPL::rstring key = entry->getName();
            storeMap_.erase(key);
        }                   

        ProcessStore & ProcessStoreFactory::getCommonStore()
        {
            return commonStore_;
        }

        ProcessStoreFactory & ProcessStoreFactory::getGlobalFactory()
        {
            static ProcessStoreFactory factory;
            return factory;
        }       

        ProcessStore::ProcessStore(SPL::boolean synchronized)
            : synchronized_(synchronized) {}
        
        void ProcessStore::clear() 
        {
            checkIterationInProgress("clear");
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::WRITE);
            map_.clear();
        }
        
        SPL::uint64 ProcessStore::size() const
        {
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::READ);
            return map_.size();
        }

        SPL::uint64 ProcessStore::beginIteration() const
        {
            checkIterationInProgress("beginIteration");
            if(synchronized_)
                rwlock_.rlock();
            iterationInProgress_ = true;
            ProcessStoreMap::const_iterator * it =
                new ProcessStoreMap::const_iterator(map_.begin());
            return reinterpret_cast<SPL::uint64>(it);
        }        

        bool ProcessStore::getNext(SPL::uint64 iterator, SPL::rstring & key, SPL::rstring & value,
                                   SPL::rstring & keyType, SPL::rstring & valueType) const
        {
            ProcessStoreMap::const_iterator & it = 
                * reinterpret_cast<ProcessStoreMap::const_iterator *>(iterator);
            SPL::ValueHandle keyHandle(key);
            SPL::ValueHandle valueHandle(value);
            if(it==map_.end()) 
                return false; 
            ProcessStoreValue const & skey = it->first;
            ProcessStoreValue const & svalue = it->second;
            it++;
            SPL::ConstValueHandle khandle = skey.getConstHandle();
            SPL::ConstValueHandle vhandle = svalue.getConstHandle();
            key = khandle.toString();
            value = vhandle.toString();
            SPL::Meta::BaseType const & keytp = SPL::Meta::BaseType::makeType(khandle);
            SPL::Meta::BaseType const & valuetp = SPL::Meta::BaseType::makeType(vhandle);
            keyType = keytp.getName();
            valueType = valuetp.getName();
            return true;
        }
        
        void ProcessStore::endIteration(SPL::uint64 iterator) const
        {
            ProcessStoreMap::const_iterator * it = 
                reinterpret_cast<ProcessStoreMap::const_iterator *>(iterator);
            delete it;
            iterationInProgress_ = false;
            if(synchronized_)
                rwlock_.unlock();
        }

        void ProcessStore::checkIterationInProgress(char const * operation) const
        {
            if(iterationInProgress_)
                throw SPL::SPLRuntimeException("", "Operation '"+std::string(operation)+"' cannot be performed while an iteration is in progress.");
        }        

        class KeyValuePair
        {
        private:
            ProcessStoreValue const * key_;
            ProcessStoreValue const * value_;   
        public:
            KeyValuePair(ProcessStoreValue const * key,
                         ProcessStoreValue const * value)
                : key_(key), value_(value) {}
            ProcessStoreValue const * getKey() const
            {
                return key_;
            }
            ProcessStoreValue const * getValue() const
            {
                return value_;
            }
        };

        class TypedStoreView 
        {
        private:
            KeyValueType type_;
            std::vector<KeyValuePair> data_;
        public:
            TypedStoreView(KeyValueType type)
                : type_(type) 
            {} 
            KeyValueType & getType() 
            {
                return type_;
            }
            const KeyValueType & getType() const
            {
                return type_;
            }
            std::vector<KeyValuePair> & getData() 
            {
                return data_;
            }
            std::vector<KeyValuePair> const & getData() const
            {
                return data_;
            }
        };

        static void collectKeyValueTypes(SPL::Tuple const & types, 
            std::tr1::unordered_set<KeyValueType> & keyValueTypes)
        {
            for(SPL::ConstTupleIterator it=types.getBeginIterator(); it!=types.getEndIterator(); ++it)
            {
                SPL::ConstTupleAttribute attribute = *it;
                SPL::ConstValueHandle handle = attribute.getValue();
                if(handle.getMetaType()!=SPL::Meta::Type::TUPLE)
                    continue;
                SPL::Tuple const & kv = handle;
                if(kv.getNumberOfAttributes()!=2)
                    continue;
                SPL::ConstValueHandle keyHandle = kv.getAttributeValue(0);
                SPL::ConstValueHandle valueHandle = kv.getAttributeValue(1);
                KeyValueType keyValueType(keyHandle.getTypeId().name(),
                                          valueHandle.getTypeId().name());
                keyValueTypes.insert(keyValueType);
            }                
        }

        void ProcessStore::collectKeyValueTypes(std::tr1::unordered_set<KeyValueType> & keyValueTypes) const
        {
            for (ProcessStoreMap::const_iterator it = map_.begin(); it != map_.end(); ++it) {
                ProcessStoreValue const & skey = it->first;
                ProcessStoreValue const & svalue = it->second;
                KeyValueType keyValueType(skey.getConstHandle().getTypeId().name(),
                                          svalue.getConstHandle().getTypeId().name());
                keyValueTypes.insert(keyValueType);
            }
        }

        void ProcessStore::serializeAll(SPL::blob & data) const
        {
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::READ);
            std::tr1::unordered_set<KeyValueType> keyValueTypes;
            collectKeyValueTypes(keyValueTypes);
            serialize(data, keyValueTypes);
        }

        void ProcessStore::serialize(SPL::blob & data, SPL::Tuple const & types) const
        {
            std::tr1::unordered_set<KeyValueType> keyValueTypes;
            store::collectKeyValueTypes(types, keyValueTypes);
            serialize(data, keyValueTypes);
        }

        void ProcessStore::serialize(SPL::blob & data, std::tr1::unordered_set<KeyValueType> const & keyValueTypes) const
        {
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::READ);
            std::tr1::unordered_map<KeyValueType, TypedStoreView> typedStores;   
            for (ProcessStoreMap::const_iterator it = map_.begin(); it != map_.end(); ++it) {
                ProcessStoreValue const & skey = it->first;
                ProcessStoreValue const & svalue = it->second;
                KeyValueType keyValueType(skey.getConstHandle().getTypeId().name(),
                                          svalue.getConstHandle().getTypeId().name());
                if(keyValueTypes.count(keyValueType)==0)
                    continue; // not it the given set of types
                std::tr1::unordered_map<KeyValueType, TypedStoreView>::iterator 
                    it = typedStores.find(keyValueType);
                if(it==typedStores.end()) {
                    TypedStoreView storeView(keyValueType);
                    it =  typedStores.insert(std::make_pair(keyValueType, storeView)).first;
                }
                KeyValuePair keyValuePair(&skey, &svalue);
                it->second.getData().push_back(keyValuePair);
            }
            
            uint64_t size;
            unsigned char * rdata = data.releaseData(size);            
            SPL::NativeByteBuffer nbf(rdata, size);
            nbf.setICursor(0);
            nbf.setAutoDealloc(true);            

            for (std::tr1::unordered_map<KeyValueType, TypedStoreView>::const_iterator 
                     it = typedStores.begin(); it != typedStores.end(); ++it) {
                TypedStoreView const & storeView = it->second;
                KeyValueType const & type = storeView.getType();
                nbf << type.getKeyType();
                nbf << type.getValueType(); 
                std::vector<KeyValuePair> const & data = storeView.getData(); 
                uint64_t contentSizeCursor = nbf.getICursor();
                nbf.addUInt64(0); // will fix up later
                for(std::vector<KeyValuePair>::const_iterator 
                        it=data.begin(); it!= data.end(); ++it) {
                    KeyValuePair const & keyValue = *it;
                    ProcessStoreValue const & skey = * keyValue.getKey();
                    ProcessStoreValue const & svalue = * keyValue.getValue();
                    skey.getConstHandle().serialize(nbf);
                    svalue.getConstHandle().serialize(nbf);                    
                }
                uint64_t cursor = nbf.getICursor();
                nbf.setICursor(contentSizeCursor);
                uint64_t contentSize = cursor-(contentSizeCursor+sizeof(uint64_t));
                nbf.addUInt64(contentSize);
                nbf.setICursor(cursor);
            }           

            nbf.setAutoDealloc(false); 
            data.adoptData(nbf.getPtr(), nbf.getSerializedDataSize());                
        }

        class KeyValueStorePair 
        {
        private:            
            ProcessStoreValue key_;
            ProcessStoreValue value_;
        public:
            KeyValueStorePair(ProcessStoreValue const & key, 
                              ProcessStoreValue const &value)
                : key_(key), value_(value) {}           
            ProcessStoreValue & getKey() 
            {
                return key_;
            }
            ProcessStoreValue & getValue() 
            {
                return value_;
            }
        };

        static void collectKeyValueTypes(SPL::Tuple const & types, 
          std::tr1::unordered_map<KeyValueType, KeyValueStorePair> & keyValueTypes)
        {
            for(SPL::ConstTupleIterator it=types.getBeginIterator(); it!=types.getEndIterator(); ++it)
            {
                SPL::ConstTupleAttribute attribute = *it;
                SPL::ConstValueHandle handle = attribute.getValue();
                if(handle.getMetaType()!=SPL::Meta::Type::TUPLE)
                    continue;
                SPL::Tuple const & kv = handle;
                if(kv.getNumberOfAttributes()!=2)
                    continue;
                SPL::ConstValueHandle keyHandle = kv.getAttributeValue(0);
                SPL::ConstValueHandle valueHandle = kv.getAttributeValue(1);
                KeyValueType keyValueType(keyHandle.getTypeId().name(),
                                          valueHandle.getTypeId().name());
                ProcessStoreValue keyPsv(keyHandle);
                ProcessStoreValue valuePsv(valueHandle);
                KeyValueStorePair keyValueSE(keyPsv, valuePsv);
                keyValueTypes.insert(std::make_pair(keyValueType, keyValueSE));
            }                
        }

        void ProcessStore::deserialize(SPL::blob const & data, SPL::Tuple const & types)
        {                
            lock::AutoCondLock am(rwlock_, synchronized_, lock::RWLock::WRITE);
            map_.clear();
            std::tr1::unordered_map<KeyValueType, KeyValueStorePair> keyValueTypes;
            store::collectKeyValueTypes(types, keyValueTypes);
            
            uint64_t size;
            unsigned char const * rdata = data.getData(size);
            SPL::NativeByteBuffer nbf(const_cast<unsigned char*>(rdata), size);

            uint64_t endCursor;
            while(nbf.getNRemainingBytes()>0) {
                KeyValueType keyValueType;
                nbf >> keyValueType.getKeyType();
                nbf >> keyValueType.getValueType();
                nbf >> endCursor;
                endCursor += nbf.getOCursor();
    
                std::tr1::unordered_map<KeyValueType, KeyValueStorePair>::iterator it = keyValueTypes.find(keyValueType);
                if(it==keyValueTypes.end()) {
                    nbf.setOCursor(endCursor);
                    continue;                    
                }                
                ProcessStoreValue & skey(it->second.getKey());
                ProcessStoreValue & svalue(it->second.getValue());
                while (nbf.getOCursor()<endCursor) {
                    skey.getHandle().deserialize(nbf);
                    svalue.getHandle().deserialize(nbf);
                    map_.insert(std::make_pair(skey, svalue));
                }
            } 
        }
    }
} } } }
    
