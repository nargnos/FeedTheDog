#pragma once


#include <json\json.h>
#define ASIO_HAS_MOVE
#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\bind\protect.hpp>
#include <boost\atomic.hpp>
#include <boost\pool\object_pool.hpp>
#include <boost\pool\pool.hpp>

#include <boost\utility.hpp>
#include <boost\thread.hpp>
#include <boost\lockfree\queue.hpp>
#include <boost\lockfree\spsc_queue.hpp>
#include <boost\lexical_cast.hpp>

#include <filesystem>
#include <fstream>
#include <vector>
#include <list>
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

using _STD shared_ptr;
using _STD unique_ptr;
using _STD weak_ptr;
using _STD make_shared;
using _STD make_unique;
using _STD dynamic_pointer_cast;
using _STD static_pointer_cast;

using Concurrency::concurrent_unordered_map;
using Concurrency::concurrent_unordered_set;

using Concurrency::concurrent_unordered_multimap;

#define _BOOST boost::
#define _ASIO _BOOST asio::
