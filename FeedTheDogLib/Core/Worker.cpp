#include "Worker.h"
#include "Core.h"
namespace FeedTheDog
{
	Worker::Worker(TCore * corePtr) :
		core(corePtr)
	{
		threadCount = _BOOST thread::hardware_concurrency() * 2;

	}

	Worker::~Worker()
	{
		Stop();
	}

	int Worker::Count() const
	{
		return threadCount;
	}

	void Worker::Start()
	{
		auto& ioservice = core->GetIoService();
		_STD vector<shared_ptr<_BOOST thread>> threads;
		auto tmpCount = threadCount - 1;
		if (tmpCount > 0)
		{
			for (auto i = 0; i < tmpCount; i++)
			{
				auto tmpThread = make_shared<_BOOST thread>(_BOOST bind(&_ASIO io_service::run, &ioservice));

				threads.push_back(tmpThread);
			}
		}
		// 自身线程也算在内
		ioservice.run();
		// join等待所有线程结束
		for each (auto& var in threads)
		{
			var->join();
		}
	}
	void Worker::Stop()
	{
		// TODO:
	}

}  // namespace FeedTheDog

