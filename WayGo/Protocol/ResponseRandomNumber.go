package Protocol

func (r *ResponseMessage) SetRandomNumber(val uint64) {
	r.Value = &ResponseMessage_RandomNumber{RandomNumber: val}
}
