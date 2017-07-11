package Protocol

func (m *RequestMessage) SetUdpAssociateCommand(addr string) {
	m.Command = &RequestMessage_UdpAssociate{
		UdpAssociate: addr,
	}
}

type UdpAssociateVisitor interface {
	VisitUdpAssociate(*RequestMessage_UdpAssociate, []byte)
}

func (m *RequestMessage_UdpAssociate) Accept(visitor CommandVisitor, buf []byte) {
	if v, ok := visitor.(UdpAssociateVisitor); ok {
		v.VisitUdpAssociate(m, buf)
		return
	}
	panic("not impl")
}
