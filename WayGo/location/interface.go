package location

var MaxUdpMsgSize = 128

// 找地址，之后从lookup订阅服务，就不用这个了
// 找不到返回nil
type LocationFinder interface {
	GetLocations() ([]string, error)
}

// 发布地址
type LocationServer interface {
	Serve() error
}

// TODO: 用udp找，或用其它方式
