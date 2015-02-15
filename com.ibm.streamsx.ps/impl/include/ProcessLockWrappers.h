/*
# Licensed Materials - Property of IBM
# Copyright IBM Corp. 2011, 2013
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
*/
#ifndef PROCESS_LOCK_WRAPPERS_H_
#define PROCESS_LOCK_WRAPPERS_H_

#include "RWLock.h"

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace lock
    {
        /// Unlock the global read write lock
        inline void plUnlock()
        {
            RWLockFactory::getGlobalFactory().getCommonLock().unlock();
        }        

        /// Read lock on the global read write lock
        inline void plReadLock()
        {
            RWLockFactory::getGlobalFactory().getCommonLock().rlock();
        }   

        /// Write lock on the global read write lock
        inline void plWriteLock()
        {
            RWLockFactory::getGlobalFactory().getCommonLock().wlock();
        }                

        /// Create a read write lock
        /// @return lock handle
        inline SPL::uint64 plCreateLock()
        {
            return RWLockFactory::getGlobalFactory().createLock();
        }

        /// Create a named read write lock
        /// @param name lock name
        /// @return lock handle if created, 0 otherwise (a lock with the same
        /// name exists)
        inline SPL::uint64 plCreateLock(SPL::rstring const & name)
        {
            return RWLockFactory::getGlobalFactory().createLock(name);
        }

        /// Create a named read write lock pr get it if it already exists
        /// @param name lock name
        /// @return lock handle 
        inline SPL::uint64 plCreateOrGetLock(SPL::rstring const & name)
        {
            return RWLockFactory::getGlobalFactory().createOrGetLock(name);
        }

        /// Destroy a read write lock
        /// @param handle lock handle
        inline void plDestroy(SPL::uint64 handle)
        {
            RWLockFactory::getGlobalFactory().destroyLock(handle);
        }        

        /// Find a named read write lock
        /// @param name lock name
        /// @return lock handle if exists, 0 otherwise.
        inline SPL::uint64 plFindLock(SPL::rstring const & name)
        {
            return RWLockFactory::getGlobalFactory().findLock(name);
        }

        /// Unlock a read write lock
        /// @param handle lock handle
        inline void plUnlock(SPL::uint64 handle)
        {
            RWLockFactory::getGlobalFactory().getLock(handle).unlock();
        }        

        /// Read lock on a read write lock
        /// @param handle lock handle
        inline void plReadLock(SPL::uint64 handle)
        {
            RWLockFactory::getGlobalFactory().getLock(handle).rlock();
        }   

        /// Write lock on a read write lock
        /// @param handle lock handle
        inline void plWriteLock(SPL::uint64 handle)
        {
            RWLockFactory::getGlobalFactory().getLock(handle).wlock();
        }                
    }
} } } }

#endif /* PROCESS_LOCK_WRAPPERS_H_ */
