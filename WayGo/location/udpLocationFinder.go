package location

import (
	"net"
	"strings"
	"time"
)

type udpLocationFinder struct {
	keyword       string
	replyKeyword  string
	deadline      time.Duration
	maxFind       int
	broadcastPort uint16
}

// 广播方式
func (u *udpLocationFinder) GetLocations() ([]string, error) {

	udp, err := net.ListenUDP("udp", nil)
	if err != nil {
		return nil, err
	}
	_, err = udp.WriteToUDP([]byte(u.keyword), &net.UDPAddr{IP: net.IPv4bcast, Port: int(u.broadcastPort)})
	if err != nil {
		return nil, err
	}
	var ret []string
	buf := make([]byte, MaxUdpMsgSize)
	lenRplKw := len(u.replyKeyword)

	for i := 0; i < u.maxFind; i++ {
		if udp.SetReadDeadline(time.Now().Add(u.deadline)) != nil {
			break
		}
		n, addr, err := udp.ReadFromUDP(buf)
		if err != nil {
			break
		}
		if n <= lenRplKw || string(buf[:lenRplKw]) != u.replyKeyword {
			continue
		}

		msg := string(buf[lenRplKw:n])
		tmpRet := strings.Join([]string{addr.IP.String(), msg}, ":")
		_, err = net.ResolveTCPAddr("tcp", tmpRet)
		if err != nil {
			continue
		}
		ret = append(ret, tmpRet)
	}
	return ret, nil
}
