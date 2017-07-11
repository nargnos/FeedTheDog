package Encryption

import (
	"WayGo/Config"
	"crypto/md5"
	"crypto/sha256"
	"crypto/sha512"
	"encoding/binary"
)

// 下面的算法没什么根据，尽量乱一些而已
// 从psw凑出几个需要的字节数组
// 生成的东西保存在本地, 这部分再复杂没用，因为用的是生成的byte
func GenKey(password string) (keyA, keyB []byte) {
	pwdByte := []byte(password)

	saltByte := []byte(Config.Salt)
	md5Gen := md5.New()
	md5Gen.Write(saltByte)
	md5Gen.Write(pwdByte)
	md5Gen.Write(saltByte)
	pwdMd5 := md5Gen.Sum(nil)

	gen := sha512.New()
	gen.Write(pwdMd5)

	// 生成的总长不够，用新的凑

	var sumPwd uint64
	for _, v := range pwdMd5 {
		sumPwd += uint64(v)
	}
	sumPwd = sumPwd ^ Config.MagicNumber

	num := make([]byte, 8)
	binary.BigEndian.PutUint64(num, uint64(sumPwd))
	gen.Write(num)

	for i := 0; i < 100000; i++ {
		binary.LittleEndian.PutUint64(num, uint64(i))

		tmp := gen.Sum(num)
		gen.Write(tmp)
	}
	ret := gen.Sum(nil)
	keyA = ret[:32]
	keyB = ret[32:]
	return
}

// 貌似很容易逆推
// GetNewKey 从商定的随机数生成新的key
func GetNewKey(key []byte, randomNumber uint64) []byte {
	num := make([]byte, 8)
	binary.BigEndian.PutUint64(num, randomNumber)
	return GetNewKeyWithBytes(key, num)
}
func GetNewKeyWithBytes(key []byte, seed []byte) []byte {
	s := sha256.New()
	keyMd5 := md5.Sum(key)
	s.Write(keyMd5[:])
	s.Write(seed)
	s.Write([]byte(Config.Salt))
	loop := 0
	tmpBytes := s.Sum(nil)
	for _, v := range tmpBytes {
		loop += int(v)
	}
	num := make([]byte, 8)
	loop = loop % 10
	for i := 0; i < loop; i++ {
		binary.BigEndian.PutUint64(num, uint64(i))
		tmp := s.Sum(num)
		binary.BigEndian.PutUint64(num, Config.MagicNumber)
		s.Write(tmp)
		s.Write(num)
	}
	return s.Sum(nil)
}
