from email import message_from_bytes
from email.mime.text import MIMEText
from email.header import Header
import imaplib
import smtplib

# 简单点，收发的方式简单点


class Imap(object):
    def __init__(self, imaphost, port=imaplib.IMAP4_SSL_PORT):
        self.__imap = imaplib.IMAP4_SSL(imaphost, port)

    def __del__(self):
        self.__imap.logout()

    def Connect(self, username, password):
        try:
            ret, _ = self.__imap.login(username, password)
            return ret == 'OK'
        except Exception as e:
            return False

    def Select(self,  mailbox='INBOX', readonly=False):
        host = self.__imap
        ret, count = host.select()
        return ret == 'OK', count

    def Search(self, searchStr='UNSEEN'):
        host = self.__imap
        ret, ids = host.search(None, searchStr)
        if ret != 'OK' or ids[0] == b'':
            return []
        return ids[0].split()

    def GetMails(self, ids, msgPart='(body[])'):
        # TODO: 找了一圈没发现只取from和text的方法，看了协议文档好像不允许组合,网上一堆二手文档找不到想要的
        host = self.__imap
        if len(ids) <= 0:
            return []
        ret, msg = host.fetch(b','.join(ids), msgPart)
        if ret != 'OK':
            return []
        return [message_from_bytes(item[1]) for item in msg[0::2]]


def GetMailContent(mails, types=("text/plain")):
    # 因为前面fetch没解决，就在这里取需要的邮件内容
    result = dict()
    for item in mails:
        # 需要收集发件人
        fromAddr = item["From"]
        if fromAddr not in result:
            result[fromAddr] = []
        cts = [pl.get_payload(decode=True).decode(pl.get_content_charset()).strip(
            '\r\n') for pl in item.get_payload() if pl.get_content_type() in types]

        result[fromAddr] += [ct for ct in cts if len(ct) > 0]
    return result


class Smtp(object):
    def __init__(self, smtphost, port=smtplib.SMTP_SSL_PORT):
        self.__smtp = smtplib.SMTP_SSL(smtphost, port)

    def __del__(self):
        self.__smtp.close()

    def Connect(self, username, password):
        try:
            ret, _ = self.__smtp.login(username, password)
            return ret == 235
        except Exception as e:
            return False

    def Send(self, text, fromAddr, toAddr, subject="", types='plain', charset='utf-8', **kwArgs):
        try:
            msg = MIMEText(text, types, charset)
            msg.add_header('To', toAddr)
            msg.add_header('Subject', subject)
            for k, v in kwArgs.items():
                msg.add_header(k, v)
            self.__smtp.sendmail(fromAddr, toAddr, msg.as_string())
            return True
        except Exception as e:
            return False, e


if __name__ == '__main__':
    import Config
    cfg = Config.CONFIG
    uname = cfg["account"]["username"]
    passwd = cfg["account"]["password"]
    emailAddr = cfg["account"]["address"]
    s = Smtp(cfg["smtp"]["host"])
    if(s.Connect(uname, passwd)):
        s.Send("test", emailAddr, emailAddr)

    m = Imap(cfg["imap"]["host"])
    if(m.Connect(uname, passwd) and m.Select()):
        print(GetMailContent(m.GetMails(m.Search('UNSEEN SUBJECT "COMMAND"'))))
