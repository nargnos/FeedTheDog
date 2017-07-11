package Protocol

func (m *RequestMessage) SetConnectCommand(addr string) {
	m.Command = &RequestMessage_Connect{
		Connect: addr,
	}
}

type ConnectVisitor interface {
	VisitConnect(*RequestMessage_Connect, []byte)
}

func (m *RequestMessage_Connect) Accept(visitor CommandVisitor, buf []byte) {
	if v, ok := visitor.(ConnectVisitor); ok {
		v.VisitConnect(m, buf)
		return
	}
	panic("not impl")
}
