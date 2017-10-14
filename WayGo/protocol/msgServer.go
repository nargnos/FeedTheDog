package protocol

import (
	"net"
)

type MsgHandler func(*MsgReadWriteCloser)

type MsgServer struct {
	listener net.TCPListener
}

func (m *MsgServer) ServeFunc(h MsgHandler) error {
	for {
		conn, err := m.listener.AcceptTCP()
		if err != nil {
			return err
		}
		go h(newMsgReadWriteCloser(conn))
	}
}

func NewMsgServer(l net.TCPListener) *MsgServer {
	return &MsgServer{listener: l}
}
