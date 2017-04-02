#ifndef RUNPROXY_H_
#define RUNPROXY_H_


class Loop;
class RunProxy
{
public:
	static void Start(Loop& loop);
	static void Stop(Loop& loop);
	static void PrepareBuffers(Loop& loop);
private:
	RunProxy() = delete;
	~RunProxy() = delete;
};


#endif // RUNPROXY_H_

