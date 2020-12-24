#pragma once

#include "Utils/Globals.h"

#include <memory>
#include <vector>

template <class T, class D = std::default_delete<T>>
class Pool {

public:

	Pool(uint size){
		_size = size;
	}

	void Init(uint size) {
		_size = size;
		_pool = std::vector<std::unique_ptr<T>>(size);
	}

	void Add(std::unique_ptr<T> ptr){
		_pool.emplace_back(std::move(ptr));
	}

private:

	uint _size = 0;
	std::vector<std::unique_ptr<T>> _pool;

};
