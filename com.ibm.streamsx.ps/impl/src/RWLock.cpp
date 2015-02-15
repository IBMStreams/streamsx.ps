/*
# Licensed Materials - Property of IBM
# Copyright IBM Corp. 2011, 2013
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
*/
#include "RWLock.h"

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace lock
    {
        RWLockFactory::RWLockFactory()
            : lockCounter_(0) {}

        SPL::uint64 RWLockFactory::createLock()
        {
            std::ostringstream ostr;
            ostr << "auto_" << lockCounter_++;
            SPL::rstring name = ostr.str();
            return createLock(name);
        }
        
        SPL::uint64 RWLockFactory::createLock(SPL::rstring const & name)
        {
            AutoLock al(factoryLock_, RWLock::WRITE);
            SPL::rstring modName = "named_" + name;
            if(lockMap_.count(modName)!=0)
                return 0;
            std::tr1::shared_ptr<RWLockEntry> entryPtr(new RWLockEntry());
            entryPtr->second = modName;
            lockMap_.insert(std::make_pair(modName, entryPtr));
            return reinterpret_cast<SPL::uint64>(entryPtr.get());
        }

        SPL::uint64 RWLockFactory::createOrGetLock(SPL::rstring const & name)
        {
            AutoLock al(factoryLock_, RWLock::WRITE);
            SPL::rstring modName = "named_" + name;
            RWLockFactoryMap::iterator it = lockMap_.find(modName);
            if(it==lockMap_.end()) {
                std::tr1::shared_ptr<RWLockEntry> entryPtr(new RWLockEntry());
                entryPtr->second = modName;
                lockMap_.insert(std::make_pair(modName, entryPtr));
                return reinterpret_cast<SPL::uint64>(entryPtr.get());
            } else {
                std::tr1::shared_ptr<RWLockEntry> entryPtr = it->second;
                return reinterpret_cast<SPL::uint64>(entryPtr.get());                
            }
        }
        
        SPL::uint64 RWLockFactory::findLock(SPL::rstring const & name)
        {
            AutoLock al(factoryLock_, RWLock::READ);
            SPL::rstring modName = "named_" + name;
            RWLockFactoryMap::iterator it = lockMap_.find(modName);
            if(it==lockMap_.end())
                return 0;                
            std::tr1::shared_ptr<RWLockEntry> entryPtr = it->second;
            return reinterpret_cast<SPL::uint64>(entryPtr.get());
        }            
        
        RWLock & RWLockFactory::getLock(SPL::uint64 handle)
        {
            AutoLock al(factoryLock_, RWLock::READ);
            RWLockEntry * entry = reinterpret_cast<RWLockEntry *>(handle);
            return entry->first;
        }            
            
        void RWLockFactory::destroyLock(SPL::uint64 handle)
        {
            AutoLock al(factoryLock_, RWLock::WRITE);
            RWLockEntry * entry = reinterpret_cast<RWLockEntry *>(handle);
            SPL::rstring key = entry->second;
            lockMap_.erase(key);
        }     

        RWLock & RWLockFactory::getCommonLock()
        {
            return commonLock_;
        }                     

        RWLockFactory & RWLockFactory::getGlobalFactory()
        {
            static RWLockFactory factory;
            return factory;
        }      

        RWLock::RWLock() 
        {
            pthread_rwlock_init(&rwlock_, NULL);
        }

        void RWLock::lock(Mode mode) {
            if(mode==READ)
                rlock();
            else 
                wlock();
        }
        
        void RWLock::rlock()
        {
            bool done = false;
            while(!done) {
                int res = pthread_rwlock_rdlock(&rwlock_);
                if(res==0) {
                    done = true;
                } else {
                    if (res==EAGAIN) 
                        continue;
                    if (res==EDEADLK)
                        throw SPL::SPLRuntimeException("", "Deadlock detected...");
                } 
            }
        }

        void RWLock::wlock()
        {
            int res = pthread_rwlock_wrlock(&rwlock_);                
            if (res == EDEADLK)
                throw SPL::SPLRuntimeException("", "Deadlock detected...");
        }

        void RWLock::unlock()
        {               
            pthread_rwlock_unlock(&rwlock_);                
        }        

        AutoLock::AutoLock(RWLock & lock, RWLock::Mode mode) 
            : lock_(lock) 
        {
            lock_.lock(mode);
        } 
        
        AutoLock::~AutoLock() 
        {
            lock_.unlock();
        }

        AutoCondLock::AutoCondLock(RWLock & lock, bool active, RWLock::Mode mode) 
            : lock_(lock) , active_(active)
        {
            if(active_)
                lock_.lock(mode);
        } 

        AutoCondLock::~AutoCondLock() 
        {
            if(active_)
                lock_.unlock();
        }

    }
} } } }
 
