package Encryption

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"io"

	"encoding/binary"

	"WayGo/Config"
	"WayGo/Utils"

	"net"

	"github.com/codahale/chacha20"
)

const (
	DoAes = iota
	DoChaCha20
	DoMaxLen
)

// 加密时附带IV，nonce可以明文吗？
// 当前iv:后面是加密:nextMth:NextKey:dataLen:data
// 当前nonce:后面是加密:nextMth:NextKey:dataLen:data
// 固定字节数有8+4+1
type cryptDataUnpackHeader struct {
	Buf []byte
}

func (data *cryptDataUnpackHeader) NextKeySeed() []byte {
	return data.Buf[:8]
}
func (data *cryptDataUnpackHeader) DataLen() []byte {
	return data.Buf[8 : 8+4]
}
func (data *cryptDataUnpackHeader) NextMethod() byte {
	return data.Buf[8+4 : 8+4+1][0]
}

// 固定字节数有8+4+1=13
type cryptDataHeader struct {
	NextKey    [8]byte // 生成下个key的随机数
	DataLen    [4]byte
	NextMethod [1]byte
	Data       []byte
}

const HeaderLen = 13

type EncryptionReadWriter struct {
	RKey []byte // 32
	RMth []byte // 1
	WKey []byte // 32
	WMth []byte // 1
	RW   io.ReadWriter
}

var AesMth []byte = []byte{DoAes}
var ChaCha20Mth []byte = []byte{DoChaCha20}

// 这里都是复制进去的
func CreateEncryptionReadWriter(rw io.ReadWriter, readKey, readMth, writeKey, writeMth []byte) *EncryptionReadWriter {
	if len(readKey) != 32 ||
		len(writeKey) != 32 {
		panic("key size err")
	}
	if len(readMth) != 1 ||
		len(writeMth) != 1 {
		panic("mth size err")
	}
	rkey := make([]byte, 32)
	wkey := make([]byte, 32)
	rmth := make([]byte, 1)
	wmth := make([]byte, 1)
	copy(rkey, readKey)
	copy(wkey, writeKey)
	copy(rmth, readMth)
	copy(wmth, writeMth)

	return &EncryptionReadWriter{
		RW:   rw,
		RKey: rkey,
		RMth: rmth,
		WKey: wkey,
		WMth: wmth,
	}
}
func (e *EncryptionReadWriter) readIv(buf []byte) (*cryptDataUnpackHeader, cipher.Stream, error) {
	r := e.RW
	_, err := io.ReadFull(r, buf[:16])
	if err != nil {
		return nil, nil, err
	}
	block, err := aes.NewCipher(e.RKey)
	if err != nil {
		panic(err)
	}
	// 设置iv
	stream := cipher.NewCTR(block, buf[:16])
	// 读剩下的头
	_, err = io.ReadFull(r, buf[:HeaderLen])
	if err != nil {
		return nil, nil, err
	}
	stream.XORKeyStream(buf[:HeaderLen], buf[:HeaderLen])
	return &cryptDataUnpackHeader{Buf: buf[:HeaderLen]}, stream, nil
}

func (e *EncryptionReadWriter) readNonce(buf []byte) (*cryptDataUnpackHeader, cipher.Stream, error) {
	r := e.RW
	_, err := io.ReadFull(r, buf[:8])
	if err != nil {
		return nil, nil, err
	}
	// 设置nonce
	stream, err := chacha20.New(e.RKey, buf[:8])
	if err != nil {
		return nil, nil, err
	}
	// 读剩下的头
	_, err = io.ReadFull(r, buf[:HeaderLen])
	if err != nil {
		return nil, nil, err
	}
	stream.XORKeyStream(buf[:HeaderLen], buf[:HeaderLen])
	return &cryptDataUnpackHeader{Buf: buf[:HeaderLen]}, stream, nil
}

func (e *EncryptionReadWriter) Read(buf []byte) (int, error) {
	var header *cryptDataUnpackHeader
	var err error
	var stream cipher.Stream
	switch e.RMth[0] {
	case DoAes:
		header, stream, err = e.readIv(buf)
	case DoChaCha20:
		header, stream, err = e.readNonce(buf)
	}
	if err != nil {
		return 0, err
	}
	// 设置下次的加密方式
	e.RMth[0] = header.NextMethod()
	// 设置下一次key
	e.RKey = GetNewKeyWithBytes(e.RKey, header.NextKeySeed())
	dataLen := binary.BigEndian.Uint32(header.DataLen())
	if dataLen > Config.BufferSize {
		return 0, Utils.ErrSizeTooLarge
	}
	_, err = io.ReadFull(e.RW, buf[:dataLen])
	if err != nil {
		return 0, err
	}
	stream.XORKeyStream(buf[:dataLen], buf[:dataLen])
	return int(dataLen), nil
}
func (e *EncryptionReadWriter) createIv() ([]byte, cipher.Stream, error) {
	ret := make([]byte, 16)
	_, err := io.ReadFull(rand.Reader, ret)
	if err != nil {
		return nil, nil, err
	}
	block, err := aes.NewCipher(e.WKey)
	if err != nil {
		return nil, nil, err
	}
	stream := cipher.NewCTR(block, ret)
	return ret, stream, nil
}
func (e *EncryptionReadWriter) createNonce() ([]byte, cipher.Stream, error) {
	ret := make([]byte, 8)
	_, err := io.ReadFull(rand.Reader, ret)
	if err != nil {
		return nil, nil, err
	}
	stream, err := chacha20.New(e.WKey, ret)
	if err != nil {
		return nil, nil, err
	}

	return ret, stream, nil
}
func (e *EncryptionReadWriter) Write(buf []byte) (int, error) {
	if len(buf) > Config.BufferSize {
		panic(Utils.ErrSizeTooLarge)
	}
	w := e.RW
	// 根据本次的mth组织头
	// 生成下次的mth和key放进去
	var ivNonce []byte
	var stream cipher.Stream
	var err error
	switch e.WMth[0] {
	case DoAes:
		ivNonce, stream, err = e.createIv()
	case DoChaCha20:
		ivNonce, stream, err = e.createNonce()
	}
	if err != nil {
		return 0, err
	}
	// 生成下一个key
	nextKeySeed := make([]byte, 8)
	_, err = io.ReadFull(rand.Reader, nextKeySeed)
	e.WKey = GetNewKeyWithBytes(e.WKey, nextKeySeed)
	if err != nil {
		return 0, err
	}
	// 数据长度
	dataLen := make([]byte, 4)

	binary.BigEndian.PutUint32(dataLen, uint32(len(buf)))
	// 设置下次mth
	_, err = io.ReadFull(rand.Reader, e.WMth)
	if err != nil {
		return 0, err
	}
	e.WMth[0] %= DoMaxLen
	nextMth := []byte{e.WMth[0]}
	stream.XORKeyStream(nextKeySeed, nextKeySeed)
	stream.XORKeyStream(dataLen, dataLen)
	stream.XORKeyStream(nextMth, nextMth)
	stream.XORKeyStream(buf, buf)
	writeBuf := net.Buffers{ivNonce, nextKeySeed, dataLen, nextMth, buf}
	_, err = writeBuf.WriteTo(w)
	if err != nil {
		return 0, err
	}
	return len(buf), nil
}
