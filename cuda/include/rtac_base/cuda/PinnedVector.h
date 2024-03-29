#ifndef _DEF_RTAC_BASE_CUDA_PINNED_VECTOR_H_
#define _DEF_RTAC_BASE_CUDA_PINNED_VECTOR_H_

#include <iostream>
#include <vector>

#include <cuda_runtime.h>

#include <rtac_base/cuda/utils.h>
#include <rtac_base/types/VectorView.h>

#ifndef RTAC_CUDACC
#include <rtac_base/types/common.h>
#endif

//#ifdef RTAC_CUDACC
//#include <thrust/device_ptr.h> // thrust is causing linking issues with OptiX for unclear reasons
//#endif

namespace rtac { namespace cuda {

template <typename T>
class HostVector;

template <typename T>
class DeviceVector;

template <typename T>
class PinnedVector
{
    public:

    using value_type      = T;
    using difference_type = std::ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = pointer;
    using const_iterator  = const_pointer;

    protected:

    T*     data_;
    size_t size_;
    size_t capacity_;

    void allocate(size_t size);
    void free();

    public:

    PinnedVector();
    PinnedVector(size_t size);
    PinnedVector(const PinnedVector<T>& other);
    PinnedVector(const HostVector<T>& other);
    PinnedVector(const DeviceVector<T>& other);
    PinnedVector(const std::vector<T>& other);
    ~PinnedVector();

    void copy_from_host(size_t size, const T* data);
    void copy_from_device(size_t size, const T* data);
    
    PinnedVector& operator=(const PinnedVector<T>& other);
    PinnedVector& operator=(const HostVector<T>& other);
    PinnedVector& operator=(const DeviceVector<T>& other);
    PinnedVector& operator=(const std::vector<T>& other);
    
    #ifndef RTAC_CUDACC
    PinnedVector(const types::Vector<T>& other);
    PinnedVector& operator=(const types::Vector<T>& other);
    #endif

    void resize(size_t size);
    void clear() { this->free(); }
    size_t size() const;
    size_t capacity() const;

    pointer       data();
    const_pointer data() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    auto view() const { return types::make_view(*this); }
    auto view()       { return types::make_view(*this); }

    value_type& operator[](size_t idx);
    const value_type& operator[](size_t idx) const;

    value_type& front();
    const value_type& front() const;
    value_type& back();
    const value_type& back() const;

