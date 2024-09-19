#include "../../Header/Header.hpp"

/* "DOWN"명령어 실행 메서드 */
void ft::Command::DOWN(ft::Client &client)
{
    if (client._mode & MODE_OPERATOR) // 서버의 관리자일때만 "DOWN"명령어를 실행하게 하는 분기
    {
        client._messageOut += RPL_DIE(client);
        ft::Server::_serverDown = true; //"_serverDown"플래그를 "true"로 해서 서버를 강제종료
    }
    else // 서버의 관리자가 아닐때는 에러메세지 출력 분기
    {
        client._messageOut += ERR_NOPRIVILEGES(client);
    }
}