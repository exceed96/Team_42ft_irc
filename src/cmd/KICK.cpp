#include "../../Header/Header.hpp"

/* 채널에서 클라이언트를 강퇴하는 메서드 */
void ft::Command::kickMember(ft::Client &client, std ::string &nickName, ft::Channel *channel, std::string &kickComment)
{
    ft::Channel::memberIterator memberIt = channel->getMember(&client); //"getMember"메서드를 통해서 강퇴를 하려는 클라이언트의 반복자를 "memberIt"에 초기화
    if (memberIt == channel->_members.end())                            // 만약 강퇴를 하려는 클라이언트가 해당 채널에 없는 클라이언트일때 분기
    {
        client._messageOut += ERR_NOTONCHANNEL(client, channel->_name);
        return;
    }
    else if (!(memberIt->second & MODE_CHANNELADMINISTRATOR)) // 해당 클라이언트가 채널 생성자가 아닌 경우일때 분기
    {
        client._messageOut += ERR_CHANOPRIVSNEEDED(client, channel->_name);
        return;
    }

    ft::Channel::memberIterator clientIt = channel->getMemberByNickName(nickName); //"getMemberByNickName"메서드를 통해서 해당 클라이언트가 채널에 속해있는지 없는지 확인한다.
    if (clientIt == channel->_members.end())                                       // 만약 해당 클라이언트가 채널에 속해있지 않은 클라이언트일때 분기
    {
        client._messageOut += ERR_USERNOTINCHANNEL(client, nickName, channel->_name); // 에러메세지를 출력
        return;
    }
    else if (clientIt->first->_nickName != client._nickName) // 강퇴하려는 사람이 자기 자신이 아닌경우(즉, 다른 유저를 강퇴하는 경우일때 분기)
    {
        clientIt->first->_message.clear();                              // 해당 클라이언트의 메세지를 비워준다.
        clientIt->first->_message._arguments.push_back(channel->_name); //"PART"메서드를 실행하기 위한 인자를 넣어준다.
        clientIt->first->_message._arguments.push_back(kickComment);    //"PART"메서드를 실행하기 위한 강퇴 이유를 넣어준다.
        ft::Command::PART(*clientIt->first);                            // 강퇴하려는 해당 유저의 "PART"명령어를 실행함으로써 강퇴시킨다.
    }
}

/* "KICK"명령어 실행 메서드 */
void ft::Command::KICK(ft::Client &client)
{
    if (client._message._arguments.size() < 2) //"KICK"명령어에 인자가 2개 미만일 시 에러처리 분기
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
        return;
    }
    std::string kickComment = client._message._arguments[2].empty() ? client._nickName : client._message._arguments[2]; //"KICK"명령어에 이유를 넣는 인자가 없다면 강퇴하는 클라이언트의 닉네임을 넣거나 인자가 있다면 해당 이유로 초기화해준다.
    std::vector<std::string> channels = ft::tools::ft_splitMessage(client._message._arguments[0], ',');                 // 강퇴하려는 방이 ','를 통해서 여러개 들어왔는지 확인하고 파싱해서 "channels"벡터에 초기화해준다.
    std::transform(channels.begin(), channels.end(), channels.begin(), ft::tools::ft_toLower);                          // 대문자로 입력했으면 소문자로 변환해준다.
    std::vector<std::string> members = ft::tools::ft_splitMessage(client._message._arguments[1], ',');                  // 강퇴하려는 유저가 2명 이상일 경우 "ft_splitMessage"함수로 분할해서 "members"벡터에 초기화해준다.
    ft::Channel *channel;                                                                                               // 각각의 채널을 임시로 저장할 "channel"객체를 선언해준다.
    if (channels.size() != 1 && channels.size() != members.size())                                                      // 존재하지 않는 채널이면서 채널 개수와 멤버의 개수가 일치하지 않는 경우 잘못된 명령어 입력이므로 함수를 종료한다.
    {
        return;
    }
    else if (channels.size() == 1) // 강퇴하려는 채널이 1개만 들어올때 분기
    {
        channel = ft::Server::_server->findChannel(channels.front()); // "channel"임시객체에 "searchChannel"메서드를 이용해서 강퇴하려는 채널로 초기화한다.
        if (channel == NULL)                                          // 존재하지 않는 채널이면 에러 메세지를 출력
        {
            client._messageOut += ERR_NOSUCHCHANNEL(client, channels.front());
        }
        else
        {
            for (std::vector<std::string>::iterator vectorIt = members.begin(); vectorIt != members.end(); ++vectorIt) // 서버에 존재하는 채널이라면 해당 채널에 있는 클라이언트들에 대해서 순회를 한다.
            {
                ft::Command::kickMember(client, *vectorIt, channel, kickComment); // 해당 클라이언트를 채널에서 강퇴시킨다.
            }
        }
    }
    else // 채널이 여러개 들어왔을 경우
    {
        for (std::vector<std::string>::iterator vectorIt = members.begin(), channelIt = channels.begin(); vectorIt != members.end(); ++vectorIt, ++channelIt)
        {
            channel = ft::Server::_server->findChannel(*channelIt); // 해당 채널이 서버에 존재하는 채널인지 체크해서 초기화한다.
            if (channel == NULL)                                    // 채널을 찾지 못한 경우 분기
            {
                client._messageOut += ERR_NOSUCHCHANNEL(client, channels.front());
            }
            else // 서버에 존재하는 채널을 찾았을 경우 분기
            {
                ft::Command::kickMember(client, *vectorIt, channel, kickComment);
            }
        }
    }
}