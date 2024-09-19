#include "../../Header/Header.hpp"

/* "NICK"명령어 실행 메서드 */
void ft::Command::NICK(ft::Client &client)
{
    std::time_t timePass = std::difftime(std::time(NULL), client._nickChange + (client._nickChange == -1)); // 현재시간과 닉네임을 지정했을때의 시간을 비교해서 그 차이를 "time_pass"에 저장한다.
    // std::time(NULL)현재시간
    if (client._mode & MODE_RESTRICTED) // 해당 클라이언트가 제한된 권한을 가지고 있을때 분기
    {
        client._messageOut += ERR_RESTRICTED(client);
    }
    else if (client._message._arguments.empty() || client._message._arguments.front().empty()) //"NICK"명령어에 추가로 인자가 없거나 인자는 있지만 비어있는 상태일때 분기
    {
        client._messageOut += ERR_NONICKNAMEGIVEN(client);
    }
    else if (!ft::tools::ft_nickIsValid(client._message._arguments.front())) // 닉네임 유효성 검증에 통과하지 못할때 분기
    {
        client._messageOut += ERR_ERRONEUSNICKNAME(client);
    }
    else if (ft::Server::_server->alreadyNickName(client._message._arguments.front())) // 다른 클라이언트와 닉네임이 겹치는지 안 겹치는지 확인하는 분기
    {
        client._messageOut += client._realName.compare("*") ? ERR_NICKNAMEINUSE(client) : ERR_NICKCOLLISION(client); // 닉네임을 아직 정하지 않은 상태라면 즉, "*"일때랑 기존 닉네임과 다른 닉네임으로 변경시에 에러 케이스를 2개를 만들었다.
    }
    else if (client._nickChange != -1 && timePass < NICK_DELAY) // 닉네임을 변경한 상태이고 아직 닉네임 변경 제한 시간을 채우지 못했다면
    {
        std::stringstream ss;
        ss << (NICK_DELAY - timePass);                               // 남은 시간만큼을 ss 스트림에 저장하고
        client._messageOut += ERR_UNAVAILRESOURCE(client, ss.str()); // 얼마만큼 남았는지 시간에 대해서 에러 메세지를 클라이언트에게 출력
    }
    else
    {
        client._nickFlag = true;
        client._messageOut += RPL_NICKNAME(client, client._message._arguments.front());
        ft::Server::_server->broadcastJoinedChannels(client, RPL_NICKNAME(client, client._message._arguments.front())); // 클라이언트가 닉네임 변경한 사실을 해당 클라이언트가 "broadcast"메서드를 통해서 속한 채널들에 뿌려준다.
        client._nickName = client._message._arguments.front();                                                          // 해당 클라이언트의 객체에서 닉네임을 새로운 닉네임으로 초기화 해준다.
        client._nickChange = std::time(NULL);                                                                           // 닉네임을 바꾼 시간을 변경한 시간(즉, 현재시간)으로 초기화 해준다.
        return;
    }
    if (!client._realName.compare("*")) // 만약 현재 클라이언트가 아직 닉네임을 정하지 못한상태에서 겹치는 닉네임을 쓰는거면 종료시킨다.
    {
        client._out = true;
    }
}
