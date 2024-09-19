#pragma once

#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <string.h>
#include <string>
#include <sstream>
#include <set>
#include <ostream>
#include <netdb.h>
#include <map>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <exception>
#include <curl/curl.h>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <csignal>
#include <arpa/inet.h>
#include <algorithm>

#include "Reply.hpp"

#define BACKLOG 30
#define MAXFD 1024
#define MAXUSERS "1024"
#define SPACE ' '
#define CRLF "\r\n"
#define COLON ':'
#define SPCL " :"
#define SPECIAL_CHARACTERS "[\\]^_'{|}"
#define NULCRLFSPCL "\0\r\n :"
#define NICK_DELAY 30

#define CLIENTMODE_VAILABLE "awiro"
enum userModes
{
    MODE_AWAY = 1,        // 사용자가 자리를 비울때 유저모드
    MODE_WALLOPS = 2,     // 서버의 관리자 모드
    MODE_INVISIBLE = 8,   // 클라이언트 정보 숨김 모드
    MODE_RESTRICTED = 16, // 제한된 권한을 가진 유저모드
    MODE_OPERATOR = 32,   // 운영자 권한을 가진 유저모드
};

#define CHANNELMODE_VAILABLE "imnotklp"
enum channelModes
{
    MODE_INVITE = 1,           // 초대로만 참가할수 있는 채널모드
    MODE_MODERATED = 2,        // 특정권한을 가진 클라이언트만 메세지를 보낼수 있는 모드
    MODE_NOEXTERNAL = 4,       // 외부에서 오는 메세지를 차단하는 채널모드
    MODE_PRIVATE = 8,          // 비밀방인 채널 모드
    MODE_CHANNELOPERATOR = 16, // 관리자를 서로 변경할 수 있는 모드
    MODE_TOPIC = 32,           // 토픽을 지정할 수 있는 모드
    MODE_KEY = 64,             // 비밀번호를 입력해야 들어갈 수 있는 모드
    MODE_LIMIT = 128,          // 채널에 인원제한이 있는 모드
};

#define CLIENTCHANNELMODE_VAILABLE "vo"
enum clientChannelModes
{
    MODE_CHANNELADMINISTRATOR = 1,
    MODE_CHANNELVOICE = 2,
    MODE_CHANNELOWNER = 4,
};

namespace ft
{
    class Client;
    class Channel;

    /* Command 클래스 */
    class Command
    {
    private:
        typedef std::map<int, ft::Client>::iterator clientIterator;                                   // 수신한 명령어가 어떤 클라이언트에게 온건지 찾기 위한 "Map"컨테이너 반복자 별칭 선언
        typedef std::map<std::string, void (ft::Command::*)(ft::Client &)> commandType;               // 명령어를 처리하기 위한 컨테이너 별칭 선언, 함수포인터를 value로 넣음으로 해당 명령어를 실행한다, "ft::Command::*"클래스 내부의 멤버함수를 가르킨다.
        typedef std::map<std::string, void (ft::Command::*)(ft::Client &)>::iterator commandIterator; // 위 "Map"컨테이너를 순회하기 위한 반복자 별칭 선
        commandType _commands;                                                                        // 클라이언트로부터 수신받을 메세지를 저장할 "Map"컨테이너 선언
        void PASS(ft::Client &);                                                                      //"PASS"명령어 메서드
        void NICK(ft::Client &);                                                                      //"NICK"명령어 메서드
        void USER(ft::Client &);                                                                      //"User"명령어 메서드
        void JOIN(ft::Client &);                                                                      //"JOIN"명령어 메서드
        void QUIT(ft::Client &);                                                                      //"QUIT"명령어 메서드
        void LIST(ft::Client &);                                                                      //"LIST"명령어 메서드
        void NOTICE(ft::Client &);                                                                    //"NOTICE"명령어 메서드
        void KICK(ft::Client &);                                                                      //"KICK"명령어 메서드
        void PART(ft::Client &);                                                                      //"PART"명령어 메서드
        void TOPIC(ft::Client &);                                                                     //"TOPIC"명령어 메서드
        void INVITE(ft::Client &);                                                                    //"INVITE"명령어 메서드
        void WHOIS(ft::Client &);                                                                     //"WHOIS"명령어 메서드
        void MODE(ft::Client &);                                                                      //"MODE"명령어 메서드
        void OPER(ft::Client &);                                                                      //"OPER"명령어 메서드
        void PRIVMSG(ft::Client &);                                                                   //"PRIVMSG"명령어 메서드
        void RESTART(ft::Client &);                                                                   //"RESTART"명령어 메서드
        void AWAY(ft::Client &);                                                                      //"AWAY"명령어 메서드
        void DOWN(ft::Client &);                                                                      //"DOWN"명령어 메서드
        void NAMES(ft::Client &);                                                                     //"NAMES"명령어 메서드
        void WHO(ft::Client &);                                                                       //"WHO"명령어 메서드
        void BOT(ft::Client &);                                                                       //"BOT"명령어 메서드
        void PONG(ft::Client &);                                                                      //"PONG"명령어 메서드
        void WALLOPS(ft::Client &);                                                                   //"WALLOPS"명령어 메서드
        void kickMember(ft::Client &, std::string &, ft::Channel *, std::string &);                   // 채널에서 클라이언트를 강퇴하는 메서드

