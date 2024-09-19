#include "../../Header/Header.hpp"

/* "USER" 명령어 실행 메서드 */
void ft::Command::USER(ft::Client &client)
{
    if (client._message._arguments.size() != 4) //"USER"명령어와 함께온 인자가 4개보다 작다면
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client); // 에러메세지 클라이언트에게 출력
    }
    else if (client._realName.compare("*")) // 이미 "USER"명령어를 통해서 닉네임이 등록되어 있는 상태일때 분기
    {
        client._messageOut += ERR_ALREADYREGISTRED(client);
    }
    else //"USER"명령어로 유저 등록에 성공한 경우일때 분기
    {
        client._sign = time(NULL);
        client._idle = client._sign;
        client._userName = client._message._arguments[0];
        client._realName = client._message._arguments[3];
        std::stringstream ss;
        int i;                               // 숫자로 바꿔서 모드를 적용하기 위한 정수 변환
        ss << client._message._arguments[1]; // 유저의 모드를 지정한 1번쨰 인자를 "ss"스트림에 넣어준다.
        ss >> i;
        client._mode = ft::tools::ft_modeSetting(i); // 모드 세팅하는 "ft_modeSetting"함수에서 해당 클라이언트가 입력한 모드로 세팅한다.
        client._userFlag = true;
        std::cout << "User '" << client._nickName << "' - '" << client._hostAddr << std::endl;
        return;
    }
    if (!client._realName.compare("*")) // 만약 USER이름을 지정하지 않은 경우일때 분기
    {
        client._out = true; // 서버와의 접속을 강제로 끊어버린다.
    }
}