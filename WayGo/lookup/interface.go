package lookup

import cmd "WayGo/lookup/command"

// 提供查找服务
type LookupServer interface {
	Serve() error
}

type LookupClient interface {
	Send(req cmd.Request) error
	Recv() (cmd.Response, error)
}

type LookupServiceSubscriber interface {
	Sub(t string) error
	Unsub(t string) error
}
type LookupServiceRegister interface {
	Reg(t string, addr string) error
	// 注册后维持长连接，发消息或断线就注销
	Unreg() error
}

// 常用的分布式架构，这里是ParkServer
// TODO:
// 多lookup，几种方案：
// 推举领导者，这个实现起来好像有点麻烦
// 设置固定服务器查找，这样会引入单点失败
// 服务同时跟多个lookup注册，并跟其中一个订阅。使用者感知到lookup挂掉就换另一个。服务向能找到的lookup注册，需要确保能注册到所有lookup，否则使用时会漏掉，lookup可以维持其它lk的列表，这样似乎可行。实现起来比较简单。用udp广播的方式查找如果不可行就需要换成设置固定服务器方式

// 可用一些成熟的组件来实现，这里先做一层接口隔离
// 似乎不需要从lookup得到服务下线信息，因为使用者跟服务在维持心跳
// 注册时服务只需提供服务类型和端口即可，可从连接获取到ip
