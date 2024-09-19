#include "../../Header/Header.hpp"

/* "channelName"인자를 통해 받아온 채널이름이 존재하면 클라이언트에게  출력하는 함수 */
static void printChannel(ft::Client &client, std::string &channelName)
{
    ft::Channel *channel = ft::Server::_server->findChannel(channelName); // 임시 채널 객체를 만들어서 "searchChannel"을 통해서 해당 채널을 해당 채널로 찾으면 초기화해준다.
    if (channel == NULL)                                                  // 만약 채널이 없어서 반환값이 NULL일때 분기
    {
        client._messageOut += ERR_NOSUCHCHANNEL(client, channelName);
    }
    else
    {
        std::stringstream ss;                                                              // 문자열을 다루기 위한 스트림 객체 선언
        ss << channel->_members.size();                                                    // 채널 리스트에 참가한 클라이언트의 수를 "ss"스트림 객체에 초기화
        client._messageOut += RPL_LIST(client, channel->_name, ss.str(), channel->_topic); // 채널 리스트를 클라이언트에게 출력해준다.
    }
}

/* "LIST"명령어 실행 메서드 */
void ft::Command::LIST(ft::Client &client)
{
    if (client._message._arguments.empty() || client._message._arguments.front().empty()) //"LIST"명령어에 인자가 들어오지 않고 파라미터 자체가 비워 있을 떄 분기
    {
        if (ft::Server::_server->getChannels().size() > 10) // 만약 현재 서버에 만들어진 채널의 개수가 10개 이상일 때 분기
        {
            client._messageOut += ERR_TOOMANYMATCHES(client);
        }
        for (ft::Server::channelIterator channelIt = ft::Server::_server->getChannels().begin(); channelIt != ft::Server::_server->getChannels().end(); ++channelIt) // 채널 목록이 저장된 컨테이너를 순회해서 비밀방, private방이 아닌경우에 리스트에 출력하게 해준다.
        {
            if (!(channelIt->second._mode & MODE_PRIVATE)) // 채널의 모드가 "PRIVATE"가 아닐경우 리스트로 나오게 해준다.
            {
                printChannel(client, channelIt->second._name); //"printChannels"함수를 통해서 "LIST"명령어에 대한 결과값을 출력해준다.
            }
        }
    }
    else //"LIST"명령어 뒤에 인자가 올시 즉, 특정 채널에 대한 리스트를 보고 싶은 경우, ex) "LIST #test, #test2"
    {
        std::vector<std::string> channelNames = ft::tools::ft_splitMessage(client._message._arguments[0], ',');              //"ft_splitMessage"함수를 통해서 인자를 ','으로 구분해서 "channelNames"벡터에 초기화 해준다.
        std::transform(channelNames.begin(), channelNames.end(), channelNames.begin(), ft::tools::ft_toLower);               // 채널명을 대문자로 입력했을 경우 소문자로 변환해준다.
        for (std::vector<std::string>::iterator vectorIt = channelNames.begin(); vectorIt != channelNames.end(); ++vectorIt) // 반복자를 선언하여서 "channelNames"벡터를 순회하여 인자로 입력한 해당 채널을 "listChannels" 함수를 통해 출력한다.
        {
            printChannel(client, *vectorIt);
        }
    }
    client._messageOut += RPL_LISTEND(client); // 채널 리스트의 마지막을 알리는 출력
}
