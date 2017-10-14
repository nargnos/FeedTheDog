// Code generated by protoc-gen-go. DO NOT EDIT.
// source: lookup/command/command.proto

/*
Package command is a generated protocol buffer package.

It is generated from these files:
	lookup/command/command.proto

It has these top-level messages:
	Request
	Response
*/
package command

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion2 // please upgrade the proto package

type Request struct {
	// Types that are valid to be assigned to Message:
	//	*Request_Heartbeat
	//	*Request_Sub
	//	*Request_Unsub
	//	*Request_Reg
	//	*Request_Unreg
	//	*Request_Update
	Message isRequest_Message `protobuf_oneof:"Message"`
}

func (m *Request) Reset()                    { *m = Request{} }
func (m *Request) String() string            { return proto.CompactTextString(m) }
func (*Request) ProtoMessage()               {}
func (*Request) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0} }

type isRequest_Message interface {
	isRequest_Message()
}

type Request_Heartbeat struct {
	Heartbeat string `protobuf:"bytes,1,opt,name=Heartbeat,oneof"`
}
type Request_Sub struct {
	Sub *Request_SubMessage `protobuf:"bytes,2,opt,name=Sub,oneof"`
}
type Request_Unsub struct {
	Unsub *Request_UnsubMessage `protobuf:"bytes,4,opt,name=Unsub,oneof"`
}
type Request_Reg struct {
	Reg *Request_RegServiceMessage `protobuf:"bytes,3,opt,name=Reg,oneof"`
}
type Request_Unreg struct {
	Unreg *Request_UnregServiceMessage `protobuf:"bytes,5,opt,name=Unreg,oneof"`
}
type Request_Update struct {
	Update *Request_UpdateMessage `protobuf:"bytes,128,opt,name=Update,oneof"`
}

func (*Request_Heartbeat) isRequest_Message() {}
func (*Request_Sub) isRequest_Message()       {}
func (*Request_Unsub) isRequest_Message()     {}
func (*Request_Reg) isRequest_Message()       {}
func (*Request_Unreg) isRequest_Message()     {}
func (*Request_Update) isRequest_Message()    {}

func (m *Request) GetMessage() isRequest_Message {
	if m != nil {
		return m.Message
	}
	return nil
}

func (m *Request) GetHeartbeat() string {
	if x, ok := m.GetMessage().(*Request_Heartbeat); ok {
		return x.Heartbeat
	}
	return ""
}

func (m *Request) GetSub() *Request_SubMessage {
	if x, ok := m.GetMessage().(*Request_Sub); ok {
		return x.Sub
	}
	return nil
}

func (m *Request) GetUnsub() *Request_UnsubMessage {
	if x, ok := m.GetMessage().(*Request_Unsub); ok {
		return x.Unsub
	}
	return nil
}

func (m *Request) GetReg() *Request_RegServiceMessage {
	if x, ok := m.GetMessage().(*Request_Reg); ok {
		return x.Reg
	}
	return nil
}

func (m *Request) GetUnreg() *Request_UnregServiceMessage {
	if x, ok := m.GetMessage().(*Request_Unreg); ok {
		return x.Unreg
	}
	return nil
}

func (m *Request) GetUpdate() *Request_UpdateMessage {
	if x, ok := m.GetMessage().(*Request_Update); ok {
		return x.Update
	}
	return nil
}

// XXX_OneofFuncs is for the internal use of the proto package.
func (*Request) XXX_OneofFuncs() (func(msg proto.Message, b *proto.Buffer) error, func(msg proto.Message, tag, wire int, b *proto.Buffer) (bool, error), func(msg proto.Message) (n int), []interface{}) {
	return _Request_OneofMarshaler, _Request_OneofUnmarshaler, _Request_OneofSizer, []interface{}{
		(*Request_Heartbeat)(nil),
		(*Request_Sub)(nil),
		(*Request_Unsub)(nil),
		(*Request_Reg)(nil),
		(*Request_Unreg)(nil),
		(*Request_Update)(nil),
	}
}

func _Request_OneofMarshaler(msg proto.Message, b *proto.Buffer) error {
	m := msg.(*Request)
	// Message
	switch x := m.Message.(type) {
	case *Request_Heartbeat:
		b.EncodeVarint(1<<3 | proto.WireBytes)
		b.EncodeStringBytes(x.Heartbeat)
	case *Request_Sub:
		b.EncodeVarint(2<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Sub); err != nil {
			return err
		}
	case *Request_Unsub:
		b.EncodeVarint(4<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Unsub); err != nil {
			return err
		}
	case *Request_Reg:
		b.EncodeVarint(3<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Reg); err != nil {
			return err
		}
	case *Request_Unreg:
		b.EncodeVarint(5<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Unreg); err != nil {
			return err
		}
	case *Request_Update:
		b.EncodeVarint(128<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Update); err != nil {
			return err
		}
	case nil:
	default:
		return fmt.Errorf("Request.Message has unexpected type %T", x)
	}
	return nil
}

