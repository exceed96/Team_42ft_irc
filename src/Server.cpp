#include "../Header/Header.hpp"

/* 하나의 서버 객체만 사용하므로 "static"을 고정값인 애들을 관리하는데 이럴려면 해당 소스파일에서 정적멤버변수는 초기화 해줘야 한다.*/
bool ft::Server::_serverDown = false;
bool ft::Server::_restart = false;
std::string ft::Server::_password = "";
std::string ft::Server::_hostAddr = "";
std::string ft::Server::_createServerTime = "";
std::string ft::Server::_operatorPass = "aaaa";
ft::Server *ft::Server::_server = NULL;

/* "Server"클래스의 생성자 */
ft::Server::Server(const int &ac, char **ag)
{
    if (ac != 3) // 인자의 개수가 3개가 아닐 시 인자 개수 에러 출력하는 분기
    {
        ERROR_ARGUS;
    }
    int port = ft::tools::ft_stoi(ag[1]); //"ft_stoi"함수를 통해서 문자열인 포트넘버를 정수형으로 바꿔준다.
    if (port < 1024 || port > 65534)      // 만약 입력된 포트 넘버가 1025 ~ 65535 범위가 아니라면 에러 출력하는 분기
    {
        ERROR_PORTNUMBER;
    }
    this->_port = port;                // 입력받은 포트넘버를 "Server"객체의 "_port"에 초기화해준다.
    ft::Server::_password = ag[2];     // 입력받은 포트넘버를 "Server"객체의 "_password"에 초기화해준다.
    if (ft::Server::_password.empty()) // 만약 입력받은 비밀번호가 비워있는 상태일때 분기
    {
        ERROR_PASSWORD;
    }
    this->_buf = new char[513];                                                       //"Server"객체에 있는 버퍼의 크기를 할당해준다.
    ft::Server::_server = this;                                                       // 만들어진 "Server"객체를 "_server"객체에 초기화 해준다.
    std::signal(SIGINT, Server::staticSigHandler);                                    //"Ctrl + c"를 누를시 서버를 종료하게 되는 메서드
    int res = std::system("ifconfig | grep 'inet ' | awk 'NR==2 {print $2}' > .log"); // 쉘 명령어를 통해서 현재 접속된 네트워크의 IP주소를 ".log"파일에다가 저장한다.
    if (res != 0)
    {
        std::runtime_error("Ifconfig command not active");
    }
    std::stringstream ss;                // 쉘 명령어를 통해서 현재 접속된 네트워크의 IP주소를 ".log"파일에다가 저장한다.
    ss << std::ifstream(".log").rdbuf(); //".log"파일의 내용을 "ss"스트림에 입력한다. "rdbuf"함수를 사용해서 스트림 객체 내부의 버퍼를 다른 스트림 버퍼로 변경해줄 수 있다.
    int ret = std::system("rm -f .log"); // 사용한 ".log"파일을 삭제해준다.
    if (ret != 0)
    {
        std::runtime_error("Rm command not active");
    }
    ft::Server::_hostAddr = ss.str().substr(0, ss.str().find('\n')); // 스트림에 저장된 문자열 중에서 첫 번째 줄을 "_hostaddr"멤버변수에 저장한다. 즉 , "substr()"함수를 통해서 첫번쨰 줄을 찾고 줄바꿈 문자를 찾아서 이전까지의 문자열을 저장하는 것이다.
}

/* 서버가 종료되었을때 "_serverDown"플래그 멤버변수를 변경하는 메서드 */
void ft::Server::staticSigHandler(int)
{
    ft::Server::_serverDown = true; // 서버가 종료된거면 "_serverDown"플래그를 true로 초기화해준다.
}

