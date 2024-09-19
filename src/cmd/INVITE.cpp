#include "../../Header/Header.hpp"

/* "INVITE" 명령어 실행 메서드 */
void ft::Command::INVITE(ft::Client &client)
{
    if (client._message._arguments.size() < 2 || client._message._arguments[0].empty() || client._message._arguments[1].empty()) //"INVITE"명령어와 함께 인자를 입력하지 않았을때 분기
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
        return;
    }
    std::string channelName = ft::tools::ft_toLower(client._message._arguments[1]);      // 대문자로 들어온 서버을 소문자로 변환해준다.
    ft::Client *target = ft::Server::_server->findClient(client._message._arguments[0]); // 초대를 보내려는 유저가 서버에 있는지 "findClient"메서드로 확인한다.
    ft::Channel *channel = ft::Server::_server->findChannel(channelName);                // 초대하려는 채널이 서버에 있는지 "searchChannel"메서드로 확인한다.
    if (target == NULL)                                                                  // 해당 유저가 서버에 없을 때 분기
    {
        client._messageOut += ERR_NOSUCHNICK(client, client._message._arguments[0]);
        return;
    }
    if (channel == NULL) // 초대하려는 채널이 존재하지 않는 경우
    {
        client._messageOut += ERR_NOSUCHCHANNEL(client, channelName);
        return;
    }
    else if (channel) // 초대를 보내려는 유저도 있고 채널도 존재할때 분기
    {
        ft::Channel::memberIterator memberIt = channel->getMember(&client); // 초대를 보내려는 유저가 채널의 멤버인지 아닌지 확인하기 위해 채널의 목록 조회
        if (memberIt == channel->_members.end())                            // 해당 클라이언트가 채널의 멤버가 아닐때 분기
        {
            client._messageOut += ERR_NOTONCHANNEL(client, channel->_name);
        }
        else if (channel->_mode & MODE_INVITE && !(memberIt->second & MODE_CHANNELADMINISTRATOR)) // 채널에 초대를 보내기 위해서 채널 관리자 권한이 필요한 경우 클라이언트가 채널관리자가 아니면 에러메세지 출력하는 분기
        {
            client._messageOut += ERR_CHANOPRIVSNEEDED(client, channel->_name);
        }
        else if (target->inChannel(channel->_name)) // 채널에 이미 초대하려는 해당 클라이언트가 있을때 분기
        {
            client._messageOut += ERR_USERONCHANNEL(client, channel->_name);
        }
        else // 해당 클라이언트를 채널에 초대할 수 있을 때 분기
        {
            target->inviteChannel(channel->_name);                                            //"inviteChannel"메서드를 통해서 해당 유저의 초대받은 채널 컨테이너에 "channel"을 추가
            target->_messageOut += RPL_INVITATION(client, channel->_name, target->_nickName); // 초대받는 사람에게 보내는 메세지
            client._messageOut += RPL_INVITING(client, channel->_name, target->_nickName);    // 초대하는 사람에게 보내는 메세지
            if (target->_mode & MODE_AWAY)                                                    // 만약 초대받는 클라이언트가 부재중 상태라면 초대하려는 유저에게 부재중상태라는 메세지 보내는 분기
            {
                client._messageOut += RPL_AWAY(client, target->_nickName, target->_messageAway);
            }
        }
    }
}
