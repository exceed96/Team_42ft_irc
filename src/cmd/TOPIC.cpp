#include "../../Header/Header.hpp"

/* "TOPIC"명령어 실행 메서드 */
void ft::Command::TOPIC(ft::Client &client)
{
    if (client._message._arguments.empty() || client._message._arguments.front().empty()) //"TOPIC"명령어와 함께 인자가 입력되지 않을때 분기
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
    }
    else
    {
        std::string channelName = ft::tools::ft_toLower(client._message._arguments[0]); //"TOPIC"을 적용할 채널의 이름이 대문자로 들어올 경우 소문자로 바꿔준다.
        ft::Channel *channel = ft::Server::_server->findChannel(channelName);           //"channel"임시객체에 "searchChannel"을 통해서 서버에 저장된 채널의 이름을 찾고 초기화 한다.
        if (channel == NULL)                                                            // 서버에 채널이 없는 경우
        {
            client._messageOut += ERR_NOSUCHCHANNEL(client, channelName);
        }
        else if (client._message._arguments.size() == 1 && !channel->_topic.empty()) // "TOPIC"명령어에 인자가 1개만 들어오고 해당 채널에 토픽이 이미 있을 때 해당 토픽을 채널에 뿌린다.
        {
            client._messageOut += RPL_TOPIC(client, channel->_name, channel->_topic);
        }
        else if (client._message._arguments.size() == 1 && channel->_topic.empty()) // "TOPIC"명령어에 인자가 1개만 들어오고 채널의 토픽이 없다면, 토픽이 없음을 클라이언트에게 알려주는 분기
        {
            client._messageOut += RPL_NOTOPIC(client, channel->_name);
        }
        else if (!(channel->_mode & MODE_TOPIC)) // 해당 채널이 "TOPIC"모드가 아닐경우 분기
        {
            client._messageOut += ERR_NOTOPICMODE(client, channel->_name);
            return;
        }
        else
        {
            ft::Channel::memberIterator memberIt = channel->getMember(&client); // 클라이언트가 해당 채널의 멤버인지 아닌지 "getMember"메서드를 통해서 확인
            if (memberIt == channel->_members.end())                            // 클라이언트가 해당 채널에 속한 클라이언트가 아닐때 분기
            {
                client._messageOut += ERR_NOTONCHANNEL(client, channel->_name);
            }
            else if (channel->_mode & MODE_TOPIC && !(memberIt->second & MODE_CHANNELADMINISTRATOR)) // 해당 채널이 "TOPIC"모드이지만 해당 클라이언트가 관리자가 아닐때 분기
            {
                client._messageOut += ERR_CHANOPRIVSNEEDED(client, channel->_name);
            }
            else // 토픽을 쓸수있는 채널모드이고 채널의 관리자일때 분기
            {
                channel->_topic = client._message._arguments[1];                               // 받아온 인자로 채널의 토픽을 초기화
                channel->broadcast(RPL_TOPIC_CHANGE(client, channel->_name, channel->_topic)); // "broadcast"메서드를 이용해서 해당 채널에 있는 유저들에게 바뀐 토픽을 뿌려준다.
            }
        }
    }
}