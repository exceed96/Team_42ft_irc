#include "../../Header/Header.hpp"

/* "RESTART"명령어 실행 메서드 */
void ft::Command::RESTART(ft::Client &client)
{
    if (client._mode & MODE_OPERATOR) //"RESTART"명령어를 실행한 클라이언트가 서버의 관리자일때 분기
    {
        client._messageOut += RPL_RESTART(client); // 서버를 재시작한다는 메세지 출력
        ft::Server::_restart = true;               //"_restart"플래그를 true로 바꿔준다.
    }
    else // 서버의 관리자가 아닐경우 분기
    {
        client._messageOut += ERR_NOPRIVILEGES(client); // 서버의 관리자가 아니라는 에러메세지 출력
    }
}