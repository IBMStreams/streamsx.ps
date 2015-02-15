/*
# Licensed Materials - Property of IBM
# Copyright IBM Corp. 2011, 2013
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with
# IBM Corp.
*/
#ifndef PROCESS_LOCK_RWLOCK_H_
#define PROCESS_LOCK_RWLOCK_H_

#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <tr1/memory>

namespace com {
namespace ibm {
namespace streamsx {
namespace process 
{
    namespace lock
    {
        /// A read write lock
        class RWLock 
        {
        public:
            /// lock mode
            enum Mode { READ, WRITE };

            /// Constructor
            ///
            RWLock(); 

            /// Lock the lock
            /// @param mode mode of the lock operation (read or write)
            void lock(Mode mode);

            /// Read lock the lock
            void rlock();

            /// Write lock the lock
            void wlock();

            /// Unlock the lock
            void unlock();

        private:
            pthread_rwlock_t rwlock_;
        };
        
        /// RAII class for scoped lock acquisition
        class AutoLock
        {
        public:
            /// Constructor (acquire the lock)
            /// @param lock the lock to acquire
            /// @param mode acquire mode
            AutoLock(RWLock & lock, RWLock::Mode mode);

            /// Destructor (release the lock)
            ///
            ~AutoLock();

        private:
            RWLock & lock_;
        };

        /// RAII class for conditional scoped lock acquisition
        class AutoCondLock
        {
        public:
            AutoCondLock(RWLock & lock, bool active, RWLock::Mode mode);
            ~AutoCondLock();

        private:
            RWLock & lock_;
            bool active_;
        };

        /// A lock factory
        class RWLockFactory
        {
        private:
            RWLock commonLock_;
            RWLock factoryLock_;
            SPL::uint64 lockCounter_;
            typedef std::pair<RWLock, SPL::rstring> RWLockEntry;
            typedef std::tr1::unordered_map<SPL::rstring, 
                std::tr1::shared_ptr<RWLockEntry> > RWLockFactoryMap;
            RWLockFactoryMap lockMap_;           
        public:
            /// Constructor
            ///
            RWLockFactory();

            /// Create a lock
            /// @return lock handle
            SPL::uint64 createLock();
            
            /// Create a lock
            /// @param name of the lock
            /// @return lock handle, or 0 if a lock with the same name exists
            SPL::uint64 createLock(SPL::rstring const & name);

            /// Create a named read write lock pr get it if it already exists
            /// @param name lock name
            /// @return lock handle 
            SPL::uint64 createOrGetLock(SPL::rstring const & name);

            /// Find a lock
            /// @param name name of the lock
            /// @return lock handle, or 0 if a lock with the given name does
            /// not exist
            SPL::uint64 findLock(SPL::rstring const & name);

            /// Get a lock
            /// @param handle lock handle
            /// @return lock
            RWLock & getLock(SPL::uint64 handle);
    
            /// Remove a lock
            /// @param lock handle
            void destroyLock(SPL::uint64 handle);

            /// Get the global lock
            /// @return the global lock
            RWLock & getCommonLock();

            /// Get the global lock factory
            /// @return the global lock factory
            static RWLockFactory & getGlobalFactory();
        };
    }
} } } }

#endif /* PROCESS_LOCK_RWLOCK_H_ */
