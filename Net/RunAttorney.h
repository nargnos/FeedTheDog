#ifndef RUNATTORNEY_H_
#define RUNATTORNEY_H_

namespace Detail
{

	class Loop;
	class RunAttorney
	{
	public:
		static void Start(Loop& loop);
		static void Stop(Loop& loop);
		static void PrepareBuffers(Loop& loop);
	private:
		RunAttorney() = delete;
		~RunAttorney() = delete;
	};
}  // namespace Detail


#endif // RUNATTORNEY_H_

