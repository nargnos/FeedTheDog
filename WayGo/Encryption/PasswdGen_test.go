package Encryption

import "testing"

func TestGenKeyIv(t *testing.T) {

	a := []byte("123456")

	newA := GetNewKey(a, 123123)
	newB := GetNewKey(a, 321321)
	_, _ = newA, newB
}
