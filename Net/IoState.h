#ifndef IOSTATE_H_
#define IOSTATE_H_

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
	IoState(IoStatus stat) :
		current_(stat)
	{
	}
	explicit IoState(char val);
	explicit operator char() const;
	char Value() const;
	IoStatus State() const;
	bool HasError() const;
	bool IsClose() const;
	bool IsGood() const;
	void SetIoReady(bool val);
	void SetBuffReady(bool val);
	void SetClose();
	void SetError();
	bool IsIoReady() const;
	bool IsBuffReady() const;
	bool IsDoIo() const;
private:
	union
	{
		IoStatus current_;
		char val_;
	};
};
#endif // IOSTATE_H_

