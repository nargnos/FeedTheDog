package Protocol

import (
	"errors"

	"github.com/golang/protobuf/proto"
)

func Marshal(pb proto.Message) ([]byte, error) {
	return proto.Marshal(pb)
}
func Unmarshal(buf []byte, pb proto.Message) error {
	return proto.Unmarshal(buf, pb)
}

var ErrGreeting = errors.New("greeting")
