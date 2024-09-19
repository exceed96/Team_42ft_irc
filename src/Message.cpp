#include "../Header/Header.hpp"

/* 클라이언트로부터 온 메세지를 서버에서 받는 메서드 */
void ft::Server::receiveMessage(ft::Client &client)
{
    bzero(_buf, 513);                            // 클라이언트로부터 온 메세지를 저장하기 위한 버퍼를 초기화 해준다.
    int nbytes = recv(client._fd, _buf, 512, 0); //"recv"함수를 이용해서 첫번째 소켓 디스크립터로부터 온 메세지의 크기를 저장한다.
    if (nbytes == -1)                            //"recv"함수 실행에 실패할때 분기
    {
        client._messageIn.clear(); // 메세지를 받을때 쓰는 "_messageIn"멤버변수를 비워준다.
        THROW_EXCEPT("Unable to receive data from Client");
    }
    client._messageIn.append(_buf, nbytes);                                                                 //"recv"함수가 성공했다면 "append"함수를 이용해서 메세지를 "_messageIn"에 추가해준다.
    if (client._messageIn.size() >= 2 && !client._messageIn.compare(client._messageIn.size() - 2, 2, CRLF)) //"_messageIn"의 크기가 2이상, 즉 "\r\n"외에 다른 문자가 들어와야하고 마지막 두 문자가 Carriage Return, \n"일때 분기(메세지가 형식에 맞춰서 성공적으로 왔다)
    {
        this->execCommand(client); //"execCommand"메서드를 통해서 해당 메세지를 실행한다.
    }
}

/* 서버에서 클라이언트에게 메세지를 보내는 메서드*/
void ft::Server::sendMessage(ft::Client &client)
{
    int nbytes = send(client._fd, client._messageOut.c_str(), client._messageOut.length(), 0); //"send"함수를 통해서 클라이언트에게 메세지를 보낸다.
    if (nbytes == -1)                                                                          //"send"함수 실행에 실패할때 분기
    {
        THROW_EXCEPT("Unable to send data to Client");
    }
    client._messageOut.erase(0, nbytes); // 송신이 성공적으로 이뤄진 경우에는 "_messageOut"변수를 비워준다.(다음에도 쓸거니까)
}

/* 클라이언트의 이전 메세제를 저장한 멤버변수를 비워주는 메서드 */
void ft::Message::clear(void)
{
    this->_command.clear();
    this->_arguments.clear();
}

/* 클라이언트로부터 받아온 메세지를 파싱하는 메서드 */
void ft::Message::parseMessage(std::string &message)
{
    std::size_t index;        // 문자열의 인덱스로 사용하기 위한 변수 선언
    if (!message.find(COLON)) // 만약 메세지의 첫글자가 ":" 일때 분기
    {
        index = message.find(SPACE);                            // 메세지에서 스페이스를 찾아서 그 인덱스를 "pos"에 초기화
        message.erase(0, index + (index != std::string::npos)); // ": PASS 1234" 와 같이 명령어가 들어왔을때 ':'뒤에 있는 공백부분을 제거한다.
    }
    index = message.find(SPACE);
    this->_command = message.substr(0, index);                                                                 // index에는 메세지에서 명령어에 해당하는 부분 바로 뒤로 초기화 되 어 있으니 "substr"함수를 써서 "_command"변수를 메세지의 명령어로 초기화 가능
    message.erase(0, index + (index != std::string::npos));                                                    // 메세지에서 명령어에 해당하는 부분을 "erase"메서드로 삭제해준다.
    for (index = message.find_first_not_of(SPACE); !message.empty(); index = message.find_first_not_of(SPACE)) // 메세지에서 스페이스가 아닌 첫 번째 문자의 위치를 찾아 "index"에 저장하고, 메세지가 비어 있는지 아닌지를 확인하면서 다음 과정을 반복. 즉 메세지에 있는 파라미터를 뽑는 순회문이다.
    {
        message.erase(0, index);                                  // 메세지에서 "index"이전의 공백문자를 제거한다. 이렇게 해서 공백문자를 지우고 다시 문자가 시작하는 부부능ㄹ 찾는 것이다.
        if (message[0] == COLON || this->_arguments.size() == 14) // 만약 메세지의 첫문자가 ":"이거나 "_arguments"벡터의 크기가 14(맥스)이면 파싱작업을 그만한다.
        {
            index = message.erase(0, message[0] == COLON).npos;
        }
        else
        {
            index = message.find(SPACE); // 그게 아닐 경우 스페이스 문자를 계속 "index"변수에 저장해서 지워 나간다.
        }
        this->_arguments.push_back(message.substr(0, index));   // 파싱된 파라미터를 "_arguments"벡터에 추가해준다.
        message.erase(0, index + (index != std::string::npos)); //"_arguments"에 추가된 파라미터는 제거해준다.
    }
    message.clear(); // 파싱을 마친 메세지는 "clear"메서드를 이용해서 비워준다.
}