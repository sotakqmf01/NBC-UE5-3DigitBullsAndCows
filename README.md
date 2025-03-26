# NBC-UE5-3DigitBullsAndCows
### 🎮 소개
랜덤한 3자리 숫자를 맞추는 숫자 야구 게임

![스크린샷 2025-03-26 173459](https://github.com/user-attachments/assets/50d6487d-84bf-4998-9297-762b9c9242b4)

[유튜브](https://youtu.be/giHDk64eYwE)

입력 창에 "/start"를 입력하면 게임 시작 (서버에서만 가능)

각 플레이어는 3회의 기회를 가지고, 각 기회는 30초의 제한 시간이 있음

플레이어가 "/"를 입력하고 숫자 3자리를 입력하면, (ex : /123) 
기회 1회를 소모하고, 판정 결과를 볼 수 있음
그리고 다음 플레이어에게 턴이 넘어감
("/"를 입력하지 않으면 일반 채팅)

판정 결과
Bulls : 같은 자리 + 같은 수 의 개수
Cows : 다른 자리 + 같은 수 의 개수
OUT : 잘못된 입력

### 📝 구현 과정
Blueprint Function Library 만들고, 숫자 생성, 정답 비교 같은 함수를 구현

리슨 서버를 기반으로 하고
PlayerController랑 GameMode 만들어서 RPC를 통해 둘 사이에 채팅 메시지를 주고 받을 수 있도록 구현
- 채팅 : UI 입력 -> PlayerController -> GameMode -> 모든 PlayerController -> UI 출력

GameMode에서 게임 진행을 위해 Blueprint Function Library에서 만든 함수들을 사용해서 숫자를 생성하고 
이를 채팅으로 예측하고 판정 결과를 알려주도록 하는 게임의 기본 로직을 구현

PlayerState를 만들어서 각 플레이어의 이름과 시도 횟수를 가지도록 하고, 예측할 때 마다 횟수를 차감하도록 구현
- 시도 횟수는 GameMode에서 플레이어의 입력을 판단하고 차감

턴과 제한 시간을 추가하기 위해 GameState를 만들어서 턴과 제한 시간을 관리
- 턴이 바뀔 때마다 모든 플레이어의 UI 갱신
- 제한 시간도 UI로 갱신
