#pragma once

#include "Utils/Globals.h"

#include <memory>
#include <vector>

template <class T>
class Pool {

public:

	Pool(){}

	void Init(uint size) {
		_size = size;
		_pool = std::vector<std::unique_ptr<T>>(size);
	}

	void Add(std::unique_ptr<T> ptr){
		_pool.emplace_back(std::move(ptr));
	}

private:

	unsigned int _size = 0;
	std::vector<std::unique_ptr<T>> _pool;

};
