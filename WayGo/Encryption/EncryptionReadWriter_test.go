package Encryption

import (
	"bytes"
	"fmt"
	"io"
	"testing"
)

func TestEncryptionReadWriter(t *testing.T) {
	keya := []byte("12345678901234567890123456789012")
	keyb := []byte("12345678901234567890123456789000")

	buf := bytes.NewBuffer(nil)
	rw := CreateEncryptionReadWriter(buf, keya, AesMth, keyb, ChaCha20Mth)
	// 试多次才能产生不同方法
	rw.Write([]byte("1"))
	rw.Write([]byte("2"))
	rw.Write([]byte("3"))
	rw.Write([]byte("4"))
	rw.Write([]byte("5"))
	rw.Write([]byte("6"))

	rwNew := CreateEncryptionReadWriter(buf, keyb, ChaCha20Mth, keya, AesMth)

	reads := make([]byte, 1024)
	nread := 0

	for {
		n, err := rwNew.Read(reads[nread:])
		if err != nil {
			if err != io.EOF {
				t.Fatal(err)
			}
			return
		}
		nread += n
		fmt.Println(string(reads[:nread]))
	}
}
