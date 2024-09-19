#include "../../Header/Header.hpp"

static void partChannel(ft::Client &client, std::string &channelName)
{
    ft::Channel *channel = ft::Server::_server->findChannel(channelName);
    if (channel == NULL)
    {
        client._messageOut += ERR_NOSUCHCHANNEL(client, channelName);
    }
    else
    {
        ft::Channel::memberIterator memberIt = channel->_members.find(&client);
        if (memberIt == channel->_members.end())
        {
            client._messageOut += ERR_NOTONCHANNEL(client, channel->_name);
        }
        else
        {
            channel->broadcast(client._message._arguments.size() == 1 ? RPL_PART(client, channel->_name) : RPL_PARTMSG(client, channel->_name, client._message._arguments[1]));
            channel->_members.erase(memberIt);
            client.partChannel(channel->_name);
            if (channel->_members.empty())
            {
                ft::Server::_server->removeChannel(*channel);
            }
        }
    }
}

/* "PART"명령어 실행 메서드 */
void ft::Command::PART(ft::Client &client)
{
    if (client._message._arguments.empty() || client._message._arguments.front().empty()) //"PART"명령어에 인자를 추가 하지 않거나 인자가 비워있는 상태일때 분기
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
    }
    else
    {
        std::vector<std::string> channelNames = ft::tools::ft_splitMessage(client._message._arguments[0], ',');                 // 인자가 들어있다면 해당 인자가 여러개 들어올 경우 "ft_splitMessage"함수를 이용해서 ','으로 분할하여 벡터에 초기화 한다.
        std::transform(channelNames.begin(), channelNames.end(), channelNames.begin(), ft::tools::ft_toLower);                  // 해당 인자가 대문자로 들어올 경우 소문자로 변환해준다.
        for (std::vector<std::string>::iterator channelIt = channelNames.begin(); channelIt != channelNames.end(); ++channelIt) //"PART"명령어를 적용할 채널 벡터를 순회한다.
        {
            partChannel(client, *channelIt); //"partChannel"함수를 통해서 해당 채널을 나간다.
        }
    }
}