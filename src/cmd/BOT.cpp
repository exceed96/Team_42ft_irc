#include "../../Header/Header.hpp"

const std::string client_id = "u-s4t2ud-8b32aa536b5480a3573a4945b9404b50ad4f4b277ce414f70aa13ff2dd6ab1f7";
const std::string client_secret = "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";
const std::string redirect_uri = "http://localhost:3000/";

static std::string access42Api()
{
    std::stringstream ss;
    ss << "curl -s -X POST \"https://api.intra.42.fr/oauth/token\" ";   //"curl"명령어 X옵션을 줘서 post요청을 보낸다.
    ss << "-d \"grant_type=client_credentials&client_id=" << client_id; //-d옵션으로 윗줄과 이어지게 하고 code 값이 없으므로 grant_type을 client_credential로 한다.
    ss << "&client_secret=" << client_secret << "&redirect_uri=" << redirect_uri;
    ss << "\"";

    FILE *pipe = popen(ss.str().c_str(), "r"); // popen함수를 이용해서 파이프를 생성하여 ss객체에 있는 명령어를 실행한다.
    if (!pipe)                                 // 파이프 생성에 실패 했을 시 에러 메세지 출력
    {
        throw std::runtime_error("Failed to open pipe");
    }

    std::string response; // 반환값을 저장할 문자열 변수
    char buffer[128];     // 버퍼로 활용할 변수

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) //"fgets"함수를 이용해서 curl로 보낸 명령의 반환값을 읽어온다. 즉 buffer에 저장된 값을 response로 옮겨준다.
    {
        response += buffer;
    }

    int status = pclose(pipe); // 파이프를 닫고 그 결과를 status변수에 저장
    if (status == -1)          // 파이프 닫기에 실패했으면 에러 메세지 출력
    {
        throw std::runtime_error("Failed to close pipe");
    }

    return response; // response를 반환한다.
}

/* 42API에 접근하여 액세스 토큰을 얻어서 환경변수에 저장하는 함수 */
static void getAccessToken()
{
    try
    {
        std::string response = access42Api();                                     //"access42Api"함수를 이용해서 토큰을 받아와 "response"변수에 저장한다.
        size_t start = response.find("\"access_token\":\"");                      //"find"함수를 통해서 "access_token"부분을 시작점을 추출한다.
        size_t end = response.find("\"", start + 16);                             //"find"함수를 통해서 "access_token"값의 끝 부분을 추출한다.
        std::string access_token = response.substr(start + 16, end - start - 16); //"substr"메서드를 이용해서 엑세스 토큰 값을 추출한다.

        setenv("api42token", access_token.c_str(), 1); // "setenv"함수를 이용해서 해당 토큰을 환경변수에 지정한다.
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

/* api요청을 통해서 데이터를 받아오는 함수 */
static std::string receiveApiData(const std::string &requestUrl)
{
    getAccessToken();                                 //"getAccessToken"함수를 이용해서 액세스 토큰을 받아온다.
    const char *apiToken = std::getenv("api42token"); // 환경변수에 저장된 "42api" 토큰을 "charToken"에 저장한다.
    if (apiToken == NULL)                             // 만약 해당하는 환경변수가 없을 때 분기
    {
        throw std::runtime_error("42Api token environment not exist");
    }
    std::string request = "rm -f .log && touch .log && curl -sH \"Authorization: Bearer " + //"기존에" host를 저장했더 log파일을 삭제 한 후 다시 만들어서 "curl" 명령어를 통해서 H옵션으로 헤더에 토큰을 지정하낟.
                          std::string(apiToken) + "\" " + requestUrl + " > .log";
    int status = std::system(request.c_str()); // "curl"명령어를 실행하고 요청의 결과를 "status"변수에 저장한다.
    std::stringstream ss;                      // 스트림을 사용하기 위한 "ss" 스트림 변수 선언
    ss << std::ifstream(".log").rdbuf();       // 이전에 생성된 ".log"파일을 read하고 "ss"스트림에 저장한다. "ss"에는 api요청의 결과값이 들어가 있게 된다.
    int res = std::system("rm -f .log");       // "ss"스트림에 데이터를 옮겨 놓았으면 .log파일을 삭제한다.
    if (res != 0)
    {
        throw std::runtime_error("Rm command not active");
    }
    if (status)
    {
        throw std::runtime_error("Curl command not active or Receive Data Api Error");
    }
    return ss.str(); // api요청으로 받아온 JSON데이터를 반환해준다.
}

/* "BOT" 명령어 실행 메서드 */
void ft::Command::BOT(ft::Client &client)
{
    if (client._message._arguments.size() < 2 || client._message._arguments.front().empty()) // "BOT"명령어와 함께 들어온 인자가 2개 미만이거나 인자가 비어있을 때 분기
    {
        client._messageOut += ERR_NEEDMOREPARAMS(client);
        return;
    }
    const std::string &nickName = client._message._arguments[1];    // 인자의 1번째를 "nickName"변수에 초기화한다.
    std::string replyMessage;                                       //"BOT"명령어에 대한 처리 결과를 저장하는 변수
    std::string saveApiData;                                        // API를통해 얻은 데이터를 저장하는 변수
    std::string requestUrl = "\"https://api.intra.42.fr/v2/users/"; // api요청을 보내기 위한 url을 저장한 변수
    try
    {
        if (!client._message._arguments.front().compare("location")) // 인자벡터의 0번째 원소가 "location"일때 분기(클러스터에 어느 자리에 있는지 알려주는 명령어)
        {
            requestUrl += client._message._arguments[1] + "/locations?filter\\[end\\]=false\" | awk -F\'\"location\":\' \'{print $2}\' " //"https://api.intra.42.fr/v2/users/sangyeki/locations?filter[end]=false\" 와 같이 api요청을 보내고 "awk"명령어를 통해서 받아온 결과값을 추출하고 "location"부분을 추출한다.
                                                          "| awk -F\'\"\' \'{print $2}\'";
            saveApiData = receiveApiData(requestUrl);
            replyMessage = "< " + nickName + " > Client is " + (saveApiData.size() == 1 ? "unavailable" : ("at => " + saveApiData));
        }
        else if (!client._message._arguments.front().compare("coalition")) // 해당 클라이언트가 속한 코알리숑이 어딘지를 알려주는 명령어
        {
            requestUrl += client._message._arguments[1] + "/coalitions\" | awk -F\'\"name\":\' \'{print $2}\' "
                                                          "| awk -F\'\"\' \'{print $2}\'";
            saveApiData = receiveApiData(requestUrl);
            replyMessage = "< " + nickName + " > Client is " + (saveApiData.size() == 1 ? "doesn't belongs to the coalition" : "belongs to the => " + saveApiData);
        }
        else //"BOT"명령어에 정의되지 않은 커맨드를 칠 경우 에러 출력
        {
            client._messageOut += ERR_UNKNOWNCOMMAND(client);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        replyMessage = "Error\r\n";
    }
    client._messageOut += RPL_BOT(client, replyMessage);
}
