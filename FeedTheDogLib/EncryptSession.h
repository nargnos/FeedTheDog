#pragma once


namespace FeedTheDog
{
	// TODO: 加密\压缩session，在创建时传入service，由service确定加密\压缩方式
	// 加密方式由service管理，不同的service可自由定义方式，遍历插件链调用处理函数
	// 加密可能会增加数据长度，需要返回新buffer（如果在buffer范围可以返回原buffer）
	// 多次buffer复制会导致性能下降，需要做好处理
	// 用修饰方式做可能较好
	// 劣势：需要实现跟session一样的接口，每个session都要保存service指针副本，影响性能
	// 优势：使用直观方便
	/*template<typename TSession>
	class EncryptSession
	{
	public:
	EncryptSession(shared_ptr<TSession>&& session)
	{
	}

	~EncryptSession()
	{
	}

	private:

	};*/
}  // namespace FeedTheDog
