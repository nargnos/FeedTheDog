#pragma once

#ifdef _WINDOWS_
#include <Windows.h>
#endif


#include <json\json.h>
// #define ASIO_HAS_MOVE
#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\align.hpp>
#include <boost\pool\object_pool.hpp>
#include <boost\pool\pool.hpp>
#include <boost\utility.hpp>
#include <boost\pool\pool_alloc.hpp>
#include <boost\lexical_cast.hpp>
#include <boost\algorithm\string.hpp>
#include <boost\endian\conversion.hpp>
#include <boost\container\allocator.hpp>
#include <boost\container\adaptive_pool.hpp>
#include <boost\container\deque.hpp>
#include <boost\thread.hpp>
#include <boost\thread\thread_pool.hpp>
// #include <boost\lockfree\stack.hpp>
#include <atomic>
#include <algorithm>
#include <fstream>
#include <vector>
#include <list>
#include <queue>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <time.h>
#include <ctime>
#include <memory>
#include <cassert>
#include <type_traits>
#include <mutex>
#include <filesystem>
#include <future>
#define FASTCALL __fastcall
#define _BOOST boost::
#define _ASIO _BOOST asio::

using _STD shared_ptr;
using _STD unique_ptr;
using _STD weak_ptr;
using _STD make_shared;
using _STD make_unique;
using _STD dynamic_pointer_cast;
using _STD static_pointer_cast;
using _STD string;
using _STD ostringstream;
using _BOOST thread;
using _BOOST this_thread::get_id;
using _ASIO io_service;

using Concurrency::concurrent_unordered_map;
using Concurrency::concurrent_unordered_set;

using Concurrency::concurrent_unordered_multimap;

#define ALIGNSIZE 64
#define ALIGN alignas(ALIGNSIZE)//_declspec (align(ALIGNSIZE))

#define NOVTABLE __declspec(novtable)
using Tcp = _ASIO ip::tcp;
using Udp = _ASIO ip::udp;
using TcpSocket = Tcp::socket;
using UdpSocket = Udp::socket;

