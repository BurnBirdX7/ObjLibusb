#ifndef OBJLIBUSB_LIBUSBTRANSFER_HPP
#define OBJLIBUSB_LIBUSBTRANSFER_HPP

#include <memory>
#include <functional>
#include <variant>
#include <atomic>
#include <mutex>

#include "ObjLibusb.hpp"


namespace ObjLibusb {

    class Transfer
            : public std::enable_shared_from_this<Transfer>
    {
    public:
        using UniqueLock = std::unique_lock<std::recursive_mutex>;

        using SharedPointer = std::shared_ptr<Transfer>;
        using WeakPointer = SharedPointer::weak_type;
        using TransferCallback = std::function<void(const SharedPointer& transfer, const UniqueLock& lock)>;

        enum State : int {
            EMPTY           =  0,
            READY           =  1,
            SUBMITTED       =  2,
            IN_CALLBACK     =  3,
            };

        enum Type : uint8_t {
            CONTROL = LIBUSB_TRANSFER_TYPE_CONTROL,
            ISOCHRONOUS = LIBUSB_TRANSFER_TYPE_ISOCHRONOUS,
            BULK = LIBUSB_TRANSFER_TYPE_BULK,
            INTERRUPT = LIBUSB_TRANSFER_TYPE_INTERRUPT,
            BULK_STREAM = LIBUSB_TRANSFER_TYPE_BULK_STREAM
        };

        enum Status : uint8_t {
            COMPLETED = LIBUSB_TRANSFER_COMPLETED,
            TRANSFER_ERROR = LIBUSB_TRANSFER_ERROR,
            TIMED_OUT = LIBUSB_TRANSFER_TIMED_OUT,
            CANCELLED = LIBUSB_TRANSFER_CANCELLED,
            STALL = LIBUSB_TRANSFER_STALL,
            NO_DEVICE = LIBUSB_TRANSFER_NO_DEVICE,
            TRANSFER_OVERFLOW = LIBUSB_TRANSFER_OVERFLOW
        };

        enum Flag : uint8_t {
            SHORT_NOT_OK = LIBUSB_TRANSFER_SHORT_NOT_OK,
            FREE_BUFFER = LIBUSB_TRANSFER_FREE_BUFFER,
            FREE_TRANSFER = LIBUSB_TRANSFER_FREE_TRANSFER,
            ADD_ZERO_PACKET = LIBUSB_TRANSFER_ADD_ZERO_PACKET
        };

        using LibusbError = libusb_error;

    public:
        Transfer(const Transfer&) = delete;
        Transfer(Transfer&&) = delete;
        ~Transfer();

        static SharedPointer createTransfer(int isoPacketsNumber = 0);

        // Control
        bool submit(const UniqueLock& lock); // READY-state only
        void cancel(const UniqueLock& lock); // SUBMITTED-state only

        LibusbError getLastError() const;

        // Locks
        UniqueLock getUniqueLock();

        // You need to pass shared or unique lock to these functions //
        uint8_t getFlags        (const UniqueLock& lock) const;
        uint8_t getEndpoint     (const UniqueLock& lock) const;
        uint8_t getType         (const UniqueLock& lock) const;
        unsigned int getTimeout (const UniqueLock& lock) const;
        uint8_t getStatus       (const UniqueLock& lock) const;
        int getLength           (const UniqueLock& lock) const;
        int getActualLength     (const UniqueLock& lock) const;
        uint8_t* getBuffer      (const UniqueLock& lock) const;
        void* getUserData       (const UniqueLock& lock) const;

        void setNewBuffer   (unsigned char* buffer, uint8_t length, const UniqueLock& lock);
        void setNewUserData (void* userData,                        const UniqueLock& lock);
        void setNewCallback (TransferCallback callback,             const UniqueLock& lock);
        void deleteCallback (                                       const UniqueLock& lock);

        // ANY-state:
        State getState() const;

        // READY-state only:
        void reset                  (             const UniqueLock& lock);
        void enableFreeBufferFlag   (bool enable, const UniqueLock& lock);
        void enableAddZeroPacketFlag(bool enable, const UniqueLock& lock);

        /* EMPTY-state only:
         * NOTE:
         * user must destroy supplied buffer
         * ONLY after callback is called
         */
        void fillBulk(const DeviceHandle& device,
                      uint8_t             endpoint,
                      unsigned char*      buffer,
                      int                 length,
                      TransferCallback    callback,
                      void*               userData,
                      unsigned int        timeout,
                      const UniqueLock&   lock);

        // Callback's auxiliary
        static void staticCallbackWrapper(libusb_transfer* libusbTransfer);


    private:
        // Callback's auxiliary
        static bool staticSubmitTransfer(Transfer& transfer, const UniqueLock& lock);
        void callbackWrapper(const UniqueLock& lock);

        // Threads auxiliary
        bool isLocked(const UniqueLock & lock) const;

    private:
        explicit Transfer(int isoPacketsNumber);

        mutable std::recursive_mutex mMutex;

        libusb_transfer* mTransfer;
        std::atomic<State> mState;
        TransferCallback mUserCallback;
        void* mUserData;

        std::atomic<LibusbError> mLastError;
    };

}

// Alias
using ObjLibusbTransfer = ObjLibusb::Transfer;

#endif //OBJLIBUSB_LIBUSBTRANSFER_HPP
