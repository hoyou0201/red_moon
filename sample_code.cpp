#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>
#define ROW 10
#define COLUMN 17

using namespace std;

struct Rect {
    int r1, c1, r2, c2;
    int area;

    Rect(int _r1, int _c1, int _r2, int _c2)
        : r1(_r1), c1(_c1), r2(_r2), c2(_c2) {
        area = (r2 - r1 + 1) * (c2 - c1 + 1);  // 생성자에서 미리 계산
    }

    bool operator<(const Rect& other) const {
        if (area != other.area)
            return area < other.area;
        return tie(r1, c1, r2, c2) < tie(other.r1, other.c1, other.r2, other.c2);
    }
};



// 게임 상태를 관리하는 클래스
class Game
{
private:
    vector<vector<int>> board; // 게임 보드 (2차원 벡터)
    bool first;                // 선공 여부
    bool passed;               // 마지막 턴에 패스했는지 여부
    priority_queue<Rect> rectangles; // 유효한 사각형 목록

public:
    Game() {}

    Game(const vector<vector<int>> &board, bool first)
        : board(board), first(first), passed(false) {}


    int sumRectangle(Rect &rect)
    {
        int sum = 0;
        for (int r = rect.r1; r <= rect.r2; r++)
            for (int c = rect.c1; c <= rect.c2; c++)
                sum += board[r][c];
        return sum;
    }


    // 사각형 (r1, c1) ~ (r2, c2)이 유효한지 검사 (합이 10이고, 네 변을 모두 포함)
    bool isValid(Rect &rect)
    {
        int sums = 0;
        bool r1fit = false, c1fit = false, r2fit = false, c2fit = false;

        for (int r = rect.r1; r <= rect.r2; r++)
            for (int c = rect.c1; c <= rect.c2; c++)
                if (board[r][c] != 0)
                {
                    sums += board[r][c];
                    if (r == rect.r1)
                        r1fit = true;
                    if (r == rect.r2)
                        r2fit = true;
                    if (c == rect.c1)
                        c1fit = true;
                    if (c == rect.c2)
                        c2fit = true;
                }
        return (sums == 10) && r1fit && r2fit && c1fit && c2fit;
    }

    // ================================================================
    // ===================== [필수 구현] ===============================
    // 합이 10인 유효한 사각형을 찾아 {r1, c1, r2, c2} 벡터로 반환
    // 없으면 {-1, -1, -1, -1}을 반환하여 패스를 의미함
    // ================================================================
    vector<int> calculateMove(int myTime, int oppTime)
    {
        for (int r1 = 0; r1 < ROW; r1++)
            for (int c1 = 0; c1 < COLUMN; c1++)
            {
                for(int r2 = r1; r2 < ROW; r2++)
                {
                    for (int c2 = c1 + 1; c2 < COLUMN; c2++)
                    {
                        Rect rect = {r1, c1, r2, c2}; // 사각형 정의
                        // (r1, c1) ~ (r2, c2) 범위가 유효한지 검사
                        if (isValid(rect))
                        {
                            int s = sumRectangle(rect); // 합을 계산
                        }
                    }
                }
            }
        return {-1, -1, -1, -1}; // 유효한 사각형이 없으면 패스
    }






    // =================== [필수 구현 끝] =============================

    // 상대방의 수를 받아 보드에 반영
    void updateOpponentAction(const vector<int> &action, int time)
    {
        updateMove(action[0], action[1], action[2], action[3], false);
    }

    // 주어진 수를 보드에 반영 (칸을 0으로 지움)
    void updateMove(int r1, int c1, int r2, int c2, bool isMyMove)
    {
        if (r1 == -1 && c1 == -1 && r2 == -1 && c2 == -1)
        {
            passed = true;
            return;
        }
        for (int r = r1; r <= r2; r++)
            for (int c = c1; c <= c2; c++)
                board[r][c] = 0;
        passed = false;
    }
};

// 표준 입력을 통해 명령어를 처리하는 메인 함수
int main()
{
    Game game;
    bool first = false;

    while (true)
    {
        string line;
        getline(cin, line);

        istringstream iss(line);
        string command;
        if (!(iss >> command))
            continue;

        if (command == "READY")
        {
            // 선공 여부 확인
            string turn;
            iss >> turn;
            first = (turn == "FIRST");
            cout << "OK" << endl;
            continue;
        }

        if (command == "INIT")
        {
            // 보드 초기화
            vector<vector<int>> board;
            string row;
            while (iss >> row)
            {
                vector<int> boardRow;
                for (char c : row)
                {
                    boardRow.push_back(c - '0'); // 문자 → 숫자 변환
                }
                board.push_back(boardRow);
            }
            game = Game(board, first);
            continue;
        }

        if (command == "TIME")
        {
            // 내 차례: 수 계산 및 출력
            int myTime, oppTime;
            iss >> myTime >> oppTime;

            vector<int> ret = game.calculateMove(myTime, oppTime);
            game.updateMove(ret[0], ret[1], ret[2], ret[3], true);

            cout << ret[0] << " " << ret[1] << " " << ret[2] << " " << ret[3] << endl; // 내 행동 출력
            continue;
        }

        if (command == "OPP")
        {
            // 상대 행동 반영
            int r1, c1, r2, c2, time;
            iss >> r1 >> c1 >> r2 >> c2 >> time;
            game.updateOpponentAction({r1, c1, r2, c2}, time);
            continue;
        }

        if (command == "FINISH")
        {
            // 게임 종료
            break;
        }

        // 알 수 없는 명령 처리
        cerr << "Invalid command: " << command << endl;
        return 1;
    }

    return 0;
}