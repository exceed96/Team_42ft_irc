#include "../../Header/Header.hpp"

/* "PASS"명령어를 실행하는 메서드 */
void ft::Command::PASS(ft::Client &client)
{
    if (client._message._arguments.empty()) //"PASS"명령어에 인자가 없을때 분기 ex)"PASS "
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client); // 해당 클라이언트에게 에러메세지를 출력하도록 메세지 Output을 초기화
    }
    else if (client._realName.compare("*")) // 해당 클라이언트가 아직 정보를 다 등록했을 때 분기
    {
        client._messageOut += ERR_ALREADYREGISTRED(client);
    }
    else if (client._message._arguments.front().compare(ft::Server::_password)) //"PASS"명령어로 받아온 비밀번호를 비교하여서 다를때 분기
    {
        client._messageOut += ERR_PASSWDMISMATCH(client);
    }
    else // 위 3가지 케이스에 걸리지 않을경우 비밀번호가 일치하므로 메서드를 종료시킨다.
    {
        client._passFlag = true;
        return;
    }
    client._out = true; // 비밀번호가 틀릴 경우에는 클라이언트와 서버의 접속을 끊어버린다.
}