#pragma once
#include <iostream>
#include <queue>
#include <mutex>
using namespace std;

std::mutex mtx;
template <typename T>
class BufferManager
{
public:
	BufferManager(int capacity);
	~BufferManager();
	int PushBuffer(T* item, int size);
	const T* PopBuffer(int* size);

private:
	int capacity;//容量，单位元素
	queue<pair<T*, int>> buffer;//pair中item0为地址，item1为长度
};

template<typename T>
inline BufferManager<T>::BufferManager(int capacity)
	:capacity(capacity)
{

}

template<typename T>
inline BufferManager<T>::~BufferManager()
{
}

template<typename T>
inline int BufferManager<T>::PushBuffer(T* item, int size)
{
	pair<T*, int> p = pair<T*, int>(item, size);
	std::lock_guard<std::mutex> lk(mtx);
	buffer.push(p);
	if (buffer.size() > capacity)
	{
		buffer.pop();
	}
	//cout << "Push" << endl;
	return 0;
}

template<typename T>
inline const T* BufferManager<T>::PopBuffer(int* size)
{
	std::lock_guard<std::mutex> lk(mtx);
	if (buffer.size() == 0)
		return 0;

	pair<T*, int> t = buffer.front();
	buffer.pop();
	//cout << "Pop" << endl;
	*size = t.second;
	return t.first;
}
