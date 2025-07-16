#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdint>
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
    vector<vector<int>> sumboard; // 누적합 보드 (2차원 벡터)
    bool first;                // 선공 여부
    bool passed;               // 마지막 턴에 패스했는지 여부
    priority_queue<Rect> rectangles; // 유효한 사각형 목록
    uint32_t rowBits[10];   // 행 10개  → 각 17비트 정도
    uint16_t colBits[17];   // 열 17개  → 각 10비트
    void initBits() {
        uint32_t fullRow = (1u << COLUMN) - 1;  // 17비트: 0b111...1 (17개)
        uint16_t fullCol = (1u << ROW) - 1;  // 10비트: 0b111...1 (10개)

        for (int r = 0; r < ROW; ++r)
            rowBits[r] = fullRow;

        for (int c = 0; c < COLUMN; ++c)
            colBits[c] = fullCol;
    }

public:
    Game() {}

    Game(const vector<vector<int>> &board, bool first)
        : board(board), sumboard(vector<vector<int>>(ROW + 1, vector<int>(COLUMN + 1, 0))), first(first), passed(false) {
            initBits(); // 비트 배열 초기화
            updatesumboard(); // 누적합 보드 초기화
        }

    void updatesumboard()
    {
        int R = board.size();
        int C = board[0].size();

        for (int r = 0; r < R; ++r)
            for (int c = 0; c < C; ++c)
                sumboard[r + 1][c + 1] = board[r][c]
                                + sumboard[r][c + 1]
                                + sumboard[r + 1][c]
                                - sumboard[r][c];
    }



    int getSum(const Rect& r) {
        int r1 = r.r1, c1 = r.c1, r2 = r.r2, c2 = r.c2;
        return sumboard[r2+1][c2+1]
            - sumboard[r1][c2+1]
            - sumboard[r2+1][c1]
            + sumboard[r1][c1];
    }


    // 사각형 (r1, c1) ~ (r2, c2)이 유효한지 검사 (합이 10이고, 네 변을 모두 포함)
    bool isValid(Rect &rect)
    {
        // 위, 아래 중 하나라도 있으면 true
        uint32_t tt = (1u << (rect.c2 - rect.c1 + 1));
        tt = tt-1 << rect.c1;
        uint32_t rowMask = ((1u << (rect.c2 - rect.c1 + 1)) - 1) << rect.c1;
        if ( (rowBits[rect.r1] & rowMask) == 0 ) return false;  // 위에 하나도 없음
        if ( (rowBits[rect.r2] & rowMask) == 0 ) return false;  // 아래에 하나도 없음

        // 왼, 오른 중 하나라도 있으면 true
        uint16_t colMask = ((1u << (rect.r2 - rect.r1 + 1)) - 1) << rect.r1;
        if ( (colBits[rect.c1] & colMask) == 0 ) return false;  // 왼쪽에 없음
        if ( (colBits[rect.c2] & colMask) == 0 ) return false;  // 오른쪽에 없음

        //누적합으로 10인지 확인


        return true;  // 네 변 각각에 사과가 하나 이상 있음
    }

    // ================================================================
    // ===================== [필수 구현] ===============================
    // 합이 10인 유효한 사각형을 찾아 {r1, c1, r2, c2} 벡터로 반환
    // 없으면 {-1, -1, -1, -1}을 반환하여 패스를 의미함
    // ================================================================
    void calculateMove()
    {
        rectangles = priority_queue<Rect>(); // 우선순위 큐 초기화
        for (int r1 = 0; r1 < ROW; r1++)
        {
            for (int c1 = 0; c1 < COLUMN; c1++)
            {
                int tempc = COLUMN;
                for(int r2 = r1; r2 < ROW; r2++)
                {
                    Rect tall = {r1, c1, r2, c1}; // 세로로 긴 사각형
                    if (isValid(tall))
                    {
                        int s = getSum(tall); // 사각형의 합 계산
                        if(s == 10) // 합이 10인지 확인
                        {
                            rectangles.push(tall); // 유효한 사각형을 우선순위 큐에 추가
                        }
                        else if(s > 10) // 합이 10보다 크면
                        {
                            // 사과가 너무 많으므로 다음 행으로
                            break;
                        }
                    }
                    for (int c2 = c1+1; c2 < tempc; c2++)
                    {
                        Rect rect = {r1, c1, r2, c2}; // 사각형 정의
                        // (r1, c1) ~ (r2, c2) 범위가 유효한지 검사
                        if (isValid(rect))
                        {
                            int s = getSum(rect); // 사각형의 합 계산
                            if(s == 10) // 합이 10인지 확인
                            {
                                rectangles.push(rect); // 유효한 사각형을 우선순위 큐에 추가
                            }
                            else if(s > 10) // 합이 10보다 크면
                            {
                                // 사과가 너무 많으므로 다음 행으로
                                tempc = c2;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }


    vector<int> maxout(){
        calculateMove();
        if(rectangles.empty()) {
            return {-1, -1, -1, -1}; // 유효한 사각형이 없으면 패스
        }
        Rect r = rectangles.top();
        rectangles.pop();
        vector<int> ret = {r.r1, r.c1, r.r2, r.c2};
        return ret;
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
            for (int c = c1; c <= c2; c++){
                board[r][c] = 0;
                // 해당 비트를 0으로 꺼줌 (AND 연산)
                rowBits[r] &= ~(1u << c);
                colBits[c] &= ~(1u << r);
            }
        passed = false;
        updatesumboard(); // 누적합 보드 업데이트
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
            vector<vector<int>> sumboard(ROW + 1, vector<int>(COLUMN + 1, 0));  // 1-based
            string row;
            for (int r = 0; r < ROW; ++r) {
                iss >> row;

                vector<int> boardRow;

                for (int c = 0; c < COLUMN; ++c) {
                    int val = row[c] - '0';
                    boardRow.push_back(val);
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

            vector<int> ret = game.maxout();
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