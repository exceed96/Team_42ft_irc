#include "../../Header/Header.hpp"

/* "AWAY" 명령어 실행 메서드 */
void ft::Command::AWAY(ft::Client &client)
{
    client._idle = std::time(NULL);            // 클라이언트의 최근 활동 시간을 최신화 해준다.
    if (client._message._arguments.size() < 1) //"AWAY"명령어에 추가적으로 인자를 넣지 않은 경우, 즉 "AWAY"모드를 해제할 경우
    {
        client._messageOut += RPL_UNAWAY(client);
        client._mode &= ~MODE_AWAY;  // 해당 클라이언트가 가진 모드에서 'w'를 제거해준다.
        client._messageAway.clear(); // 부재중시 출력할 메세지 변수인 "_awayMessage"를 비워준다.
        return;
    }
    client._messageOut += RPL_NOWAWAY(client);
    client._mode |= MODE_AWAY;                           // 부재중 모드로 설정해준다.
    client._messageAway = client._message._arguments[0]; // 부재중일때 상대에게 보일 메세지를 인자의 첫번째 원소로 초기화해준다.
}