func _Request_OneofUnmarshaler(msg proto.Message, tag, wire int, b *proto.Buffer) (bool, error) {
	m := msg.(*Request)
	switch tag {
	case 1: // Message.Heartbeat
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		x, err := b.DecodeStringBytes()
		m.Message = &Request_Heartbeat{x}
		return true, err
	case 2: // Message.Sub
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(Request_SubMessage)
		err := b.DecodeMessage(msg)
		m.Message = &Request_Sub{msg}
		return true, err
	case 4: // Message.Unsub
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(Request_UnsubMessage)
		err := b.DecodeMessage(msg)
		m.Message = &Request_Unsub{msg}
		return true, err
	case 3: // Message.Reg
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(Request_RegServiceMessage)
		err := b.DecodeMessage(msg)
		m.Message = &Request_Reg{msg}
		return true, err
	case 5: // Message.Unreg
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(Request_UnregServiceMessage)
		err := b.DecodeMessage(msg)
		m.Message = &Request_Unreg{msg}
		return true, err
	case 128: // Message.Update
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(Request_UpdateMessage)
		err := b.DecodeMessage(msg)
		m.Message = &Request_Update{msg}
		return true, err
	default:
		return false, nil
	}
}

func _Request_OneofSizer(msg proto.Message) (n int) {
	m := msg.(*Request)
	// Message
	switch x := m.Message.(type) {
	case *Request_Heartbeat:
		n += proto.SizeVarint(1<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(len(x.Heartbeat)))
		n += len(x.Heartbeat)
	case *Request_Sub:
		s := proto.Size(x.Sub)
		n += proto.SizeVarint(2<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case *Request_Unsub:
		s := proto.Size(x.Unsub)
		n += proto.SizeVarint(4<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case *Request_Reg:
		s := proto.Size(x.Reg)
		n += proto.SizeVarint(3<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case *Request_Unreg:
		s := proto.Size(x.Unreg)
		n += proto.SizeVarint(5<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case *Request_Update:
		s := proto.Size(x.Update)
		n += proto.SizeVarint(128<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case nil:
	default:
		panic(fmt.Sprintf("proto: unexpected type %T in oneof", x))
	}
	return n
}

type Request_RegServiceMessage struct {
}

func (m *Request_RegServiceMessage) Reset()                    { *m = Request_RegServiceMessage{} }
func (m *Request_RegServiceMessage) String() string            { return proto.CompactTextString(m) }
func (*Request_RegServiceMessage) ProtoMessage()               {}
func (*Request_RegServiceMessage) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0, 0} }

type Request_UnregServiceMessage struct {
}

func (m *Request_UnregServiceMessage) Reset()                    { *m = Request_UnregServiceMessage{} }
func (m *Request_UnregServiceMessage) String() string            { return proto.CompactTextString(m) }
func (*Request_UnregServiceMessage) ProtoMessage()               {}
func (*Request_UnregServiceMessage) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0, 1} }

// 可订阅服务列表，刚订阅时会收到完整列表，后面会收到增删信息
type Request_SubMessage struct {
}

func (m *Request_SubMessage) Reset()                    { *m = Request_SubMessage{} }
func (m *Request_SubMessage) String() string            { return proto.CompactTextString(m) }
func (*Request_SubMessage) ProtoMessage()               {}
func (*Request_SubMessage) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0, 2} }

type Request_UnsubMessage struct {
}

func (m *Request_UnsubMessage) Reset()                    { *m = Request_UnsubMessage{} }
func (m *Request_UnsubMessage) String() string            { return proto.CompactTextString(m) }
func (*Request_UnsubMessage) ProtoMessage()               {}
func (*Request_UnsubMessage) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0, 3} }

type Request_UpdateMessage struct {
}

func (m *Request_UpdateMessage) Reset()                    { *m = Request_UpdateMessage{} }
func (m *Request_UpdateMessage) String() string            { return proto.CompactTextString(m) }
func (*Request_UpdateMessage) ProtoMessage()               {}
func (*Request_UpdateMessage) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0, 4} }