    public:
        Command(void);
        ~Command(void) {}
        void operator[](ft::Client &); // "[]"오버로딩을 하여서 객체를 인자로 받아서 해당 클라이언트의 명령어를 처리하는 메서드
    };

    /* Server 클래스 */
    class Server
    {
    public:
        typedef std::vector<pollfd> pollType;                                 // 서버의 소켓 디스크립터와 클라이언트의 소켓 디스크립터를 저장할 "poll"구조체 벡터 별칭 선언
        typedef std::vector<pollfd>::iterator pollIterator;                   // 소켓들이 저장된 "poll"벡터의 반복자를 별칭 선언
        typedef std::map<std::string, ft::Channel> channelType;               // 채널들이 저장된 "Map"컨테이너 별칭 선언
        typedef std::map<std::string, ft::Channel>::iterator channelIterator; // 채널들이 저장된 "Map"컨테이너를 위한 반복자 별칭 선언
        typedef std::map<int, ft::Client> clientType;                         // 서버에서 클라이언트를 관리할 벡터 별칭 선언
        typedef std::map<int, ft::Client>::iterator clientIterator;           // 클라이언트들이 담겨있는 Map 컨테이너에서 사용할 반복자를 별칭 선언
    private:
        unsigned short _port;  // 서버가 열어둔 포트 넘버 멤버변수
        char *_buf;            // 버퍼로 사용하기 위한 멤버변수
        pollType _allSockets;  // 서버의 소켓 디스크립터와 클라이언트의 소켓 디스크립터를 저장할 "poll"구조체 벡터 멤버변수
        clientType _clients;   // 서버에서 클라이언트를 관리할 벡터 멤버변수
        channelType _channels; // 서버에 있는 채널들을 관리하기 위한 "Map"컨테이너 멤버변수
        Command _commands;     // 클라이언트가 입력한 메세지를 저장하는 객체 멤버변수

        void sendMessage(ft::Client &);                          // 클라이언트에게 메세지를 보내는 메서드
        void receiveMessage(ft::Client &);                       // 클라이언트에게서 메세지를 받는 메서드
        void disconnectClient(clientIterator &, pollIterator &); // 서버에 있는 클라이언트의 접속을 끊어버리는 메서드
        void acceptClient(void);                                 // 서버에 클라이언트가 접근 시 처리하는 메서드
        static void staticSigHandler(int sig);                   // 서버가 종료되었을때 "_serverDown"플래그 멤버변수를 변경하는 메서드

    public:
        Server(const int &ac, char **ag);     //"Server"클래스의 생성자(인자의 개수, 인자)
        ~Server(void);                        //"Server"클래스의 소멸자
        static Server *_server;               //"Server"클래스로 만든 객체
        static bool _serverDown;              // 서버가 종료되었는지 아닌지 판단하는 플래그 멤버변수, static으로 선언하여서 모든 객체가 하나의 변수를 공유하도록 설정
        static bool _restart;                 // 서버가 재시작 여부를 지정한 플래그 멤버변수
        static std::string _hostAddr;         // 서버가 연결된 네트워크의 주소를 저장하기 위한 멤버변수
        static std::string _password;         // 서버에 접근하기 위한 비밀번호 멤버변수
        static std::string _operatorPass;     // 서버의 관리자 비밀번호를 저장하기 위한 멤버변수
        static std::string _createServerTime; // 서버를 생성한 시간을 저장하기 위한 멤버변수