    #ifdef RTAC_CUDACC  // the following methods are only usable in CUDA code.
    //thrust::device_ptr<T>       begin_thrust();
    //thrust::device_ptr<T>       end_thrust();
    //thrust::device_ptr<const T> begin_thrust() const;
    //thrust::device_ptr<const T> end_thrust() const;
    #endif
};

// implementation
template <typename T>
PinnedVector<T>::PinnedVector() :
    data_(NULL),
    size_(0),
    capacity_(0)
{}

template <typename T>
PinnedVector<T>::PinnedVector(size_t size) :
    PinnedVector()
{
    this->resize(size);
}

template <typename T>
PinnedVector<T>::PinnedVector(const PinnedVector<T>& other) :
    PinnedVector(other.size())
{
    *this = other;
}

template <typename T>
PinnedVector<T>::PinnedVector(const HostVector<T>& other) :
    PinnedVector(other.size())
{
    *this = other;
}

template <typename T>
PinnedVector<T>::PinnedVector(const DeviceVector<T>& other) :
    PinnedVector(other.size())
{
    *this = other;
}

template <typename T>
PinnedVector<T>::PinnedVector(const std::vector<T>& other) :
    PinnedVector(other.size())
{
    *this = other;
}

template <typename T>
PinnedVector<T>::~PinnedVector()
{
    this->free();
}

template <typename T>
void PinnedVector<T>::copy_from_host(size_t size, const T* data)
{
    this->resize(size);
    CUDA_CHECK( cudaMemcpy(reinterpret_cast<void*>(data_),
                           reinterpret_cast<const void*>(data),
                           sizeof(T)*size_,
                           cudaMemcpyHostToHost) );
}

template <typename T>
void PinnedVector<T>::copy_from_device(size_t size, const T* data)
{
    this->resize(size);
    CUDA_CHECK( cudaMemcpy(reinterpret_cast<void*>(data_),
                           reinterpret_cast<const void*>(data),
                           sizeof(T)*size_,
                           cudaMemcpyDeviceToHost) );
}

template <typename T>
PinnedVector<T>& PinnedVector<T>::operator=(const PinnedVector<T>& other)
{
    this->copy_from_host(other.size(), other.data());
    return *this;
}

template <typename T>
PinnedVector<T>& PinnedVector<T>::operator=(const HostVector<T>& other)
{
    this->copy_from_host(other.size(), other.data());
    return *this;
}

template <typename T>
PinnedVector<T>& PinnedVector<T>::operator=(const DeviceVector<T>& other)
{
    this->copy_from_device(other.size(), other.data());
    return *this;
}

template <typename T>
PinnedVector<T>& PinnedVector<T>::operator=(const std::vector<T>& other)
{
    this->copy_from_host(other.size(), other.data());
    return *this;
}

#ifndef RTAC_CUDACC
template <typename T>
PinnedVector<T>::PinnedVector(const types::Vector<T>& other) :
    PinnedVector(other.size())
{
    *this = other;
}

template <typename T>
PinnedVector<T>& PinnedVector<T>::operator=(const types::Vector<T>& other)
{
    this->copy_from_host(other.size(), other.data());
    return *this;
}
#endif

template <typename T>
void PinnedVector<T>::allocate(size_t size)
{
    this->free();
    CUDA_CHECK( cudaMallocHost(&data_, sizeof(T)*size) );
    capacity_ = size;
}

template <typename T>
void PinnedVector<T>::free()
{
    CUDA_CHECK( cudaFreeHost(data_) );
    capacity_ = 0;
    size_     = 0;
}

template <typename T>
void PinnedVector<T>::resize(size_t size)
{
    if(capacity_ < size)
        this->allocate(size);
    size_ = size;
}

template <typename T>
size_t PinnedVector<T>::size() const
{
    return size_;
}

template <typename T>
size_t PinnedVector<T>::capacity() const
{
    return capacity_;
}

template <typename T> typename PinnedVector<T>::
pointer PinnedVector<T>::data()
{
    return data_;
}

template <typename T> typename PinnedVector<T>::
const_pointer PinnedVector<T>::data() const
{
    return data_;
}

template <typename T> typename PinnedVector<T>::
iterator PinnedVector<T>::begin()
{
    return data_;
}

template <typename T> typename PinnedVector<T>::
iterator PinnedVector<T>::end()
{
    return data_ + size_;
}

template <typename T> typename PinnedVector<T>::
const_iterator PinnedVector<T>::begin() const
{
    return data_;
}

template <typename T> typename PinnedVector<T>::
const_iterator PinnedVector<T>::end() const
{
    return data_ + size_;
}

#ifdef RTAC_CUDACC
template <typename T> typename PinnedVector<T>::
value_type& PinnedVector<T>::operator[](size_t idx)
{
    return data_[idx];
}

template <typename T> const typename PinnedVector<T>::
value_type& PinnedVector<T>::operator[](size_t idx) const
{
    return data_[idx];
}

template <typename T> typename PinnedVector<T>::
value_type& PinnedVector<T>::front()
{
    return data_[0];
}

template <typename T> const typename PinnedVector<T>::
value_type& PinnedVector<T>::front() const
{
    return data_[0];
}

template <typename T> typename PinnedVector<T>::
value_type& PinnedVector<T>::back()
{
    return data_[this->size() - 1];
}

template <typename T> const typename PinnedVector<T>::
value_type& PinnedVector<T>::back() const
{
    return data_[this->size() - 1];
}

//template <typename T>
//thrust::device_ptr<T> PinnedVector<T>::begin_thrust()
//{
//    return thrust::device_pointer_cast(data_);
//}
//
//template <typename T>
//thrust::device_ptr<T> PinnedVector<T>::end_thrust()
//{
//    return thrust::device_pointer_cast(data_ + size_);
//}
//
//template <typename T>
//thrust::device_ptr<const T> PinnedVector<T>::begin_thrust() const
//{
//    return thrust::device_pointer_cast(data_);
//}
//
//template <typename T>
//thrust::device_ptr<const T> PinnedVector<T>::end_thrust() const
//{
//    return thrust::device_pointer_cast(data_ + size_);
//}
#endif //RTAC_CUDACC

}; //namespace cuda
}; //namespace rtac

template <typename T>
std::ostream& operator<<(std::ostream& os, const rtac::cuda::PinnedVector<T>& v)
{
    os << "(";
    auto it = v.begin();
    if(v.size() <= 16) {
        os << *it;
        it++;
        for(; it != v.end(); it++) {
            os << " " << *it;
        }
    }
    else {
        for(auto it = v.begin(); it != v.begin() + 3; it++) {
            os << *it << " ";
        }
        os << "...";
        for(auto it = v.end() - 3; it != v.end(); it++) {
            os << " " << *it;
        }
    }
    os << ")";
    return os;
}

#endif //_DEF_RTAC_BASE_CUDA_PINNED_VECTOR_H_
