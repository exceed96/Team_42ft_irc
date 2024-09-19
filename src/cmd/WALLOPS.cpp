#include "../../Header/Header.hpp"

/* "WALLOPS"명령어 실행 메서드 */
void ft::Command::WALLOPS(ft::Client &client)
{
    if (client._mode & MODE_OPERATOR) //"NOTICE"명령어를 친 클라이언트가 서버의 관리자일때 분기
    {
        if (client._message._arguments.empty() || client._message._arguments.front().empty()) // 명령어와 함께 메세지를 보낼 인자를 입력하지 않을 때 분기
        {
            client._messageOut += ERR_NEEDMOREPARAMS(client);
        }
        else //"broadcastAddministratorClients"메서드를 이용해서 관리자 권한을 가진 클라이언트들에게 메세지를 뿌린다.
        {
            ft::Server::_server->broadcastAddministratorClients(client);
        }
    }
    else // 서버의 관리자가 아닐때 분기
    {
        client._messageOut += ERR_NOPRIVILEGES(client);
    }
}