#pragma once

#include "tested.h"

////////////////////////////////////////////////////////
#include <memory>
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>

#include <condition_variable>
#include <mutex>
#include <shared_mutex >
#include <thread>
#include <vector>
#include <queue>

//#include <variant>
#include <functional>
#include <optional>


#include <future>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

#include <assert.h>

//////////////////////////////////////////////////////////////////////////////////


class Semaphore {
public:
	explicit Semaphore(int max_count = 0) : max_count_(max_count) {}

	void acquire() {
		std::unique_lock<std::mutex> lock(mut_);
		while (count_ == max_count_) cv_.wait(lock);
		++count_;
	}

	void release() {
		std::lock_guard<std::mutex> lock(mut_);
		//assert(count_ > 0); //todo check it
		--count_;
		cv_.notify_one();
	}


protected:
	int max_count_;
	int count_ = 0;
	std::mutex mut_;
	std::condition_variable cv_;
};

template< typename T >
class QueueSemaphore : public Semaphore
{
public:

	friend class QueueSemaphore;
	class Finish {
	public:
		Finish(QueueSemaphore * p) :m_p(p) {};
		~Finish()
		{
			m_p->my_queue.pop_front();
			++m_p->count_;
		}
		QueueSemaphore * m_p;
	};


	std::optional<std::shared_ptr<T>> get()
	{
			std::unique_lock<std::mutex> lock(mut_);
			while (count_ == max_count_) cv_.wait(lock);
			if (!(my_queue.empty()))
			{
				Finish final(this);//for destructor
				return my_queue.front();				
			}
			++count_;
			return {};
	}

	void push(std::shared_ptr<T> data)
	{
		std::lock_guard<std::mutex> lock(mut_);
		my_queue.push_back(data);
		--count_;
		cv_.notify_one();
	}

	bool isEmpty()
	{
		return my_queue.empty();
	}

protected:
	std::deque<std::shared_ptr<T>> my_queue;
};

template class QueueSemaphore<TEST>;

///////////////////////////////////////////////////////////////////


class LogDuration {
public:
	explicit LogDuration(const string& msg = "")
		: message(msg + ": ")
		, start(steady_clock::now())
	{
	}

	~LogDuration() {
		auto finish = steady_clock::now();
		auto dur = finish - start;
		//printf("%s ", message.c_str());
		std::cout << message.c_str() << duration_cast<milliseconds>(dur).count() << " ms" << std::endl;


	}
private:
	string message;
	steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};

/////////////////////////////////////////////////////////////////////////////////


template< typename T >
class PULL
{
public:

	PULL(int num_threads = 4)
	{
		m_num_threads = num_threads;
		for (int i = 0; i < num_threads; i++)
		{
			m_threads.push_back(std::async(std::launch::async, &PULL::run, this));
		}
	};

	~PULL()
	{
		stop();
		for (int i = 0; i < m_threads.size(); i++) m_threads[i].wait();
	}

	int run()
	{
		while (true) {
			if (m_stop) break;
			std::optional<std::shared_ptr<T>> a = queue.get();
			if (a)
				(*a)->run();
			else
				printf("queue empty - no data - final activity\n");
		}
		return 0;
	}

	void stop()
	{
		m_stop = 1;
		for (size_t a = 0; a < m_num_threads; a++) queue.release();
	}

	void push(std::shared_ptr<T>  data)
	{
		queue.push(data);
	}

private:

	QueueSemaphore<T> queue;	
	std::vector<std::future<int>> m_threads;
	size_t m_num_threads;
	mutable std::atomic<int> m_stop{ 0 };

};


template class PULL<TEST>;