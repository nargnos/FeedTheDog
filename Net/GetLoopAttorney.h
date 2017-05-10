#ifndef GETLOOPATTORNEY_H_
#define GETLOOPATTORNEY_H_
#include <type_traits>
namespace Detail
{

	class Loop;
	class GetLoopAttorney
	{
	public:
		template<typename T>
		static Loop& GetLoop(T& obj)
		{
			return obj.GetLoop();
		}
		template<typename T>
		static const Loop& GetLoop(const T& obj)
		{
			return obj.GetLoop();
		}

	private:
		GetLoopAttorney() = delete;
		~GetLoopAttorney() = delete;
	};

}  // namespace Detail

#endif // GETLOOPATTORNEY_H_

