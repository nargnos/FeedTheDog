#ifndef IOSTATE_H_
#define IOSTATE_H_
namespace Detail
{

	class IoState
	{
	public:
		enum IoStatus :char
		{
			None = 0,
			IoReady = 1 << 0,

			BufReady = 1 << 1,

			Close = 1 << 2,
			Error = 1 << 3,

			DoIo = BufReady | IoReady,
		};
		// 默认IO已准备好
		explicit IoState(IoStatus stat);
		explicit IoState(char val);
		explicit operator char() const;
		bool HasError() const;
		bool IsBuffReady() const;
		bool IsClose() const;
		bool IsDoIo() const;
		bool IsGood() const;
		bool IsIoReady() const;
		char Value() const;
		IoStatus State() const;
		void SetBuffReady(bool val);
		void SetClose();
		void SetError();
		void SetIoReady(bool val);
	private:
		union
		{
			IoStatus current_;
			char val_;
		};
	};
}  // namespace Detail
#endif // IOSTATE_H_