        void createServerSocket(void);                           // 서버와 서버에서 사용하는 소켓을 생성하는 메서드
        void activatingServer(void);                             // 생성한 서버를 실행하는 메서드
        void execCommand(ft::Client &);                          // 명령어를 실행하는 메서드
        void closeServer(void);                                  // 서버를 종료시키는 메서드
        bool alreadyNickName(const std::string &);               // 클라이언트가 닉네임을 등록할때 이미 있는 닉네임인지 확인하는 메서드
        ft::Client *findClient(const std::string &);             // 특정 클라이언트를 찾는 메서드
        ft::Channel *addChannel(std::string &);                  // 새로운 채널을 채널들이 모여있는 "Map"컨테이너에 추가하는 메서드
        void removeChannel(Channel &);                           // 서버에 있는 채널을 삭제하는 메서드
        ft::Channel *findChannel(std::string &);                 // 서버에 있는 채널을 찾는 메서드
        channelType &getChannels(void);                          // 현재 서버에 만들어진 채널의 리스트를 반환해주는 메서드
        clientType &getClients(void);                            // 서버에 있는 모든 클라이언트를 반환하는 메서드
        void broadcastAddministratorClients(ft::Client &);       // 서버에 있는 모든 클라이언트에게 메세지를 보내는 메서드
        void broadcastJoinedChannels(ft::Client &, std::string); // 해당 채널에 접속한 클라이언트들에게 메세지를 뿌려주는 메서드

        std::string getClientsCount(void);
        std::string getOperCount(void);
        std::string getChannelsCount(void);
    };

    /* Message 클래스 */
    class Message
    {
    public:
        std::string _command;                // 클라이언트로부터 받은 메세지의 명령어를 저장할 멤버변수
        std::vector<std::string> _arguments; // 클래스로부터 받은 메세지를 파싱하여 벡터에 저장하는 멤버변수

        Message(void){};
        ~Message(void){};
        void parseMessage(std::string &); // 클라이언트로부터 받은 메세지를 파싱하는 메서드
        void clear(void);                 // 멤버변수에 기존에 들어있던 메세지를 비워주는 메서드
    };

    /* Client 클래스 */
    class Client
    {
    public:
        typedef std::set<std::string> stringSetType;                            // 문자열원소 기반의 set컨테이너 별칭 선언
        typedef std::set<std::string>::iterator stringSetIterator;              // 문자열원소 기반의 set컨테이너 반복자 별칭 선언
        typedef stringSetType channelType;                                      // 클라이언트가 접속한 채널을 저장할 set컨테이너 별칭 선언
        typedef stringSetIterator channelIterator;                              // 클라이언트가 접속한 채널을 저장한 set컨테이너 반복자 별칭 선언
        typedef std::map<ft::Client *, unsigned char> memberType;               // 클라이언트 객체와 클라이언트의 모드를 저장한 map 컨테이너 별칭 선언
        typedef std::map<ft::Client *, unsigned char>::iterator memberIterator; // 클라이언트의 객체와 모드를 저장한 컨테이너의 반복자 별칭 선언

        Client(int &fd) : _fd(fd), _mode(0), _out(false), _nickName("*"), _userName("*"), _realName("*"), _nickChange(0), _passFlag(false), _nickFlag(false), _userFlag(false), _loginFlag(false) {}
        ~Client(void) {}
        int _fd;                         // 클라이언트의 소켓 디스크립터를 저장할 멤버변수
        unsigned char _mode;             // 클라이언트의 모드를 저장할 멤버변수
        bool _out;                       // 클라이언트가 서버에서 나갔는지 판단하는 플래그 멤버변수
        std::string _hostAddr;           // 호스트 주소 정보를 저장할 멤버변수
        std::string _nickName;           // 클라이언트의 닉네임을 저장할 멤버변수
        std::string _userName;           // 클라이언트의 유저명을 저장할 멤버변수
        std::string _realName;           // 클라이언트의 실제이름을 저장할 멤버변수
        std::string _messageIn;          // 클라이언트가 수신한 메시지
        std::string _messageOut;         // 클라이언트가 송신할 메시지
        std::string _messageAway;        // 클라이언트가 부재중일때 표시하는 메세지
        std::time_t _sign;               // 클라이언트가 서버에 등록한 시간을 저장하는 시간멤버변수
        std::time_t _idle;               // 클라이언트가 서버에서 마지막으로 활동한 시간을 기록하는 시간멤버변수
        std::time_t _nickChange;         // 클라이언트가 닉네임을 바꿨는지 안바꿨는지 확인하는 플래그
        ft::Message _message;            // 클라이언트가 보낸 메세지를 파싱하는 "Message"클래스의 객체를 생성
        channelType _channelsJoined;     // 클라이언트가 접속한 채널들을 저장한 "set"멤버변수
        channelType _channelInvited;     // 클라이언트가 초대받은 채널들을 저장한 "set"멤버변수
        stringSetType _clientsConnected; // 클라이언트와 연결된 다른 클라이언트의 닉네임을 저장하는 "set"컨테이너
        bool _passFlag;
        bool _nickFlag;
        bool _userFlag;
        bool _loginFlag;

