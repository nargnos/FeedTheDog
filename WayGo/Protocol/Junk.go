package Protocol

import mr "math/rand"

// 用来填充数据包的，只是使发送字节长度不定，不处理
func (r *RequestMessage) SetJunk() {
	r.Junk = getJunk()
}
func getJunk() []byte {
	size := mr.Intn(512) + 1
	ret := make([]byte, size)
	// 好像并不需要做随机填充
	// _, err := io.ReadFull(rand.Reader, ret)
	// if err != nil {
	// 	panic(err)
	// }
	return ret
}

func (r *ResponseMessage) SetJunk() {
	// server回复不设置了，费cpu
	//	r.Junk = getJunk()
}
