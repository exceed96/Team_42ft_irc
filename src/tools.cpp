#include "../Header/Header.hpp"

/* 문자열을 정수로 바꾸기 위한 함수 */
int ft::tools::ft_stoi(const std::string &str)
{
    int res;              // 반환값을 저장할 변수
    std::stringstream ss; // stringstream을 이용해서 "str"문자열 스트림 객체를 생성해서 문자열을 컨트롤 할 수 있게 변경
    ss << str;            // 문자열이 저장된 "ss"을 int형인 "res"로 넣어준다(정수로 변환)
    ss >> res;
    ss.str(std::string());                           //"ss"문자열 스트림을 비워준다.
    ss.clear();                                      //"ss"스트림의 상태를 초기화해준다.
    ss << res;                                       // 다시 정수를 문자열로 바꿔준다.
    std::string _str(&str[str[0] && str[0] == '+']); //"_str"문자열에 str의 맨 앞 문자가 '+'인 경우에만 저장한다.(음수인 포트가 들어올시 예외)
    if (ss.str().compare(_str))                      // 만약 변환된 문자열과 스트림에 저장한 문자열이 다르다면 -1을 반환하는 분기
    {
        res = -1;
    }
    return res; // 만약 스트림과 비교한 문자열이 같고 음수가 아닌 포트넘버라면 그 값을 반환해준다.
}

/* "poll"벡터에 있는 구조체들을 정렬하는 메서드 */
bool ft::tools::ft_pollfdCmp(const pollfd &pfd_a, const pollfd &pfd_b)
{
    return pfd_a.fd < pfd_b.fd;
}

/* 클라이언트에게 "NICK"명령어로 닉네임을 받았을 시 유효성을 검증하는 함수 */
bool ft::tools::ft_nickIsValid(const std::string &nickName)
{
    if (nickName.empty() || nickName.length() > 9) // 닉네임 인자가 비어 있거나 9글자보다 많을때 분기
    {
        return false;
    }
    if (!std::isalpha(nickName[0]) && !std::strchr(SPECIAL_CHARACTERS, nickName[0])) // 닉네임의 첫 문자가 알파벳이 아니고 특수문자가 아닐경우일때 분기
    {
        return false;
    }
    std::string::const_iterator stringIt = nickName.begin() + 1;                                                                      // 문자열의 반복자를 지정해서 체크한 맨 앞글자를 뺀 나머지 글자를 반복자로 순회한다.
    while (stringIt != nickName.end() && (std::isalnum(*stringIt) || std::strchr(SPECIAL_CHARACTERS, *stringIt) || *stringIt == '-')) // 알파벳이거나 숫자거나 지정한 특수문자거나 '-'일 경우에는 가능한 닉네임 문자로 허용한다.(이건 구현자 마다 맘대로 지정해도 가능)
    {
        ++stringIt;
    }
    if (stringIt != nickName.end()) // 반복문이 종료되었는데도 닉네임의 끝과 다르다면 올바른 문자가 있는 것이므로 "false"를 반환
    {
        return false;
    }
    return true; // 허용된 문자로 이루어진 닉네임일 경우 "true"를 반환
}

/* "USER"명령어로 유저의 모드를 받아오면 세팅하는 함수 */
unsigned char ft::tools::ft_modeSetting(const int &mode)
{
    unsigned char res = 0;
    if (mode & MODE_AWAY) // and연산자를 통해서 해당 비트연산자의 값이 0이 아닐경우 즉, 해당하는 모드일경우 OR연산자로 해당 결과값을 모드로 지정
    {
        res |= MODE_AWAY;
    }
    if (mode & MODE_WALLOPS)
    {
        res |= MODE_WALLOPS;
    }
    if (mode & MODE_INVISIBLE)
    {
        res |= MODE_INVISIBLE;
    }
    if (mode & MODE_RESTRICTED)
    {
        res |= MODE_RESTRICTED;
    }
    if (mode & MODE_OPERATOR)
    {
        res |= MODE_OPERATOR;
    }
    if (!res)
    {
        res |= MODE_WALLOPS;
    }
    return res;
}

/* 클라이언트로부터 받아온 메세지를 특정 구분자로 분할하여 파싱하는 메서드 */
std::vector<std::string> ft::tools::ft_splitMessage(const std::string &str, const char &separator)
{
    std::vector<std::string> res;                       // 파싱하여서 분할한 메세지를 반환할 변수
    std::string::const_iterator stringIt = str.begin(); //"message"변수에 접근할 반복자 선언
    std::string::const_iterator stringIt2;

    while (stringIt != str.end()) // "message"변수에 마지막에 도달할때까지 순회한다.
    {
        stringIt2 = std::find(stringIt, str.end(), separator); // 구분자를 통해서 문자열의 시작과 구분자를 만나기전, 즉 문자열이 끝나는 지점을 "it2"에 초기화한다.
        res.push_back(std::string(stringIt, stringIt2));       // 찾은 메세지를 "res"벡터에 넣어준다.
        stringIt = stringIt2;                                  // 시작 위치를 다시 구분자를 만나기 직전의 문자열 위치로 초기화 해준다(다음 문자열을 찾기 위해서)
        if (stringIt != str.end())                             //"message"의 끝에 도달하기 전까지 "it"반복자를 1칸씩 뒤로 밀어버린다.
        {
            stringIt++;
        }
    }
    return res; // 완성된 "res"를 반환한다.
}

/* 문자열을 소문자로 바꿔주는 함수 */
std::string ft::tools::ft_toLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

/* 채널 이름이 가능한지 불가능한지 판단하는 함수 */
bool ft::tools::ft_channelNameIsValid(const std::string &channelName)
{
    if (channelName.length() < 2 || channelName.length() > 31 || channelName[0] != '#') // 채널이름이 2글자 이상 30글자 이하가 아니거나 처음 문자가 '#'이 아닐경우 분기
    {
        return false;
    }
    std::string::const_iterator stringIt = channelName.begin() + 1;                                     //'#'다음의 문자를 반복자로 선언해서 초기화 해준다.
    while (stringIt != channelName.end() && !std::strchr(NULCRLFSPCL, *stringIt) && *stringIt != COLON) // 반복자가 "channelName"에 끝에 도달할때까지 순회한다. 널이나 Carriage return, 개행, 콜론일때를 만나면 순회문을 중지한다.
    {
        ++stringIt;
    }
    if (stringIt != channelName.end()) // 만약 제대로된 채널이름이라면 "channelName"의 마지막에 도달할텐데 그게 아니라면 허용하지 않은 문자가 온것이다.
    {
        return false; // 허용되지 않은 문자가 오면 "false"반환
    }
    return true; // 모든 문자가 허용된 문자들이라면 "true"반환
}

/* 소문자를 대문자로 바꿔주는 함수 */
std::string ft::tools::ft_toUpper(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}