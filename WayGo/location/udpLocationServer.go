package location

import "net"

import "fmt"

type udpLocationServer struct {
	listenAddr string
	// 这里直接放端口（纯数字）,接收到时组合ip跟这个，就得到发布地址
	replyPort    uint16
	keyword      string
	replyKeyword string
}

func (u *udpLocationServer) Serve() error {
	udpAddr, err := net.ResolveUDPAddr("udp", u.listenAddr)
	if err != nil {
		return err
	}
	udp, err := net.ListenUDP("udp", udpAddr)
	if err != nil {
		return err
	}

	rpl := []byte(fmt.Sprintf("%s%d", u.replyKeyword, u.replyPort))
	buf := make([]byte, MaxUdpMsgSize)
	for {
		n, addr, err := udp.ReadFromUDP(buf)
		if err != nil {
			return err
		}
		if string(buf[:n]) == u.keyword {
			go udp.WriteToUDP(rpl, addr)
		}
	}
}
