#include <cassert>
#include <utility>
#include "LibusbTransfer.hpp"
#include "LibusbError.hpp"
#include "LibusbDeviceHandle.hpp"

LibusbTransfer::LibusbTransfer(int isoPacketsNumber)
    : mTransfer(libusb_alloc_transfer(isoPacketsNumber))
    , mState(State::EMPTY)
    , mUserCallback()
    , mUserData(nullptr)
{
    if (mTransfer == nullptr)
        throw LibusbError(LIBUSB_ERROR_OTHER);
}

LibusbTransfer::~LibusbTransfer()
{
    libusb_free_transfer(mTransfer);
}

LibusbTransfer::SharedPointer LibusbTransfer::createTransfer(int isoPacketsNumber)
{
    return LibusbTransfer::SharedPointer(new LibusbTransfer(isoPacketsNumber));
}

void LibusbTransfer::submit(const UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    assert(mState == READY);
    staticSubmitTransfer(*this);
}

void LibusbTransfer::cancel(const UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    assert(mState == SUBMITTED);
    CHECK_LIBUSB_ERROR(libusb_cancel_transfer(mTransfer))
}

LibusbTransfer::SharedLock LibusbTransfer::getSharedLock() const
{
    return LibusbTransfer::SharedLock(mMutex);
}

LibusbTransfer::UniqueLock LibusbTransfer::getUniqueLock()
{
    return LibusbTransfer::UniqueLock(mMutex);
}

// GETTERS DEFINITION:

uint8_t LibusbTransfer::getFlags(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState >= READY);
    return mTransfer->flags;
}

uint8_t LibusbTransfer::getEndpoint(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState >= READY);
    return mTransfer->endpoint;
}

uint8_t LibusbTransfer::getType(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState >= READY);
    return mTransfer->type;
}

uint LibusbTransfer::getTimeout(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState >= READY);
    return mTransfer->timeout;
}

uint8_t LibusbTransfer::getStatus(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState == IN_CALLBACK);
    return mTransfer->status;
}

int LibusbTransfer::getLength(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState >= READY);
    return mTransfer->length;
}

int LibusbTransfer::getActualLength(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState == IN_CALLBACK);
    return mTransfer->actual_length;
}

uint8_t* LibusbTransfer::getBuffer(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState >= READY);
    return mTransfer->buffer;
}

void* LibusbTransfer::getUserData(const VariantLock& lock) const
{
    assert(isVariantLocked(lock));
    assert(mState >= READY);
    return mUserData;
}

void LibusbTransfer::setNewBuffer(unsigned char* buffer, uint8_t length, const UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    assert(mState <= READY || mState == IN_CALLBACK);
    mTransfer->buffer = buffer;
    mTransfer->length = length;
}

void LibusbTransfer::setNewUserData(void* userData, const UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    assert(mState <= READY || mState == IN_CALLBACK);
    mUserData = userData;
}

void LibusbTransfer::setNewCallback(LibusbTransfer::TransferCallback callback, const LibusbTransfer::UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    mUserCallback = std::move(callback);
}

void LibusbTransfer::deleteCallback(const LibusbTransfer::UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    mUserCallback = {};
}


LibusbTransfer::State LibusbTransfer::getState() const
{
    return mState;
}

void LibusbTransfer::reset(const UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    assert(mState == READY);
    const auto isoPackets = mTransfer->num_iso_packets;
    libusb_free_transfer(mTransfer);

    mTransfer = libusb_alloc_transfer(isoPackets);
    mState = EMPTY;
}

void LibusbTransfer::enableFreeBufferFlag(bool enable, const UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    assert(mState == READY);
    constexpr auto flag = Flag::FREE_BUFFER;
    if (enable)
        mTransfer->flags |= flag;
    else
        mTransfer->flags &= ~flag;
}

void LibusbTransfer::enableAddZeroPacketFlag(bool enable, const UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    assert(mState == READY);
    constexpr auto flag = Flag::ADD_ZERO_PACKET;
    if (enable)
        mTransfer->flags |= flag;
    else
        mTransfer->flags &= ~flag;
}

void LibusbTransfer::fillBulk(const LibusbDeviceHandle &device,
                              uint8_t endpoint,
                              unsigned char *buffer,
                              int length,
                              TransferCallback callback,
                              void *userData,
                              unsigned int timeout,
                              const UniqueLock* lock)
{
    assert(lock && isLocked(*lock));
    assert(mState == EMPTY && "Only empty transfer can be filled, call reset() if you want to refill transfer");

    mUserCallback = std::move(callback);
    mUserData = userData;
    libusb_fill_bulk_transfer(mTransfer,
                              device.mHandle,
                              endpoint,
                              buffer,
                              length,
                              LibusbTransfer::staticCallbackWrapper,
                              nullptr,
                              timeout);

    mState = READY;
}

void LibusbTransfer::staticCallbackWrapper(libusb_transfer* libusbTransfer)
{
    assert(libusbTransfer && libusbTransfer->user_data);

    // Acquire shared pointer
    auto* shared = static_cast<SharedPointer*>(libusbTransfer->user_data);
    auto& transfer = *shared;

    auto lock = transfer->getUniqueLock();
    transfer->callbackWrapper(&lock);

    // Destroy shared pointer
    delete shared;
    transfer->mTransfer->user_data = nullptr;
    transfer->mState = READY;
}

void LibusbTransfer::staticSubmitTransfer(LibusbTransfer& transfer)
{
    // This function constructs shared pointer from *this* in dynamic memory
    // This shared pointer has to be destroyed in callback

    transfer.mTransfer->user_data = new SharedPointer{transfer.shared_from_this()};
    CHECK_LIBUSB_ERROR(libusb_submit_transfer(transfer.mTransfer))
    transfer.mState = SUBMITTED;
}

bool LibusbTransfer::isVariantLocked(const LibusbTransfer::VariantLock& lockPtr) const
{
    if (std::holds_alternative<const UniqueLock*>(lockPtr))
        return isLocked(*std::get<const UniqueLock*>(lockPtr));

    if (std::holds_alternative<const SharedLock*>(lockPtr))
        return isLocked(*std::get<const SharedLock*>(lockPtr));

    return false;
}

void LibusbTransfer::callbackWrapper(const UniqueLock* lock) {
    if (mUserCallback) { // if callback is set
        mState = IN_CALLBACK;
        mUserCallback(shared_from_this(), *lock);
    }
}
