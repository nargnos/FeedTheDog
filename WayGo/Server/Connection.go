package main

import "net"

import "WayGo/Encryption"

type Connection struct {
	// 懒得实现所有的net.conn接口，所以放一个
	Client net.Conn
	Remote net.Conn
	RW     *Encryption.EncryptionReadWriter
	IP     string
}