        void partChannel(std::string &);     // 클라이언트가 채널에서 나가면 관련된 컨테이너에서 삭제하는 메서드
        void joinChannel(std::string &);     // 클라이언트가 채널에 추가하면 관련된 컨테이너에 추가하는 메서드 (_channelJoined)
        void inviteChannel(std::string &);   // 클라이언트가 초대받을때마다 해당 채널을 추가하는 메서드
        bool inChannel(std::string &);       // 클라이언트가 특정 채널에 들어가 있는지 아닌지 판단하는  메서드
        bool isInvited(std::string &);       // 클라이언트가 채널에 초대받았는지 아닌지 판단하는 플래그
        std::string getIdleTime(void);       // 클라이언트가 서버에 들어온지 얼마나 지났는지 알려주는 메서드
        std::string getJoinServerTime(void); // 클라이언트가 서버에 등록한 시간을 알려주는 메서드
        std::string getChannels(char);       // 클라이언트가 참여한 채널목록을 반환해주는 메서드
        std::string getClientModes(void);    // 클라이언트가 가지고 있는 모드를 반환하는 메서드
        std::string who(ft::Channel &);      // 해당 클라이언트의 디테일한 정보를 반환하는 메서드
        std::string who(std::string &);      // 인자로 들어온 채널이름을 통해서 해당 유저의 디테일한 정보를 반환하는 메서드
    };

    /* Channel 클래스 */
    class Channel
    {
    public:
        typedef std::map<ft::Client *, unsigned char> memberType;               // 해당 채널에 속한 클라이언트들을 저장하기 위한 "Map"컨테이너 별칭 선언
        typedef std::map<ft::Client *, unsigned char>::iterator memberIterator; // 해당 채널에 속한 클라이언트들을 저장하기 위한 "Map"컨테이너 반복자 별칭 선언

        Channel(const std::string name_ = std::string()) : _name(name_) {} //"Channel"클래스의 생성자, 만약 아무이름도 안 들어올시 빈 문자열로 생성
        ~Channel(void) {}

        std::string _key;           // 채널의 비밀번호를 저장하는 멤버변수
        std::string _name;          // 채널 이름을 저장하는 멤버변수
        std::string _topic;         // 채널에 토픽을 저장하는 멤버변수
        unsigned char _mode;        // 채널의 모드를 저장하는 멤버변수
        unsigned char _limitClient; // 채널이 수용할 수 있는 최대 인원수를 저장한 멤버변수
        memberType _members;        // 해당 채널에 속한 클라이언트들을 저장하기 위한 "Map"컨테이너

        memberIterator getMemberByNickName(std::string &);    // 입력된 해당 닉네임을 가진 유저가 해당 채널에 들어가 있는지 확인하는 메서드
        memberIterator getMember(ft::Client *);               // 해당 채널에 "client"객체로 들어온 인자를 통해서 클라이언트가 해당 채널에 있는지 없는지 확인하는 메서드
        memberIterator getMember(std::string &);              // 해당 채널에 인자로 들어온 이름을 가진 클라이언트가 있는지 없는지 확인하는 메서드
        std::string getChannelModes(void);                    // 해당 채널의 모드들을 반환해주는 메서드
        std::string getMembers(void);                         // 해당 채널에 있는 클라이언트 목록을 반환해주는 메서드
        bool addClientInChannel(ft::Client *, std::string &); // 채널에 클라이언트를 추가하는 메서드
        bool isMember(ft::Client *);
        void broadcast(std::string);               //  해당 채널에 속한 클라이언트들에게 메세지를 보내는 메서드
        void broadcast(ft::Client &, std::string); // 해당 채널에 속한 클라이언트들에게 메세지를 보내는 메서드(단, 본인 제외)
    };

    namespace tools
    {
        int ft_stoi(const std::string &);                                            // 문자열을 정수형으로 바꿔주는 함수
        std::string ft_toLower(std::string &);                                       // 문자열을 소문자로 바꿔주는 함수
        std::string ft_toUpper(std::string &);                                       // 문자열을 대문자로 바꿔주는 함수
        bool ft_pollfdCmp(const pollfd &, const pollfd &);                           // "poll"벡터에 있는 구조체 원소들을 정렬하는 메서드
        bool ft_nickIsValid(const std::string &);                                    // 클라이언트에게 "NICK"명령어로 닉네임을 받았을 시 유효성을 검증하는 함수
        unsigned char ft_modeSetting(const int &);                                   // "USER"명령어로 유저의 모드를 받아오면 세팅하는 함수
        std::vector<std::string> ft_splitMessage(const std::string &, const char &); // 메세지를 특정 구분자에 맞춰서 분할하여 반환하는 메서드
        bool ft_channelNameIsValid(const std::string &);                             // 채널이름이 유효한지 아닌지 판단하는 함수
    }
}