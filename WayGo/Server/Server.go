package main

import (
	"WayGo/Config"
	"WayGo/Encryption"
	"WayGo/Protocol"
	"WayGo/Utils"
	"crypto/tls"
	"crypto/x509"
	"io/ioutil"
	"log"
	"math/rand"
	"net"
	"time"
)

type Server struct {
	ServerReadKey  []byte
	ServerWriteKey []byte
	Address        string
	verifyMng      *verifyManager
	tlsCfg         *tls.Config
}

func createConfig(certpath, privateKeyPath string) (*tls.Config, error) {
	certBin, err := ioutil.ReadFile(certpath)
	if err != nil {
		return nil, err
	}
	pkeyBin, err := ioutil.ReadFile(privateKeyPath)
	if err != nil {
		return nil, err
	}
	pkey, err := x509.ParsePKCS1PrivateKey(pkeyBin)
	if err != nil {
		return nil, err
	}

	cert := tls.Certificate{Certificate: [][]byte{certBin}, PrivateKey: pkey}

	// 这里只是输出信息
	x509Cert, err := x509.ParseCertificate(certBin)
	log.Println("[ServerName] ", x509Cert.Subject.CommonName)
	_ = x509Cert

	return &tls.Config{
		Certificates: []tls.Certificate{cert},
	}, nil
}

func CreateServer(
	address, certpath, privateKeyPath string,
	serverReadKey, serverWriteKey []byte) (*Server, error) {

	cfg, err := createConfig(certpath, privateKeyPath)
	if err != nil {
		return nil, err
	}
	mng := createVerifyManager()
	// TODO: 载入用户信息
	ret := Server{
		Address:        address,
		ServerReadKey:  serverReadKey,
		ServerWriteKey: serverWriteKey,
		tlsCfg:         cfg,
		verifyMng:      mng,
	}
	return &ret, nil
}

func (s *Server) ListenAndServe() error {

	ln, err := tls.Listen("tcp", s.Address, s.tlsCfg)
	if err != nil {
		return err
	}
	defer ln.Close()
	log.Println("[Listen] ", ln.Addr().String())

	for {
		client, err := ln.Accept()
		if err != nil {
			// TODO: 某些错误需要处理
			log.Println("[Error] ", err)
			continue
		}
		go s.handleConn(client)
	}
}

func (s *Server) dispatchCommand(req *Protocol.RequestMessage, h *Connection, buf []byte) {
	if act, ok := req.GetCommand().(Protocol.CommandAcceptor); ok {
		act.Accept(h, buf)
		return
	}
	panic("cmd not impl act")
}

func (s *Server) handleConn(client net.Conn) {
	defer client.Close()
	addr := client.RemoteAddr().(*net.TCPAddr)
	ip := addr.IP.String()
	defer func(ip string) {
		if e := recover(); e != nil {
			log.Printf("[Error] [%s] %s", ip, e)
		}
	}(ip)

	if s.verifyMng.DarkRoom.Check(ip) {
		log.Println("[DarkRoom] ", ip)
		return
	}

	cnt := Connection{IP: ip, Client: client, RW: s.createEncryptionReadWriter(client)}

	buf := make([]byte, Config.BufferSize)
	// 连接验证
	cmd := s.greeting(&cnt, buf)
	// 连接认证完毕,分发命令
	s.dispatchCommand(cmd, &cnt, buf)
}

func (s *Server) createEncryptionReadWriter(client net.Conn) *Encryption.EncryptionReadWriter {
	return Encryption.CreateEncryptionReadWriter(
		client,
		s.ServerReadKey,
		Encryption.AesMth,
		s.ServerWriteKey,
		Encryption.ChaCha20Mth)
}

func (s *Server) getSayHi(c *Connection, reqBuf *Protocol.RequestMessage, buf []byte) (*string, uint64) {
	// 1.
	r := c.RW
	c.Client.SetReadDeadline(time.Now().Add(Config.TimeoutSecond))
	n, err := r.Read(buf)
	Utils.CheckError(err)
	err = Protocol.Unmarshal(buf[:n], reqBuf)
	if err != nil {
		s.verifyMng.DarkRoom.Add(c.IP)
		panic(err)
	}

	hi := reqBuf.GetHi()
	if hi == nil {
		s.verifyMng.DarkRoom.Add(c.IP)
		panic(Protocol.ErrGreeting)
	}
	uname := hi.GetUserName()
	rn := hi.GetRandomNumber()
	return &uname, rn
}

// 返回命令
func (s *Server) greeting(cnt *Connection, buf []byte) *Protocol.RequestMessage {
	req := Protocol.RequestMessage{}
	uname, rn := s.getSayHi(cnt, &req, buf)

	keys := s.verifyMng.Account.GetKeys(*uname)
	if keys == nil {
		s.verifyMng.DarkRoom.Add(cnt.IP)
		panic("user name err")
	}
	rw := cnt.RW
	// rkey待生成
	rw.WKey = Encryption.GetNewKey(keys.W, rn)
	newRn := rand.Uint64()
	rsp := Protocol.ResponseMessage{}
	rsp.SetJunk()
	rsp.SetRandomNumber(newRn)
	rspByte, err := Protocol.Marshal(&rsp)
	Utils.CheckError(err)

	// 2.
	cnt.Client.SetWriteDeadline(time.Now().Add(Config.TimeoutSecond))
	_, err = rw.Write(rspByte)
	Utils.CheckError(err)

	rw.RKey = Encryption.GetNewKey(keys.R, newRn)
	rw.WKey = Encryption.GetNewKey(keys.W, newRn)
	// 接下来用这套新key处理接下来的通信
	// 3.
	cnt.Client.SetReadDeadline(time.Now().Add(Config.TimeoutSecond))
	n, err := rw.Read(buf)
	Utils.CheckError(err)

	err = Protocol.Unmarshal(buf[:n], &req)
	if err != nil {
		s.verifyMng.DarkRoom.Add(cnt.IP)
		panic(err)
	}
	return &req
}
