#ifndef GETSOCKETATTORNEY_H_
#define GETSOCKETATTORNEY_H_

namespace Detail
{

	class GetSocketAttorney
	{
	public:
		template<typename TConn>
		static TcpSocket& Socket(TConn& conn)
		{
			return conn.Socket();
		}
	private:
		GetSocketAttorney() = delete;
		~GetSocketAttorney() = delete;
	};
}  // namespace Detail
#endif // GETSOCKETATTORNEY_H_

