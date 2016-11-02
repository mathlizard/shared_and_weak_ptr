#include <iostream>
#include <unordered_map>

// - shared_ptr - 

template<typename T>
class ptr_table{
protected:
	static std::unordered_map<T*,size_t> table_;
};

template<typename T>
std::unordered_map<T*,size_t> ptr_table<T>::table_;


template<typename T>
class shared_ptr : public ptr_table<T> {
private:
	T* ptr_;
	using ptr_table<T>::table_;
public:
	explicit shared_ptr( T* ptr = nullptr ) noexcept : ptr_(ptr) { table_[ptr] = 1u; }
	
	void reset( T* ptr = nullptr ) noexcept { 
		if(--table_[ptr_] == 0u){
			table_.erase(ptr_);
			delete ptr_;
		}
		ptr_ = ptr; 
		table_[ptr] = 1u;
	}
	
	~shared_ptr() noexcept {
		if(--table_[ptr_] == 0u){
			table_.erase(ptr_);
			delete ptr_;
		}
	}
	
	shared_ptr& operator=(shared_ptr<T> const& other) noexcept { 
		if(--table_[ptr_] == 0u){
			table_.erase(ptr_);
			delete ptr_;
		}
		ptr_ = other.ptr_;
		++table_[ptr_];
	}
	
	shared_ptr& operator=(shared_ptr<T>&& other) noexcept { 
		std::swap(ptr_,other.ptr_);
	}
	
	operator bool() const noexcept { return ptr_ == nullptr; }
	
	T* get() const noexcept { return ptr_; }
	
	T operator*() const noexcept { return *ptr_; }
	
	T* operator->() const noexcept { return ptr_; }
	
	size_t use_count() const noexcept { return table_[ptr_]; }
	
	bool unique() const noexcept { return table_[ptr_] == 1u; }
	
	void swap(shared_ptr<T>& other) noexcept {
		std::swap(ptr_,other.ptr_);
	}
	
};


// - weak_ptr -

template <typename T>
class weak_ptr : public ptr_table<T> {
private:
	T* ptr_;
	using ptr_table<T>::table_;
public:
	explicit weak_ptr( shared_ptr<T> const& other = shared_ptr<T>() ) noexcept : ptr_(other.get()) {}
	
	void reset( shared_ptr<T> const& other = shared_ptr<T>() ) noexcept { ptr_ = other.get(); }
	
	~weak_ptr() noexcept = default;
	
	weak_ptr& operator=(weak_ptr<T> const& other) noexcept {
		ptr_ = other.ptr_;
	}
	
	weak_ptr& operator=(shared_ptr<T> const& other) noexcept {
		ptr_ = other.get();
	}
	
	weak_ptr& operator=(weak_ptr<T>&& other) noexcept { 
		std::swap(ptr_,other.ptr_);
	}
	
	size_t use_count() const noexcept { return table_[ptr_]; }
	
	bool expired() const noexcept { return table_.find(ptr_) == table_.end(); }
	
	shared_ptr<T> lock() const noexcept {
		return shared_ptr<T>(ptr_);
	}
	
	void swap(weak_ptr<T>& other) noexcept {
		std::swap(ptr_,other.ptr_);
	}

};