type Response struct {
}

func (m *Response) Reset()                    { *m = Response{} }
func (m *Response) String() string            { return proto.CompactTextString(m) }
func (*Response) ProtoMessage()               {}
func (*Response) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{1} }

func init() {
	proto.RegisterType((*Request)(nil), "Request")
	proto.RegisterType((*Request_RegServiceMessage)(nil), "Request.RegServiceMessage")
	proto.RegisterType((*Request_UnregServiceMessage)(nil), "Request.UnregServiceMessage")
	proto.RegisterType((*Request_SubMessage)(nil), "Request.SubMessage")
	proto.RegisterType((*Request_UnsubMessage)(nil), "Request.UnsubMessage")
	proto.RegisterType((*Request_UpdateMessage)(nil), "Request.UpdateMessage")
	proto.RegisterType((*Response)(nil), "Response")
}

func init() { proto.RegisterFile("lookup/command/command.proto", fileDescriptor0) }

var fileDescriptor0 = []byte{
	// 258 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x6c, 0x91, 0xbb, 0x4e, 0xc3, 0x30,
	0x14, 0x86, 0x1b, 0x42, 0x5b, 0x72, 0x28, 0x20, 0x1c, 0x15, 0x59, 0x56, 0x85, 0xaa, 0x2e, 0x74,
	0x21, 0x88, 0xcb, 0x13, 0x30, 0x65, 0x61, 0x71, 0xd4, 0x07, 0xb0, 0xdb, 0x23, 0x0b, 0x41, 0xe3,
	0xe0, 0x0b, 0x73, 0x1f, 0x1d, 0xd5, 0x0d, 0x75, 0x10, 0x99, 0x6c, 0xfd, 0xfa, 0xbe, 0xff, 0xf8,
	0x02, 0xb3, 0x4f, 0xad, 0x3f, 0x7c, 0xf3, 0xb0, 0xd6, 0xdb, 0xad, 0xa8, 0x37, 0xbf, 0x6b, 0xd1,
	0x18, 0xed, 0xf4, 0x62, 0x97, 0xc2, 0x98, 0xe3, 0x97, 0x47, 0xeb, 0xc8, 0x2d, 0x64, 0x25, 0x0a,
	0xe3, 0x24, 0x0a, 0x47, 0x93, 0x79, 0xb2, 0xcc, 0xca, 0x01, 0x8f, 0x11, 0xb9, 0x83, 0xb4, 0xf2,
	0x92, 0x9e, 0xcc, 0x93, 0xe5, 0xf9, 0x53, 0x5e, 0xb4, 0x5a, 0x51, 0x79, 0xf9, 0x86, 0xd6, 0x0a,
	0x85, 0xe5, 0x80, 0xef, 0x09, 0x72, 0x0f, 0xc3, 0x55, 0x6d, 0xbd, 0xa4, 0xa7, 0x01, 0x9d, 0x1e,
	0xd1, 0x90, 0x46, 0xf8, 0x40, 0x91, 0x02, 0x52, 0x8e, 0x8a, 0xa6, 0x01, 0x66, 0x47, 0x98, 0xa3,
	0xaa, 0xd0, 0x7c, 0xbf, 0xaf, 0xb1, 0x53, 0xcf, 0x51, 0x91, 0x97, 0x7d, 0xbd, 0x41, 0x45, 0x87,
	0xc1, 0x98, 0x75, 0xea, 0x4d, 0x8f, 0x73, 0x80, 0xc9, 0x23, 0x8c, 0x56, 0xcd, 0x46, 0x38, 0xa4,
	0xbb, 0x24, 0x78, 0x37, 0xd1, 0x0b, 0x79, 0x34, 0x5a, 0x90, 0xe5, 0x70, 0xfd, 0xef, 0x10, 0x6c,
	0x0a, 0x79, 0xcf, 0x1c, 0x36, 0x01, 0x88, 0x0f, 0xc1, 0x2e, 0x61, 0xd2, 0xbd, 0x2b, 0xbb, 0x82,
	0x8b, 0x3f, 0x43, 0x5e, 0x33, 0x18, 0xb7, 0xdb, 0x05, 0xc0, 0x19, 0x47, 0xdb, 0xe8, 0xda, 0xa2,
	0x1c, 0x85, 0x5f, 0x79, 0xfe, 0x09, 0x00, 0x00, 0xff, 0xff, 0x8a, 0x7c, 0x0f, 0x5c, 0xb5, 0x01,
	0x00, 0x00,
}
