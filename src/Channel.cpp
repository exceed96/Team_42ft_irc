#include "../Header/Header.hpp"

/* 어떤 채널에 모드가 변하거나 클라이언트가 닉네임을 변경하거나 하면 채널에 뿌려주는 메서드 */
void ft::Server::broadcastJoinedChannels(ft::Client &client, std::string message)
{
    ft::Server::channelIterator channelIt;                                                     // 닉네임을 바꾼 해당 클라이언트가 어떤 채널에 있는지 순회하기 위한 반복자 선언
    for (channelIt = this->_channels.begin(); channelIt != this->_channels.end(); channelIt++) // 해당 클라이언트가 속한 채널들이 뭐가 있는지 순회하는 반복문
    {
        if (client._channelsJoined.find(channelIt->first) != client._channelsJoined.end()) // 해당 클라이언트가 속한 채널이 있을때 분기
        {
            channelIt->second.broadcast(client, message); // 해당 채널의 클라이언트들에게 메세지를 보내준다.
        }
    }
}

/* 클라이언트가 입력한 메세지를 실행시키는 메서드 */
void ft::Server::execCommand(ft::Client &client)
{
    this->_commands[client]; //"operator[]"메서드를 통해서 해당 클라이언트의 정보에서 메세지의 명령어 부분을 실행시킨다.
}

/* 서버에 있는 채널을 삭제하는 메서드 */
void ft::Server::removeChannel(Channel &channel)
{
    this->_channels.erase(channel._name); //"erase"메서드를 이용해서 해당 채널을 "_channels"컨테이너에서 삭제한다.
}

/* 현재 서버에 만들어진 채널 목록을 반환해주는 메서드 */
ft::Server::channelType &ft::Server::getChannels(void)
{
    return this->_channels;
}

/* 채널명을 통해서 해당 채널을 찾는 메서드 */
ft::Channel *ft::Server::findChannel(std::string &channelName)
{
    ft::Server::channelIterator channelIt;                                                                                                     // 채널들이 저장된 "Map"컨테이너를 순회하기 위한 반복자 선언
    for (channelIt = this->_channels.begin(); channelIt != this->_channels.end() && channelIt->second._name.compare(channelName); channelIt++) // 반복문을 통해서 채널들이 저장된 "Map"컨테이너에서 동일한 채널 이름이 있는지 찾는다.
    {
        continue; // 없으면 계속 continue
    }
    return channelIt != this->_channels.end() ? &channelIt->second : NULL; // 동일한 채널 이름을 찾으면 해당 채널을 반환해주고 아니면 NULL을 반환
}

/* 새로운 채널을 채널들이 모여있는 "Map"컨테이너에 추가하는 메서드 */
ft::Channel *ft::Server::addChannel(std::string &channelName)
{
    std::pair<ft::Server::channelIterator, bool> channelPtr = this->_channels.insert(std::make_pair(channelName, ft::Channel(channelName))); //"channelPtr"변수를 통해서 새롭게 추가할 데이터를 만들어준다.(key : 채널이름, value : 해당 채널이름을 가진 "Channel"객체)
    channelPtr.first->second._mode = MODE_NOEXTERNAL | MODE_PRIVATE;                                                                         // 새로 추가 된 채널의 반복자를 가르키를 "addCh.first"에서 새로 추가된 채널의 모드를 "MODE_NOEXTERNAL"과 "MODE_SECRET으로 지정해준다.
    return &channelPtr.first->second;                                                                                                        // 새로 추가된 채널 객체의 포인터를 반환한다. 즉 포인터를 통해서 새로운 채널을 추가하는 것이다.
}

/* 해당 채널에 클라이언트 이름으로 들어온 인자를 통해서 클라이언트가 해당 채널에 있는지 없는지 확인하는 메서드 */
ft::Channel::memberIterator ft::Channel::getMember(std::string &memberName)
{
    ft::Channel::memberIterator memberIt = _members.begin();
    while (memberIt != this->_members.end() && memberIt->first->_nickName != memberName)
    {
        memberIt++;
    }
    return (memberIt);
}

/* 해당 채널에 "client"객체로 들어온 인자를 통해서 클라이언트가 해당 채널에 있는지 없는지 확인하는 메서드 */
ft::Channel::memberIterator ft::Channel::getMember(ft::Client *member)
{
    return (_members.find(member)); //"find"메서드를 통해서 인자로들어온 "client"객체가 채널 멤버 변수인 "_members"에 있는지 찾는다.
}

/* 해당 채널에 클라이언트 이름으로 들어온 인자를 통해서 클라이언트가 해당 채널에 있는지 없는지 확인하는 메서드 */
ft::Channel::memberIterator ft::Channel::getMemberByNickName(std::string &memberName)
{
    ft::Channel::memberIterator memberIt = _members.begin();
    while (memberIt != this->_members.end() && memberIt->first->_nickName != memberName)
    {
        memberIt++;
    }
    return (memberIt);
}

