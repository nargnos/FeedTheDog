package location

import (
	"time"
)

var KeyWord = "WHOISLOOKUPSERVER"
var ReplyKeyword = "IMLOOKUPSERVER"

// TODO: 配置点
// FIX: 函数参数不可以是这样（否则无法替换），需要配合配置文件创建对象
func NewLocationServer(addr string, rplPort uint16) LocationServer {
	return &udpLocationServer{
		listenAddr:   addr,
		replyPort:    rplPort,
		keyword:      KeyWord,
		replyKeyword: ReplyKeyword,
	}
}
func NewLocationFinder(bcPort uint16, deadline time.Duration) LocationFinder {
	return &udpLocationFinder{
		keyword:       KeyWord,
		replyKeyword:  ReplyKeyword,
		deadline:      deadline,
		maxFind:       5,
		broadcastPort: bcPort,
	}
}
