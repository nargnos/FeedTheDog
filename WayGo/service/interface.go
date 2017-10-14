package service

import "net"

type Service interface {
	Serve() error
}
type ServiceClient interface {
	net.Conn
}

// TODO: 监听，然后将监听端口注册到lookup
// 注册时提供服务类型，contrator直连过来，可设置不同协议
// 可注册多个类型。可以提供代理服务用来连接到不同网络，貌似需要lookup使用它才能发现其它网络的服务，有点复杂先不考虑这个
// 需要能够提供性能信息，但是怎样高效的实现需要多想一下