/* 해당 채널의 모드들을 반환해주는 메서드 */
std::string ft::Channel::getChannelModes(void)
{
    std::string modes = "+";       // 해당 채널의 모드를 출력할 문자열 "modes"선언
    if (this->_mode & MODE_INVITE) // 해당 채널의 모드가 "초대방모드"일때 분기
    {
        modes += "i";
    }
    if (this->_mode & MODE_MODERATED) // 해당 채널의 모드가 "제한모드"일때 분기
    {
        modes += "m";
    }
    if (this->_mode & MODE_TOPIC) // 해당 채널의 모드가 "토픽모드"일때 분기
    {
        modes += "t";
    }
    if (this->_mode & MODE_NOEXTERNAL) // 해당 채널의 모드가 "외부사용자압정금지모드"일때 분기
    {
        modes += "n";
    }
    if (this->_mode & MODE_KEY) // 해당 채널의 모드가 "비밀번호모드"일때 분기
    {
        modes += "k";
    }
    if (this->_mode & MODE_LIMIT) // 해당 채널의 모드가 "클라이언트인원제한모드"일때 분기
    {
        modes += "l";
    }
    if (this->_mode & MODE_CHANNELOPERATOR) // 해당 채널의 모드가 "사적인모드"일때 분기(private이랑 비슷하긴 하다)
    {
        modes += "o";
    }
    if (this->_mode & MODE_PRIVATE) // 해당 채널의 모드가 "사적인모드"일때 분기(secret이랑 비슷하긴 하다)
    {
        modes += "p";
    }
    return modes;
}

/* 해당 채널에 있는 클라이언트 리스트를 반환해주는 메서드 */
std::string ft::Channel::getMembers(void)
{
    std::string members;                  // 채널에 속해있는 클라이언트 목록을 저장하기 위한 변수
    ft::Channel::memberIterator memberIt; // 채널에 속해있는 클라이언트 객체가 모여있는 컨테이너를 순회하기 위한 반복자
    for (memberIt = this->_members.begin(); memberIt != this->_members.end(); ++memberIt)
    {
        members += (memberIt->second & MODE_CHANNELADMINISTRATOR ? "@" : (memberIt->second & MODE_CHANNELVOICE ? "+" : "")) + (*memberIt).first->_nickName + ' '; // 해당 채널에 있는 클라이언트의 모드가 관리자이면 @으로 한다.
    }
    return members;
}

/* 채널에 클라이언트를 추가하는 메서드 */
bool ft::Channel::addClientInChannel(ft::Client *client, std::string &key)
{
    ft::Client &client_ = *client; // 포인터를 참조로 바꾸어서 접근에 더 용이하게 한다.(안해도 상관없지만 로직도 줄고 짱구 굴릴일 없이 이게 더 편하다)
    if (this->_members.empty())    // 채널에 아무도 없다면, 즉 채널을 생성한 클라이언트 일때 분기
    {
        goto add;
    }
    if (this->_mode & MODE_LIMIT && static_cast<unsigned char>(this->_members.size()) >= this->_limitClient) // 채널의 모드에 "MODE_LIMIT"이 지정되어 있고 채널이 가득 차 있는 경우일때 분기
    {
        client->_messageOut += ERR_CHANNELISFULL(client_, this->_name);
        return false;
    }
    if (this->_mode & MODE_KEY && this->_key != key) // 채널의 모드가 "MODE_KEY"가 지정되어 있을때 분기, 입력한 키와 채널에 저장된 키가 다르다면
    {
        client->_messageOut += ERR_BADCHANNELKEY(client_, this->_name);
        return false;
    }
    if (this->_mode & (MODE_INVITE | MODE_PRIVATE) && !client->isInvited(this->_name)) // 해당 채널들은 초대받아야 들어갈 수 있으므로 클라이언트가 초대 받았는지 아닌지 "isInvited"메서드를 통해서 확인하는 분기
    {
        client->_messageOut += ERR_INVITEONLYCHAN(client_, this->_name);
        return false;
    }
add:
    std::pair<ft::Channel::memberIterator, bool> memberIt = this->_members.insert(std::make_pair(client, '\0')); // 새로운 멤버를 추가하는데 해당 클라이언트를 추가하고, 권한은 기본권한을 준다.
    if (this->_members.size() == 1)                                                                              // 만약 채널을 만든사람이라면
    {
        memberIt.first->second = MODE_CHANNELADMINISTRATOR | MODE_CHANNELOWNER; // 그 유저에게는 채널의 관리자 및 채널 소유자로 지정한다.
    }
    client_.joinChannel(this->_name); // 해당 클라이언트가 가진 참여한 채널 그룹 컨테이너에 채널 이름을 추가하는 메서드인 "joinChannel"를 실행
    return true;
}

/* 해당 채널에 속한 유저들에게 본인 빼고 메세지를 보내는 메서드 */
void ft::Channel::broadcast(ft::Client &client, std::string message)
{
    ft::Channel::memberIterator memberIt;                                                 // 해당 채널에 속한 클라이언트 들을 순회하기 위한 반복자
    for (memberIt = this->_members.begin(); memberIt != this->_members.end(); ++memberIt) // 채널에 속한 모든 클라이언트들을 순회한다.
    {
        if (client._nickName.compare(memberIt->first->_nickName)) // 닉네임을 바꾼 "clinet"를 제외한 모든 클라이언트들을 찾는 분기
        {
            memberIt->first->_messageOut += message; // 해당 메세지를 클라이언트들에게 보낸다.
        }
    }
}

/* 해당 채널에 속한 유저들에게 메세지를 보내주는 메서드 */
void ft::Channel::broadcast(std::string message)
{
    ft::Channel::memberIterator memberIt;
    for (memberIt = this->_members.begin(); memberIt != this->_members.end(); ++memberIt) // 해당 채널에 있는 모든 클라이언트들을 순회한다.
    {
        (*memberIt).first->_messageOut += message; // 순회하면서 메세지를 보내준다.
    }
}