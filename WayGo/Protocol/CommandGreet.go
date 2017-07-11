package Protocol

// 不需要做命令分发，不用实现对应接口
func (m *RequestMessage) SetGreet(username string, randomNumber uint64) {
	m.Command = &RequestMessage_Hi{
		Hi: &RequestMessage_Greet{
			UserName:     username,
			RandomNumber: randomNumber,
		}}
}
