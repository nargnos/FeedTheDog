#pragma once
#include "ServiceBase.h"
namespace FeedTheDog
{
	enum class Rfc1928ServiceType
	{
		Server,
		// 其它扩展待编写
	};
	// 根据协议原文声明类型
#pragma pack(push,1)
	// The client connects to the server, and sends a version
	// identifier / method selection message :
	// +---- + ---------- + ---------- +
	// | VER | NMETHODS   |   METHODS  |
	// +---- + ---------- + ---------- +
	// |  1  |      1     |  1 to 255  |
	// +---- + ---------- + ---------- +
	struct VersionMethodMessage
	{
		// X'05'
		unsigned char Ver;
		unsigned char NMethods;
		unsigned char Methods[];
	};
	// The server selects from one of the methods given in METHODS, and
	// sends a METHOD selection message :

	// +---- + -------- +
	// | VER |  METHOD  |
	// +---- + -------- +
	// |  1  |     1    |
	// +---- + -------- +
	// If the selected METHOD is X'FF', none of the methods listed by the
	// client are acceptable, and the client MUST close the connection.
	//   X'00' NO AUTHENTICATION REQUIRED
	//   X'01' GSSAPI（通用安全服务应用程序接口），Compliant implementations MUST support GSSAPI and SHOULD support USERNAME / PASSWORD authentication methods.
	//   X'02' USERNAME / PASSWORD
	//   X'03' to X'7F' IANA ASSIGNED
	//   X'80' to X'FE' 私有函数保留位
	//   X'FF' 不接受客户端请求的函数
	struct MethodSelectionMessage
	{
		unsigned char Ver;
		// X'FF'关连接
		// x01先放着吧
		unsigned char Method;
	};


#pragma pack(pop)

	void J()
	{
		VersionMethodMessage k;
		k.Methods[0] = 1;
	}
	class Rfc1928 :
		public ServiceBase
	{
	public:
		Rfc1928(const char* name);
		~Rfc1928();
		static constexpr int ddd = sizeof(VersionMethodMessage);
		// 通过 ServiceBase 继承
		virtual void AsyncStart() override;
		virtual void Stop() override;
		virtual bool Init(TCore *) override;
	private:
	};
}  // namespace FeedTheDog


