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

int getSum(const Rect& r) {
    int r1 = r.r1, c1 = r.c1, r2 = r.r2, c2 = r.c2;
    return sumboard[r2+1][c2+1]
        - sumboard[r1][c2+1]
        - sumboard[r2+1][c1]
        + sumboard[r1][c1];
}

bool isValid(Rect &rect)
{
    // 위, 아래 중 하나라도 있으면 true
    uint32_t rowMask = ((1u << (rect.c2 - rect.c1 + 1)) - 1) << rect.c1;
    if ( (rowBits[rect.r1] & rowMask) == 0 ) return false;  // 위에 하나도 없음
    if ( (rowBits[rect.r2] & rowMask) == 0 ) return false;  // 아래에 하나도 없음

    // 왼, 오른 중 하나라도 있으면 true
    uint16_t colMask = ((1u << (rect.r2 - rect.r1 + 1)) - 1) << rect.r1;
    if ( (colBits[rect.c1] & colMask) == 0 ) return false;  // 왼쪽에 없음
    if ( (colBits[rect.c2] & colMask) == 0 ) return false;  // 오른쪽에 없음

    return true;  // 네 변 각각에 사과가 하나 이상 있음
}

void calculateMove()
{
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

int main(){
    string row;
    
    sumboard = vector<vector<int>>(ROW + 1, vector<int>(COLUMN + 1, 0));  // 1-based

    for (int r = 0; r < ROW; ++r) {
        string row;
        cin >> row;

        vector<int> boardRow;

        for (int c = 0; c < COLUMN; ++c) {
            int val = row[c] - '0';
            boardRow.push_back(val);

            // 1-based 누적합 계산 (r+1, c+1 위치에 저장)
            sumboard[r + 1][c + 1] = val
                                + sumboard[r][c + 1]
                                + sumboard[r + 1][c]
                                - sumboard[r][c];
        }

        board.push_back(boardRow);
    }
    initBits();
    calculateMove();
    while(!rectangles.empty()) {
        Rect rect = rectangles.top();
        rectangles.pop();
        cout << "Rect: (" << rect.r1 << ", " << rect.c1 << ") to (" 
             << rect.r2 << ", " << rect.c2 << "), Area: " << rect.area << endl;
    }

}