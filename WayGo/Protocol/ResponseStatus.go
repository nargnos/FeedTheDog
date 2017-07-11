package Protocol

import (
	"net"
)

func statusConvert(err error) ResponseMessage_ErrorStatus {
	if err == nil {
		return ResponseMessage_Succeeded
	}
	switch err.(type) {
	case *net.AddrError:
		return ResponseMessage_AddressTypeNotSupported
		// TODO: 补全
	}
	return ResponseMessage_CommandNotSupported
}

func (r *ResponseMessage) SetStatus(err error) {
	r.Status = statusConvert(err)
}
