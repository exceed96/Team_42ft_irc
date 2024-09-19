#include "../../Header/Header.hpp"

/* "PRIVMSG"명령어 실행 메서드 */
void ft::Command::PRIVMSG(ft::Client &client)
{
    if (client._message._arguments.empty() || client._message._arguments.front().empty()) //"PRIVMSG"명령어에 인자가 없거나 첫번째 인자가 없을경우 에러메세지 출력
    {
        client._messageOut += ERR_NORECIPIENT(client);
    }
    else if (client._message._arguments.size() < 2 || client._message._arguments[2].empty()) //"PRIVMSG"명령어에 인자가 2개보다 적거나 2번째 인자가 비어있는 경우 에러메세지 출력
    {
        client._messageOut += ERR_NOTEXTTOSEND(client);
    }
    else if (client._message._arguments.front()[0] == '#') // 채널에 메세지를 보내려는 분기
    {
        std::string channelName = ft::tools::ft_toLower(client._message._arguments.front()); // 대문자로 들어온 채널명을 소문자로 변경해주고 "channelName"변수에 초기화 해준다.
        ft::Channel *channel = ft::Server::_server->findChannel(channelName);                // 해당 채널이 실제로 서버에 존재하고 있는 채널인지 "searchChannel"메서드를 통해서 찾는다.
        if (channel == NULL)                                                                 // 실제 채널에 입력한 채널이 존재하지 않을때 분기
        {
            client._messageOut += ERR_NOSUCHCHANNEL(client, channelName);
        }
        else
        {
            ft::Channel::memberIterator memberIt = channel->getMember(&client);          // 해당 채널에 있는 모든 클라이언트에게 메세지를 뿌려야 하므로 "getMember"메서드를 통해서 해당 채널 클라이언트들로 초기화해준다.
            if (channel->_mode & MODE_NOEXTERNAL && memberIt == channel->_members.end()) // 해당 채널이 "외부메세지차단모드"이고 클라이언트가 채널의 클라이언트가 아닌경우 분기
            {
                client._messageOut += ERR_CHANNOEXT(client, channel->_name);
            }
            else if (channel->_mode & MODE_MODERATED && memberIt == channel->_members.end())
            {
                client._messageOut += ERR_CANNOTSENDTOCHAN(client, channel->_name);
            }
            else if (channel->_mode & MODE_MODERATED && memberIt != channel->_members.end() && !(memberIt->second & MODE_CHANNELVOICE || memberIt->second & MODE_CHANNELADMINISTRATOR || memberIt->second & MODE_CHANNELOPERATOR)) // 해당 채널이 "특정권한모드"이면서 해당 채널에 참가한 클라이언트고 "MODE_CHANNELVOICE"모드가 아닐때 분기
            {
                client._messageOut += ERR_CANNOTSENDTOCHAN(client, channel->_name);
            }
            else
            {
                client._idle = std::time(NULL); // 클라이언트의 최근 활동시간을 최신화 한다.
                std::string replyMessage;
                for (std::vector<std::string>::iterator stringIt = client._message._arguments.begin() + 1; stringIt != client._message._arguments.end(); stringIt++) // 만약 메세지가 공백까지 포함해서 올 경우 전체 메세지 출력
                {
                    replyMessage += *stringIt;
                    replyMessage += " ";
                }
                replyMessage.erase(replyMessage.end());
                channel->broadcast(client, MSG(client, channel->_name, replyMessage)); // 해당 채널에 있는 클라이언트들에게 "broadcast"메서드로 메세지를 보내준다.
            }
        }
    }
    else // 채널이 아닌 특정 클라이언트에게 메세지를 보내는 분기
    {
        ft::Client *receiveClient = ft::Server::_server->findClient(client._message._arguments.front()); //"findClient"메서드를 통해서 메세지를 받는 클라이언트를 찾는다.
        if (receiveClient == NULL)                                                                       // 해당 클라이언트를 찾지 못한 경우 분기
        {
            client._messageOut += ERR_NOSUCHNICK(client, client._message._arguments.front());
        }
        else if (receiveClient->_nickName.compare(client._nickName)) // 메세지를 보내는 사용자와 받는 사용자가 같지 않을때 즉, 다른사람한테 보낼때
        {
            std::string replyMessage;
            for (std::vector<std::string>::iterator stringIt = client._message._arguments.begin() + 1; stringIt != client._message._arguments.end(); stringIt++) // 만약 메세지가 공백까지 포함해서 올 경우 전체 메세지 출력
            {
                replyMessage += *stringIt;
                replyMessage += " ";
            }
            replyMessage.erase(replyMessage.end());
            receiveClient->_messageOut += MSG(client, receiveClient->_nickName, replyMessage);
            client._clientsConnected.insert(receiveClient->_nickName); // 해당 클라이언트와 메세지 보내기에 성공했다면 연결된 클라이언트 목록에 추가해준다.(안해줘도 될듯?)
            receiveClient->_clientsConnected.insert(client._nickName); // 상대 클라이언트에게도 메세지를 보낸 클라이언트의 이름을 연결된 클라이언트 목록에 추가해준다.
            if (receiveClient->_mode & MODE_AWAY)                      // 해당 클라이언트가 만약 자리비움 상태일때 분기
            {
                client._messageOut += RPL_AWAY(client, receiveClient->_nickName, receiveClient->_messageAway); // 부재중 메모를 메세지를 보내는 클라이언트에게 출력해준다.
            }
        }
    }
}
