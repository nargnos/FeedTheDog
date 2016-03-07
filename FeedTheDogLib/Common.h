#pragma once


#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\atomic.hpp>
#include <boost\utility.hpp>
#include <boost\thread.hpp>
#include <boost\lockfree\queue.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <type_traits>

using _STD shared_ptr;
using _STD unique_ptr;
using _STD make_shared;
using _STD make_unique;
using _STD dynamic_pointer_cast;
using _STD static_pointer_cast;


#define _BOOST boost::
#define _ASIO _BOOST asio::
