package protocol

import "github.com/golang/protobuf/proto"

func WriteProtoMessage(conn *MsgReadWriteCloser, msg proto.Message) error {
	buf, err := proto.Marshal(msg)
	if err != nil {
		return err
	}
	return conn.WriteMsg(buf)
}
func ReadProtoMessage(conn *MsgReadWriteCloser, msg proto.Message) error {
	msgBuf, err := conn.ReadMsg()
	if err != nil {
		return err
	}
	return proto.Unmarshal(msgBuf, msg)
}
