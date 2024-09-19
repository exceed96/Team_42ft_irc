#include "../../Header/Header.hpp"

/* 해당 채널에서 유저의 모드를 바꿔주는 함수 */
static bool changeClientChannelMode(ft::Client &client, char mode, bool add, ft::Channel &channel, std::string &channelModes)
{
    if (client._message._arguments.size() < 3)
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
        return false;
    }
    else if (!(channel._mode & MODE_CHANNELOPERATOR))
    {
        client._messageOut += ERR_NEEDOPERATORMODE(client);
        return false;
    }
    else
    {
        ft::Channel::memberIterator memberIt = channel.getMember(client._message._arguments[2]);
        if (memberIt == channel._members.end())
        {
            client._messageOut += ERR_USERNOTINCHANNEL(client, client._message._arguments[2], channel._name);
            return false;
        }
        else if (mode == 'o' && add)
        {
            memberIt->second |= MODE_CHANNELADMINISTRATOR;
        }
        else if (mode == 'o')
        {
            memberIt->second &= ~MODE_CHANNELADMINISTRATOR;
        }
        else if (mode == 'v' && add)
        {
            memberIt->second |= MODE_CHANNELVOICE;
        }
        else
        {
            memberIt->second &= ~MODE_CHANNELVOICE;
        }
    }
    channelModes += client._message._arguments[2] + SPACE;
    client._message._arguments.erase(client._message._arguments.begin() + 2);
    return true;
}

/* 채널의 모드를 바꾸는 함수 */
static bool changeChannelMode(ft::Client &client, char mode, bool add, ft::Channel &channel, std::string &channelModes)
{
    if (client._message._arguments.size() != 2 && (mode != 'l' && mode != 'k'))
    {
        return false;
    }
    if (mode == 'i' && add)
    {
        if (channel._mode & MODE_PRIVATE)
        {
            client._messageOut += ERR_PRIVATEMODE(client);
        }
        else
        {
            channel._mode |= MODE_INVITE;
        }
    }
    else if (mode == 'i')
    {
        channel._mode &= ~MODE_INVITE;
    }
    else if (mode == 'm' && add)
    {
        channel._mode |= MODE_MODERATED;
    }
    else if (mode == 'm')
    {
        channel._mode &= ~MODE_MODERATED;
    }
    else if (mode == 'o' && add)
    {
        channel._mode |= MODE_CHANNELOPERATOR;
    }
    else if (mode == 'o')
    {
        channel._mode &= ~MODE_CHANNELOPERATOR;
    }
    else if (mode == 'p' && add)
    {
        if (channel._mode & MODE_INVITE)
        {
            client._messageOut += ERR_INVITEMODE(client);
        }
        else
        {
            channel._mode |= MODE_PRIVATE;
        }
    }
    else if (mode == 'p')
    {
        channel._mode &= ~MODE_PRIVATE;
    }
    else if (mode == 'n' && add)
    {
        channel._mode |= MODE_NOEXTERNAL;
    }
    else if (mode == 'n')
    {
        channel._mode &= ~MODE_NOEXTERNAL;
    }
    else if (mode == 't' && add)
    {
        channel._mode |= MODE_TOPIC;
    }
    else if (mode == 't')
    {
        channel._mode &= ~MODE_TOPIC;
    }
    else if (mode == 'k' && add)
    {
        if (channel._mode & MODE_KEY)
        {
            client._messageOut += ERR_KEYSET(client, channel._name);
        }
        else if (client._message._arguments.size() > 2 && !client._message._arguments[2].empty())
        {
            channel._key = client._message._arguments[2];
            channelModes += client._message._arguments[2] + SPACE;
            client._message._arguments.erase(client._message._arguments.begin() + 2);
            channel._mode |= MODE_KEY;
        }
    }
    else if (mode == 'k')
    {
        channel._mode &= ~MODE_KEY;
    }
    else if (mode == 'l' && add)
    {
        if (client._message._arguments.size() < 3)
        {
            client._messageOut += ERR_MODELIMITNEEDMOREPARAMS(client);
        }
        else
        {
            int limit = ft::tools::ft_stoi(client._message._arguments[2]);
            channelModes += client._message._arguments[2] + SPACE;
            client._message._arguments.erase(client._message._arguments.begin() + 2);
            if (limit < 0)
            {
                return true;
            }
            channel._limitClient = std::min(limit, MAXFD);
            channel._mode |= MODE_LIMIT;
        }
    }
    else if (mode == 'l')
    {
        channel._mode &= ~MODE_LIMIT;
    }
    else
    {
        client._messageOut += ERR_UNKNOWNMODE(client, mode);
        return false;
    }
    return true;
}

