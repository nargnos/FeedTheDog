package main

import (
	"WayGo/Config"
	"WayGo/Protocol"
	"WayGo/Utils"
	"net"
	"time"
)

func (h *Connection) VisitConnect(v *Protocol.RequestMessage_Connect, buf []byte) {
	addr := v.Connect
	remoteConn, err := net.DialTimeout("tcp", addr, Config.TimeoutSecond)
	rsp := Protocol.ResponseMessage{}
	rsp.SetJunk()
	if err != nil {
		rsp.SetStatus(err)
		// 回复并关闭, 会在上层函数关闭
		rspByte, err := Protocol.Marshal(&rsp)
		if err != nil {
			return
		}
		h.RW.Write(rspByte)
		return
	}
	defer remoteConn.Close()
	rspByte, err := Protocol.Marshal(&rsp)
	if err != nil {
		return
	}
	h.Client.SetReadDeadline(time.Now().Add(Config.TimeoutSecond))
	_, err = h.RW.Write(rspByte)
	Utils.CheckError(err)

	// 清空超时设置
	h.Client.SetReadDeadline(time.Time{})
	h.Client.SetWriteDeadline(time.Time{})

	// 建立转发关系
	ar := Utils.ConnReader{Conn: remoteConn, R: remoteConn}
	aw := Utils.ConnWriter{Conn: h.Client, W: h.RW}
	go Utils.Trans(&ar, &aw, buf)

	br := Utils.ConnReader{Conn: h.Client, R: h.RW}
	bw := Utils.ConnWriter{Conn: remoteConn, W: remoteConn}
	newBuf := make([]byte, Config.BufferSize)
	Utils.Trans(&br, &bw, newBuf)
}
