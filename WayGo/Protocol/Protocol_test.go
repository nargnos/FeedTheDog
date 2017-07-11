package Protocol

import (
	"fmt"
	"testing"
)

func TestSetConnectCommand(t *testing.T) {
	t.Parallel()
	req := RequestMessage{}
	req.SetConnectCommand(":1234")
	switch obj := req.GetCommand().(type) {
	case *RequestMessage_Connect:
		if obj.Connect != ":1234" {
			t.Fail()
		}
	default:
		t.Error("类型错误")
	}
}
func TestSetBindCommand(t *testing.T) {
	t.Parallel()
	req := RequestMessage{}
	req.SetBindCommand(":1234")
	switch obj := req.GetCommand().(type) {
	case *RequestMessage_Bind:
		if obj.Bind != ":1234" {
			t.Fail()
		}
	default:
		t.Error("类型错误")
	}
}
func TestSetUdpAssociateCommand(t *testing.T) {
	t.Parallel()
	req := RequestMessage{}
	req.SetUdpAssociateCommand(":1234")
	switch obj := req.GetCommand().(type) {
	case *RequestMessage_UdpAssociate:
		if obj.UdpAssociate != ":1234" {
			t.Fail()
		}
	default:
		t.Error("类型错误")
	}
}

func GetSampleMsg() *RequestMessage {
	req := RequestMessage{}
	req.SetMagicNumber(4321)
	req.SetConnectCommand("hello world")

	return &req
}

type CommandVisitorTest struct {
	Msg string
	T   *testing.T
}

func (t *CommandVisitorTest) VisitConnect(v *RequestMessage_Connect) {
	fmt.Println(t.Msg, v.Connect)
}
func (t *CommandVisitorTest) VisitBind(*RequestMessage_Bind) {
	t.T.Fail()
}
func (t *CommandVisitorTest) VisitVisitUdpAssociate(*RequestMessage_UdpAssociate) {
	t.T.Fail()
}

func TestConv(t *testing.T) {
	req := GetSampleMsg()
	v := CommandVisitorTest{Msg: "hello world", T: t}
	cmd := req.GetCommand()
	if act, ok := cmd.(CommandAcceptor); ok {
		act.Accept(&v)
	} else {
		t.Fatal("cant conv")
	}

}
