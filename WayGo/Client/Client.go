package main

import (
	"WayGo/Config"
	"WayGo/Encryption"
	"WayGo/Protocol"
	"WayGo/Utils"
	"crypto/tls"
	"crypto/x509"
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"math/rand"
	"net"
	"os"
	"strconv"
	"strings"
)

type Client struct {
	ListenPort     uint16
	ServerAddress  string
	ServerName     string
	ServerReadKey  []byte
	ServerWriteKey []byte
	cfg            *tls.Config
	Username       string
	ReadKey        []byte
	WriteKey       []byte
}

func (c *Client) dial() (net.Conn, error) {
	conn, err := tls.Dial("tcp", c.ServerAddress, c.cfg)
	if err != nil {
		if x509err, ok := err.(x509.UnknownAuthorityError); ok {
			cert := x509err.Cert
			certPath := "server.cer"
			fmt.Println("输出了Server的证书:", certPath)
			ioutil.WriteFile(certPath, cert.Raw, 0777)
			os.Exit(-1)
		}
		return nil, err
	}
	return conn, nil
}
func (c *Client) setConfig() {
	// 设置cfg
	if len(c.ServerName) == 0 {
		c.cfg = &tls.Config{
			InsecureSkipVerify: true,
		}
	} else {
		// 检查时检查证书是否受信任并检查ServerName是否相符
		c.cfg = &tls.Config{
			ServerName: c.ServerName,
		}
	}
}
func (c *Client) ListenAndServe() error {
	c.setConfig()

	// 监听本地
	ln, err := net.Listen("tcp", ":"+strconv.Itoa(int(c.ListenPort)))
	if err != nil {
		return err
	}

	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Println(err)
			continue
		}
		go c.handleConn(conn)
	}
}
func checkVer(v byte) {
	if v != 5 {
		panic("socks ver error")
	}
}

