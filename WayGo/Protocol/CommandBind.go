package Protocol

func (m *RequestMessage) SetBindCommand(addr string) {
	m.Command = &RequestMessage_Bind{
		Bind: addr,
	}
}

type BindVisitor interface {
	VisitBind(*RequestMessage_Bind, []byte)
}

func (m *RequestMessage_Bind) Accept(visitor CommandVisitor, buf []byte) {
	if v, ok := visitor.(BindVisitor); ok {
		v.VisitBind(m, buf)
		return
	}
	panic("not impl")
}
