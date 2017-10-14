package lookup

import (
	"WayGo/protocol"
	"net"
)

type lookupServer struct {
	msgServer *protocol.MsgServer
}

func (l *lookupServer) Serve() error {
	return l.msgServer.ServeFunc(l.msgHandler)
}

func NewLookupServer(l net.TCPListener) LookupServer {
	return &lookupServer{msgServer: protocol.NewMsgServer(l)}
}
