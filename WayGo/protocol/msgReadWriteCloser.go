package protocol

import (
	"encoding/binary"
	"errors"
	"io"
	"net"
)

// TODO: 未测试
// [len:4][data:len]
var LenHeaderSize = 4

var MaxBufferSize = 1 << 18

var ErrMsgTooBig = errors.New("Message Too Big")

type MsgReadWriteCloser struct {
	*net.TCPConn
}

func newMsgReadWriteCloser(conn *net.TCPConn) *MsgReadWriteCloser {
	return &MsgReadWriteCloser{TCPConn: conn}
}

func checkMsgSize(len uint32) bool {
	return len <= uint32(MaxBufferSize)
}

func (m *MsgReadWriteCloser) ReadMsg() ([]byte, error) {
	buf := make([]byte, MaxBufferSize)
	lenBuf := buf[:LenHeaderSize]
	_, err := io.ReadFull(m, lenBuf)
	if err != nil {
		return nil, err
	}
	len := binary.BigEndian.Uint32(lenBuf)

	if !checkMsgSize(len) {
		return nil, ErrMsgTooBig
	}
	dataBuf := buf[:len]
	_, err = io.ReadFull(m, dataBuf)
	if err != nil {
		return nil, err
	}
	return dataBuf, nil
}

func (m *MsgReadWriteCloser) WriteMsg(buf []byte) error {
	lenBuf := make([]byte, LenHeaderSize)
	binary.BigEndian.PutUint32(lenBuf, uint32(len(buf)))

	bufs := &net.Buffers{lenBuf, buf}
	_, err := bufs.WriteTo(m)
	return err
}
