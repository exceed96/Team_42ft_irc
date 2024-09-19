#include "../../Header/Header.hpp"

/* "OPER"명령어 실행 메서드 */
void ft::Command::OPER(ft::Client &client)
{
    if (client._message._arguments.size() < 2) //"OPER"명령어에 인자가 2개 미만으로 들어올 경우
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
        return;
    }
    else if (client._message._arguments[0].compare(client._nickName)) // 첫번째 인자가 클라이언트의 닉네임과 다를 경우 분기
    {
        client._messageOut += ERR_NICKMISMATCH(client);
        return;
    }
    else if (client._message._arguments[1].compare(ft::Server::_operatorPass)) // 클라이언트가 입력한 서버 관리자 비밀번호가 서버에 저장된 비밀번호랑 다를 경우 분기
    {
        client._messageOut += ERR_PASSWDMISMATCH(client);
        return;
    }
    else if (!(client._mode & MODE_OPERATOR)) // 해당 클라이언트가 서버 관리자가 아닐때 분기
    {
        client._mode |= MODE_OPERATOR;               // 해당 클라이언트를 서버의 관리자로 모드를 지정한다.
        client._messageOut += RPL_YOUREOPER(client); // 서버의 관리자 모드가 성공적으로 실행했다는 출력문구
    }
}