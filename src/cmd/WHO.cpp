#include "../../Header/Header.hpp"

/* 명령어를 클라이언트와 "WHO"의 대상이 되는 클라이언트가 겹치는 채널이 있는지 확인하는 함수 */
bool isMatch(ft::Client::channelType firstChannels, ft::Client::channelType secondChannels)
{
    ft::Client::channelIterator channelIt1, channelIt2;
    if (firstChannels.size() == 0 || secondChannels.size() == 0) // 두 클라이언트중 채널에 속하지 않은 클라이언트가 있으면 두 클라이언트는 공통으로 속한 채널이 없을때 분기
    {
        return (true);
    }
    for (channelIt1 = firstChannels.begin(), channelIt2 = secondChannels.begin(); channelIt1 != firstChannels.end() && channelIt2 != secondChannels.end(); channelIt1++, channelIt2++)
    {
        if (*channelIt1 == *channelIt2) // 두 클라이언트 중 겹치는 채널이 있을때 분기
        {
            return true;
        }
    }
    return true;
}

/* 해당 채널에 참여했으면 "*", 참여하지 않았으면 아무 표시를 하지 않게 하는 함수 */
void clientChannels(ft::Client &client, ft::Client &member)
{
    std::string channelName = "*";
    if (member._channelsJoined.size() == 0) // 채널에 들어간게 없을때, 즉 겹치는 없는 경우이다.
    {
        client._messageOut += RPL_WHOREPLYNAME(client, channelName, member); // 겹치는게 없을 경우 해당 채널 이름은 '*'으로 표시된다.
        return;
    }
    for (ft::Client::channelIterator channelIt = member._channelsJoined.begin(); channelIt != member._channelsJoined.end(); channelIt++)
    {
        channelName = *channelIt;                                            // 현재 순회중인 채널의 이름을 "channelName"에 초기화한다.
        client._messageOut += RPL_WHOREPLYNAME(client, channelName, member); // 해당 클라이언트의 정보를 출력
    }
}

/* "WHO" 명령어 실행 메서드 */
void ft::Command::WHO(ft::Client &client)
{
    ft::Server::clientIterator clientIt;
    if (client._message._arguments.size() == 1 && !client._message._arguments.front().empty()) //"WHO"명령어에 인자가 1개 이상있고 비어있지 않을 경우 분기
    {
        std::string name = client._message._arguments.front(); // 인자로 들어온 채널 혹은 클라이언트의 이름을 "clientName"변수에 초기화해준다.
        if (name[0] == '#')                                    // 채널의 자세한 정보를 보고싶을때 분기
        {
            name = ft::tools::ft_toLower(client._message._arguments.front()); // 해당 채널 이름이 대문자로 들어올 경우 소문자로 바꿔준다.
            ft::Channel *channel = NULL;                                      // 채널을 임시 저장하는 "channel"객체
            if (client.inChannel(name))                                       // 클라이언트가 해당 채널에 속해있는지 확인하는 분기
            {
                channel = ft::Server::_server->findChannel(name); // 해당 채널에 속해 있다면 해당 채널을 "channel"변수에 초기화해준다.
            }
            bool oper;
            if (channel)
            {
                if (client._mode & MODE_OPERATOR || channel->getMember(&client)->second & MODE_CHANNELADMINISTRATOR) // 해당 클라이언트가 채널의 관리자인지 혹은 서버의 관리자인지 확인한다.
                {
                    oper = true;
                }
                for (ft::Channel::memberIterator memberIt = channel->_members.begin(); memberIt != channel->_members.end(); memberIt++) // 해당 채널에 있는 모든 클라이언트를 순회한다.
                {
                    if (oper) // 해당 유저가 채널이나 서버의 관리자일때 분기
                    {
                        client._messageOut += RPL_WHOREPLY_PTR(client, channel, memberIt->first);
                    }
                    else // 해당 유저가 채널이나 서버의 관리자가 아닐때 분기
                    {
                        if (!(memberIt->first->_mode & MODE_INVISIBLE)) // 온라인 상태가 아닌 유저들의 정보는 보여주지 않는다.
                        {
                            client._messageOut += RPL_WHOREPLY_PTR(client, channel, memberIt->first);
                        }
                    }
                }
            }
            client._messageOut += RPL_ENDOFWHO(client, client._message._arguments.front()); //"WHO"명령어로 보여준 유저 정보 리스트의 마지막 줄을 출력
        }
        else // 채널이 아닌 클라이언트나 서버의 전체 클라이언트의 정보를 보고싶을때 분기
        {
            ft::Server::clientType clients = ft::Server::_server->getClients();
            if (name == "0") // 서버의 전체 클라이언트의 정보를 보고싶을때 분기
            {
                bool oper = false; // 관리자인지 확인한느 플래그
                bool matchingChannels = false;
                if (client._mode & MODE_OPERATOR) // 해당 명령어를 입력한 클라이언트가 관리자일 경우
                {
                    oper = true; //"oper"변수를 true로 초기화해준다.
                }
                for (clientIt = clients.begin(); clientIt != clients.end(); clientIt++) // 모든 클라이언트를 순횐한다.
                {
                    matchingChannels = isMatch(client._channelsJoined, clientIt->second._channelsJoined);                                                                 //"isMatchClient"메서드를 통해서 해당 클라이언트가
                    if (clientIt->second._nickName == client._nickName || (oper && matchingChannels) || (matchingChannels && !(clientIt->second._mode & MODE_INVISIBLE))) // 클라이언트의 닉네임과 순회하는 채널에서 만난 클라이언트의 이름이 같고 관리자이면서 겹치는 채널이 있고 유저의 모드가 "INVISIBLE"모드가 아닐 경우
                    {
                        clientChannels(client, clientIt->second);
                    }
                }
                client._messageOut += RPL_ENDOFWHO(client, "*");
            }
            else // 서버의 전체 클라이언트의 정보가 아닌 특정 클라이언트의 정보를 보고싶을때 분기
            {
                for (clientIt = clients.begin(); clientIt != clients.end(); clientIt++) // 모든 클라이언트를 순회한다.
                {
                    if (clientIt->second._nickName == name) // 현재 순회 중인 클라이언트의 닉네임이 요청한 클라이언트의 이름과 같을 경우
                    {
                        clientChannels(client, clientIt->second); // 해당 클라이언트가 가입한 채널 목록에 대한 정보를 출력한다.
                    }
                }
                client._messageOut += RPL_ENDOFWHO(client, name); //"WHO"명령어의 마지막줄 출력
            }
        }
    }
    else
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
    }
}