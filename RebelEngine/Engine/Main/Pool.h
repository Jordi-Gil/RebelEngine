#pragma once

#include "Utils/Globals.h"

#include <memory>
#include <vector>

template <class T>
class Pool {

public:

	Pool(){
		_size = 0;
		_pool = std::vector<std::unique_ptr<T>>();
	}

	Pool(uint size) { 
		_size = size;
		for(int i = 0; i < _size; i++) _pool.push_back(std::make_unique<T>());
	}

	std::unique_ptr<T> get() {

		if(_pool.empty()) {
			//Allocate a new group of objects
			for(uint i = 0; i < 1000; i++) Add();
		}

		std::unique_ptr<T> ptr(_pool.back().release());

		_pool.pop_back();
 		return std::move(ptr);
	}

	bool empty() const
	{
		return _pool.empty();
	}

	int size() const
	{
		return _pool.size();
	}

private:

	void Add() {
			
		_pool.push_back(std::make_unique<T>());
		++_size;

	}

private:

	uint _size = 0;
	std::vector<std::unique_ptr<T>> _pool;

};
