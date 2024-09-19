#include "../../Header/Header.hpp"

/* "WHOIS"명령어 실행 메서드 */
void ft::Command::WHOIS(ft::Client &client)
{
    if (client._message._arguments.size() < 1) //"WHOIS"명령어와 함꼐 인자가 오지 않을 시
    {
        client._messageOut += ERR_NONICKNAMEGIVEN(client);
        return;
    }
    std::string nickName = client._message._arguments[0];           // 인자로 받아온 닉네임을 "nickName"변수에 초기화
    ft::Client *target = ft::Server::_server->findClient(nickName); // 해당 닉네임을 가진 클라이언트의 정보를 받아오기 위한 "target"객체 선언 후 "findClient"메서드를 통해서 해당 닉네임을 가진 클라이언트로 초기화
    if (target == NULL)                                             // 해당 닉네임을 가진 클라이언트를 찾지 못할때 분기
    {
        client._messageOut += ERR_NOSUCHNICK(client, nickName);
        return;
    }
    client._messageOut += RPL_WHOISUSER(client, target); // 해당 닉네임을 가진 클라이언트를 찾았을때
    client._messageOut += RPL_WHOISSERVER(client, target);
    client._messageOut += RPL_WHOISCHANNELS(client, target); // 해당 닉네임을 가진 클라이언트가 속한 채널들을 알려준다.
    if (target->_mode & MODE_AWAY)                           // 해당 닉네임을 가진 클라이언트가 자리 비움 상태일때 분기
    {
        client._messageOut += RPL_AWAY(client, target->_nickName, target->_messageAway); // away메세지도 같이 출력한다.
    }
    if (target->_mode & MODE_OPERATOR) // 해당 닉네임을 가진 클라이언트가 채널의 관리자일때 분기
    {
        client._messageOut += RPL_WHOISOPERATOR(client, target); // 채널의 관리자라는 메세지도 출력한다.
    }
    client._messageOut += RPL_WHOISIDLE(client, target); // 서버에 접속하고 경과한 시간과, 등록한 시간을 출력한다.
    client._messageOut += RPL_ENDOFWHOIS(client, target);
}