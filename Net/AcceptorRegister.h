#ifndef ACCEPTORREGISTER_H_
#define ACCEPTORREGISTER_H_
class IoService;
class ITcpAcceptor;
class AcceptorRegister
{
public:
	static void Register(IoService& ios, ITcpAcceptor& act);
	static void UnRegister(IoService& ios, ITcpAcceptor& act);
private:
	AcceptorRegister() = delete;
	~AcceptorRegister() = delete;
};

#endif // ACCEPTORREGISTER_H_


