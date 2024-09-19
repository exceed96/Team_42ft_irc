#include "../Header/Header.hpp"

/* 서버에 접속한 클라이언트에게 소켓을 생성해주는 메서드 */
void ft::Server::acceptClient(void)
{
    if (this->_clients.size() >= MAXFD) // 서버에 클라이언트가 꽉 찼을때 분기
    {
        return; // 함수를 종료시킨다.
    }
    pollfd pfd;                                                                               // 새로운 클라이언트 소켓을 감시할 "pfd"구조체 선언
    bzero(&pfd, sizeof(pfd));                                                                 // 생성한 "pfd"구조체를 초기화해준다.
    sockaddr_in clientAddr;                                                                   // 클라이언트의 주소 정보를 저장할 "clientAddr"선언
    socklen_t clientAddrLen = sizeof(clientAddr);                                             //"clientAddr"구조체의 사이즈 크기 초기화
    pfd.fd = accept(this->_allSockets.front().fd, (sockaddr *)(&clientAddr), &clientAddrLen); //"accept" 함수를 통해 클라이언트와 서버와 통신할 클라이언트의 소켓을 입력된 클라이언트의 주소 정보를 토대로 생성
    // 서버 소켓 디스크립터, 클라이언트의 주소 정보, 클라이언트의 주소 정보 크기
    if (pfd.fd == -1) // 클라이언트 소켓 생성에 실패했을 시 함수를 종료
    {
        return;
    }
    pfd.events = POLLIN | POLLOUT;                                                                                                        // 새로운 클라이언트의 이벤트 옵션도 입력가능한상태, 출력가능한상태로 초기화
    ft::Server::pollIterator pollIt = std::lower_bound(this->_allSockets.begin(), this->_allSockets.end(), pfd, ft::tools::ft_pollfdCmp); // 새로 생성한 클라이언트 소켓을 "poll"벡터에서 어디에 위치할지 정해서 "pollIt"에 초기화
    this->_allSockets.insert(pollIt, pfd);                                                                                                //"insert"함수를 통해서 정해준 위치에 클라이언트의 소켓 디스크립터를 push한다.
    std::string clientHostAddr = inet_ntoa(clientAddr.sin_addr);                                                                          // 클라이언트의 IP주소를 가져와 "clientHostAddr"에 초기화해준다. "inet_ntoa"함수를 통해서 구조체의 주소를 문자열로 변환
    if (clientHostAddr == "127.0.0.1")                                                                                                    // 클라이언트의 접속주소가 "127.0.0.1"일때 분기
    {
        clientHostAddr = ft::Server::_hostAddr; // 서버의 IP주소로 바꿔준다.(클라이언트가 서버와 동일한 호스트에서 접속한 경우를 위한 처리이다.) 안해도 될려나?
    }
    this->_clients.insert(std::make_pair(pfd.fd, ft::Client(pfd.fd))).first->second._hostAddr = clientHostAddr; // 생성한 클라이언트의 소켓 디스크립터를 클라이언트 관리 벡터에 key -> 클라이언트 소켓 디스크립터, value -> 해당 클라이언트의 "Client" 객체에 호스트의 주소를 초기화해준다.
}

/* 서버에 있는 클라이언트가 접속을 끊었을 때 처리하는 메서드 */
void ft::Server::disconnectClient(clientIterator &clientIt, pollIterator &pollIt)
{
    clientIt->second._messageIn = "PART " + clientIt->second.getChannels(',') + SPACE + clientIt->second._messageAway + CRLF; // 클라이언트가 나갈때 보내는 메세지를 초기화
    this->execCommand(clientIt->second);
    ft::Client *target; // 서버와 연결이 끊어진 클라이언트 객체를 저장할 "target" 변수
    for (ft::Client::stringSetIterator stringIt = clientIt->second._clientsConnected.begin();
         stringIt != clientIt->second._clientsConnected.end(); stringIt++)
    {
        target = this->findClient(*stringIt); // 서버에서 나간 클라이언트를 찾는다면 "target"객체에다가 초기화 해준다.
        if (target)
        {
            target->_messageOut += RPL_QUIT(clientIt->second); // 서버에서 나간 클라이언트에게 메세지를 수신한다.
        }
    }
    close(clientIt->second._fd);                                                                                        // 서버에서 나간 클라이언트의 소켓 디스크립터를 닫아준다.
    std::cout << "User '" << clientIt->second._nickName << "' - '" << clientIt->second._hostAddr << "' disconnected\n"; // 서버에 해당 유저의 연결이 끊어졌다고 출력
    this->_allSockets.erase(pollIt);                                                                                    // 소켓 디스크립터 벡터에서도 삭제해준다.
    this->_clients.erase(clientIt);                                                                                     // 클라이언트들의 벡터에서도 삭제해준다.
    pollIt = this->_allSockets.begin();                                                                                 // 해당하는 소켓 디스크립터를 지웠으면 pollIt에는 이제는 사용하지 않는 메모리에 접근하는 것이므로 새롭게 초기화 해줘야 한다. (세그먼트 해결)
    clientIt = this->_clients.begin();                                                                                  // 해당하는 클라이언트 "Map"컨테이너에서 원소를 지웠으면 이제는 사용하지 않는 메모리에 접근하는 것이므로 새롭게 초기화 해줘야 한다.(세그먼트 해결)
}

