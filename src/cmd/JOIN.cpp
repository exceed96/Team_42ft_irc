#include "../../Header/Header.hpp"

/* 매칭 시켜준 채널이름과 채널 비밀번호를 통해서 입장 가능한지 안한지 판별하는 함수 */
static void joinChannel(ft::Client &client, std::string &channelName, std::string &channelKey)
{
    if (!ft::tools::ft_channelNameIsValid(channelName)) //"ft_channelNameIsValid"함수를 통해서 사용가능한 채널이름인지 확인한다.
    {
        client._messageOut += ERR_NOSUCHCHANNEL(client, channelName);
    }
    else if (client.inChannel(channelName)) //"inChannel"메서드를 통해서 해당 클라이언트가 이미 해당 채널에 있는지 없는지 판단하는 분기
    {
        client._messageOut += ERR_USERONCHANNEL(client, channelName);
    }
    else // 유효성 검증을 통과한 채널의 이름을 처리하는 분기
    {
        ft::Channel *channel = ft::Server::_server->findChannel(channelName); //"searchChannel"메서드를 통해서 주어진 채널이름과 동일한 채널이름이 있는지 찾는다.
        if (channel == NULL)                                                  // 찾지 못해서 "NULL"을 반환했을때 분기
        {
            channel = ft::Server::_server->addChannel(channelName); //"addChannel"메서드를 통해서 주어진 채널이름을 채널 "Map"컨테이너에 추가해준다.
        }
        if (channel->_mode & MODE_LIMIT && static_cast<unsigned char>(channel->_members.size()) == channel->_limitClient) // 채널의 모드가 "MODE_LIMIT"모드이고 채널의 인원이 꽉 찼을때 분기
        {
            client._messageOut += ERR_CHANNELISFULL(client, channelName);
        }
        else if (channel->_mode & MODE_KEY && channel->_key != channelKey) // 채널의 모드가 "MODE_KEY"이고 채널키가 입력한 채널 비밀번호랑 일치하지 않을때 분기
        {
            client._messageOut += ERR_BADCHANNELKEY(client, channelName);
        }
        else
        {
            bool addCh = channel->addClientInChannel(&client, channelKey); // "addClientChannel"메서드를 통해서 채널에 해당 클라이언트를 추가해준다.
            if (addCh)                                                     // 성공적으로 클라이언트를 채널에 추가했을때 분기
            {
                channel->broadcast(RPL_JOIN(client, channel->_name)); //"broadcast"메서드로 웰컴메세지를 띄워준다.
                if (!channel->_topic.empty())                         // 채널에 토픽이 있을 때 분기
                {
                    client._messageOut += RPL_TOPIC(client, channel->_name, channel->_topic); // 채널에 토픽을 출력해준다.
                }
                client._messageOut += RPL_NAMREPLY(client, channel->_name, channel->getMembers());
                client._messageOut += RPL_ENDOFNAMES(client, channel->_name);
            }
        }
    }
}
/* "JOIN"명령어 실행 메서드 */
void ft::Command::JOIN(ft::Client &client)
{
    if (client._message._arguments.empty() || client._message._arguments.front().empty()) //"JOIN"명령어에 인자가 없거나 인자는 있지만 비워져 있는 상태일때 분기
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
    }
    else
    {
        std::vector<std::string> channelNames = ft::tools::ft_splitMessage(client._message._arguments[0], ',');                                                                     //"ft_splitMessage"함수를 통해서 ','를 기준으로 파싱해준다.
        std::transform(channelNames.begin(), channelNames.end(), channelNames.begin(), ft::tools::ft_toLower);                                                                      // "JOIN"명령어로 입력받은 채널의 이름을 소문자로 바꿔준다.
        std::vector<std::string> channelKeys = client._message._arguments.size() > 1 ? ft::tools::ft_splitMessage(client._message._arguments[1], ',') : std::vector<std::string>(); // 만약 채널에 입장할때 비밀번호가 주어진다면 비밀번호 부분도 똑같이 ','로 구분된걸 파싱해서 초기화 시킨다.
        std::string channelKey;                                                                                                                                                     // 채널키들을 저장할 변수
        for (std::vector<std::string>::iterator vectorIt = channelNames.begin(); vectorIt != channelNames.end(); ++vectorIt, channelKey.clear())                                    //"channelNames"벡터를 순회한다., 순회하면서 채널키들을 비워준다.(순서에 맞게 매칭시키기 위해서)
        {
            if (!channelKeys.empty()) // 채널에 입장할때 필요한 비밀번호 인자가 주어졌을때 분기
            {
                channelKey = channelKeys.front();       // 맨 앞에 있는 채널에다가 맨앞에 있는 비밀번호를 매칭시켜준다.
                channelKeys.erase(channelKeys.begin()); // 매칭 시켜준 맨 앞 비밀번호는 삭제한다.
            }
            joinChannel(client, *vectorIt, channelKey); // 매칭시켜준 비밀번호를 통해서 "JoinChannel"함수를 통해서 입장이 가능한지 안한지 판별한다.
        }
    }
}