func (c *Client) handleConn(conn net.Conn) {
	defer func() {
		if e := recover(); e != nil {
			log.Println("[Error] ", e)
		}
	}()
	defer conn.Close()

	// 做协议转换, 做中间层
	buf := make([]byte, Config.BufferSize)
	// 方式选择
	reads, err := io.ReadAtLeast(conn, buf, 2)
	Utils.CheckError(err)
	checkVer(buf[0])
	nmtd := int(buf[1])
	maxLen := nmtd + 2
	if reads < maxLen {
		_, err = io.ReadFull(conn, buf[reads:maxLen])
		Utils.CheckError(err)
	}

	// 丢弃methods
	// 只支持0,发送回复
	buf[1] = 0
	_, err = conn.Write(buf[:2])
	Utils.CheckError(err)

	// 读ver cmd rsv aytp
	reads = 0
	// 最小长度至少有5
	reads, err = io.ReadAtLeast(conn, buf, 5)
	Utils.CheckError(err)
	checkVer(buf[0])

	cmd := buf[1]
	var addr string
	switch aytp := buf[3]; aytp {
	case 1:
		maxLen = 4 + 4 + 2
		if reads < maxLen {
			_, err = io.ReadFull(conn, buf[reads:maxLen])
			Utils.CheckError(err)
		}
		v4Addr := net.TCPAddr{
			IP:   buf[4 : maxLen-2],
			Port: int(binary.BigEndian.Uint16(buf[maxLen-2 : maxLen])),
		}
		addr = v4Addr.String()
	case 3:
		maxLen = 4 + 1 + int(buf[4]) + 2
		if reads < maxLen {
			_, err = io.ReadFull(conn, buf[reads:maxLen])
			Utils.CheckError(err)
		}
		domain := string(buf[5 : maxLen-2])
		port := binary.BigEndian.Uint16(buf[maxLen-2 : maxLen])
		addr = strings.Join([]string{domain, strconv.Itoa(int(port))}, ":")
	case 4:
		maxLen = 4 + 2 + net.IPv6len
		if reads < maxLen {
			_, err = io.ReadFull(conn, buf[reads:maxLen])
			Utils.CheckError(err)
		}
		v6Addr := net.TCPAddr{IP: buf[4 : maxLen-2], Port: int(binary.BigEndian.Uint16(buf[maxLen-2 : maxLen]))}
		addr = v6Addr.String()
	default:
		panic("aytp err")
	}

	switch cmd {
	case 1:
		c.doConnect(conn, addr, buf)
	case 2:
		c.doBind(conn, addr, buf)
	case 3:
		c.doUdpAssociate(conn, addr, buf)
	default:
		panic("cmd err")
	}
}
func (c *Client) greeting(conn, client net.Conn) (*Encryption.EncryptionReadWriter, error) {
	rw := Encryption.CreateEncryptionReadWriter(conn,
		c.ServerReadKey,
		Encryption.ChaCha20Mth,
		c.ServerWriteKey,
		Encryption.AesMth)
	req := Protocol.RequestMessage{}
	req.SetJunk()
	rn := rand.Uint64()
	req.SetGreet(c.Username, rn)
	reqByte, err := Protocol.Marshal(&req)
	if err != nil {
		return nil, err
	}
	// 1.
	rw.Write(reqByte)
	rw.RKey = Encryption.GetNewKey(c.ReadKey, rn)
	buf := make([]byte, Config.BufferSize)
	// 2.
	n, err := rw.Read(buf)
	if err != nil {
		return nil, err
	}
	rsp := Protocol.ResponseMessage{}
	if Protocol.Unmarshal(buf[:n], &rsp) != nil {
		return nil, err
	}
	stat := rsp.GetStatus()
	if stat != Protocol.ResponseMessage_Succeeded {
		client.Write([]byte{5, byte(stat)})
		return nil, errors.New(strconv.Itoa(int(stat)))
	}
	newRn := rsp.GetRandomNumber()
	// 生成新的key
	rw.RKey = Encryption.GetNewKey(c.ReadKey, newRn)
	rw.WKey = Encryption.GetNewKey(c.WriteKey, newRn)
	return rw, nil
}
func (c *Client) doConnect(conn net.Conn, addr string, buf []byte) {
	serverConn, err := c.dial()
	Utils.CheckError(err)
	defer serverConn.Close()
	rw, err := c.greeting(serverConn, conn)

	Utils.CheckError(err)
	req := Protocol.RequestMessage{}
	req.SetConnectCommand(addr)
	reqByte, err := Protocol.Marshal(&req)
	Utils.CheckError(err)
	// 3.
	rw.Write(reqByte)
	n, err := rw.Read(buf)
	Utils.CheckError(err)
	rsp := Protocol.ResponseMessage{}
	err = Protocol.Unmarshal(buf[:n], &rsp)
	Utils.CheckError(err)
	if rsp.Status != Protocol.ResponseMessage_Succeeded {
		conn.Write([]byte{5, byte(rsp.Status)})
		Utils.CheckError(errors.New(strconv.Itoa(int(rsp.Status))))
	}
	// TODO: 现在先忽略bind
	portbuf := make([]byte, 2)
	binary.BigEndian.PutUint16(portbuf, c.ListenPort)

	_, err = (&net.Buffers{[]byte{5, 0, 0, 1, 1, 0, 0, 127}, portbuf}).WriteTo(conn)
	Utils.CheckError(err)

	// 建立转发关系
	ar := Utils.ConnReader{Conn: serverConn, R: rw}
	aw := Utils.ConnWriter{Conn: conn, W: conn}

	go Utils.Trans(&ar, &aw, buf)

	br := Utils.ConnReader{Conn: conn, R: conn}
	bw := Utils.ConnWriter{Conn: serverConn, W: rw}
	newBuf := make([]byte, Config.BufferSize)
	Utils.Trans(&br, &bw, newBuf)
}

func (c *Client) doBind(conn net.Conn, addr string, buf []byte) {
	panic("Bind not impl")
}
func (c *Client) doUdpAssociate(conn net.Conn, addr string, buf []byte) {
	panic("UdpAssociate not impl")

}
