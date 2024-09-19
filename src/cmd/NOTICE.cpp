#include "../../Header/Header.hpp"

void ft::Command::NOTICE(ft::Client &client)
{
    if (client._message._arguments.size() >= 2 && !client._message._arguments[2].empty()) // 받아온 인자의 개수가 2개 이상이고, 세번째 인자가 비어있지 않을때 분기
    {
        if (!client._message._arguments.front().empty() && client._message._arguments.front()[0] == '#') // 첫글자가 '#', 즉 채널에 보내는 경우
        {
            std::string channelName = ft::tools::ft_toLower(client._message._arguments.front()); // 대문자로 받아온 채널명도 소문자로 변경해준다.
            ft::Channel *channel = ft::Server::_server->findChannel(channelName);                // "findCHannel"메서드를 통해서 해당 채널이 있는지 체크한다.
            if (channel)                                                                         // 해당 채널이 존재한다면
            {
                ft::Channel::memberIterator memberIt = channel->getMember(&client);                                                                                                                                   // 호출한 클라이언트가 해당 채널의 멤버인지 확인한다.
                if ((channel->_mode & MODE_NOEXTERNAL && memberIt == channel->_members.end()) || (channel->_mode & MODE_MODERATED && memberIt != channel->_members.end() && !(memberIt->second & MODE_CHANNELVOICE))) // 채널이 외부에서 메세지를 받을 수 없는 모드이고, 클라이언트가 채널의 멤버가 아니거나, 채널이 제한권한모드이고 클라이언트가 채널의 멤버이면서 보이스 모드가 아닌경우 아무 작업을 하지 않는다.
                {
                    return;
                }
                for (ft::Channel::memberIterator memberIt = channel->_members.begin(); memberIt != channel->_members.end(); ++memberIt) // 채널의 멤버들을 순회한다.
                {
                    if ((*memberIt).first->_nickName != client._nickName)
                    {
                        (*memberIt).first->_messageOut += NOTICE_MSG(client, channel->_name, client._message._arguments.back());
                    }
                }
            }
        }
        else // 채널이 아닌 클라이언트에게 메세지를 보낼 경우 분기
        {
            ft::Client *receiveClient = ft::Server::_server->findClient(client._message._arguments.front()); //"findClient"메서드로 해당 클라이언트를 찾느낟.
            if (receiveClient)                                                                               // 해당 클라이언트가 존재한다면
            {
                receiveClient->_messageOut += NOTICE_MSG(client, receiveClient->_nickName, client._message._arguments.back()); // 해당 클라이언트에게 메세지를 보낸다.
            }
        }
    }
}