/* 서버에 있는 모든 클라이언트를 반환하는 메서드 */
ft::Server::clientType &ft::Server::getClients(void)
{
    return this->_clients;
}

/* 특정 클라이언트를 찾는 메서드 */
ft::Client *ft::Server::findClient(const std::string &nickName)
{
    ft::Server::clientIterator clientIt; // 클라이언트 "Map"컨테이너를 순회해서 닉네임이 같은게 있는지 찾는다.
    for (clientIt = this->_clients.begin(); clientIt != this->_clients.end() && clientIt->second._nickName.compare(nickName); clientIt++)
    {
        continue; // 만약 두 닉네임이 같지 않다면 반복문이 계속 진행된다.
    }
    return clientIt != this->_clients.end() ? &clientIt->second : NULL; // 만약 같은 닉네임을 찾으면 해당 클라이언트 객체를 반환하고 못찾으면 널포인터를 반환한다.
}

/* 클라이언트가 닉네임을 등록할 때 이미 등록된 닉네임인지 확인하는 메서드 */
bool ft::Server::alreadyNickName(const std::string &nickName)
{
    ft::Server::clientIterator clientIt;                                                                                                  // 다른 클라이언트들의 닉네임을 비교하기 위해서 "_clients" 컨테이너를 순회할 반복자 선언
    for (clientIt = this->_clients.begin(); clientIt != this->_clients.end() && clientIt->second._nickName.compare(nickName); clientIt++) // 다른 클라이언트들과 닉네임이 다르다면 continue
    {
        continue;
    }
    return clientIt != this->_clients.end(); // 반복문을 끝까지 돌았다면 즉, 닉네임과 다른 클라이언트들과 다르다면 반복자가 "_clients"마지막에 있겠지만 그게 아니라면 겹치는 닉네임이 있는거로 판단
}

/* 클라이언트가 채널에서 나가면 관련된 컨테이너에서 삭제하는 메서드 */
void ft::Client::partChannel(std::string &channelName)
{
    this->_channelsJoined.erase(channelName);
}

/* 클라이언트가 채널에 추가하면 관련된 컨테이너에 추가하는 메서드 (_channelJoined) , 채널을 생성한거지 채널에 초대받은게 아니니 "_channelInvited"에서는 삭제한다. */
void ft::Client::joinChannel(std::string &channelName)
{
    this->_channelsJoined.insert(channelName);
    this->_channelInvited.erase(channelName);
}

/* 클라이언트가 초대받을때마다 해당 채널을 추가하는 메서드 */
void ft::Client::inviteChannel(std::string &channelName)
{
    this->_channelInvited.insert(channelName);
}

/* 클라이언트가 특정 채널에 있는지 없는지 확인하는 메서드 */
bool ft::Client::inChannel(std::string &channelName)
{
    return this->_channelsJoined.find(channelName) != this->_channelsJoined.end();
}

/* 클라이언트가 특정 채널에 초대받았는지 아닌지 확인하는 메서드 */
bool ft::Client::isInvited(std::string &channelName)
{
    return this->_channelInvited.find(channelName) != this->_channelInvited.end();
}

