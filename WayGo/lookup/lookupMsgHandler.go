package lookup

import (
	utils "WayGo/common"
	"WayGo/lookup/command"
	"WayGo/protocol"
)

func (l *lookupServer) msgHandler(conn *protocol.MsgReadWriteCloser) {
	defer utils.WarningPanic()

	req := &command.Request{}
	err := protocol.ReadProtoMessage(conn, req)
	utils.PanicIfErr(err)

	l.reqHandler(conn, req)
}
