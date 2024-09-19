#include "../../Header/Header.hpp"

/* 인자로 받아온 채널이름에 들어있는 클라이언트 리스트를 출력하는 함수 */
static void printChannel(ft::Client &client, std::string &channelName)
{
    ft::Channel *channel = ft::Server::_server->findChannel(channelName); //"searchChannel"메서드를 통해서 인자로 들어온 채널이 서버에 있는 채널인지 확인한다.
    if (channel)                                                          // 해당 채널이름이 서버에 없는경우 분기
    {
        std::stringstream ss;
        ss << channel->_members.size();                                                    // 데이터 처리를 위한 스트림 "ss"선언
        client._messageOut += RPL_NAMREPLY(client, channel->_name, channel->getMembers()); // 채널에 들어있는 클라이언트의 수를 "ss"스트림에 초기화시킨다.
        client._messageOut += RPL_ENDOFNAMES(client, channel->_name);                      // 채널에 있는 클라이언트를 출력해준다.
    }
}
/* "NAMES"명령어 실행 메서드 */
void ft::Command::NAMES(ft::Client &client)
{
    if (client._message._arguments.empty() || client._message._arguments.front().empty()) //"NAMES"명령어와 함께 인자가 입력되지 않은 경우, 즉 모든 채널에 순회하면서 각 채널에 있는 유저의 리스트를 보여주는 분기
    {
        for (ft::Server::channelIterator channelIt = ft::Server::_server->getChannels().begin(); channelIt != ft::Server::_server->getChannels().end(); ++channelIt)
        {
            if (ft::Server::_server->getChannels().size() > 10) // 뽑아야 하는 채널의 이름이 10개가 넘어가는 경우(즉, 너무 많이 출력해서 데이터를 구분하기 힘든 경우)
            {
                client._messageOut += ERR_TOOMANYMATCHESNAMES(client, channelIt->second._name);
            }
            else
            {
                printChannel(client, channelIt->second._name); //"printChannel"함수를 통해서 각 채널에 있는 클라이언트 리스트를 출력한다.
            }
        }
    }
    else
    {
        std::vector<std::string> channelNames = ft::tools::ft_splitMessage(client._message._arguments[0], ',');                 // 여러개의 채널이름이 들어올 경우에는 "ft_splitMessage"함수를 통해서 ','로 분할하여서 각각의 채널 이름을 벡터에 저장한다.
        std::transform(channelNames.begin(), channelNames.end(), channelNames.begin(), ft::tools::ft_toLower);                  // 대문자로 들어온 채널이름은 소문자로 변환해준다.
        for (std::vector<std::string>::iterator channelIt = channelNames.begin(); channelIt != channelNames.end(); ++channelIt) // 반복문을 통해서 벡터안에 담긴 채널들을 순회한다.
        {
            printChannel(client, *channelIt); //"printChannel"함수를 통해서 해당 채널에 있는 클라이언트의 리스트를 출력한다.
        }
    }
}