/* 클라이언트가 서버에 들어온지 얼마나 지났는지 알려주는 메서드 */
std::string ft::Client::getIdleTime(void)
{
    std::stringstream ss;                         // 시간값을 스트림에 저장하기 위한 "ss"객체
    ss << std::difftime(time(NULL), this->_idle); // 현재시간과 클라이언트가 생성된 시간을 비교해서 그 차이를 "ss"스트림 초기화
    return ss.str();                              // 스트림객체를 문자열로 변경해서 반환해준다.
}

/* 클라이언트가 서버에 등록한 시간을 알려주는 메서드 */
std::string ft::Client::getJoinServerTime(void)
{
    std::stringstream ss; // 시간값을 스트림에 저장하기 위한 "ss"객체
    ss << this->_sign;    // 클라이언트를 생성할때 해당 시간으로 초기화 해놓은 시간인 "this->_sign"으로 "ss"스트림 초기화
    return ss.str();      // 스트림객체를 문자열로 변경해서 반화해준다.
}

/* 클라이언트가 참여한 채널목록을 반환해주는 메서드 */
std::string ft::Client::getChannels(char c)
{
    std::string channels; // 클라이언트가 참여한 채널들의 목록을 저장할 변수
    for (std::set<std::string>::iterator setIt = this->_channelsJoined.begin(); setIt != this->_channelsJoined.end(); setIt++)
    {
        channels += *setIt; // 클라이언트가 접속한 채널이 있다면 "chanels"변수에다가 추가해준다.
        channels += c;      //'c'문자는 채널들을 구분하기 위한 구분자이다.
    }
    if (!channels.empty()) // 클라이언트가 참여한 채널이 있다면
    {
        channels.erase(channels.end() - 1); // 맨 마지막에 구분자를 제거해준다(이거때문에 진짜)
    }
    return channels; // 클라이언트가 참여한 채널목록을 반환해준다.
}

/* 클라이언트가 가지고 있는 모드를 반환하는 메서드 */
std::string ft::Client::getClientModes(void)
{
    std::string modes = "+";
    if (_mode & MODE_AWAY) // 해당 클라이언트의 모드가 "AWAY"일 경우 분기
    {
        modes += "a";
    }
    if (_mode & MODE_WALLOPS) // 해당 클라이언트의 모드가 "WALLOPS"일 경우 분기
    {
        modes += "w";
    }
    if (_mode & MODE_INVISIBLE) // 해당 클라이언트의 모드가 "INVISIBLE"일 경우 분기
    {
        modes += "i";
    }
    if (_mode & MODE_RESTRICTED) // 해당 클라이언트의 모드가 "RESTRICTED"일 경우 분기
    {
        modes += "r";
    }
    if (_mode & MODE_OPERATOR) // 해당 클라이언트의 모드가 "OPERATOR"일 경우 분기
    {
        modes += "o";
    }
    return modes;
}

/* 채널에 있는 해당 유저에 대한 디테일한 정보를 반환하는 메서드 */
std::string ft::Client::who(ft::Channel &channel)
{
    std::string res = this->_mode & MODE_AWAY ? "G" : "H"; // 해당 클라이언트가 "AWAY"모드이면 "G", "AWAY"모드가 아니면 "H"
    if (this->_mode & MODE_OPERATOR)                       // 해당 클라이언트가 서버의 관리자이면 "[operator]"문자열 추가
    {
        res += " *";
    }
    if (channel.getMember(this->_nickName)->second & MODE_CHANNELADMINISTRATOR) // 해당 클라이언트가 해당 채널의 관리자일때 분기
    {
        res += " @";
    }
    else if (channel.getMember(this->_nickName)->second & MODE_CHANNELVOICE) // 해당 클라이언트가 해당 채널의 발언권한이 있을때 분기
    {
        res += " +";
    }
    return res;
}

/* */
std::string ft::Client::who(std::string &channelName)
{
    ft::Channel *channel = ft::Server::_server->findChannel(channelName); //"searchChannel"메서드를 통해서 인자로 들어온 채널이름으로 해당 채널을 "channel"객체에 저장한다.
    std::string res;
    if (channel != NULL) // 만약 해당 채널이 존재할때 분기
    {
        res = this->who(*channel);
    }
    else
    {
        res = this->_mode & MODE_AWAY ? "G" : "H"; // 해당 클라이언트가 "AWAY"모드이면 "G", "AWAY"모드가 아니면 "H"
        if (this->_mode & MODE_OPERATOR)           // 클라이언트가 서버의 관리자일때 분기
        {
            res += " *";
        }
    }
    return res;
}