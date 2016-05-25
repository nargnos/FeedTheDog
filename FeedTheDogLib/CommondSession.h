#pragma once
namespace FeedTheDog
{
	// TODO: 加密\压缩session的另一个实现方式
	// 把加密函数们和原session打包，传递给管理类处理发送，接收到数据提交给管理类解析
	// 可以在各自的类里实现处理方式，由管理类调用就行
	// 劣势：需要实现区分每一条命令的函数或类，产生过多对象，使用不方便
	// 优势：扩展方便

	// 另一个实现方式：
	// 由管理类直接处理发送接收
	// 劣势：需要实现跟session一样的接口并扩展参数，使用不方便
	// 优势：比上面的好一点

	// 实现方式+：
	// 使用session的无buffer实例，并替换扩展的buffer，类似deque，加解密压缩什么的如果大于buffer附加新buffer，发送什么的就发送整个链
	// 好像可行

	//class CommondSession
	//{
	//public:
	//	CommondSession()
	//	{
	//	}

	//	~CommondSession()
	//	{
	//	}

	//private:

	//};
}  // namespace FeedTheDog

