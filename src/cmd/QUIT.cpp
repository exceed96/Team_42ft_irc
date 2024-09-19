#include "../../Header/Header.hpp"

/* "QUIT"명령어 실행 메서드 */
void ft::Command::QUIT(ft::Client &client)
{
    client._out = true;                      // 클라이언트의 서버 접속 플래그를 "true"로 초기화
    if (!client._message._arguments.empty()) //"QUIT" 명령어에 인자가 있을때 분기
    {
        client._messageAway = client._message._arguments[0]; // 클라이언트가 서버를 끊을때 출력하는 메세지를 인자의 첫번째로 한다.
    }
    else
    {
        client._messageAway.clear(); // 만약 아무 인자가 없다면 서버를 끊을때 출력하는 메세지를 비워준다.
    }
    client._messageOut = RPL_CLOSINGLINK(client); // 클라이언트가 서버를 나갈때 출력하는 메세지를 출력
}