/* 서버와 서버에서 사용하는 소켓을 생성하는 메서드 */
void ft::Server::createServerSocket(void)
{
    addrinfo hints;                                                       // 지정하고자 하는 옵션으로 만들기 위한 네트워크 주소정보 구조체
    addrinfo *servinfo = NULL;                                            // 지정한 네트워크 주소 정보 구조체로 인한 결과값을 저장할 네트워크 주소 정보 구조체
    addrinfo *ptr = NULL;                                                 // 임시로 네트워크 주소를 저장할 네트워크 주소 구조체(순회할때 임시로 저장하기 위한 구조체)
    bzero(&hints, sizeof(hints));                                         // hints구조체를 "bzero"함수로 초기화 시켜줘야한다.
    hints.ai_family = AF_INET;                                            // 사용하고자 하는 주소 프로토콜을 IPv4로 지정
    hints.ai_socktype = SOCK_STREAM;                                      // 사용하고자 하는 소켓의 타입을 TCP로 지정
    hints.ai_flags = AI_PASSIVE;                                          // 서버 소켓을 생성할때 생성된 소켓을 "bind"함수에 사용하여 특정 IP주소와 포트에 바인딩 되도록 설정
    std::stringstream ss;                                                 // 문자열을 다루는 스트림을 생성
    ss << this->_port;                                                    // 포트 넘버를 스트림에 넘겨준다.
    int addrRes = getaddrinfo(NULL, ss.str().c_str(), &hints, &servinfo); // 지정한 주소 옵션대로 네트워크 주소로 변환하기 위해서 "getaddrinfo"함수를 호출한다.
    // 도메인 이름이거나 IP주소, 포트넘버, 사용하고자 하는 옵션을 가진 네트워크 주소 구조체, 변환된 네트워크 주소 구조체
    if (addrRes) //"getaddrinfo"함수가 실패했을때 분기
    {
        ERRNO_EXCEPT; //"getaddrinfo"함수가 실패했을시 "EAI_SYSTEM"은 자세하게 원인을 모르는 경우이므로 그때는 에러문구만 출력하고 나머지 경우는 에러코드를 출력한다.
    }
    pollfd serverSocket;                        // 서버 이벤트 관리를 위한 "poll"배열에 넣을 "poll"구조체 선언
    bzero(&serverSocket, sizeof(serverSocket)); //"servinfo"를 순회한다.
    for (ptr = servinfo; ptr; ptr = ptr->ai_next)
    {
        if ((serverSocket.fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) //"socket"함수를 통해서 소켓 생성에 실패했을때 분기
        // 네트워크 주소 프로토콜, 소켓 타입, 프로토콜 타입
        {
            continue; // 만약 소켓 생성에 실패했다면 다음 네트워크 주소 정보를 체크한다.
        }
    }
    serverSocket.events = POLLIN | POLLOUT;                                                       // 소켓 생성에 성공했다면 소켓 이벤트 상태를 입력가능한상태(POLLIN), 출력가능한상태(POLLOUT)으로 지정한다.
    addrRes = 1;                                                                                  //"getaddrinfo"의 반환값을 다시 1로 지정, 소켓의 옵션 value로 사용하기 위해서 지정
    if (serverSocket.fd == -1 ||                                                                  // 서버 소켓 생성에 실패했거나
        setsockopt(serverSocket.fd, SOL_SOCKET, SO_REUSEADDR, &addrRes, sizeof(addrRes)) == -1 || // 서버소켓 옵션 설정에 실패했거나
        // 서버 소켓 디스크립터, 일반소켓 옵션 사용, 소켓 재사용 가능, 소켓 옵션 value, 서버 소켓 옵션 사이즈
        fcntl(serverSocket.fd, F_SETFL, O_NONBLOCK) == -1 || // 서버 소켓을 "non-blocking"상태로 만들어주는데 실패했거나
        // 서버 소켓 디스크립터, 3번째 인자로 소켓을 설정, "non-blocking"모드로 설정
        bind(serverSocket.fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1 || //"bind"함수로 생성한 소켓과 엮을 네트워크 주소를 엮어주는데 실패했거나
        // 서버 소켓 디스크립터, 소켓에 바인딩할 주소 정보, 바인딩할 주소 정보의 크기
        listen(serverSocket.fd, BACKLOG) == -1) //"listen"함수를 통해서 서버를 외부의 접근에 대해서 감시하도록 설정하는데 실패했거나
    // 서버 소켓 디스크립터, 동시 최대 접근 클라이언트 수 (Queue)
    {
        ERRNO_EXCEPT; // 실패했다면 에러 코드를 출력해준다.
    }
    this->_allSockets.push_back(serverSocket);                                                                           // 첫번째로 서버 소켓을 "poll"구조체의 소켓 관리 벡터에 push해준다.
    freeaddrinfo(servinfo);                                                                                              // 다 쓴 "servinfo"는 무조건 해제해줘야 한다.
    std::time_t startTime = std::time(NULL);                                                                             // 서버의 시작 시간을 저장하기 위한 "startTime"변수
    (ft::Server::_createServerTime = std::asctime(std::localtime(&startTime))).erase(this->_createServerTime.end() - 1); //"startTime"을 로컬시간으로 변환한뒤 "asctime"함수를 이용해서 날짜, 시간을 파싱한다. 파싱한 후 마지막에 개행을 지워준다.
}

/* 생성한 서버를 실행하는 메서드 */
void ft::Server::activatingServer(void)
{
    clientIterator clientIt; // 별칭을 통한 클라이언트 Map컨테이너 반복자 선언
    pollIterator pollIt;     // 별칭을 통한 "poll"벡터의 반복자 선언
activating:
    std::cout << "IRC Server started successfully ~ " << this->_hostAddr << ":" << this->_port << "\n";
    while (1) // 무한루프를 통해서 서버를 활성화 한다.
    {
        if (poll(&this->_allSockets.front(), this->_allSockets.size(), -1) == -1) //"poll"함수를 통해서 소켓들의 이벤트 동작들을 감시하는데 실패했을 때 분기
        // 서버 소켓 디스크립터, 관리할 소켓컨테이너의 사이즈, 무한대기 상태
        {
            ERRNO_EXCEPT; // 에러 문구를 출력
        }
        if (this->_allSockets.front().revents == POLLIN) // 서버 소켓 디스크립터가 이벤트 수신준비를 완료했을 때 분기
        {
            acceptClient(); // 새로운 클라이언트의 소켓 디스크립터를 클라이언트 관리 벡터에 넣어준다.
        }
        for (pollIt = this->_allSockets.begin() + 1, clientIt = this->_clients.begin(); //"pollIt", "clientIt"를 사용하여 클라이언트 관리 벡터와 소켓들을 체크한다.
             clientIt != this->_clients.end();
             clientIt != this->_clients.end() ? ++clientIt : clientIt,
            pollIt != this->_allSockets.end() ? ++pollIt : pollIt)
        {
            if (pollIt->revents & (POLLERR | POLLHUP | POLLNVAL)) // 소켓에서 발생한 이벤트에서 에러가 생길때 처리하는 분기
            {
                disconnectClient(clientIt, pollIt); // 서버에서 해당 클라이언트의 연결을 끊어버린다.
                // 해당 클라이언트의 클라이언트벡터 위치, 소켓 벡터에서의 위치
                continue;
            }
            try
            {
                if (pollIt->revents & POLLIN) // 이벤트 수신 준비완료 상태일때 분기
                {
                    receiveMessage(clientIt->second); //"receiveMessage"메서드를 통해서 해당 메세지를 실행한다.
                }
                if (pollIt->revents & POLLOUT && !clientIt->second._messageOut.empty())
                {
                    sendMessage(clientIt->second); //"sendMessage"메서드를 통해서 해당 메세지를 클라이언트에게 송신한다.
                }
                if (this->_serverDown || this->_restart) // 서버가 꺼지거나 재시작했으면 "shutdown"으로 이동
                {
                    goto out;
                }
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
            if (clientIt->second._out) // 클라이언트가 연결해제 요청을 한 경우 "disconnectClient"메서드를 이용해서 해당 클라이언트와 서버의 연결을 끊는다.
            {
                disconnectClient(clientIt, pollIt);
            }
        }
    }
out:                       // 서버가 종료되는 상황일시 발동
    this->closeServer();   //"closeServer"메서드를 이용해서 서버를 닫아준다.
    if (this->_serverDown) //"_serverDown"변수가 참이면 함수를 종료시켜서 완전히 닫는다.
    {
        return;
    }
    this->createServerSocket(); // 다시 서버를 시작한다.
    this->_restart = false;     //"_restart" 변수를 false로 초기화
    goto activating;            // 다시 서버를 돌리는 루프문으로 돌아간다.
}

/* 서버를 종료시키는 메서드 */
void ft::Server::closeServer(void)
{
    clientIterator clientIt;                                                              // 서버를 종료하기전 현재 등록되어 있는 클라이언트에게 소켓 디스크립트를 닫고 메세지를 보내기 위한 반복자 선언
    for (clientIt = this->_clients.begin(); clientIt != this->_clients.end(); ++clientIt) // 모든 클라이언트에게 서버가 종료된다는 메세지를 보낸다
    {
        try
        {
            clientIt->second._messageOut = RPL_CLOSINGLINK(clientIt->second);
            sendMessage(clientIt->second);
        }
        catch (std::exception &e)
        {
        }
        close(clientIt->second._fd); // 순회문을 돌면서 각 클라이언트 소켓을 닫아버린다.
    }
    if (close(this->_allSockets.begin()->fd)) // 서버소켓을 닫는데 실패했을때 분기
    {
        ERRNO_EXCEPT;
    }
    this->_channels.clear();   // 등록된 모든 채널 정보들을 비워준다.
    this->_clients.clear();    // 등록된 모든 클라이언트 정보들을 비워준다.
    this->_allSockets.clear(); // 등록된 모든 소켓을 비워준다.
    std::cout << "======= FT_IRC server close =======" << std::endl;
    // int res = std::system("leaks ircserv");
    // if (res != 0)
    // {
    //     std::runtime_error("Leaks command not active");
    // }
}

/* 서버에 있는 서버 운영진에게 메세지를 보내는 메서드 */
void ft::Server::broadcastAddministratorClients(ft::Client &client)
{
    ft::Server::clientIterator clientIt; //"client"객체가 모여있는 컨테이너를 순회하기 위한 반복자를 선언
    for (clientIt = this->_clients.begin(); clientIt != this->_clients.end(); ++clientIt)
    {
        if (client._nickName != clientIt->second._nickName && clientIt->second._mode & MODE_WALLOPS) //"NOTICE"를 쓰는 클라이언트를 제외한 "운영자모드"를 가지고 있는 클라이언트일때 분기
        {
            clientIt->second._messageOut += NOTICE_MSG(client, clientIt->second._nickName, client._message._arguments.front()); // 해당 운영자들에게 메세지를 보내준다.
        }
    }
}

std::string ft::Server::getClientsCount(void)
{
    std::stringstream ss;
    ss << this->_clients.size();
    return ss.str();
}

std::string ft::Server::getOperCount(void)
{
    std::stringstream ss;
    int count = 0;
    for (ft::Server::clientIterator clientIt = this->_clients.begin(); clientIt != this->_clients.end(); clientIt++)
    {
        if (clientIt->second._mode & MODE_OPERATOR)
        {
            count++;
        }
    }
    ss << count;
    return ss.str();
}

std::string ft::Server::getChannelsCount(void)
{
    std::stringstream ss;
    ss << this->_channels.size();
    return ss.str();
}

/* Server 클래스의 소멸자*/
ft::Server::~Server(void)
{
    delete[] this->_buf;
    this->_buf = NULL;
}