/* 클라이언트의 모드를 변경하는 함수 */
static void changeClientMode(ft::Client &client, char mode, bool add)
{
    if (mode == 'a' && !add)
    {
        client._mode &= ~MODE_AWAY;
    }
    else if (mode == 'w' && add)
    {
        client._mode |= MODE_WALLOPS;
    }
    else if (mode == 'w')
    {
        client._mode &= ~MODE_WALLOPS;
    }
    else if (mode == 'i' && add)
    {
        client._mode |= MODE_INVISIBLE;
    }
    else if (mode == 'i')
    {
        client._mode &= ~MODE_INVISIBLE;
    }
    else if (mode == 'r' && add)
    {
        client._mode |= MODE_RESTRICTED;
    }
    else if (mode == 'r')
    {
        client._mode &= ~MODE_RESTRICTED;
    }
    else if (mode == 'o' && add)
    {
        client._messageOut += ERR_OPERATORMODE(client);
    }
    else if (mode == 'o' && !add)
    {
        client._mode &= ~MODE_OPERATOR;
    }
}

/* "MODE"명령어 실행 메서드 */
void ft::Command::MODE(ft::Client &client)
{
    if (client._message._arguments.empty() || client._message._arguments.front().empty()) //"MODE"명령어와 함께 인자가 입력되지 않을때 분기
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
    }
    else if (client._message._arguments.front()[0] == '#') // 인자의 맨 앞에 '#"일 경우 즉, 채널의 모드를 바꿀때 분기
    {
        std::string channelName = ft::tools::ft_toLower(client._message._arguments[0]); // 인자로 받아온 채널이름이 대문자일 경우 소문자로 변경해준다.
        ft::Channel *channel = NULL;                                                    // 해당 채널을 저장하기 위한 임시 "channel"객체 선언
        if (client.inChannel(channelName))                                              // 클라이언트가 해당 채널에 들어가 있는 경우
        {
            channel = ft::Server::_server->findChannel(channelName); //"channel"을 모드를 바꾸려는 해당 채널의 이름으로 초기화해준다.
        }
        if (channel == NULL) // 해당 채널에 모드를 바꾸려는 유저가 없을때 분기
        {
            client._messageOut += ERR_NOSUCHCHANNEL(client, channelName);
        }
        else if (client._message._arguments.size() == 1) // 모드를 바꾸려는 채널의 이름만 들어오고 어떤 모드인지에 대한 인자가 들어오지 않았을때 분기, 즉 해당 채널이 가지고 있는 모드를 출력
        {
            client._messageOut += RPL_CHANNELMODEIS(client, channel->_name, channel->getChannelModes());
        }
        else
        {
            ft::Channel::memberIterator memberIt = channel->_members.find(&client); // 해당 채널에 유저가 있는지 없는지 "_members"컨테이너에서 확인한다.
            if (memberIt == channel->_members.end())                                // 채널의 모드를 바꾸려는 클라이언트가 해당 채널에 없을때 분기
            {
                client._messageOut += ERR_NOTONCHANNEL(client, channel->_name);
            }
            else if (!(memberIt->second & MODE_CHANNELADMINISTRATOR)) // 해당 클라이언트가 채널의 관리자가 아닐경우
            {
                client._messageOut += ERR_CHANOPRIVSNEEDED(client, channel->_name);
            }
            else
            {
                bool add = true;
                std::string channelModes[2];                 // 변경하려는 모드를 저장할 "channelModes"변수
                if (client._message._arguments[1][0] != '-') // 모드의 앞에 '+'가 있을때 분기
                {
                    channelModes[0] += '+'; // 채널의 모드를 추가하는 '+'를 넣어준다.
                }
                for (std::string::const_iterator stringIt = client._message._arguments[1].begin(); stringIt != client._message._arguments[1].end(); ++stringIt)
                {
                    if (*stringIt == '+') // 만약 '+'인 채널 추가를 만날때 분기
                    {
                        if (!add) //"add"변수가 false이면 true로 바꿔주는 분기
                        {
                            add = true;
                            channelModes[0] += '+';
                        }
                    }
                    else if (*stringIt == '-') // 만약 '-'인 채널 제거를 만날 때 분기
                    {
                        if (add) //"add"변수가 true이면 false로 바꿔주는 분기
                        {
                            add = false;
                            channelModes[0] += '-';
                        }
                    }
                    else if (std::strchr(CHANNELMODE_VAILABLE, *stringIt) != NULL && changeChannelMode(client, *stringIt, add, *channel, channelModes[1]))
                    {
                        channelModes[0] += *stringIt; // 해당 채널의 모드 변경이 가능하다면 해당 모드로 channelModes[0]을 초기화해준다.
                    }
                    else if (std::strchr(CLIENTCHANNELMODE_VAILABLE, *stringIt) != NULL && changeClientChannelMode(client, *stringIt, add, *channel, channelModes[1]))
                    {
                        channelModes[0] += *stringIt; // 채널에 속한 해당 유저의 모드 변경이 가능하다면 해당 모드로 channelModes[0]을 초기화해준다.
                    }
                }
                channelModes[0] += SPACE + channelModes[1];                                // 최종으로 챈러 모드 변경 문자열을 "channelModes[0]"에 초기화 한다.
                channel->broadcast(RPL_CHANMODE(client, channel->_name, channelModes[0])); // 해당 채널에 모드 변경 문자열인 "channelModes[0]"을 "broadcast" 채널에 속한 클라이언트에게 뿌려준다.
            }
        }
    }
    else // 유저
    {
        if (client._message._arguments.size() == 1 && !client._message._arguments.front().compare(client._nickName)) // 유저 자신의 모드를 확인하려고 할때 분기
        {
            client._messageOut += RPL_UMODEIS(client); // 자신이 가지고 있는 모드를 출력한다.
        }
        else if (client._message._arguments.front().compare(client._nickName))
        {
            client._messageOut += ERR_USERSDONTMATCH(client);
        }
        else
        {
            bool add = true; // 모드변경이 가능한지 안한지 판단하는 플래그
            std::string clientModes;
            if (client._message._arguments[1][0] != '-') // ex) "MODE a w"처럼 명령어가 들어오면 "clientMode"에 "+"를 추가한다.
            {
                clientModes += '+';
            }
            for (std::string::const_iterator stringIt = client._message._arguments[1].begin(); stringIt != client._message._arguments[1].end(); ++stringIt)
            {
                if (*stringIt == '+') // 클라이언트에 모드를 추가할때 분기
                {
                    if (!add)
                    {
                        add = true;
                        clientModes += '+';
                    }
                }
                else if (*stringIt == '-') // 클라이언트에 모드를 제거할 때 분기
                {
                    if (add)
                    {
                        add = false;
                        clientModes += '-';
                    }
                }
                else if (std::strchr(CLIENTMODE_VAILABLE, *stringIt) != NULL)
                {
                    changeClientMode(client, *stringIt, add);
                }
                else // 유효하지 않은 모드가 들어왔을 시 에러 출력하는 분기
                {
                    client._messageOut += ERR_UNKNOWNMODE(client, *stringIt);
                }
            }
            client._messageOut += RPL_UMODE(client);
        }
    }
}