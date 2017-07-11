package Protocol

// CommandVisitor 的声明由各个Cmd确定
type CommandVisitor interface{}

type CommandAcceptor interface {
	Accept(visitor CommandVisitor, buf []byte)
}
